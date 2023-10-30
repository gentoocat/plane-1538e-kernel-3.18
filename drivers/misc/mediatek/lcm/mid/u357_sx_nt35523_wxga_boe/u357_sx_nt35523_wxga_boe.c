#ifndef BUILD_LK
#include <linux/string.h>
#else
#include <string.h>
#endif 

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#elif (defined BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
	#include "upmu_common.h"
    #include <mt-plat/mt_gpio.h>
	#include <mach/gpio_const.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (800)
#define FRAME_HEIGHT (1280)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))
#if 0
#ifdef GPIO_LCM_PWR_EN
#define GPIO_LCD_PWR_EN      GPIO_LCM_PWR_EN
#else
#define GPIO_LCD_PWR_EN      0xFFFFFFFF
#endif

#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST      GPIO_LCM_RST
#else
#define GPIO_LCD_RST      0xFFFFFFFF
#endif

#ifdef GPIO_LCD_BL_EN
#define GPIO_LCM_BL_EN      GPIO_LCD_BL_EN
#else
#define GPIO_LCM_BL_EN      0xFFFFFFFF
#endif
#else

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCM_BL_EN     	(GPIO82 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCD_RST      	(GPIO146 | 0x80000000)
#endif

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
//static const unsigned char LCD_MODULE_ID = 0x01; //  haobing modified 2013.07.11
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define   LCM_DSI_CMD_MODE	0
#define REGFLAG_DELAY       0XFE
#define REGFLAG_END_OF_TABLE    0xFF   // END OF REGISTERS MARKER



// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

#if 0
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//update initial param for IC nt35523 0.01
static struct LCM_setting_table lcm_suspend_setting[] = {
#if 1
	{0x28,0,{}},
	{REGFLAG_DELAY, 80, {}},
	{0xF0,2,{0x5A,0x5A}},
	{0xF1,2,{0x5A,0x5A}},
	{0xC3,3,{0x40,0x00,0x20}},
	{0x10,0,{}},
	{REGFLAG_DELAY, 34, {}}
#endif

#if 0
	{0x28,0,{}},
	{0x10,0,{}},
	{REGFLAG_DELAY, 120, {}}
#endif
};
//add by eric.wang
static struct LCM_setting_table lcm_resume_setting[] = {

	{0xF0,2,{0x5A,0x5A}},
	{0xF1,2,{0x5A,0x5A}},

	{0xC3,3,{0x40,0x00,0x28}},
	{REGFLAG_DELAY_MS_V3, 20, {}}, 

	{0x11,0,{}},
	{REGFLAG_DELAY_MS_V3, 120, {}}, 
	
	{0xF0,2,{0xA5,0xA5}},
	{0xF1,2,{0xA5,0xA5}},
	{0xFC,2,{0x5A,0x5A}},
	{0x29,0,{}},
	{REGFLAG_DELAY_MS_V3, 200, {}}

};	
//update initial param for IC nt35523 0.01

static struct LCM_setting_table lcm_initialization_setting[] = {
// page 1
	/*{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
	{0xCA, 1, {0x00}},
	{0xCE, 1, {0x34}},
	{0xBC, 2, {0x68,0x01}},
	{0xBD, 2, {0x68,0x01}},

// page 3 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x03}},
	{0xB0, 4, {0x00,0x23,0x00,0x00}},
	{0xB2, 7, {0x01,0x00,0x06,0x04,0x00,0xD8,0x42}},
	{0xB3, 7, {0x01,0x00,0x05,0x04,0x00,0xD8,0x42}},
	{0xBA, 7, {0x85,0x03,0x00,0x04,0x01,0xD8,0x42}},
	{0xBB, 7, {0x85,0x03,0x00,0x03,0x01,0xD8,0x42}},

// page 2 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x02}},
	{0xB0, 1, {0x40}},
	{0xD1, 16, {0x00,0x00,0x00,0x12,0x00,0x3C,0x00,0x55,0x00,0x78,0x00,0x9D,0x00,0xBE,0x00,0xF0}},
	{0xD2, 16, {0x01,0x18,0x01,0x59,0x01,0x8E,0x01,0xDD,0x02,0x1F,0x02,0x21,0x02,0x5D,0x02,0x9E}},
	{0xD3, 16, {0x02,0xC6,0x03,0x00,0x03,0x26,0x03,0x59,0x03,0x7A,0x03,0xA2,0x03,0xBB,0x03,0xD8}},
	{0xD4, 4, {0x03,0xF6,0x03,0xFF}},

// page 6 
	{0xF0, 5, {0x55,0xAA,0x52,0x08,0x06}},
	{0xB3, 1, {0x03}},
	{0xB1, 5, {0x19,0x18,0x18,0x02,0x3A}},
	{0xB2, 2, {0x3A,0x3A}},
	{0xB6, 5, {0x13,0x12,0x12,0x00,0x3A}},*/
	
	/*{0xF0, 5, {0x55,0xAA,0x52,0x08,0x01}},
		//{0xCA, 1, {0x00}},
		{0xCE, 1, {0x04}},
		{0xBC, 2, {0x68,0x01}},
		{0xBD, 2, {0x68,0x01}},
	
	// page 3 
		{0xFF, 4, {0xAA,0x55,0xA5,0x80}},
		{0x6F, 1, {0x09}},
		{0xF7, 1, {0x82}},
		{0x6F, 1, {0x0B}},
		{0xF7, 1, {0xE0}},
		{0x55, 1, {0x81},
	
	// page 2 
		{0x53, 1, {0x2C}},
		{0x51, 1, {0xFF}},*/

#if 1 //for sangsumg panle by eric.wang
	
	{0xF0,	2,	{0x5A, 0x5A}},//page 0
	{0xF1,	2,	{0x5A, 0x5A}},
	{0xFC,	2,	{0xA5, 0xA5}},
	{0xD0,	2,	{0x00, 0x10}},
	{0x35,	0,	{}},
	{0xC3,	3,	{0x40,0x00,0x28}},
	{REGFLAG_DELAY, 20, {}},
	{0xF6,	6,	{0x63, 0x20, 0x86, 0x00, 0x00, 0x10}},
	{0x11,	0,	{}},
	{REGFLAG_DELAY, 150, {}},
	{0x36,	1,	{0x00}},
	{0x29,	0,	{}},
	{REGFLAG_DELAY, 200, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}},

#endif

#if 0
	{0xFF,5,{0xFF,0x98,0x07,0x00,0x01}},
	{0x31,1,{0x02}},
	{0x40,1,{0xD0}},
	{0x41,1,{0x55}},
	{0x42,1,{0x02}},
	{0x43,1,{0x09}},
	{0x44,1,{0x06}},
	{0x46,1,{0xbb}},
	{0x50,1,{0x78}},
	{0x51,1,{0x78}},
	{0x52,1,{0x00}},
	{0x53,1,{0x44}},
					 
	{0x60,1,{0x05}},
	{0x61,1,{0x08}},
	{0x62,1,{0x08}},
	{0x63,1,{0x08}},
	{0x64,1,{0x88}},
	
	{0xA0,1,{0x00}},
	{0xA1,1,{0x07}},
	{0xA2,1,{0x0E}},
	{0xA3,1,{0x0B}},
	{0xA4,1,{0x04}},
	{0xA5,1,{0x08}},
	{0xA6,1,{0x06}},
	{0xA7,1,{0x04}},
	{0xA8,1,{0x07}},
	{0xA9,1,{0x0B}},
	{0xAA,1,{0x12}},
	{0xAB,1,{0x08}},
	{0xAC,1,{0x0E}},
	{0xAD,1,{0x19}},
	{0xAE,1,{0x10}},
	{0xAF,1,{0x00}},
	{0xC0,1,{0x00}},
	{0xC1,1,{0x07}},
	{0xC2,1,{0x0E}},
	{0xC3,1,{0x0A}},
	{0xC4,1,{0x04}},
	{0xC5,1,{0x09}},
	{0xC6,1,{0x06}},
	{0xC7,1,{0x04}},
	{0xC8,1,{0x08}},
	{0xC9,1,{0x0B}},
	{0xCA,1,{0x12}},
	{0xCB,1,{0x08}},
	{0xCC,1,{0x0E}},
	{0xCD,1,{0x19}},
	{0xCE,1,{0x10}},
	{0xCF,1,{0x00}},
				
	{0xFF,5,{0xFF,0x98,0x07,0x00,0x06}}, 
	{0x00,1,{0x21}}, 
	{0x58,1,{0x5b}},									 
	{0x01,1,{0x0A}},										
	{0x02,1,{0x00}},										 
	{0x03,1,{0x00}},										 
	{0x04,1,{0x0B}},										
	{0x05,1,{0x01}},										
	{0x06,1,{0x80}},										
	{0x07,1,{0x06}},										
	{0x08,1,{0x01}},										 
	{0x09,1,{0x00}},										 
	{0x0A,1,{0x00}},										 
	{0x0B,1,{0x00}},										 
	{0x0C,1,{0x0B}},										 
	{0x0D,1,{0x0B}},										 
	{0x10,1,{0xF0}},										 
	{0x11,1,{0xF4}},										 
	{0x12,1,{0x04}},										
	{0x13,1,{0x00}},										
	{0x14,1,{0x00}},										
	{0x15,1,{0xC0}},										
	{0x16,1,{0x08}},										
	{0x17,1,{0x00}},										
	{0x18,1,{0x00}},										
	{0x19,1,{0x00}},										
	{0x1A,1,{0x00}},										
	{0x1B,1,{0x00}},										
	{0x1C,1,{0x00}},										
	{0x1D,1,{0x00}},										
	{0x20,1,{0x01}},										
	{0x21,1,{0x23}},										
	{0x22,1,{0x45}},										
	{0x23,1,{0x67}},										
	{0x24,1,{0x01}},										
	{0x25,1,{0x23}},										
	{0x26,1,{0x45}},										
	{0x27,1,{0x67}},										
	{0x30,1,{0x01}},										
	{0x31,1,{0x11}},										
	{0x32,1,{0x00}},										
	{0x33,1,{0xEE}},										
	{0x34,1,{0xFF}},										
	{0x35,1,{0x22}},										
	{0x36,1,{0xCB}},										
	{0x37,1,{0x22}},										
	{0x38,1,{0xDA}},										
	{0x39,1,{0x22}},										
	{0x3A,1,{0xAD}},										
	{0x3B,1,{0x22}},										
	{0x3C,1,{0xBC}},										
	{0x3D,1,{0x76}},										
	{0x3E,1,{0x67}},										
	{0x3F,1,{0x22}},										
	{0x40,1,{0x22}},										
	{0x41,1,{0x22}},										
	{0x42,1,{0x22}},										
	{0x43,1,{0x22}},										
									   
	{0xFF,5,{0xFF,0x98,0x07,0x00,0x07}},
	{0x02,1,{0x77}},						   
	{0x18,1,{0x1D}},											  
	{0x80,1,{0x04}}, 
	{0xe5,1,{0x5f}},									   
													   
	{0xFF,5,{0xFF,0x98,0x07,0x00,0x00}},
	{0x35,1,{0x00}},			
											   
	{0x3A,1,{0x77}},										
	{0x11,1,{0x00}},					  
	{REGFLAG_DELAY_MS_V3, 150, {}},															   
	{0x29,1,{0x00}},									
	{REGFLAG_DELAY_MS_V3, 50, {}},
#endif
#if 0
	{0x11, 0, {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29, 0, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
#endif
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
//	params->physical_width  = PHYSICAL_WIDTH;
//  params->physical_height = PHYSICAL_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE ,SYNC_EVENT_VDO_MODE
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      		= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	//params->dsi.packet_size=256;
	//video mode timing
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    //   params->dsi.word_count=FRAME_WIDTH*3;


	params->dsi.vertical_sync_active				= 4;//4
	params->dsi.vertical_backporch					= 4;//4
	params->dsi.vertical_frontporch					= 8;//8
	params->dsi.vertical_active_line					= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 14;//16
	params->dsi.horizontal_backporch				= 120;//48
	params->dsi.horizontal_frontporch				= 16;//16
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
    	params->dsi.ssc_disable							= 1;
	params->dsi.cont_clock 	= 0;//0
	params->dsi.PLL_CLOCK = 180 ;//56//220
	params->dsi.clk_lp_per_line_enable   = 1;//0
 
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
#if 0
static void lcm_init_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_init_power() enter\n");
	//lcm_set_gpio_output(GPIO_BL_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	
	MDELAY(20);
		
#else
	printk("[Kernel/LCM] lcm_init_power() enter\n");
	//lcm_set_gpio_output(GPIO_BL_EN, GPIO_OUT_ONE);
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
#endif
static void lcd_reset(unsigned char enabled)
{

    if (enabled)
    {   
        lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
    }   
    else
    {   
        lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);    
    }   

}


#if 0
static void lcm_resume_power(void)
{
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume_power() enter\n");
	 lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(50);	
			
#else
	printk("[Kernel/LCM] lcm_resume_power() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	MDELAY(50);	
		
#endif

}
#endif
static void init_lcm_registers(void)
{		
	 unsigned int data_array[16];
    MDELAY(20);
   data_array[0]=0x00033902;
    data_array[1]=0x005A5AF0;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00033902;
    data_array[1]=0x005A5AF1;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00033902;
    data_array[1]=0x00A5A5FC;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00033902;
    data_array[1]=0x001000D0;
    dsi_set_cmdq(data_array, 2, 1); // 
    

    data_array[0] = 0x00043902;  //Enable 	
    data_array[1] = 0x280040C3;			 
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(20);
    

    data_array[0] = 0x04361500;
    dsi_set_cmdq(data_array, 1, 1);

    data_array[0]=0x00073902;
    data_array[1]=0x862063F6;
    data_array[2]=0x00100000;
    dsi_set_cmdq(data_array, 3, 1);  

    data_array[0] = 0x00110500;  //sleep out                        
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);
    
    data_array[0] = 0x00361500;
    dsi_set_cmdq(data_array, 1, 1);

    data_array[0]=0x00033902;
    data_array[1]=0x00A5A5F0;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00033902;
    data_array[1]=0x00A5A5F1;
    dsi_set_cmdq(data_array, 2, 1);
   
    data_array[0]=0x00033902;
    data_array[1]=0x005A5AFC;
    dsi_set_cmdq(data_array, 2, 1);
   


    data_array[0] = 0x00290500;  //display on                        
    dsi_set_cmdq(data_array, 1, 1);
    
    MDELAY(200);


	}

static void lcm_suspend_registers(void)
{
       unsigned int data_array[16];
    MDELAY(200);
    data_array[0] = 0x00280500;  //display off                        
    dsi_set_cmdq(data_array, 1, 1); 
    MDELAY(100);
	
    data_array[0]=0x00033902;
    data_array[1]=0x005A5AF0;
    dsi_set_cmdq(data_array, 2, 1);
    
    data_array[0]=0x00033902;
    data_array[1]=0x005A5AF1;
    dsi_set_cmdq(data_array, 2, 1);
     
    data_array[0] = 0x00043902;  //software reset	
    data_array[1] = 0x200040C3;			 
    dsi_set_cmdq(data_array, 2, 1);
    MDELAY(20);
    
    data_array[0] = 0x00100500;  //display off                        
    dsi_set_cmdq(data_array, 1, 1); 
    
    MDELAY(34);
}


static void lcm_init(void)
{ 
#ifdef BUILD_LK 
	//printf("[LK/LCM] lcm_init() enter\n");
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
    lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	//SET_RESET_PIN(0);
	lcd_reset(0);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	
	MDELAY(50);
	//SET_RESET_PIN(1);
	lcd_reset(1);
	MDELAY(20);//200
	//d_reset(0);
	//SET_RESET_PIN(0); 
	lcd_reset(0);
	//SET_RESET_PIN(1);
	//ELAY(50);//200
	//d_reset(1);
	//ELAY(50);//200
	MDELAY(50);
	// when phone initial , config output high, enable backlight drv chip  
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
       lcd_reset(1);
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(200);
	  init_lcm_registers();
	
	
	//printf("[LK/LCM] lcm_init() end\n");
#else
    lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);

	//SET_RESET_PIN(0);
	lcd_reset(0);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	
	MDELAY(50);
	//SET_RESET_PIN(1);
	lcd_reset(1);
	MDELAY(20);//200
	//d_reset(0);
	//SET_RESET_PIN(0); 
	lcd_reset(0);
	//SET_RESET_PIN(1);
	//ELAY(50);//200
	//d_reset(1);
	//ELAY(50);//200
	MDELAY(50);
	// when phone initial , config output high, enable backlight drv chip  
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
       lcd_reset(1);
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(200);
	  init_lcm_registers();
	



#endif

}

static void lcm_suspend(void)
{
#ifdef BUILD_LK
	//printf("[LK/LCM] lcm_suspend() enter\n");

	//m_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	//MDELAY(200);
	//push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  
	lcm_suspend_registers();
	//SET_RESET_PIN(0);
	MDELAY(10);
	lcd_reset(0);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);
#else
	//printf("[LK/LCM] lcm_suspend() enter\n");

	//m_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	//MDELAY(200);
	//push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);  
	lcm_suspend_registers();
	//SET_RESET_PIN(0);
	MDELAY(10);
	lcd_reset(0);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN,GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN,GPIO_OUT_ZERO);
	MDELAY(10);

