// SPDX-License-Identifier: GPL-2.0-only
//
// aw88166.h --  ALSA SoC AW88166 codec support
//
// Copyright (c) 2025 AWINIC Technology CO., LTD
//
// Author: Weidong Wang <wangweidong.a@awinic.com>
//

#ifndef __AW88166_H__
#define __AW88166_H__

/* registers list */
#define AW88166_ID_REG			(0x00)
#define AW88166_SYSST_REG		(0x01)
#define AW88166_SYSINT_REG		(0x02)
#define AW88166_SYSINTM_REG		(0x03)
#define AW88166_SYSCTRL_REG		(0x04)
#define AW88166_SYSCTRL2_REG		(0x05)
#define AW88166_I2SCTRL1_REG		(0x06)
#define AW88166_I2SCTRL2_REG		(0x07)
#define AW88166_I2SCTRL3_REG		(0x08)
#define AW88166_DACCFG1_REG		(0x09)
#define AW88166_DACCFG2_REG		(0x0A)
#define AW88166_DACCFG3_REG		(0x0B)
#define AW88166_DACCFG4_REG		(0x0C)
#define AW88166_DACCFG5_REG		(0x0D)
#define AW88166_DACCFG6_REG		(0x0E)
#define AW88166_DACCFG7_REG		(0x0F)
#define AW88166_MPDCFG1_REG		(0x10)
#define AW88166_MPDCFG2_REG		(0x11)
#define AW88166_MPDCFG3_REG		(0x12)
#define AW88166_MPDCFG4_REG		(0x13)
#define AW88166_PWMCTRL1_REG		(0x14)
#define AW88166_PWMCTRL2_REG		(0x15)
#define AW88166_PWMCTRL3_REG		(0x16)
#define AW88166_I2SCFG1_REG		(0x17)
#define AW88166_DBGCTRL_REG		(0x18)
#define AW88166_HAGCST_REG		(0x20)
#define AW88166_VBAT_REG		(0x21)
#define AW88166_TEMP_REG		(0x22)
#define AW88166_PVDD_REG		(0x23)
#define AW88166_ISNDAT_REG		(0x24)
#define AW88166_I2SINT_REG		(0x25)
#define AW88166_I2SCAPCNT_REG		(0x26)
#define AW88166_ANASTA1_REG		(0x27)
#define AW88166_ANASTA2_REG		(0x28)
#define AW88166_ANASTA3_REG		(0x29)
#define AW88166_TESTDET_REG		(0x2A)
#define AW88166_TESTIN_REG		(0x38)
#define AW88166_TESTOUT_REG		(0x39)
#define AW88166_MEMTEST_REG		(0x3A)
#define AW88166_DSPMADD_REG		(0x40)
#define AW88166_DSPMDAT_REG		(0x41)
#define AW88166_WDT_REG		(0x42)
#define AW88166_ACR1_REG		(0x43)
#define AW88166_ACR2_REG		(0x44)
#define AW88166_ASR1_REG		(0x45)
#define AW88166_ASR2_REG		(0x46)
#define AW88166_DSPCFG_REG		(0x47)
#define AW88166_ASR3_REG		(0x48)
#define AW88166_ASR4_REG		(0x49)
#define AW88166_DSPVCALB_REG		(0x4A)
#define AW88166_CRCCTRL_REG		(0x4B)
#define AW88166_DSPDBG1_REG		(0x4C)
#define AW88166_DSPDBG2_REG		(0x4D)
#define AW88166_DSPDBG3_REG		(0x4E)
#define AW88166_ISNCTRL1_REG		(0x50)
#define AW88166_PLLCTRL1_REG		(0x51)
#define AW88166_PLLCTRL2_REG		(0x52)
#define AW88166_PLLCTRL3_REG		(0x53)
#define AW88166_CDACTRL1_REG		(0x54)
#define AW88166_CDACTRL2_REG		(0x55)
#define AW88166_CDACTRL3_REG		(0x56)
#define AW88166_SADCCTRL1_REG		(0x57)
#define AW88166_SADCCTRL2_REG		(0x58)
#define AW88166_BOPCTRL1_REG		(0x59)
#define AW88166_BOPCTRL2_REG		(0x5A)
#define AW88166_BOPCTRL3_REG		(0x5B)
#define AW88166_BOPCTRL4_REG		(0x5C)
#define AW88166_BOPCTRL5_REG		(0x5D)
#define AW88166_BOPCTRL6_REG		(0x5E)
#define AW88166_BOPCTRL7_REG		(0x5F)
#define AW88166_BSTCTRL1_REG		(0x60)
#define AW88166_BSTCTRL2_REG		(0x61)
#define AW88166_BSTCTRL3_REG		(0x62)
#define AW88166_BSTCTRL4_REG		(0x63)
#define AW88166_BSTCTRL5_REG		(0x64)
#define AW88166_BSTCTRL6_REG		(0x65)
#define AW88166_DSMCFG1_REG		(0x66)
#define AW88166_DSMCFG2_REG		(0x67)
#define AW88166_DSMCFG3_REG		(0x68)
#define AW88166_DSMCFG4_REG		(0x69)
#define AW88166_DSMCFG5_REG		(0x6A)
#define AW88166_DSMCFG6_REG		(0x6B)
#define AW88166_DSMCFG7_REG		(0x6C)
#define AW88166_DSMCFG8_REG		(0x6D)
#define AW88166_TESTCTRL1_REG		(0x70)
#define AW88166_TESTCTRL2_REG		(0x71)
#define AW88166_EFCTRL1_REG		(0x72)
#define AW88166_EFCTRL2_REG		(0x73)
#define AW88166_EFWH_REG		(0x74)
#define AW88166_EFWM2_REG		(0x75)
#define AW88166_EFWM1_REG		(0x76)
#define AW88166_EFRH_REG		(0x77)
#define AW88166_EFRM2_REG		(0x78)
#define AW88166_EFRM1_REG		(0x79)
#define AW88166_EFRL_REG		(0x7A)
#define AW88166_TM_REG			(0x7C)
#define AW88166_TM2_REG		(0x7D)

