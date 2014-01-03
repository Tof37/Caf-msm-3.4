/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/regulator/consumer.h>

#include <mach/rpm-regulator-smd.h>
#include <mach/socinfo.h>
#include <mach/rpm-smd.h>

#include "clock-local2.h"
#include "clock-pll.h"
#include "clock-rpm.h"
#include "clock-voter.h"
#include "clock.h"

/*
 * Drivers need to fill in the clock names and device names for the clocks
 * they need to control.
 */
static struct clk_lookup msm_clocks_8092[] = {
	CLK_DUMMY("core_clk",   BLSP1_UART_CLK, "msm_serial_hsl.0", OFF),
	CLK_DUMMY("iface_clk",  BLSP1_UART_CLK, "msm_serial_hsl.0", OFF),
	CLK_DUMMY("core_clk",   BLSP1_UART_CLK, "msm_serial_hsl.1", OFF),
	CLK_DUMMY("iface_clk",  BLSP1_UART_CLK, "msm_serial_hsl.1", OFF),
	CLK_DUMMY("core_clk",	SDC1_CLK,	"msm_sdcc.1", OFF),
	CLK_DUMMY("iface_clk",	SDC1_P_CLK,	"msm_sdcc.1", OFF),
	CLK_DUMMY("core_clk",	SDC2_CLK,	"msm_sdcc.2", OFF),
	CLK_DUMMY("iface_clk",	SDC2_P_CLK,	"msm_sdcc.2", OFF),
	CLK_DUMMY("dfab_clk",	DFAB_CLK,	"msm_sps", OFF),
	CLK_DUMMY("dma_bam_pclk",	DMA_BAM_P_CLK,	"msm_sps", OFF),
	CLK_DUMMY("",	usb30_master_clk_src.c,	"", OFF),
	CLK_DUMMY("",	tsif_ref_clk_src.c,	"", OFF),
	CLK_DUMMY("",	ce1_clk_src.c,	"", OFF),
	CLK_DUMMY("",	ce2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	ce3_clk_src.c,	"", OFF),
	CLK_DUMMY("",	geni_ser_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gmac_125m_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gmac_core_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gmac_sys_25m_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gp1_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gp2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gp3_clk_src.c,	"", OFF),
	CLK_DUMMY("",	pcie_aux_clk_src.c,	"", OFF),
	CLK_DUMMY("",	pcie_pipe_clk_src.c,	"", OFF),
	CLK_DUMMY("",	pdm2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	pwm_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sata_asic0_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sata_pmalive_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sata_rx_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sata_rx_oob_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sdcc1_apps_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sdcc2_apps_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb30_mock_utmi_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb_hs_system_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb_hs2_system_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb_hsic_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb_hsic_io_cal_clk_src.c,	"", OFF),
	CLK_DUMMY("",	usb_hsic_system_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gcc_bam_dma_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bcss_cfg_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bimc_gfx_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bimc_kpss_axi_mstr_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bimc_sysnoc_axi_mstr_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup1_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup1_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup2_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup2_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup3_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup3_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup4_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup4_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup5_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup5_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup6_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_qup6_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart1_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart2_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart3_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart4_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart5_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp1_uart6_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup1_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup1_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup2_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup2_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup3_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup3_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup4_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup4_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup5_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup5_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup6_i2c_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_qup6_spi_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart1_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart2_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart3_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart4_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart5_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_blsp2_uart6_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_boot_rom_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce1_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce1_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce1_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce2_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce2_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce2_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce3_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce3_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_ce3_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_xo_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_xo_div4_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_geni_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_geni_ser_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_125m_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_cfg_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_core_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_rx_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_sys_25m_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gmac_sys_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gp1_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gp2_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_gp3_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_klm_core_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_klm_s_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_lpass_q6_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sys_noc_lpass_mport_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sys_noc_lpass_sway_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_mmss_a5ss_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_mmss_bimc_gfx_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pcie_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pcie_axi_mstr_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pcie_cfg_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pcie_pipe_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pcie_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pdm2_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pdm_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_prng_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pwm_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_pwm_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_asic0_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_cfg_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_pmalive_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_rx_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sata_rx_oob_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sdcc1_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sdcc1_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sdcc2_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sdcc2_apps_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_spss_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_sys_noc_usb3_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb2a_phy_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb2b_phy_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb2c_phy_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb30_master_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb30_mock_utmi_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb30_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hs_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hs_system_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hs2_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hs2_system_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hsic_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hsic_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hsic_io_cal_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_usb_hsic_system_clk.c,	"", OFF),
	/* MMSS Clock Dummy */
	CLK_DUMMY("",	axi_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmpll0_pll_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmpll1_pll_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmpll2_pll_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmpll3_pll_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmpll6_pll_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vcodec0_clk_src.c,	"", OFF),
	CLK_DUMMY("",	extpclk_clk_src.c,	"", OFF),
	CLK_DUMMY("",	lvds_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mdp_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vbyone_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gfx3d_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vp_clk_src.c,	"", OFF),
	CLK_DUMMY("",	jpeg2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	hdmi_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vbyone_symbol_clk_src.c,	"", OFF),
	CLK_DUMMY("",	mmss_spdm_axi_div_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_spdm_gfx3d_div_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_spdm_jpeg2_div_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_spdm_mdp_div_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_spdm_vcodec0_div_clk.c,	"", OFF),
	CLK_DUMMY("",	afe_pixel_clk_src.c,	"", OFF),
	CLK_DUMMY("",	cfg_clk_src.c,	"", OFF),
	CLK_DUMMY("",	hdmi_bus_clk_src.c,	"", OFF),
	CLK_DUMMY("",	hdmi_rx_clk_src.c,	"", OFF),
	CLK_DUMMY("",	md_clk_src.c,	"", OFF),
	CLK_DUMMY("",	ttl_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vafe_ext_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vcap_vp_clk_src.c,	"", OFF),
	CLK_DUMMY("",	gproc_clk_src.c,	"", OFF),
	CLK_DUMMY("",	hdmc_frcf_clk_src.c,	"", OFF),
	CLK_DUMMY("",	kproc_clk_src.c,	"", OFF),
	CLK_DUMMY("",	maple_clk_src.c,	"", OFF),
	CLK_DUMMY("",	preproc_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sdmc_frcs_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sdme_frcf_clk_src.c,	"", OFF),
	CLK_DUMMY("",	sdme_vproc_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vdp_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vpu_bus_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vpu_frc_xin_clk_src.c,	"", OFF),
	CLK_DUMMY("",	vpu_vdp_xin_clk_src.c,	"", OFF),
	CLK_DUMMY("",	avsync_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	avsync_extpclk_clk.c,	"", OFF),
	CLK_DUMMY("",	avsync_lvds_clk.c,	"", OFF),
	CLK_DUMMY("",	avsync_vbyone_clk.c,	"", OFF),
	CLK_DUMMY("",	avsync_vp_clk.c,	"", OFF),
	CLK_DUMMY("",	camss_jpeg_jpeg2_clk.c,	"", OFF),
	CLK_DUMMY("",	camss_jpeg_jpeg_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	camss_jpeg_jpeg_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	camss_micro_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	camss_top_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_extpclk_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_hdmi_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_hdmi_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_lvds_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_mdp_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_mdp_lut_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_vbyone_clk.c,	"", OFF),
	CLK_DUMMY("",	mdss_vbyone_symbol_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_misc_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_mmssnoc_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_mmssnoc_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	mmss_s0_axi_clk.c,	"", OFF),
	CLK_DUMMY("core_clk",  ocmemgx_core_clk.c, "fdd00000.qcom,ocmem", OFF),
	CLK_DUMMY("iface_clk",	ocmemcx_ocmemnoc_clk.c,	"fdd00000.qcom.ocmem",
							 OFF),
	CLK_DUMMY("",	oxili_ocmemgx_clk.c,	"", OFF),
	CLK_DUMMY("",	oxili_gfx3d_clk.c,	"", OFF),
	CLK_DUMMY("",	oxilicx_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcss_mmss_ifdemod_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_afe_pixel_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_audio_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_cfg_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_hdmi_bus_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_hdmi_rx_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_md_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_ttl_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_ttl_debug_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_vafe_ext_clk.c,	"", OFF),
	CLK_DUMMY("",	vcap_vp_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_core0_vcodec_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_core1_vcodec_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_ocmemnoc_clk.c,	"", OFF),
	CLK_DUMMY("",	venus0_vcodec0_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_bus_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_cxo_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_frc_xin_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_gproc_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_hdmc_frcf_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_kproc_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_maple_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_preproc_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_sdmc_frcs_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_sdme_frcf_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_sdme_frcs_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_sdme_vproc_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_sleep_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_vdp_clk.c,	"", OFF),
	CLK_DUMMY("",	vpu_vdp_xin_clk.c,	"", OFF),
	CLK_DUMMY("iface_clk", NULL, "fda64000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fda64000.qcom,iommu", OFF),
	CLK_DUMMY("alt_core_clk", NULL, "fda64000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fd928000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fd928000.qcom,iommu", oFF),
	CLK_DUMMY("core_clk", NULL, "fdb10000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fdb10000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fdc84000.qcom,iommu", OFF),
	CLK_DUMMY("alt_core_clk", NULL, "fdc84000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fdc84000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fdee4000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fdee4000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fdfb6000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fdfb6000.qcom,iommu", OFF),
	CLK_DUMMY("alt_core_clk", NULL, "fdfb6000.qcom,iommu", OFF),
	CLK_DUMMY("iface_clk", NULL, "fc734000.qcom,iommu", OFF),
	CLK_DUMMY("core_clk", NULL, "fc734000.qcom,iommu", OFF),
	CLK_DUMMY("alt_core_clk", NULL, "fc734000.qcom,iommu", OFF),
	/* BCSS broadcast */
	CLK_DUMMY("",	bcc_dem_core_b_clk_src.c,	"", OFF),
	CLK_DUMMY("",	adc_01_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_adc_0_in_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_klm_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_lnb_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_tsc_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_tspp2_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_vbif_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bcss_ahb_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_atv_rxfe_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_atv_rxfe_resamp_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_core_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_core_div2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_core_x2_b_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_dem_core_x2_pre_cgf_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_tsc_ci_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_tsc_cicam_ts_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_tsc_par_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_tsc_ser_clk_src.c,	"", OFF),
	CLK_DUMMY("",	bcc_tspp2_clk_src.c,	"", OFF),
	CLK_DUMMY("",	dig_dem_core_b_div2_clk.c,	"", OFF),
	CLK_DUMMY("",	atv_x5_pre_cgc_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_albacore_cvbs_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_atv_x1_clk.c,	"", OFF),
	CLK_DUMMY("",	nidaq_out_clk.c,	"", OFF),
	CLK_DUMMY("",	gcc_bcss_axi_clk.c,	"", OFF),
	CLK_DUMMY("",	bcc_lnb_core_clk.c,	"", OFF),

	/* USB */
	CLK_DUMMY("core_clk", NULL, "msm_otg", OFF),
	CLK_DUMMY("iface_clk", NULL, "msm_otg", OFF),
	CLK_DUMMY("xo", NULL, "msm_otg", OFF),
};

struct clock_init_data mpq8092_clock_init_data __initdata = {
	.table = msm_clocks_8092,
	.size = ARRAY_SIZE(msm_clocks_8092),
};
