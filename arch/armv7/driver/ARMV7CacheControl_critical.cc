/*
 * ARMV7CacheControl_critical.cc
 *
 *  Created on: 27.01.2017
 *      Author: hendrik
 */

#include "ARMV7CacheControl.h"

#include <eventhandling/EventTask.h>

#include <common/bitmanipulation.h>
#include <common/types.h>

#include <common/memreg.h>
#include <common/armhelper.h>

#include <driver/ARMGIC.h>
#include <arch/armv7/driver/ARMMMU.h>

#include <common/cyclecounter.h>

#include <common/memorytest.h>

#define TAG "CACHECONTROL"


namespace CacheManagement {

/* Static instance */
ARMV7CacheControl ARMV7CacheControl::pInstance;

void ARMV7CacheControl::pl310_LockCacheWay(uint8_t way, bool lockdown) {
	uint32_t offset = 0;
#ifdef PL310_LOCKDOWN_BY_MASTER
	offset = getCPUID();
#endif

	dword_t lockdownRegisterD = READ_REGISTER(PL310_D_LOCKDOWN__(offset));
	dword_t lockdownRegisterI = READ_REGISTER(PL310_I_LOCKDOWN__(offset));

	lockdownRegisterD = setbit(lockdownRegisterD, way, lockdown);
	lockdownRegisterI = setbit(lockdownRegisterI, way, lockdown);

	WRITE_REGISTER(PL310_D_LOCKDOWN__(offset), lockdownRegisterD);
	WRITE_REGISTER(PL310_I_LOCKDOWN__(offset), lockdownRegisterI);

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG, "Wrote D lockdown("<< dec << offset <<  "):" << hex << READ_REGISTER(PL310_D_LOCKDOWN__(offset)););
	DEBUG_STREAM(TAG, "Wrote I lockdown("<< dec << offset <<  "):" << hex << READ_REGISTER(PL310_I_LOCKDOWN__(offset)););
#endif
	DSB;ISB;

	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}
}

#ifdef PL310_LOCKDOWN_BY_MASTER
void ARMV7CacheControl::pl310_LockCacheWay_All_Master(uint8_t way, bool lockdown) {
	// FIXME this will for now only work if lockdown by master is unimplemented and lockdown is
	// handled by lockdown register 0

	for(uint32_t offset=0; offset<cMaxLockdownRegisters; offset++) {

		dword_t lockdownRegisterD = READ_REGISTER(PL310_D_LOCKDOWN__(offset));
		dword_t lockdownRegisterI = READ_REGISTER(PL310_I_LOCKDOWN__(offset));

		lockdownRegisterD = setbit(lockdownRegisterD, way, lockdown);
		lockdownRegisterI = setbit(lockdownRegisterI, way, lockdown);

		WRITE_REGISTER(PL310_D_LOCKDOWN__(offset), lockdownRegisterD);
		WRITE_REGISTER(PL310_I_LOCKDOWN__(offset), lockdownRegisterI);
#ifdef CONFIG_CACHE_DEBUG
		DEBUG_STREAM(TAG, "Wrote D lockdown("<< dec << offset <<  "):" << hex << READ_REGISTER(PL310_D_LOCKDOWN__(offset)););
		DEBUG_STREAM(TAG, "Wrote I lockdown("<< dec << offset <<  "):" << hex << READ_REGISTER(PL310_I_LOCKDOWN__(offset)););
#endif
	}


	DSB;

}
#endif



/**
 * Clean & invalidate of a L2 shared cache-way. This needs to be done with prefetch lock taken, otherwise
 * data corruption may occur!
 * @param way Cache way to clean
 */
void ARMV7CacheControl::cleanAndInvalidateWay(uint8_t way) {
	// Prevent other cores from using the l2 cache way that is currently written back to the main memory

	// Clean data through the cache hierarchy l1->l2->memory
	OSC_FUNC_ASM(CacheControl,asm_clean_data_caches)();
	DSB;

	// Writeback (clean) of l2 cache way
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_CLEAN_AND_INVALIDATE_BY_WAY), (0x1 << way));
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_CLEAN_AND_INVALIDATE_BY_WAY) != 0) {}
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}

	// Invalidate the l1 cache FIXME, check if this is necessary, seems to introduce a
	// lot of execution time jitter
	// OSC_FUNC_ASM(CacheControl,asm_clean_and_invalidate_data_caches)();
	//DSB;
}

