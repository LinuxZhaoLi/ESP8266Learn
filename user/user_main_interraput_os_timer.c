#include "user_config.h"		// �û�����
#include "ets_sys.h"
#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "osapi.h" 	// ϵͳ����
#include "user_interface.h"  // ϵͳ����
#include "driver/uart.h"  // ����
#include "ets_sys.h"			// �ص�����
#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro
#define		ProjectName		"GPIO_EXTI"
u8 F_LED = 0;		// LED״̬��־λ

//OS_Timer_1���붨��Ϊȫ�ֱ�������ΪESP8266���ں˻�Ҫʹ��

/*
 typedef struct _ETSTIMER_ {
    struct _ETSTIMER_    *timer_next;
    uint32_t              timer_expire;
    uint32_t              timer_period;
    ETSTimerFunc         *timer_func;
    void                 *timer_arg;
} ETSTimer;
*/
os_timer_t OS_Timer_1;  // ��ʱ������
// �����ʱ�� �ص�����
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)		// �ڣ�����ص�����
{
	F_LED = !F_LED;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);		// LED״̬��ת
	os_printf("\r\n----OS_Timer_1_cb----\r\n");	// ����ص�������־
}


// �����ʱ����ʼ��(ms����)
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	// �رն�ʱ��
	// ����һ��Ҫ�رյĶ�ʱ��
	os_timer_disarm(&OS_Timer_1);	// �ۣ��ر������ʱ��
	// ���ö�ʱ��
	// ����һ��Ҫ���õĶ�ʱ�������������ص�����(������ת��)�����������ص������Ĳ���
	//��ע��os_timer_setfn�����������ʱ��δʹ�ܵ�����µ��á� ����ת��
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// �ܣ����ûص�����
	// ʹ��(����)ms��ʱ��
	// ����һ��Ҫʹ�ܵĶ�ʱ��������������ʱʱ�䣨��λ��ms������������1=�ظ�/0=ֻһ��
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // �ݣ����ö�ʱ��������ʹ�ܶ�ʱ��
	// ����δ����system_timer_reinit����֧�ַ�Χ��[5ms �� 6,870,947ms]��
	// ���������system_timer_reinit����֧�ַ�Χ��[100ms �� 428,496 ms]��
}
// ��ʼ��LED
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4��ΪIO��

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}

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


// �жϻص�
void GPIO_INTERRUPT(void)
{
	u32	S_GPIO_INT;		// ����IO�ڵ��ж�״̬
	u32 F_GPIO_0_INT;	// GPIO_0���ж�״̬

	// ��ȡGPIO�ж�״̬  ��ȡ�Ĵ���ֵ
	S_GPIO_INT = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	os_printf("\r\n========�ж�״̬  %d========\r\n",S_GPIO_INT);

	// ����ж�״̬λ(��������״̬λ�������������ж�)
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, S_GPIO_INT);  //


	F_GPIO_0_INT = S_GPIO_INT & (0x01<<0);	// ��ȡGPIO_0�ж�״̬

	// �ж��Ƿ���KEY�ж�(δ������)
	if(F_GPIO_0_INT)	// GPIO_0���½����ж�
	{
		F_LED = !F_LED;

		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);	// LED״̬��ת
	}
}

// �жϵ���

void ICACHE_FLASH_ATTR user_init1(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// ��ʼ��LED(ע�⡾PIN_NAME������FUNC������gpio_no����Ҫ����)
	//-------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4��ΪIO��    ����Ϊgpio��
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1  ����ߵ�ƽ


	// ��ʼ������(BOOT == GPIO0)
	//-----------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0��ΪGPIO��  boot
	// ���عܽ�
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0ʧ�����(Ĭ��)  �����빦��
//	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʧ������(Ĭ��)
//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0ʹ������


	// GPIO_0�ж�����
	//----------------------------------------------------------------------------------
	ETS_GPIO_INTR_DISABLE();										// �ر�GPIO�жϹ���
	//                     ����ת��
	ETS_GPIO_INTR_ATTACH((ets_isr_t)GPIO_INTERRUPT,NULL);			// ע���жϻص�����  ����  �������� ����
	gpio_pin_intr_state_set(GPIO_ID_PIN(0),GPIO_PIN_INTR_NEGEDGE);	// ����GPIO_0�½����ж�  �����½��������ж�һ��
	ETS_GPIO_INTR_ENABLE();		// ���жϹ���									// ��GPIO�жϹ���


}


// �����ʱ��
void ICACHE_FLASH_ATTR user_init1(void)
{
	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");
	LED_Init_JX();  // ��ʼ��LED
	OS_Timer_1_Init_JX(500,1);// ��ʼ������ж�

	// ��ѭ�� �ᵼ�������ʱ�� �����á�

}

