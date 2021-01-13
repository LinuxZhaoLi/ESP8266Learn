#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ets_sys.h"			// �ص�����
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��

#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro



#define		ProjectName		"HW_Timer"		// �������궨��
u8 F_LED = 0;	// LED״̬��־λ


void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

// Ӳ����ʱ���жϻص�������ע�⣺�жϺ���ǰ��Ҫ��"ICACHE_FLASH_ATTR"�꡿
void HW_Timer_INT(void)		// �ڣ�Ӳ����ʱ���жϻص�����
{
	F_LED = !F_LED;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);		// LED״̬��ת

	os_printf("\r\n--- HW_Timer_INT ---\r\n");	// ����Ӳ����ʱ���жϻص�������־
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
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	LED_Init_JX();		// LED��ʼ��


    // Ӳ����ʱ����ʼ����FRC1_SOURCE==0��NMI_SOURCE=1��
	hw_timer_init(0, 1);				// �٣���ʼ��Ӳ����ʱ��������1���ж�Դ��������2���Ƿ��ظ���
	hw_timer_set_func(HW_Timer_INT);	// �ۣ�ע��Ӳ����ʱ���жϻص�����
	hw_timer_arm(500000);				// �ܣ����ö�ʱ������(��λus����������<=1,677,721)
}
//=================================================================================================

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
