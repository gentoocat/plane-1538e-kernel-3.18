/*****************************************************************************
 *
 * Filename:
 * ---------
 *    charging_pmic.c
 *
 * Project:
 * --------
 *   ALPS_Software
 *
 * Description:
 * ------------
 *   This file implements the interface between BMT and ADC scheduler.
 *
 * Author:
 * -------
 *  Oscar Liu
 *
 *============================================================================
  * $Revision:   1.0  $
 * $Modtime:   11 Aug 2005 10:28:16  $
 * $Log:   //mtkvs01/vmdata/Maui_sw/archives/mcu/hal/peripheral/inc/bmt_chr_setting.h-arc  $
 *
 * 03 05 2015 wy.chuang
 * [ALPS01921641] [L1_merge] for PMIC and charging
 * .
 *
 * 12 01 2014 wy.chuang
 * [ALPS01814017] MT6328 check in
 * .
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
 #if 0
#include <mach/upmu_common.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <mach/upmu_hw.h>
#include <linux/xlog.h>
#include <linux/delay.h>
#include <mach/mt_sleep.h>
#include <mach/mt_boot.h>
#include <mach/system.h>
#include <mt-plat/charging.h>
#include <mach/mt_charging.h>
#include <mach/mt_pwm.h>
#endif
#include <linux/types.h>
#include <mt-plat/charging.h>
#include <mt-plat/upmu_common.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <mt-plat/mt_boot.h>
#include <mt-plat/battery_common.h>
#include <mach/mt_charging.h>
#include <mach/mt_pmic.h>
#include <mt-plat/mt_pwm.h>
#include <mt-plat/mt_gpio.h>
#ifdef CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT
#include <mt-plat/diso.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#ifdef MTK_DISCRETE_SWITCH
#include <mt-plat/mt_gpio.h>
#endif
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
#include <linux/kthread.h>
#include <linux/wakelock.h>
#include <linux/mutex.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#endif
#endif
 // ============================================================ //
 //define
 // ============================================================ //
#define STATUS_OK	0
#define STATUS_UNSUPPORTED	-1
#define GETARRAYNUM(array) (sizeof(array)/sizeof(array[0]))


 // ============================================================ //
 //global variable
 // ============================================================ //
static CHARGER_TYPE g_charger_type = CHARGER_UNKNOWN;

#if defined(MTK_WIRELESS_CHARGER_SUPPORT)
#define WIRELESS_CHARGER_EXIST_STATE 0
int wireless_charger_gpio_number   = (168 | 0x80000000); 
#endif

#if 0
#include <cust_gpio_usage.h>
int gpio_number   = GPIO_SWCHARGER_EN_PIN; 
int gpio_off_mode = GPIO_SWCHARGER_EN_PIN_M_GPIO;
int gpio_on_mode  = GPIO_SWCHARGER_EN_PIN_M_GPIO;
//#else
int gpio_number   = (78 | 0x80000000); 
int gpio_off_mode = 0;
int gpio_on_mode  = 0;

static int gpio_int_number   = (3 | 0x80000000); 
#define CHARGER_PWM (78 | 0x80000000)
#define CHARGER_PWM_NUM 0

#endif
#define CHARGER_PWM_NUM 2
//int gpio_off_dir  = GPIO_DIR_OUT;
//int gpio_off_out  = GPIO_OUT_ONE;
//int gpio_on_dir   = GPIO_DIR_OUT;
//int gpio_on_out   = GPIO_OUT_ZERO;
static int gpio_int_number   = (0 | 0x80000000); 
static kal_bool charging_type_det_done = KAL_TRUE;
kal_bool chargin_hw_init_done = KAL_TRUE; 

//add by cxw
#define CHARGERPWM_OFF 0
#define CHARGERPWM_ON 1
#define CHARGERPWM_BLINK 2
//static int pwm_num = 4;

//static int charging_current = 0;
//static struct kobject *dev_kobj = NULL;
//static int g_current = 0;


struct pwm_settings  {   
  	u8 chargerpwm_mode; //0, off; 1, on; 2, blink;   
  	u32 pwm_data_width ;	 
	u32 pwm_thresh; 
};
struct pwm_settings charger_pwm;


/*
void mt_led_pwm_disable(int pwm_num)
{
	struct cust_mt65xx_led *cust_led_list = get_cust_led_list();
	mt_pwm_disable(pwm_num, cust_led_list->config_data.pmic_pad);
}
*/

static int charging_set_charge_pwm(int pwm_num,struct pwm_settings *charger_pwm)
{
	  
	 struct pwm_spec_config pwm_setting; //in pwm/mt_pwm.c
	 pwm_setting.pwm_no = pwm_num;
	 pwm_setting.mode = PWM_MODE_OLD; //select a kind of mode, such as old mode
	 /* We won't choose 32K to be the clock src of old mode because of system performance. */
	 /* The setting here will be clock src = 26MHz, CLKSEL = 26M/1625 (i.e. 16K) */
	 pwm_setting.clk_src = PWM_CLK_OLD_MODE_32K;

	 battery_log(BAT_LOG_CRTI, "eta charging_set_charge_pwm charger_pwm->chargerpwm_mode = %d\n", charger_pwm->chargerpwm_mode);
 	
	 battery_log(BAT_LOG_CRTI, "eta charging_set_charge_pwm pwm_data_width = %d, pwm_thresh = %d\n", charger_pwm->pwm_data_width, charger_pwm->pwm_thresh);
	 switch (charger_pwm->chargerpwm_mode)
	 {
	 case CHARGERPWM_ON: 
		 pwm_setting.PWM_MODE_OLD_REGS.THRESH = 0;
		 pwm_setting.clk_div = CLK_DIV1; //in include/platform/mt_pwm.h
		 pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = 255;
		 break;
 
	 case CHARGERPWM_OFF :
		 pwm_setting.PWM_MODE_OLD_REGS.THRESH = 255;	 //reference value
		 pwm_setting.clk_div = CLK_DIV1; //in include/platform/mt_pwm.h
		 pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = 255;
		 break;
 
	 case CHARGERPWM_BLINK :
		 //time_index = find_time_index(charger_pwm->blink_on_time + charger_pwm->blink_off_time);
		 pwm_setting.clk_div = CLK_DIV1;
		// pwm_setting.clk_div = time_index;
		 //pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = (charger_pwm->blink_on_time + charger_pwm->blink_off_time) * MIN_FRE_OLD_PWM / div_array_hal[time_index];
		 //pwm_setting.PWM_MODE_OLD_REGS.THRESH = (charger_pwm->blink_on_time*100) / (charger_pwm->blink_on_time + charger_pwm->blink_off_time);
		 pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = charger_pwm->pwm_data_width;
		 pwm_setting.PWM_MODE_OLD_REGS.THRESH = charger_pwm->pwm_thresh;
		 
		 battery_log(BAT_LOG_CRTI, "eta charging_set_charge_pwm pwm_level = %d\n", charger_pwm->pwm_thresh);
		 
	 }
	 
	 pwm_setting.PWM_MODE_FIFO_REGS.IDLE_VALUE = 0;
	 pwm_setting.PWM_MODE_FIFO_REGS.GUARD_VALUE = 0;
	 pwm_setting.PWM_MODE_FIFO_REGS.GDURATION = 0;
	 pwm_setting.PWM_MODE_FIFO_REGS.WAVE_NUM = 0;
	 pwm_set_spec_config(&pwm_setting); 		 //in pwm/mt_pwm.c	 
 
	 return 0;
  }


