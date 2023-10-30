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

#include "mt65xx_lcm_list.h"
#include <lcm_drv.h>
#ifdef BUILD_LK
#include <platform/disp_drv_platform.h>
#else
#include <linux/delay.h>
/* #include <mach/mt_gpio.h> */
#endif
LCM_DSI_MODE_CON lcm_dsi_mode;

/* used to identify float ID PIN status */
#define LCD_HW_ID_STATUS_LOW      0
#define LCD_HW_ID_STATUS_HIGH     1
#define LCD_HW_ID_STATUS_FLOAT 0x02
#define LCD_HW_ID_STATUS_ERROR  0x03

#ifdef BUILD_LK
#define LCD_DEBUG(fmt)  dprintf(CRITICAL, fmt)
#else
#define LCD_DEBUG(fmt, args...)  pr_debug("[KERNEL/LCM]"fmt, ##args)
#endif
/**######################## MID_LCM_START ###########################**/
extern LCM_DRIVER u357_fy_ili6136s_wxga_ips_lcm_drv;
extern LCM_DRIVER l696_mipi_xy_ili9881c_wxga_ips_cpt_lcm_drv;
extern LCM_DRIVER l696_mipi_wxga_qc_n070icegb2_lcm_drv;
extern LCM_DRIVER l695_gx_kd070d27_lcm_drv;
extern LCM_DRIVER l696_hjy_hx8394d_wxga_dsi_vdo_lcm_drv;
extern LCM_DRIVER l696_hz_jd9366_wxga_lcm_drv;
extern LCM_DRIVER u706_wsvga_mipi_lcm_drv; 
extern LCM_DRIVER u357_fy_ili6136s_wxga_lcm_drv; 
extern LCM_DRIVER u705_wxga_mipi_lcm_drv;
extern LCM_DRIVER l696_mipi_wxga_lg_ld070wx4_lcm_drv;
extern LCM_DRIVER u706_fc070nq29_nt51012_wxga_lcm_drv;
extern LCM_DRIVER lf705_sat_sat695cp30i20r2_26095m140in_lcm_drv; 
extern LCM_DRIVER lf705_pxjt_px695ia18610423a_lcm_drv; 
extern LCM_DRIVER lf705_dy_hx8394_wxga_ips_lcm_drv; 
extern LCM_DRIVER lf705_xy_hc070cwf01_wxga_ips_cpt_lcm_drv; 
extern LCM_DRIVER u706_dy_jd9366_wxga_ips_boe_lcm_drv;
extern LCM_DRIVER u101_m101ni01_tii40_a01_wxga_lcm_drv;
extern LCM_DRIVER lf102_sl_nt51021_wuxga_ips_qc_lcm_drv;
extern LCM_DRIVER lf705_qingyuqn_jd9366ab_wxga_ips_lcm_drv;
extern LCM_DRIVER l710g_hxty_wxga_ips_10_auo_lcm_drv;
extern LCM_DRIVER lf705_hz_jd9366_wxga_ips_lcm_drv;
extern LCM_DRIVER lf705_xinxian_ek79029_wxga_ips_cpt_lcm_drv;
extern LCM_DRIVER l710g_px101ih27810178a_nt35521s_wxga_ips_lcm_drv;
extern LCM_DRIVER l710g_krgd_ek79007ad_wsvgalnl_ips_hsd_lcm_drv;
extern LCM_DRIVER lf705_mc_jd9366_wxga_cpt_8_lcm_drv;
extern LCM_DRIVER lf710_kr_jd9366_wxga_ips_lcm_drv;
extern LCM_DRIVER l805b_nt35523_wxga_ips_lcm_drv;
extern LCM_DRIVER l696_qc_nt35523_wxga_ips_boe_mipi_lcm_drv;
extern LCM_DRIVER lf141_it6151_fhd_edp_dsi_video_lcm_drv;
extern LCM_DRIVER lf705_hzx_hx8394d_hd720_tn_cpt_695_lcm_drv;
extern LCM_DRIVER u706_wsvga_mipi_starry_lcm_drv;
extern LCM_DRIVER u357_yql_s6d7aa0x04_wxga_lcm_drv;
extern LCM_DRIVER u357_sx_nt35523_wxga_boe_lcm_drv;
extern LCM_DRIVER lf808_pvo_otm1287a_wxga_ips_auo_lcm_drv;
extern LCM_DRIVER l710g_gx_101n66_40ni_k2_wxga_lcm_drv;
extern LCM_DRIVER u357_hz_wjwx8007a_jd9366_wxga_lcm_drv;
extern LCM_DRIVER lf808_sl_nt35521s_wxga_ips_cpt_lcm_drv;
extern LCM_DRIVER lf808_mc_nt35521s_wxga_ips_lcm_drv;
extern LCM_DRIVER u101_sl_pn27_a00_nt35521_wxga_lcm_drv;
extern LCM_DRIVER lf808_mc_nt35521s_wuxga_ips_lcm_drv;
extern LCM_DRIVER u101_sat_otm1287a_wxga_ips_lcm_drv;
extern LCM_DRIVER u101_yql_ili9881c_wxga_lcm_drv;
extern LCM_DRIVER lf710_hz_jd9364ac_boe_wxga_ips_101_lcm_drv;
extern LCM_DRIVER lf705_hh_hx8394d_boe_wxga_ips_7_lcm_drv;
extern LCM_DRIVER l700_fx_im10704_wxga_ips_lcm_drv;
extern LCM_DRIVER ue78_he080ia_1_lcm_drv;
extern LCM_DRIVER ls103i_pnl_bn101_wxga_lcm_drv;
extern LCM_DRIVER lf705_ch_jd9366_boe_wxga_ips_7_lcm_drv;
extern LCM_DRIVER lf705_fx_im10705_jd9362_wxga_ips_lcm_drv;
extern LCM_DRIVER lf808_mc_jd9366_wxga_cpt_8_lcm_drv; 
extern LCM_DRIVER lf808_hnh_jd9367_wxga_cpt_8_lcm_drv;
extern LCM_DRIVER lf808_xinxian_jd9366_longteng_wxga_ips_8_lcm_drv;
extern LCM_DRIVER lf710_hz_ek79029ba_hsd_wxga_ips_101_lcm_drv;
extern LCM_DRIVER lf705_fx_jd9366_boe_wxga_ips_8_lcm_drv;
extern LCM_DRIVER lf808_pvo_jd9365_wxga_ips_auo_lcm_drv;
extern LCM_DRIVER	u101_hygd_ek79029_hsd_wxga_ips_101_lcm_drv;
extern LCM_DRIVER	l696_hnh_jd9366_ivo_wxga_ips_8_lcm_drv;
extern LCM_DRIVER	l696_hh_s6d7aa0x01_boe_wxga_ips_7_lcm_drv;
extern LCM_DRIVER	u357_pxjt_s6d7aa0x01_boe_wxga_ips_8_lcm_drv;
extern LCM_DRIVER	l700g_yd_nt51012_cpt_wxga_ips_7_lcm_drv;
extern LCM_DRIVER    lf705_ypd_ek79007_ivo_wsvga_7_lcm_drv;
extern LCM_DRIVER    lf705_xy_hx8394d_boe_wxga_ips_lcm_drv;
extern LCM_DRIVER	lf705_hz_jd9365_hsd_wxga_ips_8_lcm_drv;
extern LCM_DRIVER	lf710_sl_nt35521_cpt_wxga_ips_101_lcm_drv;
extern LCM_DRIVER	lf705_xinxian_jd9366_longteng_wxga_ips_8_lcm_drv;
extern LCM_DRIVER	lf705_hx_hx8686b_cpt_wxga_ips_7_lcm_drv;
extern LCM_DRIVER    lf710_hnh_jd9364_wxga_ips_101_lcm_drv;
extern LCM_DRIVER	lf705_bsw_rm72010_auo_wxga_ips_8_lcm_drv;
extern LCM_DRIVER lf705_zs_ek79029aa_boe_wxga_ips_8_lcm_drv;

