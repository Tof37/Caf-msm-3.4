/* Copyright (c) 2008-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef MDSS_PANEL_H
#define MDSS_PANEL_H

#include <linux/platform_device.h>
#include <linux/types.h>

/* panel id type */
struct panel_id {
	u16 id;
	u16 type;
};

#define DEFAULT_FRAME_RATE	60

/* panel type list */
#define NO_PANEL		0xffff	/* No Panel */
#define MDDI_PANEL		1	/* MDDI */
#define EBI2_PANEL		2	/* EBI2 */
#define LCDC_PANEL		3	/* internal LCDC type */
#define EXT_MDDI_PANEL		4	/* Ext.MDDI */
#define TV_PANEL		5	/* TV */
#define HDMI_PANEL		6	/* HDMI TV */
#define DTV_PANEL		7	/* DTV */
#define MIPI_VIDEO_PANEL	8	/* MIPI */
#define MIPI_CMD_PANEL		9	/* MIPI */
#define WRITEBACK_PANEL		10	/* Wifi display */
#define LVDS_PANEL		11	/* LVDS */
#define EDP_PANEL		12	/* LVDS */

/* panel class */
enum {
	DISPLAY_LCD = 0,	/* lcd = ebi2/mddi */
	DISPLAY_LCDC,		/* lcdc */
	DISPLAY_TV,		/* TV Out */
	DISPLAY_EXT_MDDI,	/* External MDDI */
	DISPLAY_WRITEBACK,
};

/* panel device locaiton */
enum {
	DISPLAY_1 = 0,		/* attached as first device */
	DISPLAY_2,		/* attached on second device */
	DISPLAY_3,              /* attached on third writeback device */
	MAX_PHYS_TARGET_NUM,
};

enum {
	MDSS_PANEL_INTF_INVALID = -1,
	MDSS_PANEL_INTF_DSI,
	MDSS_PANEL_INTF_EDP,
	MDSS_PANEL_INTF_HDMI,
};

#define MDSS_MAX_PANEL_LEN      256
#define MDSS_INTF_MAX_NAME_LEN 5
struct mdss_panel_intf {
	char name[MDSS_INTF_MAX_NAME_LEN];
	u8   len;
	int  type;
};

struct mdss_panel_cfg {
	char arg_cfg[MDSS_MAX_PANEL_LEN + 1];
	int  pan_intf;
	bool lk_cfg;
	bool init_done;
};

/**
 * enum mdss_intf_events - Different events generated by MDP core
 *
 * @MDSS_EVENT_RESET:		MDP control path is being (re)initialized.
 * @MDSS_EVENT_UNBLANK:		Sent before first frame update from MDP is
 *				sent to panel.
 * @MDSS_EVENT_PANEL_ON:	After first frame update from MDP.
 * @MDSS_EVENT_BLANK:		MDP has no contents to display only blank screen
 *				is shown in panel. Sent before panel off.
 * @MDSS_EVENT_PANEL_OFF:	MDP has suspended frame updates, panel should be
 *				completely shutdown after this call.
 * @MDSS_EVENT_CLOSE:		MDP has tore down entire session.
 * @MDSS_EVENT_SUSPEND:		Propagation of power suspend event.
 * @MDSS_EVENT_RESUME:		Propagation of power resume event.
 * @MDSS_EVENT_CHECK_PARAMS:	Event generated when a panel reconfiguration is
 *				requested including when resolution changes.
 *				The event handler receives pointer to
 *				struct mdss_panel_info and should return one of:
 *				 - negative if the configuration is invalid
 *				 - 0 if there is no panel reconfig needed
 *				 - 1 if reconfig is needed to take effect
 * @MDSS_EVENT_CONT_SPLASH_BEGIN: Special event used to handle transition of
 *				display state from boot loader to panel driver.
 *				The event handler will disable the panel.
 * @MDSS_EVENT_CONT_SPLASH_FINISH: Special event used to handle transition of
 *				display state from boot loader to panel driver.
 *				The event handler will enable the panel and
 *				vote for the display clocks.
 * @MDSS_EVENT_PANEL_UPDATE_FPS: Event to update the frame rate of the panel.
 * @MDSS_EVENT_FB_REGISTERED:	Called after fb dev driver has been registered,
 *				panel driver gets ptr to struct fb_info which
 *				holds fb dev information.
 * @MDSS_EVENT_PANEL_CLK_CTRL:	panel clock control
				 - 0 clock disable
				 - 1 clock enable
 * @MDSS_EVENT_DSI_CMDLIST_KOFF: kickoff sending dcs command from command list
 */
