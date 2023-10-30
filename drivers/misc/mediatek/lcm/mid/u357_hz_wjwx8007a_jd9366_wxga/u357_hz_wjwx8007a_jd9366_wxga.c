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
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	  lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)				lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)																			lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)									lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)																				lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   						lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   

#define   LCM_DSI_CMD_MODE	0
#define REGFLAG_DELAY       0XFE
#define REGFLAG_END_OF_TABLE    0xFF   // END OF REGISTERS MARKER


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
{0xE0,1,{0x00}},

//--- PASSWORD  ----//
{0xE1,1,{0x93}},
{0xE2,1,{0x65}},
{0xE3,1,{0xF8}},

//Page0
{0xE0,1,{0x00}},
//--- Sequence Ctrl  ----//
{0x70,1,{0x10}},
{0x71,1,{0x13}},
{0x72,1,{0x06}},

//--- Page1  ----//
{0xE0,1,{0x01}},

//Set VCOM
{0x00,1,{0x00}},
{0x01,1,{0xA0}},
//Set VCOM_Reverse
{0x03,1,{0x00}},
{0x04,1,{0xA0}},

//Set Gamma Power, VGMP,VGMN,VGSP,VGSN
{0x17,1,{0x00}},
{0x18,1,{0xB1}},
{0x19,1,{0x01}},
{0x1A,1,{0x00}},
{0x1B,1,{0xB1}},
{0x1C,1,{0x01}},
       
//Set Gate Power
{0x1F,1,{0x3E}},
{0x20,1,{0x2D}},
{0x21,1,{0x2D}},
{0x22,1,{0x0E}},

//SETPANEL
{0x37,1,{0x19}},	
//SET RGBCYC
{0x38,1,{0x05}},	
{0x39,1,{0x08}},	
{0x3A,1,{0x12}},	
{0x3C,1,{0x78}},	
{0x3E,1,{0x80}},	
{0x3F,1,{0x80}},	

//Set TCON
{0x40,1,{0x06}},	
{0x41,1,{0xA0}},	

//--- power voltage  ----//
{0x55,1,{0x01}},	
{0x56,1,{0x01}},
{0x57,1,{0x69}},
{0x58,1,{0x0A}},
{0x59,1,{0x0A}},
{0x5A,1,{0x28}},
{0x5B,1,{0x19}},

//--- Gamma  ----//
{0x5D,1,{0x7C}},
{0x5E,1,{0x60}},
{0x5F,1,{0x50}},
{0x60,1,{0x43}},
{0x61,1,{0x3F}},
{0x62,1,{0x30}},
{0x63,1,{0x34}},
{0x64,1,{0x1E}},
{0x65,1,{0x37}},
{0x66,1,{0x35}},
{0x67,1,{0x34}},
{0x68,1,{0x52}},
{0x69,1,{0x3F}},
{0x6A,1,{0x44}},
{0x6B,1,{0x35}},
{0x6C,1,{0x2F}},
{0x6D,1,{0x21}},
{0x6E,1,{0x10}},
{0x6F,1,{0x00}},
{0x70,1,{0x7C}},
{0x71,1,{0x60}},
{0x72,1,{0x50}},
{0x73,1,{0x43}},
{0x74,1,{0x3F}},
{0x75,1,{0x30}},
{0x76,1,{0x34}},
{0x77,1,{0x1E}},
{0x78,1,{0x37}},
{0x79,1,{0x35}},
{0x7A,1,{0x34}},
{0x7B,1,{0x52}},
{0x7C,1,{0x3F}},
{0x7D,1,{0x44}},
{0x7E,1,{0x35}},
{0x7F,1,{0x2F}},
{0x80,1,{0x21}},
{0x81,1,{0x10}},
{0x82,1,{0x00}},

//Page2, for GIP
{0xE0,1,{0x02}},

//GIP_L Pin mapping
{0x00,1,{0x47}},
{0x01,1,{0x47}},  
{0x02,1,{0x45}},
{0x03,1,{0x45}},
{0x04,1,{0x4B}},
{0x05,1,{0x4B}},
{0x06,1,{0x49}},
{0x07,1,{0x49}},
{0x08,1,{0x41}},
{0x09,1,{0x1F}},
{0x0A,1,{0x1F}},
{0x0B,1,{0x1F}},
{0x0C,1,{0x1F}},
{0x0D,1,{0x1F}},
{0x0E,1,{0x1F}},
{0x0F,1,{0x43}},
{0x10,1,{0x1F}},
{0x11,1,{0x1F}},
{0x12,1,{0x1F}},
{0x13,1,{0x1F}},
{0x14,1,{0x1F}},
{0x15,1,{0x1F}},

