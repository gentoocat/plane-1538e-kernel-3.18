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

#if 0
struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
#if 0 //by eric.wangu127
 //   {0x25,1,{0x55}},
 //   {0x36,1,{0x01}},
    //{0x25,1,{0x00}},
    
 //   {0xB1,1,{0x30}},
    //{0xB2,1,{0x10}}, //2 channel
    {0x80,1,{0x58}},
    {0x81,1,{0x47}},
    {0x82,1,{0xd4}},
    {0x83,1,{0x88}},
    {0x84,1,{0xa9}},
    {0x85,1,{0xc3}},
    {0x86,1,{0x82}},
    //{0x25,1,{0x00}},
    
    //{0x11,1,{0x00}},
#endif
#if 1
	{0xFF,4,{0xAA,0x55,0xA5,0x80}},
	{0x6F,2,{0x11,0x00}},
	{0xF7,2,{0x20,0x00}},
	{0x6F,1,{0x06}},
	{0xF7,1,{0xA0}},
	{0x6F,1,{0x19}},
	{0xF7,1,{0x12}},
	{0xF4,1,{0x03}},
	// new Vcom floating 
	{0x6F,1,{0x08}},
	{0xFA,1,{0x40}},
	{0x6F,1,{0x11}},
	{0xF3,1,{0x01}},
	// for abnormal power off
	{0x6F,1,{0x06}},
	{0xFC,1,{0x03}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
	{0xB1,2,{0x68,0x01}},
	{0xB6,1,{0x08 }},
	{0x6F,1,{0x02 }},
	{0xB8,1,{0x08 }},
	{0xBB,2,{0x54,0x44}},
	{0xBC,2,{0x05,0x05}},
	{0xC7,1,{0x01}},
	{0xBD,5,{0x02,0xB0,0x1E,0x1E,0x00}},
	{0xC5,2,{0x01,0x07}},
	{0xC8,1,{0x80 }},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},
	{0xB0,2,{0x05,0x05}},
	{0xB1,2,{0x05,0x05}},
	//new 
	{0xB2,2,{0x00,0x00}},
	{0xBC,2,{0x90,0x01}},
	{0xBD,2,{0x90,0x01}},
	{0xCA,1,{0x00}},
	{0xC0,1,{0x04}},
	{0xBE,1,{0x29}},
	{0xB3,2,{0x28,0x28}},
	{0xB4,1,{0x12,0x12}},
	{0xB9,1,{0x35,0x35}},
	{0xBA,1,{0x25,0x25}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x02 }},
	{0xEE,1,{0x01}},
	{0xEF,4,{0x09,0x06,0x15,0x18}},
	{0xB0,6,{0x00,0x00,0x00,0x24,0x00,0x55}},
	{0x6F,1,{0x06}},
	{0xB0,6,{0x00,0x77,0x00,0x94,0x00,0xC0}},
	{0x6F,1,{0x0C}},
	{0xB0,4,{0x00,0xE3,0x01,0x1A}},                                                                                                                                         
	{0xB1,6,{0x01,0x46,0x01,0x88,0x01,0xBC}},
	{0x6F,1,{0x06}},
	{0xB1,6,{0x02,0x0B,0x02,0x4B,0x02,0x4D}},
	{0x6F,1,{0x0C}},
	{0xB1,4,{0x02,0x88,0x02,0xC9}},
	{0xB2,6,{0x02,0xF3,0x03,0x29,0x03,0x4E}},
	{0x6F,1,{0x06}},
	{0xB2,6,{0x03,0x7D,0x03,0x9B,0x03,0xBE}},
	{0x6F,1,{0x0C}},
	{0xB2,4,{0x03,0xD3,0x03,0xE9}},
	{0xB3,4,{0x03,0xFB,0x03,0xFF}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x06}},
	{0xB0,2,{0x0B,0x2E}},
	{0xB1,2,{0x2E,0x2E}},
	{0xB2,2,{0x2E,0x09}},
	{0xB3,2,{0x2A,0x29}},
	{0xB4,2,{0x1B,0x19}},
	{0xB5,2,{0x17,0x15}},
	{0xB6,2,{0x13,0x11}},
	{0xB7,2,{0x01,0x2E}},
	{0xB8,2,{0x2E,0x2E}},
	{0xB9,2,{0x2E,0x2E}},
	{0xBA,2,{0x2E,0x2E}},
	{0xBB,2,{0x2E,0x2E}},
	{0xBC,2,{0x2E,0x00}},
	{0xBD,2,{0x10,0x12}},
	{0xBE,2,{0x14,0x16}},
	{0xBF,2,{0x18,0x1A}},
	{0xC0,2,{0x29,0x2A}},
	{0xC1,2,{0x08,0x2E}},
	{0xC2,2,{0x2E,0x2E}},
	{0xC3,2,{0x2E,0x0A}},
	{0xE5,2,{0x2E,0x2E}},
	{0xC4,2,{0x0A,0x2E}},
	{0xC5,2,{0x2E,0x2E}},
	{0xC6,2,{0x2E,0x00}},
	{0xC7,2,{0x2A,0x29}},
	{0xC8,2,{0x10,0x12}},
	{0xC9,2,{0x14,0x16}},
	{0xCA,2,{0x18,0x1A}},
	{0xCB,2,{0x08,0x2E}},
	{0xCC,2,{0x2E,0x2E}},
	{0xCD,2,{0x2E,0x2E}},
	{0xCE,2,{0x2E,0x2E}},
	{0xCF,2,{0x2E,0x2E}},
	{0xD0,2,{0x2E,0x09}},
	{0xD1,2,{0x1B,0x19}},
	{0xD2,2,{0x17,0x15}},
	{0xD3,2,{0x13,0x11}},
	{0xD4,2,{0x29,0x2A}},
	{0xD5,2,{0x01,0x2E}},
	{0xD6,2,{0x2E,0x2E}},
	{0xD7,2,{0x2E,0x0B}},
	{0xE6,2,{0x2E,0x2E}},
	{0xD8,5,{0x00,0x00,0x00,0x00,0x00}},
	{0xD9,5,{0x00,0x00,0x00,0x00,0x00}},
	{0xE7,1,{0x00}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x03}},
	{0xB0,2,{0x20,0x00}},
	{0xB1,2,{0x20,0x00}},
	{0xB2,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xB6,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xB7,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xBA,5,{0x57,0x00,0x00,0x00,0x00}},
	{0xBB,5,{0x57,0x00,0x00,0x00,0x00}},
	{0xC0,4,{0x00,0x00,0x00,0x00}},
	{0xC1,4,{0x00,0x00,0x00,0x00}},
	{0xC4,1,{0x60}},
	{0xC5,1,{0x40}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x05}},
	{0xBD,5,{0x03,0x01,0x03,0x03,0x03}},
	{0xB0,2,{0x17,0x06}},
	{0xB1,2,{0x17,0x06}},
	{0xB2,2,{0x17,0x06}},
	{0xB3,2,{0x17,0x06}},
	{0xB4,2,{0x17,0x06}},
	{0xB5,2,{0x17,0x06}},
	{0xB8,1,{0x00}},
	{0xB9,1,{0x00}},
	{0xBA,1,{0x00}},
	{0xBB,1,{0x02}},
	{0xBC,1,{0x00}},
	{0xC0,1,{0x07}},
	{0xC4,1,{0x80}},
	{0xC5,1,{0xA4}},
	{0xC8,2,{0x05,0x30}},
	{0xC9,2,{0x01,0x31}},
	{0xCC,3,{0x00,0x00,0x3C}},
	{0xCD,3,{0x00,0x00,0x3C}},
	{0xD1,5,{0x00,0x05,0x09,0x07,0x10}},
	{0xD2,5,{0x00,0x05,0x0E,0x07,0x10}},
	{0xE5,1,{0x06}},
	{0xE6,1,{0x06}},
	{0xE7,1,{0x06}},
	{0xE8,1,{0x06}},
	{0xE9,1,{0x06}},
	{0xEA,1,{0x06}},
	{0xED,1,{0x30}},
	{0x6F,1,{0x11}},
	{0xF3,1,{0x01}},
	{0x35,1,{0x00}},
	{0x11,1,{0x00}},
	{0x29,1,{0x00}},
