#include "user_config.h"		// 用户配置
#include "driver/uart.h"  		// 串口
#include "c_types.h"			// 变量类型
#include "eagle_soc.h"			// GPIO函数、宏定义
#include "ets_sys.h"			// 回调函数
#include "os_type.h"			// os_XXX
#include "osapi.h"  			// os_XXX、软件定时器

#include "user_interface.h" 	// 系统接口、system_param_xxx接口、WIFI、RateContro



#define		ProjectName		"HW_Timer"		// 工程名宏定义
u8 F_LED = 0;	// LED状态标志位


void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

// 硬件定时器中断回调函数【注意：中断函数前不要有"ICACHE_FLASH_ATTR"宏】
void HW_Timer_INT(void)		// ②：硬件定时器中断回调函数
{
	F_LED = !F_LED;
	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),F_LED);		// LED状态翻转

	os_printf("\r\n--- HW_Timer_INT ---\r\n");	// 进入硬件定时器中断回调函数标志
}

void ICACHE_FLASH_ATTR LED_Init_JX(void)
{
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U,	FUNC_GPIO4);	// GPIO4设为IO口

	GPIO_OUTPUT_SET(GPIO_ID_PIN(4),1);						// IO4 = 1
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	LED_Init_JX();		// LED初始化


    // 硬件定时器初始化【FRC1_SOURCE==0、NMI_SOURCE=1】
	hw_timer_init(0, 1);				// ①：初始化硬件定时器【参数1：中断源】【参数2：是否重复】
	hw_timer_set_func(HW_Timer_INT);	// ③：注册硬件定时器中断回调函数
	hw_timer_arm(500000);				// ④：设置定时器参数(单位us，参数必须<=1,677,721)
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
