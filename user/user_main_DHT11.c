#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "driver/oled.h"  		// OLED
#include "driver/dht11.h"		// DHT11ͷ�ļ�
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ets_sys.h"			// �ص�����
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

#define		ProjectName			"DHT11"			// �������궨��

u8 F_LED = 0;				// LED״̬��־λ

os_timer_t OS_Timer_1;		// �����ʱ��


void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);  // ��ʱ1��
}

void ICACHE_FLASH_ATTR OS_Timer_1_test(void)
{
	os_printf("=��OS_Timer_1_cb\n");
}

// ��ʱ���ص�����
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)
{
	os_printf("=��OS_Timer_1_cb\n");
	u32 res = DHT11_Read_Data_Complete();
	if(res == 0 || res == 1)		// ��ȡDHT11��ʪ��ֵ
	{
		// �¶ȳ���30�棬LED��
		//----------------------------------------------------
		if(DHT11_Data_Array[5]==1 && DHT11_Data_Array[2]>=30)
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);		// LED��
		else
			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);		// LED��


		// ���������ʪ��
		if(DHT11_Data_Array[5] == 1)			// �¶� >= 0��
		{
			os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
			os_printf("\r\n�¶� == %d.%d ��\r\n", DHT11_Data_Array[2],DHT11_Data_Array[3]);
		}
		else // if(DHT11_Data_Array[5] == 0)	// �¶� < 0��
		{
			os_printf("\r\nʪ�� == %d.%d %RH\r\n",DHT11_Data_Array[0],DHT11_Data_Array[1]);
			os_printf("\r\n�¶� == -%d.%d ��\r\n",DHT11_Data_Array[2],DHT11_Data_Array[3]);
		}

		// OLED��ʾ��ʪ��
		DHT11_NUM_Char();	// DHT11����ֵת���ַ���
		os_printf("=��LED ��ʾ\r\n");
		OLED_ShowString(0,2,DHT11_Data_Char[0]);	// DHT11_Data_Char[0] == ��ʪ���ַ�����
		OLED_ShowString(0,6,DHT11_Data_Char[1]);	// DHT11_Data_Char[1] == ���¶��ַ�����
	}
	os_printf("=�������ݷ��ؽ�� %d",res);

}

// �����ʱ����ʼ��(ms����)
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive) // time_repetitive 1�ظ���0���ظ�
{
	os_printf("=> time_ms %d time_repetivere %d\n",time_ms,time_repetitive);
	os_timer_disarm(&OS_Timer_1);	// �رն�ʱ��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ���ö�ʱ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ʹ�ܶ�ʱ��
}

// LED��ʼ��
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	os_printf("\r\n=============init Led =====================\r\n");
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());

	OLED_Init();							// OLED��ʼ��
	OLED_ShowString(0,0,"Humidity:");		// ʪ��
	OLED_ShowString(0,4,"Temperature:");	// �¶�

	LED_Init_JX();		// LED��ʼ��
	os_printf("=>init Timer\r\n");

	OS_Timer_1_Init_JX(3000,1);		// 3�붨ʱ(�ظ�)
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