//GIP_R Pin mapping
{0x16,1,{0x46}},
{0x17,1,{0x46}},
{0x18,1,{0x44}},
{0x19,1,{0x44}},
{0x1A,1,{0x4A}},
{0x1B,1,{0x4A}},
{0x1C,1,{0x48}},
{0x1D,1,{0x48}},
{0x1E,1,{0x40}},
{0x1F,1,{0x1F}},
{0x20,1,{0x1F}},
{0x21,1,{0x1F}},
{0x22,1,{0x1F}},
{0x23,1,{0x1F}},
{0x24,1,{0x1F}},
{0x25,1,{0x42}},
{0x26,1,{0x1F}},
{0x27,1,{0x1F}},
{0x28,1,{0x1F}},
{0x29,1,{0x1F}},
{0x2A,1,{0x1F}},
{0x2B,1,{0x1F}},
             
//GIP_L_GS Pin mapping
{0x2C,1,{0x11}},
{0x2D,1,{0x0F}},   
{0x2E,1,{0x0D}}, 
{0x2F,1,{0x0B}}, 
{0x30,1,{0x09}}, 
{0x31,1,{0x07}}, 
{0x32,1,{0x05}}, 
{0x33,1,{0x18}}, 
{0x34,1,{0x17}}, 
{0x35,1,{0x1F}}, 
{0x36,1,{0x01}}, 
{0x37,1,{0x1F}}, 
{0x38,1,{0x1F}}, 
{0x39,1,{0x1F}}, 
{0x3A,1,{0x1F}}, 
{0x3B,1,{0x1F}}, 
{0x3C,1,{0x1F}}, 
{0x3D,1,{0x1F}}, 
{0x3E,1,{0x1F}}, 
{0x3F,1,{0x13}}, 
{0x40,1,{0x1F}}, 
{0x41,1,{0x1F}},

//GIP_R_GS Pin mapping
{0x42,1,{0x10}},
{0x43,1,{0x0E}},   
{0x44,1,{0x0C}}, 
{0x45,1,{0x0A}}, 
{0x46,1,{0x08}}, 
{0x47,1,{0x06}}, 
{0x48,1,{0x04}}, 
{0x49,1,{0x18}}, 
{0x4A,1,{0x17}}, 
{0x4B,1,{0x1F}}, 
{0x4C,1,{0x00}}, 
{0x4D,1,{0x1F}}, 
{0x4E,1,{0x1F}}, 
{0x4F,1,{0x1F}}, 
{0x50,1,{0x1F}}, 
{0x51,1,{0x1F}}, 
{0x52,1,{0x1F}}, 
{0x53,1,{0x1F}}, 
{0x54,1,{0x1F}}, 
{0x55,1,{0x12}}, 
{0x56,1,{0x1F}}, 
{0x57,1,{0x1F}}, 

//GIP Timing  
{0x58,1,{0x40}},
{0x59,1,{0x00}},
{0x5A,1,{0x00}},
{0x5B,1,{0x30}},
{0x5C,1,{0x03}},
{0x5D,1,{0x30}},
{0x5E,1,{0x01}},
{0x5F,1,{0x02}},
{0x60,1,{0x00}},
{0x61,1,{0x01}},
{0x62,1,{0x02}},
{0x63,1,{0x03}},
{0x64,1,{0x6B}},
{0x65,1,{0x00}},
{0x66,1,{0x00}},
{0x67,1,{0x73}},
{0x68,1,{0x05}},
{0x69,1,{0x06}},
{0x6A,1,{0x6B}},
{0x6B,1,{0x08}},
{0x6C,1,{0x00}},
{0x6D,1,{0x04}},
{0x6E,1,{0x04}},
{0x6F,1,{0x88}},
{0x70,1,{0x00}},
{0x71,1,{0x00}},
{0x72,1,{0x06}},
{0x73,1,{0x7B}},
{0x74,1,{0x00}},
{0x75,1,{0x07}},
{0x76,1,{0x00}},
{0x77,1,{0x5D}},
{0x78,1,{0x17}},
{0x79,1,{0x1F}},
{0x7A,1,{0x00}},
{0x7B,1,{0x00}},
{0x7C,1,{0x00}},
{0x7D,1,{0x03}},
{0x7E,1,{0x7B}},

//Page1
{0xE0,1,{0x01}},
{0x0E,1,{0x01}},

//Page3
{0xE0,1,{0x03}},
{0x98,1,{0x2F}},

//Page0
{0xE0,1,{0x00}},

//SLP OUT
{0x11,0,{}}, 	
{REGFLAG_DELAY, 120, {}},

//DISP ON
{0x29,0,{}},  	
{REGFLAG_DELAY, 5, {}},

//--- TE----//
{0x35,1,{0x00}}
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
//		params->physical_width  = PHYSICAL_WIDTH;
//		params->physical_height = PHYSICAL_HEIGHT;
		
    #if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
    #else
		params->dsi.mode   = BURST_VDO_MODE;//BURST_VDO_MODE;	//SYNC_EVENT_VDO_MODE;		//SYNC_PULSE_VDO_MODE;
    #endif
	
		// DSI
		/* Command mode setting */
		// Three lane or Four lane
	   params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_TWO_LANE;

#if 1
  	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
  	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
