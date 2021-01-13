#include "user_config.h"		// �û�����
#include "driver/uart.h"  		// ����

#include "c_types.h"			// ��������
#include "eagle_soc.h"			// GPIO�������궨��
#include "ip_addr.h"			// ��"espconn.h"ʹ��
#include "espconn.h"			// TCP/UDP�ӿ�
#include "ets_sys.h"			// �ص�����
#include "mem.h"				// �ڴ�����Ⱥ���
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX�������ʱ��

#include "user_interface.h" 	// ϵͳ�ӿڡ�system_param_xxx�ӿڡ�WIFI��RateContro

#define		ProjectName			"Task"		// �������궨��

#define		MESSAGE_QUEUE_LEN	2			// ��Ϣ�������(����ͬһ������ϵͳ�����ܵĵ���������)

os_event_t * Pointer_Task_1 ;	// ��������1(���ڴ�ӡ����)		// �ڢٲ�����������ָ��
/*
  struct ETSEventTag {
    ETSSignal sig;
    ETSParam  par;
};*/
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}


// ����ִ�к���(�βΣ����ͱ���Ϊ��os_event_t *��)		// �ڢ۲�������������
//======================================================================================
void Func_Task_1(os_event_t * Task_message)	// Task_message = Pointer_Task_1
{
	// Task_message->sig=��Ϣ���͡�Task_message->par=��Ϣ����	// �ڢ޲�����д������(������Ϣ����/��Ϣ����ʵ����Ӧ����)
	os_printf("��Ϣ����=%d����Ϣ����=%c\r\n",Task_message->sig, Task_message->par);
}



// user_init��entry of user application, init user function here
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	u8 C_Task = 0 ;			// �����������
	u8 Message_Type = 1;	// ��Ϣ����
	u8 Message_Para = 'A';	// ��Ϣ����


	uart_init(115200,115200);	// ��ʼ�����ڲ�����
	os_delay_us(10000);			// �ȴ������ȶ�
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// ������1����ռ�(����1�ռ� = 1�����пռ� * ������)	// �ڢڲ���Ϊ��������ڴ�
	//-------------------------------------------------------------------------------
	Pointer_Task_1=(os_event_t*)os_malloc((sizeof(os_event_t))*MESSAGE_QUEUE_LEN);

	// ��������(����1=����ִ�к��� / ����2=����ȼ� / ����3=����ռ�ָ�� / ����4=��Ϣ�������)	// �ڢܲ�����������
	//-----------------------------------------------------------------------------------------
	system_os_task(Func_Task_1, USER_TASK_PRIO_0, Pointer_Task_1, MESSAGE_QUEUE_LEN);


	// ����4������
	//--------------------------------
	for(C_Task=1; C_Task<=4; C_Task++)
	{
		system_soft_wdt_feed();	// ι������ֹ��λ

		delay_ms(1000);			// ��ʱ1��
		os_printf("\r\n��������Task == %d\r\n",C_Task);

		// ��������(����1=����ȼ� / ����2=��Ϣ���� / ����3=��Ϣ����)
		// ע�⣺����3����Ϊ�޷���������������Ҫǿ������ת��
		//---------------------------------------------------------------
		system_os_post(USER_TASK_PRIO_0, Message_Type++, Message_Para++);	// �ڢݲ�����ϵͳ��������
	}

	os_printf("\r\n------------------ user_init OVER ----------------\r\n\r\n");
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
