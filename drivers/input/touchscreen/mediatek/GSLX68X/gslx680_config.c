#include "tpd.h"
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/fb.h>

#ifdef CONFIG_COMPAT
#include <linux/compat.h>
#endif

#include "gslx680_config.h"
#include "tp_list.h"

#include <linux/time.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#include <mt-plat/items.h>

static unsigned int GSL_TP_ID_USED = 0;
static int touchpanel_gsl_modle = 0; // we can delete it
static u8 light_sleep_flag = 0;
struct timer_list  led_timer; 

int sf_hwrotation = 0;
int xy_swap = 0;
int x_reverse= 0;
int y_reverse = 0;
int xy_deal = 0;
int xy_deal_minor = 0;

//#define GSL_DEBUG

#ifdef GSL_DEBUG 
#define print_info(fmt, args...)   \
        do{                              \
                printk(fmt, ##args);     \
        }while(0)
#else
#define print_info(fmt, args...)
#endif


#define STRSPLICE(dest, a, b, c) \
	do{ \
		strcpy(dest, a); \
		strcat(dest, b); \
		strcat(dest, c); \
	} while(0)

static void led_set_gpio_output(unsigned int GPIO, unsigned int output)
{
}

static void timer_func(unsigned long data)
{
	if(light_sleep_flag==1)
	{
		light_sleep_flag=0;

		if(LED_PIN) 
			led_set_gpio_output((LED_PIN | 0x80000000), 0);

	}	  
}

int tpd_hwPowerControl(unsigned int powerVolt, char *name, bool on)
{
	static int reg_ok = 0;
	int ret = 0;

	print_info("[%s] [%s] %s power.\n", __func__, name, on?"open":"off");
	if(!reg_ok){
		tpd->reg = regulator_get(tpd->tpd_dev, name);
		reg_ok = 1;		
	}

	if(on) {
		ret=regulator_set_voltage(tpd->reg, powerVolt, powerVolt);
		if (ret)
			print_info("[%s] [%s] regulator_set_voltage() failed!\n", __func__, name);

		ret=regulator_enable(tpd->reg);  //enable regulator
		if (ret)
			print_info("[%s] [%s] regulator_enable() failed!\n", __func__, name);
	} else {
		if (regulator_is_enabled(tpd->reg) != 0) {
			print_info("[%s] [%s] is enabled, begin to close.\n",__func__ , name);
		}
		if ((ret = regulator_disable(tpd->reg)) != 0) {
			print_info("[%s]fail to regulator_disable, powertype: %s \n\n",__func__, name);
			return ret;
		}
	}

	return ret;
}

int control_input_orientation(int *x, int *y, int screenwidth, int screenheight)
{
	int tpd_x_res=0, tpd_y_res=0;
	int z;

	if(90 == sf_hwrotation || 270 == sf_hwrotation)
	{	
		tpd_y_res = screenwidth;
		tpd_x_res = screenheight;
	} else {
		tpd_x_res = screenwidth;
		tpd_y_res = screenheight;
	}

	if(1 == xy_swap)
	{	
		z=*y; *y=*x; *x=z;
	}

	if(1 == x_reverse)
		*x=tpd_x_res-*x;

	if(1 == y_reverse)	
		*y=tpd_y_res-*y;

	if(1 == xy_deal)
	{
		*x=(*x)*tpd_y_res/tpd_x_res;
		*y=(*y)*tpd_x_res/tpd_y_res; 
	}

	if(1 == xy_deal_minor)
	{
		*x=(*x)*tpd_x_res/tpd_y_res;
		*y=(*y)*tpd_y_res/tpd_x_res; 
	}

	return 0;
}

int key_light_mod_timer(void)
{
	if(LED_PIN)
		mod_timer(&led_timer, jiffies + 300/(1000/HZ));

	return 0;
}

int key_light_init_timer(void)
{
	init_timer(&led_timer);
	led_timer.expires	= jiffies + 1/(1000/HZ);
	led_timer.function	= timer_func;

	return 0;
}

int control_key_light(int x, int y)
{
	int kx[4], ky;
   	int i;

   	print_info("[%s] in.\n", __func__);

	if(KEY_NUM && LED_PIN) {
   		for(i = 0; i < KEY_NUM; i++)
   		{
   			kx[i] = PTR_TPD_KEYS_DIM_LOCAL[i][0];   		
   		}	
		
		ky = PTR_TPD_KEYS_DIM_LOCAL[0][1];
	}

	if(LED_PIN) {
		switch(KEY_NUM)
		{
			case 0:
				break;
			case 1:
				if( x == kx[0] && y == ky ) {
					led_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			case 2:
				if( (x == kx[0] || x == kx[1]) && y == ky ) {
					led_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			case 3:
				if( (x == kx[0] || x == kx[1] || x == kx[2]) && y == ky ) {
					led_set_gpio_output( (LED_PIN | 0x80000000), 1 );
					light_sleep_flag=1;
				}
				break;
			default:
				break;
		}
	}

	return 0;
}

int match_gsl_tp_button(void)
{

	switch(KEY_NUM)
	{
		case 0:
			break;
		case 1:
			tpd_button_setting(KEY_NUM, tpd_keys_local_home, PTR_TPD_KEYS_DIM_LOCAL);
			break;
		case 3:
			tpd_button_setting(KEY_NUM, tpd_keys_local, PTR_TPD_KEYS_DIM_LOCAL);
			break;
		default:
			break;
	}

	return 0;

}

int match_gsl_tp_id(char *read_buf)
{
	int i,j;
	u8 temp[4]={0};

	print_info("[%s] GSL_TP_ID_USED = %d, name_tp_mode = %s.\n", __func__, GSL_TP_ID_USED, name_tp_mode);

	if(GSL_TP_ID_USED) {
		for(i=0;i<TP_NUM;i++) { 
			
			print_info("\n[%s] ID  %x %x %x %x \n", __func__, \
				PTR_TP_PROJECT_DATA[i].tp_id[3], \
				PTR_TP_PROJECT_DATA[i].tp_id[2], \
				PTR_TP_PROJECT_DATA[i].tp_id[1], \
				PTR_TP_PROJECT_DATA[i].tp_id[0]);

			for(j=0;j<4;j++) {
				temp[j]=PTR_TP_PROJECT_DATA[i].tp_id[j];
			}
			// 同时进行id匹配和全名匹配，兼容之前无ID的TP
			if((( read_buf[3] == temp[0] ) && \
				( read_buf[2] == temp[1] ) && \
				( read_buf[1] == temp[2] ) && \
				( read_buf[0] == temp[3] ))
				|| (0 == strcmp(name_tp_mode, PTR_TP_PROJECT_DATA[i].name))) { 
				PTR_FW = PTR_TP_PROJECT_DATA[i].p_fw;
				PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[i].p_gsl_config_data_id;
				PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[i].tpd_keys;
				TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[i].tp_source_len;
				NAME_TP = PTR_TP_PROJECT_DATA[i].name;					
				GSL_TP_ID_USED=i; 
				break; 

			}
		}

		if( i==TP_NUM ) { // 
			PTR_FW = PTR_TP_PROJECT_DATA[i-1].p_fw;
			PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[i-1].p_gsl_config_data_id;
			PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[i-1].tpd_keys;
			TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[i-1].tp_source_len;
			NAME_TP = PTR_TP_PROJECT_DATA[i-1].name;				
			GSL_TP_ID_USED=i-1;
		}	
		printk("%s : name_tp_mode=%s, real top_name=%s\n", __func__, name_tp_mode, NAME_TP);
	}
	return 0;
}

int tp_list_config(void)
{
    int i;
    int sizeoflist;

	sizeoflist = sizeof(gsl_tp_info) / sizeof(ALL_TP_INFO);
	for (i = 0; i < sizeoflist; i++){
		//if(strcmp(name_tp_mode, gsl_tp_info[i].name) == 0)
		// 当给出完成tp名时，也能匹配到项目TP
		if(0 == strncmp(name_tp_mode, gsl_tp_info[i].name, strlen(gsl_tp_info[i].name)))
	    	break;
	}

	printk("\n[%s] name_tp_mode =%s\n",__func__, name_tp_mode);
	if (i == sizeoflist)
	{
		print_info("\n[%s] list of designated tp isn't exist name_tp_mode =%s\n",__func__, name_tp_mode);
		return -1;
	}

	touchpanel_gsl_modle = gsl_tp_info[i].tp_gsl_modle;
	PTR_TP_PROJECT_DATA = gsl_tp_info[i].pro_tp_data;
	KEY_NUM = gsl_tp_info[i].HAVE_KEY;
	TP_NUM = gsl_tp_info[i].tp_num;
	LED_PIN = gsl_tp_info[i].gpio_led_pin;

	if( 1 != TP_NUM && 0 != TP_NUM) {  //项目配置有多个TP
		if( KEY_NUM ) { 
			PTR_FW = GSL_TEST_HAVE_KEY_FW; 
			TP_SOURCE_LEN = ARRAY_SIZE(GSL_TEST_HAVE_KEY_FW);
		} else { 
			PTR_FW = GSL_TEST_FW; 
			TP_SOURCE_LEN = ARRAY_SIZE(GSL_TEST_FW);
		}
		GSL_TP_ID_USED = 0xff;    
	} else if( 1 == TP_NUM ) { // 项目只配一个TP
		if( KEY_NUM )   { PTR_TPD_KEYS_DIM_LOCAL = PTR_TP_PROJECT_DATA[0].tpd_keys; } 
		PTR_GSL_CONFIG_DATA_ID = PTR_TP_PROJECT_DATA[0].p_gsl_config_data_id;
		PTR_FW = PTR_TP_PROJECT_DATA[0].p_fw;
		TP_SOURCE_LEN = PTR_TP_PROJECT_DATA[0].tp_source_len;
		GSL_TP_ID_USED = 0;

	} else {
      print_info("\n[%s] TP_NUM = %d\n",__func__, TP_NUM);
      return 0;
   }

   return 0;
}

int _tp_gpio_config(char *items_key)
{
	char STR_GPIO[64];
	char DTS_GPIO_OUT0[64];
	char DTS_GPIO_OUT1[64];
	char DTS_GPIO_EINT[64];
	int ret = 0;

	if(item_exist(items_key)) {
		item_string(STR_GPIO, items_key, 0);
		STRSPLICE(DTS_GPIO_OUT0, "gpio", STR_GPIO, "_out0");
		STRSPLICE(DTS_GPIO_OUT1, "gpio", STR_GPIO, "_out1");
		print_info("[%s] [GPIO%s] DTS_GPIO_OUT0 = %s, DTS_GPIO_OUT1 = %s\n", __func__, STR_GPIO, DTS_GPIO_OUT0, DTS_GPIO_OUT1);
		print_info("[%s] items_key = %s\n", __func__, items_key);

		if(!strcmp(TPD_RST, items_key)) {
			rst_output1 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT1);
			if (IS_ERR(rst_output1)) {
				ret = PTR_ERR(rst_output1);
				print_info("%s : pinctrl err, rst_output1\n", __func__);
			}
			rst_output0 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT0);
			if (IS_ERR(rst_output0)) {
				ret = PTR_ERR(rst_output0);
				print_info("%s : pinctrl err, rst_output0\n", __func__);
			}
		} else if(!strcmp(TPD_EINT, items_key)) {
			STRSPLICE(DTS_GPIO_EINT, "gpio", STR_GPIO, "_eint");
			eint_as_int = pinctrl_lookup_state(pinctrl1, DTS_GPIO_EINT);
			if (IS_ERR(eint_as_int)) {
				ret = PTR_ERR(eint_as_int);
				print_info("%s : pinctrl err, eint_as_int\n", __func__);
				return ret;
			}
			eint_output1 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT1);
			if (IS_ERR(eint_output1)) {
				ret = PTR_ERR(eint_output1);
				print_info("%s : pinctrl err, eint_output1\n", __func__);
			}
			eint_output0 = pinctrl_lookup_state(pinctrl1, DTS_GPIO_OUT0);
			if (IS_ERR(eint_output0)) {
				ret = PTR_ERR(eint_output0);
				print_info("%s : pinctrl err, eint_output0\n", __func__);
			}
		} 
	} else {
		print_info("[%s] \"%s\" not exist in items.ini !!! \n", __func__, items_key);
	} 

	return ret;
}


