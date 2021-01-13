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

#define		ProjectName			"Flash"		// 工程名宏定义

#define		SPI_FLASH_SEC_SIZE	4096		// Flash扇区大小

u16 N_Data_FLASH_SEC = 0x77;	// 存储数据的扇区编号
u32 A_W_Data[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};	// 写入Flash的数据
u32 A_R_Data[16] = {0};			// 缓存读Flash的数据

void ICACHE_FLASH_ATTR delay_ms(u32 C_time)
{	for(;C_time>0;C_time--)
		os_delay_us(1000);
}

void ICACHE_FLASH_ATTR user_init(void)
{
	u8 C_loop = 0;

	uart_init(115200,115200);	// 初始化串口波特率
	os_delay_us(10000);			// 等待串口稳定
	os_printf("\r\n=================================================\r\n");
	os_printf("\t Project:\t%s\r\n", ProjectName);
	os_printf("\t SDK version:\t%s", system_get_sdk_version());
	os_printf("\r\n=================================================\r\n");


	// 向【0x77 000】地址起，写入16个数据(每个数据占4字节)
	spi_flash_erase_sector(0x77);	// 擦除0x77扇区		参数==【扇区编号】
	// 向Flash写数据(参数1=【字节地址】、参数2=写入数据的指针、参数3=数据长度)
	spi_flash_write(0x77*4096, (uint32 *)A_W_Data, sizeof(A_W_Data));


	os_printf("\r\n---------- Write Flash Data OVER ----------\r\n");
	// 从【0x77 000】地址起，读出16个数据(每个数据占4字节) 参数1 字节地址， 参数2 将要写入数据指针，参数3 写入数据长度
	spi_flash_read(0x77*4096, (uint32 *)A_R_Data, sizeof(A_W_Data));
	// 串口打印读出的数据
	for(C_loop=0; C_loop<16; C_loop++)
	{
		os_printf("Read Data = %d \r\n",A_R_Data[C_loop]);
		delay_ms(10);
	}
	os_printf("\r\n\r\n------------ user_init OVER ------------\r\n\r\n");
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
