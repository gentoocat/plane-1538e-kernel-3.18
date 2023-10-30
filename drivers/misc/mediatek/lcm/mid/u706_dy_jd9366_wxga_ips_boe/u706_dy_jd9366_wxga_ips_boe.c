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
#define FRAME_WIDTH  (720)
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


#define USING_TABLE 1


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

static struct LCM_setting_table lcm_initialization_setting[] =
{
	//JD9366 initial code
//Page0
	{0xE0, 1,{0x00}},

//--- PASSWORD  ----//
	{0xE1, 1,{0x93}},
	{0xE2, 1,{0x65}},
	{0xE3, 1,{0xF8}},
	{0x80, 1,{0x03}},         //0x03=4lanes //0x02=3lanes

//Page4
	{0xE0, 1,{0x04}},
	{0x2D, 1,{0x03}},         //4lanes
	
//--- Page1  ----//
	{0xE0, 1,{0x01}},
//	{0x4a, 1,{0x35}},    //bist mode open
	
//Set VCOM
	{0x00, 1,{0x00}},
	{0x01, 1,{0xA8}},	//VCOM A8 AE 
//Set VCOM_Reverse
	{0x03, 1,{0x00}},
	{0x04, 1,{0xB0}},	

//Set Gamma Power, VGMP,VGMN,VGSP,VGSN
	{0x17, 1,{0x00}},
	{0x18, 1,{0xB1}},
	{0x19, 1,{0x01}},
	{0x1A, 1,{0x00}},
	{0x1B, 1,{0xB1}},  //VGMN=0
	{0x1C, 1,{0x01}},
                
//Set Gate Power
	{0x1F, 1,{0x3E}},     //VGH_R  = 15V                       
	{0x20, 1,{0x28}},     //VGL_R  = -11V                      
	{0x21, 1,{0x28}},     //VGL_R2 = -11V                      
	{0x22, 1,{0x0E}},     //PA[6]=0, PA[5]=0, PA[4]=0, PA[0]=0 

//SETPANEL
	{0x37, 1,{0x19}},	//SS=1,BGR=1  //19

//SET RGBCYC
	{0x38, 1,{0x05}},	//JDT=101 zigzag inversion
	{0x39, 1,{0x08}},	//RGB_N_EQ1, modify 20140806
	{0x3A, 1,{0x12}},	//RGB_N_EQ2, modify 20140806
	{0x3C, 1,{0x78}},	//SET EQ3 for TE_H
	{0x3D, 1,{0xFF}},	//SET CHGEN_OFF, modify 20140827 
	{0x3E, 1,{0xFF}},	//SET CHGEN_OFF, modify 20140827 
	{0x3F, 1,{0xFF}},	//SET CHGEN_OFF2, modify 20140827


//Set TCON
	{0x40, 1,{0x04}},	//RSO=720 RGB
	{0x41, 1,{0xA0}},	//LN=640->1280 line
	{0x4B, 1,{0x04}},	//Source EQ iption

//--- power voltage  ----//
	{0x55, 1,{0x0F}},	//DCDCM=0001, JD PWR_IC
	{0x56, 1,{0x01}},
	{0x57, 1,{0x89}},	//VGH_RT=100, Charge pump Ratio:3*AVDD-VCL
	{0x58, 1,{0x0A}},
	{0x59, 1,{0x0A}},	//VCL = -2.9V
	{0x5A, 1,{0x28}},	//VGH = 15V
	{0x5B, 1,{0x14}},	//VGL = -11V


//modify 20160616
//--- Gamma  ----//
	{0x5D, 1,{0x7C}}, // 
	{0x5E, 1,{0x66}}, //
	{0x5F, 1,{0x57}}, //
	{0x60, 1,{0x4A}}, //
	{0x61, 1,{0x46}}, //
	{0x62, 1,{0x36}}, //
	{0x63, 1,{0x3A}}, //
	{0x64, 1,{0x23}}, //
	{0x65, 1,{0x3A}}, //
	{0x66, 1,{0x38}}, //
	{0x67, 1,{0x37}}, //
	{0x68, 1,{0x54}}, //
	{0x69, 1,{0x41}}, //
	{0x6A, 1,{0x48}}, //
	{0x6B, 1,{0x39}}, //
	{0x6C, 1,{0x34}}, //
	{0x6D, 1,{0x27}}, //
	{0x6E, 1,{0x16}}, //
	{0x6F, 1,{0x00}}, //
	{0x70, 1,{0x7C}}, //
	{0x71, 1,{0x66}}, //
	{0x72, 1,{0x57}}, //
	{0x73, 1,{0x4A}}, //
	{0x74, 1,{0x46}}, //
	{0x75, 1,{0x36}}, //
	{0x76, 1,{0x3A}}, //
	{0x77, 1,{0x23}}, //
	{0x78, 1,{0x3A}}, //
	{0x79, 1,{0x38}}, //
	{0x7A, 1,{0x37}}, //
	{0x7B, 1,{0x54}}, //
	{0x7C, 1,{0x41}}, //
	{0x7D, 1,{0x48}}, //
	{0x7E, 1,{0x39}}, //
	{0x7F, 1,{0x34}}, //
	{0x80, 1,{0x27}}, //
	{0x81, 1,{0x16}}, //
	{0x82, 1,{0x00}}, //


//Page2, for GIP
	{0xE0, 1,{0x02}},

//GIP_L Pin mapping
	{0x00, 1,{0x47}},
	{0x01, 1,{0x47}},  
	{0x02, 1,{0x45}},
	{0x03, 1,{0x45}},
	{0x04, 1,{0x4B}},
	{0x05, 1,{0x4B}},
	{0x06, 1,{0x49}},
	{0x07, 1,{0x49}},
	{0x08, 1,{0x41}},
	{0x09, 1,{0x1F}},
	{0x0A, 1,{0x1F}},
	{0x0B, 1,{0x1F}},
	{0x0C, 1,{0x1F}},
	{0x0D, 1,{0x1F}},
	{0x0E, 1,{0x1F}},
	{0x0F, 1,{0x43}},
	{0x10, 1,{0x1F}},
	{0x11, 1,{0x1F}},
	{0x12, 1,{0x1F}},
	{0x13, 1,{0x1F}},
	{0x14, 1,{0x1F}},
	{0x15, 1,{0x1F}},

//GIP_R Pin mapping
	{0x16, 1,{0x46}},
	{0x17, 1,{0x46}},
	{0x18, 1,{0x44}},
	{0x19, 1,{0x44}},
	{0x1A, 1,{0x4A}},
	{0x1B, 1,{0x4A}},
	{0x1C, 1,{0x48}},
	{0x1D, 1,{0x48}},
	{0x1E, 1,{0x40}},
	{0x1F, 1,{0x1F}},
	{0x20, 1,{0x1F}},
	{0x21, 1,{0x1F}},
	{0x22, 1,{0x1F}},
	{0x23, 1,{0x1F}},
	{0x24, 1,{0x1F}},
	{0x25, 1,{0x42}},
	{0x26, 1,{0x1F}},
	{0x27, 1,{0x1F}},
	{0x28, 1,{0x1F}},
	{0x29, 1,{0x1F}},
	{0x2A, 1,{0x1F}},
	{0x2B, 1,{0x1F}},
                      
//GIP_L_GS Pin mapping
	{0x2C, 1,{0x48}}, 
	{0x2D, 1,{0x48}}, 
	{0x2E, 1,{0x4A}}, 
	{0x2F, 1,{0x4A}}, 
	{0x30, 1,{0x44}}, 
	{0x31, 1,{0x44}}, 
	{0x32, 1,{0x46}}, 
	{0x33, 1,{0x46}}, 
	{0x34, 1,{0x42}}, 
	{0x35, 1,{0x1F}}, 
	{0x36, 1,{0x1F}}, 
	{0x37, 1,{0x1F}}, 
	{0x38, 1,{0x1F}}, 
	{0x39, 1,{0x1F}}, 
	{0x3A, 1,{0x1F}}, 
	{0x3B, 1,{0x40}}, 
	{0x3C, 1,{0x1F}}, 
	{0x3D, 1,{0x1F}}, 
	{0x3E, 1,{0x1F}}, 
	{0x3F, 1,{0x1F}}, 
	{0x40, 1,{0x1F}}, 
	{0x41, 1,{0x1F}}, 
 
//GIP_R_GS Pin mapping
	{0x42, 1,{0x49}},
	{0x43, 1,{0x49}},
	{0x44, 1,{0x4B}},
	{0x45, 1,{0x4B}},
	{0x46, 1,{0x45}},
	{0x47, 1,{0x45}},
	{0x48, 1,{0x47}},
	{0x49, 1,{0x47}},
	{0x4A, 1,{0x43}},
	{0x4B, 1,{0x1F}},
	{0x4C, 1,{0x1F}},
	{0x4D, 1,{0x1F}},
	{0x4E, 1,{0x1F}},
	{0x4F, 1,{0x1F}},
	{0x50, 1,{0x1F}},
	{0x51, 1,{0x41}},
	{0x52, 1,{0x1F}},
	{0x53, 1,{0x1F}},
	{0x54, 1,{0x1F}},
	{0x55, 1,{0x1F}},
	{0x56, 1,{0x1F}},
	{0x57, 1,{0x1F}},

//GIP Timing  
	{0x58, 1,{0x40}},
//	{0x59, 1,{0x00}},
//	{0x5A, 1,{0x00}},
	{0x5B, 1,{0x30}},
	{0x5C, 1,{0x04}},
	{0x5D, 1,{0x40}},//60
	{0x5E, 1,{0x01}},
	{0x5F, 1,{0x02}},
//	{0x60, 1,{0x00}},
//	{0x61, 1,{0x01}},
//	{0x62, 1,{0x02}},
	{0x63, 1,{0x62}},
	{0x64, 1,{0x62}},
//	{0x65, 1,{0x00}},
//	{0x66, 1,{0x00}},
	{0x67, 1,{0x74}},
	{0x68, 1,{0x06}},
	{0x69, 1,{0x62}},
	{0x6A, 1,{0x66}}, //66 CLK Falling to Source Falling time 3us
	{0x6B, 1,{0x08}},
	{0x6C, 1,{0x00}},
	{0x6D, 1,{0x00}}, //GEQ_GGND1
	{0x6E, 1,{0x00}}, //GEQ_GGND2
	{0x6F, 1,{0x08}},

//	{0x70, 1,{0x00}},
//	{0x71, 1,{0x00}},
//	{0x72, 1,{0x06}},
//	{0x73, 1,{0x7B}},
//	{0x74, 1,{0x00}},
//	{0x75, 1,{0x07}},
//	{0x76, 1,{0x00}},
//	{0x77, 1,{0x5D}},
//	{0x78, 1,{0x17}},
//	{0x79, 1,{0x1F}},
//	{0x7A, 1,{0x00}},
//	{0x7B, 1,{0x00}},
//	{0x7C, 1,{0x00}},
//	{0x7D, 1,{0x03}},
//	{0x7E, 1,{0x7B}},



//Page3
	{0xE0, 1,{0x03}},
	{0x98, 1,{0x3E}},	//From 2E to 3E, LED_ON


//Page4
	{0xE0, 1,{0x04}},
	{0x09, 1,{0x14}},	
	{0x2B, 1,{0x2B}},	
	{0x2E, 1,{0x44}},	


// Add ESD Protect
//Page0
	{0xE0, 1,{0x00}},
	{0xE6, 1,{0x02}},
	{0xE7, 1,{0x06}},


