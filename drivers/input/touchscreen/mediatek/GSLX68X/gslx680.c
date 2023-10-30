#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/bitops.h>
#include <linux/kernel.h>
#include <linux/byteorder/generic.h>

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif


#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/timer.h>

#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>

#include <linux/dma-mapping.h>
#include <linux/device.h> 
#include <linux/cdev.h> 
#include <linux/fs.h> 
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/stat.h>
#include <linux/wakelock.h>
#include <linux/of_irq.h>

//#include <mach/cust_eint.h>
#include <mach/upmu_sw.h>
#include <mt-plat/items.h>

#include "tpd.h"
#include "upmu_common.h"
#include "mt_boot_common.h"
#include "upmu_common.h"

#include <hwmsensor.h>
#include <hwmsen_dev.h>
#include <sensors_io.h>

//--------------------------------------------------
#define TP_COMPATIBLE

#ifdef TP_COMPATIBLE
#include "gslx680_config.h"
#else

//#define TPD_HAVE_BUTTON
#ifdef TPD_HAVE_BUTTON
#define TPD_KEY_COUNT   3
#define TPD_KEYS     {KEY_MENU, KEY_HOMEPAGE, KEY_BACK, KEY_SEARCH}
#endif

#endif
//--------------------------------------------------

//#define GSL_DEBUG      
//#define GSL_MONITOR
//#define HIGH_SPEED_I2C
//#define FILTER_POINT
#define GSL_NOID_VERSION
#define GSLX680_NAME	"gsl1680"
#define GSLX680_ADDR	0x40
#define MAX_FINGERS	  	10
#define MAX_CONTACTS	10
#define DMA_TRANS_LEN	0x20
#define SMBUS_TRANS_LEN	0x01
#define GSL_PAGE_REG		0xf0
#define ADD_I2C_DEVICE_ANDROID_4_0

#ifdef FILTER_POINT
#define FILTER_MAX	9
#endif
//=========================== tp proc debug related variable definition =========================================
#define TPD_PROC_DEBUG
#ifdef TPD_PROC_DEBUG
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>  //lzk
//static struct proc_dir_entry *gsl_config_proc = NULL;
#define GSL_CONFIG_PROC_FILE "gsl_config"
#define CONFIG_LEN 31
static char gsl_read[CONFIG_LEN];
static u8 gsl_data_proc[8] = {0};
static u8 gsl_proc_flag = 0;
#endif
//====================================================================================
#define TPD_PROXIMITY
#define REMOVE_FLOATING_TOUCH //zqb add for tp proximity sometimes call back 20140209

//=========================== tp proximity related variable definition =========================================
#ifdef TPD_PROXIMITY
static u8 tpd_proximity_flag = 0; //flag whether start alps
static u8 tpd_proximity_detect = 1;//0-->close ; 1--> far away
static char TPD_PROC_DISABLE = 0;
static int roller_touch_count = 0;
static struct wake_lock ps_lock;
static u8 gsl_psensor_data[12]={0};
#endif
//====================================================================================


#define TP_POWER "vtouch" //MT6328_POWER_LDO_VGP1

static int touchpanel_gsl_ic=0;


extern unsigned int DISP_GetScreenHeight(void);
extern unsigned int DISP_GetScreenWidth(void);


int TOUCH_IRQ;
u8 int_type = 0;
static struct kobject *mid_tp_kobj;   // all tp related object should place here

#define TPD_PROC_DEBUG




#ifdef TP_COMPATIBLE
unsigned int *PTR_GSL_CONFIG_DATA_ID;
const struct fw_data *PTR_FW;
char name_tp_mode[64];
char *NAME_TP;
unsigned int TP_SOURCE_LEN;

int KEY_NUM;
int TP_NUM;
int LED_PIN;
int (*PTR_TPD_KEYS_DIM_LOCAL)[4];

TP_PROJECT_DATA *PTR_TP_PROJECT_DATA;
#endif
//--------------------------------------------------

//static unsigned int GSL_TP_ID_USED = 0;							     

static int tpd_flag = 0;
static int tpd_halt=0;
static char eint_flag = 0;
extern struct tpd_device *tpd;
static struct i2c_client *i2c_client = NULL;
static struct task_struct *thread = NULL;
#ifdef GSL_MONITOR
static struct delayed_work gsl_monitor_work;
static struct workqueue_struct *gsl_monitor_workqueue = NULL;
static u8 int_1st[4] = {0};
static u8 int_2nd[4] = {0};
static char bc_counter = 0;
static char b0_counter = 0;
static char i2c_lock_flag = 0;
#endif

static uint32_t id_sign[MAX_CONTACTS+1] = {0};
static uint8_t id_state_flag[MAX_CONTACTS+1] = {0};
static uint8_t id_state_old_flag[MAX_CONTACTS+1] = {0};
static uint16_t x_old[MAX_CONTACTS+1] = {0};
static uint16_t y_old[MAX_CONTACTS+1] = {0};
static uint16_t x_new = 0;
static uint16_t y_new = 0;

#define GSL_SEPERATE 
#if 1 //#ifdef GSL_SEPERATE     //Paul@ added 20140116
static u8 gsl_seperate_type = 0; 
#endif
//static u8 light_sleep_flag = 0; //add 20140505

static DECLARE_WAIT_QUEUE_HEAD(waiter);

#ifdef GSL_DEBUG 
#define print_info(fmt, args...)   \
        do{                              \
                printk(fmt, ##args);     \
        }while(0)
#else
#define print_info(fmt, args...)
#endif

#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
static int tpd_wb_start_local[TPD_WARP_CNT] = TPD_WARP_START;
static int tpd_wb_end_local[TPD_WARP_CNT]   = TPD_WARP_END;
#endif
#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
static int tpd_calmat_local[8]     = TPD_CALIBRATION_MATRIX;
static int tpd_def_calmat_local[8] = TPD_CALIBRATION_MATRIX;
#endif


#define TOUCH_LIGHT_CONTROL  1

/*********************** TP GUESTURE START  ********************************************/
#define GSL_GESTURE
#ifdef GSL_GESTURE
static int gsl_gesture_flag = 0;
static struct input_dev *gsl_power_idev;
static int gsl_lcd_flag = 0;
static char gsl_gesture_c = 0;
//static struct mutex gsl_i2c_lock;
extern int gsl_obtain_gesture(void);
extern void gsl_FunIICRead(unsigned int (*)(unsigned int *, unsigned int,  unsigned int));
extern void gsl_GestureExternInt(unsigned int *model, int len);

#define ALPHABET_NUM 13
typedef struct alphabet {
	unsigned int keyCode;
	unsigned int assicCode;
} alphabet_t;

alphabet_t alphabets[ALPHABET_NUM] = {
	{KEY_C, 'C'},
	{KEY_S, 'S'},
	{KEY_E, 'E'},
	{KEY_W, 'W'},
	{KEY_O, 'O'},
	{KEY_M, 'M'},
	{KEY_Z, 'Z'},
	{KEY_V, 'V'},
	{KEY_F1, '*'},
	{KEY_R, (int)0xa1fa},
	{KEY_D, (int)0xa1fd},
	{KEY_U, (int)0xa1fc},
	{KEY_L, (int)0xa1fb}
};

unsigned int gsl_model_extern[] =
{
	0x10, 0x56,
	0x08000f00, 0x0f0c2f1f, 0x12125040, 0x13127060, 0x16149181, 0x1a18b1a1, 0x1c1bd2c2, 0x201df2e2,
	0x3324f7fe, 0x4f41e7ef, 0x6d5ed8df, 0x8a7bc8d0, 0xa698b7c0, 0xc3b4a6af, 0xe0d2959d, 0xffee848d,
	0x10, 0x57,
	0x00062610, 0x03015c41, 0x06049277, 0x0f09c8ad, 0x2918f7e0, 0x5142e4fb, 0x685eb2cb, 0x77707c97,
	0x857d9177, 0x978dc5ab, 0xb4a1f3de, 0xdbcbd5ec, 0xebe4a2bd, 0xf4f06c87, 0xfaf73651, 0xfffd001b,
	0x10, 0x49,
	0x0e00f4ff, 0x2f1ee4ec, 0x4f3ed4dc, 0x6f5fc4cc, 0x8f7fb3bc, 0xae9ea3ab, 0xcebe949b, 0xf0df858d,
	0xf0ff707a, 0xcfdf6268, 0xadbe525a, 0x8d9e434a, 0x6d7d353c, 0x4c5c262e, 0x2c3c151e, 0x0c3c000b,
	0x10, 0x49,
	0x775df6f9, 0xab91e6ef, 0xdac3cedb, 0xf9eda2b9, 0xfdff6d88, 0xf1f93a53, 0xcce21424, 0x9ab50209,
	0x65800101, 0x354d1409, 0x0f1f3a25, 0x01056e53, 0x0100a288, 0x1407d3bc, 0x4128f1e4, 0x765bfffb,
	0x10, 0x57,
	0x0a0a725a, 0x0608a48b, 0x0103d6bd, 0x0300f8ee, 0x2010cee2, 0x382ca2b8, 0x5145758c, 0x695c5e5e,
	0x6b6a8f76, 0x696ac1a8, 0x7367d5da, 0x9081acc0, 0xafa08398, 0xc9bc596f, 0xe4d72d43, 0xfff00016,

	0x10, 0x57,
	0x0f0d1d04, 0x0e104f36, 0x080b8168, 0x0204b39a, 0x0001e5cc, 0x1c05f5fd, 0x3e2ecee2, 0x594da4b9,
	0x7366788d, 0x897f4a61, 0xa0941932, 0xaaaa1900, 0xaaaa4b32, 0xc4af5863, 0xdfd22c43, 0x000e0117,
	0x10, 0x57,
	0x02009380, 0x1408b8a7, 0x3628b0bd, 0x49408c9f, 0x5f52707a, 0x6c659784, 0x7a73beab, 0x8a81e3d1,
	0xa895fef4, 0xc5bae7f7, 0xd9d0c4d6, 0xe5df9db1, 0xeeea768a, 0xf5f14f63, 0xfcfa283b, 0xfffe0014,
	0x10, 0x57,
	0x121a1900, 0x090d4d33, 0x03068167, 0x0102b69b, 0x0400e9d0, 0x2e16f5fd, 0x5241cee2, 0x6e62a2b9,
	0x8479728a, 0x998f405a, 0xaaa10c26, 0xa4a84026, 0x9ea1755a, 0xa99fa78f, 0xd6c297a8, 0x00087384,
	0x10, 0x57,
	0x0c002608, 0x24186344, 0x372ea484, 0x4b40e4c4, 0x6857dbf9, 0x81749ebc, 0xa392627f, 0xc2b42846,
	0xd3cd1907, 0xd5d55939, 0xd3d59a7a, 0xd1d3daba, 0xdcd5cded, 0xeae48dad, 0xf6f04c6c, 0xfffd0b2c,
	0x10, 0x4d,
	0x0100e6ff, 0x0603b3cd, 0x0e09809a, 0x1f164e67, 0x33291d34, 0x5d460009, 0x796f2e15, 0x88826046,
	0x908b4f69, 0xa2972037, 0xcab2030a, 0xebde2c15, 0xf9f45f46, 0xfefc9278, 0xffffc5ab, 0xfffff8de,
	0x10, 0x4d,
	0x0004ddff, 0x040099bb, 0x160d5676, 0x32211635, 0x704e0002, 0xa08f3011, 0xb5ae7350, 0xbdbab795,
	0xc1c0eeda, 0xc2c1a9cc, 0xc8c46587, 0xd9ce2042, 0xf6ec2e0b, 0xfcfb7250, 0xfdfdb795, 0xfffefcd9,
	0x10, 0x4d,
	0x0003788f, 0x0b034960, 0x25161f32, 0x4f37080f, 0x6c622c16, 0x77735c44, 0x7c798674, 0x8c83586e,
	0x9f952b41, 0xbdab0616, 0xe6d51100, 0xf7f13f27, 0xfcfa6f57, 0xfffd9f87, 0xffffcfb7, 0x001fffe7,
	0x10, 0x4d,
	0x0400869e, 0x1b10586f, 0x35262e43, 0x59460b1a, 0x83710d00, 0x82863e26, 0x70797057, 0x7b6c6d81,
	0x9a8b4458, 0xbeab2232, 0xedd51214, 0xfffd3e25, 0xf9fd6f57, 0xe6ef9f87, 0xd6deceb6, 0xbfcaffe6,
};

#endif

/*********************** TP GUESTURE END  ********************************************/

/*********************** TP GUESTURE  sysfs START  ********************************************/
#ifdef GSL_GESTURE
static ssize_t gsl_sysfs_tpgesture_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%c\n", gsl_gesture_c);
}
//wuhao start
static ssize_t gsl_sysfs_tpgesture_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return count;
}