/*
 * items.ini file
 * for example:
 * touchpanel.power.num 	vgp1
 */
int _tp_pmic_config(char *items_key)
{
	char STR_PMIC[64];

	print_info("[%s]\n", __func__);
	item_string(STR_PMIC, items_key, 0);
	print_info("[%s] items_key = %s, STR_PMIC = %s .\n", __func__, items_key, STR_PMIC);

	if(!strcmp(TPD_PWR, items_key)) {
		tpd->reg = regulator_get(tpd->tpd_dev, STR_PMIC);
		if (IS_ERR(tpd->reg)) {
			print_info("[%s] regulator_get() failed!\n", __func__);
		}
	} else {
		print_info("[%s] regulator get cust tp node failed!\n", __func__);
		return -1;
	}

	return 0;
}

int tp_gsl_model_get(void)
{
	if(item_exist("touchpanel.gsl.modle"))
		item_string(name_tp_mode,"touchpanel.gsl.modle",0);
	else {
		print_info("[%s] <touchpanel.gsl.modle> not exist in items.\n", __func__);
		return -1;
	}

	return 0;
}

int tp_gpio_config(void)
{
	_tp_gpio_config("touchpanel.reset.pin");
	_tp_gpio_config("touchpanel.eint.pin");
	return 0;
}

