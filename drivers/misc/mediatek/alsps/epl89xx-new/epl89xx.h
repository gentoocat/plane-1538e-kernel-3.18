#ifndef __ELAN_EPL89xx__
#define __ELAN_EPL89xx__

#define ELAN_IOCTL_MAGIC 'c'
#define ELAN_EPL8800_IOCTL_GET_PFLAG _IOR(ELAN_IOCTL_MAGIC, 1, int *)
#define ELAN_EPL8800_IOCTL_GET_LFLAG _IOR(ELAN_IOCTL_MAGIC, 2, int *)
#define ELAN_EPL8800_IOCTL_ENABLE_PFLAG _IOW(ELAN_IOCTL_MAGIC, 3, int *)
#define ELAN_EPL8800_IOCTL_ENABLE_LFLAG _IOW(ELAN_IOCTL_MAGIC, 4, int *)
#define ELAN_EPL8800_IOCTL_GETDATA _IOR(ELAN_IOCTL_MAGIC, 5, int *)

#define MODE_IDLE			(0)
#define MODE_ALS			(1)
#define MODE_PS				(2)
#define MODE_PS_ALS		(5)
#define MODE_GES 			(9)
#define MODE_GES_ALS		(12)

#define EPL_MODE_IDLE		(0x00)
#define EPL_MODE_ALS		(0x01)
#define EPL_MODE_PS			(0x02)
#define EPL_MODE_COLOR		(0x03)
#define EPL_MODE_ALS_PS		(0x05)
#define EPL_MODE_PS_COLOR	        (0x06)
#define EPL_MODE_GES		        (0x08)
#define EPL_MODE_GES_GL		(0x09)
#define EPL_MODE_GES_SS		(0x0a)
#define EPL_MODE_GES_ALS	        (0x0b)
#define EPL_MODE_GES_GL_ALS	(0x0c)
#define EPL_MODE_GES_SS_ALS	(0x0d)

#define POWER_DOWN		(1)
#define POWER_WAKE			(0)

#define EPL_INDEX_1			(0<<5)
#define EPL_INDEX_2			(1<<5)
#define EPL_INDEX_4			(2<<5)
#define EPL_INDEX_8			(3<<5)

#define GES_INT_DISABLE		(0<<7)
#define GES_INT_ENABLE		(1<<7)


#define EPL_GES_INTT_32			(2<<1)
#define EPL_GES_INTT_40			(3<<1)
#define EPL_GES_INTT_60			(4<<1)
#define EPL_GES_INTT_80			(5<<1)
#define EPL_GES_INTT_120		(6<<1)
#define EPL_GES_INTT_160		(7<<1)
#define EPL_GES_INTT_200		(8<<1)
#define EPL_GES_INTT_240		(9<<1)
#define EPL_GES_INTT_280		(10<<1)
#define EPL_GES_INTT_360		(11<<1)
#define EPL_GES_INTT_440		(12<<1)
#define EPL_GES_INTT_520		(13<<1)
#define EPL_GES_INTT_600		(14<<1)
#define EPL_GES_INTT_700		(15<<1)
#define EPL_GES_INTT_800		(16<<1)