void ARMV7CacheControl::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration) {
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Preload from: " << hex << start << " to textEnd: " << textEnd << " end: " << end << " in way: " << dec << way);
#endif
#ifdef CONFIG_ARM_STANDBY_CACHE_WAYS
	// Add offset to cache way as first ways are reserved for standby usage
	way += NR_CPUS;
#endif

	/* Unlock the cache way */
#ifndef PL310_LOCKDOWN_BY_MASTER_LOCK_ALL
	pl310_LockCacheWay(way, false);
#else
	pl310_LockCacheWay_All_Master(way, false);
#endif

#ifdef PL310_LOCKDOWN_BY_LINE
	pl310_LockdownByLineCleanLockdown(way);
#endif

#define PRELOAD_ASSEMBLY
#ifdef PRELOAD_ASSEMBLY
#ifdef CONFIG_PROFILING_PRELOAD
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif
	/* Prefetch the data */
		__asm__ __volatile__ (""
				".arch_extension mp\n"
				"mov r0,%[oscStart]\n"
				"0: CMP R0,%[oscTextEnd]\n"
				"BGE 2f;"
				// Instruction prefetch
	#ifdef CONFIG_CACHE_PLDW_FOR_INSTRUCTION
				"1: PLDW [r0]; B 3f;"
	#else
				"1: PLI [r0]; B 3f;"
	#endif
				// Data prefetch
				"2: PLDW [r0];"
				//"0: LDR r2,[r0]\n"
				"3:ADD r0, #32\n"
				"CMP %[oscEnd],r0\n"
				"BGE 0b\n"
				"DSB\n"
				""::[oscStart]"r"(start),[oscEnd]"r"(end),[oscTextEnd]"r"(textEnd):"r0","memory");
#ifdef CONFIG_PROFILING_PRELOAD
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
#endif
#ifdef PRELOAD_C_RELOAD
		static const uint32_t maxRetries = 3;
		cycle_t cycles = 0;
		uint32_t retries = 0;
		for (uintptr_t preloadStart = start; preloadStart <= end; preloadStart += CONFIG_CACHE_LINE_SIZE) {
			for (uint32_t retry = 0; retry < maxRetries; retry++) {
				MEASURE_MEMORY_ACCESS(preloadStart, cycles);
//				DEBUG_STREAM(TAG,"Try: " << dec << retry << " cycles: " << dec << cycles);
				if (cycles < 100) {
					break;
				}
				if (retry > 0) {
					retries++;
				}
			}
		}

		if ( retries > 0) {
			DEBUG_STREAM(TAG,"Retries: " << dec << retries);
		}
#endif

	/* Lock the cache way */
#ifndef PL310_LOCKDOWN_BY_MASTER_LOCK_ALL
	pl310_LockCacheWay(way, true);
#else
	pl310_LockCacheWay_All_Master(way, true);
#endif
}

void ARMV7CacheControl::evictCacheWay(cacheways_t way, cycle_t *duration) {
#ifdef CONFIG_ARM_STANDBY_CACHE_WAYS
	// Add offset to cache way as first ways are reserved for standby usage
	way += NR_CPUS;
#endif
#ifdef CONFIG_PROFILING_WRITEBACK
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif
	cleanAndInvalidateWay(way);

#ifdef CONFIG_PROFILING_WRITEBACK
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
}

void ARMV7CacheControl::evictMemoryRange(uintptr_t start, uint64_t size) {
	DEBUG_STREAM(TAG,"NOT IMPLEMENTED YET");
}


void ARMV7CacheControl::pl310_LockdownByLine(bool enable) {
	// enable the l2 lockdown by line mechanism which locks all lines after they have been allocated
	dword_t regVal = enable ? 1 : 0;
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_ENABLE, regVal);
}


void ARMV7CacheControl::pl310_LockdownByLineCleanLockdown(uint8_t way) {
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_UNLOCK, (1 << way));

	while (READ_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_UNLOCK)) {}
}

}