#endif
}

static void lcm_resume(void)
{
#if 0
#ifdef BUILD_LK 
	printf("[LK/LCM] lcm_resume() enter\n");
lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
MDELAY(50);	
	SET_RESET_PIN(1);
	MDELAY(20);
	
	SET_RESET_PIN(0);
	MDELAY(30);
	
	SET_RESET_PIN(1);
	MDELAY(20);
	
	// when phone initial , config output high, enable backlight drv chip  
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	 //init_lcm_registers();
	MDELAY(10);
	
#else
	printk("[Kernel/LCM] lcm_resume() enter\n");
 lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
 lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
 MDELAY(50);	
	SET_RESET_PIN(1);
	MDELAY(10);
	
	SET_RESET_PIN(0);
	MDELAY(10);
	
	SET_RESET_PIN(1);
	MDELAY(10);
	
	// when phone initial , config output high, enable backlight drv chip  
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

//	lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	 //init_lcm_registers();
	MDELAY(50);
	printk("[Kernel/LCM] lcm_resume() end\n");
#endif
#endif 
//m_init();
//d_reset(0);
//m_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
//cm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	//m_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
// lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	//SET_RESET_PIN(0);
	lcd_reset(0);
	//lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	
	MDELAY(50);
	//SET_RESET_PIN(1);
	lcd_reset(1);
	MDELAY(20);//200
	//d_reset(0);
	//SET_RESET_PIN(0); 
	lcd_reset(0);

	MDELAY(2);//10
       lcd_reset(1);

	//SET_RESET_PIN(1);
	//ELAY(50);//200
	//d_reset(1);
	//ELAY(50);//200
	//MDELAY(1);
	// when phone initial , config output high, enable backlight drv chip  
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
      // lcd_reset(1);
	//lcm_set_gpio_output(GPIO_LCD_BL_EN, GPIO_OUT_ONE);
	MDELAY(10);
	  init_lcm_registers();
	//  MDELAY(100);
	
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);	
}
#endif
#if 0
static unsigned int lcm_compare_id(void)
{

}
#endif
LCM_DRIVER u357_sx_nt35523_wxga_boe_lcm_drv=
{
    .name           	= "u357_sx_nt35523_wxga_boe",
    .set_util_funcs 	= lcm_set_util_funcs,
    .get_params     	= lcm_get_params,
    .init           		= lcm_init, 
    .suspend        	= lcm_suspend,
    .resume         	= lcm_resume,
     //.compare_id     	= lcm_compare_id,
   //  .init_power		= lcm_init_power,
    // .resume_power = lcm_resume_power,
   // .suspend_power = lcm_suspend_power,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
   
};