#endif

    {REGFLAG_DELAY,150,{}},
    //{0x29,1,{0x00}},
    //{REGFLAG_DELAY,70,{}},
    {REGFLAG_END_OF_TABLE, 0x00, {}},
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }   
    }   

}

#endif
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
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	  params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;
	  //params->dsi.LANE_NUM				= LCM_THREE_LANE;
		//params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
//	//	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
//	//	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
//		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
//		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		//params->dsi.packet_size=256;

		// Video mode setting		
	//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
//	    params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //   params->dsi.word_count=FRAME_WIDTH*3;
		#if 1
		params->dsi.vertical_sync_active				= 5;//4// by eric.wang
		params->dsi.vertical_backporch					= 3;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 5;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 5;//4
		params->dsi.horizontal_backporch				= 59;//132 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 16;//24 by eric.wang 160
		#endif
		#if 0
		params->dsi.vertical_sync_active				= 5;//4// by eric.wang
		params->dsi.vertical_backporch					= 12;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 12;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;//4
		params->dsi.horizontal_backporch				= 59;//132 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 32;//24 by eric.wang 160
		#endif
		
		//params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		// Bit rate calculation
		// Every lane speed
		//params->dsi.pll_div1=0;				// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0;				// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
	//	params->dsi.cont_clock 	= 1;//0
		params->dsi.ssc_disable = 0;//0
		params->dsi.PLL_CLOCK 	= 220;//
	//	params->dsi.clk_lp_per_line_enable   = 1; //1
		
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
    