#define AW88166_REG_MAX		(0x7E)
#define AW88166_MUTE_VOL		(1023)

#define AW88166_DSP_CFG_ADDR		(0x9B00)
#define AW88166_DSP_REG_CFG_ADPZ_RA	(0x9B68)
#define AW88166_DSP_FW_ADDR		(0x8980)
#define AW88166_DSP_ROM_CHECK_ADDR	(0x1F40)

#define AW88166_CALI_RE_HBITS_MASK	(~(0xFFFF0000))
#define AW88166_CALI_RE_HBITS_SHIFT	(16)

#define AW88166_CALI_RE_LBITS_MASK	(~(0xFFFF))
#define AW88166_CALI_RE_LBITS_SHIFT	(0)

#define AW88166_I2STXEN_START_BIT	(9)
#define AW88166_I2STXEN_BITS_LEN	(1)
#define AW88166_I2STXEN_MASK		\
	(~(((1<<AW88166_I2STXEN_BITS_LEN)-1) << AW88166_I2STXEN_START_BIT))

#define AW88166_I2STXEN_DISABLE	(0)
#define AW88166_I2STXEN_DISABLE_VALUE	\
	(AW88166_I2STXEN_DISABLE << AW88166_I2STXEN_START_BIT)

#define AW88166_I2STXEN_ENABLE		(1)
#define AW88166_I2STXEN_ENABLE_VALUE	\
	(AW88166_I2STXEN_ENABLE << AW88166_I2STXEN_START_BIT)

#define AW88166_VOL_START_BIT		(0)
#define AW88166_VOL_BITS_LEN		(10)
#define AW88166_VOL_MASK		\
	(~(((1<<AW88166_VOL_BITS_LEN)-1) << AW88166_VOL_START_BIT))

#define AW88166_PWDN_START_BIT		(0)
#define AW88166_PWDN_BITS_LEN		(1)
#define AW88166_PWDN_MASK		\
	(~(((1<<AW88166_PWDN_BITS_LEN)-1) << AW88166_PWDN_START_BIT))

#define AW88166_PWDN_POWER_DOWN	(1)
#define AW88166_PWDN_POWER_DOWN_VALUE	\
	(AW88166_PWDN_POWER_DOWN << AW88166_PWDN_START_BIT)

