#ifndef BUILD_LK
#include <linux/string.h>
#else
#include <string.h>
#endif 

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#elif (defined BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
	#include "upmu_common.h"
    #include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (800)
#define FRAME_HEIGHT (1280)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#if 0
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN      GPIO_LCM_PWR_EN
#else
#define GPIO_LCD_PWR_EN      0xFFFFFFFF
#endif

#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST      GPIO_LCM_RST
#else
#define GPIO_LCD_RST      0xFFFFFFFF
#endif

#ifdef GPIO_LCD_BL_EN
#define GPIO_LCM_BL_EN      GPIO_LCD_BL_EN
#else
#define GPIO_LCM_BL_EN      0xFFFFFFFF
#endif
#else

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCM_BL_EN     	(GPIO82 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCD_RST      	(GPIO146 | 0x80000000)
#endif
//#define GPIO_LCD_PWR_EN 	 GPIO_LCM_PWR_EN
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	  lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)				lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)																			lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)									lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)																				lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   						lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE	0
#define REGFLAG_DELAY       0XFFE
#define REGFLAG_END_OF_TABLE    0xFFF   // END OF REGISTERS MARKER


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
		
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	  params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;

		//The following defined the fomat for data coming from LCD engine.

		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting	
	    params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
       params->dsi.word_count=FRAME_WIDTH*3;
	   
		params->dsi.vertical_sync_active				= 4;//4// by eric.wang
		params->dsi.vertical_backporch					= 12;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 2;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 8;//4
		params->dsi.horizontal_backporch				= 160;//132 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 16;//24 by eric.wang 160
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		

		params->dsi.PLL_CLOCK 	= 220;//		
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

static void lcm_initial_registers(void)
{
    unsigned int data_array[16];
data_array[0] = 0xAACD1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x387E1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00301500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00321500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00331500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x08651500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x043A1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00361500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x82671500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x23691500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0xC16C1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x006d1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00093902;
data_array[1] = 0x000F0055;
data_array[2] = 0x000F000F;
data_array[3] = 0x0000000F;
dsi_set_cmdq(data_array, 4, 1);
MDELAY(1);

data_array[0] = 0x00113902;
data_array[1] = 0x000F0056;
data_array[2] = 0x000F000F;
data_array[3] = 0x000F000F;
data_array[4] = 0x000F000F;
data_array[5] = 0x0000000F;
dsi_set_cmdq(data_array, 6, 1);
MDELAY(1);

data_array[0] = 0x006B1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x01581500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0xF0731500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x17741500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x035E1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x02681500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x006A1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x31281500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x08291500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x04631500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00093902;
data_array[1] = 0x000F0057;
data_array[2] = 0x000F000F;
data_array[3] = 0x0000000F;
dsi_set_cmdq(data_array, 4, 1);
MDELAY(1);

data_array[0] = 0x464E1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x4A4F1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x22411500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00143902;
data_array[1] = 0x1B1C1F53;
data_array[2] = 0x1A181817;
data_array[3] = 0x10151B1B;
data_array[4] = 0x0E0F0E0E;
data_array[5] = 0x0006080C;
dsi_set_cmdq(data_array, 6, 1);
MDELAY(1);

data_array[0] = 0x00143902;
data_array[1] = 0x1B1C1F54;
data_array[2] = 0x19181817;
data_array[3] = 0x10151B1B;
data_array[4] = 0x0D0E0D0E;
data_array[5] = 0x0006080C;
dsi_set_cmdq(data_array, 6, 1);
MDELAY(1);

data_array[0] = 0x00500500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00D00500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x34761500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00771500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x1F471500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x67781500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x67481500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x312D1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x004D1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);
   




}

static void lcm_init(void)
{
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	MDELAY(90);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);

	MDELAY(30);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);
	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(130);//Must > 5ms
	//lcd_bl_en(1);
	
	lcm_initial_registers();
}



static void lcm_suspend(void)
{
	unsigned int data_array[16];
   data_array[0] = 0x00280500;  //display off                        
   dsi_set_cmdq(data_array, 1, 1);

   data_array[0] = 0x00100500;
   dsi_set_cmdq(data_array,1,1);//enter into sleep
   MDELAY(5);
	
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);
	MDELAY(200);
    lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
    lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);

    MDELAY(10);

}

static void lcm_resume(void)
{
	lcm_init();
}

LCM_DRIVER u101_hygd_ek79029_hsd_wxga_ips_101_lcm_drv = 
{
  .name				= "u101_hygd_ek79029_hsd_wxga_ips_101",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
};
