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
#define FRAME_WIDTH  										(800)
#define FRAME_HEIGHT 										(1280)

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

static struct LCM_setting_table lcm_initialization_setting[] = {
//liuhanhui add 20150808+++
#if 1

{0xFF, 4, {0xAA,0x55,0xA5,0x80}},//========== Internal setting ==========
{0x6F, 2, {0x11,0x00}},// MIPI related Timing Setting
{0xF7, 2, {0x20,0x00}},

{0x6F, 1, {0x06}},  ////  Improve ESD option
{0xF7, 1, {0xA0}},
{0x6F, 1, {0x19}},  
{0xF7, 1, {0x12}},

//{0xF4, 1, {0x03}},//majuscule red//20150821 from dingjingbo
{0x6F, 1, {0x08}}, // Vcom floating
{0xFA, 1, {0x40}}, 
{0x6F, 1, {0x11}}, 
{0xF3, 1, {0x01}}, 

{0xF0, 5, {0x55,0xAA,0x52,0x08,0x00}},//========== page0 relative ==========
{0xC8, 1, {0x80}}, 
{0xB1, 2, {0x6C,0x01}}, // Set WXGA resolution
{0xB6, 1, {0x08}}, // Set source output hold time
{0x6F, 1, {0x02}},//EQ control function
{0xB8, 1, {0x08}}, 
{0xBB, 2, {0x54,0x54}}, // Set bias current for GOP and SOP
{0xBC, 2, {0x05,0x05}}, // Inversion setting 
{0xC7, 1, {0x01}}, // zigzag setting
{0xBD, 5, {0x02,0xB0,0x0C,0x0A,0x00}},// DSP Timing Settings update for BIST

{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},//========== page1 relative ==========
{0xB0, 2, {0x05,0x05}}, // Setting AVDD, AVEE clamp
{0xB1, 2, {0x05,0x05}}, 

{0xBC, 2, {0x3A,0x01}}, // VGMP, VGMN, VGSP, VGSN setting
{0xBD, 2, {0x3E,0x01}}, 
{0xCA, 1, {0x00}}, // gate signal control
{0xC0, 1, {0x04}}, // power IC control

{0xB2, 2, {0x00,0x00}},// VCL SET -2.5V//20150821 from dingjingbo
{0xBE, 1, {0x80}},// VCOM = -1.888V

{0xB3, 2, {0x19,0x19}},// Setting VGH=15V, VGL=-11V
{0xB4, 2, {0x12,0x12}},
{0xB9, 2, {0x24,0x24}}, // power control for VGH, VGL
{0xBA, 2, {0x14,0x14}}, 

{0xF0, 5, {0x55,0xAA,0x52,0x08,0x02}},//========== page2 relative ==========
{0xEE, 1, {0x01}}, //gamma setting
{0xEF, 4, {0x09,0x06,0x15,0x18}},//Gradient Control for Gamma Voltage

{0xB0, 6, {0x00,0x00,0x00,0x08,0x00,0x17}},
{0x6F, 1, {0x06}},
{0xB0, 6, {0x00,0x25,0x00,0x30,0x00,0x45}},
{0x6F, 1, {0x0C}},
{0xB0, 4, {0x00,0x56,0x00,0x7A}},//20150821 from dingjingbo
{0xB1, 6, {0x00,0xA3,0x00,0xE7,0x01,0x20}},
{0x6F, 1, {0x06}},
{0xB1, 6, {0x01,0x7A,0x01,0xC2,0x01,0xC5}},
{0x6F, 1, {0x0C}},
{0xB1, 4, {0x02,0x06,0x02,0x5F}},
{0xB2, 6, {0x02,0x92,0x02,0xD0,0x02,0xFC}},
{0x6F, 1, {0x06}},
{0xB2, 6, {0x03,0x35,0x03,0x5D,0x03,0x8B}},
{0x6F, 1, {0x0C}},
{0xB2, 4, {0x03,0xA2,0x03,0xBF}},
{0xB3, 4, {0x03,0xD2,0x03,0xFF}},

{0xBC, 6, {0x00,0x00,0x00,0x08,0x00,0x18}},
{0x6F, 1, {0x06}},
{0xBC, 6, {0x00,0x27,0x00,0x32,0x00,0x49}},
{0x6F, 1, {0x0C}},
{0xBC, 4, {0x00,0x5C,0x00,0x83}},
{0xBD, 6, {0x00,0xAF,0x00,0xF3,0x01,0x2A}},
{0x6F, 1, {0x06}},
{0xBD, 6, {0x01,0x84,0x01,0xCA,0x01,0xC3}},
{0x6F, 1, {0x0C}},
{0xBD, 4, {0x02,0x0E,0x02,0x65}},
{0xBE, 6, {0x02,0x98,0x02,0xD4,0x03,0x00}},
{0x6F, 1, {0x06}},
{0xBD, 6, {0x03,0x37,0x03,0x5F,0x03,0x8D}},
{0x6F, 1, {0x0C}},
{0xBE, 4, {0x03,0xA4,0x03,0xBF}},
{0xBF, 4, {0x03,0xE8,0x03,0xFF}},
//========== GOA relative ==========
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x06}},// PAGE6 : GOUT Mapping, VGLO select
{0xB0, 2, {0x00,0x17}}, 
{0xB1, 2, {0x16,0x15}}, 
{0xB2, 2, {0x14,0x13}}, 
{0xB3, 2, {0x12,0x11}}, 
{0xB4, 2, {0x10,0x2D}}, 
{0xB5, 2, {0x01,0x08}}, 
{0xB6, 2, {0x09,0x31}}, 
{0xB7, 2, {0x31,0x31}}, 
{0xB8, 2, {0x31,0x31}}, 
{0xB9, 2, {0x31,0x31}}, 
{0xBA, 2, {0x31,0x31}}, 
{0xBB, 2, {0x31,0x31}}, 
{0xBC, 2, {0x31,0x31}}, 
{0xBD, 2, {0x31,0x09}},//20150821 from dingjingbo

