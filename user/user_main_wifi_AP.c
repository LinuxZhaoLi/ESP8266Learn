// 头文件引用
#include "user_config.h"		// 用户配置
#include "driver/uart.h"  		// 串口
#include "driver/oled.h"  		// OLED
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "ets_sys.h"			// 回调函数
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX、软件定时器
#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro

// 宏定义
#define		ProjectName			"AP_Mode"			// 工程名宏定义

#define		ESP8266_AP_SSID		"8266"		// 创建的WIFI名
#define		ESP8266_AP_PASS		"82668266"			// 创建的WIFI密码

os_timer_t OS_Timer_1;		// 软件定时器
// 毫秒延时函数
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}
// 初始化ESP8266_AP模式
void ICACHE_FLASH_ATTR ESP8266_AP_Init_JX()
{
	struct softap_config AP_Config;				// AP参数结构体
	os_printf("\r\n===============AP_config====================\r\n");

	wifi_set_opmode(0x02);						// 设置为AP模式，并保存到Flash

	// 结构体赋值(注意：【服务集标识符/密码】须设为字符串形式)
	//--------------------------------------------------------------------------------------
	os_printf("\r\n===============memset  0==================\r\n");

	os_memset(&AP_Config, 0, sizeof(struct softap_config));	// AP参数结构体 = 0

	os_strcpy(AP_Config.ssid,ESP8266_AP_SSID);		// 设置SSID(将字符串复制到ssid数组)
	os_strcpy(AP_Config.password,ESP8266_AP_PASS);	// 设置密码(将字符串复制到password数组)
	AP_Config.ssid_len=os_strlen(ESP8266_AP_SSID);	// 设置ssid长度(和SSID的长度一致)
	AP_Config.channel=1;                      		// 通道号1～13
	AP_Config.authmode=AUTH_WPA2_PSK;           	// 设置加密模式
	AP_Config.ssid_hidden=0;                  		// 不隐藏SSID
	AP_Config.max_connection=4;               		// 最大连接数
	AP_Config.beacon_interval=100;            		// 信标间隔时槽100～60000 ms
	os_printf("\r\n============wifi_softap-set_config===============\r\n");

	wifi_softap_set_config(&AP_Config);				// 设置soft-AP，并保存到Flash
}
// 定时的回调函数
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{

	os_printf("\r\n==========time call function =============\r\n");

	struct ip_info ST_ESP8266_IP;	// IP信息结构体

	u8  ESP8266_IP[4];		// 点分十进制形式保存IP

	os_printf("\nESP8266_name = %s\n",ESP8266_AP_SSID);
	os_printf("\nESP8266_pass = %s\n",ESP8266_AP_PASS);
	// 查询并打印ESP8266的工作模式
	switch(wifi_get_opmode())	// 输出工作模式
	{
		case 0x01:	os_printf("\nESP8266_Mode = Station\n");		break;
		case 0x02:	os_printf("\nESP8266_Mode = SoftAP\n");			break;
		case 0x03:	os_printf("\nESP8266_Mode = Station+SoftAP\n");	break;
	}


	// 获取ESP8266_AP模式下的IP地址
	//【AP模式下，如果开启DHCP(默认)，并且未设置IP相关参数，ESP8266的IP地址=192.168.4.1】
	wifi_get_ip_info(SOFTAP_IF,&ST_ESP8266_IP);	// 参数2：IP信息结构体指针

	// ESP8266_AP_IP.ip.addr==32位二进制IP地址，将它转换为点分十进制的形式
	ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;			// 点分十进制IP的第一个数 <==> addr低八位
	ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;		// 点分十进制IP的第二个数 <==> addr次低八位
	ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;		// 点分十进制IP的第三个数 <==> addr次高八位
	ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;		// 点分十进制IP的第四个数 <==> addr高八位

	// 打印ESP8266的IP地址

	os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
	OLED_ShowIP(24,2,ESP8266_IP);			// 显示ESP8266的IP地址


	// 查询并打印接入此WIFI的设备数量
	os_printf("Number of devices connected to this WIFI = %d\n",wifi_softap_get_station_num());
}

// 软件定时器初始化(ms毫秒)
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	os_printf("\r\n===========close Timer=============\r\n");

	os_timer_disarm(&OS_Timer_1);	// 关闭定时器

	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// 设置定时器
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // 使能定时器
}

// user_init：entry of user application, init user function here
void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// OLED初始化
	os_printf("\r\n===============  OLED_ INIT====================\r\n");
	OLED_Init();							// |
	OLED_ShowString(0,0,"ESP8266 = AP");	// |
	OLED_ShowString(0,2,"IP:");				// |
	//－－－－－－－－－－－－－－－－－－－－－
	os_printf("\r\n===============ESP8266_AP_Init_JX====================\r\n");


	ESP8266_AP_Init_JX();			// 设置ESP8266_AP模式相关参数

    OS_Timer_1_Init_JX(1000,1);		// 1秒软件定时


//	while(1) system_soft_wdt_feed();	// 死循环，测试用

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
