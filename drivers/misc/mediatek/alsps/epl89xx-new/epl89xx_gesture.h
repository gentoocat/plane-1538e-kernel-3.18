#ifndef __EGT3295GESTURE__
#define __EGT3295GESTURE__

#define EVENT_AMBIGUOUS			0
#define EVENT_HOLD				1
#define EVENT_RIGHT				2
#define EVENT_LEFT				3
#define EVENT_UP				4
#define EVENT_DOWN				5
#define EVENT_ZOOM_L1			6
#define EVENT_ZOOM_L2			7
#define EVENT_ZOOM_L3			8
#define EVENT_ZOOM_END			9
#define EVENT_UNKNOWN			10

#define CHANNEL_COUNT			4
#define BUF_SIZE				CHANNEL_COUNT*100

#define GES_MAX_INTERNAL_TIME	300	//ms
#define GES_MIN_INTERNAL_TIME	20	//ms
#define GES_STABLE_TIME			20	//ms
#define GES_ZOOM_START_TIME		400	//ms
#define GES_ZOOM_SPACING_TIME	200	//ms
#define GES_HOLD_TIME			700	//ms

#define GES_MODE_DISABLE		0
#define GES_MODE_HORIZONTAL		2
#define GES_MODE_VERTICAL		3
#define GES_MODE_4D				4
#define GES_MODE_4D_HOLD		5
#define GES_MODE_4D_ZOOM		6

#define STATUS_WAITING			0
#define STATUS_LOW				1
#define STATUS_WORK				2
#define STATUS_FINISH 			3
#define STATUS_RESTART			4

#define CROSSTALK_PERIOD		300

#define LIB_VERSION				"1.0.4"

extern int zoom_delta;
extern int sleep_mode;
extern unsigned short crosstalk_offset;
extern unsigned short crosstalk;
extern unsigned short ges_count;
extern unsigned short ges_raws[BUF_SIZE];

/**********************************************************************************
 * public settings for customer
 **********************************************************************************/
extern int SLEEP_PERIOD;

extern unsigned short WORK_TH;	// defalut is 10

extern unsigned short HOLD_TH;	// defalut is 20

extern unsigned short ZOOM_TH;	// defalut is 20
/**********************************************************************************
 * public settings for customer
 **********************************************************************************/
#endif

typedef void (*gesture_sleep_callback)(int);
typedef void (*gesture_event_callback)(int);

void set_gesture_callback(gesture_sleep_callback cb1, gesture_event_callback cb2);
void set_gesture_mode(int gm);
void add_gesture_data(unsigned short *data);
