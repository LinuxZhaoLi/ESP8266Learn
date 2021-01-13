#include "user_config.h"		// �û�����
#include "ets_sys.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "osapi.h" 	// ϵͳ����
#include "user_interface.h"  // ϵͳ����
#include "driver/uart.h"  // ����
#include "ets_sys.h"			// �ص�����
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

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
	u8 F_LED = 0;  // LED״̬��־λ
	u8 F_LEDO = 0;
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);
	os_printf("\r\n---------------------------------\r\n");  // ϵͳ����

	os_printf("SDK version:%s\n",system_get_sdk_version());		// ���ڴ�ӡSDK�汾
	// �ܽŹ���ѡ��(ע�⣺����1��PIN_NAME���ܽ���������2��FUNC���ܽŹ���)
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��
//	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);			// IO4 = 1(�ߵ�ƽ)
//	while(1)
//		{
//			system_soft_wdt_feed();					// ι������ֹ��λ
//
//			F_LED = !F_LED;							// LED״ֵ̬��ת
//
//			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);	// ����LED(IO4)���ֵ
//
//			delay_ms(500);							// ��ʱ500Ms
//		}

	// ��ʼ��LED(ע�⡾PIN_NAME������FUNC������gpio_no����Ҫ����)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4��ΪIO��  D2  led
		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1

		// ��ʼ������(BOOT == GPIO0)
		//----------------------------------------------------------------------------------
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0��ΪIO��  boot ��
		GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0ʧ�����(Ĭ��)
		PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʧ���ڲ�����(Ĭ��)
	//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʹ���ڲ�����


		while(1)
		{
			system_soft_wdt_feed();// ι������ֹ��λ

			if( GPIO_INPUT_GET(GPIO_ID_PIN(0)) == 0 )
			{// ��ȡGPIO_0��ƽ
				os_printf("\r\n-------- LED��----------------\r\n");  // ϵͳ����
				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);				// LED��
			}else
			{
				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);				// LED��

				os_printf("\r\n-------- LED��----------------\r\n");  // ϵͳ����

			}

		}
}