static const u32 VBAT_CV_VTH[]=
{
	BATTERY_VOLT_03_500000_V,   BATTERY_VOLT_03_520000_V,	BATTERY_VOLT_03_540000_V,   BATTERY_VOLT_03_560000_V,
	BATTERY_VOLT_03_580000_V,   BATTERY_VOLT_03_600000_V,	BATTERY_VOLT_03_620000_V,   BATTERY_VOLT_03_640000_V,
	BATTERY_VOLT_03_660000_V,	BATTERY_VOLT_03_680000_V,	BATTERY_VOLT_03_700000_V,	BATTERY_VOLT_03_720000_V,
	BATTERY_VOLT_03_740000_V,	BATTERY_VOLT_03_760000_V,	BATTERY_VOLT_03_780000_V,	BATTERY_VOLT_03_800000_V,
	BATTERY_VOLT_03_820000_V,	BATTERY_VOLT_03_840000_V,	BATTERY_VOLT_03_860000_V,	BATTERY_VOLT_03_880000_V,
	BATTERY_VOLT_03_900000_V,	BATTERY_VOLT_03_920000_V,	BATTERY_VOLT_03_940000_V,	BATTERY_VOLT_03_960000_V,
	BATTERY_VOLT_03_980000_V,	BATTERY_VOLT_04_000000_V,	BATTERY_VOLT_04_020000_V,	BATTERY_VOLT_04_040000_V,
	BATTERY_VOLT_04_060000_V,	BATTERY_VOLT_04_080000_V,	BATTERY_VOLT_04_100000_V,	BATTERY_VOLT_04_120000_V,
	BATTERY_VOLT_04_140000_V,   BATTERY_VOLT_04_160000_V,	BATTERY_VOLT_04_180000_V,   BATTERY_VOLT_04_200000_V,
	BATTERY_VOLT_04_220000_V,   BATTERY_VOLT_04_240000_V,	BATTERY_VOLT_04_260000_V,   BATTERY_VOLT_04_280000_V,
	BATTERY_VOLT_04_300000_V,   BATTERY_VOLT_04_320000_V,	BATTERY_VOLT_04_340000_V,   BATTERY_VOLT_04_360000_V,	
	BATTERY_VOLT_04_380000_V,   BATTERY_VOLT_04_400000_V,	BATTERY_VOLT_04_420000_V,   BATTERY_VOLT_04_440000_V	
	
};

static const u32 CS_VTH[]=
{
	CHARGE_CURRENT_550_00_MA,   CHARGE_CURRENT_650_00_MA,	CHARGE_CURRENT_750_00_MA, CHARGE_CURRENT_850_00_MA,
	CHARGE_CURRENT_950_00_MA,   CHARGE_CURRENT_1050_00_MA,	CHARGE_CURRENT_1150_00_MA, CHARGE_CURRENT_1250_00_MA,
	CHARGE_CURRENT_1350_00_MA,  CHARGE_CURRENT_1450_00_MA,  CHARGE_CURRENT_1500_00_MA,  CHARGE_CURRENT_1600_00_MA,	
	CHARGE_CURRENT_1700_00_MA, CHARGE_CURRENT_1800_00_MA,	CHARGE_CURRENT_1900_00_MA,	CHARGE_CURRENT_2000_00_MA
}; 

static const u32 CS_VHT_1[]=
{
	186,   173,	160, 147,
	134,   121,	108, 95,
	82,  69,  56,  52,	
	39, 26,	13,	0
}; 

static const u32 INPUT_CS_VTH[]=
 {
	 CHARGE_CURRENT_100_00_MA,	 CHARGE_CURRENT_500_00_MA,	 CHARGE_CURRENT_800_00_MA, CHARGE_CURRENT_MAX
 }; 

static const u32 VCDT_HV_VTH[]=
 {
	  BATTERY_VOLT_04_200000_V, BATTERY_VOLT_04_250000_V,	  BATTERY_VOLT_04_300000_V,   BATTERY_VOLT_04_350000_V,
	  BATTERY_VOLT_04_400000_V, BATTERY_VOLT_04_450000_V,	  BATTERY_VOLT_04_500000_V,   BATTERY_VOLT_04_550000_V,
	  BATTERY_VOLT_04_600000_V, BATTERY_VOLT_06_000000_V,	  BATTERY_VOLT_06_500000_V,   BATTERY_VOLT_07_000000_V,
	  BATTERY_VOLT_07_500000_V, BATTERY_VOLT_08_500000_V,	  BATTERY_VOLT_09_500000_V,   BATTERY_VOLT_10_500000_V		  
 };

#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
#define SW_POLLING_PERIOD 100	/* 100 ms */
#define MSEC_TO_NSEC(x)		(x * 1000000UL)
static DEFINE_MUTEX(diso_polling_mutex);
static DECLARE_WAIT_QUEUE_HEAD(diso_polling_thread_wq);
static struct hrtimer diso_kthread_timer;
static kal_bool diso_thread_timeout = KAL_FALSE;
static struct delayed_work diso_polling_work;
static void diso_polling_handler(struct work_struct *work);
static DISO_Polling_Data DISO_Polling;
#else
DISO_IRQ_Data DISO_IRQ;
#endif
int g_diso_state	= 0;
static char *DISO_state_s[8] = {
	"IDLE",
	"OTG_ONLY",
	"USB_ONLY",
	"USB_WITH_OTG",
	"DC_ONLY",
	"DC_WITH_OTG",
	"DC_WITH_USB",
	"DC_USB_OTG",
};
#endif
 // ============================================================ //
 // function prototype
 // ============================================================ //
 
 
 // ============================================================ //
 //extern variable
 // ============================================================ //
 
 // ============================================================ //
 //extern function
 // ============================================================ //
 extern u32 upmu_get_reg_value(u32 reg);
 extern bool mt_usb_is_device(void);
 extern void Charger_Detect_Init(void);
 extern void Charger_Detect_Release(void);
 extern void mt_power_off(void);
 
 // ============================================================ //
static u32 charging_value_to_parameter(const u32 *parameter, const u32 array_size, const u32 val)
{
	if (val < array_size)
	{
		return parameter[val];
	}
	else
	{
		battery_log(BAT_LOG_CRTI, "Can't find the parameter \r\n");	
		return parameter[0];
	}
}

 
static u32 charging_parameter_to_value(const u32 *parameter, const u32 array_size, const u32 val)
{
	u32 i;

	for(i=0;i<array_size;i++)
	{
		if (val == *(parameter + i))
		{
				return i;
		}
	}

    battery_log(BAT_LOG_CRTI, "NO register value match \r\n");
	//TODO: ASSERT(0);	// not find the value
	return 0;
}


 static u32 bmt_find_closest_level(const u32 *pList,u32 number,u32 level)
 {
	 u32 i;
	 u32 max_value_in_last_element;
 
	 if(pList[0] < pList[1])
		 max_value_in_last_element = KAL_TRUE;
	 else
		 max_value_in_last_element = KAL_FALSE;
 
	 if(max_value_in_last_element == KAL_TRUE)
	 {
		 for(i = (number-1); i != 0; i--)	 //max value in the last element
		 {
			 if(pList[i] <= level)
			 {
				 return pList[i];
			 }	  
		 }

 		 battery_log(BAT_LOG_CRTI, "Can't find closest level, small value first \r\n");
		 return pList[0];
		 //return CHARGE_CURRENT_0_00_MA;
	 }
	 else
	 {
		 for(i = 0; i< number; i++)  // max value in the first element
		 {
			 if(pList[i] <= level)
			 {
				 return pList[i];
			 }	  
		 }

		 battery_log(BAT_LOG_CRTI, "Can't find closest level, large value first \r\n"); 	 
		 return pList[number -1];
  		 //return CHARGE_CURRENT_0_00_MA;
	 }
 }


