//#ifndef __CONFIG_H_
//#define _GT9XX_CONFIG_H_

/* ***************************PART2:TODO define********************************** */
/* STEP_1(REQUIRED):Change config table. */
/*TODO: puts the config info corresponded to your TP here, the following is just
a sample config, send this config should cause the chip cannot work normally*/
u8 LF810_9110P_250K_GROUP0[] = {0x00,0xB0,0x04,0x80,0x07,0x0A,0x75,0x00,0x01,0xC8,0x28,0x08,0x5A,0x3C,0x03,0x04,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x18,0x1A,0x1E,0x14,0x95,0x35,0xFF,0x1F,0x21,0xAC,0x13,0x00,0x00,0x00,0x02,0x04,0x1C,0x16,0x18,0x1A,0x80,0x82,0x34,0x23,0x1E,0x28,0x4A,0x10,0x14,0x32,0x14,0xD5,0x50,0x00,0x14,0x00,0x00,0xA3,0x16,0x00,0x90,0x1A,0x00,0x7F,0x1F,0x00,0x71,0x26,0x00,0x68,0x2D,0x00,0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x25,0x00,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x43,0x01};

/* TODO puts your group2 config info here,if need. */
u8 LF810_9110P_250K_GROUP1[] = {0};


/* TODO puts your group3 config info here,if need. */
u8 LF810_9110P_250K_GROUP2[] = {0 };



/* TODO: define your config for Sensor_ID == 3 here, if needed */
u8 LF810_9110P_250K_GROUP3[] = {0};


/* TODO: define your config for Sensor_ID == 4 here, if needed */
u8 LF810_9110P_250K_GROUP4[] = {0 };



/* TODO: define your config for Sensor_ID == 5 here, if needed */
u8 LF810_9110P_250K_GROUP5[] =  {0};

struct ctp_cfg LF810_9110P_250K_CFG = {
	.lens = {CFG_GROUP_LEN(LF810_9110P_250K_GROUP0), 
			CFG_GROUP_LEN(LF810_9110P_250K_GROUP1),
			CFG_GROUP_LEN(LF810_9110P_250K_GROUP2),
			CFG_GROUP_LEN(LF810_9110P_250K_GROUP3),
			CFG_GROUP_LEN(LF810_9110P_250K_GROUP4),
			CFG_GROUP_LEN(LF810_9110P_250K_GROUP5) },
	.info = {LF810_9110P_250K_GROUP0, 
			LF810_9110P_250K_GROUP1, 
			LF810_9110P_250K_GROUP2, 
			LF810_9110P_250K_GROUP3, 
			LF810_9110P_250K_GROUP4, 
			LF810_9110P_250K_GROUP5}	
};


//#endif /* _GT1X_CONFIG_H_ */