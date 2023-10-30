#ifdef BUILD_LK
#include <string.h>
//Evan add
#include <debug.h>
#include <sys/types.h>
#include <platform/mt_i2c.h>
//Evan add end
#else
#include <linux/string.h>
#endif

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_pmic.h>
#else
//#include <mach/mt_gpio.h>
//#include <mach/mt_pm_ldo.h>
//#include <mach/upmu_common.h>
 #include "upmu_common.h"
 #include <mt-plat/mt_gpio.h>
 #include <mach/gpio_const.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>
#endif
#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (1200)
#define FRAME_HEIGHT (1920)


#define LCM_DSI_CMD_MODE    0
#if 1   // def BUILD_LK
#define GPIO_LCD_PWR_EN    (GPIO19 | 0x80000000)
#define GPIO_LCD_RST_EN    (GPIO146 | 0x80000000)
#else
extern unsigned int GPIO_LCD_PWR_EN;
extern unsigned int GPIO_LCD_RST_EN;
extern unsigned int GPIO_LCD_LED_EN;
#endif
#define GPIO_OUT_ZERO 0
#define GPIO_OUT_ONE  1
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {
    .set_gpio_out = NULL,
};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    


static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
    if(GPIO == 0xFFFFFFFF){
        return;
    }
#if 1  //#ifdef  BUILD_LK
    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
#else     // this method is for dts parse, no we do not parse it, use mt gpio operation    tongjun Add.
    gpio_direction_output(GPIO, output);
    gpio_set_value(GPIO, output);
#endif
}

static void lcd_power_en(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCD_PWR_EN,enabled);

}


static void lcd_reset(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCD_RST_EN,enabled);

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

    params->type     = LCM_TYPE_DSI;
    params->width    = FRAME_WIDTH;
    params->height   = FRAME_HEIGHT;
    params->dsi.mode = SYNC_EVENT_VDO_MODE; //BURST_VDO_MODE;BURST_VDO_MODE;

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM				= LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;  //LCM_DSI_FORMAT_RGB666;

    // Video mode setting		
    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;  //LCM_PACKED_PS_18BIT_RGB666;

    params->dsi.vertical_sync_active				= 1;//0;
    params->dsi.vertical_backporch					= 11;//23;
    params->dsi.vertical_frontporch 				= 14;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active				= 1;//0;
    params->dsi.horizontal_backporch				= 32;//160;
    params->dsi.horizontal_frontporch				= 110; //100;//160;
    params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;

    params->dsi.PLL_CLOCK = 478;//148;
}


static void init_lcm_registers(void)
{
    unsigned int data_array[16];


	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);	

}


static void lcm_init(void)
{

    lcd_reset(0);
    lcd_power_en(0);
    lcd_power_en(1);
    MDELAY(50);//Must > 5ms
    lcd_reset(1);
    
    MDELAY(80);
	
init_lcm_registers();


	MDELAY(100);
}


static void lcm_suspend(void)
{
    lcd_reset(0);
    lcd_power_en(0);
    MDELAY(10);
}


static void lcm_resume(void)
{
	lcm_init();
}

LCM_DRIVER lf808_pvo_otm1287a_wuxga_ips_auo_lcm_drv = 
{
	.name = "lf808_pvo_otm1287a_wuxga_ips_auo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

