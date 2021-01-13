#include "user_config.h"		// 用户配置
#include "ets_sys.h"
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "osapi.h" 	// 系统函数
#include "user_interface.h"  // 系统函数
#include "driver/uart.h"  // 串口
#include "ets_sys.h"			// 回调函数
#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro
#define		ProjectName		"GPIO_EXTI"
u8 F_LED = 0;		// LED状态标志位

//OS_Timer_1必须定义为全局变量，因为ESP8266的内核还要使用

/*
 typedef struct _ETSTIMER_ {
    struct _ETSTIMER_    *timer_next;
    uint32_t              timer_expire;
    uint32_t              timer_period;
    ETSTimerFunc         *timer_func;
    void                 *timer_arg;
} ETSTimer;
*/
os_timer_t OS_Timer_1;  // 定时器变量
// 软件定时器 回调函数
void ICACHE_FLASH_ATTR OS_Timer_1_cb(void)		// ②：定义回调函数
{
	F_LED = !F_LED;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);		// LED状态翻转
	os_printf("\r\n----OS_Timer_1_cb----\r\n");	// 进入回调函数标志
}


// 软件定时器初始化(ms毫秒)
void ICACHE_FLASH_ATTR OS_Timer_1_Init_JX(u32 time_ms, u8 time_repetitive)
{
	// 关闭定时器
	// 参数一：要关闭的定时器
	os_timer_disarm(&OS_Timer_1);	// ③：关闭软件定时器
	// 设置定时器
	// 参数一：要设置的定时器；参数二：回调函数(需类型转换)；参数三：回调函数的参数
	//【注：os_timer_setfn必须在软件定时器未使能的情况下调用】 类型转化
	os_timer_setfn(&OS_Timer_1,(os_timer_func_t *)OS_Timer_1_cb, NULL);	// ④：设置回调函数
	// 使能(启动)ms定时器
	// 参数一：要使能的定时器；参数二：定时时间（单位：ms）；参数三：1=重复/0=只一次
	os_timer_arm(&OS_Timer_1, time_ms, time_repetitive);  // ⑤：设置定时器参数并使能定时器
	// 【如未调用system_timer_reinit，可支持范围：[5ms ～ 6,870,947ms]】
	// 【如果调用system_timer_reinit，可支持范围：[100ms ～ 428,496 ms]】
}
// 初始化LED
void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4设为IO口

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


// 中断回调
void GPIO_INTERRUPT(void)
{
	u32	S_GPIO_INT;		// 所有IO口的中断状态
	u32 F_GPIO_0_INT;	// GPIO_0的中断状态

	// 读取GPIO中断状态  读取寄存器值
	S_GPIO_INT = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	os_printf("\r\n========中断状态  %d========\r\n",S_GPIO_INT);

	// 清除中断状态位(如果不清除状态位，则会持续进入中断)
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, S_GPIO_INT);  //


	F_GPIO_0_INT = S_GPIO_INT & (0x01<<0);	// 获取GPIO_0中断状态

	// 判断是否是KEY中断(未做消抖)
	if(F_GPIO_0_INT)	// GPIO_0的下降沿中断
	{
		F_LED = !F_LED;

		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);	// LED状态翻转
	}
}

// 中断调试

void ICACHE_FLASH_ATTR user_init1(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// 初始化LED(注意【PIN_NAME】、【FUNC】、【gpio_no】不要混淆)
	//-------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4设为IO口    设置为gpio口
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1  输出高电平


	// 初始化按键(BOOT == GPIO0)
	//-----------------------------------------------------------------------------
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0作为GPIO口  boot
	// 返回管脚
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0失能输出(默认)  打开输入功能
//	PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0失能上拉(默认)
//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0使能上拉


	// GPIO_0中断设置
	//----------------------------------------------------------------------------------
	ETS_GPIO_INTR_DISABLE();										// 关闭GPIO中断功能
	//                     类型转化
	ETS_GPIO_INTR_ATTACH((ets_isr_t)GPIO_INTERRUPT,NULL);			// 注册中断回调函数  参数  函数名， 参数
	gpio_pin_intr_state_set(GPIO_ID_PIN(0),GPIO_PIN_INTR_NEGEDGE);	// 设置GPIO_0下降沿中断  读到下降沿信心中断一次
	ETS_GPIO_INTR_ENABLE();		// 打开中断功能									// 打开GPIO中断功能


}


// 软件定时器
void ICACHE_FLASH_ATTR user_init1(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");
	LED_Init_JX();  // 初始化LED
	OS_Timer_1_Init_JX(500,1);// 初始化软件中断

	// 死循环 会导致软件定时器 不能用。

}