#define AW88166_PWDN_WORKING		(0)
#define AW88166_PWDN_WORKING_VALUE	\
	(AW88166_PWDN_WORKING << AW88166_PWDN_START_BIT)

#define AW88166_DSPBY_START_BIT	(2)
#define AW88166_DSPBY_BITS_LEN		(1)
#define AW88166_DSPBY_MASK		\
	(~(((1<<AW88166_DSPBY_BITS_LEN)-1) << AW88166_DSPBY_START_BIT))

#define AW88166_DSPBY_WORKING		(0)
#define AW88166_DSPBY_WORKING_VALUE	\
	(AW88166_DSPBY_WORKING << AW88166_DSPBY_START_BIT)

#define AW88166_DSPBY_BYPASS		(1)
#define AW88166_DSPBY_BYPASS_VALUE	\
	(AW88166_DSPBY_BYPASS << AW88166_DSPBY_START_BIT)

#define AW88166_MEM_CLKSEL_START_BIT	(3)
#define AW88166_MEM_CLKSEL_BITS_LEN	(1)
#define AW88166_MEM_CLKSEL_MASK		\
	(~(((1<<AW88166_MEM_CLKSEL_BITS_LEN)-1) << AW88166_MEM_CLKSEL_START_BIT))

#define AW88166_MEM_CLKSEL_OSCCLK	(0)
#define AW88166_MEM_CLKSEL_OSCCLK_VALUE	\
	(AW88166_MEM_CLKSEL_OSCCLK << AW88166_MEM_CLKSEL_START_BIT)

#define AW88166_MEM_CLKSEL_DAPHCLK	(1)
#define AW88166_MEM_CLKSEL_DAPHCLK_VALUE	\
	(AW88166_MEM_CLKSEL_DAPHCLK << AW88166_MEM_CLKSEL_START_BIT)

#define AW88166_DITHER_EN_START_BIT	(15)
#define AW88166_DITHER_EN_BITS_LEN	(1)
#define AW88166_DITHER_EN_MASK		 \
	(~(((1<<AW88166_DITHER_EN_BITS_LEN)-1) << AW88166_DITHER_EN_START_BIT))

#define AW88166_DITHER_EN_DISABLE	(0)
#define AW88166_DITHER_EN_DISABLE_VALUE	\
	(AW88166_DITHER_EN_DISABLE << AW88166_DITHER_EN_START_BIT)

#define AW88166_DITHER_EN_ENABLE	(1)
#define AW88166_DITHER_EN_ENABLE_VALUE	\
	(AW88166_DITHER_EN_ENABLE << AW88166_DITHER_EN_START_BIT)

#define AW88166_HMUTE_START_BIT	(8)
#define AW88166_HMUTE_BITS_LEN		(1)
#define AW88166_HMUTE_MASK		\
	(~(((1<<AW88166_HMUTE_BITS_LEN)-1) << AW88166_HMUTE_START_BIT))

#define AW88166_HMUTE_DISABLE		(0)
#define AW88166_HMUTE_DISABLE_VALUE	\
	(AW88166_HMUTE_DISABLE << AW88166_HMUTE_START_BIT)

#define AW88166_HMUTE_ENABLE		(1)
#define AW88166_HMUTE_ENABLE_VALUE	\
	(AW88166_HMUTE_ENABLE << AW88166_HMUTE_START_BIT)

#define AW88166_EF_DBMD_START_BIT	(2)
#define AW88166_EF_DBMD_BITS_LEN	(1)
#define AW88166_EF_DBMD_MASK		\
	(~(((1<<AW88166_EF_DBMD_BITS_LEN)-1) << AW88166_EF_DBMD_START_BIT))

#define AW88166_EF_DBMD_OR		(1)
#define AW88166_EF_DBMD_OR_VALUE	\
	(AW88166_EF_DBMD_OR << AW88166_EF_DBMD_START_BIT)

#define AW88166_CLKI_START_BIT		(4)
#define AW88166_NOCLKI_START_BIT	(5)
#define AW88166_PLLI_START_BIT		(0)
#define AW88166_PLLI_INT_VALUE		(1)
#define AW88166_PLLI_INT_INTERRUPT \
	(AW88166_PLLI_INT_VALUE << AW88166_PLLI_START_BIT)

