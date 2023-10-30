/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_typedef.h"
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/i2c.h>
#include <linux/leds.h>


#include <linux/string.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/vmalloc.h>
#include <linux/gpio.h>


#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>


/******************************************************************************
 * Debug configuration
******************************************************************************/
/* availible parameter */
/* ANDROID_LOG_ASSERT */
/* ANDROID_LOG_ERROR */
/* ANDROID_LOG_WARNING */
/* ANDROID_LOG_INFO */
/* ANDROID_LOG_DEBUG */
/* ANDROID_LOG_VERBOSE */

#define TAG_NAME "[leds_strobe.c]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    pr_debug(TAG_NAME "%s: " fmt, __func__ , ##arg)

/*#define DEBUG_LEDS_STROBE*/
#ifdef DEBUG_LEDS_STROBE
#define PK_DBG PK_DBG_FUNC
#else
#define PK_DBG(a, ...)
#endif

/******************************************************************************
 * local variables
******************************************************************************/

static DEFINE_SPINLOCK(g_strobeSMPLock);	/* cotta-- SMP proection */


static u32 strobe_Res;
static u32 strobe_Timeus;
static BOOL g_strobe_On;

//static int g_duty = -1;
static int g_timeOutTimeMs;

static DEFINE_MUTEX(g_strobeSem);


#define STROBE_DEVICE_ID 0xC6


static struct work_struct workTimeOut;


///===================================================


static struct pinctrl *flashctrl = NULL;
static struct pinctrl_state *flash_en_high = NULL;
static struct pinctrl_state *flash_en_low = NULL;
static struct pinctrl_state *flash_mode_high = NULL;
static struct pinctrl_state *flash_mode_low = NULL;

int flashlight_gpio_init(struct platform_device *pdev)
{
	int ret = 0;
	flashctrl = devm_pinctrl_get(&pdev->dev);
	if (IS_ERR(flashctrl)) {
		dev_err(&pdev->dev, "Cannot find camera pinctrl!");
		ret = PTR_ERR(flashctrl);
	}

	flash_en_high = pinctrl_lookup_state(flashctrl, "enable_high");
	if (IS_ERR(flash_en_high)) {
		ret = PTR_ERR(flash_en_high);
		printk("%s : pinctrl err, enable_high\n", __func__);
	}

	flash_en_low = pinctrl_lookup_state(flashctrl, "enable_low");
	if (IS_ERR(flash_en_low)) {
		ret = PTR_ERR(flash_en_low);
		printk("%s : pinctrl err, enable_low\n", __func__);
	}
	
	flash_mode_high = pinctrl_lookup_state(flashctrl, "mode_high");
	if (IS_ERR(flash_mode_high)) {
		ret = PTR_ERR(flash_mode_high);
		printk("%s : pinctrl err, mode_high\n", __func__);
	}
	
	flash_mode_low = pinctrl_lookup_state(flashctrl, "mode_low");
	if (IS_ERR(flash_mode_low)) {
		ret = PTR_ERR(flash_mode_low);
		printk("%s : pinctrl err, mode_low\n", __func__);
	}
	return ret;
}



int flashlight_gpio_enable_high(void)
{
	pinctrl_select_state(flashctrl, flash_en_high);
	return 0;
}

int flashlight_gpio_enable_low(void)
{
	pinctrl_select_state(flashctrl, flash_en_low);
	return 0;
}

int flashlight_gpio_mode_high(void)
{
	PK_DBG("%s\n", __func__);
	pinctrl_select_state(flashctrl, flash_mode_high);
	return 0;
}

int flashlight_gpio_mode_low(void)
{
	PK_DBG("%s\n", __func__);	
	pinctrl_select_state(flashctrl, flash_mode_low);
	return 0;
}



static int flashlight_probe(struct platform_device *pdev)
{
	printk("%s\n", __func__);
	flashlight_gpio_init(pdev);
	return 0;
}

static const struct of_device_id flashlight_of_ids[] = {
	{.compatible = "mediatek,flashlight",},
	{}
};

static struct platform_driver flashlight_driver = {
	.probe = flashlight_probe,
	.driver = {
		   .name = "flashlight",
#ifdef CONFIG_OF
		   .of_match_table = flashlight_of_ids,
#endif
		   },
};