static void hw_bc11_dump_register(void)
{
	u32 reg_val = 0;
	u32 reg_num = MT6328_CHR_CON20;
	u32 i = 0;

	for(i=reg_num ; i<=MT6328_CHR_CON21 ; i+=2)
	{
		reg_val = upmu_get_reg_value(i);
		battery_log(BAT_LOG_FULL, "Chr Reg[0x%x]=0x%x \r\n", i, reg_val);
	}
}


static void hw_bc11_init(void)
{
    msleep(200);
    Charger_Detect_Init();
        
    //RG_bc11_BIAS_EN=1
    bc11_set_register_value(PMIC_RG_BC11_BIAS_EN,1);
    //RG_bc11_VSRC_EN[1:0]=00
    bc11_set_register_value(PMIC_RG_BC11_VSRC_EN,0);
    //RG_bc11_VREF_VTH = [1:0]=00
    bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0);
    //RG_bc11_CMP_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0);
    //RG_bc11_IPU_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0);
    //RG_bc11_IPD_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0);
    //bc11_RST=1
    bc11_set_register_value(PMIC_RG_BC11_RST,1);
    //bc11_BB_CTRL=1
    bc11_set_register_value(PMIC_RG_BC11_BB_CTRL,1);

    msleep(50);
    //mdelay(50);
    
    if(Enable_BATDRV_LOG == BAT_LOG_FULL)
    {
        battery_log(BAT_LOG_FULL, "hw_bc11_init() \r\n");
        hw_bc11_dump_register();
    }    

}

static u32 hw_bc11_DCD(void)
{
    u32 wChargerAvail = 0;

    //RG_bc11_IPU_EN[1.0] = 10
    bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0x2);  
    //RG_bc11_IPD_EN[1.0] = 01
    bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x1);
    //RG_bc11_VREF_VTH = [1:0]=01
    bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x1);
    //RG_bc11_CMP_EN[1.0] = 10
    bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x2);

    msleep(80);
    //mdelay(80);

    wChargerAvail = bc11_get_register_value(PMIC_RGS_BC11_CMP_OUT);
    
    if(Enable_BATDRV_LOG == BAT_LOG_FULL)
    {
        battery_log(BAT_LOG_FULL, "hw_bc11_DCD() \r\n");
        hw_bc11_dump_register();
    }
    
    //RG_bc11_IPU_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0x0);
    //RG_bc11_IPD_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x0);
    //RG_bc11_CMP_EN[1.0] = 00
    bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x0);
    //RG_bc11_VREF_VTH = [1:0]=00
    bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x0);


    return wChargerAvail;
}
 

static u32 hw_bc11_stepA1(void)
{
   u32 wChargerAvail = 0;
     
   //RG_bc11_IPD_EN[1.0] = 01
   bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x1);
   //RG_bc11_VREF_VTH = [1:0]=00
   bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x0);
   //RG_bc11_CMP_EN[1.0] = 01
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x1);

   msleep(80);
   //mdelay(80);

   wChargerAvail = bc11_get_register_value(PMIC_RGS_BC11_CMP_OUT);
				
   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_log(BAT_LOG_FULL, "hw_bc11_stepA1() \r\n");
       hw_bc11_dump_register();
   }

   //RG_bc11_IPD_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x0);
   //RG_bc11_CMP_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x0);

   return  wChargerAvail;
}
 
 /*
 static u32 hw_bc11_stepB1(void)
 {
	u32 wChargerAvail = 0;
	  
	//RG_BC11_IPU_EN[1.0] = 01
	//upmu_set_rg_bc11_ipu_en(0x1);
	upmu_set_rg_bc11_ipd_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	//upmu_set_rg_bc11_vref_vth(0x2);
	upmu_set_rg_bc11_vref_vth(0x0);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);
 
	//msleep(80);
	mdelay(80);
 
	wChargerAvail = upmu_get_rgs_bc11_cmp_out();
 
	if(Enable_BATDRV_LOG == BAT_LOG_FULL)
	{
		battery_log(BAT_LOG_FULL, "hw_bc11_stepB1() \r\n");
		hw_bc11_dump_register();
	}
 
	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);
 
	return  wChargerAvail;
 }
 
 
 static u32 hw_bc11_stepC1(void)
 {
	u32 wChargerAvail = 0;
	  
	//RG_BC11_IPU_EN[1.0] = 01
	upmu_set_rg_bc11_ipu_en(0x1);
	//RG_BC11_VREF_VTH = [1:0]=10
	upmu_set_rg_bc11_vref_vth(0x2);
	//RG_BC11_CMP_EN[1.0] = 01
	upmu_set_rg_bc11_cmp_en(0x1);
 
	//msleep(80);
	mdelay(80);
 
	wChargerAvail = upmu_get_rgs_bc11_cmp_out();
 
	if(Enable_BATDRV_LOG == BAT_LOG_FULL)
	{
		battery_log(BAT_LOG_FULL, "hw_bc11_stepC1() \r\n");
		hw_bc11_dump_register();
	}
 
	//RG_BC11_IPU_EN[1.0] = 00
	upmu_set_rg_bc11_ipu_en(0x0);
	//RG_BC11_CMP_EN[1.0] = 00
	upmu_set_rg_bc11_cmp_en(0x0);
	//RG_BC11_VREF_VTH = [1:0]=00
	upmu_set_rg_bc11_vref_vth(0x0);
 
	return  wChargerAvail;
 }
 */

static u32 hw_bc11_stepA2(void)
{
   u32 wChargerAvail = 0;
     
   //RG_bc11_VSRC_EN[1.0] = 10 
   bc11_set_register_value(PMIC_RG_BC11_VSRC_EN,0x2);
   //RG_bc11_IPD_EN[1:0] = 01
   bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x1);
   //RG_bc11_VREF_VTH = [1:0]=00
   bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x0);
   //RG_bc11_CMP_EN[1.0] = 01
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x1);

   msleep(80);
   //mdelay(80);

   //msleep(80);
   mdelay(80);

   wChargerAvail = bc11_get_register_value(PMIC_RGS_BC11_CMP_OUT);

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_log(BAT_LOG_FULL, "hw_bc11_stepA2() \r\n");
       hw_bc11_dump_register();
   }

   //RG_bc11_VSRC_EN[1:0]=00
   bc11_set_register_value(PMIC_RG_BC11_VSRC_EN,0x0);
   //RG_bc11_IPD_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x0);
   //RG_bc11_CMP_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x0);

   return  wChargerAvail;
}
 

static u32 hw_bc11_stepB2(void)
{
   u32 wChargerAvail = 0;

   //RG_bc11_IPU_EN[1:0]=10
   bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0x2); 
   //RG_bc11_VREF_VTH = [1:0]=01
   bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x1);
   //RG_bc11_CMP_EN[1.0] = 01
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x1);

   msleep(80);
   //mdelay(80);

   wChargerAvail = bc11_get_register_value(PMIC_RGS_BC11_CMP_OUT);

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_log(BAT_LOG_FULL, "hw_bc11_stepB2() \r\n");
       hw_bc11_dump_register();
   }

   
   if (!wChargerAvail) {
       //RG_bc11_VSRC_EN[1.0] = 10 
       //mt6325_upmu_set_rg_bc11_vsrc_en(0x2);
       bc11_set_register_value(PMIC_RG_BC11_VSRC_EN,0x2); 
   }
   //RG_bc11_IPU_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0x0); 
   //RG_bc11_CMP_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x0); 
   //RG_bc11_VREF_VTH = [1:0]=00
   bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x0); 


   return  wChargerAvail;
}

