#ifndef GT9XX_LIST_H
#define GT9XX_LIST_H

typedef unsigned char u8;
#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))

#include "gt9xx_mid.h"
#include "LF705_9110P.h"
#include "LF810_9110P.h"
#include "LF810_9110P_250K.h"

struct mid_cfg_data project_cfg_data[] = {
	{"LF705_9110P", &LF705_9110P_CFG},
	{"LF810_9110P", &LF810_9110P_CFG},
	{"LF810_9110P_250K", &LF810_9110P_250K_CFG},
};

#endif