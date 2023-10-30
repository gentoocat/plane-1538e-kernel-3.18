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
data_array[0]=0x00043902;
data_array[1]=0xF46193BF;
dsi_set_cmdq(data_array,2,1);


data_array[0]=0x00063902;
data_array[1]=0x230100B0;
data_array[2]=0x00006645;
dsi_set_cmdq(data_array,3,1);

data_array[0]=0x000C3902;
data_array[1]=0x69010FC7;
data_array[2]=0x19280A0A;
data_array[3]=0x22323200;
dsi_set_cmdq(data_array,4,1);

data_array[0]=0x00053902;
data_array[1]=0xD30800BB;
data_array[2]=0x00000054;
dsi_set_cmdq(data_array,3,1);

data_array[0]=0x01BE1500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x00093902;
data_array[1]=0x380034CC;
data_array[2]=0x02910007;
data_array[3]=0x000000D0;
dsi_set_cmdq(data_array,4,1);

data_array[0]=0x05C41500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x38C21500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x01B41500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x00BE1500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x00053902;
data_array[1]=0x2D2D3EBA;
data_array[2]=0x0000000E;
dsi_set_cmdq(data_array,3,1);

data_array[0]=0x20C11500;
dsi_set_cmdq(data_array,1,1);

data_array[0]=0x00093902;
data_array[1]=0x0C0500C3;
data_array[2]=0xA00CA018;
data_array[3]=0x0000004E;
dsi_set_cmdq(data_array,4,1);

data_array[0]=0x000D3902;
data_array[1]=0x91A006C4;
data_array[2]=0x00000C04;
data_array[3]=0x00000000;
data_array[4]=0x00000000;
dsi_set_cmdq(data_array,5,1);

data_array[0]=0x00273902;
data_array[1]=0x5A6B7CC8;
data_array[2]=0x3F3D4C4F;
data_array[3]=0x4747482E;
data_array[4]=0x4F5D5566;
data_array[5]=0x00263E4A;
data_array[6]=0x4F5A6B7C;
data_array[7]=0x2E3F3D4C;
data_array[8]=0x66474748;
data_array[9]=0x4A4F5D55;
data_array[10]=0x0000263E;
dsi_set_cmdq(data_array,11,1);


data_array[0]=0x00043902;
data_array[1]=0x00B613CC;
dsi_set_cmdq(data_array,2,1);

data_array[0]=0x00173902;
data_array[1]=0x040505D5;
data_array[2]=0x06070704;
data_array[3]=0x1F1F0006;
data_array[4]=0x1F1F1F1F;
data_array[5]=0x1F1F1F01;
data_array[6]=0x001F1F1F;
dsi_set_cmdq(data_array,7,1);

data_array[0]=0x00173902;
data_array[1]=0x040505D6;
data_array[2]=0x06070704;
data_array[3]=0x1F1F0006;
data_array[4]=0x1F1F1F1F;
data_array[5]=0x1F1F1F01;
data_array[6]=0x001F1F1F;
dsi_set_cmdq(data_array,7,1);

data_array[0]=0x002C3902;
data_array[1]=0x000010D9;
data_array[2]=0x00200E10;
data_array[3]=0x10318003;
data_array[4]=0x0000C501;
data_array[5]=0x00860600;
data_array[6]=0x00000000;
data_array[7]=0x00078603;
data_array[8]=0x05000055;
data_array[9]=0x86030005;
data_array[10]=0x08080800;
data_array[11]=0x04080808;
dsi_set_cmdq(data_array,12,1);

data_array[0]=0x00053902;
data_array[1]=0x008000D0;
data_array[2]=0x0000009A;
dsi_set_cmdq(data_array,3,1);

data_array[0]=0x00073902;
data_array[1]=0x00B100B8;
data_array[2]=0x0000B100;
dsi_set_cmdq(data_array,3,1);


data_array[0]=0x00110500;
dsi_set_cmdq(data_array,1,1);
MDELAY(120);


data_array[0]=0x00290500;
dsi_set_cmdq(data_array,1,1);
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
    params->dsi.LANE_NUM                = LCM_THREE_LANE;

    //params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


    params->dsi.vertical_sync_active                            = 1; //2; //4;
    params->dsi.vertical_backporch                              = 16; //10; //16;
    params->dsi.vertical_frontporch                             = 3;//5; 
    params->dsi.vertical_active_line                            = FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active                          = 4; // 10; //5;//6;
    params->dsi.horizontal_backporch                            = 16; //60; //60; //80;
    params->dsi.horizontal_frontporch                           = 10; //60; 
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

LCM_DRIVER lf705_fx_im10705_jd9362_wxga_ips_lcm_drv = 
{
	.name = "lf705_fx_im10705_jd9362_wxga_ips",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

