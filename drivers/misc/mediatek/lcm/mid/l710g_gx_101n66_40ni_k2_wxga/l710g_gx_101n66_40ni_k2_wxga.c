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
// #define REGFLAG_DELAY             								0xFC
// #define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER
#define REGFLAG_DELAY       0XFFE
#define REGFLAG_END_OF_TABLE    0xFFF   // END OF REGISTERS MARKER

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
		
		params->dsi.vertical_sync_active				= 10;//4// by eric.wang
		params->dsi.vertical_backporch					= 18;//11 by eric.wang 23
		params->dsi.vertical_frontporch					= 18;//17 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 4;//8
		params->dsi.horizontal_backporch				= 8;//8 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 12;//32 by eric.wang 160
	//	params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		

		params->dsi.PLL_CLOCK 	= 205;//160;//170 by eric.wang
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

/*static void lcm_initial_registers(void)
{
	unsigned int data_array[16];

data_array[0] = 0x00E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x93E11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x65E21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xF8E31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2B2B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x032D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x442E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
data_array[0] = 0x6A011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC0181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC01B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x011C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x6B1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0E221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x28351500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x59371500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05381500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04391500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x083A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x083B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7C3C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06401500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA0411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08431500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0B441500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x88451500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0F551500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01561500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xAD571500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x285A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1E5B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x5C5D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x405E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x305F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x21611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x13621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1A631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x25661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x27671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x37691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3F6A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x326B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2E6C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x226D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x116E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x5C701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x30721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x21741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x13751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1A761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x25791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x277A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x477B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x377C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3F7D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x327E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2E7F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x22801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x11811500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00821500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44001500;
data_array[0] = 0x44011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45021500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45031500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x46041500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x46051500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47061500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47071500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D081500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D0A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D0B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D0C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D0D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D0E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x570F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x57101500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x58111500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x58121500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40131500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x55141500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x55151500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44161500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x461A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x461B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x471C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x471D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D1E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D231500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1D241500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x57251500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x57261500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x58271500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x58281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x552A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x552B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A5C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x105D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x015E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x025F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0B631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x54641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x31671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0B681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1E691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x546A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x046B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x046D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x886F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7B731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xF8751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0D771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x14781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x037D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7B7E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02E61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06E71500;
dsi_set_cmdq(data_array, 1, 1);



data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);
data_array[0] = 0x00351500;
dsi_set_cmdq(data_array, 1, 1);

}*/

struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
   //3 LANE
    {0xFF,4,{0xAA,0x55,0x25,0x01}},
    {0x6F,1,{0x16}},
    {0xF7,1,{0x10}},
    {0xFF,4,{0xAA,0x55,0x25,0x00}},
    
    
    {0xFF,4,{0xAA,0x55,0xA5,0x80}},
    
    {0x6F,2,{0x11,0x00}},
    {0xF7,2,{0x20,0x00}},	
    
    {0x6F,1,{0x06}},	
    {0xF7,1,{0xA0}},	
    {0x6F,1,{0x19}},	
    {0xF7,1,{0x12}},	
//{0x6F,1,{0x12}},
    // new Vcom floating 	
//{0xFA,1,{0x40}},
//{0x6F,1,{0x11}},
//{0xF3,1,0x01}},
{0x6F,1,{0x06}},
{0xFC,1,{0x03}},
    //========== page0 relative ==========
    {0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},	
    {0xB1,2,{0x68,0x01}},	
    {0xB6,1,{0x08}},	
    {0x6F,1,{0x02}},	
    {0xB8,1,{0x08}},	
    {0xBB,2,{0x54,0x44}},	
		{0xBC,2,{0x05,0x05}},   
    {0xC7,1,{0x01}},	
    {0xBD,5,{0x02,0xB0,0x1E,0x1E,0x00}},
    {0xC5,2,{0x01,0x07}},	
    {0xC8,1,{0x80}},	
    //========== page1 relative ==========
{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},
    {0xB0,2,{0x05,0x05}},
    {0xB1,2,{0x05,0x05}},	
    {0xB2,2,{0x00,0x00}},	
    {0xBC,2,{0x90,0x01}},	
    {0xBD,2,{0x90,0x01}},	
    {0xCA,1,{0x00}},	
    {0xC0,1,{0x04}},	
    {0xBE,1,{0x29}},	
    {0xB3,2,{0x28,0x28}},	
    {0xB4,2,{0x12,0x12}},	
    {0xB9,2,{0x35,0x35}},	
    {0xBA,2,{0x25,0x25}},	
    
    //========== page2 relative ==========//gamma setting
    {0xF0,5,{0x55,0xAA,0x52,0x08,0x02}},	
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
    
    //========== page6 GOA relative ==========
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
    
    //========== page3 relative ==========
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
    
    //========== page5 relative ==========
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
    
    //BIST mode
    //{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
    //{0xEF,2,{0x07,0xFF}},
    //{0xEE,4,{0x87,0x78,0x02,0x40}},
    
    //==========================================================================//	
    {0x11,	1,	{0x00}},
    {REGFLAG_DELAY, 120, {}},
    
    {0x29,	1,	{0x00}},
    {REGFLAG_DELAY, 20, {}},
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
	
	// lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printf("[LK/LCM] lcm_init() end\n");
#else
	printk("[Kernel/LCM] lcm_init() enter\n");

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	// lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	MDELAY(100);

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_init() end\n");

	
#endif
}



static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");

	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	printf("[LK/LCM] lcm_suspend() end\n");
#else
	printk("[Kernel/LCM] lcm_suspend() enter\n");
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ZERO);	

	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ZERO);

	MDELAY(10);

	lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST,GPIO_OUT_ZERO);
	printk("[Kernel/LCM] lcm_suspend() end\n");
#endif
}

static void lcm_resume(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume() enter\n");

	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	printf("[LK/LCM] lcm_resume() end\n");
#else
	printk("[Kernel/LCM] lcm_resume() enter\n");

	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
	//lcm standy
	lcm_set_gpio_output(GPIO_LCM_STB, GPIO_OUT_ONE);

	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(20);

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(100);
	// lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	MDELAY(100);
	
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);	
	printk("[Kernel/LCM] lcm_resume() end\n");

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


LCM_DRIVER l710g_gx_101n66_40ni_k2_wxga_lcm_drv = 
{
  .name				= "l710g_gx_101n66_40ni_k2_wxga",
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
