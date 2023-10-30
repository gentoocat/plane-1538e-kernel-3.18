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



static void lcm_initial_registers(void)
{		
    unsigned int data_array[16];
    
#ifdef BUILD_LK
    printf("%s, LK \n", __func__);
#else
    printk("%s, kernel", __func__);
#endif
    
	
	data_array[0] = 0x00053902;
	data_array[1] = 0x2555AAFF;
	data_array[2] = 0x00000001;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x166F1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x10F71500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000008;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x016F1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x01B11500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x80C81500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x0CA001BD;
	data_array[2] = 0x00000108;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x01B61500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x026F1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x0CB81500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001111BB;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000505BC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x01C71500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000108;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000A0AB0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000A0AB1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x0000A8BC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x0000A8BD;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00CA1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x04C01500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000303B5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x30BE1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003737B3;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000F0FB4;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000505B8;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x004646B9;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001515BA;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000208;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x01EE1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00113902;
	data_array[1] = 0x000000B0;
	data_array[2] = 0x00340013;
	data_array[3] = 0x0067004F;
	data_array[4] = 0x00AE008E;
	data_array[5] = 0x000000E3;
	dsi_set_cmdq( data_array, 6, 1);

	data_array[0] = 0x00113902;
	data_array[1] = 0x010C01B1;
	data_array[2] = 0x0181014E;
	data_array[3] = 0x021202D1;
	data_array[4] = 0x024E0214;
	data_array[5] = 0x0000008E;
	dsi_set_cmdq( data_array, 6, 1);

	data_array[0] = 0x00113902;
	data_array[1] = 0x02B702B2;
	data_array[2] = 0x030F03ED;
	data_array[3] = 0x03540339;
	data_array[4] = 0x03860374;
	data_array[5] = 0x0000009A;
	dsi_set_cmdq( data_array, 6, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x03AC03B3;
	data_array[2] = 0x000000FF;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000608;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002E2DB0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002A29B1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001816B2;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001210B3;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003100B4;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000231B6;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B7;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B8;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131B9;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BA;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BB;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003103BD;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131BE;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000131BF;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001113C0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001719C1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00292AC2;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002D2EC3;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131E5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002D2EC4;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002A29C5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001113C6;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001719C7;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003103C8;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131C9;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000131CA;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CB;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CD;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CE;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131CF;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131D0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003100D1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131D2;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000231D3;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001816D4;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x001210D5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x00292AD6;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002E2DD7;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x003131E6;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x000000D8;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x000000D9;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00E71500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x30ED1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000020B1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000020B0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00E51500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B0;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00B81500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B1;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000300B9;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B2;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000000BA;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B3;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000302BB;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B4;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B5;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B6;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000617B7;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x000302BC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x06E51500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x06E61500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00E71500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x06E81500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x06E91500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x06EA1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00EB1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00EC1500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x0AC01500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x08C11500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0xA6C21500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x05C31500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x860004B2;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x860004B3;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x170004B4;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x170004B5;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00C41500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x02C51500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x22C61500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x03C71500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x190002B6;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x190002B7;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x190002B8;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x190002B9;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002006C8;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x002002C9;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x006001CA;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00033902;
	data_array[1] = 0x006001CB;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x860044BA;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x860044BB;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x1A0044BC;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x1A0044BD;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x010500D1;
	data_array[2] = 0x00001007;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x050510D2;
	data_array[2] = 0x00001003;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x430020D3;
	data_array[2] = 0x00001007;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x430030D4;
	data_array[2] = 0x00001007;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00083902;
	data_array[1] = 0x000000D0;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x000C3902;
	data_array[1] = 0x000000D5;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq( data_array, 4, 1);

	data_array[0] = 0x000C3902;
	data_array[1] = 0x000000D6;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq( data_array, 4, 1);

	data_array[0] = 0x000C3902;
	data_array[1] = 0x000000D7;
	data_array[2] = 0x00000000;
	data_array[3] = 0x00000000;
	dsi_set_cmdq( data_array, 4, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x000000D8;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000508;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x3C0000CC;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x3C0000CD;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x3C0000CE;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00043902;
	data_array[1] = 0x3C0000CF;
	dsi_set_cmdq( data_array, 2, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x003400C0;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x340000C1;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x340000C2;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00053902;
	data_array[1] = 0x340000C3;
	data_array[2] = 0x00000000;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000308;
	dsi_set_cmdq( data_array, 3, 1);

	data_array[0] = 0x60C41500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0xC0C51500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00C61500;
	dsi_set_cmdq( data_array, 1, 1);

	data_array[0] = 0x00C71500;
	dsi_set_cmdq( data_array, 1, 1);


	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(150);
	
	data_array[0]= 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(30);
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy( &lcm_util, util, sizeof(LCM_UTIL_FUNCS));
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
		params->dsi.LANE_NUM				= LCM_THREE_LANE;//LCM_TWO_LANE;
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
        params->dsi.word_count=FRAME_WIDTH*3;
		
    params->dsi.vertical_sync_active				= 2;	//2;
    params->dsi.vertical_backporch					= 20;	//18;
    params->dsi.vertical_frontporch 				= 20;	//14;
    params->dsi.vertical_active_line				= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active				= 10;	//24;
    params->dsi.horizontal_backporch				= 90;	//24;
    params->dsi.horizontal_frontporch				= 90;	//72;
    params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 308;	//300;	//230; //260;
		
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
	lcm_initial_registers();

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




LCM_DRIVER l710g_px101ih27810178a_nt35521s_wxga_ips_lcm_drv = 
{
  .name				= "l710g_px101ih27810178a_nt35521s_wxga_ips",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,

};
