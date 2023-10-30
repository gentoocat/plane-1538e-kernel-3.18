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

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static void init_lcm_registers(void)
{		
	unsigned int data_array[16];

	//JD9366 initial code
    //Page0
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //--- PASSWORD  ----//
    data_array[0] = 0x93E11500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x65E21500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xF8E31500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Page0
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    //--- Sequence Ctrl  ----//
    data_array[0] = 0x10701500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x13711500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x06721500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //--- Page1  ----//
    data_array[0] = 0x01E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Set VCOM
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xA0011500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    //Set VCOM_Reverse
    data_array[0] = 0x00031500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xA0041500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Set Gamma Power, VGMP,VGMN,VGSP,VGSN
    data_array[0] = 0x00171500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xB1181500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x01191500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x001A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xB11B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x011C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Set Gate Power
    data_array[0] = 0x3E1F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x2D201500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x2D211500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0E221500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //SETPANEL
    data_array[0] = 0x19371500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //SET RGBCYC
    data_array[0] = 0x05381500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x08391500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x123A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x783C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x803E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x803F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    
    //Set TCON
    data_array[0] = 0x06401500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0xA0411500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //--- power voltage  ----//
    data_array[0] = 0x01551500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x01561500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x69571500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0A581500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0A591500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x285A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x195B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    
    
    //--- Gamma  ----//
    data_array[0] = 0x7C5D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x605E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x505F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x43601500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x3F611500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x30621500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x34631500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1E641500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x37651500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x35661500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x34671500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x52681500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x3F691500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x446A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x356B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x2F6C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x216D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x106E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x006F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x7C701500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x60711500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x50721500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x43731500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x3F741500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x30751500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x34761500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1E771500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x37781500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x35791500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x347A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x527B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x3F7C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x447D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x357E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x2F7F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x21801500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x10811500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00821500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Page2, for GIP
    data_array[0] = 0x02E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //GIP_L Pin mapping
    data_array[0] = 0x47001500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x47011500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x45021500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x45031500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x4B041500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x4B051500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x49061500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x49071500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x41081500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F091500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F0A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F0B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F0C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F0D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F0E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x430F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F101500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F111500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F121500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F131500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F141500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F151500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //GIP_R Pin mapping
    data_array[0] = 0x46161500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x46171500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x44181500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x44191500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x4A1A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x4A1B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x481C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x481D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x401E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F1F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F201500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F211500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F221500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F231500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F241500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x42251500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F261500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F271500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F281500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F291500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F2A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F2B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //GIP_L_GS Pin mapping
    data_array[0] = 0x112C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0F2D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0D2E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0B2F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x09301500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x07311500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x05321500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x18331500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x17341500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F351500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x01361500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F371500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F381500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F391500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F3A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F3B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F3C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F3D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F3E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x133F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F401500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F411500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //GIP_R_GS Pin mapping
    data_array[0] = 0x10421500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0E431500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0C441500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x0A451500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x08461500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x06471500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x04481500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x18491500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x174A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F4B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x004C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F4D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F4E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F4F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F501500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F511500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F521500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F531500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F541500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x12551500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F561500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F571500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //GIP Timing
    data_array[0] = 0x40581500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00591500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x005A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x305B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x035C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x305D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x015E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x025F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00601500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x01611500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x02621500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x03631500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x6B641500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00651500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00661500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x73671500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x05681500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x06691500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x6B6A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x086B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x006C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x046D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x046E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x886F1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00701500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00711500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x06721500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x7B731500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00741500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x07751500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x00761500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x5D771500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x17781500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x1F791500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x007A1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x007B1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x007C1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x037D1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x7B7E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Page1
    data_array[0] = 0x01E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x010E1500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Page3
    data_array[0] = 0x03E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    data_array[0] = 0x2F981500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //Page0
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(&data_array[0], 1, 1);
    
    //SLP OUT
    //SSD_CMD(0x11);  	// SLPOUT
    //Delayms(120);
	data_array[0] = 0x00110500;       
	dsi_set_cmdq(&data_array[0], 1, 1);
	MDELAY(120);

//DISP ON
//SSD_CMD(0x29);  	// DSPON
//Delayms(5);
	data_array[0] = 0x00290500;               
	dsi_set_cmdq(&data_array[0], 1, 1);
    MDELAY(5);
	}

	
/*
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
*/

static void lcd_reset(unsigned char enabled)
{
#ifdef BUILD_LK
	printf("[IND][K] y___0%s : %s\n", __func__, enabled ? "on" : "off");
#else
	printk("[IND][LK] Y___1%s : %s\n", __func__, enabled ? "on" : "off");
#endif

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


	params->dsi.mode   = BURST_VDO_MODE;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	//params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	//params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	//params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	//params->dsi.packet_size=256;

	// Video mode setting		
	//params->dsi.intermediat_buffer_num = 2; //because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	//params->dsi.word_count=720*3;	
	//params->dsi.word_count=800*3;	


	params->dsi.vertical_sync_active = 4;//4; //4;
	params->dsi.vertical_backporch =  4;//4; //14;	//16;
	params->dsi.vertical_frontporch = 8;//8; //16; //15;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 20; //4;
	params->dsi.horizontal_backporch =	20; //42;  //50;
	params->dsi.horizontal_frontporch = 32; //16; //44; //60;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;

	params->dsi.PLL_CLOCK=250; //

}



static void lcm_init(void)
{
   
#if 1 //def BUILD_LK
	//printf("%s, LK \n", __func__);
	lcd_reset(0);
	lcd_power_en(0);
	lcd_power_en(1);
	MDELAY(30);//Must > 5ms
	lcd_reset(1);
	MDELAY(20);//Must > 5ms
	lcd_reset(0);
	MDELAY(30);//Must > 5ms
	lcd_reset(1);
        MDELAY(120);
        //DSI_clk_HS_mode(1);		//lori modify 20151112, for after resume ,system failure

	//MDELAY(30);//Must > 50ms

	init_lcm_registers();
	
	//dsi_set_cmdq_V3(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(LCM_setting_table_V3), 1);
	MDELAY(120);
	lcm_set_gpio_output(GPIO_LCD_BL_EN,1);

#else
	printk("%s, kernel", __func__);
#endif

}




static void lcm_suspend(void)
{
	/*unsigned int data_array[16];

#ifdef BUILD_LK
	printf("%s, LK \n", __func__);
#else
	printk("%s, kernel", __func__);
#endif

	lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
        MDELAY(200);
        
        data_array[0] = 0x00043902;
	data_array[1] = 0xF46193BF;
	dsi_set_cmdq(&data_array[0], 2, 1);
	
	data_array[0] = 0x00280500;  //display off						  
	dsi_set_cmdq(&data_array[0], 1, 1);
	MDELAY(50);
	data_array[0] = 0x00100500;  //display off						  
	dsi_set_cmdq(&data_array[0], 1, 1);
	MDELAY(130);

	lcd_reset(0);
	MDELAY(10);*/
	lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
    MDELAY(100);	
	
	lcd_power_en(0);
	//DSI_clk_HS_mode(0);
	//MDELAY(10);
}



static void lcm_resume(void)
{
	/*
#ifdef BUILD_LK
	printf("%s, LK \n", __func__);
#else
	printk("%s, kernel", __func__);
#endif
*/
	lcm_init();
}

LCM_DRIVER l696_hz_jd9366_wxga_lcm_drv = 
{
	.name = "l696_hz_jd9366_wxga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
};

