/*
 * ExynosClockManagement.cc
 *
 *  Created on: 27.07.2015
 *      Author: hendrik
 */

#include "ExynosClockManagement.h"

/* Debug includes */
#include <common/debug.h>
#define TAG "ExynosClock"

/* Memory mapped register operations */
#include <common/memreg.h>

/* Static instantiation */
ExynosClockManagement ExynosClockManagement::mInstance;

/* Values taken from Exynos TRM p. 387 */
/* Main clock values for CPU cores */
/* Target F_out,P,M,S,F_out */
uint32_t ExynosClockManagement::pApllValues[][5] = {
		{200000000,3,100,2,200000000},
		{300000000,4,200,2,300000000},
		{400000000,3,100,1,400000000},
		{500000000,3,125,1,500000000},
		{600000000,4,200,1,600000000},
		{700000000,3,175,1,700000000},
		{800000000,3,100,0,800000000},
		{900000000,4,150,0,900000000},
		{1000000000,3,125,0,1000000000},
		{1100000000,6,275,0,1100000000},
		{1200000000,4,200,0,1200000000},
		{1300000000,6,325,0,1300000000},
		{1400000000,3,175,0,1400000000},
};

/* MPLL provides LEFT_BUS,RIGHT_BUS clocks and DVFS clocks in transitions? */
/* Target F_out,P,M,S,F_out */
uint32_t ExynosClockManagement::pMpllValues[][5] = {
		{200000000,3,100,2,200000000},
		{300000000,4,200,2,300000000},
		{400000000,3,100,1,400000000},
		{500000000,3,125,1,500000000},
		{600000000,4,200,1,600000000},
		{700000000,3,175,1,700000000},
		{800000000,3,100,0,800000000},
		{900000000,4,150,0,900000000},
		{1000000000,3,125,0,1000000000},
		{1100000000,6,275,0,1100000000},
		{1200000000,4,200,0,1200000000},
		{1300000000,6,325,0,1300000000},
		{1400000000,3,175,0,1400000000},
};

/* Primary use is the audio clock */
/* Target F_out,P,M,S,K,F_out */
uint32_t ExynosClockManagement::pEpllValues[][6] = {
		{90000000,2,60,3,0,90000000},
		{180000000,2,60,2,0,180000000},
		{180600000,3,90,2,19661,180600000},
		{180633600,3,90,2,20762,180633600},
		{192000000,2,64,2,0,192000000},
		{200000000,3,100,2,0,200000000},
		{400000000,3,100,1,0,400000000},
		{408000000,2,68,1,0,408000000},
		{416000000,3,104,1,0,416000000},
};

/* VPLL is primary used for video clock generation */
/* Target F_out,P,M,S,K,MFR,MRR,SSCG_EN */
uint32_t ExynosClockManagement::pVpllValues[][8] = {
		{100000000,3,100,3,0,0,0,0},
		{160000000,3,160,3,0,0,0,0},
		{266000000,3,133,2,0,0,0,0},
		{350000000,3,175,2,0,0,0,0},
		{440000000,3,110,1,0,0,0,0},
};

clock_t ExynosClockManagement::getCoreClock() {
	dword_t apll_con0 = READ_REGISTER(APLL_CON0);
	/* apll div values */
	dword_t apll_mdiv = (apll_con0 & (0x3FF << 16)) >> 16;
	dword_t apll_pdiv = (apll_con0 & (0x3F << 8)) >> 8;
	dword_t apll_sdiv = (apll_con0 & (0x7 << 0)) >> 0;

	/* calculate F_out (s. exynos trm p. 512)
	 * FOUT = MDIV * F_IN/(PDIV*2^SDIV)
	 * */
	clock_t sdivmult = (apll_sdiv == 0) ? 1 : (2 << (apll_sdiv - 1));
	clock_t fout = apll_mdiv * FREQ_INPUT/(apll_pdiv * sdivmult);

	return fout;
}


/* Peripheral clock depends on the main core clock
 * It is the core clock divided by DIV_Periph
 *  */
