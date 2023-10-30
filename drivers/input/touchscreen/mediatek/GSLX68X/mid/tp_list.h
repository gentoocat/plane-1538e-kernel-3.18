#ifndef _ELINK_TP_LIST_H__
#define _ELINK_TP_LIST_H__

#include <linux/kernel.h>
#include "tpd_gsl_common.h"

//--------------------------------------------
//.h file
#include "TEST_COMPATIBILITY.h"
// L695
#include "L695_DZ_PG.h"
// L696
#include "L696_MGL.h"
#include "L696A_XCL.h"
#include "L696D_DY.h"
#include "L696D_XCL.h"
#include "L696D_YLD.h"
#include "L696_HJD.h"
#include "L696_PB_PG.h"
#include "L696_XCL.h"
#include "L696_YLD_PG.h"
// L708G
#include "L708G_YLD.h"
// U706
#include "U706_HXR_B.h"
#include "U706_HXR.h"
#include "U706_QCY.h"
#include "U706_DFL.h"
#include "U706_XC.h"

// U357
#include "U357_C81_COB.h"
#include "U357_QTW_C81_COB_G_G.h"
#include "U357_DFL_C81_COB_P_G.h"
#include "U357_FC_C81_COB_P_G.h"
#include "U357_XCL_C81_COB.h"
#include "U357_XCL_C81_COB_NEW.h"
//LF705
#include "LF705_GETU.h"
#include "LF705_YLD_8.h"
#include "LF705_MINGRUI.h"
#include "LF705_MINRUI.h"
#include "LF705_HT.h"
#include "LF705_DFL.h"
#include "LF705_SHANGQIN.h"
#include "LF705_SHANGQIN8.h"
#include "lf705_fx_7.h"
#include "LF705_LH_8.h"
#include "LF705_HUINING.h"
#include "LF705_DFL_8.h"
#include "LF705_DH_WXGA_7.h"
#include "LF705_MGL_WXGA_7.h"
#include "LF705_MR_WUXGA_8.h"
#include "LF705_DFL_WUXGA_8.h"
// U101
#include "U101_C81_COB.h"
#include "U101_HT_WXGA.h"
#include "U101_DC_GETU_101_WSVGA.h"
#include "U101_DY.h"  
#include "U101_DH.h"	
#include "U101_WGJ.h"
#include "U101_WT.h"	
#include "U101_YTG.h"
#include "U101_ZHZY.h"
#include "U101_WJ.h"
#include "U101_RP.h"
#include "U101_SQ.h"

#include "U101_FC_FC101S217.h"
// L710
#include "L710_C81_COB.h"
#include "L710_YLD_101_WSVGANL.h"
#include "L710_XC_101_WSVGANL.h"
//LF710
#include "LF710_DH_101_WXGANL.h"
#include "LF710_DH_101_WUXGANL.h"
#include "LF710_MR.h"
#include "LF710_MJK.h"
#include "LF710_SQ_101_WXGANL.h"
#include "LF710_YD.h"
#include "LF710_SX_101_WXGA.h"
#include "LF710_MR_101_WXGANL.h"
#include "LF705_SX_WXGA_7.h"
#include "LF710_HT_WXGA_101.h"
#include "LF102_DPT_WUXGA.h"
#include "LF710_xld_wxga_101.h"
//L805B_F
#include "L805B_DIXIAN_8_WXGA.h"

//LF810
#include "LF810_PB.h"
#include "LF810_PB_WUXGA.h"

//LS103I
#include "LS103I_MGL_101_WSVGANL.h"
//--------------------------------------------
//LF808
#include "LF808_DZ_OGS_WUXGA.h"
#include "LF808_DFL_PG.h"
#include "LF808_DZ.h"
#include "LF808_RSS.h"
#include "LF808_PB_101.h"
#include "LF808_DFL_101.h"
#include "LF808_DX_101.h"
#include "LF808_DFL_101_WXGA.h"
#include "LF808_QTW_101.h"
#include "LF808_DZ_COB.h"
#include "LF808_DFL_PG_101.h"
#include "LF808_DFL_8.h"
#include "LF808_DZ_COF_101.h"
//--------------------------------------------
//tp key
#ifdef TPD_HAVE_BUTTON 
static int tpd_keys_local[TPD_KEY_COUNT] = TPD_KEYS;
static int tpd_keys_local_home[1] = {KEY_HOMEPAGE};