#define EPL_ALS_INTT_2			(0<<1)
#define EPL_ALS_INTT_3			(1<<1)
#define EPL_ALS_INTT_4			(2<<1)
#define EPL_ALS_INTT_6			(3<<1)
#define EPL_ALS_INTT_8			(4<<1)
#define EPL_ALS_INTT_10			(5<<1)
#define EPL_ALS_INTT_15			(6<<1)
#define EPL_ALS_INTT_20			(7<<1)
#define EPL_ALS_INTT_35			(8<<1)
#define EPL_ALS_INTT_50			(9<<1)
#define EPL_ALS_INTT_70			(10<<1)
#define EPL_ALS_INTT_100		(11<<1)
#define EPL_ALS_INTT_150		(12<<1)
#define EPL_ALS_INTT_250		(13<<1)
#define EPL_ALS_INTT_350		(14<<1)
#define EPL_ALS_INTT_500		(15<<1)
#define EPL_ALS_INTT_750		(16<<1)
#define EPL_ALS_INTT_1000		(17<<1)
#define EPL_ALS_INTT_2000		(18<<1)
#define EPL_ALS_INTT_3000		(19<<1)
#define EPL_ALS_INTT_4000		(20<<1)
#define EPL_ALS_INTT_6000		(21<<1)
#define EPL_ALS_INTT_8000		(22<<1)
#define EPL_ALS_INTT_12000		(23<<1)
#define EPL_ALS_INTT_16000		(24<<1)
static int als_intt_value[] = {2, 3, 4, 6, 8, 10, 15, 20, 35, 50, 70, 100, 150, 250, 350, 500, 750, 1000, 2000, 3000, 4000, 6000, 8000, 12000, 16000};

#define EPL_PS_INTT_2			(0<<1)
#define EPL_PS_INTT_3			(1<<1)
#define EPL_PS_INTT_4			(2<<1)
#define EPL_PS_INTT_5			(3<<1)
#define EPL_PS_INTT_8			(4<<1)
#define EPL_PS_INTT_10			(5<<1)
#define EPL_PS_INTT_13			(6<<1)
#define EPL_PS_INTT_15			(7<<1)
#define EPL_PS_INTT_20			(8<<1)
#define EPL_PS_INTT_28			(9<<1)
#define EPL_PS_INTT_35			(10<<1)
#define EPL_PS_INTT_45			(11<<1)
#define EPL_PS_INTT_55			(12<<1)
#define EPL_PS_INTT_75			(13<<1)
#define EPL_PS_INTT_100			(14<<1)
#define EPL_PS_INTT_125			(15<<1)
#define EPL_PS_INTT_175			(16<<1)
#define EPL_PS_INTT_225			(17<<1)
#define EPL_PS_INTT_275			(18<<1)
#define EPL_PS_INTT_375			(19<<1)
#define EPL_PS_INTT_475			(20<<1)
#define EPL_PS_INTT_625			(21<<1)
#define EPL_PS_INTT_800			(22<<1)
#define EPL_PS_INTT_1000		(23<<1)
#define EPL_PS_INTT_1350		(24<<1)
#define EPL_PS_INTT_1750		(25<<1)
#define EPL_PS_INTT_2250		(26<<1)
#define EPL_PS_INTT_3000		(27<<1)
#define EPL_PS_INTT_4000		(28<<1)
#define EPL_PS_INTT_5000		(29<<1)
#define EPL_PS_INTT_7500		(30<<1)

static int ps_intt_value[] = {2,3,4,5,8,10,13,15,20,28,
                              35,45,55,75,100,125,175,225,275,375,
                              475,625,800,1000,1350,1750,2250,3000,4000,5000,
                              7500};

#define EPL_WAIT_0_MS			(0<<4)
#define EPL_WAIT_2_MS			(1<<4)
#define EPL_WAIT_4_MS			(2<<4)
#define EPL_WAIT_8_MS			(3<<4)
#define EPL_WAIT_12_MS			(4<<4)
#define EPL_WAIT_20_MS			(5<<4)
#define EPL_WAIT_30_MS			(6<<4)
#define EPL_WAIT_40_MS			(7<<4)
#define EPL_WAIT_50_MS			(8<<4)
#define EPL_WAIT_75_MS			(9<<4)
#define EPL_WAIT_100_MS		(10<<4)
#define EPL_WAIT_150_MS		    (11<<4)
#define EPL_WAIT_200_MS		    (12<<4)
#define EPL_WAIT_300_MS		    (13<<4)
#define EPL_WAIT_400_MS		    (14<<4)
#define EPL_WAIT_SINGLE		    (15<<4)

static int wait_value[] = {0, 2, 4, 8, 12, 20, 30, 40, 50, 75, 100, 150, 200, 300, 400};
int epl89xx_wait_len = sizeof(wait_value)/sizeof(int);

