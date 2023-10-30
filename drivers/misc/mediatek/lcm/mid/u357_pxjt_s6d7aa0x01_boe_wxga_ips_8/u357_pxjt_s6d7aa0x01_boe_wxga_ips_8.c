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

data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF0;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF1;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x00033902;
data_array[1] = 0x00A5A5FC;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x00033902;
data_array[1] = 0x001000D0;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x10B11500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);


data_array[0] = 0x00053902;
data_array[1] = 0x2F2214B2;
data_array[2] = 0x00000004;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);


data_array[0] = 0x00063902;
data_array[1] = 0x080802F2;
data_array[2] = 0x00001040;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);


data_array[0] = 0x04B01500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);


data_array[0] = 0x09FD1500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);


data_array[0] = 0x000b3902;
data_array[1] = 0xE2D701F3;
data_array[2] = 0x77F7F462;
data_array[3] = 0x0000263C;
dsi_set_cmdq(data_array, 4, 1);
MDELAY(1);


data_array[0] = 0x002e3902;
data_array[1] = 0x030200F4;
data_array[2] = 0x09020326;
data_array[3] = 0x16160700;
data_array[4] = 0x08080003;
data_array[5] = 0x120F0E03;
data_array[6] = 0x0C1E1D1C;
data_array[7] = 0x02040109;
data_array[8] = 0x72757461;
data_array[9] = 0xB0808083;
data_array[10] = 0x28010100;
data_array[11] = 0x01280304;
data_array[12] = 0x000032D1;
dsi_set_cmdq(data_array, 13, 1);
MDELAY(1);


data_array[0] = 0x001b3902;
data_array[1] = 0x282843F5;
data_array[2] = 0x5298AB5F;
data_array[3] = 0x0443330F;
data_array[4] = 0x05525459;
data_array[5] = 0x604E6040;
data_array[6] = 0x52262740;
data_array[7] = 0x00186D25;
dsi_set_cmdq(data_array, 8, 1);
MDELAY(1);


data_array[0] = 0x00093902;
data_array[1] = 0x250025EE;
data_array[2] = 0x25002500;
data_array[3] = 0x00000000;
dsi_set_cmdq(data_array, 4, 1);
MDELAY(1);


data_array[0] = 0x00093902;
data_array[1] = 0x981234EF;
data_array[2] = 0x240020BA;
data_array[3] = 0x00000080;
dsi_set_cmdq(data_array, 4, 1);
MDELAY(1);


data_array[0] = 0x00213902;
data_array[1] = 0x0A0E0EF7;
data_array[2] = 0x0B0F0F0A;
data_array[3] = 0x0107050B;
data_array[4] = 0x01010101;
data_array[5] = 0x080C0C01;
data_array[6] = 0x090D0D08;
data_array[7] = 0x01060409;
data_array[8] = 0x01010101;
data_array[9] = 0x00000001;
dsi_set_cmdq(data_array, 10, 1);
MDELAY(1);


data_array[0] = 0x00043902;
data_array[1] = 0x084E01BC;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x00063902;
data_array[1] = 0x1C1003E1;
data_array[2] = 0x000010A0;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);


data_array[0] = 0x00073902;
data_array[1] = 0xA62560F6;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);


data_array[0] = 0x00073902;
data_array[1] = 0x030D00FE;
data_array[2] = 0x00080021;
dsi_set_cmdq(data_array, 3, 1);
MDELAY(1);


data_array[0] = 0x00123902;
data_array[1] = 0x053500FA;
data_array[2] = 0x0B06010B;
data_array[3] = 0x19160D0A;
data_array[4] = 0x19191A19;
data_array[5] = 0x0000221A;
dsi_set_cmdq(data_array, 6, 1);
MDELAY(1);


data_array[0] = 0x00123902;
data_array[1] = 0x053500FB;
data_array[2] = 0x0B06010B;
data_array[3] = 0x19160D0A;
data_array[4] = 0x19191A19;
data_array[5] = 0x0000221A;
dsi_set_cmdq(data_array, 6, 1);
MDELAY(1);


data_array[0] = 0x00043902;
data_array[1] = 0x280040C3;
dsi_set_cmdq(data_array, 2, 1);
MDELAY(1);


data_array[0] = 0x00350500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);


}

static void lcd_reset(unsigned char enabled)
{
#ifdef BUILD_LK
	printf("[IND][K] y___0%s : %s\n", __func__, enabled ? "on" : "off");
#else
	printk("[IND][LK] Y___1%s : %s\n", __func__, enabled ? "on" : "off");
#endif

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

    //params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


    params->dsi.vertical_sync_active                            = 4; //2; //4;
    params->dsi.vertical_backporch                              = 4; //10; //16;
    params->dsi.vertical_frontporch                             = 8;//5; 
    params->dsi.vertical_active_line                            = FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active                          = 16; // 10; //5;//6;
    params->dsi.horizontal_backporch                            = 48; //60; //60; //80;
    params->dsi.horizontal_frontporch                           = 16; //60; 
    params->dsi.horizontal_active_pixel                         = FRAME_WIDTH;
	params->dsi.PLL_CLOCK = 260; 

}



static void lcm_init(void)
{
	lcd_reset(0);
	lcd_power_en(0);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
	MDELAY(10);//Must > 5ms
	lcd_power_en(1);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,1);
	MDELAY(5);//Must > 5ms
	lcd_reset(1);
	MDELAY(10);//Must > 5ms
	lcd_reset(0);
	MDELAY(10);//Must > 5ms
	lcd_reset(1);
       MDELAY(50);
	init_lcm_registers();
}




static void lcm_suspend(void)
{
    lcd_reset(0);
    MDELAY(100);	
    lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
    lcd_power_en(0);
}



static void lcm_resume(void)
{
	lcm_init();
}

LCM_DRIVER u357_pxjt_s6d7aa0x01_boe_wxga_ips_8_lcm_drv = 
{
	.name = "u357_pxjt_s6d7aa0x01_boe_wxga_ips_8",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

