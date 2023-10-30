#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

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
#endif
#include "lcm_drv.h"



// ---------------------------------------------------------------------------
// Local Constants 
// ---------------------------------------------------------------------------
#define FRAME_WIDTH (800)
#define FRAME_HEIGHT (1280)


#define LCM_DSI_CMD_MODE 0

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)
#define GPIO_LCD_RST_EN       GPIO_LCM_RST_EN



#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
// Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {
	.set_gpio_out = NULL,
};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

extern void DSI_clk_HS_mode(unsigned char enter);

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{

    if(GPIO == 0xFFFFFFFF)
    {
    #ifdef BUILD_LK
    #elif (defined BUILD_UBOOT)
         // do nothing in uboot
    #else	
	 //printf("kernel] lcm_set gpio()  \n");
    #endif
        return;
    }

    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

// ---------------------------------------------------------------------------
// Local Functions   
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(ppara, size, force_update)	        lcm_util.dsi_set_cmdq_V3(ppara, size, force_update)

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size) 

static void init_lcm_registers(void)
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

data_array[0] = 0x032D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA9011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00031500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA8041500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB1181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB11B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x011C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3E1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2D201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2D211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7E221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x19371500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05381500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00391500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x013A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x783C1500;
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

data_array[0] = 0x09441500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x28451500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0F551500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x89571500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x285A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x195B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7C5D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x665E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x565F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x49601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x34621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x37631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x36651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x33661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4F681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3D691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x436A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x356B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x306C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x246D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x136E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7C701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x56721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x49731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x34751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x37761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x36781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x33791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x327A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x4F7B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3D7C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x437D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x357E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x307F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x13811500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00821500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44021500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44031500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47041500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47051500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x46061500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x46071500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40081500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F0A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F0B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F0C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F0D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F0E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x410F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F101500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F111500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F121500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F131500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F141500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F151500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45161500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x44191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x471A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x471B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x461C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x461D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x401E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F231500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F241500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x41251500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F261500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F271500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F2A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F2B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x105B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x065C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x205D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x015E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x025F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x62631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x62641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x62691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x666A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x086B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x046D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x046E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x886F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x3E981500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2B2B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x032D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x442E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00E01500;
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


}


static void lcd_reset(unsigned char enabled)
{
	if (enabled)
	{
		mt_set_gpio_mode(GPIO_LCD_RST_EN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_RST_EN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ONE);
	}
	else
	{	
		mt_set_gpio_mode(GPIO_LCD_RST_EN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_RST_EN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);		
	}
}
	
static void lcd_power_en(unsigned char enabled)
{
	if (enabled)
	{
		//VGP1 3.3v VGP2 1.8
		//upmu_set_rg_vgp2_vosel(0x3);
		//upmu_set_rg_vgp2_en(0x1);
              
		//upmu_set_rg_vgp1_vosel(0x7);
		//upmu_set_rg_vgp1_en(0x1);
		
		//lcm_set_gpio_output(GPIO_LCM_PWR, GPIO_OUT_ONE);
	}
	else
	{
		//lcm_set_gpio_output(GPIO_LCM_PWR, GPIO_OUT_ZERO);
		
		//upmu_set_rg_vgp1_en(0);
		//upmu_set_rg_vgp1_vosel(0);
		//upmu_set_rg_vgp2_en(0);
		//upmu_set_rg_vgp2_vosel(0);
		
	}
	if(enabled)
	{
		mt_set_gpio_mode(GPIO_LCD_PWR_EN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PWR_EN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	}
	else
	{
		mt_set_gpio_mode(GPIO_LCD_PWR_EN, GPIO_MODE_00);
		mt_set_gpio_dir(GPIO_LCD_PWR_EN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO); 
	}
}



// ---------------------------------------------------------------------------
// LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

extern void DSI_clk_HS_mode(unsigned char enter);

static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DSI;

    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    params->dsi.mode    = BURST_VDO_MODE;
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;

    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


    params->dsi.vertical_sync_active                            = 4; //2; //4;
    params->dsi.vertical_backporch                              = 6; //10; //16;
    params->dsi.vertical_frontporch                             = 8;//5; 
    params->dsi.vertical_active_line                            = FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active                          = 20; // 10; //5;//6;
    params->dsi.horizontal_backporch                            = 20; //60; //60; //80;
    params->dsi.horizontal_frontporch                           = 32; //60; 
    params->dsi.horizontal_active_pixel                         = FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 200; 

}



static void lcm_init(void)
{
	lcd_reset(0);
	lcd_power_en(0);
	lcd_power_en(1);
	MDELAY(50);//Must > 5ms
	lcd_reset(1);
	MDELAY(10);//Must > 5ms
	lcd_reset(0);
	MDELAY(15);//Must > 5ms
	lcd_reset(1);
    MDELAY(50);

	init_lcm_registers();

	MDELAY(50);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,1);

}




static void lcm_suspend(void)
{

	lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
    MDELAY(100);	
	
	lcd_power_en(0);

}



static void lcm_resume(void)
{
	lcm_init();
}

LCM_DRIVER lf705_ch_jd9366_boe_wxga_ips_7_lcm_drv = 
{
	.name = "lf705_ch_jd9366_boe_wxga_ips_7",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