extern LCM_DRIVER lf705_qc_nt35521_wxga_ips_lux_lcm_drv;  
extern LCM_DRIVER lf705_rhf_ili6180_wsvga_ips_inx_lcm_drv;
extern LCM_DRIVER l710g_hytc_otm1287a_wxga_ips_lcm_drv;
extern LCM_DRIVER lf710_mc_nt35521s_wxga_ips_lcm_drv;
extern LCM_DRIVER lf710_scgd_otm1287a_wxga_cpt_lcm_drv;
extern LCM_DRIVER u101_sl_pc27_a00_nt35521_wxga_lcm_drv;
extern LCM_DRIVER u101_pb_jd9365_wxga_lcm_drv;

extern LCM_DRIVER lf710_fx_ota7290b_boe_wuxga_ips_101_lcm_drv;
extern LCM_DRIVER lf810_tianan_hx8394d_wxga_ips_boe_lcm_drv;
extern LCM_DRIVER lf705_jf_jd9366_boe_wxga_ips_7_lcm_drv;

extern LCM_DRIVER lf710_pb_jd9365_hsd_wxga_ips_101_lcm_drv;

extern LCM_DRIVER lf808_rllk_NT51019_auo_wuxga_ips_lcm_drv;
extern LCM_DRIVER lf808_yds_nt52021_wuxga_ips_cpt_8_lcm_drv;
extern LCM_DRIVER u101_pb_jd9365_boe_wxga_ips_101_lcm_drv;

