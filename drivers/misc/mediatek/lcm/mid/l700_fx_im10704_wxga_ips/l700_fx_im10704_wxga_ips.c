#ifndef BUILD_LK
#include <linux/string.h>
#else
#include <string.h>
#endif
#include "lcm_drv.h"


#ifdef BUILD_LK
//#include <platform/gpio_const.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#elif (defined BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
	#include "upmu_common.h"
    #include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
//#include "ddp_hal.h"
#endif

#include "lcm_drv.h"






static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define MDELAY(n) 											(lcm_util.mdelay(n))

#define GPIO_LCD_RST_EN       (GPIO146 | 0x80000000)
#define GPIO_LCM_PWREN         (GPIO120 | 0x80000000)
#define GPIO_LCM_EN            (GPIO64 | 0x80000000)
#define GPIO_LCD_TE_EN         0xFFFFFFFF
#define GPIO_LCD_BL_EN			GPIO_LCM_PWREN
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
 
{0xFF, 4, {0xAA,0x55,0xA5,0x80}},
{0x6F, 2, {0x07,0x00}},
{0xF7, 2, {0x50,0x00}},
{0x6F, 1, {0x06}},
{0xF7, 1, {0xA0}},
{0x6F, 1, {0x19}},
{0xF7, 1, {0x12}},




{0xF0, 5, {0x55,0xAA,0x52,0x08,0x00}},
{0xC8, 1, {0x80}},
{0xB1, 2, {0x6C,0x01}},
{0xB6, 1, {0x08}},
{0x6F, 1, {0x02}},
{0xB8, 1, {0x08}},
{0xBB, 2, {0x54,0x54}},
{0xBC, 2, {0x05,0x05}},
{0xC7, 1, {0x01}},
{0xBD, 5, {0x02,0xB0,0x0C,0x0A,0x00}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
{0xB0, 2, {0x05,0x05}},
{0xB1, 2, {0x05,0x05}},
{0xBC, 2, {0x8E,0x00}},
{0xBD, 2, {0x92,0x00}},
{0xCA, 1, {0x00}},
{0xC0, 1, {0x04}},
{0xBE, 1, {0x80}},
{0xB3, 2, {0x19,0x19}},
{0xB4, 2, {0x12,0x12}},
{0xB9, 2, {0x24,0x24}},
{0xBA, 2, {0x14,0x14}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x02}},
{0xEE, 1, {0x02}},
{0xEF, 4, {0x09,0x06,0x15,0x18}},
{0xB0, 6, {0x00,0x00,0x00,0x11,0x00,0x27}},
{0x6F, 1, {0x06}},
{0xB0, 6, {0x00,0x36,0x00,0x45,0x00,0x5F}},
{0x6F, 1, {0x0C}},
{0xB0, 4, {0x00,0x74,0x00,0xA5}},
{0xB1, 6, {0x00,0xCF,0x01,0x13,0x01,0x47}},
{0x6F, 1, {0x06}},
{0xB1, 6, {0x01,0x9B,0x01,0xDF,0x01,0xE1}},
{0x6F, 1, {0x0C}},
{0xB1, 4, {0x02,0x23,0x02,0x6C}},
{0xB2, 6, {0x02,0x9A,0x02,0xD7,0x03,0x05}},
{0x6F, 1, {0x06}},
{0xB2, 6, {0x03,0x42,0x03,0x68,0x03,0x91}},
{0x6F, 1, {0x0C}},
{0xB2, 4, {0x03,0xA5,0x03,0xBD}},
{0xB3, 4, {0x03,0xD7,0x03,0xFF}},
{0xBC, 6, {0x00,0x00,0x00,0x11,0x00,0x27}},
{0x6F, 1, {0x06}},
{0xBC, 6, {0x00,0x38,0x00,0x47,0x00,0x61}},
{0x6F, 1, {0x0C}},
{0xBC, 4, {0x00,0x78,0x00,0xAB}},
{0xBD, 6, {0x00,0xD7,0x01,0x1B,0x01,0x4F}},
{0x6F, 1, {0x06}},
{0xBD, 6, {0x01,0xA1,0x01,0xE5,0x01,0xE7}},
{0x6F, 1, {0x0C}},
{0xBD, 4, {0x02,0x27,0x02,0x70}},
{0xBE, 6, {0x02,0x9E,0x02,0xDB,0x03,0x07}},
{0x6F, 1, {0x06}},
{0xBE, 6, {0x03,0x44,0x03,0x6A,0x03,0x93}},
{0x6F, 1, {0x0C}},
{0xBE, 4, {0x03,0xA5,0x03,0xBD}},
{0xBF, 4, {0x03,0xD7,0x03,0xFF}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x06}},
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
{0xBD, 2, {0x31,0x09}},
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
{0xE6, 2, {0x31,0x31}},
{0xD8, 5, {0x00,0x00,0x00,0x00,0x00}},
{0xD9, 5, {0x00,0x00,0x00,0x00,0x00}},
{0xE7, 1, {0x00}},
{0xF0, 5, {0x55,0xAA,0x52,0x08,0x03}},
{0xB0, 2, {0x20,0x00}},
{0xB1, 2, {0x20,0x00}},
{0xB2, 5, {0x05,0x00,0x42,0x00,0x00}},
{0xB6, 5, {0x05,0x00,0x42,0x00,0x00}},
{0xBA, 5, {0x53,0x00,0x42,0x00,0x00}},
{0xBB, 5, {0x53,0x00,0x42,0x00,0x00}},
{0xC4, 1, {0x40}},
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
{0x11, 1, {0x00}},
{REGFLAG_DELAY,120,{}},
{0x29, 0, {0x00}},
{REGFLAG_DELAY,5,{}},



{REGFLAG_END_OF_TABLE, 0x00, {}}
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
	params->dsi.LANE_NUM					= LCM_THREE_LANE;	//LCM_TWO_LANE;	//LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	params->dsi.packet_size=256;
	//video mode timing
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

       params->dsi.word_count=FRAME_WIDTH*3;

	
	params->dsi.vertical_sync_active				= 11;
	params->dsi.vertical_backporch					= 55; //4//4
	params->dsi.vertical_frontporch					= 22;  //8
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 16;//16
	params->dsi.horizontal_backporch				= 16;//48
	params->dsi.horizontal_frontporch				= 32;//16
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	params->dsi.PLL_CLOCK = 295; 
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

static void lcm_init_power(void)
{	
	lcm_set_gpio_output(GPIO_LCM_PWREN, GPIO_OUT_ONE);
	MDELAY(15);
	lcm_set_gpio_output(GPIO_LCM_EN, GPIO_OUT_ONE);
	MDELAY(20);
		
}

static void lcm_suspend_power(void)
{
	lcm_set_gpio_output(GPIO_LCM_PWREN, GPIO_OUT_ZERO);
	MDELAY(20);	
	lcm_set_gpio_output(GPIO_LCM_EN, GPIO_OUT_ZERO);
	MDELAY(20);	
}

static void lcm_resume_power(void)
{
	lcm_init_power();
}

static void lcm_init(void)
{
	lcm_set_gpio_output(GPIO_LCD_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(100);
	
	// when phone initial , config output high, enable backlight drv chip  
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
}

static void lcm_suspend(void)
{
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  
	
	lcm_set_gpio_output(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}
#if 0
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
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);	
}

#endif

LCM_DRIVER l700_fx_im10704_wxga_ips_lcm_drv=
{
    .name           	= "l700_fx_im10704_wxga_ips",
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
    .update         = lcm_update,
#endif
   
};

