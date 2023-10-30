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
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER



// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static void init_lcm_registers(void)
{
		unsigned int data_array[16];
data_array[0] = 0x00043902;
data_array[1] = 0x038198ff;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00021500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x55031500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x13041500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00051500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06061500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01071500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00081500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x010a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x180f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x18101500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00111500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00121500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00131500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00141500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00151500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00161500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x441e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x801f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00231500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00241500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00251500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00261500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00271500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x33281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x002f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00301500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00311500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00321500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00331500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04341500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00351500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00361500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00371500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01381500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00391500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00401500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00421500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00431500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00441500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01501500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23511500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45521500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x67531500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x89541500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xab551500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01561500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23571500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x67591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x895a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xab5b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xcd5c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xef5d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x115e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x145f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x15601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0f611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0d621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0e631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0c641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x14751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x15761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0f771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0d781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0e791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0c7a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x087b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x027c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x027d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x027e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x027f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02811500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02821500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02831500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02841500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02851500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02861500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00871500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01881500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06891500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x028a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x048198ff;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00000000;
data_array[0] = 0x156c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x6b6e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x776f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xa43a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x158d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xba871500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x76261500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xd1b21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x018198ff;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x0a221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x72531500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x77551500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xa6501500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xa6511500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00311500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x20601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08a01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1aa11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2aa21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x14a31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x17a41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2ba51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1da61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x20a71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x9da81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1ca91500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29aa1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x8fab1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x20ac1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1fad1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4fae1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x23af1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29b01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x56b11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66b21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x39b31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08c01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1ac11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2ac21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x15c31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x17c41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2bc51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1dc61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x20c71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x9dc81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1dc91500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29ca1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x8fcb1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x20cc1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1fcd1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4fce1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24cf1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29d01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x56d11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66d21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x39d31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x008198ff;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);
	
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
	
	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 3; //4//4
	params->dsi.vertical_frontporch					= 5;  //8
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 10;//16
	params->dsi.horizontal_backporch				= 48;//48
	params->dsi.horizontal_frontporch				= 16;//16
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 210; 

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
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(20);
}

static void lcm_resume_power(void)
{
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
}

static void lcm_init(void)
{
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(100);  // tongjun@mid
	
	init_lcm_registers();
	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
}

static void lcm_suspend(void)
{
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	init_lcm_registers();
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}




LCM_DRIVER l696_mipi_xy_ili9881c_wxga_ips_cpt_lcm_drv=
{
    .name           	= "l696_mipi_xy_ili9881c_wxga_ips_cpt",
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