extern LCM_DRIVER lf705_hnh_ota7290_inx_wuxga_ips_8_lcm_drv;
extern LCM_DRIVER lf810_fx_ota7290b_boe_wuxga_ips_101_lcm_drv;

extern LCM_DRIVER lf710_kr_ek79029ba2_hsd_wxga_ips_101_lcm_drv;

extern LCM_DRIVER lf710_pb_jd9365_boe_wxga_ips_101_lcm_drv;

extern LCM_DRIVER lf705_fc_jd9365_cpt_wxga_ips_7_lcm_drv;
LCM_DRIVER *lcm_driver_list[] = {
	&lf705_fc_jd9365_cpt_wxga_ips_7_lcm_drv,
    &lf808_yds_nt52021_wuxga_ips_cpt_8_lcm_drv,
	&lf710_pb_jd9365_boe_wxga_ips_101_lcm_drv,

	&lf710_kr_ek79029ba2_hsd_wxga_ips_101_lcm_drv,

	&lf705_hnh_ota7290_inx_wuxga_ips_8_lcm_drv,

	&lf710_fx_ota7290b_boe_wuxga_ips_101_lcm_drv,
	&u101_pb_jd9365_boe_wxga_ips_101_lcm_drv,

	&lf808_rllk_NT51019_auo_wuxga_ips_lcm_drv,

	&lf710_pb_jd9365_hsd_wxga_ips_101_lcm_drv,

	&lf705_jf_jd9366_boe_wxga_ips_7_lcm_drv,

	&lf810_tianan_hx8394d_wxga_ips_boe_lcm_drv,
	&u101_pb_jd9365_wxga_lcm_drv,
	&lf808_pvo_jd9365_wxga_ips_auo_lcm_drv,
	&lf810_fx_ota7290b_boe_wuxga_ips_101_lcm_drv,

  &u101_pb_jd9365_wxga_lcm_drv,
	&lf710_sl_nt35521_cpt_wxga_ips_101_lcm_drv,
	&lf710_hnh_jd9364_wxga_ips_101_lcm_drv,
	&lf705_bsw_rm72010_auo_wxga_ips_8_lcm_drv,
	&lf705_fx_jd9366_boe_wxga_ips_8_lcm_drv,
	&u357_fy_ili6136s_wxga_ips_lcm_drv,	
	&lf705_fx_im10705_jd9362_wxga_ips_lcm_drv,
	&lf705_zs_ek79029aa_boe_wxga_ips_8_lcm_drv,
	&u101_hygd_ek79029_hsd_wxga_ips_101_lcm_drv,
	&l696_hnh_jd9366_ivo_wxga_ips_8_lcm_drv,
	&l696_hh_s6d7aa0x01_boe_wxga_ips_7_lcm_drv,
	&u357_pxjt_s6d7aa0x01_boe_wxga_ips_8_lcm_drv,
	&l700g_yd_nt51012_cpt_wxga_ips_7_lcm_drv,
    &lf705_ypd_ek79007_ivo_wsvga_7_lcm_drv,
    &lf705_xy_hx8394d_boe_wxga_ips_lcm_drv,
	&lf705_hz_jd9365_hsd_wxga_ips_8_lcm_drv,
    &lf102_sl_nt51021_wuxga_ips_qc_lcm_drv,
	&lf705_xinxian_jd9366_longteng_wxga_ips_8_lcm_drv,
	&lf705_hx_hx8686b_cpt_wxga_ips_7_lcm_drv,
	&lf141_it6151_fhd_edp_dsi_video_lcm_drv,	
	&lf808_pvo_otm1287a_wxga_ips_auo_lcm_drv,
	&lf808_sl_nt35521s_wxga_ips_cpt_lcm_drv,
	&lf808_mc_nt35521s_wxga_ips_lcm_drv,
	&lf710_hz_ek79029ba_hsd_wxga_ips_101_lcm_drv,
	&lf808_mc_nt35521s_wuxga_ips_lcm_drv,
    &lf808_mc_jd9366_wxga_cpt_8_lcm_drv,
	&lf808_hnh_jd9367_wxga_cpt_8_lcm_drv,
	&lf808_xinxian_jd9366_longteng_wxga_ips_8_lcm_drv,
	&lf705_ch_jd9366_boe_wxga_ips_7_lcm_drv,
	&ue78_he080ia_1_lcm_drv,
	&ls103i_pnl_bn101_wxga_lcm_drv,
	&l700_fx_im10704_wxga_ips_lcm_drv,
	&lf710_hz_jd9364ac_boe_wxga_ips_101_lcm_drv,
	&u101_sl_pn27_a00_nt35521_wxga_lcm_drv,
	&u101_sl_pc27_a00_nt35521_wxga_lcm_drv,
	&u101_sat_otm1287a_wxga_ips_lcm_drv,
	&u101_yql_ili9881c_wxga_lcm_drv,
	&lf705_hh_hx8394d_boe_wxga_ips_7_lcm_drv,
	&lf710_mc_nt35521s_wxga_ips_lcm_drv,
	&lf710_scgd_otm1287a_wxga_cpt_lcm_drv,
	&u357_hz_wjwx8007a_jd9366_wxga_lcm_drv,
	&l710g_gx_101n66_40ni_k2_wxga_lcm_drv,
	&l710g_hytc_otm1287a_wxga_ips_lcm_drv,
	&u357_sx_nt35523_wxga_boe_lcm_drv,
	&u357_yql_s6d7aa0x04_wxga_lcm_drv,
	&lf705_hzx_hx8394d_hd720_tn_cpt_695_lcm_drv,
	&lf705_rhf_ili6180_wsvga_ips_inx_lcm_drv,
	&l696_qc_nt35523_wxga_ips_boe_mipi_lcm_drv,
	&u706_dy_jd9366_wxga_ips_boe_lcm_drv, 
	&l696_mipi_xy_ili9881c_wxga_ips_cpt_lcm_drv,
	&l696_mipi_wxga_lg_ld070wx4_lcm_drv,
	&l696_mipi_wxga_qc_n070icegb2_lcm_drv,
	&l695_gx_kd070d27_lcm_drv,
	&l696_hjy_hx8394d_wxga_dsi_vdo_lcm_drv,
	&l696_hz_jd9366_wxga_lcm_drv,
	&u706_wsvga_mipi_lcm_drv,
	&u357_fy_ili6136s_wxga_lcm_drv,
	&u705_wxga_mipi_lcm_drv,
	&u706_fc070nq29_nt51012_wxga_lcm_drv,
	&lf705_sat_sat695cp30i20r2_26095m140in_lcm_drv,
	&lf705_pxjt_px695ia18610423a_lcm_drv,
	&lf705_dy_hx8394_wxga_ips_lcm_drv,
	&lf705_xy_hc070cwf01_wxga_ips_cpt_lcm_drv,
	&u101_m101ni01_tii40_a01_wxga_lcm_drv,
	&lf705_qc_nt35521_wxga_ips_lux_lcm_drv,
	&lf705_qingyuqn_jd9366ab_wxga_ips_lcm_drv,
	&l710g_hxty_wxga_ips_10_auo_lcm_drv,
	&lf705_hz_jd9366_wxga_ips_lcm_drv, 
	&lf705_xinxian_ek79029_wxga_ips_cpt_lcm_drv,
	&l710g_px101ih27810178a_nt35521s_wxga_ips_lcm_drv,
	&l710g_krgd_ek79007ad_wsvgalnl_ips_hsd_lcm_drv,
	&lf705_mc_jd9366_wxga_cpt_8_lcm_drv,
	&lf710_kr_jd9366_wxga_ips_lcm_drv,
	&l805b_nt35523_wxga_ips_lcm_drv,
	&u706_wsvga_mipi_starry_lcm_drv,

};
/**######################## MID_LCM_END #############################**/
unsigned char lcm_name_list[][128] = {
#if defined(HX8392A_DSI_CMD)
	"hx8392a_dsi_cmd",
#endif

#if defined(S6E3HA3_WQHD_2K_CMD)
	"s6e3ha3_wqhd_2k_cmd",
#endif

#if defined(HX8392A_DSI_VDO)
	"hx8392a_vdo_cmd",
#endif

#if defined(HX8392A_DSI_CMD_FWVGA)
	"hx8392a_dsi_cmd_fwvga",
#endif

#if defined(OTM9608_QHD_DSI_CMD)
	"otm9608a_qhd_dsi_cmd",
#endif

#if defined(OTM9608_QHD_DSI_VDO)
	"otm9608a_qhd_dsi_vdo",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358)
	"r63417_fhd_dsi_cmd_truly_nt50358_drv",