static struct kobj_attribute tpgesture_attr = __ATTR(tpgesture, 0644, gsl_sysfs_tpgesture_show, gsl_sysfs_tpgesture_store);

static ssize_t gsl_sysfs_tpgesture_func_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "tp gesture is %s\n", gsl_gesture_flag == 0 ? "off" : "on");
}
static ssize_t gsl_sysfs_tpgesture_func_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	gsl_gesture_flag = buf[0] == '0' ? 0 : 1;
	return count;
}

static struct kobj_attribute tpgesture_func_attr = __ATTR(tpgesture_func, 0644, gsl_sysfs_tpgesture_func_show, gsl_sysfs_tpgesture_func_store);


static void gsl_request_power_idev(void)
{
	struct input_dev *idev;
	int rc = 0;
	idev = input_allocate_device();
	if (!idev) {
		return;
	}
	gsl_power_idev = idev;
	idev->name = "gsl_gesture";
	idev->id.bustype = BUS_I2C;
	input_set_capability(idev, EV_KEY, KEY_INFO);
	input_set_capability(idev, EV_KEY, KEY_END);

	rc = input_register_device(idev);
	if (rc) {
		input_free_device(idev);
		gsl_power_idev = NULL;
	}
}

static unsigned int gsl_gesture_init(void)
{
	int ret;

	ret = sysfs_create_file(mid_tp_kobj, &tpgesture_attr.attr);
	if (ret){
		printk("%s: sysfs_create_version_file failed\n", __func__);
		return ret;
	}
	gsl_request_power_idev();
	ret = sysfs_create_file(mid_tp_kobj, &tpgesture_func_attr.attr);
	if (ret){
		printk("%s: sysfs_create_version_file failed\n", __func__);
		return ret;
	}
	printk("[GSL_GESTURE] gsl_gesture_init success.\n");
	return 1;
}

static unsigned int gsl_read_oneframe_data(unsigned int *data,unsigned int addr, unsigned int len)
{
	u8 buf[4];
	int i;
	pr_debug("tp-gsl-gesture %s\n", __func__);

	for (i = 0; i < len / 2; i++) {
		buf[0] = ((addr + i * 8) / 0x80) & 0xff;
		buf[1] = (((addr + i * 8) / 0x80) >> 8) & 0xff;
		buf[2] = (((addr + i * 8) / 0x80) >> 16) & 0xff;
		buf[3] = (((addr + i * 8) / 0x80) >> 24) & 0xff;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		i2c_smbus_read_i2c_block_data(i2c_client, (addr + i * 8) % 0x80, 8, (char *)&data[i * 2]);
		i2c_smbus_read_i2c_block_data(i2c_client, (addr + i * 8) % 0x80, 8, (char *)&data[i * 2]);
	}
	if (len % 2) {
		buf[0] = ((addr + len * 4 - 4) / 0x80) & 0xff;
		buf[1] = (((addr + len * 4 - 4) / 0x80) >> 8) & 0xff;
		buf[2] = (((addr + len * 4 - 4) / 0x80) >> 16) & 0xff;
		buf[3] = (((addr + len * 4 - 4) / 0x80) >> 24) & 0xff;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		i2c_smbus_read_i2c_block_data(i2c_client, (addr + len * 4 - 4) % 0x80, 4, (char *)&data[len - 1]);
		i2c_smbus_read_i2c_block_data(i2c_client, (addr + len * 4 - 4) % 0x80, 4, (char *)&data[len - 1]);
	}
	return len;
}
#endif
/*********************** TP GUESTURE  sysfs END  ********************************************/

//=============== TP PEN START  ===========================

#define PEN_Adjust_Freq
#ifdef PEN_Adjust_Freq
//struct mutex pen_mutex;
//spinlock_t spin_locked;//自旋锁变量定义
static int GSL_Adjust_Freq(struct i2c_client *client)
{
	 //unsigned int mode = 0;
	//u64 time_freq,time_start,time_end;
	static unsigned int cpu_start, cpu_end, ret,real_time;
	u8 buf[4];
	struct timeval time_start,time_end;
	printk("gsl pen test\n");
		buf[3] = 0x01;
		buf[2] = 0xfe;
		buf[1] = 0x02;
		buf[0] = 0x00;
		i2c_smbus_write_i2c_block_data(client,0xf0,4,buf);
		printk("--gsl pen test 4--\n");
		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x00;	
		i2c_smbus_write_i2c_block_data(client,0x0c,4,buf);
	    printk("--gsl pen test 3--\n");
		buf[3] = 0x01;
		buf[2] = 0xfe;
		buf[1] = 0x02;
		buf[0] = 0x00;
		i2c_smbus_write_i2c_block_data(client,0xf0,4,buf);
	    printk("--gsl pen test 2--\n");
		buf[3] = 0xff;
		buf[2] = 0xff;
		buf[1] = 0xff;
		buf[0] = 0xff;	
		i2c_smbus_write_i2c_block_data(client,0x04,4,buf);
	   printk("--gsl pen test 1--\n");
		buf[3] = 0x01;
		buf[2] = 0xfe;
		buf[1] = 0x02;
		buf[0] = 0x00;
		i2c_smbus_write_i2c_block_data(client,0xf0,4,buf);
		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x09;
		i2c_smbus_write_i2c_block_data(client,0x08,4,buf);
        printk("--gsl pen test 5--\n");
//		spin_lock(&spin_locked);
	//	mutex_lock(&pen_mutex);
		mdelay(200);
		buf[3] = 0x01;
		buf[2] = 0xfe;
		buf[1] = 0x02;
		buf[0] = 0x00;
		i2c_smbus_write_i2c_block_data(client, 0xf0, 4, buf);
		 printk("--gsl pen test 11--\n");
		i2c_smbus_read_i2c_block_data(client, 0, 4, buf);
		i2c_smbus_read_i2c_block_data(client, 0, 4, buf);
		printk("--gsl pen test 12--\n");
		cpu_start = (buf[3]<<24) + (buf[2]<<16) + (buf[1]<<8) + buf[0];
		printk(" gsl cpu_start = %d\n",cpu_start);
		printk("--gsl pen test 6--\n");
 		do_gettimeofday(&time_start);
//		spin_unlock(&spin_locked);
	//	mutex_unlock(&pen_mutex);
		// start count time
		mdelay(1200);
		
//		spin_lock(&spin_locked);
		//	mutex_lock(&pen_mutex);
		buf[3] = 0x01;
		buf[2] = 0xfe;
		buf[1] = 0x02;
		buf[0] = 0x00;
		i2c_smbus_write_i2c_block_data(client, 0xf0, 4, buf);
		i2c_smbus_read_i2c_block_data(client, 0, 4, buf);
		i2c_smbus_read_i2c_block_data(client, 0, 4, buf);
		cpu_end = (buf[3]<<24) + (buf[2]<<16) + (buf[1]<<8) + buf[0];
		do_gettimeofday(&time_end);
//		spin_unlock(&spin_locked);
	//	mutex_unlock(&pen_mutex);
		printk("--gsl pen test 7--\n");
		real_time = ((time_end.tv_sec-time_start.tv_sec)*10000+(time_end.tv_usec-time_start.tv_usec)/100);
		if(real_time > 10000){ // 1 s 以上
			ret = (u32)((cpu_start - cpu_end )*100/ real_time)*0x1000/9245;
			//芯片计数器计算出来的时间0x1000/9245为换算系数
			if(ret >= 0x1000/2 && ret <= 0x1000*2){//频率偏差应该不会太大
				buf[3] = 0x00;
				buf[2] = 0x00;
				buf[1] = 0x00;
				buf[0] = 0x03;
				i2c_smbus_write_i2c_block_data(client,0xf0,4,buf);
				buf[3] = (u8)((ret>>24)&0xff);
				buf[2] = (u8)((ret>>16)&0xff);
				buf[1] = (u8)((ret>>8)&0xff);
				buf[0] = (u8)(ret&0xff);
				i2c_smbus_write_i2c_block_data(client,0x7c,4,buf); //把校正结果写入
			    printk("--gsl pen test 9--\n");
			}
			else{

				//矫正失败重新矫正
				return -1;
			}
		}
		return 0;
}