#endif		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
    //params->dsi.word_count=FRAME_WIDTH*3;
		#if 1
		params->dsi.vertical_sync_active				=4;// 2;
		params->dsi.vertical_backporch					=4;// 19;
		params->dsi.vertical_frontporch					=8;// 8;
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				=20;// 15;
		params->dsi.horizontal_backporch				=20;// 165;
		params->dsi.horizontal_frontporch				=32;// 90;
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		#endif		
		
		params->dsi.PLL_CLOCK 	=220;//210;// 220;//245 by eric.wang
		
}
 
                                                                                                       
static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
   mt_set_gpio_mode(GPIO, GPIO_MODE_00);
   mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
   mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
#if 0
static void lcm_initial_registers(void)
{
    unsigned int data_array[16];
    
#ifdef BUILD_LK
    //printf("[IND][K] y_____0%s\n", __func__);
#else
    //printk("[IND][K] y_____1%s\n", __func__);
#endif
data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x005A5AF1;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x00A5A5FC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00033902;
data_array[1] = 0x001000D0;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x10B11500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00053902;
data_array[1] = 0x2F2214B2;
data_array[2] = 0x00000004;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x01B51500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00093902;
data_array[1] = 0x1F0000EE;
data_array[2] = 0x1F000000;
data_array[3] = 0x00000000;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x000e3902;
data_array[1] = 0x433456EF;
data_array[2] = 0x24809065;
data_array[3] = 0x11910080;
data_array[4] = 0x00001111;
dsi_set_cmdq(data_array, 5, 1);

data_array[0] = 0x00213902;
data_array[1] = 0x090804F7;
data_array[2] = 0x0D0C0B0A;
data_array[3] = 0x17160F0E;
data_array[4] = 0x01010110;
data_array[5] = 0x09080401;
data_array[6] = 0x0D0C0B0A;
data_array[7] = 0x17160F0E;
data_array[8] = 0x01010110;
data_array[9] = 0x00000001;
dsi_set_cmdq(data_array, 10, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x080802F2;
data_array[2] = 0x00001040;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00073902;
data_array[1] = 0x262560F6;
data_array[2] = 0x00000000;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00123902;
data_array[1] = 0x073504FA;
data_array[2] = 0x100B120B;
data_array[3] = 0x2C241A16;
data_array[4] = 0x333B3B33;
data_array[5] = 0x00003334;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00123902;
data_array[1] = 0x073504FB;
data_array[2] = 0x100B120B;
data_array[3] = 0x2C241A16;
data_array[4] = 0x333B3B33;
data_array[5] = 0x00003334;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x000b3902;
data_array[1] = 0xE0C401F3;
data_array[2] = 0x3783D462;
data_array[3] = 0x0000243C;
dsi_set_cmdq(data_array, 4, 1);

data_array[0] = 0x00243902;
data_array[1] = 0x030200F4;
data_array[2] = 0x09020326;
data_array[3] = 0x16160700;
data_array[4] = 0x08080003;
data_array[5] = 0x121C1903;
data_array[6] = 0x1A1E1D1C;
data_array[7] = 0x02040109;
data_array[8] = 0x72757461;
data_array[9] = 0xF0808083;
dsi_set_cmdq(data_array, 10, 1);

data_array[0] = 0x01B01500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00143902;
data_array[1] = 0x5F2F2FF5;
data_array[2] = 0x0F5298AB;
data_array[3] = 0x59044333;
data_array[4] = 0x40055254;
data_array[5] = 0x40595D40;
dsi_set_cmdq(data_array, 6, 1);

data_array[0] = 0x00043902;
data_array[1] = 0x084E01BC;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x1C1003E1;
data_array[2] = 0x000010A0;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00063902;
data_array[1] = 0x111016FD;
data_array[2] = 0x00000920;
dsi_set_cmdq(data_array, 3, 1);

data_array[0] = 0x00351500;
dsi_set_cmdq(data_array, 1, 1);

data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);

data_array[0] = 0x00043902;
data_array[1] = 0x280040C3;
dsi_set_cmdq(data_array, 2, 1);

data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(20);

}
#endif
static void lcm_init(void)
{

	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
	MDELAY(5);	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);
	
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(10);
	lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ONE);
	MDELAY(10);//Must > 5ms
	//lcd_bl_en(1);
	
	//lcm_initial_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1); 
//	MDELAY(100);
}



static void lcm_suspend(void)
{
#if 1 //nt35521
    lcm_set_gpio_output(GPIO_LCD_RST, GPIO_OUT_ZERO);
	MDELAY(200);
    lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
    lcm_set_gpio_output(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
    MDELAY(10);
#endif
}

static void lcm_resume(void)
{
lcm_init();
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

	data_array[0]= 0x00290508; 				//HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}
#endif


LCM_DRIVER u357_hz_wjwx8007a_jd9366_wxga_lcm_drv = 
{
  .name				= "u357_hz_wjwx8007a_jd9366_wxga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
//	.init_power		= lcm_power,
#if (LCM_DSI_CMD_MODE)
  .update         	= lcm_update,
#endif
};