clock_t ExynosClockManagement::getPeripheralClock() {
	/* Main clock value to get divided one */
	clock_t mainClk = getCoreClock();

	/* Get the coresponding divider value */
	dword_t clkdiv0 = READ_REGISTER(CLK_DIV_CPU0);
	uint32_t periphratio = (clkdiv0 & (0x7 << 12)) >> 12;

	return (mainClk / (periphratio + 1));
}

#ifdef CONFIG_DEBUG
#ifdef CONFIG_CLOCK_DIAGNOSIS
void ExynosClockManagement::dumpAPLLStatus() {
	/* Output APLL_LOCKTIME	 */
	uint32_t lockTime = READ_REGISTER(APLL_LOCK) & (0xFFFF);
	DEBUG_STREAM(TAG,"APLL_PLL_LOCKTIME: " << hex << lockTime);

	dword_t apll_con0 = READ_REGISTER(APLL_CON0);
	dword_t apll_enable = (apll_con0 & (1 << 31)) >> 31;
	dword_t apll_locked = (apll_con0 & (1 << 29)) >> 29;
	dword_t apll_fsel = (apll_con0 & (1 << 27)) >> 27;

	/* apll div values */
	dword_t apll_mdiv = (apll_con0 & (0x3FF << 16)) >> 16;
	dword_t apll_pdiv = (apll_con0 & (0x3F << 8)) >> 8;
	dword_t apll_sdiv = (apll_con0 & (0x7 << 0)) >> 0;

	/* calculate F_out (s. exynos trm p. 512)
	 * FOUT = MDIV * F_IN/(PDIV*2^SDIV)
	 * */
	clock_t sdivmult = (apll_sdiv == 0) ? 1 : (2 << (apll_sdiv - 1));
	clock_t fout = apll_mdiv * FREQ_INPUT/(apll_pdiv * sdivmult);

	/* Output values */
	DEBUG_STREAM(TAG,"APLL_ENABLED: " << hex << apll_enable);
	DEBUG_STREAM(TAG,"APLL_LOCKED: " << hex << apll_locked);
	DEBUG_STREAM(TAG,"APLL_FSEL: " << hex << apll_fsel);

	DEBUG_STREAM(TAG,"APLL_MDIV: " << hex << apll_mdiv);
	DEBUG_STREAM(TAG,"APLL_PDIV: " << hex << apll_pdiv);
	DEBUG_STREAM(TAG,"APLL_SDIV: " << hex << apll_sdiv);
	DEBUG_STREAM(TAG,"APLL_SDIV_multiplier value: " << hex << sdivmult);
	DEBUG_STREAM(TAG,"APLL_FREQUENCY: " << dec << fout << " Hz");

	/* Output derived clocks from dividers */
	dword_t clkdiv0 = READ_REGISTER(CLK_DIV_CPU0);
	uint32_t core2ratio = (clkdiv0 & (0x7 << 28)) >> 28;
	uint32_t apllratio = (clkdiv0 & (0x7 << 24)) >> 24;
	uint32_t dbgratio = (clkdiv0 & (0x7 << 20)) >> 20;
	uint32_t atbratio = (clkdiv0 & (0x7 << 16)) >> 16;
	uint32_t periphratio = (clkdiv0 & (0x7 << 12)) >> 12;
	uint32_t corem1ratio = (clkdiv0 & (0x7 << 8)) >> 8;
	uint32_t corem0ratio = (clkdiv0 & (0x7 << 4)) >> 4;
	uint32_t coreratio = (clkdiv0 & (0x7 << 0)) >> 0;

	DEBUG_STREAM(TAG,"CLK_DIV_CPU0: " << hex << clkdiv0);
	DEBUG_STREAM(TAG,"CORE2_RATIO: " << hex << core2ratio);
	DEBUG_STREAM(TAG,"APLL_RATIO: " << hex << apllratio);
	DEBUG_STREAM(TAG,"PCLK_DBG_RATIO: " << hex << dbgratio);
	DEBUG_STREAM(TAG,"ATB_RATIO: " << hex << atbratio);
	DEBUG_STREAM(TAG,"PERIPH_RATIO: " << hex << periphratio);
	DEBUG_STREAM(TAG,"COREM1_RATIO: " << hex << corem1ratio);
	DEBUG_STREAM(TAG,"COREM0_RATIO: " << hex << corem0ratio);
	DEBUG_STREAM(TAG,"CORE_RATIO: " << hex << coreratio);

	/* Print Clock multiplexer configurations */
	dword_t clkSrcCpu = READ_REGISTER(CLK_SRC_CPU);
	dword_t muxMPLLUserSelC = (clkSrcCpu & (1 << 24)) >> 24;
	dword_t muxHPMSel = (clkSrcCpu & (1 << 20)) >> 20;
	dword_t muxCoreSel = (clkSrcCpu & (1 << 16)) >> 16;
	dword_t muxAPLLSel = (clkSrcCpu & (1 << 0)) >> 0;

	DEBUG_STREAM(TAG,"CLK_SRC_CPU: " << hex << clkSrcCpu);
	DEBUG_STREAM(TAG,"MUX_MPLL_USER_SEL_C: " << hex << muxMPLLUserSelC);
	DEBUG_STREAM(TAG,"MUX_HPM_SEL: " << hex << muxHPMSel);
	DEBUG_STREAM(TAG,"MUX_CORE_SEL: " << hex << muxCoreSel);
	DEBUG_STREAM(TAG,"MUX_APLL_SET: " << hex << muxAPLLSel);

	if ( muxAPLLSel == 1 && muxCoreSel == 0 ) {
		DEBUG_STREAM(TAG,"Core clk is using APLL for its frequency generation");
	} else {
		DEBUG_STREAM(TAG,"Core is using an external clock (SPLL probably), CLOCK DETERMINATION IS NOT IMPLEMENTED");
	}

	DEBUG_STREAM(TAG,"Peripheral clock value: " << dec << getPeripheralClock());
}