#define AW88166_CLKI_INT_VALUE		(1)
#define AW88166_CLKI_INT_INTERRUPT \
	(AW88166_CLKI_INT_VALUE << AW88166_CLKI_START_BIT)

#define AW88166_NOCLKI_INT_VALUE	(1)
#define AW88166_NOCLKI_INT_INTERRUPT \
	(AW88166_NOCLKI_INT_VALUE << AW88166_NOCLKI_START_BIT)

#define AW88166_BIT_SYSINT_CHECK \
		(AW88166_PLLI_INT_INTERRUPT | \
		AW88166_CLKI_INT_INTERRUPT | \
		AW88166_NOCLKI_INT_INTERRUPT)

#define AW88166_CRC_CHECK_START_BIT	(12)
#define AW88166_CRC_CHECK_BITS_LEN	(3)
#define AW88166_CRC_CHECK_BITS_MASK	\
	(~(((1<<AW88166_CRC_CHECK_BITS_LEN)-1) << AW88166_CRC_CHECK_START_BIT))

#define AW88166_RCV_MODE_RECEIVER	(1)
#define AW88166_RCV_MODE_RECEIVER_VALUE	\
	(AW88166_RCV_MODE_RECEIVER << AW88166_RCV_MODE_START_BIT)

#define AW88166_AMPPD_START_BIT	(1)
#define AW88166_AMPPD_BITS_LEN		(1)
#define AW88166_AMPPD_MASK		\
	(~(((1<<AW88166_AMPPD_BITS_LEN)-1) << AW88166_AMPPD_START_BIT))

#define AW88166_AMPPD_WORKING		(0)
#define AW88166_AMPPD_WORKING_VALUE	\
	(AW88166_AMPPD_WORKING << AW88166_AMPPD_START_BIT)

#define AW88166_AMPPD_POWER_DOWN	(1)
#define AW88166_AMPPD_POWER_DOWN_VALUE	\
	(AW88166_AMPPD_POWER_DOWN << AW88166_AMPPD_START_BIT)

#define AW88166_RAM_CG_BYP_START_BIT	(0)
#define AW88166_RAM_CG_BYP_BITS_LEN	(1)
#define AW88166_RAM_CG_BYP_MASK		\
	(~(((1<<AW88166_RAM_CG_BYP_BITS_LEN)-1) << AW88166_RAM_CG_BYP_START_BIT))

#define AW88166_RAM_CG_BYP_WORK	(0)
#define AW88166_RAM_CG_BYP_WORK_VALUE	\
	(AW88166_RAM_CG_BYP_WORK << AW88166_RAM_CG_BYP_START_BIT)

#define AW88166_RAM_CG_BYP_BYPASS	(1)
#define AW88166_RAM_CG_BYP_BYPASS_VALUE	\
	(AW88166_RAM_CG_BYP_BYPASS << AW88166_RAM_CG_BYP_START_BIT)

#define AW88166_CRC_END_ADDR_START_BIT	(0)
#define AW88166_CRC_END_ADDR_BITS_LEN	(12)
#define AW88166_CRC_END_ADDR_MASK	\
	(~(((1<<AW88166_CRC_END_ADDR_BITS_LEN)-1) << AW88166_CRC_END_ADDR_START_BIT))

#define AW88166_CRC_CODE_EN_START_BIT	(13)
#define AW88166_CRC_CODE_EN_BITS_LEN	(1)
#define AW88166_CRC_CODE_EN_MASK	\
	(~(((1<<AW88166_CRC_CODE_EN_BITS_LEN)-1) << AW88166_CRC_CODE_EN_START_BIT))

#define AW88166_CRC_CODE_EN_DISABLE	(0)
#define AW88166_CRC_CODE_EN_DISABLE_VALUE	\
	(AW88166_CRC_CODE_EN_DISABLE << AW88166_CRC_CODE_EN_START_BIT)

#define AW88166_CRC_CODE_EN_ENABLE	(1)
#define AW88166_CRC_CODE_EN_ENABLE_VALUE	\
	(AW88166_CRC_CODE_EN_ENABLE << AW88166_CRC_CODE_EN_START_BIT)