#endif
//=============== TP PEN END ============================================
static void startup_chip(struct i2c_client *client)
{
	char write_buf = 0x00;

	i2c_smbus_write_i2c_block_data(client, 0xe0, 1, &write_buf); 	
#ifdef TP_COMPATIBLE
	gsl_DataInit(PTR_GSL_CONFIG_DATA_ID);
#else
	gsl_DataInit(gsl_config_data_id);
#endif
	msleep(10);		
}

static void reset_chip(struct i2c_client *client)
{
	u8 write_buf[4]	= {0};
	u8 buf[4]	= {0};

	write_buf[0] = 0x88;
	i2c_smbus_write_i2c_block_data(client, 0xe0, 1, &write_buf[0]); 	
	msleep(20);

	write_buf[0] = 0x04;
	i2c_smbus_write_i2c_block_data(client, 0xe4, 1, &write_buf[0]); 	
	msleep(10);

	write_buf[0] = 0x00;
	write_buf[1] = 0x00;
	write_buf[2] = 0x00;
	write_buf[3] = 0x00;
	i2c_smbus_write_i2c_block_data(client, 0xbc, 4, write_buf); 	
	msleep(10);
	
    	//set vddio 1.8
	write_buf[0] = 0x00;
	write_buf[1] = 0x00;
	write_buf[2] = 0xfe;
	write_buf[3] = 0x01;
	i2c_smbus_write_i2c_block_data(client,0xf0,4,write_buf);
	buf[0] = 0x05; 
	buf[1] = 0x00; 
	buf[2] = 0x00; 
	buf[3] = 0x80; 
	i2c_smbus_write_i2c_block_data(client,0x78,4,buf);
	msleep(5);	
}

static void clr_reg(struct i2c_client *client)
{
	char write_buf[4]	= {0};

	write_buf[0] = 0x88;
	i2c_smbus_write_i2c_block_data(client, 0xe0, 1, &write_buf[0]); 	
	msleep(20);

	write_buf[0] = 0x03;
	i2c_smbus_write_i2c_block_data(client, 0x80, 1, &write_buf[0]); 	
	msleep(5);
	
	write_buf[0] = 0x04;
	i2c_smbus_write_i2c_block_data(client, 0xe4, 1, &write_buf[0]); 	
	msleep(5);

	write_buf[0] = 0x00;
	i2c_smbus_write_i2c_block_data(client, 0xe0, 1, &write_buf[0]); 	
	msleep(20);
}

static void gsl_load_fw(struct i2c_client *client);

