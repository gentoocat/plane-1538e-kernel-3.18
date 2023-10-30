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


struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
#if 1
static struct LCM_setting_table lcm_initialization_setting[] =
{
{0xFF,3,{0x61,0x36,0x08}},
	{0x1C,1,{0xA0}},
	{0xFF,3,{0x61,0x36,0x01}},
	{0x53,1,{0xA6}},
	
	{0xFF,3,{0x61,0x36,0x08}},
	{0x4C,1,{0x00}},	
	{0x6C,1,{0x04}},
	{0x93,1,{0x08}},	
	
	{0xFF,3,{0x61,0x36,0x07}},
	{0x1A,1,{0x05}},	
	{0x16,1,{0x1F}},
	{0x0D,1,{0x05}},	
	{0x0A,1,{0x03}},
	{0x0E,1,{0x35}},
	{0x0B,1,{0x1F}},	

	{0xFF,3,{0x61,0x36,0x01}},
	{0xA0,1,{0x0C}},
	{0xA1,1,{0x0F}},
	{0xA2,1,{0x15}},
	{0xA3,1,{0x1C}},
	{0xA4,1,{0x0D}},  
	{0xA5,1,{0x1B}},
	{0xA6,1,{0x20}},					   
	{0xA7,1,{0x22}},				   
	{0xA8,1,{0x2A}},				   
	{0xA9,1,{0x31}},
	{0xAA,1,{0x36}},				   
	{0xAB,1,{0x3D}},				   
	{0xAC,1,{0x3D}},
	{0xAD,1,{0x32}},	
	{0xAE,1,{0x33}},
	{0xAF,1,{0x32}},
	{0xB0,1,{0x35}},


	{0xFF,3,{0x61,0x36,0x01}},
	{0xC0,1,{0x0C}},
	{0xC1,1,{0x0F}},
	{0xC2,1,{0x15}},
	{0xC3,1,{0x1C}},
	{0xC4,1,{0x1D}},  
	{0xC5,1,{0x1B}},
	{0xC6,1,{0x20}},					   
	{0xC7,1,{0x22}},				   
	{0xC8,1,{0x2A}},				   
	{0xC9,1,{0x31}},
	{0xCA,1,{0x36}},				   
	{0xCB,1,{0x3D}},				   
	{0xCC,1,{0x3D}},
	{0xCD,1,{0x32}},	
	{0xCE,1,{0x33}},
	{0xCF,1,{0x32}},
	{0xD0,1,{0x35}},	

	{0xFF,3,{0x61,0x36,0x08}},
	{0xAB,1,{0x24}},
	{0x40,1,{0x00}},
	{0xE9,1,{0x0B}},

	{0xFF,3,{0x61,0x36,0x01}},
	{0x36,1,{0x14}},

	{0xFF,3,{0x61,0x36,0x06}},
	{0x72,1,{0x01}},