#endif

#if defined(R63417_FHD_DSI_CMD_TRULY_NT50358_QHD)
	"r63417_fhd_dsi_cmd_truly_nt50358_qhd_drv",
#endif

#if defined(R63417_FHD_DSI_VDO_TRULY_NT50358)
	"r63417_fhd_dsi_vdo_truly_nt50358_drv",
#endif

#if defined(R63419_WQHD_TRULY_PHANTOM_2K_CMD_OK)
	"r63419_wqhd_truly_phantom_2k_cmd_ok",
#endif
};

#define LCM_COMPILE_ASSERT(condition) LCM_COMPILE_ASSERT_X(condition, __LINE__)
#define LCM_COMPILE_ASSERT_X(condition, line) LCM_COMPILE_ASSERT_XX(condition, line)
#define LCM_COMPILE_ASSERT_XX(condition, line) char assertion_failed_at_line_##line[(condition) ? 1 : -1]

unsigned int lcm_count = sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *);
LCM_COMPILE_ASSERT(0 != sizeof(lcm_driver_list) / sizeof(LCM_DRIVER *));
#if defined(NT35520_HD720_DSI_CMD_TM) | defined(NT35520_HD720_DSI_CMD_BOE) | \
	defined(NT35521_HD720_DSI_VDO_BOE) | defined(NT35521_HD720_DSI_VIDEO_TM)
