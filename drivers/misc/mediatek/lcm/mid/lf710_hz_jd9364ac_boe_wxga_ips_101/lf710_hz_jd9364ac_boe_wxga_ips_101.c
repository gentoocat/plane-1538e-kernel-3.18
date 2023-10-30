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

#define   LCM_DSI_CMD_MODE	0



struct LCM_setting_table
{
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
#if 0
static struct LCM_setting_table lcm_initialization_setting[] =
{
{0xE0, 1, {0x00}},
{0xE1, 1, {0x93}},
{0xE2, 1, {0x65}},
{0xE3, 1, {0xF8}},
{0x70, 1, {0x10}},
{0x71, 1, {0x13}},
{0x72, 1, {0x06}},
{0x80, 1, {0x03}},
{0xE0, 1, {0x04}},
{0x2D, 1, {0x03}},
{0xE0, 1, {0x01}},
{0x01, 1, {0xB3}},
{0x17, 1, {0x00}},
{0x18, 1, {0xD8}},
{0x19, 1, {0x05}},
{0x1A, 1, {0x00}},
{0x1B, 1, {0xD8}},
{0x1C, 1, {0x05}},
{0x1F, 1, {0x7E}},
{0x20, 1, {0x28}},
{0x21, 1, {0x28}},
{0x22, 1, {0x0E}},
{0x24, 1, {0xF8}},
{0x26, 1, {0xF3}},
{0x27, 1, {0x44}},
{0x37, 1, {0x59}},
{0x38, 1, {0x05}},
{0x39, 1, {0x0C}},
{0x3A, 1, {0x18}},
{0x3C, 1, {0x78}},
{0x3E, 1, {0x78}},
{0x40, 1, {0x06}},
{0x41, 1, {0xA0}},
{0x55, 1, {0x01}},
{0x56, 1, {0x01}},
{0x57, 1, {0xA8}},
{0x58, 1, {0x0A}},
{0x59, 1, {0x2A}},
{0x5A, 1, {0x3C}},
{0x5B, 1, {0x14}},
{0x5D, 1, {0x70}},
{0x5E, 1, {0x53}},
{0x5F, 1, {0x42}},
{0x60, 1, {0x35}},
{0x61, 1, {0x32}},
{0x62, 1, {0x23}},
{0x63, 1, {0x28}},
{0x64, 1, {0x13}},
{0x65, 1, {0x2D}},
{0x66, 1, {0x2D}},
{0x67, 1, {0x2D}},
{0x68, 1, {0x4B}},
{0x69, 1, {0x39}},
{0x6A, 1, {0x40}},
{0x6B, 1, {0x2F}},
{0x6C, 1, {0x26}},
{0x6D, 1, {0x1E}},
{0x6E, 1, {0x07}},
{0x6F, 1, {0x02}},
{0x70, 1, {0x70}},
{0x71, 1, {0x53}},
{0x72, 1, {0x42}},
{0x73, 1, {0x35}},
{0x74, 1, {0x32}},
{0x75, 1, {0x23}},
{0x76, 1, {0x28}},
{0x77, 1, {0x13}},
{0x78, 1, {0x2D}},
{0x79, 1, {0x2D}},
{0x7A, 1, {0x2D}},
{0x7B, 1, {0x4B}},
{0x7C, 1, {0x39}},
{0x7D, 1, {0x40}},
{0x7E, 1, {0x2F}},
{0x7F, 1, {0x26}},
{0x80, 1, {0x1E}},
{0x81, 1, {0x07}},
{0x82, 1, {0x02}},
{0xE0, 1, {0x02}},
{0x01, 1, {0x5F}},
{0x02, 1, {0x5F}},
{0x03, 1, {0x5F}},
{0x04, 1, {0x5F}},
{0x05, 1, {0x51}},
{0x06, 1, {0x58}},
{0x07, 1, {0x57}},
{0x08, 1, {0x4F}},
{0x09, 1, {0x5F}},
{0x0A, 1, {0x4D}},
{0x0B, 1, {0x4B}},
{0x0C, 1, {0x5F}},
{0x0D, 1, {0x49}},
{0x0E, 1, {0x47}},
{0x0F, 1, {0x45}},
{0x10, 1, {0x41}},
{0x11, 1, {0x5F}},
{0x12, 1, {0x5F}},
{0x13, 1, {0x5F}},
{0x14, 1, {0x5F}},
{0x15, 1, {0x5F}},
{0x16, 1, {0x52}},
{0x17, 1, {0x5F}},
{0x18, 1, {0x5F}},
{0x19, 1, {0x5F}},
{0x1A, 1, {0x5F}},
{0x1B, 1, {0x50}},
{0x1C, 1, {0x58}},
{0x1D, 1, {0x57}},
{0x1E, 1, {0x4E}},
{0x1F, 1, {0x5F}},
{0x20, 1, {0x4C}},
{0x21, 1, {0x4A}},
{0x22, 1, {0x5F}},
{0x23, 1, {0x48}},
{0x24, 1, {0x46}},
{0x25, 1, {0x44}},
{0x26, 1, {0x40}},
{0x27, 1, {0x5F}},
{0x28, 1, {0x5F}},
{0x29, 1, {0x5F}},
{0x2A, 1, {0x5F}},
{0x2B, 1, {0x5F}},
{0x2C, 1, {0x52}},
{0x2D, 1, {0x5F}},
{0x2E, 1, {0x5F}},
{0x2F, 1, {0x5F}},
{0x30, 1, {0x5F}},
{0x31, 1, {0x40}},
{0x32, 1, {0x58}},
{0x33, 1, {0x57}},
{0x34, 1, {0x44}},
{0x35, 1, {0x5F}},
{0x36, 1, {0x46}},
{0x37, 1, {0x48}},
{0x38, 1, {0x5F}},
{0x39, 1, {0x4A}},
{0x3A, 1, {0x4C}},
{0x3B, 1, {0x4E}},
{0x3C, 1, {0x50}},
{0x3D, 1, {0x5F}},
{0x3E, 1, {0x5F}},
{0x3F, 1, {0x5F}},
{0x40, 1, {0x5F}},
{0x41, 1, {0x5F}},
{0x42, 1, {0x53}},
{0x43, 1, {0x5F}},
{0x44, 1, {0x5F}},
{0x45, 1, {0x5F}},
{0x46, 1, {0x5F}},
{0x47, 1, {0x41}},
{0x48, 1, {0x58}},
{0x49, 1, {0x57}},
{0x4A, 1, {0x45}},
{0x4B, 1, {0x5F}},
{0x4C, 1, {0x47}},
{0x4D, 1, {0x49}},
{0x4E, 1, {0x5F}},
{0x4F, 1, {0x4B}},
{0x50, 1, {0x4D}},
{0x51, 1, {0x4F}},
{0x52, 1, {0x51}},
{0x53, 1, {0x5F}},
{0x54, 1, {0x5F}},
{0x55, 1, {0x5F}},
{0x56, 1, {0x5F}},
{0x57, 1, {0x5F}},
{0x58, 1, {0x10}},
{0x59, 1, {0x00}},
{0x5A, 1, {0x00}},
{0x5B, 1, {0x10}},
{0x5C, 1, {0x05}},
{0x5D, 1, {0x30}},
{0x5E, 1, {0x00}},
{0x5F, 1, {0x00}},
{0x60, 1, {0x30}},
{0x61, 1, {0x02}},
{0x62, 1, {0x00}},
{0x63, 1, {0x03}},
{0x64, 1, {0x7B}},
{0x65, 1, {0x75}},
{0x66, 1, {0x0B}},
{0x67, 1, {0xB3}},
{0x68, 1, {0x07}},
{0x69, 1, {0x06}},
{0x6A, 1, {0x7B}},
{0x6B, 1, {0x04}},
{0x6C, 1, {0x00}},
{0x6D, 1, {0x04}},
{0x6E, 1, {0x04}},
{0x6F, 1, {0x88}},
{0x70, 1, {0x00}},
{0x71, 1, {0x00}},
{0x72, 1, {0x06}},
{0x73, 1, {0x7B}},
{0x74, 1, {0x00}},
{0x75, 1, {0x80}},
{0x76, 1, {0x00}},
{0x77, 1, {0x0D}},
{0x78, 1, {0x2A}},
{0x79, 1, {0x00}},
{0x7A, 1, {0x00}},
{0x7B, 1, {0x00}},
{0x7C, 1, {0x00}},
{0x7D, 1, {0x03}},
{0x7E, 1, {0x7B}},
{0x64, 1, {0x40}},
{0x6A, 1, {0x40}},
{0x7F, 1, {0x40}},
{0xE0, 1, {0x00}},

{0x11, 0, {0x00}},
{REGFLAG_DELAY,120,{}},
{0x29, 0, {0x00}},
{REGFLAG_DELAY,5,{}},
{0x35, 1, {0x00}},
{0xE0, 1, {0x01}},
{0x4A, 1, {0x05}},
{0xE0, 1, {0x04}},
{0x1E, 1, {0x69}},
{0xE0, 1, {0x05}},
{0x01, 1, {0x28}},
{0xE0, 1, {0x00}},
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

static void init_lcm_registers(void)
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
data_array[0] = 0x02801500;    // 0x02 3 lane 03 4 lane
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x04E01500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x032D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x01E01500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00001500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5C011500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00031500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x68041500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00171500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xD7181500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x01191500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x001A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xD71B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x011C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x6B1F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x29201500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x29211500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x7E221500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x59371500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x28351500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x05381500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x08391500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x123A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x7E3C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xFF3D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xFF3E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x7F3F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x06401500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xA0411500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1E431500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0B441500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x01551500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x01561500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x69571500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0A581500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x2A591500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x295A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x155B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x165C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x645D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x505E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x435F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x38601500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x36611500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x29621500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x2F631500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1B641500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x35651500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x35661500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x35671500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x53681500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x41691500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x476A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x386B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x346C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x276D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x196E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x126F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x64701500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x50711500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x43721500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x38731500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x36741500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x29751500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x2F761500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1B771500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x35781500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x35791500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x357A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x537B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x417C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x477D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x387E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x347F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x27801500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x19811500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x12821500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x02E01500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x53001500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F011500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F021500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F031500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F041500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x51051500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x77061500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x57071500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4F081500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F091500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4D0A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4B0B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F0C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x490D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x470E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x450F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x41101500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F111500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F121500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F131500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F141500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F151500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x52161500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F171500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F181500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F191500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F1A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x501B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x771C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x571D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4E1E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F1F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4C201500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x4A211500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F221500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x48231500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x46241500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x44251500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x40261500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F271500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F281500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F291500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F2A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x5F2B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x122C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F2D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F2E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F2F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F301500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00311500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x17321500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x17331500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x04341500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F351500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x06361500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x08371500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F381500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0A391500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0C3A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0E3B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x103C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F3D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F3E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F3F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F401500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F411500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x13421500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F431500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F441500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F451500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F461500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x01471500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x17481500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x17491500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x054A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F4B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x074C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x094D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F4E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0B4F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0D501500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0F511500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x11521500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F531500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F541500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F551500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F561500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x1F571500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00581500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00591500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x005A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x105B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x045C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x405D1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x005E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x005F1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x40601500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x03611500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x04621500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x70631500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x70641500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x75651500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x0B661500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0xB4671500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x06681500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x70691500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x706A1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x046B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x006C1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x046D1500;
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
data_array[0] = 0xBC751500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00761500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x05771500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x2A781500;
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
data_array[0] = 0x480E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x10091500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x2B2B1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x442E1500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00E01500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x02E61500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x06E71500;
dsi_set_cmdq(data_array, 1, 1);
data_array[0] = 0x00110500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(120);
data_array[0] = 0x00290500;
dsi_set_cmdq(data_array, 1, 1);
MDELAY(5);


}




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
    params->dsi.LANE_NUM                = LCM_THREE_LANE;//LCM_THREE_LANE;

    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    //params->dsi.word_count=800*3; 


	params->dsi.vertical_sync_active = 4; 
    params->dsi.vertical_backporch					=8;
    params->dsi.vertical_frontporch 				=30;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active				= 18; //20;
    params->dsi.horizontal_backporch				= 18; //20;
    params->dsi.horizontal_frontporch				= 18;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

		params->dsi.PLL_CLOCK = 272;//256;	// 256  251 

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

	init_lcm_registers();
	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

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

LCM_DRIVER lf710_hz_jd9364ac_boe_wxga_ips_101_lcm_drv = 
{
	.name = "lf710_hz_jd9364ac_boe_wxga_ips_101",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

