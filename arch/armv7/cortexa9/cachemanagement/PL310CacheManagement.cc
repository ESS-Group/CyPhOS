/*
 * PL310CacheManagement.cc
 *
 *  Created on: 21.02.2019
 *      Author: hendrik
 */

#include <arch/armv7/cortexa9/cachemanagement/PL310CacheManagement.h>

#include <driver/PL310_registers.h>

#include <driver/ARMV7CacheControl.h>
#include <common/ARM_common.h>
#include <common/armhelper.h>



#define TAG "PL310"
#include <common/debug.h>


extern "C" {
void armv7_disable_pl310() {
	CacheManagement::PL310CacheManagement::sInstance.setEnabled(false);
}
}


namespace CacheManagement {
/**
 * Static singleton instance
 */
PL310CacheManagement PL310CacheManagement::sInstance;

PL310CacheManagement::PL310CacheManagement()
#if !defined(CONFIG_ARMV7_CACHE_COLORING)
		: GenericCacheManagement()
#endif
{
#ifndef CONFIG_ARMV7_CACHE_COLORING
	mCacheWayCount = cCACHE_WAY_COUNT;

#if defined(CONFIG_ARM_STANDBY_CACHE_WAYS)
	// Reduce cache-way count by the number of cpus for standby usage
	mCacheWayCount -= NR_CPUS;
#endif
#endif
}


#if !defined(CONFIG_ARMV7_CACHE_COLORING)
void PL310CacheManagement::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t* duration) {
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

void PL310CacheManagement::evictCacheWay(cacheways_t way, cycle_t* duration) {
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


void PL310CacheManagement::evictMemoryRange(uintptr_t start, uint64_t size) {
	DEBUG_STREAM(TAG,"NOT IMPLEMENTED YET");
}

#endif // !defined(CONFIG_ARMV7_CACHE_COLORING)

void PL310CacheManagement::cleanAndInvalidateWay(cacheways_t way) {
	// Prevent other cores from using the l2 cache way that is currently written back to the main memory

	// Clean data through the cache hierarchy l1->l2->memory
	ARMV7CacheControl::pInstance.clean_Data_Caches();
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

void PL310CacheManagement::pl310_LockdownByLine(bool enable) {
	// enable the l2 lockdown by line mechanism which locks all lines after they have been allocated
	dword_t regVal = enable ? 1 : 0;
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_ENABLE, regVal);
}

void PL310CacheManagement::pl310_LockdownByLineCleanLockdown(cacheways_t way) {
	WRITE_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_UNLOCK, (1 << way));

	while (READ_REGISTER(PL310_BASE_ADDRESS + PL310_LOCKDOWN_BY_LINE_UNLOCK)) {}
}

void PL310CacheManagement::setEnabled(bool enable) {
	if (enable) {
		invalidateSharedCache();
		PL310_ENABLE_FUNC();
		BUSY_WAITING_SHORT;

#if defined(CONFIG_CACHE_CONTROL) & !defined(CONFIG_ARMV7_CACHE_COLORING)
	/* Cleanup management structure */
	for ( cacheways_t i = 0; i < PL310_CACHE_WAYS; i++ ) {
#ifdef PL310_LOCKDOWN_BY_MASTER
		pl310_LockCacheWay_All_Master(i,true);
#else
		pl310_LockCacheWay(i, true);
#endif

	}
#endif
//
//	for (cacheways_t i = 0; i < PL310_CACHE_WAYS; i++ ) {
//		// Clean all ways
//		cleanAndInvalidateWay(i);
//	}
#ifdef PL310_LOCKDOWN_BY_LINE
	pl310_LockdownByLine(true);
#endif

	} else {
		cleanAndInvalidateSharedCache();

		PL310_DISABLE_FUNC();
	}
}

void PL310CacheManagement::invalidateSharedCache() {
	DSB;
	dword_t regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_INVALIDATE_BY_WAY);
	regVal = 0xffff;
	WRITE_REGISTER(PL310_BASE_ADDRESS+PL310_INVALIDATE_BY_WAY, regVal);
	// Wait for completion
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_INVALIDATE_BY_WAY) != 0) {}

	// Sync the PL310
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}
}

void PL310CacheManagement::cleanSharedCache() {
	DSB;
	dword_t regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_BY_WAY);
	regVal = 0xffff;
	WRITE_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_BY_WAY, regVal);
	// Wait for completion
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_CLEAN_BY_WAY) != 0) {}

	// Sync the PL310
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}
}

void PL310CacheManagement::cleanAndInvalidateSharedCache() {
	DSB;
	dword_t regVal = READ_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_AND_INVALIDATE_BY_WAY);
	regVal = 0xffff;
	WRITE_REGISTER(PL310_BASE_ADDRESS+PL310_CLEAN_AND_INVALIDATE_BY_WAY, regVal);
	// Wait for completion
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_CLEAN_AND_INVALIDATE_BY_WAY) != 0) {}

	// Sync the PL310
	WRITE_REGISTER((PL310_BASE_ADDRESS + PL310_SYNC), 0x1);
	while(READ_REGISTER(PL310_BASE_ADDRESS + PL310_SYNC) != 0) {}
}

void PL310CacheManagement::printInformation() {
	DEBUG_STREAM(TAG,"Printing PL310 Level2 controller information" ); DEBUG_STREAM(TAG,"reg0_cache_id: " << hex
			<< (uint32_t) READ_REGISTER(PL310_BASE_ADDRESS+PL310_CACHE_ID)
	);
	DEBUG_STREAM(TAG,"reg0_cache_type: " << hex
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


void PL310CacheManagement::pl310_LockCacheWay(uint8_t way, bool lockdown) {
	uint32_t offset = 0;
#ifdef PL310_LOCKDOWN_BY_MASTER
	offset = getCPUID();
#endif

	dword_t lockdownRegisterD = READ_REGISTER(PL310_D_LOCKDOWN__(offset));
	dword_t lockdownRegisterI = READ_REGISTER(PL310_I_LOCKDOWN__(offset));

	lockdownRegisterD = lockdown ? (lockdownRegisterD | (0x1 << way)) : (lockdownRegisterD & ~(0x1 << way));
	lockdownRegisterI = lockdown ? (lockdownRegisterI | (0x1 << way)) : (lockdownRegisterI & ~(0x1 << way));

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
void PL310CacheManagement::pl310_LockCacheWay_All_Master(uint8_t way, bool lockdown) {
	// FIXME this will for now only work if lockdown by master is unimplemented and lockdown is
	// handled by lockdown register 0

	for(uint32_t offset=0; offset<cMaxLockdownRegisters; offset++) {

		dword_t lockdownRegisterD = READ_REGISTER(PL310_D_LOCKDOWN__(offset));
		dword_t lockdownRegisterI = READ_REGISTER(PL310_I_LOCKDOWN__(offset));

		lockdownRegisterD = lockdown ? (lockdownRegisterD | (0x1 << way)) : (lockdownRegisterD & ~(0x1 << way));
		lockdownRegisterI = lockdown ? (lockdownRegisterI | (0x1 << way)) : (lockdownRegisterI & ~(0x1 << way));

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

} /* namespace CacheManagement */

