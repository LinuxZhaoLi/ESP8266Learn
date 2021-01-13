#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ�á���"espconn.h"��ͷ#include"ip_addr.h"��#include"ip_addr.h"����"espconn.h"֮ǰ
#include "espconn.h"			// TCP/UDP�ӿ�
#include "ets_sys.h"			// �ص�����
#include "mem.h"				// �ڴ�����Ⱥ���
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

#define		ProjectName			"AP_UDP_Server"		// �������궨��

#define		ESP8266_AP_SSID		"8266"		// ������WIFI��
#define		ESP8266_AP_PASS		"82668266"			// ������WIFI����

#define		LED_ON				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0)		// LED��
#define		LED_OFF				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1)		// LED��

os_timer_t OS_Timer_1;		// �����ʱ��

void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

void ICACHE_FLASH_ATTR ESP8266_AP_Init_JX()
{
	struct softap_config AP_Config;				// AP�����ṹ��
	wifi_set_opmode(0x02);						// ����ΪAPģʽ�������浽Flash
	// �ṹ�帳ֵ(ע�⣺�����񼯱�ʶ��/���롿����Ϊ�ַ�����ʽ)
	os_memset(&AP_Config, 0, sizeof(struct softap_config));	// AP�����ṹ�� = 0
	os_strcpy(AP_Config.ssid,ESP8266_AP_SSID);		// ����SSID(���ַ������Ƶ�ssid����)
	os_strcpy(AP_Config.password,ESP8266_AP_PASS);	// ��������(���ַ������Ƶ�password����)
	AP_Config.ssid_len=os_strlen(ESP8266_AP_SSID);	// ����ssid����(��SSID�ĳ���һ��)
	AP_Config.channel=1;                      		// ͨ����1��13
	AP_Config.authmode=AUTH_WPA2_PSK;           	// ���ü���ģʽ
	AP_Config.ssid_hidden=0;                  		// ������SSID
	AP_Config.max_connection=4;               		// ���������
	AP_Config.beacon_interval=100;            		// �ű���ʱ��100��60000 ms

	wifi_softap_set_config(&AP_Config);				// ����soft-AP�������浽Flash
}


// �ɹ������������ݵĻص�����
void ICACHE_FLASH_ATTR ESP8266_WIFI_Send_Cb_JX(void *arg)
{
	os_printf("\nESP8266_WIFI_Send_OK\n");
}
// �ɹ������������ݵĻص�����������1�����紫��ṹ��espconnָ�롢����2�����紫������ָ�롢����3�����ݳ��ȡ�
void ICACHE_FLASH_ATTR ESP8266_WIFI_Recv_Cb_JX(void * arg, char * pdata, unsigned short len)
{
	struct espconn * T_arg = arg;		// �����������ӽṹ��ָ��

	remot_info * P_port_info = NULL;	// Զ��������Ϣ�ṹ��ָ��

	// ������������LED����/��
	if(pdata[0] == 'k' || pdata[0] == 'K')	LED_ON;			// ����ĸΪ'k'/'K'������
	else if(pdata[0] == 'g' || pdata[0] == 'G')	LED_OFF;	// ����ĸΪ'g'/'G'������
	os_printf("\nESP8266_Receive_Data = %s\n",pdata);		// ���ڴ�ӡ���յ�����

	// ��ȡԶ����Ϣ��UDPͨ���������ӵģ���Զ��������Ӧʱ���ȡ�Է���IP/�˿���Ϣ��
	if(espconn_get_connection_info(T_arg, &P_port_info, 0)==ESPCONN_OK)	// ��ȡԶ����Ϣ
	{
		T_arg->proto.udp->remote_port  = P_port_info->remote_port;		// ��ȡ�Է��˿ں�
		T_arg->proto.udp->remote_ip[0] = P_port_info->remote_ip[0];		// ��ȡ�Է�IP��ַ
		T_arg->proto.udp->remote_ip[1] = P_port_info->remote_ip[1];
		T_arg->proto.udp->remote_ip[2] = P_port_info->remote_ip[2];
		T_arg->proto.udp->remote_ip[3] = P_port_info->remote_ip[3];
		//os_memcpy(T_arg->proto.udp->remote_ip,P_port_info->remote_ip,4);	// �ڴ濽��
	}
	os_printf("REMOTE_PORT = %d\n",T_arg->proto.udp->remote_port);

	os_printf("REMOTE_IP = %d.%d.%d.%d\n",T_arg->proto.udp->remote_ip[0],T_arg->proto.udp->remote_ip[1],T_arg->proto.udp->remote_ip[2],T_arg->proto.udp->remote_ip[3]);

	OLED_ShowIP(24,6,T_arg->proto.udp->remote_ip);	// ��ʾԶ������IP��ַ

	espconn_send(T_arg,"ESP8266_WIFI_Recv_OK",os_strlen("ESP8266_WIFI_Recv_OK"));	// ��Է�����Ӧ��
}


