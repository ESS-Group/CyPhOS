/*
 * ExynosClockManagement.h
 *
 *  Created on: 27.07.2015
 *      Author: hendrik
 */

#ifndef DRIVER_EXYNOSCLOCKMANAGEMENT_H_
#define DRIVER_EXYNOSCLOCKMANAGEMENT_H_

#include "../../driver/ARMClockManagement.h"

/* Should be 24MHz */
#define FREQ_INPUT 24000000

/* CMU base address (s. exynos TRM p. 405)*/
#define EXYNOS_4_CMU_BASE 0x10030000

#define EXYNOS_4_CMU_LEFTBUS EXYNOS_4_CMU_BASE + 0x4000
#define EXYNOS_4_CMU_RIGHTBUS EXYNOS_4_CMU_BASE + 0x8000
#define EXYNOS_4_CMU_TOP EXYNOS_4_CMU_BASE + 0xC000
#define EXYNOS_4_CMU_DMC EXYNOS_4_CMU_BASE + 0x10000
#define EXYNOS_4_CMU_CPU EXYNOS_4_CMU_BASE + 0x14000
#define EXYNOS_4_CMU_ISP EXYNOS_4_CMU_BASE + 0x18000


/* APLL registers */
#define APLL_LOCK EXYNOS_4_CMU_CPU + 0x0
#define APLL_CON0 EXYNOS_4_CMU_CPU + 0x100
#define APLL_CON1 EXYNOS_4_CMU_CPU + 0x104
#define CLK_SRC_CPU EXYNOS_4_CMU_CPU + 0x200
#define CLK_MUX_STAT_CPU EXYNOS_4_CMU_CPU + 0x400
#define CLK_DIV_CPU0 EXYNOS_4_CMU_CPU + 0x500
#define CLK_DIV_CPU1 EXYNOS_4_CMU_CPU + 0x504
#define CLK_DIV_STAT_CPU0 EXYNOS_4_CMU_CPU + 0x600
#define CLK_DIV_STAT_CPU1 EXYNOS_4_CMU_CPU + 0x604
#define CLK_GATE_IP_CPU EXYNOS_4_CMU_CPU + 0x900
#define CLKOUT_CMU_CPU EXYNOS_4_CMU_CPU + 0xA00
#define ARMCLK_STOPCTRL EXYNOS_4_CMU_CPU + 0x1000
#define ATCLK_STOPCTRL EXYNOS_4_CMU_CPU + 0x1004
#define PWR_CTRL EXYNOS_4_CMU_CPU + 0x1020
#define PWR_CTRL2 EXYNOS_4_CMU_CPU + 0x1024
#define L2_STATUS EXYNOS_4_CMU_CPU + 0x1400
#define CPU_STATUS EXYNOS_4_CMU_CPU + 0x1410
#define PTM_STATUS EXYNOS_4_CMU_CPU + 0x1420

/* MPLL registers */
#define MPLL_LOCK EXYNOS_4_CMU_DMC + 0x8
#define MPLL_CON0 EXYNOS_4_CMU_DMC + 0x108
#define MPLL_CON1 EXYNOS_4_CMU_DMC + 0x10C
#define CLK_SRC_DMC EXYNOS_4_CMU_DMC + 0x200


/* CMU_TOP registers (peripheral clocks, uart e.g.) */
#define EPLL_LOCK EXYNOS_4_CMU_TOP + 0x10
#define VPLL_LOCK EXYNOS_4_CMU_TOP + 0x20

#define EPLL_CON0 EXYNOS_4_CMU_TOP + 0x110
#define EPLL_CON1 EXYNOS_4_CMU_TOP + 0x114
#define EPLL_CON2 EXYNOS_4_CMU_TOP + 0x118

#define VPLL_CON0 EXYNOS_4_CMU_TOP + 0x120
#define VPLL_CON1 EXYNOS_4_CMU_TOP + 0x124
#define VPLL_CON2 EXYNOS_4_CMU_TOP + 0x128

#define CLK_SRC_TOP0 EXYNOS_4_CMU_TOP + 0x210
#define CLK_SRC_TOP1 EXYNOS_4_CMU_TOP + 0x214