int tp_i2c_config(void)
{
/*
	if(item_exist("touchpanel.i2c.num"))
		i2c_num = item_integer("touchpanel.i2c.num",0);
	else
		i2c_num = 0;
*/
	return 0;
}

int tp_pmic_config(void)
{
	if(item_exist("touchpanel.power.num")) {
		_tp_pmic_config("touchpanel.power.num");
	}
	return 0;
}

int tp_gsl_ic_config(void)
{

/*
    if(item_equal("touchpanel.gsl.ic", "GSL1686", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL1686F", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3675", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3676", 0))
		touchpanel_gsl_ic = 0;	
    else if(item_equal("touchpanel.gsl.ic", "GSL915", 0) || \
    	item_equal("touchpanel.gsl.ic", "GSL3670", 0))
		touchpanel_gsl_ic = 1;
*/

	return 0;
}

int tp_input_orientation_config(void)
{

	if(item_exist("ro.sf.hwrotation"))
		sf_hwrotation = item_integer("ro.sf.hwrotation",0);
	else
		sf_hwrotation = -1;

	if(item_exist("touchpanel.xy.swap"))
		xy_swap = item_integer("touchpanel.xy.swap",0);
	else
		xy_swap = -1;

	if(item_exist("touchpanel.x.reverse"))
		x_reverse = item_integer("touchpanel.x.reverse",0);
	else
		x_reverse = -1;

	if(item_exist("touchpanel.y.reverse"))
		y_reverse = item_integer("touchpanel.y.reverse",0);
	else
		y_reverse = -1;

	if(item_exist("touchpanel.xy.deal"))
		xy_deal = item_integer("touchpanel.xy.deal",0);
	else
		xy_deal = -1;

	if(item_exist("touchpanel.xy.deal.minor"))
		xy_deal_minor = item_integer("touchpanel.xy.deal.minor",0);
	else
		xy_deal_minor = -1;


	return 0;
}


int items_config_tp(void)
{

	print_info("[%s] in.\n", __func__);

	tp_gsl_model_get();

//	tp_gpio_config();

	tp_i2c_config();

	tp_pmic_config();

	tp_gsl_ic_config();

	tp_input_orientation_config();

	return 0;
}

