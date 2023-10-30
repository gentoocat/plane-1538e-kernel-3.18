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
    if(GPIO == 0xFFFFFFFF){
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



//#define USING_TABLE 0

#ifdef USING_TABLE
struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
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

#else
static void init_lcm_registers(void)
{		
	unsigned int data_array[16];
	//0xE0 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE1 0x93
	data_array[0] = 0x00023902;
	data_array[1] = 0x000093E1;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE2 0x65
	data_array[0] = 0x00023902;
	data_array[1] = 0x000065E2;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE3 0xF8
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000F8E3;
	dsi_set_cmdq(data_array, 2, 1);
	//0x80 0x03
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000380;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE0 0x04
	data_array[0] = 0x00023902;
	data_array[1] = 0x000004E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0x2B 0x2B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002B2B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x2D 0x03
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000032D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x2E 0x44
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000442E;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE0 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE0 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x000001E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0x00 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000000;
	dsi_set_cmdq(data_array, 2, 1);
	//0x01 0x96
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006c01;
	dsi_set_cmdq(data_array, 2, 1);
	//0x03 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000003;
	dsi_set_cmdq(data_array, 2, 1);
	//0x04 0xA8
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000A804;
	dsi_set_cmdq(data_array, 2, 1);
	//0x17 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000017;
	dsi_set_cmdq(data_array, 2, 1);
	//0x18 0xC0
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000C018;
	dsi_set_cmdq(data_array, 2, 1);
	//0x19 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000119;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1A 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000001A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1B 0xC0
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000C01B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1C 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000011C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1F 0x6A
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006A1F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x20 0x23
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002320;
	dsi_set_cmdq(data_array, 2, 1);
	//0x21 0x23
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002321;
	dsi_set_cmdq(data_array, 2, 1);
	//0x22 0x0E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000E22;
	dsi_set_cmdq(data_array, 2, 1);
	//0x35 0x28
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002835;
	dsi_set_cmdq(data_array, 2, 1);
	//0x37 0x59
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005937;
	dsi_set_cmdq(data_array, 2, 1);
	//0x38 0x05
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000538;
	dsi_set_cmdq(data_array, 2, 1);
	//0x39 0x04
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000439;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3A 0x08
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000083A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3B 0x08
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000083B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3C 0x7C
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007C3C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3D 0xFF
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000FF3D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3E 0xFF
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000FF3E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x3F 0xFF
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000FF3F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x40 0x06
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000640;
	dsi_set_cmdq(data_array, 2, 1);
	//0x41 0xA0
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000A041;
	dsi_set_cmdq(data_array, 2, 1);
	//0x43 0x08
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000843;
	dsi_set_cmdq(data_array, 2, 1);
	//0x44 0x0B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000B44;
	dsi_set_cmdq(data_array, 2, 1);
	//0x45 0x88
	data_array[0] = 0x00023902;
	data_array[1] = 0x00008845;
	dsi_set_cmdq(data_array, 2, 1);
	//0x55 0x0F
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000F55;
	dsi_set_cmdq(data_array, 2, 1);
	//0x56 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000156;
	dsi_set_cmdq(data_array, 2, 1);
	//0x57 0xAC
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000AC57;
	dsi_set_cmdq(data_array, 2, 1);
	//0x58 0x0A
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000A658;
	dsi_set_cmdq(data_array, 2, 1);
	//0x59 0xFA
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000FA59;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5A 0x28
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000285A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5B 0x1E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001E5B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5D 0x67
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000675D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5E 0x4E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004E5E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5F 0x3E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003E5F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x60 0x32
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003260;
	dsi_set_cmdq(data_array, 2, 1);
	//0x61 0x2E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002E61;
	dsi_set_cmdq(data_array, 2, 1);
	//0x62 0x20
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002062;
	dsi_set_cmdq(data_array, 2, 1);
	///0x63 0x26
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002663;
	dsi_set_cmdq(data_array, 2, 1);
	//0x64 0x11
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001164;
	dsi_set_cmdq(data_array, 2, 1);
	//0x65 0x2D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D65;
	dsi_set_cmdq(data_array, 2, 1);
	//0x66 0x2D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D66;
	dsi_set_cmdq(data_array, 2, 1);
	//0x67 0x2F
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002F67;
	dsi_set_cmdq(data_array, 2, 1);
	//0x68 0x4E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004E68;
	dsi_set_cmdq(data_array, 2, 1);
	//0x69 0x3C
	data_array[0] = 0x00023902; 
	data_array[1] = 0x00003C69;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6A 0x43
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000436A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6B 0x35
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000356B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6C 0x30
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000306C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6D 0x23
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000236D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6E 0x12
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000126E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6F 0x08
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000086F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x70 0x67
	data_array[0] = 0x00023902;
	data_array[1] = 0x00006770;
	dsi_set_cmdq(data_array, 2, 1);
	//0x71 0x4E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004E71;
	dsi_set_cmdq(data_array, 2, 1);
	//0x72 0x3E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003E72;
	dsi_set_cmdq(data_array, 2, 1);
	//0x73 0x32
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003273;
	dsi_set_cmdq(data_array, 2, 1);
	//0x74 0x2E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002E74;
	dsi_set_cmdq(data_array, 2, 1);
	//0x75 0x20
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002075;
	dsi_set_cmdq(data_array, 2, 1);
	//0x76 0x26
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002676;
	dsi_set_cmdq(data_array, 2, 1);
	//0x77 0x11
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001177;
	dsi_set_cmdq(data_array, 2, 1);
	//0x78 0x2D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D78;
	dsi_set_cmdq(data_array, 2, 1);
	//0x79 0x2D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002D79;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7A 0x2F
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002F7A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7B 0x4E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004E7B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7C 0x3C
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003C7C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7D 0x43
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000437D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7E 0x35
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000357E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7F 0x30
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000307F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x80 0x23
	data_array[0] = 0x00023902;
	data_array[1] = 0x00002380;
	dsi_set_cmdq(data_array, 2, 1);
	//0x81 0x12
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001281;
	dsi_set_cmdq(data_array, 2, 1);
	//0x82 0x08
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000882;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE0 0x02
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0x00 0x44
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004400;
	dsi_set_cmdq(data_array, 2, 1);
	//0x01 0x44
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004401;
	dsi_set_cmdq(data_array, 2, 1);
	//0x02 0x45
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004502;
	dsi_set_cmdq(data_array, 2, 1);
	//0x03 0x45
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004503;
	dsi_set_cmdq(data_array, 2, 1);
	//0x04 0x46
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004604;
	dsi_set_cmdq(data_array, 2, 1);
	//0x05 0x46
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004605;
	dsi_set_cmdq(data_array, 2, 1);
	//0x06 0x47
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004706;
	dsi_set_cmdq(data_array, 2, 1);
	//0x07 0x47
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004707;
	dsi_set_cmdq(data_array, 2, 1);
	//0x08 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D08;
	dsi_set_cmdq(data_array, 2, 1);
	//0x09 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D09;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0A 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D0A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0B 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D0B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0C 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D0C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0D 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D0D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0E 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D0E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x0F 0x57
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000570F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x10 0x57
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005710;
	dsi_set_cmdq(data_array, 2, 1);
	///0x11 0x58
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005811;
	dsi_set_cmdq(data_array, 2, 1);
	//0x12 0x58
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005812;
	dsi_set_cmdq(data_array, 2, 1);
	//0x13 0x40
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004013;
	dsi_set_cmdq(data_array, 2, 1);
	//0x14 0x55
	data_array[0] = 0x00023902;
	data_array[1] = 0x0005514;
	dsi_set_cmdq(data_array, 2, 1);
	//0x15 0x55
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005515;
	dsi_set_cmdq(data_array, 2, 1);
	//0x16 0x44
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004416;
	dsi_set_cmdq(data_array, 2, 1);
	//0x17 0x44
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004417;
	dsi_set_cmdq(data_array, 2, 1);
	//0x18 0x45
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004518;
	dsi_set_cmdq(data_array, 2, 1);
	//0x19 0x45
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004519;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1A 0x46
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000461A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1B 0x46
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000461B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1C 0x47
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000471C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1D 0x47
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000471D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1E 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D1E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x1F 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D1F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x20 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D20;
	dsi_set_cmdq(data_array, 2, 1);
	//0x21 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D21;
	dsi_set_cmdq(data_array, 2, 1);
	//0x22 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D22;
	dsi_set_cmdq(data_array, 2, 1);
	//0x23 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D23;
	dsi_set_cmdq(data_array, 2, 1);
	//0x24 0x1D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001D24;
	dsi_set_cmdq(data_array, 2, 1);
	//0x25 0x57
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005725;
	dsi_set_cmdq(data_array, 2, 1);
	//0x26 0x57
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005726;
	dsi_set_cmdq(data_array, 2, 1);
	//0x27 0x58
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005827;
	dsi_set_cmdq(data_array, 2, 1);
	//0x28 0x58
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005828;
	dsi_set_cmdq(data_array, 2, 1);
	//0x29 0x40
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004029;
	dsi_set_cmdq(data_array, 2, 1);
	//0x2A 0x55
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000552A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x2B 0x55
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000552B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x58 0x40
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004058;
	dsi_set_cmdq(data_array, 2, 1);
	//0x59 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000059;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5A 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000005A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5B 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000005B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5C 0x0A
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000A5C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5D 0x10
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000105D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5E 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000015E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x5F 0x02
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000025F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x60 0x10
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001060;
	dsi_set_cmdq(data_array, 2, 1);
	//0x61 0x01
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000161;
	dsi_set_cmdq(data_array, 2, 1);
	//0x62 0x02
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000262;
	dsi_set_cmdq(data_array, 2, 1);
	//0x63 0x0B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000B63;
	dsi_set_cmdq(data_array, 2, 1);
	//0x64 0x54
	data_array[0] = 0x00023902;
	data_array[1] = 0x00005464;
	dsi_set_cmdq(data_array, 2, 1);
	//0x65 0x45
	data_array[0] = 0x00023902;
	data_array[1] = 0x00004565;
	dsi_set_cmdq(data_array, 2, 1);
	//0x66 0x07
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000766;
	dsi_set_cmdq(data_array, 2, 1);
	//0x67 0x31
	data_array[0] = 0x00023902;
	data_array[1] = 0x00003167;
	dsi_set_cmdq(data_array, 2, 1);
	//0x68 0x0B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000B68;
	dsi_set_cmdq(data_array, 2, 1);
	//0x69 0x1E
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001E69;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6A 0x54
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000546A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6B 0x04
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000046B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6C 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000006C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6D 0x04
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000046D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6E 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000006E;
	dsi_set_cmdq(data_array, 2, 1);
	//0x6F 0x88
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000886F;
	dsi_set_cmdq(data_array, 2, 1);
	//0x70 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000070;
	dsi_set_cmdq(data_array, 2, 1);
	//0x71 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000071;
	dsi_set_cmdq(data_array, 2, 1);
	//0x72 0x06
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000672;
	dsi_set_cmdq(data_array, 2, 1);
	//0x73 0x7B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007B73;
	dsi_set_cmdq(data_array, 2, 1);
	//0x74 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000074;
	dsi_set_cmdq(data_array, 2, 1);
	//0x75 0xF8
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000F875;
	dsi_set_cmdq(data_array, 2, 1);
	//0x76 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000076;
	dsi_set_cmdq(data_array, 2, 1);
	//0x77 0x0D
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000D77;
	dsi_set_cmdq(data_array, 2, 1);
	//0x78 0x14
	data_array[0] = 0x00023902;
	data_array[1] = 0x00001478;
	dsi_set_cmdq(data_array, 2, 1);
	//0x79 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x00000079;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7A 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007A;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7B 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007B;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7C 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000007C;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7D 0x03
	data_array[0] = 0x00023902;
	data_array[1] = 0x0000037D;
	dsi_set_cmdq(data_array, 2, 1);
	//0x7E 0x7B
	data_array[0] = 0x00023902;
	data_array[1] = 0x00007B7E;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE0 0x00
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000E0;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE6 0x02
	data_array[0] = 0x00023902;
	data_array[1] = 0x000002E6;
	dsi_set_cmdq(data_array, 2, 1);
	//0xE7 0x06
	data_array[0] = 0x00023902;
	data_array[1] = 0x000006E7;
	dsi_set_cmdq(data_array, 2, 1);

	data_array[0] = 0x00110500;        //exit sleep mode
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(50);
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


		params->dsi.vertical_sync_active 			= 4; 
		params->dsi.vertical_backporch					= 8; //6; //4; //6; //10;
    params->dsi.vertical_frontporch 				=8;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active			= 4;
    params->dsi.horizontal_backporch				= 132;
    params->dsi.horizontal_frontporch				= 24;
		params->dsi.horizontal_active_pixel = FRAME_WIDTH;

        	params->dsi.PLL_CLOCK = 225;

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

#ifdef USING_TABLE
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
#else
  init_lcm_registers();
#endif

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

LCM_DRIVER lf808_pvo_jd9365_wxga_ips_auo_lcm_drv = 
{
	.name = "lf808_pvo_jd9365_wxga_ips_auo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

