#include $(srctree)/drivers/misc/mediatek/Makefile.custom
ifeq ($(ARCH_MTK_PROJECT), u705a_langmei)
obj-y += s5k4ecgxyuv_Sensor.o
else
	ifeq ($(ARCH_MTK_PROJECT), u706_mengshidai)
		obj-y += s5k4ecgxyuv_Sensor_mengshidai.o
	else
    ifeq ($(ARCH_MTK_PROJECT), u705fc_jst)
    	obj-y += s5k4ecgxyuv_Sensor_jst.o
    	else
    		ifeq ($(ARCH_MTK_PROJECT), u706a_bsw_a033)
    			obj-y += s5k4ecgxyuv_Sensor_baishiwei.o
    		else
					obj-y +=  s5k4ecgxyuv_Sensor.o
				endif	
			endif
	endif
endif