	{0x11, 1,{0x00}},
	{REGFLAG_DELAY, 200, {}},	
	{0x29, 1,{0x00}},
	{REGFLAG_DELAY, 20, {}},   

	{REGFLAG_END_OF_TABLE, 0x00, {}}
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
	params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
		// Video mode setting		
	//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	  params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 4;//1// by eric.wang
		params->dsi.vertical_backporch					= 6;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 8;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 20;//10
		params->dsi.horizontal_backporch				= 20;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 48;//60 by eric.wang 160
	//	params->dsi.horizontal_blanking_pixel = 60;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		// Bit rate calculation
		// Every lane speed
		//params->dsi.pll_div1=0;				// div1=0,1,2,3;div1_real=1,2,4,4
		//params->dsi.pll_div2=0;				// div2=0,1,2,3;div1_real=1,2,4,4	
		//params->dsi.fbk_div =0x12;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	
		//params->dsi.cont_clock 	= 1;//1
		//params->dsi.ssc_disable = 0;//0
		params->dsi.PLL_CLOCK 	= 200;
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
	MDELAY(120);

#if (USING_TABLE)
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
#else
	lcm_initial_registers();
#endif
	MDELAY(120);
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




LCM_DRIVER u706_dy_jd9366_wxga_ips_boe_lcm_drv = 
{
  .name				= "u706_dy_jd9366_wxga_ips_boe",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