#endif
//--------------------------------------------

//--------------------------------------------
//tp_modle
typedef enum {
   l696_tp_gsl_modle,
   l695_tp_gsl_modle,
   l708g_tp_gsl_modle,
   u706_tp_gsl_modle,
   u357_tp_gsl_modle,
   lf705_tp_gsl_modle,   
   u101_tp_gsl_modle,   
   l710_tp_gsl_modle,
   lf710_tp_gsl_modle,  
   lf102_tp_gsl_modle,
   l805b_tp_gsl_modle, 
   ls103i_tp_gsl_modle, 
   lf808_tp_gsl_modle,
   lf810_tp_gsl_modle,
   //default
   default_tp_gsl_modle
}NUM_GSL_MODLE;
//--------------------------------------------

TP_PROJECT_DATA l696_tp_data[]={	
    {"L696_MGL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696_MGL, GSL1688_FW_L696_MGL, NULL, ARRAY_SIZE(GSL1688_FW_L696_MGL)},	
    {"L696A_XCL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696A_XCL, GSL1688_FW_L696A_XCL, NULL, ARRAY_SIZE(GSL1688_FW_L696A_XCL)},
    {"L696D_DY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696D_DY, GSL1688_FW_L696D_DY, NULL, ARRAY_SIZE(GSL1688_FW_L696D_DY)},
    {"L696D_XCL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696D_XCL, GSL1688_FW_L696D_XCL, NULL, ARRAY_SIZE(GSL1688_FW_L696D_XCL)},
    {"L696D_YLD", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696D_YLD, GSL1688_FW_L696D_YLD, NULL, ARRAY_SIZE(GSL1688_FW_L696D_YLD)},
    {"L696_HJD", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696_HJD, GSL1688_FW_L696_HJD, NULL, ARRAY_SIZE(GSL1688_FW_L696_HJD)},
    {"L696_XCL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L696_XCL, GSL1688_FW_L696_XCL, NULL, ARRAY_SIZE(GSL1688_FW_L696_XCL)},
   {"L696_PB_PG", {0xff,0xc0,0x0c,0x00}, gsl_config_data_id_L696_PB_PG, GSL1688_FW_L696_PB_PG, NULL, ARRAY_SIZE(GSL1688_FW_L696_PB_PG)},
   {"L696_YLD_PG", {0xff,0xc0,0x00,0x00}, gsl_config_data_id_L696_YLD_PG, GSL1688_FW_L696_YLD_PG, NULL, ARRAY_SIZE(GSL1688_FW_L696_YLD_PG)},
   {"L696_YLD_PG", {0xff,0xc0,0x00,0x00}, gsl_config_data_id_L696_YLD_PG, GSL1688_FW_L696_YLD_PG, NULL, ARRAY_SIZE(GSL1688_FW_L696_YLD_PG)},
};

TP_PROJECT_DATA l695_tp_data[] = {
   {"L695_DZ_PG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L695_DZ_PG, GSL1688_FW_L695_DZ_PG, NULL, ARRAY_SIZE(GSL1688_FW_L695_DZ_PG)},   
};

TP_PROJECT_DATA l708g_tp_data[] = {
   {"L708G_YLD", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L708G_YLD, GSL1688_FW_L708G_YLD, NULL, ARRAY_SIZE(GSL1688_FW_L708G_YLD)},   
};

TP_PROJECT_DATA u706_tp_data[] = {
   {"U706_HXR_B", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_HXR, GSL1688_FW_U706_HXR, NULL, ARRAY_SIZE(GSL1688_FW_U706_HXR)},
   {"U706_HXR", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_HXR, GSL1688_FW_U706_HXR, NULL, ARRAY_SIZE(GSL1688_FW_U706_HXR)},
   {"U706_QCY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_QCY, GSL1688_FW_U706_QCY, NULL, ARRAY_SIZE(GSL1688_FW_U706_QCY)},
   {"U706_DFL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_DFL, GSL1688_FW_U706_DFL, NULL, ARRAY_SIZE(GSL1688_FW_U706_DFL)},
   {"U706_XC", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_XC, GSL1688_FW_U706_XC, NULL, ARRAY_SIZE(GSL1688_FW_U706_DFL)},
//   {"U706_FC", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U706_FC, GSL1688_FW_U706_FC, NULL, ARRAY_SIZE(GSL1688_FW_U706_FC)},
};

TP_PROJECT_DATA u357_tp_data[] = {
   {"U357_C81_COB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_C81_COB, GSL1688_FW_U357_C81_COB, NULL, ARRAY_SIZE(GSL1688_FW_U357_C81_COB)},
   {"U357_QTW_C81_COB_G_G", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_QTW_C81_COB_G_G, GSL1688_FW_U357_QTW_C81_COB_G_G, NULL, ARRAY_SIZE(GSL1688_FW_U357_QTW_C81_COB_G_G)},
   {"U357_DFL_C81_COB_P_G", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_DFL_C81_COB_P_G, GSL1688_FW_U357_DFL_C81_COB_P_G, NULL, ARRAY_SIZE(GSL1688_FW_U357_DFL_C81_COB_P_G)},
   {"U357_FC_C81_COB_P_G", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_FC_C81_COB_P_G, GSL1688_FW_U357_FC_C81_COB_P_G, NULL, ARRAY_SIZE(GSL1688_FW_U357_FC_C81_COB_P_G)},
   {"U357_XCL_C81_COB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_XCL_C81_COB, GSL1688_FW_U357_XCL_C81_COB, NULL, ARRAY_SIZE(GSL1688_FW_U357_XCL_C81_COB)},
   {"U357_XCL_C81_COB_NEW", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U357_XCL_C81_COB_NEW, GSL1688_FW_U357_XCL_C81_COB_NEW, NULL, ARRAY_SIZE(GSL1688_FW_U357_XCL_C81_COB_NEW)},
};
TP_PROJECT_DATA lf102_tp_data[] = {
   {"LF102_DPT_WUXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF102_DPT_WUXGA, GSLX680_FW_LF102_DPT_WUXGA, NULL, ARRAY_SIZE(GSLX680_FW_LF102_DPT_WUXGA)},

};

TP_PROJECT_DATA lf810_tp_data[] = {
	 {"LF810_PB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_lf810_pb, GSLX680_FW_lf810_pb, NULL, ARRAY_SIZE(GSLX680_FW_lf810_pb)},
	 {"LF810_PB_WUXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF810_PB_WUXGA, GSLX680_FW_LF810_PB_WUXGA, NULL, ARRAY_SIZE(GSLX680_FW_LF810_PB_WUXGA)},
};
TP_PROJECT_DATA lf705_tp_data[] = {
   {"LF705_GETU", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_GETU, GSL1688_FW_LF705_GETU, NULL, ARRAY_SIZE(GSL1688_FW_LF705_GETU)},
   {"LF705_YLD_8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_YLD_8, GSL1688_FW_LF705_YLD_8, NULL, ARRAY_SIZE(GSL1688_FW_LF705_YLD_8)},
   {"LF705_MINGRUI", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_MINGRUI, GSL1688_FW_LF705_MINGRUI, NULL, ARRAY_SIZE(GSL1688_FW_LF705_MINGRUI)},
   {"LF705_MINRUI", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_MINRUI, GSL1688_FW_LF705_MINRUI, NULL, ARRAY_SIZE(GSL1688_FW_LF705_MINRUI)},
   {"LF705_MR_WUXGA_8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_MR_WUXGA_8, GSL1688_FW_LF705_MR_WUXGA_8, NULL, ARRAY_SIZE(GSL1688_FW_LF705_MR_WUXGA_8)},
   {"LF705_HT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_HT, GSL1688_FW_LF705_HT, NULL, ARRAY_SIZE(GSL1688_FW_LF705_HT)},
   {"LF705_DFL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_DFL, GSL1688_FW_LF705_DFL, NULL, ARRAY_SIZE(GSL1688_FW_LF705_DFL)},
   {"LF705_SHANGQIN", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_SHANGQIN, GSL1688_FW_LF705_SHANGQIN, NULL, ARRAY_SIZE(GSL1688_FW_LF705_SHANGQIN)},
   {"LF705_SHANGQIN8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_SHANGQIN8, GSL1688_FW_LF705_SHANGQIN8, NULL, ARRAY_SIZE(GSL1688_FW_LF705_SHANGQIN8)},
   {"LF705_FX_7", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_FX_7, GSL1688_FW_LF705_FX_7, NULL, ARRAY_SIZE(GSL1688_FW_LF705_FX_7)},
   {"LF705_DH_WXGA_7", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_DH_WXGA_7, GSL1688_FW_LF705_DH_WXGA_7, NULL, ARRAY_SIZE(GSL1688_FW_LF705_DH_WXGA_7)},
   {"LF705_HUINING", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_HUINING, GSL1688_FW_LF705_HUINING, NULL, ARRAY_SIZE(GSL1688_FW_LF705_HUINING)},
   {"LF705_DFL_8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_DFL_8, GSL1688_FW_LF705_DFL_8, NULL, ARRAY_SIZE(GSL1688_FW_LF705_DFL_8)},
	{"LF705_SX_WXGA_7", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_SX_WXGA_7, GSL1688_FW_LF705_SX_WXGA_7, NULL, ARRAY_SIZE(GSL1688_FW_LF705_SX_WXGA_7)},
	{"LF705_MGL_WXGA_7", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_MGL_WXGA_7, GSL1688_FW_LF705_MGL_WXGA_7, NULL, ARRAY_SIZE(GSL1688_FW_LF705_MGL_WXGA_7)},
	{"LF705_DFL_WUXGA_8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF705_DFL_WUXGA_8, GSL1688_FW_LF705_DFL_WUXGA_8, NULL, ARRAY_SIZE(GSL1688_FW_LF705_DFL_WUXGA_8)},

	};

TP_PROJECT_DATA u101_tp_data[] = {
   {"U101_C81_COB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_C81_COB, GSL1688_FW_U101_C81_COB, NULL, ARRAY_SIZE(GSL1688_FW_U101_C81_COB)},
   {"U101_HT_WXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_HT_WXGA, GSL1688_FW_U101_HT_WXGA, NULL, ARRAY_SIZE(GSL1688_FW_U101_HT_WXGA)},
   {"U101_DC_GETU_101_WSVGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_DC_GETU_101_WSVGA, GSL1688_FW_U101_DC_GETU_101_WSVGA, NULL, ARRAY_SIZE(GSL1688_FW_U101_DC_GETU_101_WSVGA)},
   {"U101_DY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_DY, GSL1688_FW_U101_DY, NULL, ARRAY_SIZE(GSL1688_FW_U101_DY)},
   {"U101_DH", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_DH, GSL1688_FW_U101_DH, NULL, ARRAY_SIZE(GSL1688_FW_U101_DH)},
   {"U101_WGJ", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_WGJ, GSL1688_FW_U101_WGJ, NULL, ARRAY_SIZE(GSL1688_FW_U101_WGJ)},
   {"U101_WT", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_WT, GSL1688_FW_U101_WT, NULL, ARRAY_SIZE(GSL1688_FW_U101_WT)},
   {"U101_YTG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_YTG, GSL1688_FW_U101_YTG, NULL, ARRAY_SIZE(GSL1688_FW_U101_YTG)},
   {"U101_ZHZY", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_ZHZY, GSL1688_FW_U101_ZHZY, NULL, ARRAY_SIZE(GSL1688_FW_U101_ZHZY)},
   {"U101_WJ", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_WJ, GSL1688_FW_U101_WJ, NULL, ARRAY_SIZE(GSL1688_FW_U101_WJ)},
   {"U101_RP", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_RP, GSL1688_FW_U101_RP, NULL, ARRAY_SIZE(GSL1688_FW_U101_RP)},
   {"U101_SQ", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_SQ, GSL1688_FW_U101_SQ, NULL, ARRAY_SIZE(GSL1688_FW_U101_SQ)},
   {"U101_FC_FC101S217", {0xff,0xff,0xff,0xff}, gsl_config_data_id_U101_FC_FC101S217, GSL1688_FW_U101_FC_FC101S217, NULL, ARRAY_SIZE(GSL1688_FW_U101_FC_FC101S217)},

};

TP_PROJECT_DATA l710_tp_data[] = {
   {"L710_C81_COB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L710_C81_COB, GSL1688_FW_L710_C81_COB, NULL, ARRAY_SIZE(GSL1688_FW_L710_C81_COB)},
   {"L710_YLD_101_WSVGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L710_YLD_101_WSVGANL, GSL1688_FW_L710_YLD_101_WSVGANL, NULL, ARRAY_SIZE(GSL1688_FW_L710_YLD_101_WSVGANL)},
   {"L710_XC_101_WSVGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L710_XC_101_WSVGANL, GSL1688_FW_L710_XC_101_WSVGANL, NULL, ARRAY_SIZE(GSL1688_FW_L710_XC_101_WSVGANL)},
};

TP_PROJECT_DATA lf710_tp_data[] = {
   {"LF710_DH_101_WXGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_DH_101_WXGANL, GSL1688_FW_LF710_DH_101_WXGANL, NULL, ARRAY_SIZE(GSL1688_FW_LF710_DH_101_WXGANL)},
   {"LF710_DH_101_WUXGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_DH_101_WUXGANL, GSL1688_FW_LF710_DH_101_WUXGANL, NULL, ARRAY_SIZE(GSL1688_FW_LF710_DH_101_WUXGANL)},
   {"LF710_MR", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_MR, GSL1688_FW_LF710_MR, NULL, ARRAY_SIZE(GSL1688_FW_LF710_MR)},
   {"LF710_MJK", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_MJK, GSL1688_FW_LF710_MJK, NULL, ARRAY_SIZE(GSL1688_FW_LF710_MJK)},
   {"LF710_SQ_101_WXGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_SQ_101_WXGANL, GSL1688_FW_LF710_SQ_101_WXGANL, NULL, ARRAY_SIZE(GSL1688_FW_LF710_SQ_101_WXGANL)},
   {"LF710_YD", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_YD, GSL1688_FW_LF710_YD, NULL, ARRAY_SIZE(GSL1688_FW_LF710_YD)},
   {"LF710_SX_101_WXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_SX_101_WXGA, GSL1688_FW_LF710_SX_101_WXGA, NULL, ARRAY_SIZE(GSL1688_FW_LF710_SX_101_WXGA)},
   {"LF710_MR_101_WXGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_MR_101_WXGANL, GSL1688_FW_LF710_MR_101_WXGANL, NULL, ARRAY_SIZE(GSL1688_FW_LF710_MR_101_WXGANL)},
   {"LF710_HT_WXGA_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_HT_WXGA_101, GSL1688_FW_LF710_HT_WXGA_101, NULL, ARRAY_SIZE(GSL1688_FW_LF710_HT_WXGA_101)},
   {"LF710_XLD_WXGA_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF710_XLD_WXGA_101, GSLX680_FW_LF710_XLD_WXGA_101, NULL, ARRAY_SIZE(GSLX680_FW_LF710_XLD_WXGA_101)},
};

TP_PROJECT_DATA l805b_tp_data[] = {
   {"L805B_DIXIAN_8_WXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_L805B_DIXIAN_8_WXGA, GSL1688_FW_L805B_DIXIAN_8_WXGA, NULL, ARRAY_SIZE(GSL1688_FW_L805B_DIXIAN_8_WXGA)},

};

TP_PROJECT_DATA ls103i_tp_data[] = {
   {"LS103I_MGL_101_WSVGANL", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LS103I_MGL_101_WSVGANL, GSL1688_FW_LS103I_MGL_101_WSVGANL, NULL, ARRAY_SIZE(GSL1688_FW_LS103I_MGL_101_WSVGANL)},

};
TP_PROJECT_DATA lf808_tp_data[] = {
   {"LF808_QTW_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_QTW_101, GSL1688_FW_LF808_QTW_101, NULL, ARRAY_SIZE(GSL1688_FW_LF808_QTW_101)},
   {"LF808_DFL_101_WXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DFL_101_WXGA, GSL1688_FW_LF808_DFL_101_WXGA, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DFL_101_WXGA)},
   {"LF808_DX_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DX_101, GSL1688_FW_LF808_DX_101, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DX_101)},
   {"LF808_DFL_PG", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DFL_PG, GSLX680_FW_LF808_DFL_PG, NULL, ARRAY_SIZE(GSLX680_FW_LF808_DFL_PG)},
   {"LF808_DFL_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DFL_101, GSL1688_FW_LF808_DFL_101, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DFL_101)},
   {"LF808_PB_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_PB_101, GSL1688_FW_LF808_PB_101, NULL, ARRAY_SIZE(GSL1688_FW_LF808_PB_101)},
  {"LF808_DZ", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DZ, GSL1688_FW_LF808_DZ, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DZ)},
   {"LF808_RSS", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_RSS, GSL1688_FW_LF808_RSS, NULL, ARRAY_SIZE(GSL1688_FW_LF808_RSS)},
   {"LF808_DZ_COB", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DZ_COB, GSL1688_FW_LF808_DZ_COB, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DZ_COB)},
   {"LF808_DFL_PG_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DFL_PG_101, GSLX680_FW_LF808_DFL_PG_101, NULL, ARRAY_SIZE(GSLX680_FW_LF808_DFL_PG_101)},
   {"LF808_DZ_OGS_WUXGA", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DZ_OGS_WUXGA, GSL1688_FW_LF808_DZ_OGS_WUXGA, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DZ_OGS_WUXGA)},
   {"LF808_DFL_8", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DFL_8, GSL1688_FW_LF808_DFL_8, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DFL_8)},
      {"LF808_DZ_COF_101", {0xff,0xff,0xff,0xff}, gsl_config_data_id_LF808_DZ_COF_101, GSL1688_FW_LF808_DZ_COF_101, NULL, ARRAY_SIZE(GSL1688_FW_LF808_DZ_COF_101)},
   };
ALL_TP_INFO gsl_tp_info[] = {
   {"L696",    l696_tp_gsl_modle,   l696_tp_data,  0, ARRAY_SIZE(l696_tp_data),  0},
   {"L695",    l695_tp_gsl_modle,   l695_tp_data,  0, ARRAY_SIZE(l695_tp_data),  0},
   {"L708G",   l708g_tp_gsl_modle,  l708g_tp_data, 0, ARRAY_SIZE(l708g_tp_data), 0},
   {"U706",    u706_tp_gsl_modle,   u706_tp_data,  0, ARRAY_SIZE(u706_tp_data),  0},
   {"U357",    u357_tp_gsl_modle,   u357_tp_data,  0, ARRAY_SIZE(u357_tp_data),  0},
   {"LF705",   lf705_tp_gsl_modle,  lf705_tp_data, 0, ARRAY_SIZE(lf705_tp_data),  0},   
   {"U101",    u101_tp_gsl_modle,   u101_tp_data,  0, ARRAY_SIZE(u101_tp_data),  0},   
   {"L710",    l710_tp_gsl_modle,  l710_tp_data, 0, ARRAY_SIZE(l710_tp_data),  0},   
   {"LF102",   lf102_tp_gsl_modle,  lf102_tp_data,  0, ARRAY_SIZE(lf102_tp_data),  0},
   {"LF710",   lf710_tp_gsl_modle,  lf710_tp_data, 0, ARRAY_SIZE(lf710_tp_data),  0},  
   {"L805B",   l805b_tp_gsl_modle,  l805b_tp_data, 0, ARRAY_SIZE(l805b_tp_data),  0}, 
   {"LS103I",  ls103i_tp_gsl_modle, ls103i_tp_data, 0, ARRAY_SIZE(ls103i_tp_data),  0}, 
   {"LF808",   lf808_tp_gsl_modle,  lf808_tp_data,  0, ARRAY_SIZE(lf808_tp_data),  0},
   {"LF810",   lf810_tp_gsl_modle,  lf810_tp_data,  0, ARRAY_SIZE(lf810_tp_data),  0},
};

#endif
