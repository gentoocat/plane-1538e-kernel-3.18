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
#define FRAME_WIDTH  (600)
#define FRAME_HEIGHT (1024)

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
	  params->dsi.LANE_NUM				= LCM_THREE_LANE;//LCM_TWO_LANE;
		//params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		#if 0
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
		
		params->dsi.vertical_sync_active				= 1;//1// by eric.wang
		params->dsi.vertical_backporch					= 25;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 35;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 1;//16
	params->dsi.horizontal_backporch				= 60;//48
	params->dsi.horizontal_frontporch				= 80;//16

		//params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		// Bit rate calculation
		// Every lane speed
		//params->dsi.pll_div1=0;				// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0;				// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
		//params->dsi.cont_clock 	= 1;//1
		//params->dsi.ssc_disable = 0;//0
		params->dsi.PLL_CLOCK 	= 250;//170 by eric.wang
	//params->dsi.clk_lp_per_line_enable   = 1;//0
		
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

/*	data_array[0]= 0x10b21500;  //2 channel
	dsi_set_cmdq(data_array, 1, 1);*/
/*	
	data_array[0]= 0x58801500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0]= 0x47811500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0xd4821500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x88831500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0xA9841500;
	dsi_set_cmdq(data_array, 1, 1);	

	data_array[0]= 0xC3851500;	//0x5a871500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x82861500;	//0x5a871500;
	dsi_set_cmdq(data_array, 1, 1);
*/
/*
//Page0
	data_array[0]= 0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
//--- PASSWORD  ----//
	data_array[0]= 0x93E11500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x65E21500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xF8E31500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x03801500;
	dsi_set_cmdq(data_array, 1, 1);
//Lane select by internal reg
	data_array[0]= 0x04E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x032D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
//--- Sequence Ctrl  ----//
	data_array[0]= 0x10701500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x13711500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x06721500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x03751500;
	dsi_set_cmdq(data_array, 1, 1);
//--- Page1  ----//
	data_array[0]= 0x01E01500;
	dsi_set_cmdq(data_array, 1, 1);
//Set VCOM
	data_array[0]= 0x00001500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xA0011500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00031500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xA0041500;
	dsi_set_cmdq(data_array, 1, 1);
//Set Gamma Power, VGMP,VGMN,VGSP,VGSN
	data_array[0]= 0x070A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x740C1500;
	dsi_set_cmdq(data_array, 1, 1);
//Set Gamma Power, VGMP,VGMN,VGSP,VGSN
	data_array[0]= 0x00171500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xD7181500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x01191500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x001A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xD71B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x011C1500;
	dsi_set_cmdq(data_array, 1, 1);
//Set Gate Power
	data_array[0]= 0x741F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x19201500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x19211500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0E221500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x43271500;
	dsi_set_cmdq(data_array, 1, 1);
//SET RGBCYC
	data_array[0]= 0x09371500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x04381500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08391500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x183A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x183B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x723C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xFF3E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xFF3E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xFF3F1500;
	dsi_set_cmdq(data_array, 1, 1);
//Set TCON
	data_array[0]= 0x04401500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0xA0411500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08431500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x07441500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x30451500;
	dsi_set_cmdq(data_array, 1, 1);
//--- power voltage  ----//
	data_array[0]= 0x01551500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x01561500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x65571500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0A581500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0A591500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x285A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0F5B1500;
	dsi_set_cmdq(data_array, 1, 1);
//--- Gamma  ----//
	data_array[0]= 0x7C5D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x5F5E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x4D5F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3F601500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x39611500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x29621500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x2B631500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x12641500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x28651500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x24661500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x22671500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3E681500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x2C691500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x336A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x266B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x236C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x186D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x096E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x006F1500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0]= 0x7C701500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x5F711500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x4D721500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3F731500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x39741500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x29751500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x2B761500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x12771500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x28781500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x24791500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x227A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3E7B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x2C7C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x337D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x267E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x237F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x18801500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x09811500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00821500;
	dsi_set_cmdq(data_array, 1, 1);
//Page2, for GIP
	data_array[0]= 0x02E01500;
	dsi_set_cmdq(data_array, 1, 1);
//GIP_L Pin mapping
	data_array[0]= 0x37001500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x17011500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0A021500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x06031500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08041500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x04051500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00061500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F071500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F081500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F091500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F0F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3F101500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F111500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F121500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1E131500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x10141500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F151500;
	dsi_set_cmdq(data_array, 1, 1);
//GIP_R Pin mapping
	data_array[0]= 0x37161500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x17171500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0B181500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x07191500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x091A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x051B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x011C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F1D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F1E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F1F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F201500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F211500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F221500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F231500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F241500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F251500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F261500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F271500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F281500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1E291500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x112A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F2B1500;
	dsi_set_cmdq(data_array, 1, 1);
//GIP_L_GS Pin mapping
	data_array[0]= 0x372C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x172D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x052E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x092F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x07301500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0B311500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x11321500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F331500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F341500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F351500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F361500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F371500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F381500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F391500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F3A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F3B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x3F3C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F3D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F3E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1E3F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x01401500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F411500;
	dsi_set_cmdq(data_array, 1, 1);
//GIP_R_GS Pin mapping
	data_array[0]= 0x38421500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x18431500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x04441500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08451500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x06461500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0A471500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x10481500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F491500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F4F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F501500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F511500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F521500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F531500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1E541500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F551500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00561500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x1F571500;
	dsi_set_cmdq(data_array, 1, 1);
//GIP Timing
	data_array[0]= 0x10581500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00591500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x005A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x105B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x015C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x505D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x015E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x025F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x30601500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x01611500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x02621500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x06631500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x6A641500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x55651500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08661500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x73671500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x05681500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x08691500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x6E6A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x006B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x006C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x006D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x006E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x886F1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00701500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00711500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x06721500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x7B731500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00741500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x80751500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00761500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x0D771500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x18781500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x00791500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x007A1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x007B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x007C1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x037D1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x7B7E1500;
	dsi_set_cmdq(data_array, 1, 1);
//Page4
	data_array[0]= 0x04E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x10091500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x01041500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x380E1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x2B2B1500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x442E1500;
	dsi_set_cmdq(data_array, 1, 1);
//Page0
	data_array[0]= 0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x02E61500;
	dsi_set_cmdq(data_array, 1, 1);
	data_array[0]= 0x02E61500;
	dsi_set_cmdq(data_array, 1, 1);
//SLP OUT
	data_array[0] = 0x00111500;       
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
//DISP ON
	data_array[0] = 0x00291500;               
	dsi_set_cmdq(data_array, 1, 1);
    MDELAY(5);
//--- TE----//
	data_array[0] = 0x00351500;               
	dsi_set_cmdq(data_array, 1, 1);
*/

data_array[0] = 0x5AB01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03B11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xE2141500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00111500;
dsi_set_cmdq(data_array, 1, 1);

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


LCM_DRIVER lf705_pxjt_px695ia18610423a_lcm_drv = 
{
  .name				= "lf705_pxjt_px695ia18610423a",
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
