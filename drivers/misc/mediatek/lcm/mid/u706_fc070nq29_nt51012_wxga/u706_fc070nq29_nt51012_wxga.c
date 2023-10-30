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

#define GPIO_LCD_PWR_EN     (GPIO64 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (GPIO120 | 0x80000000)//GPIO_LCM_BL_EN// GPIO120
//#define GPIO_LCD_BL_EN      0xffffffff
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)

#define GPIO_LCD_RST      (GPIO146 | 0x80000000)



#define GPIO_LCM_STB       (GPIO147 | 0x80000000)


#define USING_TABLE 0


//#define GPIO_LCD_PWR_EN 	 GPIO_LCM_PWR_EN
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	  lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)				lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)																			lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)									lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)																				lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   						lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE	0
#define REGFLAG_DELAY       0XFFE
#define REGFLAG_END_OF_TABLE    0xFFF   // END OF REGISTERS MARKER


struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
#if (USING_TABLE)
static struct LCM_setting_table lcm_initialization_setting[] =
{
	{REGFLAG_DELAY,150,{}},
	{0x28,0,{}},
	{REGFLAG_DELAY,150,{}},
	{0x01,0,{}},
	{REGFLAG_DELAY,150,{}},
	{0xae,1,{0x0b}},
	{0xee,1,{0xea}},
	{0xef,1,{0x5f}},
	{0xf2,1,{0x68}},
	{0xee,1,{0x00}},
	{0xef,1,{0x00}},
//{0xb1,1,{0xdf}},//BTST TEST
	{0x29,0,{}},
	{REGFLAG_DELAY,150,{}},
	{0x11,0,{}},
	{REGFLAG_DELAY,20,{}},
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }   
    }   

}
#else
static void lcm_initial_registers(void)
{
	unsigned int data_array[16];

	MDELAY(150);
	data_array[0] = 0x00280500;  //close display                     
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);
	
	data_array[0] = 0x00010500;  //reset                    
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);		
	
	data_array[0]= 0x0bae1500;
	dsi_set_cmdq(data_array, 1, 1);

	data_array[0]= 0xeaee1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x5fef1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x68f21500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x00ee1500;
	dsi_set_cmdq(data_array, 1, 1);	

	data_array[0]= 0x00ef1500;	//open display
	dsi_set_cmdq(data_array, 1, 1);

	//data_array[0]= 0xdfb11500;	//BTST test
	//dsi_set_cmdq(data_array, 1, 1);
		
	data_array[0] = 0x00290500;  //reset                    
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);


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
		
    #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
    #else
		params->dsi.mode   = BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	  params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;
		//params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		#if 1
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
		#endif
		// Highly depends on LCD driver capability.
		// Not support in MT6573
		params->dsi.packet_size=256;

		// Video mode setting		
	//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	    params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //   params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 1;//1// by eric.wang
		params->dsi.vertical_backporch					= 10;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 16;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 1;//10
		params->dsi.horizontal_backporch				= 80;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 32;//60 by eric.wang 160
	//	params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		// Bit rate calculation
		// Every lane speed
		//params->dsi.pll_div1=0;				// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0;				// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
	//	params->dsi.cont_clock 	= 1;//1
	//	params->dsi.ssc_disable = 0;//0
		params->dsi.PLL_CLOCK 	= 214;//170 by eric.wang
	//params->dsi.clk_lp_per_line_enable   = 1;//0
		
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	if(GPIO==0xffffffff){
		return ;
	}
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}


static void lcm_reset(int value)
{
	lcm_set_gpio_output(GPIO_LCD_RST, value);
}

static void lcm_standby(int value)
{
	lcm_set_gpio_output(GPIO_LCM_STB, value);
}

static void lcm_power_en(int value)
{
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, value);
}

static void lcm_bl_en(int value)
{
	lcm_set_gpio_output(GPIO_LCD_BL_EN, value);
}


static void lcm_init(void)
{
	lcm_power_en(0);
	lcm_power_en(1);
	MDELAY(20);
	lcm_standby(1);

	lcm_reset(1);
	MDELAY(20);
	lcm_reset(0);	
	MDELAY(20);
	lcm_reset(1);

#if (USING_TABLE)
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
#else
	lcm_initial_registers();
#endif
	lcm_bl_en(1);
}



static void lcm_suspend(void)
{
	lcm_bl_en(0);
	MDELAY(10);	
	lcm_standby(0);
	MDELAY(10);	
	lcm_reset(0);	
	MDELAY(10);	
	lcm_power_en(0);
	MDELAY(10);	
}

static void lcm_resume(void)
{
	lcm_init();
}




LCM_DRIVER u706_fc070nq29_nt51012_wxga_lcm_drv = 
{
  .name				= "u706_fc070nq29_nt51012_wxga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
