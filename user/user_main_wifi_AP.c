// ͷ�ļ�����
#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ets_sys.h"			// �ص�����
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

// �궨��
#define		ProjectName			"AP_Mode"			// �������궨��

#define		ESP8266_AP_SSID		"8266"		// ������WIFI��
#define		ESP8266_AP_PASS		"82668266"			// ������WIFI����

os_timer_t OS_Timer_1;		// �����ʱ��
// ������ʱ����
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
// ��ʼ��ESP8266_APģʽ
void ICACHE_FLASH_ATTR ESP8266_AP_Init_JX()
{
	struct softap_config AP_Config;				// AP�����ṹ��
	os_printf("\r\n===============AP_config====================\r\n");

	wifi_set_opmode(0x02);						// ����ΪAPģʽ�������浽Flash

	// �ṹ�帳ֵ(ע�⣺�����񼯱�ʶ��/���롿����Ϊ�ַ�����ʽ)
	//--------------------------------------------------------------------------------------
	os_printf("\r\n===============memset  0==================\r\n");

	os_memset(&AP_Config, 0, sizeof(struct softap_config));	// AP�����ṹ�� = 0

	os_strcpy(AP_Config.ssid,ESP8266_AP_SSID);		// ����SSID(���ַ������Ƶ�ssid����)
	os_strcpy(AP_Config.password,ESP8266_AP_PASS);	// ��������(���ַ������Ƶ�password����)
	AP_Config.ssid_len=os_strlen(ESP8266_AP_SSID);	// ����ssid����(��SSID�ĳ���һ��)
	AP_Config.channel=1;                      		// ͨ����1��13
	AP_Config.authmode=AUTH_WPA2_PSK;           	// ���ü���ģʽ
	AP_Config.ssid_hidden=0;                  		// ������SSID
	AP_Config.max_connection=4;               		// ���������
	AP_Config.beacon_interval=100;            		// �ű���ʱ��100��60000 ms
	os_printf("\r\n============wifi_softap-set_config===============\r\n");

	wifi_softap_set_config(&AP_Config);				// ����soft-AP�������浽Flash
}
// ��ʱ�Ļص�����
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{

	os_printf("\r\n==========time call function =============\r\n");

	struct ip_info ST_ESP8266_IP;	// IP��Ϣ�ṹ��

	u8  ESP8266_IP[4];		// ���ʮ������ʽ����IP

	os_printf("\nESP8266_name = %s\n",ESP8266_AP_SSID);
	os_printf("\nESP8266_pass = %s\n",ESP8266_AP_PASS);
	// ��ѯ����ӡESP8266�Ĺ���ģʽ
	switch(wifi_get_opmode())	// �������ģʽ
	{
		case 0x01:	os_printf("\nESP8266_Mode = Station\n");		break;
		case 0x02:	os_printf("\nESP8266_Mode = SoftAP\n");			break;
		case 0x03:	os_printf("\nESP8266_Mode = Station+SoftAP\n");	break;
	}


	// ��ȡESP8266_APģʽ�µ�IP��ַ
	//��APģʽ�£��������DHCP(Ĭ��)������δ����IP��ز�����ESP8266��IP��ַ=192.168.4.1��
	wifi_get_ip_info(SOFTAP_IF,&ST_ESP8266_IP);	// ����2��IP��Ϣ�ṹ��ָ��

	// ESP8266_AP_IP.ip.addr==32λ������IP��ַ������ת��Ϊ���ʮ���Ƶ���ʽ
	ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// ���ʮ����IP�ĵ�һ���� <==> addr�Ͱ�λ
	ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// ���ʮ����IP�ĵڶ����� <==> addr�εͰ�λ
	ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// ���ʮ����IP�ĵ������� <==> addr�θ߰�λ
	ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// ���ʮ����IP�ĵ��ĸ��� <==> addr�߰�λ

	// ��ӡESP8266��IP��ַ

	os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
	OLED_ShowIP(24,2,ESP8266_IP);			// ��ʾESP8266��IP��ַ


	// ��ѯ����ӡ�����WIFI���豸����
	os_printf("Number of devices connected to this WIFI = %d\n",wifi_softap_get_station_num());
}

// �����ʱ����ʼ��(ms����)
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	os_printf("\r\n===========close Timer=============\r\n");

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��

	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}

// user_init��entry of user application, init user function here
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// OLED��ʼ��
	os_printf("\r\n===============  OLED_ INIT====================\r\n");
	OLED_Init();							// |
	OLED_ShowString(0,0,"ESP8266 = AP");	// |
	OLED_ShowString(0,2,"IP:");				// |
	//������������������������������������������
	os_printf("\r\n===============ESP8266_AP_Init_JX====================\r\n");


	ESP8266_AP_Init_JX();			// ����ESP8266_APģʽ��ز���

    OS_Timer_1_Init_JX(1000,1);		// 1�������ʱ


//	while(1) system_soft_wdt_feed();	// ��ѭ����������

    os_printf("\r\n-------------------- user_init OVER --------------------\r\n");
}

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR user_rf_pre_init(void){}
