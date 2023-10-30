ccflags-y += -I$(srctree)/drivers/misc/mediatek/alsps/inc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(MTK_PLATFORM)/include/mach/

obj-y	+=  epl89xx.o
obj-y	+=  epl89xx_gesture.o

$(obj)/epl89xx_gesture.o: $(srctree)/$(obj)/epl89xx_gesture.a
	cp $(srctree)/$(obj)/epl89xx_gesture.a $(obj)/epl89xx_gesture.o