static void cfg_adjust(struct i2c_client *client)
{
	u8 read_buf[4]  = {0};
	
	printk("=============gsl_load_cfg_adjust check start==============\n");
	msleep(500);
	i2c_smbus_read_i2c_block_data(client,0xb8, sizeof(read_buf), read_buf);
    printk("fuc:cfg_adjust, b8: %x %x %x %x\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);

#ifdef TP_COMPATIBLE
	match_gsl_tp_id(read_buf); // gslx_config.c
#endif
        clr_reg(client);
		reset_chip(client);

		gsl_load_fw(client);
		startup_chip(client);
		reset_chip(client);
		startup_chip(client);		
}

static void gsl_load_fw(struct i2c_client *client)
{
	unsigned char buf[SMBUS_TRANS_LEN*4] = {0};
	unsigned char reg = 0, send_flag = 1, cur = 0;
	
	unsigned int source_line = 0;
	unsigned int source_len ;
	const struct fw_data *ptr_fw;

//--------------------------------------------------
#ifdef TP_COMPATIBLE
	ptr_fw = PTR_FW;
	source_len = TP_SOURCE_LEN;
#else
   ptr_fw = GSLX680_FW;
   source_len = ARRAY_SIZE(GSLX680_FW);
#endif
//--------------------------------------------------


	printk("=============gsl_load_fw start==============\n");

	for (source_line = 0; source_line < source_len; source_line++) 
	{
		if(1 == SMBUS_TRANS_LEN)
		{
			reg = ptr_fw[source_line].offset;
			memcpy(&buf[0], &ptr_fw[source_line].val, 4);
			i2c_smbus_write_i2c_block_data(client, reg, 4, buf); 	
		}
		else
		{
			/* init page trans, set the page val */
			if (GSL_PAGE_REG == ptr_fw[source_line].offset)
			{
				buf[0] = (char)(ptr_fw[source_line].val & 0x000000ff);
				i2c_smbus_write_i2c_block_data(client, GSL_PAGE_REG, 1, &buf[0]); 	
				send_flag = 1;
			}
			else 
			{
				if (1 == send_flag % (SMBUS_TRANS_LEN < 0x08 ? SMBUS_TRANS_LEN : 0x08))
					reg = ptr_fw[source_line].offset;

				memcpy(&buf[cur], &ptr_fw[source_line].val, 4);
				cur += 4;

				if (0 == send_flag % (SMBUS_TRANS_LEN < 0x08 ? SMBUS_TRANS_LEN : 0x08)) 
				{
					i2c_smbus_write_i2c_block_data(client, reg, SMBUS_TRANS_LEN*4, buf); 	
					cur = 0;
				}

				send_flag++;

			}
		}
	}

	printk("=============gsl_load_fw end==============\n");

}


#if 1//def GSL_SEPERATE     //Paul@ added 20140116
void get_gsl_seperate_type(struct i2c_client *client)
{
	
	u8 addr[4] = {0};	
	u8 buf[4] = {0};
	u8 buf_first[4] = {0};
	u8 buf_second[4] = {0};
	int i = 0;
	
	addr[3] = 0x01;
	addr[2] = 0xfe;
	addr[1] = 0x04;
	addr[0] = 0x00;
	//config first
	i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
	buf[0] = 0x03; 
	i2c_smbus_write_i2c_block_data(client,0x00,4,buf);
	
	i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
	buf[0] = 0x00; 
	i2c_smbus_write_i2c_block_data(client,0x00,4,buf);
	
	i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
	buf[0] = 0x03; 
	i2c_smbus_write_i2c_block_data(client,0x08,4,buf);
	//read first
	for(i=0;i<3;i++)
	{
		i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
		i2c_smbus_read_i2c_block_data(client,0x04,4,buf_first);
		i2c_smbus_read_i2c_block_data(client,0x04,4,buf_first);
	}	
	printk("func gsl_seperate_type, buf_first = 0x%02x,0x%02x,0x%02x,0x%02x\n",buf_first[3],buf_first[2],buf_first[1],buf_first[0]);
	
	//config second
	i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
	buf[0] = 0x00; 
	i2c_smbus_write_i2c_block_data(client,0x08,4,buf);
	//read second
	for(i=0;i<3;i++)
	{
		i2c_smbus_write_i2c_block_data(client,0xf0,4,addr);
		i2c_smbus_read_i2c_block_data(client,0x04,4,buf_second);
		i2c_smbus_read_i2c_block_data(client,0x04,4,buf_second);
	}	
	printk("func gsl_seperate_type, buf_second = 0x%02x,0x%02x,0x%02x,0x%02x\n",buf_second[3],buf_second[2],buf_second[1],buf_second[0]);
	if((buf_first[0] == buf_second[0]) &&(buf_first[1] == buf_second[1]) &&(buf_first[2] == buf_second[2]) &&(buf_first[3] == buf_second[3]) )
		gsl_seperate_type = 0;//D
	else
		gsl_seperate_type = 1;//A
	
	printk("func gsl_seperate_type, gsl_seperate_type = %d \n",gsl_seperate_type);
}
#endif     //Paul@ added 20140116 end

static int test_i2c(struct i2c_client *client)
{
	char read_buf = 0;
	char write_buf = 0x12;
	int ret, rc = 1;
	
	ret = i2c_smbus_read_i2c_block_data( client, 0xf0, 1, &read_buf );
	if  (ret  < 0)  
    		rc --;
	else
		printk("I read reg 0xf0 is %x\n", read_buf);

	msleep(2);
	ret = i2c_smbus_write_i2c_block_data( client, 0xf0, 1, &write_buf );
	if(ret  >=  0 )
		printk("I write reg 0xf0 0x12\n");
	
	msleep(2);
	ret = i2c_smbus_read_i2c_block_data( client, 0xf0, 1, &read_buf );
	if(ret <  0 )
		rc --;
	else
		printk("I read reg 0xf0 is 0x%x\n", read_buf);

	return rc;
}
static void init_chip_without_rst(struct i2c_client *client)
{
	int rc;
	clr_reg(client);
	reset_chip(client);
	gsl_load_fw(client);
	startup_chip(client);
#ifdef PEN_Adjust_Freq
	if(item_integer("tp.gslx68x.pen",0)==1){
		rc = GSL_Adjust_Freq(client);
		if(rc < 0){
			printk("-------------SL_Adjust_Freq error -------------------\n");
			GSL_Adjust_Freq(client);
		}
	}
#endif
	reset_chip(client);
	startup_chip(client);		
}

static int init_chip(struct i2c_client *client)
{
	int rc;

	tpd_gpio_output(GTP_RST_PORT, 0);
	msleep(20); 	
	tpd_gpio_output(GTP_RST_PORT, 1);
	msleep(20); 		
	rc = test_i2c(client);
	if(rc < 0){
		printk("------gslX680 test_i2c error------\n");	
		return rc;
	}	
	init_chip_without_rst(client);

	return 0;
}

static void check_mem_data(struct i2c_client *client)
{
	char read_buf[4]  = {0};
	
	msleep(30);
	i2c_smbus_read_i2c_block_data(client,0xb0, sizeof(read_buf), read_buf);
	
	if (read_buf[3] != 0x5a || read_buf[2] != 0x5a || read_buf[1] != 0x5a || read_buf[0] != 0x5a)
	{
		printk("#########check mem read 0xb0 = %x %x %x %x #########\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		init_chip(client);
	}
}


//===============================================================================================

#ifdef TPD_PROC_DEBUG
static int char_to_int(char ch)
{
    if(ch>='0' && ch<='9')
        return (ch-'0');
    else
        return (ch-'a'+10);
}

static int gsl_config_read_proc(struct seq_file *m,void *v)
{
	//char *ptr = page;
	char temp_data[5] = {0};
	unsigned int tmp=0;
	unsigned int *ptr_fw;
	
	if('v'==gsl_read[0]&&'s'==gsl_read[1])
	{
#ifdef GSL_NOID_VERSION
		tmp=gsl_version_id();
#else 
		tmp=0x20121215;
#endif
		seq_printf(m,"version:%x\n",tmp);
	}
	else if('r'==gsl_read[0]&&'e'==gsl_read[1]){
		if('i'==gsl_read[3]){
#ifdef GSL_NOID_VERSION 

//--------------------------------------------------
#ifdef TP_COMPATIBLE
	ptr_fw = PTR_GSL_CONFIG_DATA_ID;
#else
	ptr_fw = gsl_config_data_id;
#endif
//--------------------------------------------------
 
	        tmp=(gsl_data_proc[5]<<8) | gsl_data_proc[4];
			seq_printf(m,"gsl_config_data_id[%d] = ",tmp);
			if(tmp>=0&&tmp<512/*ARRAY_SIZE(gsl_config_data_id)*/) //xjl 20160524
			{
					//ptr +=sprintf(ptr,"%d\n",gsl_config_data_id[tmp]); 
					seq_printf(m,"%d\n",ptr_fw[tmp]);
			}
#endif
		}else {
			i2c_smbus_write_i2c_block_data(i2c_client,0xf0,4,&gsl_data_proc[4]);
			if(gsl_data_proc[0] < 0x80)
				i2c_smbus_read_i2c_block_data(i2c_client,gsl_data_proc[0],4,temp_data);
			i2c_smbus_read_i2c_block_data(i2c_client,gsl_data_proc[0],4,temp_data);

			seq_printf(m,"offset : {0x%02x,0x",gsl_data_proc[0]);
			seq_printf(m,"%02x",temp_data[3]);
			seq_printf(m,"%02x",temp_data[2]);
			seq_printf(m,"%02x",temp_data[1]);
			seq_printf(m,"%02x};\n",temp_data[0]);
		}
	}
	return 0;
}
static ssize_t gsl_config_write_proc(struct file *file, const char *buffer, size_t count, loff_t *data)
{
	u8 buf[8] = {0};
	char temp_buf[CONFIG_LEN];
	char *path_buf;
	int tmp = 0;
	int tmp1 = 0;
	print_info("[tp-gsl][%s] \n",__func__);
	if(count > 512)
	{
		print_info("size not match [%d:%d]\n", CONFIG_LEN, count);
        return -EFAULT;
	}
	path_buf=kzalloc(count,GFP_KERNEL);
	if(!path_buf)
	{
		printk("alloc path_buf memory error \n");
	}	
	if(copy_from_user(path_buf, buffer, count))
	{
		print_info("copy from user fail\n");
		goto exit_write_proc_out;
	}
	memcpy(temp_buf,path_buf,(count<CONFIG_LEN?count:CONFIG_LEN));
	print_info("[tp-gsl][%s][%s]\n",__func__,temp_buf);
	
	buf[3]=char_to_int(temp_buf[14])<<4 | char_to_int(temp_buf[15]);	
	buf[2]=char_to_int(temp_buf[16])<<4 | char_to_int(temp_buf[17]);
	buf[1]=char_to_int(temp_buf[18])<<4 | char_to_int(temp_buf[19]);
	buf[0]=char_to_int(temp_buf[20])<<4 | char_to_int(temp_buf[21]);
	
	buf[7]=char_to_int(temp_buf[5])<<4 | char_to_int(temp_buf[6]);
	buf[6]=char_to_int(temp_buf[7])<<4 | char_to_int(temp_buf[8]);
	buf[5]=char_to_int(temp_buf[9])<<4 | char_to_int(temp_buf[10]);
	buf[4]=char_to_int(temp_buf[11])<<4 | char_to_int(temp_buf[12]);
	if('v'==temp_buf[0]&& 's'==temp_buf[1])//version //vs
	{
		memcpy(gsl_read,temp_buf,4);
		printk("gsl version\n");
	}
	else if('s'==temp_buf[0]&& 't'==temp_buf[1])//start //st
	{
		gsl_proc_flag = 1;
		reset_chip(i2c_client);
	}
	else if('e'==temp_buf[0]&&'n'==temp_buf[1])//end //en
	{
		msleep(20);
		reset_chip(i2c_client);
		startup_chip(i2c_client);
		gsl_proc_flag = 0;
	}
	else if('r'==temp_buf[0]&&'e'==temp_buf[1])//read buf //
	{
		memcpy(gsl_read,temp_buf,4);
		memcpy(gsl_data_proc,buf,8);
	}
	else if('w'==temp_buf[0]&&'r'==temp_buf[1])//write buf
	{
		i2c_smbus_write_i2c_block_data(i2c_client,buf[4],4,buf);
	}
#ifdef GSL_NOID_VERSION
	else if('i'==temp_buf[0]&&'d'==temp_buf[1])//write id config //
	{
		tmp1=(buf[7]<<24)|(buf[6]<<16)|(buf[5]<<8)|buf[4];
		tmp=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];

		if(tmp1>=0 && tmp1<256) {
		
//--------------------------------------------------
#ifdef TP_COMPATIBLE
		PTR_GSL_CONFIG_DATA_ID[tmp1] = tmp;
#else
		gsl_config_data_id[tmp1] = tmp;
#endif
//--------------------------------------------------

		}			  		
	}
#endif
exit_write_proc_out:
	kfree(path_buf);
	return count;
}

static int gsl_server_list_open(struct inode *inode,struct file *file)
{
	return single_open(file,gsl_config_read_proc,NULL);
}

static const struct file_operations gsl_seq_fops = {
	.open = gsl_server_list_open,
	.read = seq_read,
	.release = single_release,
	.write = gsl_config_write_proc,
	.owner = THIS_MODULE,
};


#endif


#ifdef FILTER_POINT
static void filter_point(u16 x, u16 y , u8 id)
{
	u16 x_err =0;
	u16 y_err =0;
	u16 filter_step_x = 0, filter_step_y = 0;
	
	id_sign[id] = id_sign[id] + 1;
	if(id_sign[id] == 1)
	{
		x_old[id] = x;
		y_old[id] = y;
	}
	
	x_err = x > x_old[id] ? (x -x_old[id]) : (x_old[id] - x);
	y_err = y > y_old[id] ? (y -y_old[id]) : (y_old[id] - y);

	if( (x_err > FILTER_MAX && y_err > FILTER_MAX/3) || (x_err > FILTER_MAX/3 && y_err > FILTER_MAX) )
	{
		filter_step_x = x_err;
		filter_step_y = y_err;
	}
	else
	{
		if(x_err > FILTER_MAX)
			filter_step_x = x_err; 
		if(y_err> FILTER_MAX)
			filter_step_y = y_err;
	}

	if(x_err <= 2*FILTER_MAX && y_err <= 2*FILTER_MAX)
	{
		filter_step_x >>= 2; 
		filter_step_y >>= 2;
	}
	else if(x_err <= 3*FILTER_MAX && y_err <= 3*FILTER_MAX)
	{
		filter_step_x >>= 1; 
		filter_step_y >>= 1;
	}	
	else if(x_err <= 4*FILTER_MAX && y_err <= 4*FILTER_MAX)
	{
		filter_step_x = filter_step_x*3/4; 
		filter_step_y = filter_step_y*3/4;
	}	
	
	x_new = x > x_old[id] ? (x_old[id] + filter_step_x) : (x_old[id] - filter_step_x);
	y_new = y > y_old[id] ? (y_old[id] + filter_step_y) : (y_old[id] - filter_step_y);

	x_old[id] = x_new;
	y_old[id] = y_new;
}
#else

static void record_point(u16 x, u16 y , u8 id)
{
	u16 x_err =0;
	u16 y_err =0;

	id_sign[id]=id_sign[id]+1;
	
	if(id_sign[id]==1){
		x_old[id]=x;
		y_old[id]=y;
	}

	x = (x_old[id] + x)/2;
	y = (y_old[id] + y)/2;
		
	if(x>x_old[id]){
		x_err=x -x_old[id];
	}
	else{
		x_err=x_old[id]-x;
	}

	if(y>y_old[id]){
		y_err=y -y_old[id];
	}
	else{
		y_err=y_old[id]-y;
	}

	if( (x_err > 3 && y_err > 1) || (x_err > 1 && y_err > 3) ){
		x_new = x;     x_old[id] = x;
		y_new = y;     y_old[id] = y;
	}
	else{
		if(x_err > 3){
			x_new = x;     x_old[id] = x;
		}
		else
			x_new = x_old[id];
		if(y_err> 3){
			y_new = y;     y_old[id] = y;
		}
		else
			y_new = y_old[id];
	}

	if(id_sign[id]==1){
		x_new= x_old[id];
		y_new= y_old[id];
	}
	
}
#endif

//=======================tp preximity related ========================================
#ifdef TPD_PROXIMITY
static void gsl_gain_psensor_data(struct i2c_client *client)
{
	int tmp = 0;
	u8 buf[4] = {0};
	/**************************/
	buf[0] = 0x3;
	i2c_smbus_write_i2c_block_data(client, 0xf0, 4, buf);
	tmp = i2c_smbus_read_i2c_block_data(client, 0x0, 4, &gsl_psensor_data[0]);
	if (tmp <= 0) {
		i2c_smbus_read_i2c_block_data(client, 0x0, 4, &gsl_psensor_data[0]);
	}

	buf[0] = 0x4;
	i2c_smbus_write_i2c_block_data(client, 0xf0, 4, buf);
	tmp = i2c_smbus_read_i2c_block_data(client, 0x0, 4, &gsl_psensor_data[4]);
	if (tmp <= 0) {
		i2c_smbus_read_i2c_block_data(client, 0x0, 4, &gsl_psensor_data[4]);
	}
}

static int tpd_get_ps_value(void)
{
	return tpd_proximity_detect;
}

static int tpd_enable_ps(int enable)
{
#ifndef REMOVE_FLOATING_TOUCH
	u8 buf[4] = {0};
#endif
	printk("tpd_enable_ps:enable= %d\n TPD_PROC_DISABLE=%d\n", enable, TPD_PROC_DISABLE);
	if (enable && (TPD_PROC_DISABLE == 0)) { //Paul modifed for proximity @20130812
#ifndef REMOVE_FLOATING_TOUCH
		wake_lock(&ps_lock);
		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x3;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		buf[3] = 0x5a;
		buf[2] = 0x5a;
		buf[1] = 0x5a;
		buf[0] = 0x5a;
		i2c_smbus_write_i2c_block_data(i2c_client, 0, 4, buf);

		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x4;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		buf[3] = 0x0;
		buf[2] = 0x0;
		buf[1] = 0x0;
		buf[0] = 0x2;
		i2c_smbus_write_i2c_block_data(i2c_client, 0, 4, buf);

#endif
		tpd_proximity_flag = 1;
#ifdef GSL_MONITOR
		printk( "gsl_ts_suspend () : cancel gsl_monitor_work\n");
		cancel_delayed_work_sync(&gsl_monitor_work);
#endif
		//add alps of function
		printk("tpd-ps function is on\n");
	} else {
		tpd_proximity_flag = 0;
#ifndef REMOVE_FLOATING_TOUCH
		wake_unlock(&ps_lock);
		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x3;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		buf[3] = gsl_psensor_data[3];
		buf[2] = gsl_psensor_data[2];
		buf[1] = gsl_psensor_data[1];
		buf[0] = gsl_psensor_data[0];
		printk("tpd-ps off, gsl_psensor_data[3] = 0x%02x,gsl_psensor_data[2] = 0x%02x,gsl_psensor_data[1] = 0x%02x,gsl_psensor_data[0] = 0x%02x,\n", gsl_psensor_data[3], gsl_psensor_data[2], gsl_psensor_data[1], gsl_psensor_data[0]);
		i2c_smbus_write_i2c_block_data(i2c_client, 0, 4, buf);

		buf[3] = 0x00;
		buf[2] = 0x00;
		buf[1] = 0x00;
		buf[0] = 0x4;
		i2c_smbus_write_i2c_block_data(i2c_client, 0xf0, 4, buf);
		buf[3] = gsl_psensor_data[7];
		buf[2] = gsl_psensor_data[6];
		buf[1] = gsl_psensor_data[5];
		buf[0] = gsl_psensor_data[4];
		printk("tpd-ps off, gsl_psensor_data[7] = 0x%02x,gsl_psensor_data[6] = 0x%02x,gsl_psensor_data[5] = 0x%02x,gsl_psensor_data[4] = 0x%02x,\n", gsl_psensor_data[7], gsl_psensor_data[6], gsl_psensor_data[5], gsl_psensor_data[4]);
		i2c_smbus_write_i2c_block_data(i2c_client, 0x0, 4, buf);
#endif
#ifdef GSL_MONITOR
		printk( "gsl_ts_resume () : queue gsl_monitor_work\n");
		queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 300);
#endif
		printk("tpd-ps function is off\n");
	}
	return 0;
}

