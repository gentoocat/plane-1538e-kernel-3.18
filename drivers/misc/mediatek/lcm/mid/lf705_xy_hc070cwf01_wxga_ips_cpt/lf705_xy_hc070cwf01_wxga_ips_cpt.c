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

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x018412FF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x008412FF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x92001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000230ff;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xD2001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03B01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x91001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x9AB01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000a3902;
data_array[1] = 0x006400c0;
data_array[2] = 0x64001515;
data_array[3] = 0x00001515;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x005C00C0;
data_array[2] = 0x00040001;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x81001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66C11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA6001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00010FB3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x92001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00C41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x061005C4;
data_array[2] = 0x10150502;
data_array[3] = 0x02071005;
data_array[4] = 0x00101505;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xB0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000022C4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x91001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x004246C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00C7C7D8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x006565D9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x87001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x008018C4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB3001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x84C51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xBB001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x8aC51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x82001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0aC41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC6001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03B01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC2001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40F51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC3001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x85F51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40D01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000D1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x021102F5;
data_array[2] = 0x00000015;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x50C51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x94001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x636666C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB2001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB4001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB6001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB8001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x94001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xD2001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001506F5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0xB4001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xCCC51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB4001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xCCC51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x151515CB;
data_array[2] = 0x15151515;
data_array[3] = 0x15001515;
data_array[4] = 0x00000015;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xD0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x001515CB;
data_array[2] = 0x00151515;
data_array[3] = 0x15151515;
data_array[4] = 0x15151515;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xE0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x150015CB;
data_array[2] = 0x00000015;
data_array[3] = 0x15001515;
data_array[4] = 0x00001515;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xF0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000c3902;
data_array[1] = 0xffffffCB;
data_array[2] = 0xffffffff;
data_array[3] = 0xffffffff;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x120c14CC;
data_array[2] = 0x10160e0a;
data_array[3] = 0x2f000218;
data_array[4] = 0x0000002f;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x002a29CC;
data_array[2] = 0x00062e2d;
data_array[3] = 0x09110b13;
data_array[4] = 0x170f150d;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xA0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x2f0001CC;
data_array[2] = 0x0000002f;
data_array[3] = 0x2d002a29;
data_array[4] = 0x0000052e;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xB0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x0f150dCC;
data_array[2] = 0x110b1317;
data_array[3] = 0x2f000509;
data_array[4] = 0x0000002f;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xC0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00103902;
data_array[1] = 0x002a29CC;
data_array[2] = 0x00012d2e;
data_array[3] = 0x1810160e;
data_array[4] = 0x0a120c14;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xD0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x2f0006CC;
data_array[2] = 0x0000002f;
data_array[3] = 0x2e002a29;
data_array[4] = 0x0000022d;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000d3902;
data_array[1] = 0x1A0D8FCE;
data_array[2] = 0x001A0D8E;
data_array[3] = 0x00000000;
data_array[4] = 0x00000000;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x1A0075CE;
data_array[2] = 0x001A0175;
data_array[3] = 0x00000000;
data_array[4] = 0x00000000;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xA0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840778CE;
data_array[2] = 0x001A90F0;
data_array[3] = 0xF1840678;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xB0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840578CE;
data_array[2] = 0x001A90F2;
data_array[3] = 0xF3840478;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xC0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840378CE;
data_array[2] = 0x001A90F4;
data_array[3] = 0xF5840278;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xD0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840178CE;
data_array[2] = 0x001A90F6;
data_array[3] = 0xF7840078;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840070CF;
data_array[2] = 0x001A90F8;
data_array[3] = 0xF9840170;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840270CF;
data_array[2] = 0x001A90FA;
data_array[3] = 0xFB840370;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xA0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840470CF;
data_array[2] = 0x001A90FC;
data_array[3] = 0xFD840570;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xB0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000f3902;
data_array[1] = 0x840670CF;
data_array[2] = 0x001A90FE;
data_array[3] = 0xFF840770;
data_array[4] = 0x00001A91;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0xC0001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x000c3902;
data_array[1] = 0x640101CF;
data_array[2] = 0x01000064;
data_array[3] = 0x00000000;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0xB5001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00073902;
data_array[1] = 0xffff3fC5;
data_array[2] = 0x00ffff3f;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00153902;
data_array[1] = 0x291900E1;
data_array[2] = 0x5D5C4C3A;
data_array[3] = 0x67987D8B;
data_array[4] = 0x3E406353;
data_array[5] = 0x0B1A2632;
data_array[6] = 0x00000000;
dsi_set_cmdq(data_array, 7, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00153902;
data_array[1] = 0x291900E2;
data_array[2] = 0x5D5C4C3A;
data_array[3] = 0x67987D8B;
data_array[4] = 0x3E406353;
data_array[5] = 0x0B1A2632;
data_array[6] = 0x00000000;
dsi_set_cmdq(data_array, 7, 1);

data_array[0] = 0xA4001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xF0C11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x92001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00C41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB4001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xCCC51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x90001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB6B61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x92001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06B31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC2001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40F51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x80001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00FFFFFF;
dsi_set_cmdq(data_array, 2, 1);

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

		//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
        params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 2;//1// by eric.wang
		params->dsi.vertical_backporch					= 16;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 16;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 4;//10
		params->dsi.horizontal_backporch				= 80;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 80;//60 by eric.wang 160

		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		params->dsi.PLL_CLOCK 	= 180;//170 by eric.wang
		
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




LCM_DRIVER lf705_xy_hc070cwf01_wxga_ips_cpt_lcm_drv = 
{
  .name				= "lf705_xy_hc070cwf01_wxga_ips_cpt",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