static unsigned char lcd_id_pins_value = 0xFF;

/**
 * Function:       which_lcd_module_triple
 * Description:    read LCD ID PIN status,could identify three status:highlowfloat
 * Input:           none
 * Output:         none
 * Return:         LCD ID1|ID0 value
 * Others:
 */
unsigned char which_lcd_module_triple(void)
{
	unsigned char  high_read0 = 0;
	unsigned char  low_read0 = 0;
	unsigned char  high_read1 = 0;
	unsigned char  low_read1 = 0;
	unsigned char  lcd_id0 = 0;
	unsigned char  lcd_id1 = 0;
	unsigned char  lcd_id = 0;
	/*Solve Coverity scan warning : check return value*/
	unsigned int ret = 0;

	/*only recognise once*/
	if (0xFF != lcd_id_pins_value)
		return lcd_id_pins_value;

	/*Solve Coverity scan warning : check return value*/
	ret = mt_set_gpio_mode(GPIO_DISP_ID0_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID0_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID0_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_enable fail\n");

	ret = mt_set_gpio_mode(GPIO_DISP_ID1_PIN, GPIO_MODE_00);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_mode fail\n");

	ret = mt_set_gpio_dir(GPIO_DISP_ID1_PIN, GPIO_DIR_IN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_dir fail\n");

	ret = mt_set_gpio_pull_enable(GPIO_DISP_ID1_PIN, GPIO_PULL_ENABLE);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_enable fail\n");

	/*pull down ID0 ID1 PIN*/
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

	/* delay 100ms , for discharging capacitance*/
	mdelay(100);
	/* get ID0 ID1 status*/
	low_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	low_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);
	/* pull up ID0 ID1 PIN */
	ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

	ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
	if (0 != ret)
		LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

	/* delay 100ms , for charging capacitance */
	mdelay(100);
	/* get ID0 ID1 status */
	high_read0 = mt_get_gpio_in(GPIO_DISP_ID0_PIN);
	high_read1 = mt_get_gpio_in(GPIO_DISP_ID1_PIN);

	if (low_read0 != high_read0) {
		/*float status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read0) && (LCD_HW_ID_STATUS_LOW == high_read0)) {
		/*low status , pull down ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Down fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read0) && (LCD_HW_ID_STATUS_HIGH == high_read0)) {
		/*high status , pull up ID0 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->UP fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_HIGH;
	} else {
		LCD_DEBUG(" Read LCD_id0 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID0_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID0 mt_set_gpio_pull_select->Disbale fail\n");

		lcd_id0 = LCD_HW_ID_STATUS_ERROR;
	}


	if (low_read1 != high_read1) {
		/*float status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_FLOAT;
	} else if ((LCD_HW_ID_STATUS_LOW == low_read1) && (LCD_HW_ID_STATUS_LOW == high_read1)) {
		/*low status , pull down ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DOWN);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Down fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_LOW;
	} else if ((LCD_HW_ID_STATUS_HIGH == low_read1) && (LCD_HW_ID_STATUS_HIGH == high_read1)) {
		/*high status , pull up ID1 ,to prevent electric leakage*/
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_UP);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->UP fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_HIGH;
	} else {

		LCD_DEBUG(" Read LCD_id1 error\n");
		ret = mt_set_gpio_pull_select(GPIO_DISP_ID1_PIN, GPIO_PULL_DISABLE);
		if (0 != ret)
			LCD_DEBUG("ID1 mt_set_gpio_pull_select->Disable fail\n");

		lcd_id1 = LCD_HW_ID_STATUS_ERROR;
	}
#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id0:%d\n", lcd_id0);
	LCD_DEBUG("which_lcd_module_triple,lcd_id1:%d\n", lcd_id1);
#endif
	lcd_id =  lcd_id0 | (lcd_id1 << 2);

#ifdef BUILD_LK
	dprintf(CRITICAL, "which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#else
	LCD_DEBUG("which_lcd_module_triple,lcd_id:%d\n", lcd_id);
#endif

	lcd_id_pins_value = lcd_id;
	return lcd_id;
}
#endif
