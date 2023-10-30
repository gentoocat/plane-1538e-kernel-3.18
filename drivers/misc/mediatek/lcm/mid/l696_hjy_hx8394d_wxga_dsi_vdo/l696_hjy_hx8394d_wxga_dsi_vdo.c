#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
    #include <platform/disp_drv_platform.h>
	#include <platform/mt_gpio.h>
	#include <platform/mt_i2c.h> 
	#include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
//	#include <mach/mt_pm_ldo.h>
	#include "upmu_common.h"
    #include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif
//#include <cust_gpio_usage.h>

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)
/*
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#else
//#define GPIO_LCD_PWR_EN      0xFFFFFFFF
#endif
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#else
//#define GPIO_LCD_BL_EN      0xFFFFFFFF
#endif
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCM_RST_EN      GPIO_LCM_RST
#else
#define GPIO_LCM_RST_EN      0xFFFFFFFF
#endif
*/
static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
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


//static unsigned char lcd_id_pins_value = 0xFF;
//liuhanhui 20150917 +++
//static const unsigned char LCD_MODULE_ID = 0x01; //  haobing modified 2013.07.11
const static unsigned char LCD_MODULE_ID = 0xc5;
//liuhanhui 20150917 ---
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE 0
#define FRAME_WIDTH  										(800)
#define FRAME_HEIGHT 										(1280)

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER



// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------


struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//update initial param for IC nt35523 0.01
static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28,0,{}},
	{REGFLAG_DELAY, 10, {}},
//	{0xC3,3,{0x40,0x00,0x20}},
//	{REGFLAG_DELAY, 5, {}},
	{0x10,0,{}},
	{REGFLAG_DELAY, 100, {}}
};
	
//update initial param for IC nt35523 0.01
/*
static struct LCM_setting_table lcm_initialization_setting[] = {
{0xB9,3,{0xFF,0x83,0x94}},
{0xB0,4,{0x00,0x00,0x7d,0x0c}},

{0xBA,2,{0x33,0x83}},


{0xB1,15,{0x6C,0x15,0x15,0x24,0x04,0x11,0xF1,0x80,0xE4,0x97,0x23,0x80,0xC0,0xD2,0x58}},

                        
{0xB2,11,{0x00,0x64,0x10,0x07,0x20,0x1C,0x08,0x08,0x1C,0x4D,0x00}},  

   
{0xB4,12,{0x00,0xFF,0x03,0x64,0x03,0x80,0x03,0x64,0x01,0x74,0x01,0x80}}, 


{0xD3,30,{0x00,0x06,0x00,0x40,0x1a,0x08,0x00,0x32,0x10,0x07,0x00,0x07,0x54,0x15,0x0f,0x05,0x04,0x02,0x12,0x10,0x05,0x07,0x33,0x33,0x0b,0x0b,0x37,0x10,0x07,0x07}}, 

      
{0xd5,44,{0x19,0x19,0x18,0x18,0x1a,0x1a,0x1b,0x1b,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x20,0x21,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x22,0x23,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},


{0xd6,44,{0x18,0x18,0x19,0x19,0x1a,0x1a,0x1b,0x1b,0x03,0x02,0x01,0x00,0x07,0x06,0x05,0x04,0x23,0x22,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x21,0x20,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},


{0xe0,42,{0x00,0x06,0x0c,0x31,0x34,0x3f,0x1d,0x41,0x06,0x0a,0x0c,0x17,0x0f,0x12,0x15,0x13,0x14,0x07,0x12,0x15,0x16,0x00,0x06,0x0b,0x30,0x34,0x3f,0x1d,0x40,0x07,0x0a,0x0d,0x18,0x0e,0x12,0x14,0x12,0x14,0x08,0x13,0x14,0x19}},


{0xb6,2,{0x60,0x60}},//36 36 54 54

{0xcc,1,{0x05}},

{0xd2,1,{0x55}},

{0xc0,2,{0x30,0x14}},

{0xbf,3,{0x41,0x0e,01}},

{0xc7,4,{0x00,0xc0,0x00,0xc0}},

{0xdf,1,{0x8e}},

{0x36,1,{0x03}},//0x36

{0x11,1,{0x00}},
{REGFLAG_DELAY, 10, {}},



{REGFLAG_DELAY, 120, {}},

{0x29,1,{0x00}},

{REGFLAG_DELAY, 50, {}},

};
*/

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

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

