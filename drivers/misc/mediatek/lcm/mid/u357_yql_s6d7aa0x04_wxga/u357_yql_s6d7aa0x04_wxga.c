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
#define REGFLAG_DELAY       0XFE
#define REGFLAG_END_OF_TABLE    0xFF   // END OF REGISTERS MARKER


struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
	{0xF0,	2,	{0x5A, 0x5A}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xF1,	2,	{0x5A, 0x5A}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xFC,	2,	{0xA5, 0xA5}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xD0,	2,	{0x00, 0x10}},
	{REGFLAG_DELAY,	1,	{}},
	

	//{0xB1,	2,	{0x10, 0xA0}},
	{0xB1,	1,	{0x10}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xB2,	4,	{0x14, 0x22, 0x2F, 0x04}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xB5,	1,	{0x01}},
	{REGFLAG_DELAY,	1,	{}},
	//step 11
	{0xEE,	8,	{0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x1F, 0x00}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xEF,	13,	{0x56, 0x34, 0x43, 0x65, 0x90, 0x80, 0x24, 0x80, 0x00, 0x91, 0x11, 0x11, 0x11}},
	{REGFLAG_DELAY,	1,	{}},
	
	{0xF7,	32,	{0x04, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x16, 0x17, 0x10, 0x01,
                 0x01, 0x01, 0x01, 0x04, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x16,
                 0x17, 0x10, 0x01, 0x01, 0x01, 0x01}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xF2,	5,	{0x02, 0x08, 0x08, 0x40, 0x10}},
    {REGFLAG_DELAY,	1,	{}},
	
   	{0xF6,	6,	{0x60, 0x25, 0x26, 0x00, 0x00, 0x00}},
   	{REGFLAG_DELAY,	1,	{}},
	
    {0xFA,	17,	{0x04, 0x35, 0x07, 0x0B, 0x12, 0x0B, 0x10, 0x16, 0x1A, 0x24, 0x2C, 0x33, 0x3B,
                 0x3B, 0x33, 0x34, 0x33}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xFB,	17,	{0x04, 0x35, 0x07, 0x0B, 0x12, 0x0B, 0x10, 0x16, 0x1A, 0x24, 0x2C, 0x33, 0x3B,
                 0x3B, 0x33, 0x34, 0x33}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xF3,	10,	{0x01, 0xC4, 0xE0, 0x62, 0xD4, 0x83, 0x37, 0x3C, 0x24, 0x00}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xF4,	35,	{0x00, 0x02, 0x03, 0x26, 0x03, 0x02, 0x09, 0x00, 0x07, 0x16, 0x16, 0x03, 0x00,
                 0x08, 0x08, 0x03, 0x19, 0x1C, 0x12, 0x1C, 0x1D, 0x1E, 0x1A, 0x09, 0x01, 0x04, 0x02,
                 0x61, 0x74, 0x75, 0x72, 0x83, 0x80, 0x80, 0xF0}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xB0,	1,	{0x01}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xF5,	19,	{0x2F, 0x2F, 0x5F, 0xAB, 0x98, 0x52, 0x0F, 0x33, 0x43, 0x04, 0x59, 0x54, 0x52,
                 0x05, 0x40, 0x40, 0x5D, 0x59, 0x40}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xBC,	3,	{0x01, 0x4E, 0x08}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xE1,	5,	{0x03, 0x10, 0x1C, 0xA0, 0x10}},
    {REGFLAG_DELAY,	1,	{}},
	
    {0xFD,	5,	{0x16, 0x10, 0x11, 0x20, 0x09}},
    {REGFLAG_DELAY,	1,	{}},

	
    {0x35,	1,	{0x00}},
    {REGFLAG_DELAY,	1,	{}},