#define AW88166_CRC_CFG_EN_START_BIT	(12)
#define AW88166_CRC_CFG_EN_BITS_LEN	(1)
#define AW88166_CRC_CFG_EN_MASK		\
	(~(((1<<AW88166_CRC_CFG_EN_BITS_LEN)-1) << AW88166_CRC_CFG_EN_START_BIT))

#define AW88166_CRC_CFG_EN_DISABLE	(0)
#define AW88166_CRC_CFG_EN_DISABLE_VALUE	\
	(AW88166_CRC_CFG_EN_DISABLE << AW88166_CRC_CFG_EN_START_BIT)

#define AW88166_CRC_CFG_EN_ENABLE	(1)
#define AW88166_CRC_CFG_EN_ENABLE_VALUE	\
	(AW88166_CRC_CFG_EN_ENABLE << AW88166_CRC_CFG_EN_START_BIT)

#define AW88166_OCDS_START_BIT		(3)
#define AW88166_OCDS_OC		(1)
#define AW88166_OCDS_OC_VALUE		\
	(AW88166_OCDS_OC << AW88166_OCDS_START_BIT)

#define AW88166_NOCLKS_START_BIT	(5)
#define AW88166_NOCLKS_NO_CLOCK	(1)
#define AW88166_NOCLKS_NO_CLOCK_VALUE	\
	(AW88166_NOCLKS_NO_CLOCK << AW88166_NOCLKS_START_BIT)

#define AW88166_SWS_START_BIT		(8)
#define AW88166_SWS_SWITCHING		(1)
#define AW88166_SWS_SWITCHING_VALUE	\
	(AW88166_SWS_SWITCHING << AW88166_SWS_START_BIT)

#define AW88166_BSTS_START_BIT		(9)
#define AW88166_BSTS_FINISHED		(1)
#define AW88166_BSTS_FINISHED_VALUE	\
	(AW88166_BSTS_FINISHED << AW88166_BSTS_START_BIT)

#define AW88166_UVLS_START_BIT		(14)
#define AW88166_UVLS_NORMAL		(0)
#define AW88166_UVLS_NORMAL_VALUE	\
	(AW88166_UVLS_NORMAL << AW88166_UVLS_START_BIT)

#define AW88166_BSTOCS_START_BIT	(11)
#define AW88166_BSTOCS_OVER_CURRENT	(1)
#define AW88166_BSTOCS_OVER_CURRENT_VALUE	\
	(AW88166_BSTOCS_OVER_CURRENT << AW88166_BSTOCS_START_BIT)

#define AW88166_OTHS_START_BIT		(1)
#define AW88166_OTHS_OT		(1)
#define AW88166_OTHS_OT_VALUE		\
	(AW88166_OTHS_OT << AW88166_OTHS_START_BIT)

#define AW88166_PLLS_START_BIT		(0)
#define AW88166_PLLS_LOCKED		(1)
#define AW88166_PLLS_LOCKED_VALUE	\
	(AW88166_PLLS_LOCKED << AW88166_PLLS_START_BIT)

#define AW88166_CLKS_START_BIT		(4)
#define AW88166_CLKS_STABLE		(1)
#define AW88166_CLKS_STABLE_VALUE	\
	(AW88166_CLKS_STABLE << AW88166_CLKS_START_BIT)

#define AW88166_BIT_PLL_CHECK \
		(AW88166_CLKS_STABLE_VALUE | \
		AW88166_PLLS_LOCKED_VALUE)

#define AW88166_BIT_SYSST_CHECK_MASK \
		(~(AW88166_UVLS_NORMAL_VALUE | \
		AW88166_BSTOCS_OVER_CURRENT_VALUE | \
		AW88166_BSTS_FINISHED_VALUE | \
		AW88166_SWS_SWITCHING_VALUE | \
		AW88166_NOCLKS_NO_CLOCK_VALUE | \
		AW88166_CLKS_STABLE_VALUE | \
		AW88166_OCDS_OC_VALUE | \
		AW88166_OTHS_OT_VALUE | \
		AW88166_PLLS_LOCKED_VALUE))