static void hw_bc11_done(void)
{
   //RG_bc11_VSRC_EN[1:0]=00
   bc11_set_register_value(PMIC_RG_BC11_VSRC_EN,0x0);
   //RG_bc11_VREF_VTH = [1:0]=0
   bc11_set_register_value(PMIC_RG_BC11_VREF_VTH,0x0);
   //RG_bc11_CMP_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_CMP_EN,0x0);
   //RG_bc11_IPU_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_IPU_EN,0x0);
   //RG_bc11_IPD_EN[1.0] = 00
   bc11_set_register_value(PMIC_RG_BC11_IPD_EN,0x0);
   //RG_bc11_BIAS_EN=0
   bc11_set_register_value(PMIC_RG_BC11_BIAS_EN,0x0);


   Charger_Detect_Release();

   if(Enable_BATDRV_LOG == BAT_LOG_FULL)
   {
       battery_log(BAT_LOG_FULL, "hw_bc11_done() \r\n");
       hw_bc11_dump_register();
   }
   
}


 

 static u32 charging_hw_init(void *data)
 {
 	u32 status = STATUS_OK;
	static bool charging_init_flag = KAL_FALSE;
	
	
	//mt_set_gpio_mode(gpio_number,gpio_on_mode);  
	//mt_set_gpio_dir(gpio_number,gpio_on_dir);
	//mt_set_gpio_out(gpio_number,gpio_on_out);
	//mt_set_gpio_out(gpio_number,gpio_off_out);	//关闭充电
	
	//初始化充电状态监测脚
	mt_set_gpio_mode(gpio_int_number, 0);
	mt_set_gpio_dir(gpio_int_number,GPIO_DIR_IN);
	
	
	
#if defined(MTK_WIRELESS_CHARGER_SUPPORT)
	mt_set_gpio_mode(wireless_charger_gpio_number,0); // 0:GPIO mode
	mt_set_gpio_dir(wireless_charger_gpio_number,0); // 0: input, 1: output
#endif

//	battery_log(BAT_LOG_FULL, "gpio_number=0x%x,gpio_on_mode=%d,gpio_off_mode=%d\n", gpio_number, gpio_on_mode, gpio_off_mode);

       bc11_set_register_value(PMIC_RG_USBDL_SET,0x0);//force leave USBDL mode
       bc11_set_register_value(PMIC_RG_USBDL_RST,0x1);//force leave USBDL mode

	    
	if ( !charging_init_flag ) {   
	
	
		charging_init_flag = KAL_TRUE;
	}        
	
	//初始化背光变量, 默认PWM最大，高电平，相当于关闭充电
		charger_pwm.chargerpwm_mode = CHARGERPWM_BLINK;
		charger_pwm.pwm_data_width=255;
		charger_pwm.pwm_thresh = 255;
	
 	return status;
 }


 static u32 charging_dump_register(void *data)
 {
 	u32 status = STATUS_OK;

//	fan5405_dump_register();
   	
	return status;
 }	


 static u32 charging_enable(void *data)
 {
 	u32 status = STATUS_OK;
	u32 enable = *(u32*)(data);

	
	if(KAL_TRUE == enable)
	{
		
	
		//mt_set_gpio_mode(gpio_number,5);  
		
		
		charging_set_charge_pwm(CHARGER_PWM_NUM, &charger_pwm);
	}
	else
	{
#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
		charger_pwm.chargerpwm_mode = CHARGERPWM_OFF;
		charger_pwm.pwm_data_width=255;
		charger_pwm.pwm_thresh = 255;
		charging_set_charge_pwm(CHARGER_PWM_NUM, &charger_pwm);
#endif
#if defined(CONFIG_USB_MTK_HDRC_HCD)
   		if(mt_usb_is_device())
#endif 			
    	{
	       // mt_set_gpio_mode(gpio_number,gpio_off_mode);  
	    //    mt_set_gpio_dir(gpio_number,gpio_off_dir);
	       // mt_set_gpio_out(gpio_number,gpio_off_out);	//拉高，关闭充电
    	}
	}
		
	return status;
 }


 static u32 charging_set_cv_voltage(void *data)
 {
 	u32 status = STATUS_OK;
//	u16 register_value;

//	register_value = charging_parameter_to_value(VBAT_CV_VTH, GETARRAYNUM(VBAT_CV_VTH) ,*(u32 *)(data));
//	fan5405_set_oreg(register_value); 

	return status;
 } 	


 static u32 charging_get_current(void *data)
 {
    u32 status = STATUS_OK;
    u32 array_size;
    u32 reg_value;

    array_size = GETARRAYNUM(CS_VTH);
        reg_value=pmic_get_register_value(PMIC_RG_CS_VTH);//RG_CS_VTH
    *(u32 *)data = charging_value_to_parameter(CS_VTH,array_size,reg_value);
    
    return status;
 }  


 static u32 charging_set_current(void *data)
 {
 	u32 status = STATUS_OK;
	u32 set_chr_current;
	u32 array_size;
	u32 register_value;
//	u32 current_value = *(u32 *)data;

	u32 current_value = *(u32 *) data;
    	printk("charging current_value:%d,by eric.wang\n",current_value);
	
	array_size = GETARRAYNUM(CS_VTH);
    set_chr_current = bmt_find_closest_level(CS_VTH, array_size, *(u32 *)data);
    register_value = charging_parameter_to_value(CS_VTH, array_size ,set_chr_current);
	
	

	charger_pwm.chargerpwm_mode = CHARGERPWM_BLINK;
	charger_pwm.pwm_data_width=255;
	
	charger_pwm.pwm_thresh = CS_VHT_1[register_value];
	
	charging_set_charge_pwm(CHARGER_PWM_NUM, &charger_pwm);		//????????
	
	
	return status;
 } 	
 

 static u32 charging_set_input_current(void *data)
 {
    u32 status = STATUS_OK;
    return status;
 } 	

//????????
 static u32 charging_get_charging_status(void *data)
 {
    u32 status = STATUS_OK;
#if 1
	if(mt_get_gpio_in(gpio_int_number) == 1)
		*(u32 *)data = KAL_TRUE;
	else
		*(u32 *)data = KAL_FALSE;
#endif
    printk("charging_get_charging_status=%d,by eric.wang",*(u32 *)data );
    return status;
 } 	


 static u32 charging_reset_watch_dog_timer(void *data)
 {
	 u32 status = STATUS_OK;

	 
	 return status;
 }
 
 
  static u32 charging_set_hv_threshold(void *data)
  {
	 u32 status = STATUS_OK;
 
	 u32 set_hv_voltage;
	 u32 array_size;
	 u16 register_value;
	 u32 voltage = *(u32*)(data);
	 
	 array_size = GETARRAYNUM(VCDT_HV_VTH);
	 set_hv_voltage = bmt_find_closest_level(VCDT_HV_VTH, array_size, voltage);
	 register_value = charging_parameter_to_value(VCDT_HV_VTH, array_size ,set_hv_voltage);
        bc11_set_register_value(PMIC_RG_VCDT_HV_VTH,register_value);//upmu_set_rg_vcdt_hv_vth(register_value);
 
	 return status;
  }
 
 
  static u32 charging_get_hv_status(void *data)
  {
	   u32 status = STATUS_OK;
 
	   *(kal_bool*)(data) = bc11_get_register_value(PMIC_RGS_VCDT_HV_DET);//upmu_get_rgs_vcdt_hv_det();

	   return status;
  }


 static u32 charging_get_battery_status(void *data)
 {
	   u32 status = STATUS_OK;

	 bc11_set_register_value(PMIC_BATON_TDET_EN,1);
 	 bc11_set_register_value(PMIC_RG_BATON_EN,1); 
	   *(kal_bool*)(data) = bc11_get_register_value(PMIC_RGS_BATON_UNDET);//upmu_get_rgs_baton_undet();
	   
	   return status;
 }


 static u32 charging_get_charger_det_status(void *data)
 {
	   u32 status = STATUS_OK;
 
 #if !defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
	   *(kal_bool*)(data) = bc11_get_register_value(PMIC_RGS_CHRDET);//upmu_get_rgs_chrdet();
#else
	if (((g_diso_state >> 1) & 0x3) != 0x0 || bc11_get_register_value(PMIC_RGS_CHRDET))
		*(kal_bool*)(data) = KAL_TRUE;
	else
		*(kal_bool*)(data) = KAL_FALSE;
#endif
	   
	if( bc11_get_register_value(PMIC_RGS_CHRDET)== 0 )
		g_charger_type = CHARGER_UNKNOWN;
       
	   return status;
 }


