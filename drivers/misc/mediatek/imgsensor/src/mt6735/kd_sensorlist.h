/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

//s_add new sensor driver here
//export funtions
/**********************************MID_IMGSENSOR_START*********************************/
UINT32 GC5025MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 S5K4ECGX_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);

UINT32 IMX219_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);

UINT32 OV8856_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 OV13850_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 OV5648MIPISensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
/*GC*/
UINT32 GC2385MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2375MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC5005MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2145SUB_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC5024MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC0310MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2155MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2365MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC5004MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2145MIPI_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC0312_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC030AMIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2355MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2145_YUV_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2355MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC8024MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 GC2755MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
/*SP*/
UINT32 SP5506_MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
UINT32 SP2509_MIPI_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);
/*RDA*/
UINT32 RDA2215MIPI_RAW_SensorInit(PSENSOR_FUNCTION_STRUCT *pfFunc);

UINT32 HM5040MIPI_RAW_SensorInit( PSENSOR_FUNCTION_STRUCT *pfFunc);

/**********************************MID_IMGSENSOR_END***********************************/

//! Add Sensor Init function here
//! Note:
//! 1. Add by the resolution from ""large to small"", due to large sensor
//!    will be possible to be main sensor.
//!    This can avoid I2C error during searching sensor.
//! 2. This file should be the same as mediatek\custom\common\hal\imgsensor\src\sensorlist.cpp
ACDK_KD_SENSOR_INIT_FUNCTION_STRUCT kdSensorList[MAX_NUM_OF_SUPPORT_SENSOR+1] =
{
/**********************************MID_IMGSENSOR_START*********************************/
	{HM5040_MIPI_RAW_SENSOR_ID,SENSOR_DRVNAME_HM5040_MIPI_RAW,HM5040MIPI_RAW_SensorInit},
	{GC2385MIPI_SENSOR_ID, SENSOR_DRVNAME_GC2385_MIPI_RAW, GC2385MIPI_RAW_SensorInit},
	{GC5025_SENSOR_ID, SENSOR_DRVNAME_GC5025_MIPI_RAW, GC5025MIPI_RAW_SensorInit},
	{RDA2215MIPI_SENSOR_ID, SENSOR_DRVNAME_RDA2215_MIPI_RAW, RDA2215MIPI_RAW_SensorInit},
	{S5K4ECGX_SENSOR_ID, SENSOR_DRVNAME_S5K4ECGX_YUV, S5K4ECGX_YUV_SensorInit},
	{GC2145_SENSOR_ID,SENSOR_DRVNAME_GC2145_YUV,GC2145_YUV_SensorInit},
	{SP5506_SENSOR_ID, SENSOR_DRVNAME_SP5506_MIPI_RAW, SP5506_MIPI_RAW_SensorInit},
	{SP2509MIPI_SENSOR_ID, SENSOR_DRVNAME_SP2509_MIPI_RAW, SP2509_MIPI_SensorInit},
	{IMX219_SENSOR_ID, SENSOR_DRVNAME_IMX219_MIPI_RAW, IMX219_MIPI_RAW_SensorInit},
    {GC2375MIPI_SENSOR_ID, SENSOR_DRVNAME_GC2375_MIPI_RAW, GC2375MIPI_RAW_SensorInit},
	{OV8856_SENSOR_ID, SENSOR_DRVNAME_OV8856_MIPI_RAW, OV8856_MIPI_RAW_SensorInit},	
    {OV13850_SENSOR_ID, SENSOR_DRVNAME_OV13850_MIPI_RAW,OV13850_MIPI_RAW_SensorInit},
    {OV5648MIPI_SENSOR_ID, SENSOR_DRVNAME_OV5648_MIPI_RAW, OV5648MIPISensorInit},
	{GC5005MIPI_SENSOR_ID, SENSOR_DRVNAME_GC5005MIPI_RAW,GC5005MIPI_RAW_SensorInit},
    {GC8024MIPI_SENSOR_ID, SENSOR_DRVNAME_GC8024MIPI_RAW,GC8024MIPI_RAW_SensorInit},
	{GC5004MIPI_SENSOR_ID, SENSOR_DRVNAME_GC5004MIPI_RAW,GC5004MIPI_RAW_SensorInit},

    {GC5024MIPI_SENSOR_ID, SENSOR_DRVNAME_GC5024MIPI_RAW,GC5024MIPI_RAW_SensorInit},
    {GC2755MIPI_SENSOR_ID,SENSOR_DRVNAME_GC2755MIPI_RAW,GC2755MIPI_RAW_SensorInit},

  {GC2365MIPI_SENSOR_ID,SENSOR_DRVNAME_GC2365MIPI_RAW,GC2365MIPI_RAW_SensorInit},
 
    {GC2355MIPI_SENSOR_ID, SENSOR_DRVNAME_GC2355_MIPI_RAW,GC2355MIPI_RAW_SensorInit},

    {GC2145MIPI_SENSOR_ID, SENSOR_DRVNAME_GC2145_MIPI_YUV, GC2145MIPI_YUV_SensorInit},

    {GC2155MIPI_SENSOR_ID, SENSOR_DRVNAME_GC2155_MIPI_YUV, GC2155MIPI_YUV_SensorInit},

    {GC0310MIPI_SENSOR_ID,SENSOR_DRVNAME_GC0310_MIPI_YUV,GC0310MIPI_YUV_SensorInit},

    {GC030AMIPI_SENSOR_ID,SENSOR_DRVNAME_GC030AMIPI_RAW,GC030AMIPI_RAW_SensorInit}, 
    {GC0312_SENSOR_ID, SENSOR_DRVNAME_GC0312_YUV, GC0312_YUV_SensorInit}, 
/*  ADD sensor driver before this line */
    {0,{0},NULL}, //end of list
};
//e_add new sensor driver here