enum mdss_intf_events {
	MDSS_EVENT_RESET = 1,
	MDSS_EVENT_UNBLANK,
	MDSS_EVENT_PANEL_ON,
	MDSS_EVENT_BLANK,
	MDSS_EVENT_PANEL_OFF,
	MDSS_EVENT_CLOSE,
	MDSS_EVENT_SUSPEND,
	MDSS_EVENT_RESUME,
	MDSS_EVENT_CHECK_PARAMS,
	MDSS_EVENT_CONT_SPLASH_BEGIN,
	MDSS_EVENT_CONT_SPLASH_FINISH,
	MDSS_EVENT_PANEL_UPDATE_FPS,
	MDSS_EVENT_FB_REGISTERED,
	MDSS_EVENT_PANEL_CLK_CTRL,
	MDSS_EVENT_DSI_CMDLIST_KOFF,
};

struct lcd_panel_info {
	u32 h_back_porch;
	u32 h_front_porch;
	u32 h_pulse_width;
	u32 v_back_porch;
	u32 v_front_porch;
	u32 v_pulse_width;
	u32 border_clr;
	u32 underflow_clr;
	u32 hsync_skew;
	/* Pad width */
	u32 xres_pad;
	/* Pad height */
	u32 yres_pad;
};


/* DSI PHY configuration */
struct mdss_dsi_phy_ctrl {
	uint32_t regulator[7];
	uint32_t timing[12];
	uint32_t ctrl[4];
	uint32_t strength[2];
	char bistCtrl[6];
	uint32_t pll[21];
	char laneCfg[45];
};

struct mipi_panel_info {
	char mode;		/* video/cmd */
	char interleave_mode;
	char crc_check;
	char ecc_check;
	char dst_format;	/* shared by video and command */
	char data_lane0;
	char data_lane1;
	char data_lane2;
	char data_lane3;
	char dlane_swap;	/* data lane swap */
	char rgb_swap;
	char b_sel;
	char g_sel;
	char r_sel;
	char rx_eot_ignore;
	char tx_eot_append;
	char t_clk_post; /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char t_clk_pre;  /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char vc;	/* virtual channel */
	struct mdss_dsi_phy_ctrl *dsi_phy_db;
	/* video mode */
	char pulse_mode_hsa_he;
	char hfp_power_stop;
	char hbp_power_stop;
	char hsa_power_stop;
	char eof_bllp_power_stop;
	char bllp_power_stop;
	char traffic_mode;
	char frame_rate;
	/* command mode */
	char interleave_max;
	char insert_dcs_cmd;
	char wr_mem_continue;
	char wr_mem_start;
	char te_sel;
	char stream;	/* 0 or 1 */
	char mdp_trigger;
	char dma_trigger;
	u32 dsi_pclk_rate;
	/* The packet-size should not bet changed */
	char no_max_pkt_size;
	/* Clock required during LP commands */
	char force_clk_lane_hs;

	char vsync_enable;
	char hw_vsync_mode;
};

enum dynamic_fps_update {
	DFPS_SUSPEND_RESUME_MODE,
	DFPS_IMMEDIATE_CLK_UPDATE_MODE,
};

enum lvds_mode {
	LVDS_SINGLE_CHANNEL_MODE,
	LVDS_DUAL_CHANNEL_MODE,
};

struct lvds_panel_info {
	enum lvds_mode channel_mode;
	/* Channel swap in dual mode */
	char channel_swap;
};

struct fbc_panel_info {
	u32 enabled;
	u32 target_bpp;
	u32 comp_mode;
	u32 qerr_enable;
	u32 cd_bias;
	u32 pat_enable;
	u32 vlc_enable;
	u32 bflc_enable;

	u32 line_x_budget;
	u32 block_x_budget;
	u32 block_budget;

	u32 lossless_mode_thd;
	u32 lossy_mode_thd;
	u32 lossy_rgb_thd;
	u32 lossy_mode_idx;
};

struct mdss_panel_info {
	u32 xres;
	u32 yres;
	u32 physical_width;
	u32 physical_height;
	u32 bpp;
	u32 type;
	u32 wait_cycle;
	u32 pdest;
	u32 bl_max;
	u32 bl_min;
	u32 fb_num;
	u32 clk_rate;
	u32 clk_min;
	u32 clk_max;
	u32 frame_count;
	u32 is_3d_panel;
	u32 out_format;
	u32 vic; /* video identification code */
	int bklt_ctrl;	/* backlight ctrl */
	int pwm_pmic_gpio;
	int pwm_lpg_chan;
	int pwm_period;
	bool dynamic_fps;
	char dfps_update;
	int new_fps;

