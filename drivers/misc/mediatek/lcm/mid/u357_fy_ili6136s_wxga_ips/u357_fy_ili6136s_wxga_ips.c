#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
	#include <platform/mt_pmic.h>
	#include <platform/mt_typedefs.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
/*
	#include <mach/mt_gpio.h>
	#include <mach/mt_pm_ldo.h>
	#include <mach/mt_typedefs.h> */
	#include "upmu_common.h"
	#include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (800) //(1280)
#define FRAME_HEIGHT (1280) //(800)
  
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif


#define GPIO_LCD_POWER_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_3v3      (GPIO82 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCM_RST_EN     (GPIO146 | 0x80000000)
#define GPIO_LCM_RST      GPIO_LCM_RST_EN
/*
#define GPIO_LCD_POWER_EN     (GPIO83  | 0x80000000) 
#define GPIO_LCD_3v3     (GPIO21  | 0x80000000) 
#define GPIO_LCM_RST     GPIO19
*/

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER
extern void DSI_clk_HS_mode(unsigned char enter);

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {
	.set_gpio_out = NULL,
};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if(GPIO==0xffffffff){
		return ;
	}
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
} 
static void lcd_power_en(unsigned char enabled)
{
	
	lcm_set_gpio_output(GPIO_LCD_POWER_EN, enabled);
	lcm_set_gpio_output(GPIO_LCD_3v3, enabled);
	/*
    if(enabled)
    {
		mt_set_gpio_mode(GPIO_LCD_POWER_EN, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_POWER_EN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_POWER_EN, GPIO_OUT_ONE);
		
		mt_set_gpio_mode(GPIO_LCD_3v3, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_3v3, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_3v3, GPIO_OUT_ONE);
	}
    else
    {
    	mt_set_gpio_mode(GPIO_LCD_POWER_EN, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_POWER_EN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_POWER_EN, GPIO_OUT_ZERO);
		
		mt_set_gpio_mode(GPIO_LCD_3v3, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_3v3, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_3v3, GPIO_OUT_ZERO);
	}
	*/
	MDELAY(50);
}

static void lcd_reset(unsigned char enabled)
{
	lcm_set_gpio_output(GPIO_LCM_RST, enabled);
	/*
    if(enabled)
    {
        mt_set_gpio_mode(GPIO_LCM_RST,GPIO_MODE_00);//reset
        mt_set_gpio_dir(GPIO_LCM_RST,GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ONE);
    }
    else
    {
        mt_set_gpio_mode(GPIO_LCM_RST,GPIO_MODE_00);//reset
        mt_set_gpio_dir(GPIO_LCM_RST,GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ZERO);
    }
	 */
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
data_array[0] = 0xAACD1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00301500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x035E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00321500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x25331500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08651500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x103A1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x49361500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x82671500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x27691500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x016C1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00093902;
data_array[1] = 0x09090955;
data_array[2] = 0x09090909;
data_array[3] = 0x00000009;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x00113902;
data_array[1] = 0x09090956;
data_array[2] = 0x09090909;
data_array[3] = 0x09090909;
data_array[4] = 0x09090909;
data_array[5] = 0x00000009;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x006B1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x08581500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xF0731500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x17741500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x84611500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00771500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x06681500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x35281500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x09000057;
data_array[2] = 0x00000009;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x46411500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x504E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x554F1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x04631500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x171A1F53;
data_array[2] = 0x14121213;
data_array[3] = 0x0D101515;
data_array[4] = 0x0D0C0B0B;
data_array[5] = 0x0103080A;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x171A1F54;
data_array[2] = 0x14121213;
data_array[3] = 0x0D101515;
data_array[4] = 0x0D0C0B0B;
data_array[5] = 0x0103080A;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x10291500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x032E1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x67781500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x36761500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x18471500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x312D1500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0xd0501500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x66481500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x003F1500;
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

	params->dsi.mode    = BURST_VDO_MODE;
	//params->dsi.mode	 = SYNC_EVENT_VDO_MODE; 

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;


	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active = 4;//4; //4;
	params->dsi.vertical_backporch =  8;//4; //14;  //16;
	params->dsi.vertical_frontporch = 4;//8; //16; //15;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 16;//8; //4; //4;
	params->dsi.horizontal_backporch =	32; //8; //4; //42;  //50;
	params->dsi.horizontal_frontporch = 16;//40; //105; //16; //44; //60;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;



	params->dsi.PLL_CLOCK=225; //
	
}

static void lcm_init(void)
{
	//static int lcm_inited = 0;

	// step1: power on
	lcd_reset(0);
	lcd_power_en(0);
	lcd_power_en(1);
	MDELAY(20);//Must > 5ms
	lcd_reset(1);
	MDELAY(10);//Must > 5ms
	lcd_reset(0);
	MDELAY(10);//Must > 5ms
	lcd_reset(1);
    MDELAY(50);
	init_lcm_registers();

//	DSI_clk_HS_mode(1);
}


static void lcm_suspend(void)
{
	

	printk("enter func: %s ,%4x ,%4x,%4x ................... \n",__func__,GPIO_LCD_POWER_EN,GPIO_LCD_3v3,GPIO_LCM_RST_EN);

	lcd_reset(0);
	MDELAY(10);
	lcd_power_en(0);
//	DSI_clk_HS_mode(0);
	MDELAY(10); 

	
}

static void lcm_resume(void)
{
	lcm_init();
}

static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
    char  buffer[3];
    int   array[4];

    if(lcm_esd_test)
    {
        lcm_esd_test = FALSE;
        return TRUE;
    }

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0x36, buffer, 1);
    if(buffer[0]==0x90)
    {
        return FALSE;
    }
    else
    {            
        return TRUE;
    }
#else
    return FALSE;
#endif

}

static unsigned int lcm_esd_recover(void)
{
    lcm_init();
    lcm_resume();
    //printk("[KERNEL]---cmd---lcm_esd_recover----%s------\n",__func__);
    return TRUE;
}

LCM_DRIVER u357_fy_ili6136s_wxga_ips_lcm_drv= 
{
	.name			= "u357_fy_ili6136s_wxga_ips",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id     = lcm_compare_id,
	.esd_check      = lcm_esd_check,
    .esd_recover    = lcm_esd_recover,
};