/*	
    {0xC3,	3,	{0x40, 0x00, 0x28}},
    {REGFLAG_DELAY,	20,	{}},
*/
   
    {0x11,	1,	{0}},
    {REGFLAG_DELAY,	30,	{}},

	
    {0xC3,	3,	{0x40, 0x00, 0x28}},
    {REGFLAG_DELAY,	150,	{}},

    {0x29,	1,	{0x00}},
    {REGFLAG_DELAY,	200,	{}},
	
	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
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
//		params->physical_width  = PHYSICAL_WIDTH;
//		params->physical_height = PHYSICAL_HEIGHT;
		
    #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
    #else
		params->dsi.mode   = BURST_VDO_MODE;//BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	   params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;

#if 1
  	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
  	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
#endif		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //params->dsi.word_count=FRAME_WIDTH*3;
		#if 1
		params->dsi.vertical_sync_active				=4;// 2;
		params->dsi.vertical_backporch					=4;// 19;
		params->dsi.vertical_frontporch					=8;// 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				=4;// 15;
		params->dsi.horizontal_backporch				=140;// 165;
		params->dsi.horizontal_frontporch				=16;// 90;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		#endif		
		
		params->dsi.PLL_CLOCK 	=227;//210;// 220;//245 by eric.wang
		
}
 
                                                                                                       
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
data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00A5A5FC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001000D0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x10B11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x2F2214B2;
data_array[2] = 0x00000004;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x01B51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00093902;
data_array[1] = 0x1F0000EE;
data_array[2] = 0x1F000000;
data_array[3] = 0x00000000;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x000e3902;
data_array[1] = 0x433456EF;
data_array[2] = 0x24809065;
data_array[3] = 0x11910080;
data_array[4] = 0x00001111;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x00213902;
data_array[1] = 0x090804F7;
data_array[2] = 0x0D0C0B0A;
data_array[3] = 0x17160F0E;
data_array[4] = 0x01010110;
data_array[5] = 0x09080401;
data_array[6] = 0x0D0C0B0A;
data_array[7] = 0x17160F0E;
data_array[8] = 0x01010110;
data_array[9] = 0x00000001;
dsi_set_cmdq(data_array, 10, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x080802F2;
data_array[2] = 0x00001040;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x262560F6;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00123902;
data_array[1] = 0x073504FA;
data_array[2] = 0x100B120B;
data_array[3] = 0x2C241A16;
data_array[4] = 0x333B3B33;
data_array[5] = 0x00003334;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00123902;
data_array[1] = 0x073504FB;
data_array[2] = 0x100B120B;
data_array[3] = 0x2C241A16;
data_array[4] = 0x333B3B33;
data_array[5] = 0x00003334;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x000b3902;
data_array[1] = 0xE0C401F3;
data_array[2] = 0x3783D462;
data_array[3] = 0x0000243C;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x00243902;
data_array[1] = 0x030200F4;
data_array[2] = 0x09020326;
data_array[3] = 0x16160700;
data_array[4] = 0x08080003;
data_array[5] = 0x121C1903;
data_array[6] = 0x1A1E1D1C;
data_array[7] = 0x02040109;
data_array[8] = 0x72757461;
data_array[9] = 0xF0808083;
dsi_set_cmdq(data_array, 10, 1);

data_array[0] = 0x01B01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x5F2F2FF5;
data_array[2] = 0x0F5298AB;
data_array[3] = 0x59044333;
data_array[4] = 0x40055254;
data_array[5] = 0x40595D40;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x084E01BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x1C1003E1;
data_array[2] = 0x000010A0;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x111016FD;
data_array[2] = 0x00000920;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00351500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00043902;
data_array[1] = 0x280040C3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);

}
#endif
static void lcm_init(void)
{

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	MDELAY(5);	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);//Must > 5ms
	//lcd_bl_en(1);
	
	//lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1); 
//	MDELAY(100);
}



static void lcm_suspend(void)
{
#if 1 //nt35521
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(200);
    lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
    lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
lcm_init();
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


LCM_DRIVER u357_yql_s6d7aa0x04_wxga_lcm_drv = 
{
  .name				= "u357_yql_s6d7aa0x04_wxga",
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