void init_lcm_registers(void)
{
unsigned int data_array[16];

data_array[0] = 0x00043902;
data_array[1] = 0x9483FFB9;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(10);

data_array[0] = 0x00053902;
data_array[1] = 0x7D0000B0;
data_array[2] = 0x0000000C;
dsi_set_cmdq(&data_array[0], 3, 1);
MDELAY(1);

data_array[0] = 0x00033902;
data_array[1] = 0x008333BA;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00103902;
data_array[1] = 0x15156CB1;
data_array[2] = 0xF1110424;
data_array[3] = 0x2397E480;
data_array[4] = 0x58D2C080;
dsi_set_cmdq(&data_array[0], 5, 1);
MDELAY(1);

data_array[0] = 0x000C3902;
data_array[1] = 0x106400B2;
data_array[2] = 0x081C2007;
data_array[3] = 0x004D1C08;
dsi_set_cmdq(&data_array[0], 4, 1);
MDELAY(1);

data_array[0] = 0x000D3902;
data_array[1] = 0x03FF00B4;
data_array[2] = 0x03800364;
data_array[3] = 0x01740164;
data_array[4] = 0x00000080;
dsi_set_cmdq(&data_array[0], 5, 1);
MDELAY(1);

data_array[0] = 0x00263902;
data_array[1] = 0x000600D3;
data_array[2] = 0x00081A40;
data_array[3] = 0x00071032;
data_array[4] = 0x0F155407;
data_array[5] = 0x12020405;
data_array[6] = 0x33070510;
data_array[7] = 0x370B0B33;
data_array[8] = 0x08070710;
data_array[9] = 0x0A000000;
data_array[10] = 0x00000100;
dsi_set_cmdq(&data_array[0], 11, 1);
MDELAY(1);

data_array[0] = 0x002D3902;
data_array[1] = 0x181919D5;
data_array[2] = 0x1B1A1A18;
data_array[3] = 0x0605041B;
data_array[4] = 0x02010007;
data_array[5] = 0x18212003;
data_array[6] = 0x18181818;
data_array[7] = 0x18181818;
data_array[8] = 0x22181818;
data_array[9] = 0x18181823;
data_array[10] = 0x18181818;
data_array[11] = 0x18181818;
data_array[12] = 0x00000018;
dsi_set_cmdq(&data_array[0], 13, 1);
MDELAY(1);

data_array[0] = 0x002D3902;
data_array[1] = 0x191818D6;
data_array[2] = 0x1B1A1A19;
data_array[3] = 0x0102031B;
data_array[4] = 0x05060700;
data_array[5] = 0x18222304;
data_array[6] = 0x18181818;
data_array[7] = 0x18181818;
data_array[8] = 0x21181818;
data_array[9] = 0x18181820;
data_array[10] = 0x18181818;
data_array[11] = 0x18181818;
data_array[12] = 0x00000018;
dsi_set_cmdq(&data_array[0], 13, 1);
MDELAY(1);

data_array[0] = 0x002B3902;
data_array[1] = 0x0C0600E0;
data_array[2] = 0x1D3F3431;
data_array[3] = 0x0C0A0641;
data_array[4] = 0x15120F17;
data_array[5] = 0x12071413;
data_array[6] = 0x06001615;
data_array[7] = 0x3F34300B;
data_array[8] = 0x0A07401D;
data_array[9] = 0x120E180D;
data_array[10] = 0x08141214;
data_array[11] = 0x00191413;
dsi_set_cmdq(&data_array[0], 12, 1);
MDELAY(1);

data_array[0] = 0x00033902;
data_array[1] = 0x003434B6;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00023902;
data_array[1] = 0x000009CC;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00023902;
data_array[1] = 0x000066D2;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00033902;
data_array[1] = 0x001430C0;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00043902;
data_array[1] = 0x010E41BF;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00053902;
data_array[1] = 0x00C000C7;
data_array[2] = 0x000000C0;
dsi_set_cmdq(&data_array[0], 3, 1);
MDELAY(1);

data_array[0] = 0x00023902;
data_array[1] = 0x00008EDF;
dsi_set_cmdq(&data_array[0], 2, 1);
MDELAY(1);

data_array[0] = 0x00110500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(150);

data_array[0] = 0x00290500;
dsi_set_cmdq(&data_array[0], 1, 1);
MDELAY(20);
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
	params->dsi.mode	 = SYNC_PULSE_VDO_MODE;
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
#if 0

	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 4;
	params->dsi.vertical_frontporch					= 8;
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 16;
	params->dsi.horizontal_backporch				= 48;
	params->dsi.horizontal_frontporch				= 16;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 230; 
	#else
	
	params->dsi.vertical_sync_active				= 2;
	params->dsi.vertical_backporch					= 16; //4//4
	params->dsi.vertical_frontporch					= 9;  //8
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 70;//16
	params->dsi.horizontal_backporch				= 70;//48
	params->dsi.horizontal_frontporch				= 70;//16
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;
	params->dsi.PLL_CLOCK = 200; 
	#endif
	params->dsi.clk_lp_per_line_enable   = 1;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x53;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x24;
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}

