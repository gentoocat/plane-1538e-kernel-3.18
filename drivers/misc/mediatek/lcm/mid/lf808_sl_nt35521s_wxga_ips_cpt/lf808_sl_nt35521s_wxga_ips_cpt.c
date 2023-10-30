#ifndef BUILD_LK
#include <linux/string.h>
#else
#include <string.h>
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
#define GPIO_LCD_PWR_EN     (GPIO19 | 0x80000000)//GPIO_LCM_PWR_EN //GPIO64
#define GPIO_LCD_BL_EN     (0xffffffff)//GPIO_LCM_BL_EN// GPIO120
#define GPIO_LCM_RST_EN      (GPIO146 | 0x80000000)
#define GPIO_LCD_RST_EN       GPIO_LCM_RST_EN

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

#define   LCM_DSI_CMD_MODE	0



struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
#if 1
static struct LCM_setting_table lcm_initialization_setting[] =
{
    /*
Note :

Data ID will depends on the following rule.

count of parameters > 1	=> Data ID = 0x39
count of parameters = 1	=> Data ID = 0x15
count of parameters = 0	=> Data ID = 0x05

Structure Format :

{DCS command, count of parameters, {parameter list}}
{REGFLAG_DELAY, milliseconds of time, {}},

*/
//Test command_Update 20150714
//{0xFF,4,{0xAA,0x55,0x25,0x01}},
{0xFC,1,{0x08}},
{REGFLAG_DELAY,1,{}},   
{0xFC,1,{0x00}},
{REGFLAG_DELAY,1,{}},   
{0x6F,1,{0x21}},
{0xF7,1,{0x01}},
{REGFLAG_DELAY,1,{}},   
{0x6F,1,{0x21}},
{0xF7,1,{0x00}},
{0xFF,4,{0xAA,0x55,0x25,0x00}},
//---------------Page 0 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
//Display Option Control: 800RGB
{0xB1,2,{0x68,0x01}},
//Source Output Data Hold Time Control
{0xB6,1,{0x08}},
//EQ Control Function for Source
{0xB8,3,{0x01,0x02,0x08}},
//Source Driver Control
{0xBB,2,{0x44,0x44}},
//Inversion Driving Control: Column inversion
{0xBC,2,{0x00,0x00}},
//Display Timing Control
{0xBD,5,{0x02,0x68,0x10,0x10,0x00}},
//Vsync Detection Control
{0xC8,1,{0x80}},
//---------------Page 1 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},
//General Purpose Output Pins Control
{0xC0,1,{0x0C}},
//Setting VGH Voltage VGH=15.2V
{0xB3,2,{0x29,0x29}},
//Setting VGLX Voltage VGL=-10.2V
{0xB4,2,{0x10,0x10}},
//HSOUT Signal Control
{0xB5,2,{0x05,0x05}},
//Power Control for VGH Boosting Times: 2xAVDD-AVEE
{0xB9,2,{0x36,0x35}},
//Power Control for VGLX Boosting Times:AVEE+VCL-AVDD
{0xBA,2,{0x25,0x25}},
//Setting VGMP and VGSP Voltage: 4.3V & 0.3V
{0xBC,2,{0x68,0x01}},
{0xBD,2,{0x68,0x01}},
//Setting VCOM Offset Voltage: -1.025V
{0xBE,1,{0x35}},     // 0x3B

// Gate Signal Voltages Control
{0xCA,1,{0x00}},
//---------------Page 2 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x02}},
{0xEE,1,{0x01}},
{0xB0,16,{0x00,0x00,0x00,0x0C,0x00,0x22,0x00,0x35,0x00,0x47,0x00,0x68,0x00,0x85,0x00,0xB8}},
{0xB1,16,{0x00,0xE3,0x01,0x2B,0x01,0x67,0x01,0xC5,0x02,0x15,0x02,0x18,0x02,0x60,0x02,0xAA}},
{0xB2,16,{0x02,0xD9,0x03,0x1B,0x03,0x40,0x03,0x6D,0x03,0x8D,0x03,0xB4,0x03,0xCE,0x03,0xE5}},
{0xB3,4,{0x03,0xFB,0x03,0xFF}},
//---------------Page 3 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x03}},
//EQ Control for Signal Group CLK: EQ off
{0xB0,2,{0x00,0x00}},
//EQ Control for Signal Group STV: EQ off
{0xB1,2,{0x00,0x00}},
//Control for Signal Type STV01
{0xB2,5,{0x05,0x00,0x17,0x00,0x00}},
//Control for Signal Type RST01
{0xB6,5,{0x05,0x00,0x00,0x00,0x00}},
//Control for Signal Type CLK01 & CLK02
{0xBA,5,{0x53,0x00,0xA0,0x00,0x00}},
{0xBB,5,{0x53,0x00,0xA0,0x00,0x00}},
//Control for Signal Type VAC01 & VAC02
{0xC0,4,{0x00,0x00,0x00,0x00}},
{0xC1,4,{0x00,0x00,0x00,0x00}},
{0xC4,1,{0x60}},
{0xC5,1,{0xC0}},
//---------------Page 5 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x05}},
//Sleep-out Initial Control for CLK Signal Type
{0xB0,2,{0x17,0x06}},
//Sleep-out Initial Control for STV Signal Type
{0xB1,2,{0x17,0x06}},
//Sleep-out Initial Control for RST Signal Type
{0xB2,2,{0x17,0x06}},
//Sleep-out Initial Control for VAC Signal Type
{0xB3,2,{0x17,0x06}},
//Sleep-out Initial Control for VDC1 & 2 Signal Type
{0xB4,2,{0x17,0x06}},
{0xB5,2,{0x17,0x06}},
//Operation Initial Control for CLK Signal Type
{0xB8,1,{0x0C}},
//Operation Initial Control for STV Signal Type
{0xB9,1,{0x00}},
//Operation Initial Control for RST Signal Type
{0xBA,1,{0x00}},
//Operation Initial Control for VAC Signal Type
{0xBB,1,{0x0A}},
//Operation Initial Control for VDC Signal Type
{0xBC,1,{0x02}},
//Gate Signal Output Control
{0xBD,5,{0x03,0x01,0x01,0x03,0x03}},
//STV01 Pulse Waveform Control
{0xC0,1,{0x07}},
//RST01 Pulse Waveform Control
{0xC4,1,{0xA2}},
//CLK01 & CLK02 Pulse Waveform Control
{0xC8,2,{0x03,0x20}},
{0xC9,2,{0x01,0x21}},
//VAC01 & VAC02 Pulse Waveform Control
{0xCC,3,{0x00,0x00,0x01}},
{0xCD,3,{0x00,0x00,0x01}},
//CLK01 & CLK02 Status Control in Vertical Porch Area
{0xD1,5,{0x00,0x04,0xFC,0x07,0x14}},
{0xD2,5,{0x10,0x05,0x00,0x03,0x16}},
//Status Control for CLK Signal Type in Power Off
{0xE5,1,{0x06}},
//Status Control for STV Signal Type in Power Off
{0xE6,1,{0x06}},
//Status Control for RST Signal Type in Power Off
{0xE7,1,{0x06}},
//Status Control for VAC Signal Type in Power Off
{0xE8,1,{0x06}},
//Status Control for VDC1 & VDC2 Signal Type in Power Off
{0xE9,1,{0x06}},
{0xEA,1,{0x06}},
//Gate Signal Status Control for Display Blanking
{0xED,1,{0x30}},
//---------------Page 6 Enable-----------------------------------------------------------------------
{0xF0,5,{0x55,0xAA,0x52,0x08,0x06}},
//Gout Mapping
{0xB0,2,{0x17,0x11}},
{0xB1,2,{0x16,0x10}},
{0xB2,2,{0x12,0x18}},
{0xB3,2,{0x13,0x19}},
{0xB4,2,{0x00,0x31}},
{0xB5,2,{0x31,0x34}},
{0xB6,2,{0x34,0x29}},
{0xB7,2,{0x2A,0x33}},
{0xB8,2,{0x2E,0x2D}},
{0xB9,2,{0x08,0x34}},
{0xBA,2,{0x34,0x08}},
{0xBB,2,{0x2D,0x2E}},
{0xBC,2,{0x34,0x2A}},
{0xBD,2,{0x29,0x34}},
{0xBE,2,{0x34,0x31}},
{0xBF,2,{0x31,0x00}},
{0xC0,2,{0x19,0x13}},
{0xC1,2,{0x18,0x12}},
{0xC2,2,{0x10,0x16}},
{0xC3,2,{0x11,0x17}},
{0xE5,2,{0x34,0x34}},
{0xC4,2,{0x12,0x18}},
{0xC5,2,{0x13,0x19}},
{0xC6,2,{0x17,0x11}},
{0xC7,2,{0x16,0x10}},
{0xC8,2,{0x08,0x31}},
{0xC9,2,{0x31,0x34}},
{0xCA,2,{0x34,0x29}},
{0xCB,2,{0x2A,0x33}},
{0xCC,2,{0x2D,0x2E}},
{0xCD,2,{0x00,0x34}},
{0xCE,2,{0x34,0x00}},
{0xCF,2,{0x2E,0x2D}},
{0xD0,2,{0x34,0x2A}},
{0xD1,2,{0x29,0x34}},
{0xD2,2,{0x34,0x31}},
{0xD3,2,{0x31,0x08}},
{0xD4,2,{0x10,0x16}},
{0xD5,2,{0x11,0x17}},
{0xD6,2,{0x19,0x13}},
{0xD7,2,{0x18,0x12}},
{0xE6,2,{0x34,0x34}},
{0xD8,5,{0x00,0x00,0x00,0x00,0x00}},
{0xD9,5,{0x00,0x00,0x00,0x00,0x00}},
{0xE7,1,{0x00}},
{0XFF,4,{0XAA,0X55,0X25,0X01}},
//{0X6F,1,{0X16}},
//{0XF7,1,{0X10}},
//{0XFF,4,{0XAA,0X55,0X25,0X00}},
{0x11,0,{0x00}},
{REGFLAG_DELAY,120,{}},   
{0x29,0,{0x00}},
{REGFLAG_DELAY,20,{}},
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

#endif 
#if 0
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
#endif

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
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;//LCM_THREE_LANE;

    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


		params->dsi.vertical_sync_active 			= 10; 
    params->dsi.vertical_backporch				=20;
    params->dsi.vertical_frontporch 				=20;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active			= 20;
    params->dsi.horizontal_backporch				= 80;
    params->dsi.horizontal_frontporch				= 80;
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 205;//256;	// 256  251 

}



static void lcm_init(void)
{
	lcd_reset(0);
	lcd_power_en(0);
	lcd_power_en(1);
	MDELAY(30);//Must > 5ms
	lcd_reset(1);
	MDELAY(30);//Must > 5ms
	lcd_reset(0);
	MDELAY(30);//Must > 5ms
	lcd_reset(1);
    MDELAY(50);

	//init_lcm_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

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

LCM_DRIVER lf808_sl_nt35521s_wxga_ips_cpt_lcm_drv = 
{
	.name = "lf808_sl_nt35521s_wxga_ips_cpt",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

