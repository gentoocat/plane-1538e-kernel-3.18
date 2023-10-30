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

data_array[0] = 0x00e01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x93e11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x65e21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xf8e31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02e61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02e71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01e01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB7011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB2181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x001A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xB21B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x011C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7E1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x28201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x28211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0E221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xC8241500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29371500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05381500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08391500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x123a1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x783b1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3c1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3e1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xFF3f1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06401500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA0411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08431500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07441500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x24451500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01551500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01561500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x89571500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2A591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x315A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x155B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7C5D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x645E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x545F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x42611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x34631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1C641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2E661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2B671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x46681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x386A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2A6B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x286C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1B6D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0D6E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7C701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x64711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x54721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x47731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x42741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x34761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1C771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x32781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2E791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2B7A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x467B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x327C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x387D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2A7E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x287F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1B801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0D811500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00821500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04011500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08021500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05031500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x09041500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06051500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A061500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07071500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0B081500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f091500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f0A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f0B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f0C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f0D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f0E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x170F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x37101500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10111500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F121500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F131500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F141500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F151500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00161500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04171500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08181500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05191500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x091A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x061B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0A1C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x071D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0B1E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f1F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f201500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f211500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f221500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f231500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1f241500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x17251500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x37261500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10271500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F2A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x1F2B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00591500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x015C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x305D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x005F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x30601500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00621500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x6A641500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x45651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08661500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x73671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x6A6A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x086B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x006E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x886F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00701500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00711500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06721500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7B731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x80751500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x05771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x10781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00791500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x007C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x037D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x7B7E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x010E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2F981500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x2B2B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x032d1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x442E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00E01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x93E11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x65E21500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xf8E31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02801500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);

data_array[0] = 0x00351500;
dsi_set_cmdq(data_array, 1, 1);


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
    params->dsi.LANE_NUM                = LCM_THREE_LANE;

    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 4;
	params->dsi.vertical_frontporch					= 8;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 20;
	params->dsi.horizontal_backporch				= 20;
	params->dsi.horizontal_frontporch				= 20;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;


	params->dsi.PLL_CLOCK=260;

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

LCM_DRIVER lf705_qingyuqn_jd9366ab_wxga_ips_lcm_drv = 
{
	.name = "lf705_qingyuqn_jd9366ab_wxga_ips",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

