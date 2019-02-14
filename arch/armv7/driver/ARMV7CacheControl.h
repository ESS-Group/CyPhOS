/*
 * ARMV7CacheControl.h
 *
 *  Created on: 20.08.2014
 *      Author: hb
 */

#ifndef ARMV7CACHECONTROL_H_
#define ARMV7CACHECONTROL_H_

#include <common/types.h>
#include <common/ARM_common.h>
#include <common/debug.h>
#include <component/LockedOSC.h>
#include <eventhandling/Trigger.h>
#include <eventhandling/EventTask.h>

#include <sync/Spinlock.h>
#include "PL310_registers.h"

#include <common/memreg.h>

#include <cachemanagement/GenericCacheManagement.h>

// Number of cache ways
#ifndef PL310_CACHE_WAYS
#define PL310_CACHE_WAYS 16
#error "PL310 cache-way count not defined"
#endif


// Inverted bit masks (0 means allocation, 1 lockdown)
#define CORE_0_LOCKDOWN_MASK (~0xCCCC)
#define CORE_1_LOCKDOWN_MASK (~0x3333)

// FIXME separate
// Omap 4460 Monitor Calls (see OMAP4460 TRM p. 5760)
#define OMAP_SMC_WRITE_L2_DEBUG_REGISTER 0x100
#define OMAP_SMC_CACHE_CLEAN_INVALIDATE_RANGE_PHYSICAL 0x101
#define OMAP_SMC_L2_CACHE_CONTROL_REGISTER 0x102
#define OMAP_SMC_L2_AUXILARY_CONTROL_REGISTER 0x109
#define OMAP_SMC_PREFETCH_CONTROL_REGISTER 0x113


#ifndef PL310_ENABLE_FUNC
#define PL310_ENABLE_FUNC for(;;) {}
#error "Platform specific pl310 enable function not defined"
#endif

#ifndef PL310_DISABLE_FUNC
#define PL310_DISABLE_FUNC for(;;) {}
#error "Platform specific pl310 disable function not defined"
#endif


#define LRU_MAX_VALUE 0xFF

// Assembler symbols
extern "C" {
void OSC_FUNC_ASM(CacheControl,asm_enable_instruction_cache)();
void OSC_FUNC_ASM(CacheControl,asm_disable_instruction_cache)();
void OSC_FUNC_ASM(CacheControl,asm_enable_unified_cache)();
void OSC_FUNC_ASM(CacheControl,asm_disable_unified_cache)();
void OSC_FUNC_ASM(CacheControl,asm_clean_and_invalidate_data_caches)();
void OSC_FUNC_ASM(CacheControl,asm_invalidate_data_caches)();
void OSC_FUNC_ASM(CacheControl,asm_clean_data_caches)();
void OSC_FUNC_ASM(CacheControl,asm_invalidate_instruction_cache)();
void armv7_disable_pl310();
}

namespace CacheManagement {

class ARMV7CacheControl : public GenericCacheManagement {

public:
	ARMV7CacheControl();

	// L1 cache registers
	dword_t readCCSIDR();
	dword_t readCLIDR();
	dword_t readCSSELR();
	dword_t readCTR();

	dword_t readPLEID();

	void enableInstructionCache();
	void disableInstructionCache();

	void enableUnifiedCache();
	void disableUnifiedCache();

	void cleanAndInvalidate_Data_Caches();
	void invalidate_Data_Caches();
	void clean_Data_Caches();

	/**
	 * Enables the SMP mode bit for the core calling this function.
	 * This will enable the coherence.
	 */
	void enableSMPMode();

	/**
	 * Disables the SMP mode for the calling core.
	 * This will disable the coherence and should only be used during startup.
	 */
	void disableSMPMode();

	void enablePrefetching();

	// L1 cache register information
	void printCacheInformation();
	// L2 cache controller information
	void pl310_printInformation();


//	/*
//	 * Sends software interrupt across cores for cache clearing on all cores
//	 */
//	void signalClearCaches();

	void enableL1Caches();

	// L2 cache control functions
	/**
	 * Enables the l2 cache controller with invalidate
	 */
	void pl310_enableL2Cache();
	/**
	 * Disables the l2 cache controller with invalidate
	 */
	void pl310_disableL2Cache();
	/**
	 * Invalidates the l2 cache controller
	 */
	void pl310_invalidateSharedCache();

	void pl310_cleanSharedCache();

	/*
	 * Flow prediction control. Controlled with arm coprocessor registers
	 */
	void enableBranchPrediction();
	void disableBranchPrediction();
	void invalidateBranchPrediction();

	void pl310_LockdownByLine(bool enable);

	void pl310_LockdownByLineCleanLockdown(uint8_t way);

//	void pl310LockdownByCore();
//	void pl310LockdownByCore(bool lockdown);

	/**
	 * Locks or unlocks way depending on lockdown value.
	 */
	void pl310_LockCacheWay(uint8_t way, bool lockdown);
#ifdef PL310_LOCKDOWN_BY_MASTER
	void pl310_LockCacheWay_All_Master(uint8_t way, bool lockdown);
#endif



	void swIntHandler();

	void cleanAndInvalidateWay(uint8_t way);

	static inline uint32_t readPerformanceCounter(uint8_t counter) {
		if (counter == 0) {
			return READ_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT0);
		} else if (counter == 1) {
			return READ_REGISTER(PL310_BASE_ADDRESS + PL310_EVENT_COUNTER_COUNT1);
		}
		return 4711;
	}
	/* Static instance variable */
	static ARMV7CacheControl pInstance;
private:
	static constexpr uint8_t cMaxLockdownRegisters = 8;
	static constexpr cycle_t cRamAccessTimeThreshold = 100;

	static constexpr uint32_t cCACHE_WAY_SIZE = 64 * 1024;
	/**
	 * Enum values for synchronous cache operations.
	 */
	enum CACHE_OPERATION {
		ENABLE_L1, DISABLE_L1, CLEAN_AND_INVALIDATE_L1, ENABLE_L2, NO_CACHE_OP
	};

	void setCSSELR(dword_t regContent);
	CACHE_OPERATION mOperation;


	void pl310_enable();
	void pl310_disable();


protected:
	void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration);
	void evictCacheWay(cacheways_t way, cycle_t *duration);
	void evictMemoryRange(uintptr_t start, uint64_t size);

	size_t getCacheWaySize() {return cCACHE_WAY_SIZE;}

	cycle_t getRAMAccessTimeThreshold() {return cRamAccessTimeThreshold;}
};

} /* namespace CacheManagement */

#endif /* ARMV7CACHECONTROL_H_ */
