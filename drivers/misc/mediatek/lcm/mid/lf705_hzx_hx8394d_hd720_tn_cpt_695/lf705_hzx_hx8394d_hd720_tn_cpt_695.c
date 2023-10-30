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
#define FRAME_WIDTH  (720)
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
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)

#define GPIO_LCD_RST      (GPIO146 | 0x80000000)



#define GPIO_LCM_STB       (GPIO147 | 0x80000000)




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


struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};





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
		//params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		#if 1
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		#endif
		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
	//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	    params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //   params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 5;//1// by eric.wang
		params->dsi.vertical_backporch					= 10;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 15;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;//10
		params->dsi.horizontal_backporch				= 48;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 16;//60 by eric.wang 160
		//params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK 	= 190;//170 by eric.wang
		
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

data_array[0] = 0x00043902;
data_array[1] = 0x9483FFB9;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00033902;
data_array[1] = 0x008333BA;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00103902;
data_array[1] = 0x12126cB1;
data_array[2] = 0xF1110434;
data_array[3] = 0x2395EA80;
data_array[4] = 0x58D2C080;
dsi_set_cmdq(data_array, 5, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00073902;
data_array[1] = 0x106400B2;
data_array[2] = 0x001C3207;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x000d3902;
data_array[1] = 0x01FF00B4;
data_array[2] = 0x015A015A;
data_array[3] = 0x0170015A;
data_array[4] = 0x00000070;
dsi_set_cmdq(data_array, 5, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00263902;
data_array[1] = 0x000600D3;
data_array[2] = 0x00000701;
data_array[3] = 0x00081032;
data_array[4] = 0x0E155208;
data_array[5] = 0x10320E05;
data_array[6] = 0x37000000;
data_array[7] = 0x370C0C33;
data_array[8] = 0x08470C0C;
data_array[9] = 0x0A000000;
data_array[10] = 0x00000100;
dsi_set_cmdq(data_array, 11, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x0BCC1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x002d3902;
data_array[1] = 0x1A1B1BD5;
data_array[2] = 0x0207061A;
data_array[3] = 0x00050403;
data_array[4] = 0x18212001;
data_array[5] = 0x18181818;
data_array[6] = 0x18181818;
data_array[7] = 0x18181818;
data_array[8] = 0x18181818;
data_array[9] = 0x18181818;
data_array[10] = 0x19181818;
data_array[11] = 0x18192418;
data_array[12] = 0x00000025;
dsi_set_cmdq(data_array, 13, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x002d3902;
data_array[1] = 0x1A1B1BD6;
data_array[2] = 0x0500011A;
data_array[3] = 0x07020304;
data_array[4] = 0x18242506;
data_array[5] = 0x18181818;
data_array[6] = 0x18181818;
data_array[7] = 0x18181818;
data_array[8] = 0x18181818;
data_array[9] = 0x18181818;
data_array[10] = 0x18181918;
data_array[11] = 0x18182119;
data_array[12] = 0x00000020;
dsi_set_cmdq(data_array, 13, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x002b3902;
data_array[1] = 0x1E1600E0;
data_array[2] = 0x2E390C0E;
data_array[3] = 0x0F0D093B;
data_array[4] = 0x17141119;
data_array[5] = 0x14081515;
data_array[6] = 0x12001615;
data_array[7] = 0x390C0E1B;
data_array[8] = 0x0E082E26;
data_array[9] = 0x1410180F;
data_array[10] = 0x08161517;
data_array[11] = 0x00161514;
dsi_set_cmdq(data_array, 12, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00053902;
data_array[1] = 0x00C000C7;
data_array[2] = 0x000000C0;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);

data_array[0] = 0x09091500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x1DC61500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x09091500;
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
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init() enter\n");

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	//MDELAY(50);
		lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);

	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);
	


	MDELAY(10);

	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1); 
	lcm_initial_registers();
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	//lcm_set_gpio_output(GPIO_DISP_PWM, GPIO_OUT_ONE);//eric.wang
	printf("[LK/LCM] lcm_init() end\n");
#else
	printk("[Kernel/LCM] lcm_init() enter\n");

	//hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "LCM");
	
	
	//lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	//MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	lcm_initial_registers();
		MDELAY(100);

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
//	lcm_initial_registers();
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_init() end\n");

	
#endif
	//lcm_initial_registers();
}



static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");

	//lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	//MDELAY(100);
	
	//VGP1 1.8V
	//upmu_set_rg_vgp1_vosel(0x0);
	//upmu_set_rg_vgp1_en(0x0);
	//MDELAY(50);	



	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	printf("[LK/LCM] lcm_suspend() end\n");
#else
	printk("[Kernel/LCM] lcm_suspend() enter\n");
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ZERO);	
	//lcm_set_gpio_output(GPIO_DISP_PWM, GPIO_OUT_ZERO);//eric.wang

	MDELAY(10);
	

	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST,GPIO_OUT_ZERO);
	//MDELAY(100);
	printk("[Kernel/LCM] lcm_suspend() end\n");
#endif
}

static void lcm_resume(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume() enter\n");

	//lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	//MDELAY(50);

	//VGP1 1.8V
	//upmu_set_rg_vgp1_vosel(0x3);
	//upmu_set_rg_vgp1_en(0x1);
	//MDELAY(10);
	
	//lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	//MDELAY(300);	

	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	printf("[LK/LCM] lcm_resume() end\n");
#else
	printk("[Kernel/LCM] lcm_resume() enter\n");


	//hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "LCM");//by eric.wang
	
	
	//lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	//MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
		lcm_initial_registers();
		MDELAY(100);

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
//	lcm_initial_registers();
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_resume() end\n");
	//MDELAY(10);
#endif
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508; 				//HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}
#endif


LCM_DRIVER lf705_hzx_hx8394d_hd720_tn_cpt_695_lcm_drv = 
{
  .name				= "lf705_hzx_hx8394d_hd720_tn_cpt_695",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
//	.init_power		= lcm_power,
#if (LCM_DSI_CMD_MODE)
  .update         	= lcm_update,
#endif
};