kal_bool charging_type_detection_done(void)
{
	 return charging_type_det_done;
}


 static u32 charging_get_charger_type(void *data)
 {
	 u32 status = STATUS_OK;
#if defined(CONFIG_POWER_EXT)
	 *(CHARGER_TYPE*)(data) = STANDARD_HOST;
#else

#if defined(MTK_WIRELESS_CHARGER_SUPPORT)
	int wireless_state = 0;
	wireless_state = mt_get_gpio_in(wireless_charger_gpio_number);
	if(wireless_state == WIRELESS_CHARGER_EXIST_STATE) {
		*(CHARGER_TYPE*)(data) = WIRELESS_CHARGER;
		battery_log(BAT_LOG_CRTI, "WIRELESS_CHARGER!\r\n");
		return status;
	}
#endif
	if(g_charger_type!=CHARGER_UNKNOWN && g_charger_type!=WIRELESS_CHARGER) {
		*(CHARGER_TYPE*)(data) = g_charger_type;
		battery_log(BAT_LOG_CRTI, "return %d!\r\n", g_charger_type);
		return status;
	}

	charging_type_det_done = KAL_FALSE;

	/********* Step initial  ***************/		 
	hw_bc11_init();
 
	/********* Step DCD ***************/  
	if(1 == hw_bc11_DCD())
	{
		/********* Step A1 ***************/
		if(1 == hw_bc11_stepA1())
		{
			*(CHARGER_TYPE*)(data) = APPLE_2_1A_CHARGER;
			battery_log(BAT_LOG_CRTI, "step A1 : Apple 2.1A CHARGER!\r\n");
		}	 
		else
		{
			*(CHARGER_TYPE*)(data) = NONSTANDARD_CHARGER;
			battery_log(BAT_LOG_CRTI, "step A1 : Non STANDARD CHARGER!\r\n");
		}
	}
	else
	{
		 /********* Step A2 ***************/
		 if(1 == hw_bc11_stepA2())
		 {
			 /********* Step B2 ***************/
			 if(1 == hw_bc11_stepB2())
			 {
				 *(CHARGER_TYPE*)(data) = STANDARD_CHARGER;
				 battery_log(BAT_LOG_CRTI, "step B2 : STANDARD CHARGER!\r\n");
			 }
			 else
			 {
				 *(CHARGER_TYPE*)(data) = CHARGING_HOST;
				 battery_log(BAT_LOG_CRTI, "step B2 :  Charging Host!\r\n");
			 }
		 }
		 else
		 {
			*(CHARGER_TYPE*)(data) = STANDARD_HOST;
			 battery_log(BAT_LOG_CRTI, "step A2 : Standard USB Host!\r\n");
		 }
 
	}
 
	 /********* Finally setting *******************************/
	 hw_bc11_done();

 	charging_type_det_done = KAL_TRUE;

	g_charger_type = *(CHARGER_TYPE*)(data);
#endif
	 return status;
}

static u32 charging_get_is_pcm_timer_trigger(void *data)
{
    u32 status = STATUS_OK;

  //  if(slp_get_wake_reason() == WR_PCM_TIMER)
  //      *(kal_bool*)(data) = KAL_TRUE;
  //  else
        *(kal_bool*)(data) = KAL_FALSE;

  //  battery_log(BAT_LOG_CRTI, "slp_get_wake_reason=%d\n", slp_get_wake_reason());

    return status;
}

static u32 charging_set_platform_reset(void *data)
{
    u32 status = STATUS_OK;

    battery_log(BAT_LOG_CRTI, "charging_set_platform_reset\n");
 
    kernel_restart("battery service reboot system");
        
    return status;
}

static u32 charging_get_platform_boot_mode(void *data)
{
    u32 status = STATUS_OK;
  
    *(u32*)(data) = get_boot_mode();

    battery_log(BAT_LOG_CRTI, "get_boot_mode=%d\n", get_boot_mode());
         
    return status;
}

static u32 charging_set_power_off(void *data)
{
    u32 status = STATUS_OK;
  
    battery_log(BAT_LOG_CRTI, "charging_set_power_off\n");
    mt_power_off();
         
    return status;
}

static u32 charging_get_power_source(void *data)
{
	u32 status = STATUS_UNSUPPORTED;

	return status;
}

static u32 charging_get_csdac_full_flag(void *data)
{
	return STATUS_UNSUPPORTED;	
}

static u32 charging_set_ta_current_pattern(void *data)
{
	return STATUS_UNSUPPORTED;	
}

static u32 charging_set_error_state(void *data)
{
	return STATUS_UNSUPPORTED;	
}

#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
void set_vusb_auxadc_irq(bool enable, bool flag)
{
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_cancel(&diso_kthread_timer);
	DISO_Polling.reset_polling = KAL_TRUE;
	DISO_Polling.vusb_polling_measure.notify_irq_en = enable;
	DISO_Polling.vusb_polling_measure.notify_irq = flag;
	hrtimer_start(&diso_kthread_timer, ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)), HRTIMER_MODE_REL);
#else
	unsigned short threshold = 0;
	if (enable) {
		if (flag == 0)
			threshold = DISO_IRQ.vusb_measure_channel.falling_threshold;
		else
			threshold = DISO_IRQ.vusb_measure_channel.rising_threshold;
		threshold = (threshold * R_DISO_VBUS_PULL_DOWN)/(R_DISO_VBUS_PULL_DOWN + R_DISO_VBUS_PULL_UP);
		mt_auxadc_enableBackgroundDection(DISO_IRQ.vusb_measure_channel.number, threshold,
		DISO_IRQ.vusb_measure_channel.period, DISO_IRQ.vusb_measure_channel.debounce, flag);
	} else {
		mt_auxadc_disableBackgroundDection(DISO_IRQ.vusb_measure_channel.number);
	}
#endif
	pr_debug(" [%s] enable: %d, flag: %d!\n", __func__, enable, flag);
}
void set_vdc_auxadc_irq(bool enable, bool flag)
{
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_cancel(&diso_kthread_timer);
	DISO_Polling.reset_polling = KAL_TRUE;
	DISO_Polling.vdc_polling_measure.notify_irq_en = enable;
	DISO_Polling.vdc_polling_measure.notify_irq = flag;
	hrtimer_start(&diso_kthread_timer, ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)), HRTIMER_MODE_REL);
#else
	unsigned short threshold = 0;
	if (enable) {
		if (flag == 0)
			threshold = DISO_IRQ.vdc_measure_channel.falling_threshold;
		else
			threshold = DISO_IRQ.vdc_measure_channel.rising_threshold;
		threshold = (threshold * R_DISO_DC_PULL_DOWN)/(R_DISO_DC_PULL_DOWN + R_DISO_DC_PULL_UP);
		mt_auxadc_enableBackgroundDection(DISO_IRQ.vdc_measure_channel.number, threshold,
		DISO_IRQ.vdc_measure_channel.period, DISO_IRQ.vdc_measure_channel.debounce, flag);
	} else {
		mt_auxadc_disableBackgroundDection(DISO_IRQ.vdc_measure_channel.number);
	}
