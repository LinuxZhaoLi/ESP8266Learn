#include "user_config.h"		// 用户配置
#include "driver/uart.h"  		// 串口

#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "ip_addr.h"			// 被"espconn.h"使用
#include "espconn.h"			// TCP/UDP接口
#include "ets_sys.h"			// 回调函数
#include "mem.h"				// 内存申请等函数
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX、软件定时器

#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro

#define		ProjectName			"Task"		// 工程名宏定义

#define		MESSAGE_QUEUE_LEN	2			// 消息队列深度(对于同一个任务，系统最多接受的叠加任务数)

os_event_t * Pointer_Task_1 ;	// 定义任务1(串口打印任务)		// 第①步：定义任务指针
/*
  struct ETSEventTag {
    ETSSignal sig;
    ETSParam  par;
};*/
void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}


// 任务执行函数(形参：类型必须为【os_event_t *】)		// 第③步：创建任务函数
//======================================================================================
void Func_Task_1(os_event_t * Task_message)	// Task_message = Pointer_Task_1
{
	// Task_message->sig=消息类型、Task_message->par=消息参数	// 第⑥步：编写任务函数(根据消息类型/消息参数实现相应功能)
	os_printf("消息类型=%d，消息参数=%c\r\n",Task_message->sig, Task_message->par);
}



// user_init：entry of user application, init user function here
//==============================================================================
void ICACHE_FLASH_ATTR user_init(void)
{
	u8 C_Task = 0 ;			// 调用任务计数
	u8 Message_Type = 1;	// 消息类型
	u8 Message_Para = 'A';	// 消息参数


	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// 给任务1分配空间(任务1空间 = 1个队列空间 * 队列数)	// 第②步：为任务分配内存
	//-------------------------------------------------------------------------------
	Pointer_Task_1=(os_event_t*)os_malloc((sizeof(os_event_t))*MESSAGE_QUEUE_LEN);

	// 创建任务(参数1=任务执行函数 / 参数2=任务等级 / 参数3=任务空间指针 / 参数4=消息队列深度)	// 第④步：创建任务
	//-----------------------------------------------------------------------------------------
	system_os_task(Func_Task_1, USER_TASK_PRIO_0, Pointer_Task_1, MESSAGE_QUEUE_LEN);


	// 调用4次任务
	//--------------------------------
	for(C_Task=1; C_Task<=4; C_Task++)
	{
		system_soft_wdt_feed();	// 喂狗，防止复位

		delay_ms(1000);			// 延时1秒
		os_printf("\r\n安排任务：Task == %d\r\n",C_Task);

		// 调用任务(参数1=任务等级 / 参数2=消息类型 / 参数3=消息参数)
		// 注意：参数3必须为无符号整数，否则需要强制类型转换
		//---------------------------------------------------------------
		system_os_post(USER_TASK_PRIO_0, Message_Type++, Message_Para++);	// 第⑤步：给系统安排任务
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
