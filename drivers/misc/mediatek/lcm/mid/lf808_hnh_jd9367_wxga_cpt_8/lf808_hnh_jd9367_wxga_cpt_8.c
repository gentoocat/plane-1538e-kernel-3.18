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

#define GPIO_LCD_PWR_EN    (GPIO19 | 0x80000000)
#define GPIO_LCD_BL_EN     (0xffffffff)

#define GPIO_LCD_RST      (GPIO146 | 0x80000000)    // 

#define GPIO_LCM_STB       (0xffffffff)

//#define USING_TABLE 1


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
#define REGFLAG_DELAY       0XFE
#define REGFLAG_END_OF_TABLE    0xFF   // END OF REGISTERS MARKER

#ifdef USING_TABLE
struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
 //hx080wq01  driver ic9366
    {0xe0,  1,  {0x00}},  //page0
    {0xe1,  1,  {0x93}},
    {0xe2,  1,  {0x65}},
    {0xe3,  1,  {0xf8}},
    {0xe6,  1,  {0x02}},
    {0xe7,  1,  {0x02}},
    {0xe0,  1,  {0x01}},//page1
    {0x00,  1,  {0x00}},
    {0x01,  1,  {0xB7}},
    {0x17,  1,  {0x00}},
    {0x18,  1,  {0xB2}},
    {0x19,  1,  {0x01}},
    {0x1A,  1,  {0x00}},
    {0x1B,  1,  {0xB2}},
    {0x1C,  1,  {0x01}},
    {0x1F,  1,  {0x7E}},
    {0x20,  1,  {0x28}},
    {0x21,  1,  {0x28}},
    {0x22,  1,  {0x0E}},
    {0x24,  1,  {0xC8}},    
    {0x37,  1,  {0x29}},
    {0x38,  1,  {0x05}},
    {0x39,  1,  {0x08}},
    {0x3a,  1,  {0x12}},
    {0x3b,  1,  {0x78}},
    {0x3c,  1,  {0xFF}},
    {0x3e,  1,  {0xFF}},
    {0x3f,  1,  {0xFF}},
    {0x40,  1,  {0x06}},
    {0x41,  1,  {0xA0}},
    {0x43,  1,  {0x08}},    
    {0x44,  1,  {0x07}},
    {0x45,  1,  {0x24}},
    {0x55,  1,  {0x01}},
    {0x56,  1,  {0x01}},
    {0x57,  1,  {0x89}},
    {0x58,  1,  {0x0A}},
    {0x59,  1,  {0x2A}},
    {0x5A,  1,  {0x31}},
    {0x5B,  1,  {0x15}},
    //gammar
    {0x5D,  1,  {0x7C}},
    {0x5E,  1,  {0x64}},
    {0x5F,  1,  {0x54}},
    {0x60,  1,  {0x47}},
    {0x61,  1,  {0x42}},
    {0x62,  1,  {0x32}},
    {0x63,  1,  {0x34}},
    {0x64,  1,  {0x1C}},
    {0x65,  1,  {0x32}},
    {0x66,  1,  {0x2E}},
    {0x67,  1,  {0x2B}},
    {0x68,  1,  {0x46}},
    {0x69,  1,  {0x32}},
    {0x6A,  1,  {0x38}},
    {0x6B,  1,  {0x2A}},
    {0x6C,  1,  {0x28}},
    {0x6D,  1,  {0x1B}},
    {0x6E,  1,  {0x0D}},
    {0x6F,  1,  {0x00}},
    {0x70,  1,  {0x7C}},
    {0x71,  1,  {0x64}},
    {0x72,  1,  {0x54}},
    {0x73,  1,  {0x47}},
    {0x74,  1,  {0x42}},
    {0x75,  1,  {0x32}},
    {0x76,  1,  {0x34}},
    {0x77,  1,  {0x1C}},
    {0x78,  1,  {0x32}},
    {0x79,  1,  {0x2E}},
    {0x7A,  1,  {0x2B}},
    {0x7B,  1,  {0x46}},
    {0x7C,  1,  {0x32}},
    {0x7D,  1,  {0x38}},
    {0x7E,  1,  {0x2A}},
    {0x7F,  1,  {0x28}},
    {0x80,  1,  {0x1B}},
    {0x81,  1,  {0x0D}},
    {0x82,  1,  {0x00}},

    {0xE0,  1,  {0x02}},
    {0x00,  1,  {0x00}},
    {0x01,  1,  {0x04}},   
    {0x02,  1,  {0x08}}, 
    {0x03,  1,  {0x05}}, 
    {0x04,  1,  {0x09}}, 
    {0x05,  1,  {0x06}}, 
    {0x06,  1,  {0x0A}}, 
    {0x07,  1,  {0x07}}, 
    {0x08,  1,  {0x0B}}, 
    {0x09,  1,  {0x1f}}, 
    {0x0A,  1,  {0x1f}}, 
    {0x0B,  1,  {0x1f}}, 
    {0x0C,  1,  {0x1f}}, 
    {0x0D,  1,  {0x1f}}, 
    {0x0E,  1,  {0x1f}}, 
    {0x0F,  1,  {0x17}}, 
    {0x10,  1,  {0x37}}, 
    {0x11,  1,  {0x10}}, 
    {0x12,  1,  {0x1F}}, 
    {0x13,  1,  {0x1F}}, 
    {0x14,  1,  {0x1F}}, 
    {0x15,  1,  {0x1F}}, 
    {0x16,  1,  {0x00}},
    {0x17,  1,  {0x04}},   
    {0x18,  1,  {0x08}}, 
    {0x19,  1,  {0x05}}, 
    {0x1A,  1,  {0x09}}, 
    {0x1B,  1,  {0x06}}, 
    {0x1C,  1,  {0x0A}}, 
    {0x1D,  1,  {0x07}}, 
    {0x1E,  1,  {0x0B}}, 
    {0x1F,  1,  {0x1f}}, 
    {0x20,  1,  {0x1f}}, 
    {0x21,  1,  {0x1f}}, 
    {0x22,  1,  {0x1f}}, 
    {0x23,  1,  {0x1f}}, 
    {0x24,  1,  {0x1f}}, 
    {0x25,  1,  {0x17}}, 
    {0x26,  1,  {0x37}}, 
    {0x27,  1,  {0x10}}, 
    {0x28,  1,  {0x1F}}, 
    {0x29,  1,  {0x1F}}, 
    {0x2A,  1,  {0x1F}}, 
    {0x2B,  1,  {0x1F}}, 
    {0x58,  1,  {0x01}}, 
    {0x59,  1,  {0x00}}, 
    {0x5A,  1,  {0x00}}, 
    {0x5B,  1,  {0x00}}, 
    {0x5C,  1,  {0x01}}, 
    {0x5D,  1,  {0x30}}, 
    {0x5E,  1,  {0x00}}, 
    {0x5F,  1,  {0x00}}, 
    {0x60,  1,  {0x30}}, 
    {0x61,  1,  {0x00}}, 
    {0x62,  1,  {0x00}}, 
    {0x63,  1,  {0x03}}, 
    {0x64,  1,  {0x6A}}, 
    {0x65,  1,  {0x45}}, 
    {0x66,  1,  {0x08}}, 
    {0x67,  1,  {0x73}}, 
    {0x68,  1,  {0x05}}, 
    {0x69,  1,  {0x06}}, 
    {0x6A,  1,  {0x6A}},
    {0x6B,  1,  {0x08}}, 
    {0x6C,  1,  {0x00}}, 
    {0x6D,  1,  {0x00}}, 
    {0x6E,  1,  {0x00}}, 
    {0x6F,  1,  {0x88}}, 
    {0x70,  1,  {0x00}}, 
    {0x71,  1,  {0x00}}, 
    {0x72,  1,  {0x06}}, 
    {0x73,  1,  {0x7B}}, 
    {0x74,  1,  {0x00}}, 
    {0x75,  1,  {0x80}}, 
    {0x76,  1,  {0x00}}, 
    {0x77,  1,  {0x05}}, 
    {0x78,  1,  {0x10}}, 
    {0x79,  1,  {0x00}}, 
    {0x7A,  1,  {0x00}}, 
    {0x7B,  1,  {0x00}}, 
    {0x7C,  1,  {0x00}}, 
    {0x7D,  1,  {0x03}}, 
    {0x7E,  1,  {0x7B}},
    
    {0xE0,  1,  {0x01}},
    {0x0E,  1,  {0x01}},

    {0xE0,  1,  {0x03}},//page3
    {0x98,  1,  {0x2F}},
    {0xE0,  1,  {0x04}},//page4
    {0x2B,  1,  {0x2B}},
    {0x2d,  1,  {0x03}},
    {0x2E,  1,  {0x44}},

    {0xE0,  1,  {0x00}},//page0
    {0xE1,  1,  {0x93}},
    {0xE2,  1,  {0x65}},
    {0xE3,  1,  {0xf8}},
    {0x80,  1,  {0x02}}, //3 lane
    //{0x4a,    1,  {0x35}},    
    {0x11,  0,  {0x00}},
    {REGFLAG_DELAY,100,{}},
    {0x29,  0,  {0x00}},
    {REGFLAG_DELAY,200,{}},
    {0x35,  0,  {0x00}},