int tpd_ps_operate(void* self, uint32_t command, void* buff_in, int size_in, void* buff_out, int size_out, int* actualout)
{
	int err = 0;
	int value;
	struct hwm_sensor_data *sensor_data;

	printk("z3__________________ tpd_ps_operate command: %d\n", command);

	switch (command) {
	case SENSOR_DELAY:
		if ((buff_in == NULL) || (size_in < sizeof(int))) {
			printk("Set delay parameter error!\n");
			err = -EINVAL;
		}
		break;

	case SENSOR_ENABLE:
		if ((buff_in == NULL) || (size_in < sizeof(int))) {
			printk("Enable sensor parameter error!\n");
			err = -EINVAL;
		} else {
			value = *(int *)buff_in;
			if ((tpd_enable_ps(value ? 1 : 0) != 0)) {
				printk("enable ps fail: %d\n", err);
				return -1;
			}
		}
		break;
	case SENSOR_GET_DATA:
		if ((buff_out == NULL) || (size_out < sizeof(struct hwm_sensor_data))) {
			printk("get sensor data parameter error!\n");
			err = -EINVAL;
		} else {
			sensor_data = (struct hwm_sensor_data *)buff_out;
			sensor_data->values[0] = tpd_get_ps_value();
			sensor_data->value_divide = 1;
			sensor_data->status = SENSOR_STATUS_ACCURACY_MEDIUM;
		}
		break;
	default:
		printk("proxmy sensor operate function no this parameter %d!\n", command);
		err = -1;
		break;
	}

	return err;
}
#endif
//===================================================================================



/*============================TP_PROXIMITY_START sysfs related ===============================================*/
//#ifdef TPD_PROXIMITY

ssize_t tp_alsps_show(struct kobject *kobject, struct kobj_attribute *attr, char *buf)
{
//	printk("%s, attrname:%s,  alsps_en = %s \n", __func__,  attr->name, TPD_PROC_DISABLE ? "yes" : "no");
	return sprintf(buf, "TPD_PROC_DISABLE=%s\n", TPD_PROC_DISABLE ? "yes" : "no" );
}

/**
 *  hrer we do not control tpd_proximity_flag because we may want to control tp promity in tpd_enable_ps
 * when a process want to access alsps, it first call  tpd_enable_ps  first
 */
ssize_t tp_alsps_store(struct kobject *kobject, struct kobj_attribute *attr, const char *buf, size_t count)
{
	if (buf[0] == 'e' && buf[1] == 'n') {
		TPD_PROC_DISABLE = 0;
	} else {
		TPD_PROC_DISABLE = 1;
	}
	pr_debug("%s, TPD_PROC_DISABLE=%d\n", __func__, TPD_PROC_DISABLE);
	return count;
}
static struct kobj_attribute tp_alsps_en_attr = __ATTR(tp_alsps_en, 0644, tp_alsps_show, tp_alsps_store);

static int tp_alsps_init(void)
{
	int ret = 0;

	ret = sysfs_create_file(mid_tp_kobj, &tp_alsps_en_attr.attr);
	if (ret){
		printk("%s: sysfs_create_version_file failed\n", __func__);
	}
	return ret;
}

static int tp_alsps_exit(void)
{
	pr_debug("%s\n", __func__);
	return 0;
}
//#endif
/*============================TP_PROXIMITY_END===============================================*/


void tpd_down( int id, int x, int y, int p) 
{

	print_info("------tpd_down id: %d, x:%d, y:%d------ \n", id, x, y);

	control_input_orientation(&x, &y, DISP_GetScreenWidth(), DISP_GetScreenHeight()); // MID_devconfig_tp.c

#ifdef TP_COMPATIBLE
	control_key_light(x, y); // gslx_config.c 
#endif
//--------------------------------------------------


	input_report_key(tpd->dev, BTN_TOUCH, 1);
	//input_report_abs(tpd->dev, ABS_MT_TOUCH_MAJOR, 1);
	input_report_abs(tpd->dev, ABS_MT_POSITION_X, x);
	input_report_abs(tpd->dev, ABS_MT_POSITION_Y, y);
	input_report_abs(tpd->dev, ABS_MT_TRACKING_ID, id); 	
	input_mt_sync(tpd->dev);

	if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
	{   
	}
}

