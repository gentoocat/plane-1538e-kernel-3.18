#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

#
# Makefile for misc devices that really don't fit anywhere else.
#



define FILTER_PROJECT_DRV
ifeq ($(wildcard $(srctree)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/$(1)),)
platform_drv += $(1)
else
project_drv += camera_project/$(ARCH_MTK_PROJECT)/$(1)
endif
endef

MTK_PLATFORM := $(subst ",,$(CONFIG_MTK_PLATFORM))
ARCH_MTK_PROJECT := $(subst ",,$(CONFIG_ARCH_MTK_PROJECT))

obj-y				+= kd_sensorlist.o

$(info -= project path  $(srctree)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/ =-)

ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/imgsensor/src/$(MTK_PLATFORM)/camera_project/$(ARCH_MTK_PROJECT)/camera_hw),)
obj-y				+= camera_project/$(ARCH_MTK_PROJECT)/camera_hw/
$(info by porject kd_camera_hw)
else
obj-y                           += camera_hw/
endif
###################################  mid modify start #########################################################
CONFIG_CUSTOM_KERNEL_IMGSENSOR := $(shell ls $(srctree)/drivers/misc/mediatek/imgsensor/src/mt6735/mid/)
$(info ****************MID_IMGSENSOR: $(CONFIG_CUSTOM_KERNEL_IMGSENSOR) ********************)
#################################### mid modify end ########################################################
$(foreach drv,$(subst $\",,$(CONFIG_CUSTOM_KERNEL_IMGSENSOR)),$(eval $(call FILTER_PROJECT_DRV,$(drv))))

custom_kernel_imgsensor := $(addsuffix /, $(shell echo $(platform_drv)))
custom_kernel_imgsensor := $(addprefix mid/, $(shell echo $(custom_kernel_imgsensor)))
$(info custom_kernel_imgsensor: $(custom_kernel_imgsensor))
obj-y += $(custom_kernel_imgsensor)


$(info imgsensor drv by platform $(platform_drv))
$(info imgsensor drv by project $(project_drv))
