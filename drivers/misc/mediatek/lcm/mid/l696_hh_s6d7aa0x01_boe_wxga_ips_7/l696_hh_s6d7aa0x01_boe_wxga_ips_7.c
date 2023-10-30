#ifdef BUILD_LK
	#include <platform/disp_drv_platform.h>
	#include <platform/mt_gpio.h>
	#include <platform/mt_i2c.h> 
	#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <linux/string.h>
	#include "upmu_common.h"
	#include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local IOdefine
// ---------------------------------------------------------------------------
//#define 原理图控制引脚网络名 CPU实际引脚
#define GPIO_LCD_3V3_ENB    	(GPIO64  | 0x80000000)
#define GPIO_LVDS_POWER_EN      (GPIO120 | 0x80000000)
#define LCD_RSTN     			(GPIO146 | 0x80000000)

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  										(800)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             							0xFC
#define REGFLAG_END_OF_TABLE      							0xFD
#define PUSH_TABLE											1
#define LCM_DSI_CMD_MODE 									0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};
#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 											(lcm_util.udelay(n))
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


static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
static void lcd_vdd(unsigned char enabled)
{
    if (enabled)
    {
        lcm_set_gpio_output(GPIO_LCD_3V3_ENB, 1);
    }
    else
    {
        lcm_set_gpio_output(GPIO_LCD_3V3_ENB, 0);
    }
}
static void lcd_avdd(unsigned char enabled) //mipi接口虽然不使用avdd，也统一用该函数开背光
{
    if (enabled)
    {
        lcm_set_gpio_output(GPIO_LVDS_POWER_EN, 1);
    }
    else
    {
        lcm_set_gpio_output(GPIO_LVDS_POWER_EN, 0);
    }
}
static void lcd_reset(unsigned char enabled)//lvds接口没发现外接复位引脚，也统一使用该函数
{
    if (enabled)
    {
        lcm_set_gpio_output(LCD_RSTN, 1);
    }
    else
    {	
        lcm_set_gpio_output(LCD_RSTN, 0); 	
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Platform for mt6735m
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
		params->dsi.vertical_backporch					= 8;//修改此前4
		params->dsi.vertical_frontporch					= 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 4;//修改此前16
		params->dsi.horizontal_backporch				= 64;//修改此前48
		params->dsi.horizontal_frontporch				= 16;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 230; 

	params->dsi.clk_lp_per_line_enable   = 1;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x53;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;
}

// ---------------------------------------------------------------------------
//  LCM Driver InitCode
// ---------------------------------------------------------------------------
#if PUSH_TABLE
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
static struct LCM_setting_table lcm_initialization_setting[] = {
	{0x01,1,{0x00}}, 
	{REGFLAG_DELAY, 10, {}}, 
    {0xF0,2,{0x5A,0x5A}},
	{0xF1,2,{0x5A,0x5A}},
	{0xFC,2,{0xA5,0xA5}},  		
	{0xD0,2,{0x00,0x10}},

	{0xB1,1,{0x10}}, 		
	{0xB2,4,{0x14,0x22,0x2F,0x04}},
	{0xF2,5,{0x02,0x08,0x08,0x40,0x10}}, 
	{0xB0,1,{0x04}}, 
	{0xFD,1,{0x09}}, 
	{0xF3,10,{0x01,0xD7,0xE2,0x62,0xF4,0xF7,0x77,0x3C,0x26,0x00}},		
	//{0xBC,3,{0x00,0x4E,0x0A}}, 
	
    //{0xEE,8,{0x00,0x00,0x15,0x00,0x00,0x00,0x15,0x00}},  		
	//{0xEF,17,{0x12,0x12,0x43,0x43,0x90,0x84,0x24,0x81,0x00,0xA1,
     //          0x21,0x03,0x03,0x40,0x80,0x82,0x00,}},
	//{0xE1,5,{0x03,0x10,0x1C,0xA0,0x10}},  		
	    		
   		

	{0xF4,45,{0x00,0x02,0x03,0x26,0x03,0x02,0x09,0x00,0x07,0x16,
	          0x16,0x03,0x00,0x08,0x08,0x03,0x0E,0x0F,0x12,0x1C,
	          0x1D,0x1E,0x0C,0x09,0x01,0x04,0x02,0x61,0x74,0x75,
	          0x72,0x83,0x80,0x80,0xB0,0x00,0x01,0x01,0x28,0x04,
	          0x03,0x28,0x01,0xD1,0x32}},   	
	{0xF5,26,{0x87,0x28,0x28,0x5F,0xAB,0x98,0x52,0x0F,0x33,0x43,
	          0x04,0x59,0x54,0x52,0x05,0x40,0x60,0x4E,0x60,0x40,
	          0x27,0x26,0x52,0x25,0x6D, 0x18}},	
	{0xEE,8,{0x25,0x00,0x25,0x00,0x25,0x00,0x25,0x00}},  		
	{0xEF,8,{0x12,0x12,0x43,0x43,0xA0,0x04,0x24,0x81}},          
   // 
	{0xF7,32,{0x0A,0x0A,0x08,0x08,0x0B,0x0B,0x09,0x09,0x04,0x05,
			 0x01,0x01,0x01,0x01,0x01,0x01,0x0A,0x0A,0x08,0x08,
			 0x0B,0x0B,0x09,0x09,0x04,0x05,0x01,0x01,0x01,0x01,
			 0x01,0x01}},	
			 
	{0xBC,3,{0x01,0x4E,0x08}}, 	
	{0xE1,5,{0x03,0x10,0x1C,0xA0,0x10}},
	{0xF6,6,{0x60,0x21,0xA6,0x00,0x00,0x00}},
	{0xFE,6,{0x00,0x0D,0x03,0x21,0x00,0x08}},
	{0xB0,1,{0x22}}, 
	//{0xFD,5,{0x16,0x10,0x11,0x23,0x09}},
		 	
		
	{0xFA,17,{0x0E,0x33,0x0E,0x14,0x0A,0x0F,0x12,0x13,0x14,0x1D,
		     0x20,0x1F,0x1F,0x1F,0x1C,0x18,0x2B}},	
	{0xB0,1,{0x22}}, 	     
	{0xFB,17,{0x0E,0x33,0x0E,0x14,0x0A,0x0F,0x12,0x13,0x14,0x1C,
		     0x1F,0x1F,0x1F,0x1F,0x1B,0x18,0x2B}},		

	{0xB0,1,{0x11}}, 
	
	{0xFA,17,{0x20,0x33,0x21,0x24,0x16,0x19,0x1B,0x1A,0x19,0x1E,
		     0x22,0x21,0x21,0x21,0x1C,0x19,0x22}},	
	{0xB0,1,{0x11}}, 	     
	{0xFB,17,{0x20,0x33,0x21,0x24,0x16,0x19,0x1B,0x1A,0x19,0x20,
		     0x20,0x20,0x1F,0x1F,0x1E,0x19,0x22}},	
		     
	{0xFA,17,{0x1D,0x33,0x1F,0x22,0x14,0x17,0x19,0x18,0x18,0x1E,
		     0x20,0x22,0x21,0x22,0x23,0x24,0x2C}},	
	     
	{0xFB,17,{0x1D,0x33,0x1F,0x22,0x14,0x17,0x19,0x18,0x18,0x1F,
		     0x22,0x20,0x22,0x24,0x22,0x24,0x2D}},	
		     
	{0x35,1,{0x00}},

	{0x11, 1,{0x00}},
	{REGFLAG_DELAY, 100, {}},	
	{0xC3,3,{0x40,0x00,0x28}},
	{0x29, 1,{0x00}},
	{REGFLAG_DELAY, 150, {}},    
	
	// Setting ending by predefined flag
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

#else
static void init_lcm_registers(void)
{
    unsigned int data_array[16];
	// data_array[0] = 0xC2BD1500;
	// dsi_set_cmdq(&data_array, 1, 1);
	// data_array[0] = 0x01BE1500;
	// dsi_set_cmdq(&data_array, 1, 1);
	// MDELAY(200);	
}
#endif

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_init(void)
{
	lcd_vdd(1);
	MDELAY(100);	
	
	lcd_reset(1);
	MDELAY(10);
	lcd_reset(0);
	MDELAY(10);	
	lcd_reset(1);
	MDELAY(150);//Must > 80ms
#if PUSH_TABLE
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
#else
	init_lcm_registers();
#endif
	lcd_avdd(1);
	MDELAY(180);
}
static void lcm_suspend(void)
{
	unsigned int data_array[16];

    data_array[0]=0x00280500;//displayoff
    dsi_set_cmdq(data_array,1,1);
	MDELAY(50);
	data_array[0]=0x00100500;//displayoff
    dsi_set_cmdq(data_array,1,1);

	lcd_avdd(0);
    MDELAY(150);
    lcd_reset(0);
    MDELAY(20);
    lcd_vdd(0);
    MDELAY(20);
}
static void lcm_resume(void)
{
	lcm_init();
}
LCM_DRIVER l696_hh_s6d7aa0x01_boe_wxga_ips_7_lcm_drv=
{
	.name           	= "l696_hh_s6d7aa0x01_boe_wxga_ips_7",
	.set_util_funcs 	= lcm_set_util_funcs,
	.get_params     	= lcm_get_params,
	.init           	= lcm_init, 
	.suspend        	= lcm_suspend,
	.resume         	= lcm_resume,
};