#ifdef BUILD_LK
    //printf("[IND][K] y_____0%s\n", __func__);
#else
    //printk("[IND][K] y_____1%s\n", __func__);
#endif

/*

	data_array[0] = 0x10b21500;
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(1);	
*/	

	//=======================================
data_array[0]= 0x00053902;
data_array[1]= 0xA555AAff;
data_array[2]= 0x00000080;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x0000116F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000020F7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000066F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000A0F7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000196F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000012F7;
dsi_set_cmdq(data_array, 2, 1);

/*data_array[0]= 0x00023902;
data_array[1]= 0x000003F4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000086F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000040FA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000116F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000001F3;
dsi_set_cmdq(data_array, 2, 1);*/

data_array[0]= 0x00023902;
data_array[1]= 0x0000066F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000003FC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55f0;
data_array[2]= 0x00000008;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000168B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000008B6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000026F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000008B8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x004454BB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000505BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000001C7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x1EB002BD;
data_array[2]= 0x0000001E;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000701C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000080C8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55F0;
data_array[2]= 0x00000108;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000505B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000505B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000000B2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000190BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000190BD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000CA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000004C0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000029BE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002828B3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001212B4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003535B9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002525BA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55F0;
data_array[2]= 0x00000208;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000001EE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x150609EF;
data_array[2]= 0x00000018;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x000000B0;
data_array[2]= 0x00550024;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000066F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x007700B0;
data_array[2]= 0x00C00094;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x00000C6F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x01E300B0;
data_array[2]= 0x0000001A;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x014601B1;
data_array[2]= 0x00BC0188;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000066F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x020B02B1;
data_array[2]= 0x004D024B;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x00000C6F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x028802B1;
data_array[2]= 0x000000C9;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x03F302B2;
data_array[2]= 0x004E0329;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000066F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00073902;
data_array[1]= 0x037D03B2;
data_array[2]= 0x00BE039B;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x00000C6F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x03D303B2;
data_array[2]= 0x000000E9;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x03FB03B3;
data_array[2]= 0x000000FF;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55F0;
data_array[2]= 0x00000608;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E0BB0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EB1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00092EB2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00292AB3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00191BB4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001517B5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001113B6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E01B7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EB8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EB9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EBA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EBB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00002EBC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001210BD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001614BE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001A18BF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002A29C0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E08C1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EC2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000A2EC3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EE5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E0AC4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EC5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00002EC6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00292AC7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001210C8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001614C9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001A18CA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E08CB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2ECC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2ECD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2ECE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2ECF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00092ED0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00191BD1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001517D2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001113D3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002A29D4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E01D5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2ED6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000B2ED7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002E2EE6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000000D8;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000000D9;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000E7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55F0;
data_array[2]= 0x00000308;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000020B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000020B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000005B2;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000005B6;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000005B7;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000057BA;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000057BB;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x000000C0;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x000000C1;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000060C4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000040C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52AA55F0;
data_array[2]= 0x00000508;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x030103BD;
data_array[2]= 0x00000303;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000617B5;//
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000B8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000B9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000BA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000002BB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000000BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000007C0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000080C4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000A4C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003005C8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003101C9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00043902;///3
data_array[1]= 0x3C0000CC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00043902;///3
data_array[1]= 0x3C0000CD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x090500D1;
data_array[2]= 0x00001007;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x0E0500D2;
data_array[2]= 0x00001007;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006E5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006E6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006E7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006E8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006E9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006EA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000030ED;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000116F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000001F3;
dsi_set_cmdq(data_array, 2, 1);

