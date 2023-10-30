
#ifndef _TPD_GSL_COMMON_H__
#define _TPD_GSL_COMMON_H__

#include <linux/kernel.h>



struct fw_data
{
    u32 offset : 8;
    u32 : 0;
    u32 val;
};

struct gsl_touch_info
{
   int x[10];
   int y[10];
   int id[10];
   int finger_num;   
};

extern void gsl_alg_id_main(struct gsl_touch_info *cinfo);
extern void gsl_DataInit(int *ret);
extern unsigned int gsl_version_id(void);
extern unsigned int gsl_mask_tiaoping(void);
extern int gsl_obtain_gesture(void);

#endif