// struct timer_list led_timer; 

void tpd_up(void) 
{
	print_info("------tpd_up------ \n");
//--------------------------------------------------
#ifdef TP_COMPATIBLE
	key_light_mod_timer(); //gslx_config.c
#endif
//--------------------------------------------------	

	input_report_key(tpd->dev, BTN_TOUCH, 0);
	input_mt_sync(tpd->dev);
	
	if (FACTORY_BOOT == get_boot_mode()|| RECOVERY_BOOT == get_boot_mode())
	{
	}
}

static void report_data_handle(void)
{
	char touch_data[MAX_FINGERS * 4 + 4] = {0};
	char buf[4] = {0};
	unsigned char id = 0;
	char point_num = 0;
	unsigned int x, y, temp_a, temp_b, i,tmp1;

#ifdef GSL_NOID_VERSION
	struct gsl_touch_info cinfo;
#endif

#ifdef GSL_MONITOR
	if(i2c_lock_flag != 0)
		return;
	else
		i2c_lock_flag = 1;
#endif


//#ifdef TPD_PROXIMITY
	if(item_integer("sensor.tp.proximity",0)==1)
	{
    #ifndef REMOVE_FLOATING_TOUCH
	u32 err,hwm_touch_point = 0;
	struct hwm_sensor_data sensor_data;
    /*added by bernard*/
	if (tpd_proximity_flag == 1)
	{

		i2c_smbus_read_i2c_block_data(i2c_client,0xac, sizeof(buf), buf);
		printk("HWM get AC %d\n",buf[0]);
		if (buf[0] == 1 && buf[1] == 0 && buf[2] == 0 && buf[3] == 0)
		{

			//printk("tpd_proximity_counter %d\n",tpd_proximity_counter);
				//--------------@Paul 20130710-----------//
			i2c_smbus_read_i2c_block_data(i2c_client, 0x80, 4, &hwm_touch_point);
			hwm_touch_point &= 0x0f;
			printk("HWM get touch point %d\n",hwm_touch_point);
	
			if(hwm_touch_point >= 2)//if here were tow point touch 
			{//then we need to report close screen
				tpd_proximity_detect = 0;
				printk("DO DO DO close screen !! %d\n",__LINE__);
			}
			else if(hwm_touch_point == 0)
			{

				if((tpd_proximity_counter > 3)) //ar by touch
				{
					tpd_proximity_counter = 0;
					tpd_proximity_detect = 0;
				}
				else
				{
					tpd_proximity_counter ++;
				}
				roller_touch_count = 0;
			}
			else //one point else
			{
				if(roller_touch_count > 30)
				{
					roller_touch_count = 0;
					tpd_proximity_detect = 0;
				}
				else
				{
					roller_touch_count++;
				}
				tpd_proximity_counter = 0;
			}
		}
		else
		{
			tpd_proximity_counter = 0;
			roller_touch_count = 0;
			tpd_proximity_detect = 1;
			//sensor_data.values[0] = 1;
		}

		sensor_data.values[0] = tpd_get_ps_value();
		sensor_data.value_divide = 1;
		sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
		//let up layer to know
		if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
		{
			print_info("call hwmsen_get_interrupt_data fail = %d\n", err);
		}
	}
	else
	{
		tpd_proximity_counter = 0;
		roller_touch_count =0;
	}
	/*end of added*/
   #endif	
}
//#endif	
#ifdef TPD_PROC_DEBUG
    if(gsl_proc_flag == 1)
        return;
#endif

	i2c_smbus_read_i2c_block_data(i2c_client, 0x80, 4, &touch_data[0]);
	point_num = touch_data[0];
	if(point_num > 0)
		i2c_smbus_read_i2c_block_data(i2c_client, 0x84, 8, &touch_data[4]);
	if(point_num > 2)
		i2c_smbus_read_i2c_block_data(i2c_client, 0x8c, 8, &touch_data[12]);
	if(point_num > 4)
		i2c_smbus_read_i2c_block_data(i2c_client, 0x94, 8, &touch_data[20]);
	if(point_num > 6)
		i2c_smbus_read_i2c_block_data(i2c_client, 0x9c, 8, &touch_data[28]);
	if(point_num > 8)
		i2c_smbus_read_i2c_block_data(i2c_client, 0xa4, 8, &touch_data[36]);
	
#ifdef GSL_NOID_VERSION
	cinfo.finger_num = point_num;
	print_info("tp-gsl  finger_num = %d\n",cinfo.finger_num);
	for(i = 0; i < (point_num < MAX_CONTACTS ? point_num : MAX_CONTACTS); i ++)
	{
		temp_a = touch_data[(i + 1) * 4 + 3] & 0x0f;
		temp_b = touch_data[(i + 1) * 4 + 2];
		cinfo.x[i] = temp_a << 8 |temp_b;
		temp_a = touch_data[(i + 1) * 4 + 1];
		temp_b = touch_data[(i + 1) * 4 + 0];
		cinfo.y[i] = temp_a << 8 |temp_b;		
		print_info("tp-gsl  x = %d y = %d \n",cinfo.x[i],cinfo.y[i]);
	}
	cinfo.finger_num = (touch_data[3]<<24)|(touch_data[2]<<16)|
		(touch_data[1]<<8)|touch_data[0];
	gsl_alg_id_main(&cinfo);
	tmp1=gsl_mask_tiaoping();
	print_info("[tp-gsl] tmp1=%x\n",tmp1);
	if(tmp1>0&&tmp1<0xffffffff)
	{
		buf[0]=0xa;buf[1]=0;buf[2]=0;buf[3]=0;
		i2c_smbus_write_i2c_block_data(i2c_client,0xf0,4,buf);
		buf[0]=(u8)(tmp1 & 0xff);
		buf[1]=(u8)((tmp1>>8) & 0xff);
		buf[2]=(u8)((tmp1>>16) & 0xff);
		buf[3]=(u8)((tmp1>>24) & 0xff);
		print_info("tmp1=%08x,buf[0]=%02x,buf[1]=%02x,buf[2]=%02x,buf[3]=%02x\n",
			tmp1,buf[0],buf[1],buf[2],buf[3]);
		i2c_smbus_write_i2c_block_data(i2c_client,0x8,4,buf);
	}
	point_num = cinfo.finger_num;
#endif

//#ifdef TPD_PROXIMITY
  if(item_integer("sensor.tp.proximity",0)==1)
  {
  #ifdef REMOVE_FLOATING_TOUCH	
	u32 err;
	struct hwm_sensor_data sensor_data;
	/*added by bernard*/
	if(tpd_proximity_flag == 1)
	{		
		if(point_num >= 2)
		{
			tpd_proximity_detect = 0;
			roller_touch_count = 20;
		}
		else if(point_num == 1 && cinfo.x[0] < 200)
		{
			if(roller_touch_count >= 20)
			{
				tpd_proximity_detect = 0;
				roller_touch_count = 20;
			}
			else
				roller_touch_count ++;
		}
		else // no pointer on 
		{
			//if(roller_touch_count <= 0)
			{
				tpd_proximity_detect = 1;
				roller_touch_count = 0;
			}
		}

		sensor_data.values[0] = tpd_get_ps_value();
		sensor_data.value_divide = 1;
		sensor_data.status = SENSOR_STATUS_ACCURACY_MEDIUM;
		//let up layer to know
		if((err = hwmsen_get_interrupt_data(ID_PROXIMITY, &sensor_data)))
		{
			print_info("call hwmsen_get_interrupt_data fail = %d\n", err);
		}
	}
	else
	{
		roller_touch_count =0;
	}
	/*end of added*/
  #endif	
  	}
//#endif	
/*********************** TP GUESTURE   ********************************************/
	pr_debug("[GSL_GESTURE] tpd_halt =%d, gsl_gesture_flag = %d \n",tpd_halt, gsl_gesture_flag);
	if(gsl_gesture_flag == 1){
		int tmp_c;
		tmp_c = gsl_obtain_gesture();
		pr_debug("[GSL_GESTURE] tmp_c =%d \n",tmp_c);
		// only work in tp halt state
		if(tpd_halt == 1){
			unsigned int tmpKeyCode = KEY_INFO;
			int alphabetIndex = 0;
			for(alphabetIndex=0; alphabetIndex<ALPHABET_NUM; alphabetIndex++){
				if(tmp_c == (int)alphabets[alphabetIndex].assicCode){
					tmp_c = alphabets[alphabetIndex].assicCode;
					tmpKeyCode = alphabets[alphabetIndex].keyCode;
					break;
				}
			}
			gsl_gesture_c = (char)tmp_c;
			if(gsl_gesture_c > 0){
				//if(gsl_lcd_flag == 0)
				//reset_chip(i2c_client);
				//startup_chip(i2c_client); 
				input_report_key(tpd->dev,tmpKeyCode,1);
				input_sync(tpd->dev);
				input_report_key(tpd->dev,tmpKeyCode,0);
				input_sync(tpd->dev);
				gsl_lcd_flag = 1;
				msleep(300);
			}

			return;
		}
	}
/*********************** TP GUESTURE   ********************************************/

	for(i = 1 ;i <= MAX_CONTACTS; i ++)
	{
		if(point_num == 0)
			id_sign[i] = 0;	
		id_state_flag[i] = 0;
	}
	for(i = 0; i < (point_num < MAX_FINGERS ? point_num : MAX_FINGERS); i ++)
	{
	#ifdef GSL_NOID_VERSION
		id = cinfo.id[i];
		x =  cinfo.x[i];
		y =  cinfo.y[i];
	#else
		id = touch_data[(i + 1) * 4 + 3] >> 4;
		temp_a = touch_data[(i + 1) * 4 + 3] & 0x0f;
		temp_b = touch_data[(i + 1) * 4 + 2];
		x = temp_a << 8 |temp_b;
		temp_a = touch_data[(i + 1) * 4 + 1];
		temp_b = touch_data[(i + 1) * 4 + 0];
		y = temp_a << 8 |temp_b;	
	#endif
	
		if(1 <= id && id <= MAX_CONTACTS)
		{
		#ifdef FILTER_POINT
			filter_point(x, y ,id);
		#else
			record_point(x, y , id);
		#endif
			tpd_down(id, x_new, y_new, 10);
			id_state_flag[id] = 1;
		}
	}
	for(i = 1; i <= MAX_CONTACTS; i ++)
	{	
		if( (0 == point_num) || ((0 != id_state_old_flag[i]) && (0 == id_state_flag[i])) )
		{
			id_sign[i]=0;
		}
		id_state_old_flag[i] = id_state_flag[i];
	}			
	if(0 == point_num)
	{
		tpd_up();
	}
	input_sync(tpd->dev);
#ifdef GSL_MONITOR
	i2c_lock_flag = 0;
#endif
}