//3 lane
/*{0xFF,4,{0xAA,0x55,0x25,0x01}},
{0x6F,1,{0x16,}},
{0xF7,1,{0x10,}},
{0xFF,4,{0xAA,0x55,0x25,0x00}},*/

//bist
/*{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
{0xEF,2,{0x00,0x01}},
{0xEE,4,{0x87,0x78,0x02,0x40}},*/

//{0x35,0,{0x00}},          //TE on

//{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},

{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
        unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned char cmd;
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

#else


static void lcm_initial_registers(void)
{
    unsigned int data_array[16];

    data_array[0] = 0x00E01500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x93E11500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x65E21500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xF8E31500;
    dsi_set_cmdq(data_array, 1, 1);
    
    
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x10701500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x13711500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x06721500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x02E61500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x02E71500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x01E01500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00001500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x7F011500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x00031500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x7F041500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00171500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xD7181500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x05191500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x001A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xD71B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x051C1500;
    dsi_set_cmdq(data_array, 1, 1);
                    
    data_array[0] = 0x791F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2D201500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2D211500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0E221500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0xF8241500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xF1261500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x09371500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x04381500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0C391500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x183A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x783C1500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x06401500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xA0411500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x01551500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x01561500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xA9571500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0A581500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2A591500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x375A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x195B1500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x705D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x505E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x3F5F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x31601500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2D611500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1D621500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x22631500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0C641500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x25651500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x24661500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x24671500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x41681500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2F691500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x366A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x286B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x266C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1C6D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x086E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x026F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x70701500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x50711500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x3F721500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x31731500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2D741500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1D751500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x22761500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0C771500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x25781500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x24791500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x247A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x417B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2F7C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x367D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x287E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x267F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1C801500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x08811500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x02821500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x02E01500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00001500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x04011500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x06021500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x08031500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0A041500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0C051500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0E061500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x17071500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x37081500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F091500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x100A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F0B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F0C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F0D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F0E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F0F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F101500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F111500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F121500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x12131500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F141500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F151500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x01161500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x05171500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x07181500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x09191500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0B1A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0D1B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0F1C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x171D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x371E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F1F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x11201500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F211500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F221500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F231500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F241500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F251500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F261500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F271500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F281500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x13291500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F2A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x1F2B1500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x10581500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x00591500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x005A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x105B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x075C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x305D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x005E1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x005F1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x30601500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x03611500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x04621500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x03631500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x6A641500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x75651500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0D661500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0xB3671500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x09681500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x06691500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x6A6A1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x046B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x006C1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x046D1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x046E1500;
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
    data_array[0] = 0xBC751500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x00761500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x0D771500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2C781500;
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
                                                                                        
    data_array[0] = 0x04E01500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x2B2B1500;
    dsi_set_cmdq(data_array, 1, 1);
    data_array[0] = 0x442E1500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00110500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);
    
    data_array[0] = 0x00290500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(5);


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
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_FOUR_LANE;
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
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	    params->dsi.intermediat_buffer_num = 0;

		//params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888; //mask in mt8321 platform
        params->dsi.word_count=FRAME_WIDTH*3;
		
		params->dsi.vertical_sync_active				= 4;//1// by eric.wang
		params->dsi.vertical_backporch					= 10;//8 by eric.wang 23
		params->dsi.vertical_frontporch					= 30;//6 by eric.wang 12 
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 18;//10
		params->dsi.horizontal_backporch				= 18;//60 by eric.wangc 160
		params->dsi.horizontal_frontporch				= 18;//60 by eric.wang 160

		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
		
		params->dsi.PLL_CLOCK 	= 245;// 200
		
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
	MDELAY(130);
#ifdef USING_TABLE
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




LCM_DRIVER lf808_hnh_jd9367_wxga_cpt_8_lcm_drv = 
{
  .name				= "lf808_hnh_jd9367_wxga_cpt_8",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