{0xBE, 2, {0x08,0x01}}, 
{0xBF, 2, {0x2D,0x10}}, 
{0xC0, 2, {0x11,0x12}}, 
{0xC1, 2, {0x13,0x14}}, 
{0xC2, 2, {0x15,0x16}}, 
{0xC3, 2, {0x17,0x00}},

{0xE5, 2, {0x31,0x31}},
{0xC4, 2, {0x00,0x17}},
{0xC5, 2, {0x16,0x15}}, 
{0xC6, 2, {0x14,0x13}}, 
{0xC7, 2, {0x12,0x11}}, 
{0xC8, 2, {0x10,0x2D}}, 

{0xC9, 2, {0x01,0x08}}, 
{0xCA, 2, {0x09,0x31}}, 
{0xCB, 2, {0x31,0x31}}, 
{0xCC, 2, {0x31,0x31}}, 
{0xCD, 2, {0x31,0x31}}, 
{0xCE, 2, {0x31,0x31}},  
{0xCF, 2, {0x31,0x31}}, 

{0xD0, 2, {0x31,0x31}}, 
{0xD1, 2, {0x31,0x09}}, 
{0xD2, 2, {0x08,0x01}}, 
{0xD3, 2, {0x2D,0x10}}, 
{0xD4, 2, {0x11,0x12}}, 
{0xD5, 2, {0x13,0x14}}, 
{0xD6, 2, {0x15,0x16}}, 
{0xD7, 2, {0x17,0x00}}, 
{0xE5, 2, {0x31,0x31}},
{0xD8, 5, {0x00,0x00,0x00,0x00,0x00}}, //VGL level select
{0xD9, 5, {0x00,0x00,0x00,0x00,0x00}}, 
{0xE7, 1, {0x00}},

                                                           // PAGE3 :
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x03}},//gate timing control
{0xB0, 2, {0x20,0x00}}, 
{0xB1, 2, {0x20,0x00}}, 
{0xB2, 5, {0x05,0x00,0x42,0x00,0x00 }}, 
{0xB6, 5, {0x05,0x00,0x42,0x00,0x00 }}, 
{0xBA, 5, {0x53,0x00,0x42,0x00,0x00 }}, 
{0xBB, 5, {0x53,0x00,0x42,0x00,0x00 }}, 
{0xC4, 1, {0x40}},
// gate CLK EQ
// gate STV EQ

                                                           // PAGE5 :

{0xF0, 5, {0x55,0xAA,0x52,0x08,0x05}},
{0xB0, 2, {0x17,0x06}}, 
{0xB8, 1, {0x00}}, 
{0xBD, 5, {0x03,0x01,0x01,0x00,0x01}},
{0xB1, 2, {0x17,0x06}}, 

{0xB9, 2, {0x00,0x01}}, 
{0xB2, 2, {0x17,0x06}}, 
{0xBA, 2, {0x00,0x01}},
{0xB3, 2, {0x17,0x06}},

{0xBB, 2, {0x0A,0x00}},
{0xB4, 2, {0x17,0x06}},
{0xB5, 2, {0x17,0x06}},
{0xB6, 2, {0x14,0x03}},