void ExynosClockManagement::dumpCMUTopStatus() {
	/* Debug output needed from this clock management unit should go here */

	/* Uart specific debug output */
	dword_t clkSrcPeril0 = READ_REGISTER(CLK_SRC_PERIL0);

	/* UART1 multiplexer */
	uint32_t uart1_sel = (clkSrcPeril0 & (0xF << 4)) >> 4;
	DEBUG_STREAM(TAG,"UART1_SEL: " << hex << uart1_sel);

	if (uart1_sel == 6 ) {
		DEBUG_STREAM(TAG,"UART1 clocksource is SCLKMPLL_USER_T");

		dword_t clkMuxStatTop1 =  READ_REGISTER(CLK_MUX_STAT_TOP1);
		uint32_t mpllUserSelT = (clkMuxStatTop1 & (0x3 << 12)) >> 12;

		DEBUG_STREAM(TAG,"CLK_MUX_STAT_TOP1: " << hex << clkMuxStatTop1);
		DEBUG_STREAM(TAG,"MPLL_USER_SEL_T: " << hex << mpllUserSelT);

		if ( mpllUserSelT == 2 ) {
			/* MPLL_USER_SEL_T is using the MPLL_OUTPUT (p. ETRM 461) */
			/* Calculate the MPLL frequency */
			dword_t mpll_con0 = READ_REGISTER(MPLL_CON0);

			/* mpll div values */
			dword_t mpll_mdiv = (mpll_con0 & (0x3FF << 16)) >> 16;
			dword_t mpll_pdiv = (mpll_con0 & (0x3F << 8)) >> 8;
			dword_t mpll_sdiv = (mpll_con0 & (0x7 << 0)) >> 0;

			clock_t sdivmult = (mpll_sdiv == 0) ? 1 : (2 << (mpll_sdiv - 1));
			clock_t fout = mpll_mdiv * FREQ_INPUT/(mpll_pdiv * sdivmult);

			DEBUG_STREAM(TAG,"MPLL_MDIV: " << hex << mpll_mdiv);
			DEBUG_STREAM(TAG,"MPLL_PDIV: " << hex << mpll_pdiv);
			DEBUG_STREAM(TAG,"MPLL_SDIV: " << hex << mpll_sdiv);
			DEBUG_STREAM(TAG,"MPLL_SDIV_multiplier value: " << hex << sdivmult);
			DEBUG_STREAM(TAG,"MPLL frequency output is: " << dec << fout << " Hz");

			/* Get the UART1 multiplier and calculate final UART1_frequency */
			dword_t clkDivPeril0 = READ_REGISTER(CLK_DIV_PERIL0);
			uint32_t uart1_ratio = (clkDivPeril0 & (0xF << 4)) >> 4;
			DEBUG_STREAM(TAG,"UART1_RATIO: " << hex << uart1_ratio);
			uint32_t uart1_freq = fout/(uart1_ratio + 1);
			DEBUG_STREAM(TAG,"SCLK_UART1: " << dec << uart1_freq << " Hz");
		}

	} else if (uart1_sel == 7 ) {
		DEBUG_STREAM(TAG,"UART1 clocksource is SCLKEPLL");
	} else if (uart1_sel == 8 ) {
		DEBUG_STREAM(TAG,"UART1 clocksource is SCLKVPLL");
	} else {
		DEBUG_STREAM(TAG,"Determination of UART1 clksource not implemented. ERROR!");
	}

}