// �٣�����espconn�ͽṹ��(�������ӽṹ��)
struct espconn ST_NetCon;	// ע�����붨��Ϊȫ�ֱ������ں˽���ʹ�ô˱���(�ڴ�)

//esp_udp ST_UDP;	// UDPͨ�Žṹ��

// ��ʼ����������(UDPͨ��)
void ICACHE_FLASH_ATTR ESP8266_NetCon_Init_JX()
{
	// �ڣ��ṹ�帳ֵ
	ST_NetCon.type = ESPCONN_UDP;		// ͨ��Э�飺UDP

	// ST_NetCon.proto.udpֻ��һ��ָ�룬����������esp_udp�ͽṹ�����
	ST_NetCon.proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));	// �����ڴ�
	// �˴���������Ŀ��IP/�˿�(ESP8266��ΪServer������ҪԤ��֪��Client��IP/�˿�)
	ST_NetCon.proto.udp->local_port  = 8266 ;		// ���ñ��ض˿�

	// �ۣ�ע��/����ص�����
	espconn_regist_sentcb(&ST_NetCon,ESP8266_WIFI_Send_Cb_JX);	// ע���������ݷ��ͳɹ��Ļص�����
	espconn_regist_recvcb(&ST_NetCon,ESP8266_WIFI_Recv_Cb_JX);	// ע���������ݽ��ճɹ��Ļص�����
	// �ܣ�����UDP��ʼ��API
	espconn_create(&ST_NetCon);	// ��ʼ��UDPͨ��
}


// ��ʱ�Ļص�����(��ѯESP8266��IP��ַ������ʼ����������)
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	struct ip_info ST_ESP8266_IP;	// IP��Ϣ�ṹ��

	u8  ESP8266_IP[4];		// ���ʮ������ʽ����IP


	wifi_get_ip_info(SOFTAP_IF,&ST_ESP8266_IP);	// ��ѯAPģʽ��ESP8266��IP��ַ

	if(ST_ESP8266_IP.ip.addr != 0)				// ESP8266�ɹ���ȡ��IP��ַ
	{
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// ���ʮ����IP�ĵ�һ���� <==> addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// ���ʮ����IP�ĵڶ����� <==> addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// ���ʮ����IP�ĵ������� <==> addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// ���ʮ����IP�ĵ��ĸ��� <==> addr�߰�λ

		// ��ӡ����ʾESP8266��IP��ַ
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		OLED_ShowIP(24,2,ESP8266_IP);	// OLED��ʾESP8266��IP��ַ
		os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
		ESP8266_NetCon_Init_JX();		// ��ʼ����������(UDPͨ��)
	}
}

void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{

	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}

void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());

	// OLED��ʾ��ʼ��
	OLED_Init();							// OLED��ʼ��
	OLED_ShowString(0,0,"ESP8266 = AP");	// ESP8266ģʽ
	OLED_ShowString(0,2,"IP:");				// ESP8266_IP��ַ
	OLED_ShowString(0,4,"Remote  = STA");	// Զ������ģʽ
	OLED_ShowString(0,6,"IP:");				// Զ������IP��ַ
	LED_Init_JX();		// LED��ʼ��
	ESP8266_AP_Init_JX();			// ��ʼ��ESP8266_APģʽ
	OS_Timer_1_Init_JX(1000,1);		// 1�������ʱ
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