#define CLK_SRC_CAM0 EXYNOS_4_CMU_TOP + 0x220
#define CLK_SRC_TV EXYNOS_4_CMU_TOP + 0x224
#define CLK_SRC_MFC EXYNOS_4_CMU_TOP + 0x228
#define CLK_SRC_G3D EXYNOS_4_CMU_TOP + 0x22C
#define CLK_SRC_LCD0 EXYNOS_4_CMU_TOP + 0x234
#define CLK_SRC_ISP EXYNOS_4_CMU_TOP + 0x238
#define CLK_SRC_MAUDIO EXYNOS_4_CMU_TOP + 0x23C
#define CLK_SRC_FSYS EXYNOS_4_CMU_TOP + 0x240
#define CLK_SRC_PERIL0 EXYNOS_4_CMU_TOP + 0x250
#define CLK_SRC_PERIL1 EXYNOS_4_CMU_TOP + 0x254
#define CLK_SRC_CAM1 EXYNOS_4_CMU_TOP + 0x258
#define CLK_SRC_MASK_CAM0 EXYNOS_4_CMU_TOP + 0x320
#define CLK_SRC_MASK_TV EXYNOS_4_CMU_TOP + 0x324
#define CLK_SRC_MASK_LCD EXYNOS_4_CMU_TOP + 0x334
#define CLK_SRC_MASK_ISP EXYNOS_4_CMU_TOP + 0x338
#define CLK_SRC_MASK_MAUDIO EXYNOS_4_CMU_TOP + 0x33C
#define CLK_SRC_MASK_FSYS EXYNOS_4_CMU_TOP + 0x340
#define CLK_SRC_MASK_PERIL0 EXYNOS_4_CMU_TOP + 0x350
#define CLK_SRC_MASK_PERIL1 EXYNOS_4_CMU_TOP + 0x354

#define CLK_MUX_STAT_TOP0 EXYNOS_4_CMU_TOP + 0x410
#define CLK_MUX_STAT_TOP1 EXYNOS_4_CMU_TOP + 0x414
#define CLK_MUX_STAT_MFC EXYNOS_4_CMU_TOP + 0x428
#define CLK_MUX_STAT_G3D EXYNOS_4_CMU_TOP + 0x42C
#define CLK_MUX_STAT_CAM1 EXYNOS_4_CMU_TOP + 0x458

#define CLK_DIV_TOP EXYNOS_4_CMU_TOP + 0x510
#define CLK_DIV_CAM0 EXYNOS_4_CMU_TOP + 0x520
#define CLK_DIV_TV EXYNOS_4_CMU_TOP + 0x524
#define CLK_DIV_MFC EXYNOS_4_CMU_TOP + 0x528
#define CLK_DIV_G3D EXYNOS_4_CMU_TOP + 0x52C
#define CLK_DIV_LCD EXYNOS_4_CMU_TOP + 0x534
#define CLK_DIV_ISP EXYNOS_4_CMU_TOP + 0x538
#define CLK_DIV_MAUDIO EXYNOS_4_CMU_TOP + 0x53C
#define CLK_DIV_FSYS0 EXYNOS_4_CMU_TOP + 0x540
#define CLK_DIV_FSYS1 EXYNOS_4_CMU_TOP + 0x544
#define CLK_DIV_FSYS2 EXYNOS_4_CMU_TOP + 0x548
#define CLK_DIV_FSYS3 EXYNOS_4_CMU_TOP + 0x54C
#define CLK_DIV_PERIL0 EXYNOS_4_CMU_TOP + 0x550
#define CLK_DIV_PERIL1 EXYNOS_4_CMU_TOP + 0x554
#define CLK_DIV_PERIL2 EXYNOS_4_CMU_TOP + 0x558
#define CLK_DIV_PERIL3 EXYNOS_4_CMU_TOP + 0x55C
#define CLK_DIV_PERIL4 EXYNOS_4_CMU_TOP + 0x560
#define CLK_DIV_PERIL5 EXYNOS_4_CMU_TOP + 0x564
#define CLK_DIV_CAM1 EXYNOS_4_CMU_TOP + 0x568
#define CLKDIV2_RATIO EXYNOS_4_CMU_TOP + 0x580

#define CLK_DIV_STAT_TOP EXYNOS_4_CMU_TOP + 0x610
#define CLK_DIV_STAT_CAM0 EXYNOS_4_CMU_TOP + 0x620
#define CLK_DIV_STAT_TV EXYNOS_4_CMU_TOP + 0x624
#define CLK_DIV_STAT_MFC EXYNOS_4_CMU_TOP + 0x628
#define CLK_DIV_STAT_G3D EXYNOS_4_CMU_TOP + 0x62C
#define CLK_DIV_STAT_LCD EXYNOS_4_CMU_TOP + 0x634
#define CLK_DIV_STAT_ISP EXYNOS_4_CMU_TOP + 0x638
#define CLK_DIV_STAT_MAUDIO EXYNOS_4_CMU_TOP + 0x63C
#define CLK_DIV_STAT_FSYS0 EXYNOS_4_CMU_TOP + 0x640
#define CLK_DIV_STAT_FSYS1 EXYNOS_4_CMU_TOP + 0x644
#define CLK_DIV_STAT_FSYS2 EXYNOS_4_CMU_TOP + 0x648
#define CLK_DIV_STAT_FSYS3 EXYNOS_4_CMU_TOP + 0x64C
#define CLK_DIV_STAT_PERIL0 EXYNOS_4_CMU_TOP + 0x650
#define CLK_DIV_STAT_PERIL1 EXYNOS_4_CMU_TOP + 0x654
#define CLK_DIV_STAT_PERIL2 EXYNOS_4_CMU_TOP + 0x658
#define CLK_DIV_STAT_PERIL3 EXYNOS_4_CMU_TOP + 0x65C
#define CLK_DIV_STAT_PERIL4 EXYNOS_4_CMU_TOP + 0x660
#define CLK_DIV_STAT_PERIL5 EXYNOS_4_CMU_TOP + 0x664
#define CLK_DIV_STAT_CAM1 EXYNOS_4_CMU_TOP + 0x668
#define CLKDIV2_STAT EXYNOS_4_CMU_TOP + 0x680

