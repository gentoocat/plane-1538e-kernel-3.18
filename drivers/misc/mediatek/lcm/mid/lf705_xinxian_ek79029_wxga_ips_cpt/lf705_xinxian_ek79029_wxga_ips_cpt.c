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

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
//#define GPIO_LCD_BL_EN      0xffffffff
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)

#define GPIO_LCD_RST      (GPIO146 | 0x80000000)



#define GPIO_LCM_STB       (GPIO147 | 0x80000000)


#define USING_TABLE 0


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



static void lcm_initial_registers(void)
{		
	 unsigned int data_array[16];
data_array[0] = 0xAACD1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00301500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x035E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00321500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x25331500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x103A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x49361500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x82671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x27691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x016C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x146D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00093902;
data_array[1] = 0x09090955;
data_array[2] = 0x09090909;
data_array[3] = 0x00000009;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x09090956;
data_array[2] = 0x09090909;
data_array[3] = 0x09090909;
data_array[4] = 0x09090909;
data_array[5] = 0x00000009;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x006B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x30731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x35281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x09000057;
data_array[2] = 0x00000009;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x6C411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x84611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x504E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4F4F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x14151853;
data_array[2] = 0x14121211;
data_array[3] = 0x0D111616;
data_array[4] = 0x0D0D0B0C;
data_array[5] = 0x0003080C;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x14151854;
data_array[2] = 0x14121211;
data_array[3] = 0x0D111616;
data_array[4] = 0x0D0D0B0C;
data_array[5] = 0x0003080C;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x10291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0D2E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x67781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x36761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x18471500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x312D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xd0501500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66481500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x004D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);



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
		params->dsi.mode   = BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		#if 0
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		#endif
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		
		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
		//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	    params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
        params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active = 2;//4; //4;
		params->dsi.vertical_backporch =  10;//4; //14;  //16;
		params->dsi.vertical_frontporch = 12;//8; //16; //15;
		params->dsi.vertical_active_line = FRAME_HEIGHT;

		params->dsi.horizontal_sync_active = 25;//8; //4; //4;
		params->dsi.horizontal_backporch =	25; //8; //4; //42;  //50;
		params->dsi.horizontal_frontporch = 25;//40; //105; //16; //44; //60;
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;
		
		params->dsi.PLL_CLOCK=205;
		
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if(GPIO==0xffffffff){
		return ;
	}
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}


static void lcm_reset(int value)
{
	lcm_set_gpio_output(GPIO_LCD_RST, value);
}

static void lcm_standby(int value)
{
	lcm_set_gpio_output(GPIO_LCM_STB, value);
}

static void lcm_power_en(int value)
{
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, value);
}

static void lcm_bl_en(int value)
{
	lcm_set_gpio_output(GPIO_LCD_BL_EN, value);
}


static void lcm_init(void)
{
	lcm_power_en(0);
	lcm_power_en(1);
	MDELAY(20);
	lcm_standby(1);

	lcm_reset(1);
	MDELAY(20);
	lcm_reset(0);	
	MDELAY(20);
	lcm_reset(1);
	MDELAY(130);
	lcm_initial_registers();

	lcm_bl_en(1);
}



static void lcm_suspend(void)
{
	lcm_bl_en(0);
	MDELAY(10);	
	lcm_standby(0);
	MDELAY(10);	
	lcm_reset(0);	
	MDELAY(10);	
	lcm_power_en(0);
	MDELAY(10);	
}

static void lcm_resume(void)
{
	lcm_init();
}




LCM_DRIVER lf705_xinxian_ek79029_wxga_ips_cpt_lcm_drv = 
{
  .name				= "lf705_xinxian_ek79029_wxga_ips_cpt",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