#endif
	pr_debug(" [%s] enable: %d, flag: %d!\n", __func__, enable, flag);
}
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
static void diso_polling_handler(struct work_struct *work)
{
	int trigger_channel = -1;
	int trigger_flag = -1;
	if (DISO_Polling.vdc_polling_measure.notify_irq_en)
		trigger_channel = AP_AUXADC_DISO_VDC_CHANNEL;
	else if (DISO_Polling.vusb_polling_measure.notify_irq_en)
		trigger_channel = AP_AUXADC_DISO_VUSB_CHANNEL;
	battery_log(BAT_LOG_CRTI, "[DISO]auxadc handler triggered\n");
	switch (trigger_channel) {
	case AP_AUXADC_DISO_VDC_CHANNEL:
		trigger_flag = DISO_Polling.vdc_polling_measure.notify_irq;
		battery_log(BAT_LOG_CRTI, "[DISO]VDC IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			  trigger_flag);
#ifdef MTK_DISCRETE_SWITCH	/*for DSC DC plugin handle */
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			battery_log(BAT_LOG_CRTI, " cur diso_state is %s!\n", DISO_state_s[2]);
		}
#else				/* for load switch OTG leakage handle */
		set_vdc_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			battery_log(BAT_LOG_CRTI, " cur diso_state is %s!\n", DISO_state_s[5]);
		} else if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_otg_state = DISO_ONLINE;
			battery_log(BAT_LOG_CRTI, " cur diso_state is %s!\n", DISO_state_s[1]);
		} else
			battery_log(BAT_LOG_CRTI, "[%s] wrong trigger flag!\n", __func__);
#endif
		break;
	case AP_AUXADC_DISO_VUSB_CHANNEL:
		trigger_flag = DISO_Polling.vusb_polling_measure.notify_irq;
		battery_log(BAT_LOG_CRTI, "[DISO]VUSB IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel,
			  trigger_flag);
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			battery_log(BAT_LOG_CRTI, " cur diso_state is %s!\n", DISO_state_s[4]);
		} else if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state = DISO_OFFLINE;
			battery_log(BAT_LOG_CRTI, " cur diso_state is %s!\n", DISO_state_s[6]);
		} else
			battery_log(BAT_LOG_CRTI, "[%s] wrong trigger flag!\n", __func__);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		break;
	default:
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		battery_log(BAT_LOG_CRTI, "[DISO]VUSB auxadc IRQ triggered ERROR OR TEST\n");
		return;		/* in error or unexecpt state just return */
	}
	g_diso_state = *(int *)&DISO_data.diso_state;
	battery_log(BAT_LOG_CRTI, "[DISO]g_diso_state: 0x%x\n", g_diso_state);
	DISO_data.irq_callback_func(0, NULL);
}
#else
static irqreturn_t diso_auxadc_irq_handler(int irq, void *dev_id)
{
	int trigger_channel = -1;
	int trigger_flag = -1;
	trigger_channel = mt_auxadc_getCurrentChannel();
	pr_debug("[DISO]auxadc handler triggered\n");
	switch (trigger_channel) {
	case AP_AUXADC_DISO_VDC_CHANNEL:
		trigger_flag = mt_auxadc_getCurrentTrigger();
		pr_debug("[DISO]VDC IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel, trigger_flag);
#ifdef MTK_DISCRETE_SWITCH /*for DSC DC plugin handle */
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state  = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state  = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state  = DISO_OFFLINE;
			pr_debug(" cur diso_state is %s!\n", DISO_state_s[2]);
		}
#else /*for load switch OTG leakage handle*/
		set_vdc_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag) & 0x1);
		if (trigger_flag == DISO_IRQ_RISING) {
			DISO_data.diso_state.pre_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state  = DISO_OFFLINE;
			DISO_data.diso_state.pre_otg_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state  = DISO_ONLINE;
			pr_debug(" cur diso_state is %s!\n", DISO_state_s[5]);
			} else {
			DISO_data.diso_state.pre_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_otg_state  = DISO_ONLINE;
			pr_debug(" cur diso_state is %s!\n", DISO_state_s[1]);
		}