static void lcm_init_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
		
#else
	printk("[Kernel/LCM] lcm_init_power() enter\n");
	
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
	
#endif

}

static void lcm_suspend_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_suspend_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(20);
			
#else
	printk("[Kernel/LCM] lcm_suspend_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(20);
		
#endif

}

static void lcm_resume_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
			
#else
	printk("[Kernel/LCM] lcm_resume_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(20);
		
#endif

}

static void lcm_init(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init() enter\n");
        //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ONE);//ADD LIUHANHUI
 
        //MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(120);
	
	
	init_lcm_registers();
	
	// when phone initial , config output high, enable backlight drv chip  
//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
	printf("[LK/LCM] lcm_init() end\n");
#else

       //MDELAY(10);
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(120);
	
	
	init_lcm_registers();
	
	// when phone initial , config output high, enable backlight drv chip  
//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
	printk("[Kernel/LCM] lcm_init() enter\n");

#endif
}

static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");
         //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ZERO);//ADD LIUHANHUI
      //   MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
#else
	printk("[Kernel/LCM] lcm_suspend() enter\n");
         //lcm_set_gpio_output(GPIO_LCM_LVDS_PWR_EN, GPIO_OUT_ZERO);//ADD LIUHANHUI
	lcm_set_gpio_output(GPIO_LCD_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  

	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	MDELAY(10);
	printk("[Kernel/LCM] lcm_suspend() end\n");

#endif
}

static void lcm_resume(void)
{

#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume() enter\n");
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(120);
	
	
	init_lcm_registers();
	
	// when phone initial , config output high, enable backlight drv chip  
//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
#else

	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ZERO);//SET_RESET_PIN(0);
	
	MDELAY(20);
	
	lcm_set_gpio_output(GPIO_LCM_RST_EN, GPIO_OUT_ONE);//SET_RESET_PIN(1);
	
	MDELAY(120);
	
	
	init_lcm_registers();
	
	// when phone initial , config output high, enable backlight drv chip  
//	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	
#endif

}


LCM_DRIVER l696_hjy_hx8394d_wxga_dsi_vdo_lcm_drv = 
{
    	.name		  = "l696_hjy_hx8394d_wxga_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
    //.compare_id     	= lcm_compare_id,
     .init_power		= lcm_init_power,
     .resume_power = lcm_resume_power,
     .suspend_power = lcm_suspend_power,
   
};
