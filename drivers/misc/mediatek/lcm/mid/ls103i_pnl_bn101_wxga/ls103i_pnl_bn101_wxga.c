//BSW 9366//GB2
#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#endif

#ifdef BUILD_LK
#include <platform/gpio_const.h>
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#include <string.h>
#else
#include <mach/upmu_hw.h>
#include <mach/gpio_const.h>
#include <mt-plat/mt_gpio.h>
#endif
#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------


#define FRAME_WIDTH (800)
#define FRAME_HEIGHT (1280)


#define GPIO_LCD_RST_EN       (GPIO146 | 0x80000000)
#define GPIO_LCM_PWREN         (GPIO19|0x80000000)
#define GPIO_LCM_EN            0xFFFFFFFF  //(GPIO11|0x80000000)
#define GPIO_LCD_TE_EN         0xFFFFFFFF
#define GPIO_LCD_BL_EN			GPIO_LCM_PWREN


//#define GPIO_LCD_BL_EN       GPIO57

#define LCM_DSI_CMD_MODE									0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER


#define     MT6350_VGP1_EN        "VGP3"
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    



//*****************************************************************************//
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
	
//update initial param for IC nt35523 0.01

static struct LCM_setting_table lcm_initialization_setting[] = {
	{0x00,	1,	{0x00}},  //page0
	{0xff,	3,	{0x12,0x87,0x01}},
    {0x00,  1,	{0x80}},
	{0xff,	2,	{0x12,0x87}},
	{0x00,  1,	{0x92}},
	{0xff,	2,	{0x20,0x02}},

	{0x11,	0,	{0x00}},
	{REGFLAG_DELAY,100,{}},
	{0x29,	0,	{0x00}},
	{REGFLAG_DELAY,200,{}},
	{0x35,	0,	{0x00}},

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
//*****************************************************************************//

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

static void lcd_reset(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCD_RST_EN, enabled);
}

static void lcd_power_en(unsigned char enabled)
{
    lcm_set_gpio_output(GPIO_LCM_EN, enabled);
}

static void lcd_vdd_enable(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCM_PWREN, enabled);
}


static void stbyb_control(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCD_TE_EN, enabled);
}

static void lcd_bl_en(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCD_BL_EN, enabled);
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

	params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED; //LCM_DBI_TE_MODE_VSYNC_OR_HSYNC;//LCM_DBI_TE_MODE_DISABLED;//LCM_DBI_TE_MODE_VSYNC_ONLY;
	params->dbi.te_edge_polarity		= LCM_POLARITY_FALLING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif
	
	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_THREE_LANE; //LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting		
	params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
	params->dsi.word_count=FRAME_WIDTH*3; 

	

	params->dsi.vertical_sync_active 				= 4; 
    params->dsi.vertical_backporch					= 11;
    params->dsi.vertical_frontporch 				= 17;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active				= 20;
    params->dsi.horizontal_backporch				= 20;
    params->dsi.horizontal_frontporch				= 32;
	params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 285;//256;	// 256  251 


}

static void lcm_init(void)
{
	lcd_power_en(1);

	MDELAY(50);
	lcd_vdd_enable(1);
	MDELAY(30);
	
	stbyb_control(1);
	MDELAY(50);
	
	lcd_reset(1);
	MDELAY(50);
	lcd_reset(0);
	MDELAY(50);
	lcd_reset(1);
	
	MDELAY(80);//Must > 5ms

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	MDELAY(100);

	lcd_bl_en(1);
}


static void lcm_suspend(void)
{
	unsigned int data_array[16];

	lcd_bl_en(0);

	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(30); 

	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120); 

	lcd_reset(0);	
	MDELAY(10);	

	lcd_power_en(0);
	MDELAY(10);
	
	stbyb_control(0);
	MDELAY(10);
	
	lcd_vdd_enable(0);
	MDELAY(80);

}


static void lcm_resume(void)
{
    lcm_init();
}

LCM_DRIVER ls103i_pnl_bn101_wxga_lcm_drv = 
{
	.name = "ls103i_pnl_bn101_wxga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};
