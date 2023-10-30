#ifndef GT9XX_MID_H
#define GT9XX_MID_H

//#include "tpd_gt9xx_common.h"
/* 一组GTP驱动有6个ID, 可同时配置6文件Touchpanel */
struct ctp_cfg {
	unsigned char  lens[6];
	unsigned char  *info[6];
};

/* 对应一个具体项目 */
struct mid_cfg_data {
	char name[32];
	struct ctp_cfg  *cfg;
};

#endif