#define AW88166_BIT_SYSST_NOSWS_CHECK \
		(AW88166_BSTS_FINISHED_VALUE | \
		AW88166_CLKS_STABLE_VALUE | \
		AW88166_PLLS_LOCKED_VALUE)

#define AW88166_BIT_SYSST_SWS_CHECK \
		(AW88166_BSTS_FINISHED_VALUE | \
		AW88166_CLKS_STABLE_VALUE | \
		AW88166_PLLS_LOCKED_VALUE | \
		AW88166_SWS_SWITCHING_VALUE)

#define AW88166_CCO_MUX_START_BIT	(14)
#define AW88166_CCO_MUX_BITS_LEN	(1)
#define AW88166_CCO_MUX_MASK		\
	(~(((1<<AW88166_CCO_MUX_BITS_LEN)-1) << AW88166_CCO_MUX_START_BIT))

#define AW88166_CCO_MUX_DIVIDED	(0)
#define AW88166_CCO_MUX_DIVIDED_VALUE	\
	(AW88166_CCO_MUX_DIVIDED << AW88166_CCO_MUX_START_BIT)

#define AW88166_CCO_MUX_BYPASS		(1)
#define AW88166_CCO_MUX_BYPASS_VALUE	\
	(AW88166_CCO_MUX_BYPASS << AW88166_CCO_MUX_START_BIT)

#define AW88166_NOISE_GATE_EN_START_BIT	(13)
#define AW88166_NOISE_GATE_EN_BITS_LEN		(1)
#define AW88166_NOISE_GATE_EN_MASK		\
	(~(((1<<AW88166_NOISE_GATE_EN_BITS_LEN)-1) << AW88166_NOISE_GATE_EN_START_BIT))

#define AW88166_WDT_CNT_START_BIT		(0)
#define AW88166_WDT_CNT_BITS_LEN		(8)
#define AW88166_WDT_CNT_MASK			\
	(~(((1<<AW88166_WDT_CNT_BITS_LEN)-1) << AW88166_WDT_CNT_START_BIT))

#define AW88166_EF_ISN_GESLP_START_BIT		(0)
#define AW88166_EF_ISN_GESLP_BITS_LEN		(10)
#define AW88166_EF_ISN_GESLP_MASK		\
	(~(((1<<AW88166_EF_ISN_GESLP_BITS_LEN)-1) << AW88166_EF_ISN_GESLP_START_BIT))
#define AW88166_EF_ISN_GESLP_SHIFT		(0)

#define AW88166_EF_VSN_GESLP_START_BIT		(10)
#define AW88166_EF_VSN_GESLP_BITS_LEN		(6)
#define AW88166_EF_VSN_GESLP_MASK		\
	(~(((1<<AW88166_EF_VSN_GESLP_BITS_LEN)-1) << AW88166_EF_VSN_GESLP_START_BIT))
#define AW88166_EF_VSN_GESLP_SHIFT		(10)

#define AW88166_EF_VSN_H3BITS_START_BIT	(13)
#define AW88166_EF_VSN_H3BITS_BITS_LEN		(3)
#define AW88166_EF_VSN_H3BITS_MASK	\
	(~(((1<<AW88166_EF_VSN_H3BITS_BITS_LEN)-1) << AW88166_EF_VSN_H3BITS_START_BIT))
#define AW88166_EF_VSN_H3BITS_SHIFT		(10)
#define AW88166_EF_VSN_H3BITS_SIGN_MASK	(0x7)

#define AW88166_EF_ISN_H5BITS_START_BIT	(8)
#define AW88166_EF_ISN_H5BITS_BITS_LEN		(5)
#define AW88166_EF_ISN_H5BITS_MASK		\
	(~(((1<<AW88166_EF_ISN_H5BITS_BITS_LEN)-1) << AW88166_EF_ISN_H5BITS_START_BIT))
#define AW88166_EF_ISN_H5BITS_SIGN_MASK	(0x1F)
#define AW88166_EF_ISN_H5BITS_SHIFT		(3)

