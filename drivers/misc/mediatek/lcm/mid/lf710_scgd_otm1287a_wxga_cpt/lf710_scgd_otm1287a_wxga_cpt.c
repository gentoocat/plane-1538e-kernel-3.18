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
#define GPIO_LCM_PWREN         (GPIO120|0x80000000)
#define GPIO_LCM_EN            (GPIO64|0x80000000)
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



static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}


static void lcd_power_en(unsigned char enabled)
{
#ifdef BUILD_LK
    printf("[IND][K] %s : %s\n", __func__, enabled ? "on" : "off");
#else
    printk("[IND][LK] %s : %s\n", __func__, enabled ? "on" : "off");
#endif
	//
    lcm_set_gpio_output(GPIO_LCM_EN, enabled);
}

static void lcd_vdd_enable(unsigned char enabled)
{
#ifdef BUILD_LK
    printf("[IND][K] %s : %s\n", __func__, enabled ? "on" : "off");
#else
    printk("[IND][LK] %s : %s\n", __func__, enabled ? "on" : "off");
#endif
	lcm_set_gpio_output(GPIO_LCM_PWREN, enabled);
}

#if 1
static void stbyb_control(unsigned char enabled)
{
#ifdef BUILD_LK
    printf("[IND][K] %s : %s\n", __func__, enabled ? "on" : "off");
#else
    printk("[IND][LK] %s : %s\n", __func__, enabled ? "on" : "off");
#endif
	lcm_set_gpio_output(GPIO_LCD_TE_EN, enabled);
}
#endif

static void lcd_reset(unsigned char enabled)
{

#ifdef BUILD_LK
    printf("[IND][K] y___0%s : %s\n", __func__, enabled ? "on" : "off");
#else
    printk("[IND][LK] Y___1%s : %s\n", __func__, enabled ? "on" : "off");
#endif
	lcm_set_gpio_output(GPIO_LCD_RST_EN, enabled);
}

#if 1//0
static void lcd_bl_en(unsigned char enabled)
{
#ifdef BUILD_LK
    printf("[IND][K] %s : %s\n", __func__, enabled ? "on" : "off");
#else
    printk("[IND][LK] %s : %s\n", __func__, enabled ? "on" : "off");
#endif
	lcm_set_gpio_output(GPIO_LCD_BL_EN, enabled);
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

	params->physical_height = 173;
	params->physical_width	= 108;
	
	// enable tearing-free ****




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

	

	params->dsi.vertical_sync_active = 2; 
	params->dsi.vertical_backporch =  10; 
	params->dsi.vertical_frontporch = 12; 
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 25; 
	params->dsi.horizontal_backporch =	 55; 
	params->dsi.horizontal_frontporch =  25; 
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.PLL_CLOCK=273; //


}

//data1 = 0x67011500;
static void init_lcm_registers(void)
{		
	unsigned int data_array[16];

data_array[0] = 0xAACD1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00301500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00321500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x18331500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x303A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40361500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x82671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x27691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x016C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x046D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x12111255;
data_array[2] = 0x00000011;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x12111256;
data_array[2] = 0x12111211;
data_array[3] = 0x12111211;
data_array[4] = 0x12111211;
data_array[5] = 0x00000011;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x006B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x30731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x84611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x807C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x31281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00093902;
data_array[1] = 0x12121257;
data_array[2] = 0x12121212;
data_array[3] = 0x00000012;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x40411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x804C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4F4E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x504F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x804C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x27351500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x1D1E1F53;
data_array[2] = 0x19171716;
data_array[3] = 0x1011181A;
data_array[4] = 0x0D080B0C;
data_array[5] = 0x0107090D;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x16191E54;
data_array[2] = 0x12121212;
data_array[3] = 0x09121514;
data_array[4] = 0x0A0B090D;
data_array[5] = 0x0504060A;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x10291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x807C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x042E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x67781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x36761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xF4471500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x312D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xD0501500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01481500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x004D1500;
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
	lcd_reset(0);
	lcd_vdd_enable(0);
	lcd_power_en(0);
	MDELAY(10);
		
	lcd_power_en(1);   //1.8V

	MDELAY(10);
	lcd_vdd_enable(1); //3.3V
	MDELAY(1);
	
	lcd_reset(1);
	MDELAY(10);
	lcd_reset(0);
	MDELAY(60);
	lcd_reset(1);
	
	MDELAY(50);//Must > 5ms
	
	init_lcm_registers();

}


static void lcm_suspend(void)
{
    unsigned int data_array[16];

	
#ifdef BUILD_LK
	printf("[Elink_LCM_Drv ZX_______] %s, LK \n", __func__);
#else
	printk("[Elink_LCM_Drv ZX_______] %s, kernel", __func__);
#endif
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

LCM_DRIVER lf710_scgd_otm1287a_wxga_cpt_lcm_drv = 
{
	.name = "lf710_scgd_otm1287a_wxga_cpt",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};