#define EPL_GAIN_MID		(0x00)
#define EPL_GAIN_LOW		(0x01)

#define EPL_PSALS_ADC_11	(0x00 << 3)
#define EPL_PSALS_ADC_12	(0x01 << 3)
#define EPL_PSALS_ADC_13	(0x02 << 3)
#define EPL_PSALS_ADC_14	(0x03 << 3)
static int adc_value[] = {128, 256, 512, 1024};

#define EPL_GES_ADC_11		(0x00 << 6)
#define EPL_GES_ADC_12		(0x01 << 6)
#define EPL_GES_ADC_13		(0x02 << 6)
#define EPL_GES_ADC_14		(0x03 << 6)


#define EPL_CYCLE_1			(0x00)
#define EPL_CYCLE_2			(0x01)
#define EPL_CYCLE_4			(0x02)
#define EPL_CYCLE_8			(0x03)
#define EPL_CYCLE_16		(0x04)
#define EPL_CYCLE_32		(0x05)
#define EPL_CYCLE_64		(0x06)
static int cycle_value[] = {1, 2, 4, 8, 16, 32, 64};

#define EPL_IR_ON_CTRL_OFF  	(0x00 << 5)
#define EPL_IR_ON_CTRL_ON	       (0x01 << 5)

#define EPL_IR_MODE_CURRENT	(0x00 << 4)
#define EPL_IR_MODE_VOLTAGE	(0x01 << 4)

#define EPL_IR_BOOST_100	(0x00 << 2)
#define EPL_IR_BOOST_150	(0x01 << 2)
#define EPL_IR_BOOST_200	(0x02 << 2)
#define EPL_IR_BOOST_300	(0x03 << 2)

#define EPL_IR_DRIVE_100	(0x00)
#define EPL_IR_DRIVE_50		(0x01)
#define EPL_IR_DRIVE_20		(0x02)
#define EPL_IR_DRIVE_10		(0x03)


#define EPL_INT_CTRL_ALS_OR_PS		(0x00 << 4)
#define EPL_INT_CTRL_ALS			(0x01 << 4)
#define EPL_INT_CTRL_PS				(0x02 << 4)

#define EPL_PERIST_1		(0x00 << 2)
#define EPL_PERIST_4		(0x01 << 2)
#define EPL_PERIST_8		(0x02 << 2)
#define EPL_PERIST_16		(0x03 << 2)

#define EPL_INTTY_DISABLE	(0x00)
#define EPL_INTTY_BINARY	(0x01)
#define EPL_INTTY_ACTIVE	(0x02)
#define EPL_INTTY_FRAME	(0x03)

#define EPL_RESETN_RESET	(0x00 << 1)
#define EPL_RESETN_RUN		(0x01 << 1)

#define EPL_POWER_OFF		(0x01)
#define EPL_POWER_ON		(0x00)

#define EPL_ALS_INT_CHSEL_0	(0x00 << 4)
#define EPL_ALS_INT_CHSEL_1	(0x01 << 4)
#define EPL_ALS_INT_CHSEL_2	(0x02 << 4)
#define EPL_ALS_INT_CHSEL_3	(0x03 << 4)

#define EPL_SATURATION			(0x01 << 5)
#define EPL_SATURATION_NOT	(0x01 << 5)

#define EPL_CMP_H_TRIGGER		(0x01 << 4)
#define EPL_CMP_H_CLEAR		(0x00 << 4)

#define EPL_CMP_L_TRIGGER		(0x01 << 3)
#define EPL_CMP_L_CLEAR		(0x00 << 3)

#define EPL_INT_TRIGGER		(0x01 << 2)
#define EPL_INT_CLEAR		(0x00 << 2)

#define EPL_CMP_RESET		(0x00 << 1)
#define EPL_CMP_RUN			(0x01 << 1)

#define EPL_LOCK			        (0x01)
#define EPL_UN_LOCK		        (0x00)

