#include "user_config.h"		// 用户配置
#include "ets_sys.h"
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "osapi.h" 	// 系统函数
#include "user_interface.h"  // 系统函数
#include "driver/uart.h"  // 串口
#include "ets_sys.h"			// 回调函数
#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro

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
void ICACHE_FLASH_ATTR		// 初始化函数
user_init(void)
{
	u8 F_LED = 0;  // LED状态标志位
	u8 F_LEDO = 0;
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);
	os_printf("\r\n---------------------------------\r\n");  // 系统函数

	os_printf("SDK version:%s\n",system_get_sdk_version());		// 串口打印SDK版本
	// 管脚功能选择(注意：参数1【PIN_NAME】管脚名、参数2【FUNC】管脚功能)
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4设为IO口
//	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);			// IO4 = 1(高电平)
//	while(1)
//		{
//			system_soft_wdt_feed();					// 喂狗，防止复位
//
//			F_LED = !F_LED;							// LED状态值翻转
//
//			GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);	// 设置LED(IO4)输出值
//
//			delay_ms(500);							// 延时500Ms
//		}

	// 初始化LED(注意【PIN_NAME】、【FUNC】、【gpio_no】不要混淆)
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO_4设为IO口  D2  led
		GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// GPIO_4 = 1

		// 初始化按键(BOOT == GPIO0)
		//----------------------------------------------------------------------------------
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U,	FUNC_GPIO0);	// GPIO_0设为IO口  boot 键
		GPIO_DIS_OUTPUT(GPIO_ID_PIN(0));						// GPIO_0失能输出(默认)
		PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0失能内部上拉(默认)
	//	PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO0_U);					// GPIO_0使能内部上拉


		while(1)
		{
			system_soft_wdt_feed();// 喂狗，防止复位

			if( GPIO_INPUT_GET(GPIO_ID_PIN(0)) == 0 )
			{// 读取GPIO_0电平
				os_printf("\r\n-------- LED亮----------------\r\n");  // 系统函数
				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),0);				// LED亮
			}else
			{
				GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);				// LED灭

				os_printf("\r\n-------- LED灭----------------\r\n");  // 系统函数

			}

		}
}

