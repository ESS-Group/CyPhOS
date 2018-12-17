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
extern "C" {

void armv7_disable_pl310() {
	CacheManagement::ARMV7CacheControl::pInstance.pl310_disableL2Cache();
}
}


namespace CacheManagement {
ARMV7CacheControl::ARMV7CacheControl() : GenericCacheManagement(),
		mOperation(NO_CACHE_OP){
	// Nothing here for now
	mCacheWayCount = PL310_CACHE_WAYS;
#ifdef CONFIG_ARM_STANDBY_CACHE_WAYS
	// Reduce cache-way count by the number of cpus for standby usage
	mCacheWayCount -= NR_CPUS;
#endif

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

void ARMV7CacheControl::pl310_enable() {

	pl310_invalidateSharedCache();
	PL310_ENABLE_FUNC
	();
	BUSY_WAITING_SHORT
	;

#ifdef CONFIG_CACHE_CONTROL
	/* Cleanup management structure */
	for ( cacheways_t i = 0; i < PL310_CACHE_WAYS; i++ ) {
		mCacheWays[i] = {false,LRU_MAX_VALUE,0x0};
#ifdef PL310_LOCKDOWN_BY_MASTER
		pl310_LockCacheWay_All_Master(i,true);
#else
		pl310_LockCacheWay(i, true);
#endif

	}
#endif


	for (cacheways_t i = 0; i < PL310_CACHE_WAYS; i++ ) {
		// Clean all ways
		cleanAndInvalidateWay(i);
	}
	BUSY_WAITING_SHORT

#ifdef PL310_LOCKDOWN_BY_LINE
	pl310_LockdownByLine(true);
#endif

	printCacheWayInformation();
}

void ARMV7CacheControl::pl310_disable() {
	// Flush and invalidate before disabling
	pl310_cleanSharedCache();
	DSB;
	PL310_DISABLE_FUNC
	();
	ISB;
	DSB;
	BUSY_WAITING_SHORT
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

	pl310_printInformation();
}


void ARMV7CacheControl::setCSSELR(dword_t regContent) {
	asm ( "MCR p15, 2, %0, c0, c0, 0;" :: "r"(regContent));
}


/* See PL310 trm p. 70 */
void ARMV7CacheControl::pl310_enableL2Cache() {
	pl310_enable();
}

void ARMV7CacheControl::pl310_disableL2Cache() {
	/*__asm__ __volatile__ ("mrc p15, 0, r0, c1, c0, 1\n"
	 "bic r0, r0, #0x2\n"
	 "mcr p15, 0, r0, c1, c0, 1\n" : : :"r0","memory");*/
	pl310_disable();
#ifdef DEBUG_OUTPUT
	printPL310Information();
#endif
}

void ARMV7CacheControl::pl310_invalidateSharedCache() {
	dword_t regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_INVALIDATE_BY_WAY);
	regVal = 0xffff;
	WRITE_REGISTER(PL310_BASE_ADDRESS+PL310_INVALIDATE_BY_WAY, regVal);
	// Wait for completion
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_CLEAN_AND_INVALIDATE_BY_WAY) != 0) {}

	// Sync the PL310
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}
}

void ARMV7CacheControl::pl310_cleanSharedCache() {
	dword_t regVal = 0xffff;
	WRITE_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_BY_WAY, regVal);
	ISB;
	DSB;
	// Wait for completion
	regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_BY_WAY);
	while ((regVal & 0xFFFF) != 0) {
		regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_BY_WAY);
	}
	ISB;
	DSB;
}

void ARMV7CacheControl::pl310_printInformation() {
	DEBUG_STREAM(TAG,"Printing PL310 Level2 controller information" ); DEBUG_STREAM(TAG,"reg0_cache_id: " << hex
			<< (uint32_t) READ_REGISTER(PL310_BASE_ADDRESS+PL310_CACHE_ID)
	); DEBUG_STREAM(TAG,"reg0_cache_type: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_CACHE_TYPE) ); DEBUG_STREAM(TAG,"reg1_control: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_CACHE_CONTROL) ); DEBUG_STREAM(TAG,"reg1_aux_control: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_AUXILARY_CONTROL) ); DEBUG_STREAM(TAG,"reg1_tag_ram_control: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_TAG_RAM_CONTROL) ); DEBUG_STREAM(TAG,"reg1_data_ram_control: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_DATA_RAM_CONTROL) ); DEBUG_STREAM(TAG,"reg15_power_ctrl: " << hex
			<< (uint32_t) READ_REGISTER(
					PL310_BASE_ADDRESS+PL310_POWER_CONTROL) );
					DEBUG_STREAM(TAG,"reg2_ev_counter_ctrl: " << hex
								<< (uint32_t) READ_REGISTER(
										PL310_BASE_ADDRESS+PL310_EVENT_COUNTER_CONTROL) );
					DEBUG_STREAM(TAG,"reg2_ev_counter0_cfg: " << hex
								<< (uint32_t) READ_REGISTER(
										PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT0_CFG) );
					DEBUG_STREAM(TAG,"reg2_ev_counter1_cfg: " << hex
								<< (uint32_t) READ_REGISTER(
										PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT1_CFG) );
					DEBUG_STREAM(TAG,"reg15_prefetch_ctrl: " << hex
								<< (uint32_t) READ_REGISTER(
										PL310_BASE_ADDRESS + PL310_PREFETCH_CONTROL) );

#ifdef PL310_LOCKDOWN_BY_MASTER
	DEBUG_STREAM(TAG,"PL310 lockdown by master enabled");
#else
	DEBUG_STREAM(TAG,"PL310 lockdown by master disabled");
#endif
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

void ARMV7CacheControl::swIntHandler() {
	/* Various cache management operations that should be done
	 * synchronously across the cores.
	 */
	switch (mOperation) {
	case ENABLE_L1:
		cleanAndInvalidate_Data_Caches();
		enableUnifiedCache();
		enableInstructionCache();
		cleanAndInvalidate_Data_Caches();
		BUSY_WAITING_LONG
		;
		break;
	case DISABLE_L1:
		cleanAndInvalidate_Data_Caches();
		disableUnifiedCache();
		disableInstructionCache();
		cleanAndInvalidate_Data_Caches();
		BUSY_WAITING_LONG
		;
		break;
	case ENABLE_L2:
		if (getCPUID() == 0x0) {
			pl310_enableL2Cache();
		} else {
			BUSY_WAITING_LONG
			;
		}
		break;
	case CLEAN_AND_INVALIDATE_L1:
		cleanAndInvalidate_Data_Caches();
		break;
	case NO_CACHE_OP:
		break;
	}
	mOperation = NO_CACHE_OP;
	BUSY_WAITING_SHORT
	;

	return;
}

void ARMV7CacheControl::enableL1Caches() {
	invalidate_Data_Caches();
	enableUnifiedCache();
	enableInstructionCache();
	OSC_FUNC_ASM(CacheControl,asm_invalidate_instruction_cache)();
}
}
