#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
    #include <platform/disp_drv_platform.h>
	#include <platform/mt_gpio.h>
	#include <platform/mt_i2c.h> 
	#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
//	#include <mach/mt_pm_ldo.h>
	#include "upmu_common.h"
    #include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif
//#include <cust_gpio_usage.h>

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)
/*
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#else
//#define GPIO_LCD_PWR_EN      0xFFFFFFFF
#endif
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#else
//#define GPIO_LCD_BL_EN      0xFFFFFFFF
#endif
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCM_RST_EN      GPIO_LCM_RST
#else
#define GPIO_LCM_RST_EN      0xFFFFFFFF
#endif
*/
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    


//static unsigned char lcd_id_pins_value = 0xFF;
//liuhanhui 20150917 +++
//static const unsigned char LCD_MODULE_ID = 0x01; //  haobing modified 2013.07.11
const static unsigned char LCD_MODULE_ID = 0xc5;
//liuhanhui 20150917 ---
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE 0
#define FRAME_WIDTH  										(600)
#define FRAME_HEIGHT 										(1024)

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER



// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------


struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//update initial param for IC nt35523 0.01
static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28,0,{}},
	{0x10,0,{}},
	{REGFLAG_DELAY, 120, {}}
};
	
//update initial param for IC nt35523 0.01
static void init_lcm_registers(void)
{
	  unsigned int data_array[16];

data_array[0] = 0x00023902;
data_array[1] = 0x000000B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A4BA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000071BD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000022BE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000019BF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000010C0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CC2;      //2a
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000082C3;    //34
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000073C4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000005C5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000002C6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000005C7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003C8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000018C9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000001B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000B1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CB2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CB3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACB4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCB5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CB6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CB7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCB8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CB9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CBA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CBB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BBC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CABD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6BE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024BF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004EC0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DC1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000C2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015C3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AAC4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFC5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003C6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000C7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CC8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CC9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACCA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCCB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CCC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CCD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCCE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CCF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CD0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CD1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BD2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CAD3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6D4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024D5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004ED6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DD7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000D8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015D9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AADA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFDB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003DC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000DD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CDE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CDF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACE0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCE1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CE2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CE3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCE4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CE5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CE6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CE7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BE8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CAE9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6EA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024EB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004EEC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DED;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000EE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015EF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AAF0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFF1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003F2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000002B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000B1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CB2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CB3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACB4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCB5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CB6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CB7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCB8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CB9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CBA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CBB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BBC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CABD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6BE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024BF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004EC0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DC1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000C2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015C3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AAC4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFC5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003C6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000C7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CC8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CC9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACCA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCCB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CCC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CCD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCCE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CCF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CD0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CD1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BD2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CAD3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6D4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024D5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004ED6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DD7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000D8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015D9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AADA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFDB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003DC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000DD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002CDE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CDF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000ACE0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000DCE1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005CE2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009CE3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CCE4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000CE5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00003CE6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00006CE7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009BE8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000CAE9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000F6EA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000024EB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004EEC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009DED;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000EE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000015EF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AAF0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000AFF1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003F2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000040C0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000010C1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00002FC4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00009EC5;        //9e
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000041C8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000061C9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000001CA;                       //page3 
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000CB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000001DC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000007DD;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000005DE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000DF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000E9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000EA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000EB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000002EC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000008ED;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000006EE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000EF;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F1;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F4;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F6;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000F9;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000FA;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000000FB;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000005B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000052B3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000006B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A5B8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A5C0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00001FC7;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00004FCE;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005AB8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005AC0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000003B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A5B2;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x000004B3;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000FB0;
dsi_set_cmdq(&data_array[0], 2, 1);

//data_array[0] = 0x00200500;  					  
//dsi_set_cmdq(data_array, 1, 1);
//MDELAY(10);

data_array[0] = 0x00110500;  				 
dsi_set_cmdq(data_array, 1, 1);

MDELAY(50);

data_array[0] = 0x00023902;
data_array[1] = 0x000006B0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A5B8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x0000A5C0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00001FCC;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005AB8;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00005AC0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00023902;
data_array[1] = 0x00000FB0;
dsi_set_cmdq(&data_array[0], 2, 1);

data_array[0] = 0x00290500;  //display on						 
dsi_set_cmdq(data_array, 1, 1);
MDELAY(200);

}

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                if(table[i].count <= 10)
                    MDELAY(table[i].count);
                else
                    MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = SYNC_EVENT_VDO_MODE;
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      		= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	params->dsi.packet_size=256;
	//video mode timing
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

       params->dsi.word_count=FRAME_WIDTH*3;
#if 0

	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 4;
	params->dsi.vertical_frontporch					= 8;
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 16;
	params->dsi.horizontal_backporch				= 48;
	params->dsi.horizontal_frontporch				= 16;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 230; 
	#else
	
	params->dsi.vertical_sync_active				= 2;//4;
	params->dsi.vertical_backporch					= 10; //10
	params->dsi.vertical_frontporch					= 6;  //22
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 24;//16
	params->dsi.horizontal_backporch				= 60;//48
	params->dsi.horizontal_frontporch				= 20;//100
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 167; 
	#endif
	params->dsi.clk_lp_per_line_enable   = 1;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x53;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

static void lcm_init_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
		
#else
	printk("[Kernel/LCM] lcm_init_power() enter\n");
	
#endif

}

static void lcm_suspend_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_suspend_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(20);
			
#else
	printk("[Kernel/LCM] lcm_suspend_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(20);
		
#endif

}

static void lcm_resume_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
			
#else
	printk("[Kernel/LCM] lcm_resume_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
		
#endif

}

static void lcm_init(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init() enter\n");
        //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ONE);//ADD LIUHANHUI
        //MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(10);
	
	// when phone initial , config output high, enable backlight drv chip  
//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	init_lcm_registers();
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
	printf("[LK/LCM] lcm_init() end\n");
#else

	printk("[Kernel/LCM] lcm_init() enter\n");

#endif
}

static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");
         //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ZERO);//ADD LIUHANHUI
      //   MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
#else
	printk("[Kernel/LCM] lcm_suspend() enter\n");
         //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ZERO);//ADD LIUHANHUI
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  

	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
	printk("[Kernel/LCM] lcm_suspend() end\n");

#endif
}

static void lcm_resume(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume() enter\n");
        //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ONE);//ADD LIUHANHUI
    //     MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	// when phone initial , config output high, enable backlight drv chip  
	init_lcm_registers();

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
#else
	printk("[Kernel/LCM] lcm_resume() enter\n");
      //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ONE);//ADD LIUHANHUI
   //  MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	// when phone initial , config output high, enable backlight drv chip  
	init_lcm_registers();

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	printk("[Kernel/LCM] lcm_resume() end\n");
#endif
}
LCM_DRIVER l695_gx_kd070d27_lcm_drv =
{
    .name			= "l695_gx_kd070d27",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    //.compare_id     = lcm_compare_id,
     .init_power		= lcm_init_power,
     .resume_power = lcm_resume_power,
     .suspend_power = lcm_suspend_power,
#if (LCM_DSI_CMD_MODE)
//    .update         = lcm_update,
#endif
};

