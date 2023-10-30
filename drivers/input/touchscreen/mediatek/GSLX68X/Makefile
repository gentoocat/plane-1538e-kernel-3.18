# Linux driver folder
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GSLX68X/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GSLX68X/include/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/GSLX68X/mid/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include/
obj-y	+=  gslx680.o
obj-y += gslx680_config.o


obj-y += gsl_point_id.o
ifeq ($(CONFIG_ARM64), y)
$(obj)/gsl_point_id.o: $(srctree)/$(obj)/gsl_point_id_64
	cp $(srctree)/$(obj)/gsl_point_id_64 $(obj)/gsl_point_id.o
else
$(obj)/gsl_point_id.o: $(srctree)/$(obj)/gsl_point_id_32
	cp $(srctree)/$(obj)/gsl_point_id_32 $(obj)/gsl_point_id.o
endif