#define CLK_GATE_BUS_FSYS1 EXYNOS_4_CMU_TOP + 0x744
#define CLK_GATE_IP_CAM EXYNOS_4_CMU_TOP + 0x920
#define CLK_GATE_IP_TV EXYNOS_4_CMU_TOP + 0x924
#define CLK_GATE_IP_MFC EXYNOS_4_CMU_TOP + 0x928
#define CLK_GATE_IP_G3D EXYNOS_4_CMU_TOP + 0x92C
#define CLK_GATE_IP_LCD EXYNOS_4_CMU_TOP + 0x934
#define CLK_GATE_IP_ISP EXYNOS_4_CMU_TOP + 0x938
#define CLK_GATE_IP_FSYS EXYNOS_4_CMU_TOP + 0x940
#define CLK_GATE_IP_GPS EXYNOS_4_CMU_TOP + 0x94C
#define CLK_GATE_IP_PERIL EXYNOS_4_CMU_TOP + 0x950
#define CLK_GATE_BLOCK EXYNOS_4_CMU_TOP + 0x970

#define CLKOUT_CMU_TOP EXYNOS_4_CMU_TOP + 0xA00
#define CLKOUT_CMU_TOP_DIV_STAT EXYNOS_4_CMU_TOP + 0xA04

/* CMU Rightbus registers (IP peripheral)*/
#define CLK_SRC_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0x200
#define CLK_MUX_STAT_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0x400
#define CLK_DIV_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0x500
#define CLK_DIV_STAT_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0x600
#define CLK_GATE_IP_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0x800
#define CLK_GATE_IP_PERIR EXYNOS_4_CMU_RIGHTBUS + 0x960
#define CLKOUT_CMU_RIGHTBUS EXYNOS_4_CMU_RIGHTBUS + 0xA00
#define CLKOUT_CMU_RIGHTBUS_DIV_STAT EXYNOS_4_CMU_RIGHTBUS + 0xA04


class ExynosClockManagement: public ARMClockManagement {
public:

	/* This should return the core specific frequency
	 * it may be the same for every core
	 */
	clock_t getCoreClock();

	/* This is the ARM PERIPHCLK which should be synchronous to the main
	 * clock but can be divided by a value > 2
	 */
	clock_t getPeripheralClock();

	/**
	 * Determine the SCLK_UART
	 */
	clock_t getUARTClk();

	/**
	 * Determine the MCT (Multicore Timer) clock
	 */
	clock_t getMCTClk();

	/**
	 * Get MPLL clock output frequency
	 */
	clock_t getAPLLFreq();

	/**
	 * Get MPLL clock output frequency
	 */
	clock_t getMPLLFreq();

#ifdef CONFIG_DEBUG
#ifdef CONFIG_CLOCK_DIAGNOSIS
	/**
	 * Debug functions for status information
	 */

	/**
	 * Dump APLL clock information
	 */
	void dumpAPLLStatus();

	/**
	 * Dump CMU TOP information about all the special peripheral blocks
	 */
	void dumpCMUTopStatus();

	/**
	 * Dump CMU Rightbus clock information
	 */
	void dumpCMURightbusStatus();
#endif
#endif

	/* Static instance */
	static ExynosClockManagement mInstance;

private:
	/* Values taken from Exynos TRM p. 387 */
	/* Main clock values for CPU cores */
	/* Target F_out,P,M,S,F_out */
	static uint32_t pApllValues[][5];

	/* MPLL provides LEFT_BUS,RIGHT_BUS clocks and DVFS clocks in transitions? */
	/* Target F_out,P,M,S,F_out */
	static uint32_t pMpllValues[][5];

	/* Primary use is the audio clock */
	/* Target F_out,P,M,S,K,F_out */
	static uint32_t pEpllValues[][6];

	/* VPLL is primary used for video clock generation */
	/* Target F_out,P,M,S,K,MFR,MRR,SSCG_EN */
	static uint32_t pVpllValues[][8];


};

#endif /* DRIVER_EXYNOSCLOCKMANAGEMENT_H_ */