	{0xFF,3,{0x61,0x36,0x00}},
	{0x11,0,{}},
	//{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
{REGFLAG_DELAY, 150, {}},
	{0x29,0,{}},  
{REGFLAG_DELAY, 20, {}},
{REGFLAG_END_OF_TABLE, 0x00, {}}	
/*	
{0xff,  3 ,{0x61,0x36,0x08}},
{0x1c,  1 ,{0xa0}},

{0xff,  3 ,{0x61,0x36,0x01}},
{0x53,  1 ,{0xa6}},

{0xff,  3 ,{0x61,0x36,0x08}},
{0x4c,  1 ,{0x00}},
{0x6c,  1 ,{0x04}},
{0x93,  1 ,{0x08}},

{0xff,  3 ,{0x61,0x36,0x07}},
{0x1a,  1 ,{0x05}},
{0x16,  1 ,{0x1f}},
{0x0d,  1 ,{0x05}},
{0x0a,  1 ,{0x03}},
{0x0e,  1 ,{0x35}},
{0x0b,  1 ,{0x1f}},

{0xff,  3 ,{0x61,0x36,0x01}},
{0xa0,  1 ,{0x0c}},
{0xa1,  1 ,{0x0f}},
{0xa2,  1 ,{0x15}},
{0xa3,  1 ,{0x1c}},
{0xa4,  1 ,{0x0d}},
{0xa5,  1 ,{0x1b}},
{0xa6,  1 ,{0x20}},
{0xa7,  1 ,{0x22}},
{0xa8,  1 ,{0x2a}},
{0xa9,  1 ,{0x31}},
{0xaa,  1 ,{0x36}},
{0xab,  1 ,{0x3d}},
{0xac,  1 ,{0x3d}},
{0xad,  1 ,{0x32}},
{0xae,  1 ,{0x33}},
{0xaf,  1 ,{0x32}},
{0xb0,  1 ,{0x35}},

{0xff,  3 ,{0x61,0x36,0x01}},
{0xc0,  1 ,{0x0c}},
{0xc1,  1 ,{0x0f}},
{0xc2,  1 ,{0x15}},
{0xc3,  1 ,{0x1c}},
{0xc4,  1 ,{0x1d}},
{0xc5,  1 ,{0x1b}},
{0xc6,  1 ,{0x20}},
{0xc7,  1 ,{0x22}},
{0xc8,  1 ,{0x2a}},
{0xc9,  1 ,{0x31}},
{0xca,  1 ,{0x36}},
{0xcb,  1 ,{0x3d}},
{0xcc,  1 ,{0x3d}},
{0xcd,  1 ,{0x32}},
{0xce,  1 ,{0x33}},
{0xcf,  1 ,{0x32}},
{0xd0,  1 ,{0x35}},


{0xff,  3 ,{0x61,0x36,0x08}},
{0xab,  1 ,{0x24}},
{0x40,  1 ,{0x00}},
{0xe9,  1 ,{0x0b}},


{0xff,  3 ,{0x61,0x36,0x01}},
{0x36,  1 ,{0x14}},

{0xff,  3 ,{0x61,0x36,0x06}},
{0x72,  1 ,{0x01}},

{0xff,  3 ,{0x61,0x36,0x00}},
{0x11,  1 ,{0x00}},

{REGFLAG_DELAY, REGFLAG_DELAY, {150}},
{0x29,  1 ,{0x00}},
//end ------------ */
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
		params->dsi.mode   = BURST_VDO_MODE;//BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	   params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;
	  //params->dsi.LANE_NUM				= LCM_THREE_LANE;
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
		//params->dsi.packet_size=256;

		// Video mode setting		
	//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
   // params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //params->dsi.word_count=FRAME_WIDTH*3;
		#if 1
		params->dsi.vertical_sync_active				=5;// 2;
		params->dsi.vertical_backporch					=3;// 19;
		params->dsi.vertical_frontporch					=8;// 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				=5;// 15;
		params->dsi.horizontal_backporch				=59;// 165;
		params->dsi.horizontal_frontporch				=16;// 90;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		#endif
		//mipiclk=480m;hbp=59;hfp=16;HSW=5;VBP=3;VFP=8;VSW=5;
		#if 0
		params->dsi.vertical_sync_active				= 5;//4// by eric.wang
		params->dsi.vertical_backporch					= 12;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 12;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10;//4
		params->dsi.horizontal_backporch				= 59;//132 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 32;//24 by eric.wang 160
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		#endif
		
		//params->dsi.horizontal_blanking_pixel = 60;

		
		// Bit rate calculation
		// Every lane speed
		//params->dsi.pll_div1=0;				// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0;				// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
		params->dsi.cont_clock 	= 0;//0
		params->dsi.ssc_disable = 1;//0
		params->dsi.PLL_CLOCK 	=220;//210;// 220;//245 by eric.wang
		params->dsi.clk_lp_per_line_enable   = 1; //1
		
}
 
//unsigned int temp[14] ={0x000030be,0x000031be,0x000032be,0x000033be,0x000034be,0x000035be,0x000036be,0x000037be,0x000038be,0x000039be,0x00003abe,0x00003bbe,0x00003cbe,0x00003dbe}; 
//static int index=0;                                                                                                        
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
#if 0
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
data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000008;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000001fb;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000168b1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000008b6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00043902;
data_array[1]= 0x080201b8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x004444bb;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000000bc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x106802bd;
data_array[2]= 0x00000010;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000080c8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000108;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002929b3;		//0x004f4fb3   0511
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001010b4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000505b5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003536b9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002525ba;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000168bc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000168bd;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000030be;
dsi_set_cmdq(data_array, 2, 1);


