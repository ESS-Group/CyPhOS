/*
 * ARMV7CacheControl.cc
 *
 *  Created on: 20.08.2014
 *      Author: hb
 */
#include "ARMV7CacheControl.h"

#include <common/bitmanipulation.h>
#include <common/types.h>

#include <common/memreg.h>
#include <common/armhelper.h>

#include <driver/ARMGIC.h>

#include <driver/ARMV7PerformanceCounter.h>

#define TAG "CACHECONTROL"
#include <common/SyncOutput.h>


namespace CacheManagement {
/**
 * Static singleton instance
 */
ARMV7CacheControl ARMV7CacheControl::pInstance;

ARMV7CacheControl::ARMV7CacheControl(){


}


void ARMV7CacheControl::enableInstructionCache() {
	OSC_FUNC_ASM(CacheControl,asm_enable_instruction_cache)();
}

void ARMV7CacheControl::disableInstructionCache() {
	OSC_FUNC_ASM(CacheControl,asm_disable_instruction_cache)();
}

void ARMV7CacheControl::enableUnifiedCache() {
	OSC_FUNC_ASM(CacheControl,asm_enable_unified_cache)();
}

void ARMV7CacheControl::disableUnifiedCache() {
	OSC_FUNC_ASM(CacheControl,asm_disable_unified_cache)();
}

void ARMV7CacheControl::enableSMPMode() {
	__asm__ __volatile__ ("MRC     p15, 0, r0, c1, c0, 1;"/* Enable SMP bit in Aux control register */
			//  Only try to set SMP mode if not yet set - cannot write AuxCtrl if in normal world
			"ORR     r0, #(1 << 6);"//  SMP bit
			"MCR   p15, 0, r0, c1, c0, 1;"//  write Aux Control Register
			:::"memory");
}


void ARMV7CacheControl::enablePrefetching() {
	__asm__ __volatile__ ("MRC     p15, 0, r0, c1, c0, 1;"/* Enable SMP bit in Aux control register */
			//  Only try to set SMP mode if not yet set - cannot write AuxCtrl if in normal world
			"ORR     r0, #(1 << 1);"//  L2 prefetch enable
			"ORR     r0, #(1 << 2);"//  L1 prefetch enable
			"MCR   p15, 0, r0, c1, c0, 1;"//  write Aux Control Register
			:::"memory");
}

void ARMV7CacheControl::disableSMPMode() {
	__asm__ __volatile__ ("MRC     p15, 0, r0, c1, c0, 1;"/* Enable SMP bit in Aux control register */
			//  Only try to set SMP mode if not yet set - cannot write AuxCtrl if in normal world
			"BIC     r0, #(1 << 6);"//  SMP bit
			"MCR   p15, 0, r0, c1, c0, 1;"//  write Aux Control Register
			:::"memory");
}




void ARMV7CacheControl::invalidateInstructionCache() {
	OSC_FUNC_ASM(CacheControl,asm_invalidate_instruction_cache)();
}

void ARMV7CacheControl::cleanAndInvalidate_Data_Caches() {
	OSC_FUNC_ASM(CacheControl,asm_clean_and_invalidate_data_caches)();
}

void ARMV7CacheControl::invalidate_Data_Caches() {
	OSC_FUNC_ASM(CacheControl,asm_invalidate_data_caches)();
}

void ARMV7CacheControl::clean_Data_Caches() {
	OSC_FUNC_ASM(CacheControl,asm_clean_data_caches)();
}

/**
 * See ARMv7 architecture reference
 */
dword_t ARMV7CacheControl::readCCSIDR() {
	dword_t regValue;
	asm ( "MRC p15, 1, %0, c0, c0, 0;" : "=&r"(regValue));
	return regValue;
}

dword_t ARMV7CacheControl::readCTR() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c0, c0, 1;" : "=&r"(regValue));
	return regValue;
}

dword_t ARMV7CacheControl::readCSSELR() {
	dword_t regValue;
	asm ( "MRC p15, 2, %0, c0, c0, 0;" : "=&r"(regValue));
	return regValue;
}