#endif
		break;
	case AP_AUXADC_DISO_VUSB_CHANNEL:
		trigger_flag = mt_auxadc_getCurrentTrigger();
		pr_debug("[DISO]VUSB IRQ triggered, channel ==%d, flag ==%d\n", trigger_channel, trigger_flag);
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		if (trigger_flag == DISO_IRQ_FALLING) {
			DISO_data.diso_state.pre_vusb_state  = DISO_ONLINE;
			DISO_data.diso_state.pre_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state  = DISO_OFFLINE;
			pr_debug(" cur diso_state is %s!\n", DISO_state_s[4]);
		} else {
			DISO_data.diso_state.pre_vusb_state  = DISO_OFFLINE;
			DISO_data.diso_state.pre_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.pre_otg_state  = DISO_OFFLINE;
			DISO_data.diso_state.cur_vusb_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_vdc_state  = DISO_ONLINE;
			DISO_data.diso_state.cur_otg_state  = DISO_OFFLINE;
			pr_debug(" cur diso_state is %s!\n", DISO_state_s[6]);
		}
		set_vusb_auxadc_irq(DISO_IRQ_ENABLE, (~trigger_flag)&0x1);
		break;
	default:
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		pr_debug("[DISO]VUSB auxadc IRQ triggered ERROR OR TEST\n");
		return IRQ_HANDLED; /* in error or unexecpt state just return */
	}
	g_diso_state = *(int *)&DISO_data.diso_state;
	return IRQ_WAKE_THREAD;
}
#endif
static unsigned int diso_get_current_voltage(int Channel)
{
	int ret = 0, data[4], i, ret_value = 0, ret_temp = 0, times = 5;
	if (IMM_IsAdcInitReady() == 0) {
		battery_log(BAT_LOG_CRTI, "[DISO] AUXADC is not ready");
		return 0;
	}
	i = times;
	while (i--) {
		ret_value = IMM_GetOneChannelValue(Channel, data, &ret_temp);
		if (ret_value == 0) {
			ret += ret_temp;
		} else {
			times = times > 1 ? times - 1 : 1;
			battery_log(BAT_LOG_CRTI, "[diso_get_current_voltage] ret_value=%d, times=%d\n",
				  ret_value, times);
		}
	}
	ret = ret * 1500 / 4096;
	ret = ret / times;
	return ret;
}
static void _get_diso_interrupt_state(void)
{
	int vol = 0;
	int diso_state = 0;
	mdelay(AUXADC_CHANNEL_DELAY_PERIOD);
	vol = diso_get_current_voltage(AP_AUXADC_DISO_VDC_CHANNEL);
	vol = (R_DISO_DC_PULL_UP + R_DISO_DC_PULL_DOWN) * 100 * vol / (R_DISO_DC_PULL_DOWN) / 100;
	battery_log(BAT_LOG_CRTI, "[DISO]  Current DC voltage mV = %d\n", vol);
	mdelay(SWITCH_RISING_TIMING + LOAD_SWITCH_TIMING_MARGIN);
	if (vol > VDC_MIN_VOLTAGE / 1000 && vol < VDC_MAX_VOLTAGE / 1000)
		diso_state |= 0x4;	/* SET DC bit as 1 */
	else
		diso_state &= ~0x4;	/* SET DC bit as 0 */
	vol = PMIC_IMM_GetOneChannelValue(MT6328_AUX_TSX, 5, 1);//modify mid
	vol = (R_DISO_VBUS_PULL_UP + R_DISO_VBUS_PULL_DOWN)*100*vol/(R_DISO_VBUS_PULL_DOWN)/100;
	battery_log(BAT_LOG_CRTI, "[DISO]  Current VBUS voltage  mV = %d\n", vol);
	if (vol > VBUS_MIN_VOLTAGE / 1000 && vol < VBUS_MAX_VOLTAGE / 1000) {
		if (!mt_usb_is_device()) {
			diso_state |= 0x1;	/* SET OTG bit as 1 */
			diso_state &= ~0x2;	/* SET VBUS bit as 0 */
		} else {
			diso_state &= ~0x1;	/* SET OTG bit as 0 */
			diso_state |= 0x2;	/* SET VBUS bit as 1; */
		}
	} else {
		diso_state &= 0x4;	/* SET OTG and VBUS bit as 0 */
	}
	battery_log(BAT_LOG_CRTI, "[DISO] DISO_STATE==0x%x\n", diso_state);
	g_diso_state = diso_state;
}
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
int _get_irq_direction(int pre_vol, int cur_vol)
{
	int ret = -1;
	if ((cur_vol - pre_vol) > 1000)
		ret = DISO_IRQ_RISING;
	else if ((pre_vol - cur_vol) > 1000)
		ret = DISO_IRQ_FALLING;
	return ret;
}
static void _get_polling_state(void)
{
	int vdc_vol = 0, vusb_vol = 0;
	int vdc_vol_dir = -1;
	int vusb_vol_dir = -1;
	DISO_polling_channel *VDC_Polling = &DISO_Polling.vdc_polling_measure;
	DISO_polling_channel *VUSB_Polling = &DISO_Polling.vusb_polling_measure;
	vdc_vol = diso_get_current_voltage(AP_AUXADC_DISO_VDC_CHANNEL);
	vdc_vol =
	    (R_DISO_DC_PULL_UP + R_DISO_DC_PULL_DOWN) * 100 * vdc_vol / (R_DISO_DC_PULL_DOWN) / 100;
	printk("[_get_polling_state]	Current vdc_vol = %d\n", vdc_vol);
	vusb_vol = PMIC_IMM_GetOneChannelValue(MT6328_AUX_TSX, 5, 1); //modify mid 
	vusb_vol = (R_DISO_VBUS_PULL_UP + R_DISO_VBUS_PULL_DOWN)*100*vusb_vol/(R_DISO_VBUS_PULL_DOWN)/100;
	printk("[_get_polling_state]	Current vusb_vol = %d\n", vusb_vol);
	VDC_Polling->preVoltage = VDC_Polling->curVoltage;
	VUSB_Polling->preVoltage = VUSB_Polling->curVoltage;
	VDC_Polling->curVoltage = vdc_vol;
	VUSB_Polling->curVoltage = vusb_vol;
	if (DISO_Polling.reset_polling) {
		DISO_Polling.reset_polling = KAL_FALSE;
		VDC_Polling->preVoltage = vdc_vol;
		VUSB_Polling->preVoltage = vusb_vol;
		if (vdc_vol > 1000)
			vdc_vol_dir = DISO_IRQ_RISING;
		else
			vdc_vol_dir = DISO_IRQ_FALLING;
		if (vusb_vol > 1000)
			vusb_vol_dir = DISO_IRQ_RISING;
		else
			vusb_vol_dir = DISO_IRQ_FALLING;
	} else {
		vdc_vol_dir = _get_irq_direction(VDC_Polling->preVoltage, VDC_Polling->curVoltage);
		vusb_vol_dir =
		    _get_irq_direction(VUSB_Polling->preVoltage, VUSB_Polling->curVoltage);
	}
	if (VDC_Polling->notify_irq_en && (vdc_vol_dir == VDC_Polling->notify_irq)) {
		schedule_delayed_work(&diso_polling_work, 10 * HZ / 1000);	/* 10ms */
		battery_log(BAT_LOG_CRTI, "[%s] ready to trig VDC irq, irq: %d\n",
			  __func__, VDC_Polling->notify_irq);
	} else if (VUSB_Polling->notify_irq_en && (vusb_vol_dir == VUSB_Polling->notify_irq)) {
		schedule_delayed_work(&diso_polling_work, 10 * HZ / 1000);
		battery_log(BAT_LOG_CRTI, "[%s] ready to trig VUSB irq, irq: %d\n",
			  __func__, VUSB_Polling->notify_irq);
	} else if ((vdc_vol <= 200) && (vusb_vol <= 200)) {
		VDC_Polling->notify_irq_en = 0;
		VUSB_Polling->notify_irq_en = 0;
	}
}
enum hrtimer_restart diso_kthread_hrtimer_func(struct hrtimer *timer)
{
	diso_thread_timeout = KAL_TRUE;
	wake_up(&diso_polling_thread_wq);
	return HRTIMER_NORESTART;
}
int diso_thread_kthread(void *x)
{
	while (1) {
		wait_event(diso_polling_thread_wq, (diso_thread_timeout == KAL_TRUE));
		diso_thread_timeout = KAL_FALSE;
		mutex_lock(&diso_polling_mutex);
		_get_polling_state();
		if (DISO_Polling.vdc_polling_measure.notify_irq_en ||
		    DISO_Polling.vusb_polling_measure.notify_irq_en)
			hrtimer_start(&diso_kthread_timer,
				      ktime_set(0, MSEC_TO_NSEC(SW_POLLING_PERIOD)),
				      HRTIMER_MODE_REL);
		else
			hrtimer_cancel(&diso_kthread_timer);
		mutex_unlock(&diso_polling_mutex);
	}
	return 0;
}
#endif
#endif
static unsigned int charging_diso_init(void *data)
{
#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
	DISO_ChargerStruct *pDISO_data = (DISO_ChargerStruct *) data;
	pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
	pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
	pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;
	pDISO_data->diso_state.pre_otg_state = DISO_OFFLINE;
	pDISO_data->diso_state.pre_vusb_state = DISO_OFFLINE;
	pDISO_data->diso_state.pre_vdc_state = DISO_OFFLINE;
	pDISO_data->chr_get_diso_state = KAL_FALSE;
	pDISO_data->hv_voltage = VBUS_MAX_VOLTAGE;
#if !defined(MTK_AUXADC_IRQ_SUPPORT)
	hrtimer_init(&diso_kthread_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	diso_kthread_timer.function = diso_kthread_hrtimer_func;
	INIT_DELAYED_WORK(&diso_polling_work, diso_polling_handler);
	kthread_run(diso_thread_kthread, NULL, "diso_thread_kthread");
	battery_log(BAT_LOG_CRTI, "[%s] done\n", __func__);
#else
	struct device_node *node;
	int ret;
	DISO_IRQ.vdc_measure_channel.number   = AP_AUXADC_DISO_VDC_CHANNEL;
	DISO_IRQ.vusb_measure_channel.number  = AP_AUXADC_DISO_VUSB_CHANNEL;
	DISO_IRQ.vdc_measure_channel.period   = AUXADC_CHANNEL_DELAY_PERIOD;
	DISO_IRQ.vusb_measure_channel.period  = AUXADC_CHANNEL_DELAY_PERIOD;
	DISO_IRQ.vdc_measure_channel.debounce	  = AUXADC_CHANNEL_DEBOUNCE;
	DISO_IRQ.vusb_measure_channel.debounce  = AUXADC_CHANNEL_DEBOUNCE;
	DISO_IRQ.vusb_measure_channel.falling_threshold = VBUS_MIN_VOLTAGE/1000;
	DISO_IRQ.vdc_measure_channel.falling_threshold = VDC_MIN_VOLTAGE/1000;
	DISO_IRQ.vusb_measure_channel.rising_threshold = VBUS_MIN_VOLTAGE/1000;
	DISO_IRQ.vdc_measure_channel.rising_threshold = VDC_MIN_VOLTAGE/1000;
	node = of_find_compatible_node(NULL, NULL, "mediatek,AUXADC");
	if (!node) {
		pr_debug("[diso_adc]: of_find_compatible_node failed!!\n");
	} else {
		pDISO_data->irq_line_number = irq_of_parse_and_map(node, 0);
		pr_debug("[diso_adc]: IRQ Number: 0x%x\n", pDISO_data->irq_line_number);
	}
	mt_irq_set_sens(pDISO_data->irq_line_number, MT_EDGE_SENSITIVE);
	mt_irq_set_polarity(pDISO_data->irq_line_number, MT_POLARITY_LOW);
	ret = request_threaded_irq(pDISO_data->irq_line_number, diso_auxadc_irq_handler,
	pDISO_data->irq_callback_func, IRQF_ONESHOT, "DISO_ADC_IRQ", NULL);
	if (ret) {
		pr_debug("[diso_adc]: request_irq failed.\n");
	} else {
		set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
		set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
		pr_debug("[diso_adc]: diso_init success.\n");
	}
#endif
#endif
	return STATUS_OK;
}

static unsigned int charging_get_diso_state(void *data)
{
#if defined(CONFIG_MTK_DUAL_INPUT_CHARGER_SUPPORT)
	int diso_state = 0x0;
	DISO_ChargerStruct *pDISO_data = (DISO_ChargerStruct *) data;
	_get_diso_interrupt_state();
	diso_state = g_diso_state;
	battery_log(BAT_LOG_CRTI, "[do_chrdet_int_task] current diso state is %s!\n", DISO_state_s[diso_state]);
	if (((diso_state >> 1) & 0x3) != 0x0) {
		switch (diso_state) {
		case USB_ONLY:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
#ifdef MTK_DISCRETE_SWITCH
			set_vdc_auxadc_irq(DISO_IRQ_ENABLE, 1);
#endif
			pDISO_data->diso_state.cur_vusb_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_ONLY:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_RISING);
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_WITH_USB:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_ENABLE, DISO_IRQ_FALLING);
			pDISO_data->diso_state.cur_vusb_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_OFFLINE;
			break;
		case DC_WITH_OTG:
			set_vdc_auxadc_irq(DISO_IRQ_DISABLE, 0);
			set_vusb_auxadc_irq(DISO_IRQ_DISABLE, 0);
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_ONLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_ONLINE;
			break;
		default:	/* OTG only also can trigger vcdt IRQ */
			pDISO_data->diso_state.cur_vusb_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_vdc_state = DISO_OFFLINE;
			pDISO_data->diso_state.cur_otg_state = DISO_ONLINE;
			battery_log(BAT_LOG_CRTI, " switch load vcdt irq triggerd by OTG Boost!\n");
			break;	/* OTG plugin no need battery sync action */
		}
	}
	if (DISO_ONLINE == pDISO_data->diso_state.cur_vdc_state)
		pDISO_data->hv_voltage = VDC_MAX_VOLTAGE;
	else
		pDISO_data->hv_voltage = VBUS_MAX_VOLTAGE;