#define EPL_GES_INT_EN 		(0x01 << 7)
#define EPL_GES_INT_DIS 	(0x00 << 7)

#define EPL_REVNO       (0x61)

#define EPL_OSC_SEL_1MHZ	(0 << 3)
#define EPL_OSC_SEL_4MHZ	(1 << 3)

#define EPL_GFIN_DISABLE      (0 << 2)
#define EPL_GFIN_ENABLE       (1 << 2)

#define EPL_VOS_DISABLE      (0 << 1)
#define EPL_VOS_ENABLE       (1 << 1)

#define EPL_DOC_OFF         (0)
#define EPL_DOC_ON          (1)

struct _ps_data
{
    u16 ir_data;
    u16 data;
};

struct _ps_factory
{
    int calibration_enable;
    bool calibrated;
    int cancelation;
	int high_threshold;
	int low_threshold;
};

#define ALS_CHANNEL_SIZE	4
struct _als_ges_data
{
    u16 channels[ALS_CHANNEL_SIZE];
    u16 lux;
};

struct _als_factory
{
    bool calibration_enable;
    bool calibrated;
    int lux_per_count;
};

struct _hs_setting
{
    bool dynamic_intt;
	u8 integration_time;
	u8 integration_time_max;
	u8 integration_time_min;
	u8 gain;
	u8 adc;
	u8 cycle;
    u8 compare_reset;
	u8 ir_on_control;
	u8 ir_mode;
	u8 ir_drive;
	u8 lock;
	u16 low_threshold;
	u16 mid_threshold;
	u16 high_threshold;
	u16 raw;
	u16 raws_count;
	u16 raws[200];
};


#define GES_T_SIZE	8
struct _ges_data
{
    u8 ir_data;
    struct _als_ges_data t[GES_T_SIZE];
};

struct _ges_factory
{
};

struct _ps_setting
{
    bool polling_mode;
    u8 integration_time;
    u8 gain;
    u8 adc;
    u8 cycle;
    u16 high_threshold;
	u16 low_threshold;
    u8 ir_on_control;
    u8 ir_mode;
    u8 ir_boost;
    u8 ir_drive;
    u8 persist;
    u8 interrupt_type;
    u8 saturation;
    u8 compare_high;
    u8 compare_low;
    u8 interrupt_flag;
    u8 compare_reset;
    u8 lock;
    int cancelation;
    struct _ps_data data;
    struct _ps_factory factory;
};

struct _als_setting
{
    bool polling_mode;
    u8 report_type;
	u8 lsrc_type;
    u16 report_count;
    u8 integration_time;
    u8 gain;
    u8 adc;
    u8 cycle;
    u16 high_threshold;
    u16 low_threshold;
    u8 ir_on_control;
    u8 ir_mode;
    u8 ir_boost;
    u8 ir_drive;
    u8 persist;
    u8 interrupt_type;
    u8 saturation;
    u8 compare_high;
    u8 compare_low;
    u8 interrupt_flag;
    u8 compare_reset;
    u8 lock;
    u8 interrupt_channel_select;
    u16 dyn_intt_raw;
    struct _als_ges_data data;
    struct _als_factory factory;
};

struct _ges_setting
{
    bool polling_mode;
    u8 integration_time;
    u8 wait;
	u8 wait_sleep;
    u8 gain;
    u8 adc;
    u8 interrupt_enable;
    u8 interrupt_index;
    u8 ir_mode;
    u8 ir_boost;
    u8 ir_drive;
    struct _ges_data data;
    struct _ges_factory factory;
};

typedef struct _sensor
{
    u8 wait;
    u8 mode;
    bool enable_factory_calibration;
    u8 early_suspend_mode;
    u8 osc_sel;
    u8 interrupt_control;
    u8 reset;
    u8 power;
    struct _hs_setting hs;
    struct _ps_setting ps;
    struct _als_setting als;
    struct _ges_setting ges;
    u16 revno;
} epl_optical_sensor;
#endif