#ifdef GSL_MONITOR
static void gsl_monitor_worker(void)
{
	u8 read_buf[4]  = {0};
	u8 init_chip_flag = 0;
	
	print_info("----------------gsl_monitor_worker-----------------\n");	
#ifdef TPD_PROC_DEBUG
    if(gsl_proc_flag == 1)
    {
    	// printk("<<<< wanghe gsl_proc_flag == 1");
        return;
    }
#endif	
	if(i2c_lock_flag != 0)
		goto queue_monitor_work;
	else
		i2c_lock_flag = 1;
	
	i2c_smbus_read_i2c_block_data(i2c_client, 0xb0, 4, read_buf);
	if(read_buf[3] != 0x5a || read_buf[2] != 0x5a || read_buf[1] != 0x5a || read_buf[0] != 0x5a)
		b0_counter ++;
	else
		b0_counter = 0;

	if(b0_counter > 1)
	{
		printk("======read 0xb0: %x %x %x %x ======\n",read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		init_chip_flag = 1;
		b0_counter = 0;
	}

	i2c_smbus_read_i2c_block_data(i2c_client, 0xb4, 4, read_buf);	
	
	int_2nd[3] = int_1st[3];
	int_2nd[2] = int_1st[2];
	int_2nd[1] = int_1st[1];
	int_2nd[0] = int_1st[0];
	int_1st[3] = read_buf[3];
	int_1st[2] = read_buf[2];
	int_1st[1] = read_buf[1];
	int_1st[0] = read_buf[0];

	if (int_1st[3] == int_2nd[3] && int_1st[2] == int_2nd[2] &&int_1st[1] == int_2nd[1] && int_1st[0] == int_2nd[0]) 
	{
		printk("======int_1st: %x %x %x %x , int_2nd: %x %x %x %x ======\n",int_1st[3], int_1st[2], int_1st[1], int_1st[0], int_2nd[3], int_2nd[2],int_2nd[1],int_2nd[0]);
		init_chip_flag = 1;
	}

	i2c_smbus_read_i2c_block_data(i2c_client, 0xbc, 4, read_buf);
	if(read_buf[3] != 0 || read_buf[2] != 0 || read_buf[1] != 0 || read_buf[0] != 0)
		bc_counter ++;
	else
		bc_counter = 0;

	if(bc_counter > 1)
	{
		printk("======read 0xbc: %x %x %x %x ======\n",read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		init_chip_flag = 1;
		bc_counter = 0;
	}
	
	if(init_chip_flag)
		init_chip(i2c_client);
	
	i2c_lock_flag = 0;
	
queue_monitor_work:	
	queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 100);
}
#endif

static int touch_event_handler(void *unused)
{
	struct sched_param param = { .sched_priority = 4 };
	sched_setscheduler(current, SCHED_RR, &param);
	
	do{
		set_current_state(TASK_INTERRUPTIBLE);
		wait_event_interruptible(waiter, tpd_flag != 0);
		tpd_flag = 0;
		TPD_DEBUG_SET_TIME;
		set_current_state(TASK_RUNNING);
		print_info("===touch_event_handler, task running===\n");

		eint_flag = 0;
		report_data_handle();
		
	} while (!kthread_should_stop());
	
	return 0;
}

static irqreturn_t tpd_eint_interrupt_handler(void)
{
	TPD_DEBUG_PRINT_INT;
	//printk("[TouchScreen] %s() start. \n", __func__);
	eint_flag = 1;
	tpd_flag=1; 
	wake_up_interruptible(&waiter);
	return IRQ_HANDLED;
}

static int tpd_i2c_detect(struct i2c_client *client, struct i2c_board_info *info) {
	strcpy(info->type, TPD_DEVICE);
	return 0;
}

static int tpd_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id) {
	int err = 0;
	int ret = 0;


	struct device_node *node;
	
//#ifdef TPD_PROXIMITY
	struct hwmsen_object obj_ps;
//#endif
	printk("[TouchScreen] %s() start. \n", __func__);;

	tpd_gpio_output(GTP_RST_PORT, 0);

		msleep(100);

	tpd_gpio_output(GTP_RST_PORT, 1);
	tpd_gpio_as_int(GTP_INT_PORT);
	msleep(50);

	node = of_find_matching_node(NULL, touch_of_match);
	//node = of_find_compatible_node(NULL, NULL, "mediatek,cap_touch");
		
	if (node) {
		TOUCH_IRQ = irq_of_parse_and_map(node, 0);
		ret = request_irq(TOUCH_IRQ,
				  (irq_handler_t)tpd_eint_interrupt_handler,
				  !int_type ? IRQF_TRIGGER_RISING :
				  IRQF_TRIGGER_FALLING,
				  "TOUCH_PANEL-eint", NULL);
		if (ret > 0) {
			ret = -1;
			printk("[TouchScreen] tpd request_irq IRQ LINE NOT AVAILABLE!.");
		}
	}


#if 1
	get_gsl_seperate_type(client);
#endif
//#ifdef GSL_GESTURE
	gsl_FunIICRead(gsl_read_oneframe_data);
	gsl_GestureExternInt(gsl_model_extern,sizeof(gsl_model_extern)/sizeof(unsigned int)/18);
//#endif
	i2c_client = client;
	i2c_client->addr = 0x80 >> 1;  // force i2c addresss 0x40
	i2c_client->timing = 400;	//Paul@ added for i2c speed 
#ifdef PEN_Adjust_Freq
	if(item_integer("tp.gslx68x.pen",0)==1){
		//	spin_lock_init(&spin_locked);
	}
#endif
	ret = init_chip(i2c_client);
	if(ret < 0)
			return ret;

	check_mem_data(i2c_client);
	//#ifdef GSL915_TP_COMPATIBILITY
	cfg_adjust(i2c_client);//
      //#endif
#ifdef PEN_Adjust_Freq
	if(item_integer("tp.gslx68x.pen",0)==1){
		GSL_Adjust_Freq(client);
	}
#endif
	tpd_load_status = 1; // load flag bit
	  
	thread = kthread_run(touch_event_handler, 0, TPD_DEVICE);
	if (IS_ERR(thread)) {
		err = PTR_ERR(thread);
		TPD_DMESG(TPD_DEVICE " failed to create kernel thread: %d\n", err);
	}

#ifdef GSL_MONITOR
	printk( "tpd_i2c_probe () : queue gsl_monitor_workqueue\n");

	INIT_DELAYED_WORK(&gsl_monitor_work, gsl_monitor_worker);
	gsl_monitor_workqueue = create_singlethread_workqueue("gsl_monitor_workqueue");
	queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 1000);
#endif


#ifdef TPD_PROC_DEBUG
    proc_create(GSL_CONFIG_PROC_FILE,0666,NULL,&gsl_seq_fops);
    gsl_proc_flag = 0;
#endif

	printk("[TouchScreen] %s() End. \n", __func__);

		
//#ifdef TPD_PROXIMITY
	if(item_integer("sensor.tp.proximity",0)==1){
		obj_ps.polling = 0;//interrupt mode
		obj_ps.sensor_operate = tpd_ps_operate;  //gsl1680p_ps_operate;
		if((err = hwmsen_attach(ID_PROXIMITY, &obj_ps))){
			printk("attach fail = %d\n", err);
		}
		gsl_gain_psensor_data(i2c_client);
		wake_lock_init(&ps_lock, WAKE_LOCK_SUSPEND, "ps wakelock");
	}
//#endif
#ifdef GSL_GESTURE
	{
		int alphabetIndex = 0;
		input_set_capability(tpd->dev, EV_KEY, KEY_POWER);
		input_set_capability(tpd->dev, EV_KEY, KEY_INFO);
		
	        for(alphabetIndex=0; alphabetIndex<ALPHABET_NUM; alphabetIndex++){
			input_set_capability(tpd->dev,EV_KEY,alphabets[alphabetIndex].keyCode);
		}	
		gsl_gesture_init();
	}
#endif

	tpd_load_status = 1;

	return 0;
}

static int tpd_i2c_remove(struct i2c_client *client)
{
	printk("[TouchScreen] %s() start. \n", __func__);
	
	return 0;
}


