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

data_array[0] = 0x00053902;
data_array[1] = 0xA555AAFF;
data_array[2] = 0x00000080;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x0000116F;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000020F7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x066F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA0F71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x196F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x12F71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x03F41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000008;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x80C81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00016CB1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x08B61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x026F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08B81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x004474BB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000000BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x0CB002BD;
data_array[2] = 0x0000000A;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000108;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000505B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000505B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000190BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000190BD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00CA1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x0CC01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29BE1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x29BF1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003737B3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001919B4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x004444B9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x002424BA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000208;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x01EE1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x150609EF;
data_array[2] = 0x00000018;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x000000B0;
data_array[2] = 0x00430025;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x066F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x005400B0;
data_array[2] = 0x00A00068;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x0C6F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x01C000B0;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x013001B1;
data_array[2] = 0x00AE0178;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x066F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x020802B1;
data_array[2] = 0x00540252;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x0C6F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x029902B1;
data_array[2] = 0x000000F0;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x032003B2;
data_array[2] = 0x00760356;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x066F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x039303B2;
data_array[2] = 0x00B903A4;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x0C6F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x03C903B2;
data_array[2] = 0x000000E3;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x03FC03B3;
data_array[2] = 0x000000FF;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000608;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001000B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001412B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001816B2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00291AB3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00082AB4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131B5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131B6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131B7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000A31B8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131B9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131BA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00310BBB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131BD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131BE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x002A09BF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001B29C0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001719C1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001315C2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000111C3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131E5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001B09C4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001719C5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001315C6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x002911C7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00012AC8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131C9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131CA;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131CB;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000B31CC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131CD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131CE;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00310ACF;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131D0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131D1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131D2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x002A00D3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001029D4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001412D5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001816D6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00081AD7;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003131E6;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000000D8;
data_array[2] = 0x00000054;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x001500D9;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00E71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000308;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000020B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000020B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000005B2;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000005B6;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000005B7;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000057BA;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x000057BB;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x000000C0;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x000000C1;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x60C41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x40C51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x52AA55F0;
data_array[2] = 0x00000508;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x030103BD;
data_array[2] = 0x00000303;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B2;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B4;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x000617B5;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00B81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00B91500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00BA1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x02BB1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00BC1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x07C01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x80C41500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xA4C51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003005C8;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x003101C9;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x3C0000CC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x3C0000CD;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0xFD0400D1;
data_array[2] = 0x00001007;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x020500D2;
data_array[2] = 0x00001007;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x06E51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06E61500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06E71500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06E81500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06E91500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06EA1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x30ED1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x116F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x01F31500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00351500;
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


	params->dsi.vertical_sync_active = 4;
	params->dsi.vertical_backporch =  8;
	params->dsi.vertical_frontporch = 8;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 4;
	params->dsi.horizontal_backporch =	132; 
	params->dsi.horizontal_frontporch = 24;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;


	params->dsi.PLL_CLOCK=225;

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

LCM_DRIVER lf705_qc_nt35521_wxga_ips_lux_lcm_drv = 
{
	.name = "lf705_qc_nt35521_wxga_ips_lux",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