#endif
	return STATUS_OK;
}

static unsigned int charging_set_vbus_ovp_en(void *data)
{
	return STATUS_OK;
}

static unsigned int charging_set_vindpm(void *data)
{
	return STATUS_OK;
}

static unsigned int charging_set_pwrstat_led_en(void *data)
{
	return STATUS_OK;
}

static unsigned int charging_run_aicl(void *data)
{
	return STATUS_OK;
}

 static u32 (* charging_func[CHARGING_CMD_NUMBER])(void *data)=
 {
 	//  charging_hw_init
	//,charging_dump_register  	
	//,charging_enable
	//,charging_set_cv_voltage
	//,charging_get_current
	//,charging_set_current
	//,charging_set_input_current
	//,charging_get_charging_status
	//,charging_reset_watch_dog_timer
	//,charging_set_hv_threshold
	//,charging_get_hv_status
	//,charging_get_battery_status
	//,charging_get_charger_det_status
	//,charging_get_charger_type
	//,charging_get_is_pcm_timer_trigger	
	//,charging_set_platform_reset
	//,charging_get_platform_boot_mode
	//,charging_set_power_off
	//,charging_get_power_source
	//,charging_get_csdac_full_flag
	//,charging_set_ta_current_pattern
	//,charging_set_error_state
 };

 
 /*
 * FUNCTION
 *		Internal_chr_control_handler
 *
 * DESCRIPTION															 
 *		 This function is called to set the charger hw
 *
 * CALLS  
 *
 * PARAMETERS
 *		None
 *	 
 * RETURNS
 *		
 *
 * GLOBALS AFFECTED
 *	   None
 */
 s32 chr_control_interface(CHARGING_CTRL_CMD cmd, void *data)
 {
	 s32 status;
	 	static signed int init = -1;

	if (init == -1) {
		init = 0;
		charging_func[CHARGING_CMD_INIT] = charging_hw_init;
		charging_func[CHARGING_CMD_DUMP_REGISTER] = charging_dump_register;
		charging_func[CHARGING_CMD_ENABLE] = charging_enable;
		charging_func[CHARGING_CMD_SET_CV_VOLTAGE] = charging_set_cv_voltage;
		charging_func[CHARGING_CMD_GET_CURRENT] = charging_get_current;
		charging_func[CHARGING_CMD_SET_CURRENT] = charging_set_current;
		charging_func[CHARGING_CMD_SET_INPUT_CURRENT] = charging_set_input_current;
		charging_func[CHARGING_CMD_GET_CHARGING_STATUS] = charging_get_charging_status;
		charging_func[CHARGING_CMD_RESET_WATCH_DOG_TIMER] = charging_reset_watch_dog_timer;
		charging_func[CHARGING_CMD_SET_HV_THRESHOLD] = charging_set_hv_threshold;
		charging_func[CHARGING_CMD_GET_HV_STATUS] = charging_get_hv_status;
		charging_func[CHARGING_CMD_GET_BATTERY_STATUS] = charging_get_battery_status;
		charging_func[CHARGING_CMD_GET_CHARGER_DET_STATUS] = charging_get_charger_det_status;
		charging_func[CHARGING_CMD_GET_CHARGER_TYPE] = charging_get_charger_type;
		charging_func[CHARGING_CMD_GET_IS_PCM_TIMER_TRIGGER] = charging_get_is_pcm_timer_trigger;
		charging_func[CHARGING_CMD_SET_PLATFORM_RESET] = charging_set_platform_reset;
		charging_func[CHARGING_CMD_GET_PLATFORM_BOOT_MODE] = charging_get_platform_boot_mode;
		charging_func[CHARGING_CMD_SET_POWER_OFF] = charging_set_power_off;
		charging_func[CHARGING_CMD_GET_POWER_SOURCE] = charging_get_power_source;
		charging_func[CHARGING_CMD_GET_CSDAC_FALL_FLAG] = charging_get_csdac_full_flag;
		charging_func[CHARGING_CMD_SET_TA_CURRENT_PATTERN] = charging_set_ta_current_pattern;
		charging_func[CHARGING_CMD_SET_ERROR_STATE] = charging_set_error_state;
		charging_func[CHARGING_CMD_DISO_INIT] = charging_diso_init;
		charging_func[CHARGING_CMD_GET_DISO_STATE] = charging_get_diso_state;
		charging_func[CHARGING_CMD_SET_VBUS_OVP_EN] = charging_set_vbus_ovp_en;
		charging_func[CHARGING_CMD_SET_VINDPM] = charging_set_vindpm;
		charging_func[CHARGING_CMD_SET_PWRSTAT_LED_EN] = charging_set_pwrstat_led_en;
		charging_func[CHARGING_CMD_RUN_AICL] = charging_run_aicl;
	}
	 if(cmd < CHARGING_CMD_NUMBER)
		 status = charging_func[cmd](data);
	 else
		 return STATUS_UNSUPPORTED;
 
	 return status;
 }


