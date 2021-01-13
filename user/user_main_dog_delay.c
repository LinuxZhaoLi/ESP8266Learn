#include "user_config.h"		// �û�����
#include "ets_sys.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "osapi.h" 	// ϵͳ����
#include "user_interface.h"  // ϵͳ����
#include "osapi.h"
#include "driver/uart.h"  // ����


uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
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
            rf_cal_sec = 512 - 5;
            break;
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
            rf_cal_sec = 1024 - 5;
            break;
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



void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
	{ os_delay_us(1000);}
}
void ICACHE_FLASH_ATTR		// ��ʼ������
user_init(void)
{
	uart_init(115200,115200);	//��ʼ��������
	os_delay_us(10000);
	os_printf("\r\n---------------------------------\r\n");  // ϵͳ����

	os_printf("SDK version:%s\n",system_get_sdk_version());		// ���ڴ�ӡSDK�汾

	uart0_sendStr("\r\nHello World\r\n");
	uart0_sendStr("\r\nHello World\r\n");


	os_printf("\r\n---------------------------------\r\n");

	while(1)
	{
		system_soft_wdt_feed();
		os_printf("\r\n Hello\r\n");
		delay_ms(1000);

	}
}