#define AW88166_VSCAL_FACTOR			(65300)
#define AW88166_ISCAL_FACTOR			(34667)
#define AW88166_CABL_BASE_VALUE		(1000)
#define AW88166_VCALK_SIGN_MASK		(~(1 << 5))
#define AW88166_VCALK_NEG_MASK			(0xFFE0)
#define AW88166_ICALK_SIGN_MASK		(~(1 << 9))
#define AW88166_ICALK_NEG_MASK			(0xFE00)
#define AW88166_ICABLK_FACTOR			(1)
#define AW88166_VCABLK_FACTOR			(2)
#define AW88166_VCALB_ADJ_FACTOR		(12)
#define AW88166_VCALB_ACCURACY			(1 << 12)
#define AW88166_DSP_RE_SHIFT			(12)
#define AW88166_CALI_RE_MAX			(15000)
#define AW88166_CALI_RE_MIN			(4000)
#define AW88166_VOLUME_STEP_DB			(64)
#define AW88166_VOL_DEFAULT_VALUE		(0)
#define AW88166_DSP_RE_TO_SHOW_RE(re, shift)	(((re) * (1000)) >> (shift))
#define AW88166_SHOW_RE_TO_DSP_RE(re, shift)	(((re) << shift) / (1000))

#define AW88166_DSP_ODD_NUM_BIT_TEST		(0x5555)
#define AW88166_DSP_ROM_CHECK_DATA		(0xFF99)

#define AW88166_DEV_DEFAULT_CH			(0)
#define AW88166_DEV_DSP_CHECK_MAX		(5)
#define AW88166_MAX_RAM_WRITE_BYTE_SIZE	(128)
#define AW_FW_ADDR_LEN				(4)
#define AW88166_CRC_CHECK_PASS_VAL		(0x4)
#define AW88166_CRC_CFG_BASE_ADDR		(0xD80)
#define AW88166_CRC_FW_BASE_ADDR		(0x4C0)
#define AW88166_DEV_SYSST_CHECK_MAX		(10)
#define AW88166_START_RETRIES			(5)
#define AW88166_START_WORK_DELAY_MS		(0)
#define FADE_TIME_MAX				100000
#define FADE_TIME_MIN				0
#define AW88166_CHIP_ID			(0x2066)
#define AW88166_I2C_NAME			"aw88166"
#define AW88166_ACF_FILE			"aw88166_acf.bin"

#define AW88166_RATES (SNDRV_PCM_RATE_8000_48000 | \
			SNDRV_PCM_RATE_96000)
#define AW88166_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | \
			SNDRV_PCM_FMTBIT_S24_LE | \
			SNDRV_PCM_FMTBIT_S32_LE)

#define AW88166_PROFILE_EXT(xname, profile_info, profile_get, profile_set) \
{ \
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER, \
	.name = xname, \
	.info = profile_info, \
	.get = profile_get, \
	.put = profile_set, \
}

enum {
	AW_EF_AND_CHECK = 0,
	AW_EF_OR_CHECK,
};

enum {
	AW88166_DSP_FW_UPDATE_OFF = 0,
	AW88166_DSP_FW_UPDATE_ON = 1,
};

enum {
	AW88166_FORCE_UPDATE_OFF = 0,
	AW88166_FORCE_UPDATE_ON = 1,
};

enum {
	AW88166_1000_US = 1000,
	AW88166_2000_US = 2000,
	AW88166_3000_US = 3000,
	AW88166_4000_US = 4000,
};

enum AW88166_DEV_STATUS {
	AW88166_DEV_PW_OFF = 0,
	AW88166_DEV_PW_ON,
};

enum AW88166_DEV_FW_STATUS {
	AW88166_DEV_FW_FAILED = 0,
	AW88166_DEV_FW_OK,
};

enum AW88166_DEV_MEMCLK {
	AW88166_DEV_MEMCLK_OSC = 0,
	AW88166_DEV_MEMCLK_PLL = 1,
};

enum AW88166_DEV_DSP_CFG {
	AW88166_DEV_DSP_WORK = 0,
	AW88166_DEV_DSP_BYPASS = 1,
};

enum {
	AW88166_DSP_16_DATA = 0,
	AW88166_DSP_32_DATA = 1,
};

enum {
	AW88166_SYNC_START = 0,
	AW88166_ASYNC_START,
};

enum {
	AW88166_RECORD_SEC_DATA = 0,
	AW88166_RECOVERY_SEC_DATA = 1,
};

#endif
