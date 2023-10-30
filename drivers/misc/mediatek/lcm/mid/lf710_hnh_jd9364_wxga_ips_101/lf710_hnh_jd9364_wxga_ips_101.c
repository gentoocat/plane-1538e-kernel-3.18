/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE 
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/


#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"


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


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  (800) //(1280)
#define FRAME_HEIGHT (1280) //(800)
  
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#define GPIO_LCD_POWER_EN     GPIO64 
#define GPIO_LCM_RST     GPIO146

//#define GPIO_LCD_POWER_EN    GPIO93
//#define GPIO_DISP_RST_PIN	GPIO117

//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER
//extern void DSI_clk_HS_mode(unsigned char enter);

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
static LCM_UTIL_FUNCS lcm_util = {
	.set_gpio_out = NULL,
};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

static void lcd_power_en(unsigned char enabled)
{
    if(enabled)
    {
		mt_set_gpio_mode(GPIO_LCD_POWER_EN, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_POWER_EN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_POWER_EN, GPIO_OUT_ONE);
	}
    else
    {
    	mt_set_gpio_mode(GPIO_LCD_POWER_EN, GPIO_MODE_00);
        mt_set_gpio_dir(GPIO_LCD_POWER_EN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_POWER_EN, GPIO_OUT_ZERO);
	}
	MDELAY(50);
}

static void lcd_reset(unsigned char enabled)
{
    if(enabled)
    {
        mt_set_gpio_mode(GPIO_LCM_RST,GPIO_MODE_00);//reset
        mt_set_gpio_dir(GPIO_LCM_RST,GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ONE);
    }
    else
    {
        mt_set_gpio_mode(GPIO_LCM_RST,GPIO_MODE_00);//reset
        mt_set_gpio_dir(GPIO_LCM_RST,GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_RST,GPIO_OUT_ZERO);
    }
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
    data_array[0] = 0x03801500;
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
    data_array[0] = 0x032D1500;
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
    
    data_array[0] = 0x00351500;
    dsi_set_cmdq(data_array, 1, 1);
    
    data_array[0] = 0x00E01500;
    dsi_set_cmdq(data_array, 1, 1);

	}

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

	params->dsi.mode    = BURST_VDO_MODE;
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;

	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	//params->dsi.word_count=800*3;	


	params->dsi.vertical_sync_active = 4;
	params->dsi.vertical_backporch =  10;
	params->dsi.vertical_frontporch = 30;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 18;
	params->dsi.horizontal_backporch =	18; 
	params->dsi.horizontal_frontporch = 18;
	params->dsi.horizontal_active_pixel = FRAME_WIDTH;


	params->dsi.PLL_CLOCK=180; //225
	
#if defined(BUILD_LK)
	printf("[LCD] PLL_CLOCK=%d\n",params->dsi.PLL_CLOCK);
#else
    printk("[LCD] PLL_CLOCK=%d\n",params->dsi.PLL_CLOCK);
#endif
    params->dsi.cont_clock = 1;
    	
}

static void lcm_init(void)
{
	//static int lcm_inited = 0;

	// step1: power on
	lcd_reset(0);
	lcd_power_en(0);
	lcd_power_en(1);
	MDELAY(20);//Must > 5ms
	lcd_reset(1);
	MDELAY(10);//Must > 5ms
	lcd_reset(0);
	MDELAY(10);//Must > 5ms
	lcd_reset(1);
    MDELAY(120);
	init_lcm_registers();
	
}


static void lcm_suspend(void)
{
	//unsigned int data_array[16];

#ifdef BUILD_LK
	printf("%s, LK \n", __func__);
#else
	printk("%s, kernel", __func__);
#endif

//	lcm_set_gpio_output(GPIO_LCD_BL_EN,0);
    MDELAY(200);

	lcd_reset(0);
	MDELAY(10);
	lcd_power_en(0);
	//DSI_clk_HS_mode(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}

       
LCM_DRIVER lf710_hnh_jd9364_wxga_ips_101_lcm_drv= 
{
	.name			= "lf710_hnh_jd9364_wxga_ips_101",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	//.compare_id     = lcm_compare_id,
	//.esd_check      = lcm_esd_check,
    //.esd_recover    = lcm_esd_recover,
};