dword_t ARMV7CacheControl::readCLIDR() {
	dword_t regValue;
	asm ( "MRC p15, 1, %0, c0, c0, 1;" : "=&r"(regValue));
	return regValue;
}

dword_t ARMV7CacheControl::readPLEID() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c11, c0, 0;" : "=&r"(regValue));
	return regValue;
}

void ARMV7CacheControl::printCacheInformation() {
	dword_t clidr = readCLIDR();
	DEBUG_STREAM(TAG,"Cache information:" ); DEBUG_STREAM(TAG,"CTR: " << hex << (dword_t) readCTR() ); DEBUG_STREAM(TAG,"CLIDR: " << hex << (dword_t) clidr );
//	DEBUG_STREAM(TAG,"PLEID: " << hex << (dword_t) readPLEID() );
	for (int i = 0; i < 7; i++) {
		byte_t cacheLevelStatus = clidr & (0x7 << 3 * i);
		switch (cacheLevelStatus) {
		case 0x0:
			DEBUG_STREAM(TAG,"No level " << dec << i + 1 << " cache" );
			break;
		case 0x1:
			DEBUG_STREAM(TAG,"Level " << dec << i + 1 << " instruction only cache"
			);
			break;
		case 0x2:
			DEBUG_STREAM(TAG,"Level " << dec << i + 1 << " data only cache" );
			break;
		case 0x3:
			DEBUG_STREAM(TAG,"Level " << dec << i + 1
					<< " instruction/data separate cache" );
			break;
		case 0x4:
			DEBUG_STREAM(TAG,"Level " << dec << i + 1 << " unified cache" );
			break;
		}
		if (cacheLevelStatus == 0x3) {
			// Get cache size
			// Set selection register to instruction cache and level
			dword_t csselr = readCSSELR();
			csselr = setbit(csselr, 0, true);
			csselr &= ~(0xE);
			csselr |= (i << 1);
			setCSSELR(csselr);

			DEBUG_STREAM(TAG,"Instruction cache: " << hex << readCCSIDR() );

			csselr = readCSSELR();
			csselr = setbit(csselr, 0, false);
			csselr &= ~(0xE);
			csselr |= (i << 1);
			setCSSELR(csselr);

			DEBUG_STREAM(TAG,"Data cache: " << hex << readCCSIDR() );
		} else if (cacheLevelStatus == 0x2 || cacheLevelStatus == 0x4) {
			dword_t csselr = readCSSELR();
			csselr = setbit(csselr, 0, false);
			csselr &= ~(0xE);
			csselr |= (i << 1);
			setCSSELR(csselr);

			DEBUG_STREAM(TAG,"Data cache: " << hex << readCCSIDR());
		}
	}
}


void ARMV7CacheControl::setCSSELR(dword_t regContent) {
	asm ( "MCR p15, 2, %0, c0, c0, 0;" :: "r"(regContent));
}

void ARMV7CacheControl::enableBranchPrediction() {
	__asm__ __volatile__ (
			// Read SCTLR
			"MRC    p15, 0, r0, c1, c0, 0\n"
			// Enable BPD Bit
			"ORR   r0, r0, #(1 << 11)\n"
			// Write SCTLR
			"MCR    p15, 0, r0, c1, c0, 0\n" :::"r0","memory");
}

void ARMV7CacheControl::disableBranchPrediction() {
	__asm__ __volatile__ (
			// Read SCTLR
			"MRC    p15, 0, r0, c1, c0, 0\n"
			// Enable BPD Bit
			"BIC   r0, r0, #(1 << 11)\n"
			// Write SCTLR
			"MCR    p15, 0, r0, c1, c0, 0\n" :::"r0","memory");
}

void ARMV7CacheControl::invalidateBranchPrediction() {
	//FIXME search for BPIALL
	__asm__ __volatile__ (
			"mov r0,#0x0;"
			"mcr	p15, 0, r0, c7, c5, 6;"
			"dsb;"
			"isb;"
			:::"r0","memory");
}


void ARMV7CacheControl::enableL1Caches() {
	invalidate_Data_Caches();
	invalidateInstructionCache();
	enableUnifiedCache();
	enableInstructionCache();
	OSC_FUNC_ASM(CacheControl,asm_invalidate_instruction_cache)();
}
}