data_array[0]= 0x00023902;
data_array[1]= 0x000000ca;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000208;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000001ee;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00113902;
data_array[1]= 0x000000b0;
data_array[2]= 0x002a000f;
data_array[3]= 0x00540040;
data_array[4]= 0x00930076;
data_array[5]= 0x000000c5;
dsi_set_cmdq(data_array, 6, 1);

data_array[0]= 0x00113902;
data_array[1]= 0x01f000b1;
data_array[2]= 0x01660132;
data_array[3]= 0x02ff01bb;
data_array[4]= 0x02420201;
data_array[5]= 0x00000085;
dsi_set_cmdq(data_array, 6, 1);

data_array[0]= 0x00113902;
data_array[1]= 0x02af02b2;
data_array[2]= 0x030503e0;
data_array[3]= 0x03540335;
data_array[4]= 0x03a00384;
data_array[5]= 0x000000c4;
dsi_set_cmdq(data_array, 6, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x03f203b3;
data_array[2]= 0x000000ff;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000308;
dsi_set_cmdq(data_array, 3, 1);					

data_array[0]= 0x00033902;
data_array[1]= 0x000000b0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000000b1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x170005b2;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000005b6;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0xa00053ba;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00063902;
data_array[1]= 0xa00053bb;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x000000c0;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00053902;
data_array[1]= 0x000000c1;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000060c4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000c0c5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000508;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b0;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b1;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b2;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b3;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b4;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00033902;
data_array[1]= 0x000617b5;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x00000cb8;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000000b9;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000000ba;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x00000abb;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000002bc;
dsi_set_cmdq(data_array, 2, 1);	

data_array[0]= 0x00063902;
data_array[1]= 0x010103bd;
data_array[2]= 0x00000303;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000007c0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x0000a2c4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002003c8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002101c9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00043902;
data_array[1]= 0x010000cc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00043902;
data_array[1]= 0x010000cd;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0xfc0400d1;
data_array[2]= 0x00001407;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00063902;
data_array[1]= 0x000510d2;
data_array[2]= 0x00001603;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000006e5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006e6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006e7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006e8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006e9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000006ea;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x000030ed;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000608;
dsi_set_cmdq(data_array, 3, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001117b0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001016b1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001812b2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001913b3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003100b4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003431b5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002934b6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00332ab7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002d2eb8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003408b9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000834ba;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002e2dbb;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002a34bc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003429bd;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003134be;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000031bf;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001319c0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001218c1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001610c2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001711c3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003434e5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001812c4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001913c5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001117c6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001016c7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003108c8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003431c9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002934ca;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x00332acb;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002e2dcc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003400cd;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000034ce;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002d2ecf;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002a34d0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003429d1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003134d2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000831d3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001610d4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001711d5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001319d6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001218d7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003434e6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00063902;
data_array[1]= 0x000000d8;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00063902;
data_array[1]= 0x000000d9;
data_array[2]= 0x00000000;
dsi_set_cmdq(data_array, 3, 1);	

data_array[0]= 0x00023902;
data_array[1]= 0x000000e7;
dsi_set_cmdq(data_array, 2, 1);

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
#endif
static void lcm_init(void)
{
#if 1 //nt35521
#ifdef BUILD_LK
    printf("[IND][K] Y___0%s\n", __func__);    
#if 1 //def BUILD_LK
	//lcd_bl_en(0);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	//lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	MDELAY(90);
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);

	MDELAY(30);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);
//	MDELAY(30);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);//Must > 5ms
	//lcd_bl_en(1);
	
	//lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1); 
	MDELAY(100);
	//avdd_enable(1);
	//MDELAY(180);
	//lcd_bl_en(1);	
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
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
	//lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	//lcm_initial_registers();	
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
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

//	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	//avdd_enable(0);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);
	//MDELAY(90);
	
//	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	
	//MDELAY(30);
//	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);//Must > 5ms
	//lcd_bl_en(1);
	
	//lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1); 
	MDELAY(100);
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
	// push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
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


LCM_DRIVER u357_fy_ili6136s_wxga_lcm_drv = 
{
  .name				= "u357_fy_ili6136s_wxga",
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
