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

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000008;
dsi_set_cmdq(data_array, 3, 1);

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
data_array[1]= 0x004f4fb3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x001010b4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000505b5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x003535b9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x002525ba;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000068bc;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00033902;
data_array[1]= 0x000068bd;
dsi_set_cmdq(data_array, 2, 1);

data_array[0]= 0x00023902;
data_array[1]= 0x00000cc0;
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
data_array[1]= 0x170008b2;
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

data_array[0]= 0x00063902;
data_array[1]= 0x52aa55f0;
data_array[2]= 0x00000108;
dsi_set_cmdq(data_array, 3, 1);
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
		
		params->dsi.vertical_sync_active				= 4;//1// by eric.wang
		params->dsi.vertical_backporch					= 8;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 30;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 18;//10
		params->dsi.horizontal_backporch				= 18;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 18;//60 by eric.wang 160

		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		params->dsi.PLL_CLOCK 	= 200;//170 by eric.wang
		
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




LCM_DRIVER lf710_sl_nt35521_cpt_wxga_ips_101_lcm_drv = 
{
  .name				= "lf710_sl_nt35521_cpt_wxga_ips_101",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