static const struct i2c_device_id tpd_i2c_id[] = {{TPD_DEVICE,0},{}};
#ifdef ADD_I2C_DEVICE_ANDROID_4_0
static struct i2c_board_info __initdata gslX680_i2c_tpd={ I2C_BOARD_INFO(TPD_DEVICE, (GSLX680_ADDR))};
#else
static unsigned short force[] = {0, (GSLX680_ADDR << 1), I2C_CLIENT_END,I2C_CLIENT_END};
static const unsigned short * const forces[] = { force, NULL };
//static struct i2c_client_address_data addr_data = { .forces = forces,};
#endif

static const struct of_device_id tpd_of_match[] = {
	{.compatible = "mediatek,cap_touch"},
	{},
};

struct i2c_driver tpd_i2c_driver = {
	.driver = {
		.name = TPD_DEVICE,		
#ifndef ADD_I2C_DEVICE_ANDROID_4_0	 
		.owner = THIS_MODULE,
#endif
		.of_match_table = tpd_of_match,
		},
	.probe = tpd_i2c_probe,
	.remove = tpd_i2c_remove,
	.id_table = tpd_i2c_id,
	.detect = tpd_i2c_detect,
#ifndef ADD_I2C_DEVICE_ANDROID_4_0
	.address_list = (const unsigned short *)forces,
#endif
};

int tpd_local_init(void)
{
	int ret = 0;

	printk("==tpd_local_init==\n");
	
	printk("[TouchScreen] %s() start. \n", __func__);

#ifdef TP_COMPATIBLE
	items_config_tp(); //gslx_config.c
	tp_list_config(); //gslx_config.c
#endif

#ifdef TP_COMPATIBLE
	ret = tpd_hwPowerControl(VOL_3300, TP_POWER, 1);
#else	
	//hwPowerOn(tp_power, VOL_2800, "TP");
	tpd->reg = regulator_get(tpd->tpd_dev, TP_POWER);	
	if (IS_ERR(tpd->reg))		
		printk("regulator_get() failed!\n");
	
	ret = regulator_set_voltage(tpd->reg, 2800000, 2800000);/* set 2.8v */
	if (ret)
		printk("regulator_set_voltage() failed!\n");
	ret = regulator_enable(tpd->reg);	/* enable regulator */
	if (ret)
		printk("regulator_enable() failed!\n");
#endif

	if(i2c_add_driver(&tpd_i2c_driver)!=0) {
		printk("%s() unable to add i2c driver.\n", __func__);
		return -1;
	}

	if (tpd_load_status == 0) {		
		printk("add error touch panel driver.\n");		
		i2c_del_driver(&tpd_i2c_driver);		
		return -1;	
	}	
	
//--------------------------------------------------
#ifdef TP_COMPATIBLE
	match_gsl_tp_button(); //gslx_config.c
#else
 #ifdef TPD_HAVE_BUTTON
   tpd_button_setting(TPD_KEY_COUNT, tpd_keys_local, tpd_keys_dim_local);
 #endif
#endif
//--------------------------------------------------


#if (defined(TPD_WARP_START) && defined(TPD_WARP_END))
	TPD_DO_WARP = 1;
	memcpy(tpd_wb_start, tpd_wb_start_local, TPD_WARP_CNT*4);
	memcpy(tpd_wb_end, tpd_wb_start_local, TPD_WARP_CNT*4);
#endif

#if (defined(TPD_HAVE_CALIBRATION) && !defined(TPD_CUSTOM_CALIBRATION))
	memcpy(tpd_calmat, tpd_calmat_local, 8*4);
	memcpy(tpd_def_calmat, tpd_def_calmat_local, 8*4);
#endif
	tpd_type_cap = 1;

	printk("==tpd_local_init end==\n");
	return 0;
}

/* Function to manage low power suspend */
static void tpd_suspend(struct device *h)
{
	pr_debug("[TouchScreen] %s() start. \n", __func__);
	//#ifdef TPD_PROXIMITY
	if(item_integer("sensor.tp.proximity",0)==1){
		if (tpd_proximity_flag == 1)
	    {
	        return ;
	    }
	}
	//#endif
#ifdef TPD_PROC_DEBUG
	if(gsl_proc_flag == 1){
		return;
	}
#endif

	tpd_halt = 1;
	if(gsl_gesture_flag == 1){
		u8 buf[4];

		gsl_lcd_flag = 0;
//		gsl_gesture_c_old = 0;
		msleep(200);
		buf[0] = 0xa;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		i2c_smbus_write_i2c_block_data(i2c_client,0xf0,4,buf);
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0x1;
		buf[3] = 0x5a;
		i2c_smbus_write_i2c_block_data(i2c_client,0x8,4,buf);
		msleep(50);
		return ;
	}
	
	disable_irq(TOUCH_IRQ);
#ifdef GSL_MONITOR
	pr_debug( "gsl_ts_suspend () : cancel gsl_monitor_work\n");
	cancel_delayed_work_sync(&gsl_monitor_work);
#endif

	tpd_gpio_output(GTP_RST_PORT, 0);

}

/* Function to manage power-on resume */
void tpd_resume(struct device *h)
{
	pr_debug("[TouchScreen] %s() start. \n", __func__);
	//#ifdef TPD_PROXIMITY
	if(item_integer("sensor.tp.proximity",0)==1){
		if ((tpd_proximity_flag == 1) && (tpd_halt == 0)){
				pr_debug("%s: tpd_enable_ps\n", __func__);
	        tpd_enable_ps(1);
	        return;
	    }
	}
	//#endif
#ifdef TPD_PROC_DEBUG
    if(gsl_proc_flag == 1){
        return;
    }
#endif


	pr_debug("%s: gsl_gesture_flag=%d\n", __func__, gsl_gesture_flag);
	if(gsl_gesture_flag == 1){
		u8 buf[4];
//		unsigned int temp;
		tpd_gpio_output(GTP_RST_PORT, 1);
		msleep(20);
		buf[0] = 0xa;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;
		i2c_smbus_write_i2c_block_data(i2c_client,0xf0,4,buf);
		buf[0] = 0;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0x5a;
		i2c_smbus_write_i2c_block_data(i2c_client,0x8,4,buf);
		msleep(5);		
	}else{

	tpd_gpio_output(GTP_RST_PORT, 1);
	}
	msleep(30);		
	{
		reset_chip(i2c_client);
		startup_chip(i2c_client);
		check_mem_data(i2c_client);	
	}
  
	//#ifdef TPD_PROXIMITY
	if(item_integer("sensor.tp.proximity",0)==1)
	{
		if ((tpd_proximity_flag == 1) && (tpd_halt == 1))
	    {
	        tpd_enable_ps(1);
	    }
	}
	//#endif
#ifdef GSL_MONITOR
	pr_debug( "gsl_ts_resume () : queue gsl_monitor_work\n");
	queue_delayed_work(gsl_monitor_workqueue, &gsl_monitor_work, 300);
#endif	
	//mt_eint_unmask(CUST_EINT_TOUCH_PANEL_NUM);
	enable_irq(TOUCH_IRQ);
	tpd_halt = 0;
}

static struct tpd_driver_t tpd_device_driver = {
	.tpd_device_name = GSLX680_NAME,
	.tpd_local_init = tpd_local_init,
	.suspend = tpd_suspend,
	.resume = tpd_resume,
#ifdef TPD_HAVE_BUTTON
	.tpd_have_button = 1,
#else
	.tpd_have_button = 0,
#endif
};


/* called when loaded into kernel */
static int __init tpd_driver_init(void) {

    int i2c_num = 0;
    tpd_get_dts_info();
		
	printk("[TouchScreen] Start Sileadinc gslX680 touch panel driver init \n");
	if((item_exist("touchpanel.ic")) &&(!item_equal("touchpanel.ic", "gslx680", 0)) ){
		printk("other touchpanel.ic choose , not load GT9XX driver \n");
		return -1;
	}	
	if(item_exist("touchpanel.i2c.num"))
		i2c_num = item_integer("touchpanel.i2c.num",0);
	else
		i2c_num = 1;

    if(item_equal("touchpanel.gsl.ic", "GSL1686", 0) || item_equal("touchpanel.gsl.ic", "GSL1686F", 0) || item_equal("touchpanel.gsl.ic", "GSL3675", 0) || item_equal("touchpanel.gsl.ic", "GSL3676", 0))
	  touchpanel_gsl_ic = 0;	
    else if(item_equal("touchpanel.gsl.ic", "GSL915", 0) || item_equal("touchpanel.gsl.ic", "GSL3670", 0))
	  touchpanel_gsl_ic = 1;
	  
#ifdef ADD_I2C_DEVICE_ANDROID_4_0
	i2c_register_board_info(i2c_num, &gslX680_i2c_tpd, 1);	
#endif
	if(tpd_driver_add(&tpd_device_driver) < 0)
		printk("add gslX680 driver failed\n");
//================ sysfs interface init ========================
	mid_tp_kobj = kobject_create_and_add("mid", NULL);

	if (!mid_tp_kobj) {
		printk("kobject_create_and_add: mid fail\n");
	}
//=============================================================
//#ifdef TPD_PROXIMITY
if(item_integer("sensor.tp.proximity",0)==1)
	tp_alsps_init();//by lai
//#endif
	return 0;
}

/* should never be called */
static void __exit tpd_driver_exit(void) {
	printk("Sileadinc gslX680 touch panel driver exit\n");
	//input_unregister_device(tpd->dev);
	tpd_driver_remove(&tpd_device_driver);
//#ifdef TPD_PROXIMITY
if(item_integer("sensor.tp.proximity",0)==1)
	tp_alsps_exit();//by lai
//#endif
}

module_init(tpd_driver_init);
module_exit(tpd_driver_exit);