void ExynosClockManagement::dumpCMURightbusStatus() {
	/* Read the Rightbus clock configuration registers */
	DEBUG_STREAM(TAG,"CLK_SRC_RIGHTBUS: " << hex << READ_REGISTER(CLK_SRC_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLK_MUX_STAT_RIGHTBUS: " << hex << READ_REGISTER(CLK_MUX_STAT_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLK_DIV_RIGHTBUS: " << hex << READ_REGISTER(CLK_DIV_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLK_DIV_STAT_RIGHTBUS: " << hex << READ_REGISTER(CLK_DIV_STAT_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLK_GATE_IP_RIGHTBUS: " << hex << READ_REGISTER(CLK_GATE_IP_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLK_GATE_IP_PERIR: " << hex << READ_REGISTER(CLK_GATE_IP_PERIR));
	DEBUG_STREAM(TAG,"CLKOUT_CMU_RIGHTBUS: " << hex << READ_REGISTER(CLKOUT_CMU_RIGHTBUS));
	DEBUG_STREAM(TAG,"CLKOUT_CMU_RIGHTBUS_DIV_STAT: " << hex << READ_REGISTER(CLKOUT_CMU_RIGHTBUS_DIV_STAT));
}

#endif
#endif

/**
 * Determines the UART clock rate in Hz.
 * FIXME: Implement for other uart devices than 1
 */
clock_t ExynosClockManagement::getUARTClk() {
	/* Debug output needed from this clock management unit should go here */

	/* Uart specific debug output */
	dword_t clkSrcPeril0 = READ_REGISTER(CLK_SRC_PERIL0);

	/* UART1 multiplexer */
	uint32_t uart1_sel = (clkSrcPeril0 & (0xF << 4)) >> 4;

	if (uart1_sel == 6 ) {
		dword_t clkMuxStatTop1 =  READ_REGISTER(CLK_MUX_STAT_TOP1);
		uint32_t mpllUserSelT = (clkMuxStatTop1 & (0x3 << 12)) >> 12;

		if ( mpllUserSelT == 2 ) {
			/* MPLL_USER_SEL_T is using the MPLL_OUTPUT (p. ETRM 461) */
			/* Calculate the MPLL frequency */
			dword_t mpll_con0 = READ_REGISTER(MPLL_CON0);

			/* mpll div values */
			dword_t mpll_mdiv = (mpll_con0 & (0x3FF << 16)) >> 16;
			dword_t mpll_pdiv = (mpll_con0 & (0x3F << 8)) >> 8;
			dword_t mpll_sdiv = (mpll_con0 & (0x7 << 0)) >> 0;

			clock_t sdivmult = (mpll_sdiv == 0) ? 1 : (2 << (mpll_sdiv - 1));
			clock_t fout = mpll_mdiv * FREQ_INPUT/(mpll_pdiv * sdivmult);


			/* Get the UART1 multiplier and calculate final UART1_frequency */
			dword_t clkDivPeril0 = READ_REGISTER(CLK_DIV_PERIL0);
			uint32_t uart1_ratio = (clkDivPeril0 & (0xF << 4)) >> 4;
			uint32_t uart1_freq = fout/(uart1_ratio + 1);

			return uart1_freq;
		}

	} else if (uart1_sel == 7 ) {
		return 0;
	} else if (uart1_sel == 8 ) {
		return 0;
	} else {
		return 0;
	}
	return 0;
}

clock_t ExynosClockManagement::getMCTClk() {
	/* Read the first mux in cmu */
	dword_t rightbusSrc = READ_REGISTER(CLK_SRC_RIGHTBUS);
	DEBUG_STREAM(TAG,"rightbusSrc: " << hex << rightbusSrc);
	if ( (rightbusSrc & (0x10)) == 0 ) {
		/**
		 * Mux is set to F_inPLL
		 */
		// FIXME: check if clock is really 24000000 Hz
		return 24000000;
	} else {
		/**
		 * Mux is set to Fout_Mpll, check next mux
		 */
		clock_t muxClk = 0;
		if ( (rightbusSrc & (0x1)) == 0 ) {
			/**
			 * Next mux is set to MPLL as well, get Mpll clock
			 */
			muxClk = getMPLLFreq();
		} else {
			/**
			 * Next mux is set to APLL, get Apll clock
			 */
			muxClk = getAPLLFreq();
		}
		DEBUG_STREAM(TAG,"muxFreq for mct: " << dec << muxClk << " Hz");
		/* Check possible divider values */
		dword_t divRB = READ_REGISTER(CLK_DIV_RIGHTBUS);
		uint32_t gprRatio = ((divRB & (0x70)));
		gprRatio >>= 4;
		uint32_t gdrRatio = (divRB & (0x7));
		DEBUG_STREAM(TAG,"gprRatio: " << dec << gprRatio);
		DEBUG_STREAM(TAG,"gdrRatio: " << dec << gdrRatio);
		/**
		 * MCT should be connected to the IP_PERIR output
		 */
		return (muxClk / (gdrRatio + 1))/(gprRatio + 1);
	}
	return 0;
}

clock_t ExynosClockManagement::getMPLLFreq() {
	/* Calculate the MPLL frequency */
	dword_t mpll_con0 = READ_REGISTER(MPLL_CON0);

	/* mpll div values */
	dword_t mpll_mdiv = (mpll_con0 & (0x3FF << 16)) >> 16;
	dword_t mpll_pdiv = (mpll_con0 & (0x3F << 8)) >> 8;
	dword_t mpll_sdiv = (mpll_con0 & (0x7 << 0)) >> 0;

	clock_t sdivmult = (mpll_sdiv == 0) ? 1 : (2 << (mpll_sdiv - 1));
	clock_t fout = mpll_mdiv * FREQ_INPUT/(mpll_pdiv * sdivmult);
	return fout;
}

clock_t ExynosClockManagement::getAPLLFreq() {
	dword_t apll_con0 = READ_REGISTER(APLL_CON0);
	/* apll div values */
	dword_t apll_mdiv = (apll_con0 & (0x3FF << 16)) >> 16;
	dword_t apll_pdiv = (apll_con0 & (0x3F << 8)) >> 8;
	dword_t apll_sdiv = (apll_con0 & (0x7 << 0)) >> 0;

	/* calculate F_out (s. exynos trm p. 512)
	 * FOUT = MDIV * F_IN/(PDIV*2^SDIV)
	 * */
	clock_t sdivmult = (apll_sdiv == 0) ? 1 : (2 << (apll_sdiv - 1));
	clock_t fout = apll_mdiv * FREQ_INPUT/(apll_pdiv * sdivmult);
	return fout;
}