{0xB7, 2, {0x00,0x00}},
{0xBC, 2, {0x02,0x01}},
{0xC0, 1, {0x05}},
{0xC4, 1, {0xA5}},

{0xC8, 2, {0x03,0x30}},
{0xC9, 2, {0x03,0x51}},
{0xD1, 5, {0x00,0x05,0x03,0x00,0x00}},
{0xD2, 5, {0x00,0x05,0x09,0x00,0x00}},

{0xE5, 1, {0x02}},
{0xE6, 1, {0x02}},
{0xE7, 1, {0x02}},
{0xE9, 1, {0x02}},
{0xED, 1, {0x33}},
//reload function

//{0x11, 0, {}},// Normal Display
//{0x29, 0, {}},

	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
#else
{0xF0, 5, {0x55,0xaa,0x52,0x08,0x00}},
{0xee, 4, {0x87,0x78,0x02,0x40}},
#endif
//liuhanhui add 20150808---
#if 0
// page 1
	/*{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
	{0xCA, 1, {0x00}},
	{0xCE, 1, {0x34}},
	{0xBC, 2, {0x68,0x01}},
	{0xBD, 2, {0x68,0x01}},

// page 3 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x03}},
	{0xB0, 4, {0x00,0x23,0x00,0x00}},
	{0xB2, 7, {0x01,0x00,0x06,0x04,0x00,0xD8,0x42}},
	{0xB3, 7, {0x01,0x00,0x05,0x04,0x00,0xD8,0x42}},
	{0xBA, 7, {0x85,0x03,0x00,0x04,0x01,0xD8,0x42}},
	{0xBB, 7, {0x85,0x03,0x00,0x03,0x01,0xD8,0x42}},

// page 2 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x02}},
	{0xB0, 1, {0x40}},
	{0xD1, 16, {0x00,0x00,0x00,0x12,0x00,0x3C,0x00,0x55,0x00,0x78,0x00,0x9D,0x00,0xBE,0x00,0xF0}},
	{0xD2, 16, {0x01,0x18,0x01,0x59,0x01,0x8E,0x01,0xDD,0x02,0x1F,0x02,0x21,0x02,0x5D,0x02,0x9E}},
	{0xD3, 16, {0x02,0xC6,0x03,0x00,0x03,0x26,0x03,0x59,0x03,0x7A,0x03,0xA2,0x03,0xBB,0x03,0xD8}},
	{0xD4, 4, {0x03,0xF6,0x03,0xFF}},

// page 6 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x06}},
	{0xB3, 1, {0x03}},
	{0xB1, 5, {0x19,0x18,0x18,0x02,0x3A}},
	{0xB2, 2, {0x3A,0x3A}},
	{0xB6, 5, {0x13,0x12,0x12,0x00,0x3A}},*/
	
	/*{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
		//{0xCA, 1, {0x00}},
		{0xCE, 1, {0x04}},
		{0xBC, 2, {0x68,0x01}},
		{0xBD, 2, {0x68,0x01}},
	
	// page 3 
		{0xFF, 4, {0xAA,0x55,0xA5,0x80}},
		{0x6F, 1, {0x09}},
		{0xF7, 1, {0x82}},
		{0x6F, 1, {0x0B}},
		{0xF7, 1, {0xE0}},
		{0x55, 1, {0x81},
	
	// page 2 
		{0x53, 1, {0x2C}},
		{0x51, 1, {0xFF}},*/


	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
#endif
};

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
	
	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 12; //4//4
	params->dsi.vertical_frontporch					= 10;  //8
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 4;//16
	params->dsi.horizontal_backporch				= 40;//48
	params->dsi.horizontal_frontporch				= 40;//16
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 230; 
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
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

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
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

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
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	printk("[Kernel/LCM] lcm_resume() end\n");
#endif
}




LCM_DRIVER l696_mipi_wxga_qc_n070icegb2_lcm_drv=
{
    .name           	= "l696_mipi_wxga_qc_n070icegb2",
    .set_util_funcs 	= lcm_set_util_funcs,
    .get_params     	= lcm_get_params,
    .init           		= lcm_init, 
    .suspend        	= lcm_suspend,
    .resume         	= lcm_resume,
    //.compare_id     	= lcm_compare_id,
     .init_power		= lcm_init_power,
     .resume_power = lcm_resume_power,
     .suspend_power = lcm_suspend_power,
#if (LCM_DSI_CMD_MODE)
//    .update         = lcm_update,
#endif
   
};