	u32 cont_splash_enabled;
	struct ion_handle *splash_ihdl;
	u32 panel_power_on;

	struct lcd_panel_info lcdc;
	struct fbc_panel_info fbc;
	struct mipi_panel_info mipi;
	struct lvds_panel_info lvds;
};

struct mdss_panel_data {
	struct mdss_panel_info panel_info;
	void (*set_backlight) (struct mdss_panel_data *pdata, u32 bl_level);
	unsigned char *mmss_cc_base;

	/**
	 * event_handler() - callback handler for MDP core events
	 * @pdata:	Pointer refering to the panel struct associated to this
	 *		event. Can be used to retrieve panel info.
	 * @e:		Event being generated, see enum mdss_intf_events
	 * @arg:	Optional argument to pass some info from some events.
	 *
	 * Used to register handler to be used to propagate different events
	 * happening in MDP core driver. Panel driver can listen for any of
	 * these events to perform appropriate actions for panel initialization
	 * and teardown.
	 */
	int (*event_handler) (struct mdss_panel_data *pdata, int e, void *arg);

	struct mdss_panel_data *next;
};

/**
 * mdss_get_panel_framerate() - get panel frame rate based on panel information
 * @panel_info:	Pointer to panel info containing all panel information
 */
static inline u32 mdss_panel_get_framerate(struct mdss_panel_info *panel_info)
{
	u32 frame_rate, pixel_total;

	if (panel_info == NULL)
		return DEFAULT_FRAME_RATE;

	switch (panel_info->type) {
	case MIPI_VIDEO_PANEL:
	case MIPI_CMD_PANEL:
		frame_rate = panel_info->mipi.frame_rate;
		break;
	case WRITEBACK_PANEL:
		frame_rate = DEFAULT_FRAME_RATE;
		break;
	default:
		pixel_total = (panel_info->lcdc.h_back_porch +
			  panel_info->lcdc.h_front_porch +
			  panel_info->lcdc.h_pulse_width +
			  panel_info->xres) *
			 (panel_info->lcdc.v_back_porch +
			  panel_info->lcdc.v_front_porch +
			  panel_info->lcdc.v_pulse_width +
			  panel_info->yres);
		if (pixel_total)
			frame_rate = panel_info->clk_rate / pixel_total;
		else
			frame_rate = DEFAULT_FRAME_RATE;

		break;
	}
	return frame_rate;
}

/*
 * mdss_panel_get_vtotal() - return panel vertical height
 * @pinfo:	Pointer to panel info containing all panel information
 *
 * Returns the total height of the panel including any blanking regions
 * which are not visible to user but used to calculate panel pixel clock.
 */
static inline int mdss_panel_get_vtotal(struct mdss_panel_info *pinfo)
{
	return pinfo->yres + pinfo->lcdc.v_back_porch +
			pinfo->lcdc.v_front_porch +
			pinfo->lcdc.v_pulse_width;
}

/*
 * mdss_panel_get_htotal() - return panel horizontal width
 * @pinfo:	Pointer to panel info containing all panel information
 *
 * Returns the total width of the panel including any blanking regions
 * which are not visible to user but used for calculations.
 */
static inline int mdss_panel_get_htotal(struct mdss_panel_info *pinfo)
{
	return pinfo->xres + pinfo->lcdc.h_back_porch +
			pinfo->lcdc.h_front_porch +
			pinfo->lcdc.h_pulse_width;
}

int mdss_register_panel(struct platform_device *pdev,
	struct mdss_panel_data *pdata);

/**
 * mdss_panel_intf_type: - checks if a given intf type is primary
 * @intf_val: panel interface type of the individual controller
 *
 * Individual controller queries with MDP to check if it is
 * configured as the primary interface.
 *
 * returns a pointer to the configured structure mdss_panel_cfg
 * to the controller that's configured as the primary panel interface.
 * returns NULL on error or if @intf_val is not the configured
 * controller.
 */
struct mdss_panel_cfg *mdss_panel_intf_type(int intf_val);

/**
 * mdss_panel_get_boot_cfg() - checks if bootloader config present
 *
 * Function returns true if bootloader has configured the parameters
 * for primary controller and panel config data.
 *
 * returns true if bootloader configured, else false
 */
int mdss_panel_get_boot_cfg(void);

/**
 * mdss_is_ready() - checks if mdss is probed and ready
 *
 * Checks if mdss resources have been initialized
 *
 * returns true if mdss is ready, else returns false.
 */
bool mdss_is_ready(void);
#endif /* MDSS_PANEL_H */