/*data_array[0]= 0x00023902;
data_array[1]= 0x00000035;
dsi_set_cmdq(data_array, 2, 1);*/


data_array[0] = 0x00110500;        //exit sleep mode 
dsi_set_cmdq(data_array, 1, 1); 
MDELAY(200); 
  
data_array[0] = 0x00290500;        //exit sleep mode 
dsi_set_cmdq(data_array, 1, 1); 
MDELAY(20);

data_array[0]= 0x00023902;
data_array[1]= 0x00000035;
dsi_set_cmdq(data_array, 2, 1);

MDELAY(20);



}

static void lcm_init(void)
{
#if 1 //nt35521
#ifdef BUILD_LK
    printf("[IND][K] Y___0%s\n", __func__);    
#if 1 //def BUILD_LK
	//lcd_bl_en(0);
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
	MDELAY(400);
	//avdd_enable(1);
	//MDELAY(180);
	//lcd_bl_en(1);	
#endif  
#else

    //printk("[IND][K] Y___1%s\n", __func__);
   // return;
#endif
#endif

#if 0 //ek79007
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init() enter\n");

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	//MDELAY(50);
		lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);

	//lcm standy
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);
	


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
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	lcm_initial_registers();	
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	MDELAY(100);

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_init() end\n");

	
#endif
#endif	
}



static void lcm_suspend(void)
{
#if 1 //nt35521
//unsigned int data_array[16];

#ifdef BUILD_LK
    //printf("[IND][K] y__________0%s\n", __func__);
#else
    //printk("[IND][LK] y______1%s\n", __func__);
#ifdef BUILD_UBOOT
	// do nothing in uboot 

       return;
#endif 


 //   data_array[0] = 0x00280500;  //display off                        
 //   dsi_set_cmdq(data_array, 1, 1);

 //   data_array[0] = 0x00100500;
 //   dsi_set_cmdq(data_array,1,1);//enter into sleep
 //   MDELAY(5);
	
  //  lcd_bl_en(0);
	//avdd_enable(0);
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);
	MDELAY(200);
    lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
    lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
   // DSI_clk_HS_mode(0);
    MDELAY(10);
#endif
#endif

#if 0 //ek79007
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");

	//lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	//MDELAY(100);
	
	//VGP1 1.8V
	//upmu_set_rg_vgp1_vosel(0x0);
	//upmu_set_rg_vgp1_en(0x0);
	//MDELAY(50);	



	//lcm standy
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	printf("[LK/LCM] lcm_suspend() end\n");
#else
	printk("[Kernel/LCM] lcm_suspend() enter\n");
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ZERO);	
	//lcm_set_gpio_output(GPIO_DISP_PWM, GPIO_OUT_ZERO);//eric.wang

	MDELAY(10);
	

	//lcm standy
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST,GPIO_OUT_ZERO);
	//MDELAY(100);
	printk("[Kernel/LCM] lcm_suspend() end\n");
#endif
#endif
}

static void lcm_resume(void)
{
#if 1 //nt35521
	#ifdef BUILD_LK
    //printf("[IND][K] y_______0%s\n", __func__);
#else
    printk("[IND][LK] y_______1%s\n", __func__);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	
	//avdd_enable(0);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);
	//MDELAY(90);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	
	//MDELAY(30);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(130);//Must > 5ms
	//lcd_bl_en(1);
	
	lcm_initial_registers();
	MDELAY(400);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
#endif
#endif

#if 0 //ek79007
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
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	printf("[LK/LCM] lcm_resume() end\n");
#else
	printk("[Kernel/LCM] lcm_resume() enter\n");


	//hwPowerOn(MT6323_POWER_LDO_VGP3, VOL_1800, "LCM");//by eric.wang
	
	
	//lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	//MDELAY(20);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
	//lcm standy
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	lcm_initial_registers();	
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	MDELAY(100);


	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_resume() end\n");
	//MDELAY(10);
#endif
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


LCM_DRIVER u101_m101ni01_tii40_a01_wxga_lcm_drv = 
{
  .name				= "u101_m101ni01_tii40_a01_wxga",
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