static int __init flashlight_init(void)
{
	printk("Flashlight: \n");
	if (platform_driver_register(&flashlight_driver)) {
		pr_err("Flashlight: failed to register this driver!\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit flashlight_exit(void)
{
	platform_driver_unregister(&flashlight_driver);
	pr_notice("Flashlight: Unregister this driver done\n");
}


late_initcall(flashlight_init);
module_exit(flashlight_exit);
MODULE_AUTHOR("tongjun@szmid.com");
MODULE_DESCRIPTION("Flashlight subsystem driver");
MODULE_LICENSE("GPL");

//======================================================

void FL_Flash_Mode(void)
{    
    PK_DBG("FL_Flash_Mode\n");

    flashlight_gpio_mode_high();
}

void FL_Torch_Mode(void)
{    
    PK_DBG("FL_Torch_Mode\n");
    flashlight_gpio_mode_low();
}



/*****************************************************************************
Functions
*****************************************************************************/
static void work_timeOutFunc(struct work_struct *data);


int FL_Enable(void)
{
	flashlight_gpio_enable_high();
	return 0;
}



int FL_Disable(void)
{
	flashlight_gpio_enable_low();
	return 0;
}

int FL_dim_duty(kal_uint32 duty)
{
	PK_DBG(" FL_dim_duty line=%d\n", __LINE__);
//	if(duty <= 0)
		FL_Torch_Mode();
//	else
//		FL_Flash_Mode();
//	g_duty = duty;
//----------------------------------------------
	return 0;
}




int FL_Init(void)
{
	flashlight_gpio_enable_low();
	flashlight_gpio_mode_low();

    INIT_WORK(&workTimeOut, work_timeOutFunc);

	return 0;
}


int FL_Uninit(void)
{
	FL_Disable();
	return 0;
}

/*****************************************************************************
User interface
*****************************************************************************/

static void work_timeOutFunc(struct work_struct *data)
{
	FL_Disable();
	PK_DBG("ledTimeOut_callback\n");
}



enum hrtimer_restart ledTimeOutCallback(struct hrtimer *timer)
{
	schedule_work(&workTimeOut);
	return HRTIMER_NORESTART;
}

static struct hrtimer g_timeOutTimer;
void timerInit(void)
{
	static int init_flag;

	if (init_flag == 0) {
		init_flag = 1;
		INIT_WORK(&workTimeOut, work_timeOutFunc);
		g_timeOutTimeMs = 1000;
		hrtimer_init(&g_timeOutTimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
		g_timeOutTimer.function = ledTimeOutCallback;
	}
}


static int constant_flashlight_ioctl(unsigned int cmd, unsigned long arg)
{
	int i4RetValue = 0;
	int ior_shift;
	int iow_shift;
	int iowr_shift;

	ior_shift = cmd - (_IOR(FLASHLIGHT_MAGIC, 0, int));
	iow_shift = cmd - (_IOW(FLASHLIGHT_MAGIC, 0, int));
	iowr_shift = cmd - (_IOWR(FLASHLIGHT_MAGIC, 0, int));
/*	PK_DBG
	    ("LM3642 constant_flashlight_ioctl() line=%d ior_shift=%d, iow_shift=%d iowr_shift=%d arg=%d\n",
	     __LINE__, ior_shift, iow_shift, iowr_shift, (int)arg);
*/
	switch (cmd) {

	case FLASH_IOC_SET_TIME_OUT_TIME_MS:
		PK_DBG("FLASH_IOC_SET_TIME_OUT_TIME_MS: %d\n", (int)arg);
		g_timeOutTimeMs = arg;
		break;


	case FLASH_IOC_SET_DUTY:
		PK_DBG("FLASHLIGHT_DUTY: %d\n", (int)arg);
		FL_dim_duty(arg);
		break;


	case FLASH_IOC_SET_STEP:
		PK_DBG("FLASH_IOC_SET_STEP: %d\n", (int)arg);

		break;

	case FLASH_IOC_SET_ONOFF:
		PK_DBG("FLASHLIGHT_ONOFF: %d\n", (int)arg);
		if (arg == 1) {

			int s;
			int ms;

			if (g_timeOutTimeMs > 1000) {
				s = g_timeOutTimeMs / 1000;
				ms = g_timeOutTimeMs - s * 1000;
			} else {
				s = 0;
				ms = g_timeOutTimeMs;
			}

			if (g_timeOutTimeMs != 0) {
				ktime_t ktime;

				ktime = ktime_set(s, ms * 1000000);
				hrtimer_start(&g_timeOutTimer, ktime, HRTIMER_MODE_REL);
			}
			FL_Enable();
		} else {
			FL_Disable();
			hrtimer_cancel(&g_timeOutTimer);
		}
		break;
	default:
		PK_DBG(" No such command\n");
		i4RetValue = -EPERM;
		break;
	}
	return i4RetValue;
}




static int constant_flashlight_open(void *pArg)
{
	int i4RetValue = 0;

	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	if (0 == strobe_Res) {
		FL_Init();
		timerInit();
	}
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);
	spin_lock_irq(&g_strobeSMPLock);


	if (strobe_Res) {
		PK_DBG(" busy!\n");
		i4RetValue = -EBUSY;
	} else {
		strobe_Res += 1;
	}


	spin_unlock_irq(&g_strobeSMPLock);
	PK_DBG("constant_flashlight_open line=%d\n", __LINE__);

	return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{
	PK_DBG(" constant_flashlight_release\n");

	if (strobe_Res) {
		spin_lock_irq(&g_strobeSMPLock);

		strobe_Res = 0;
		strobe_Timeus = 0;

		/* LED On Status */
		g_strobe_On = FALSE;

		spin_unlock_irq(&g_strobeSMPLock);

		FL_Uninit();
	}

	PK_DBG(" Done\n");

	return 0;

}


FLASHLIGHT_FUNCTION_STRUCT constantFlashlightFunc = {
	constant_flashlight_open,
	constant_flashlight_release,
	constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{
	if (pfFunc != NULL)
		*pfFunc = &constantFlashlightFunc;
	return 0;
}
EXPORT_SYMBOL(constantFlashlightInit);


/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{

	return 0;
}
EXPORT_SYMBOL(strobe_VDIrq);
