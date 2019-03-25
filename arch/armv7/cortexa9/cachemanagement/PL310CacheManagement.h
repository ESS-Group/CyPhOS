/*
 * PL310CacheManagement.h
 *
 *  Created on: 21.02.2019
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_CORTEXA9_CACHEMANAGEMENT_PL310CACHEMANAGEMENT_H_
#define ARCH_ARMV7_CORTEXA9_CACHEMANAGEMENT_PL310CACHEMANAGEMENT_H_

#include <cachemanagement/GenericCacheManagement.h>

namespace CacheManagement {

class PL310CacheManagement
#if !defined(CONFIG_ARMV7_CACHE_COLORING)
		: public GenericCacheManagement
#endif
{

public:
	PL310CacheManagement();

	/**
	 * Enables/disables the level 2 PL310 cache controller
	 * @param enable Sets if LLC should be enabled
	 */
	void setEnabled(bool enable);

	/**
	 * Invalidates the l2 cache controller
	 */
	void invalidateSharedCache();

	void cleanSharedCache();
	void cleanAndInvalidateSharedCache();


	void printInformation();

	/**
	 * Method to print information of the current state of the managed cache.
	 */
	virtual void printCacheWayInformation();

	static PL310CacheManagement sInstance;
private:
	static constexpr cacheways_t cCACHE_WAY_COUNT = 16;
	static constexpr uint32_t cCACHE_WAY_SIZE = 64 * 1024;
	static constexpr cycle_t cRamAccessTimeThreshold = 100;
	static constexpr uint8_t cMaxLockdownRegisters = 8;

	/**
	 * Clean & invalidate of a L2 shared cache-way. This needs to be done with prefetch lock taken, otherwise
	 * data corruption may occur!
	 * @param way Cache way to clean
	 */
	void cleanAndInvalidateWay(cacheways_t way);

	void pl310_LockdownByLine(bool enable);

	void pl310_LockdownByLineCleanLockdown(cacheways_t way);

	void pl310_LockCacheWay(uint8_t way, bool lockdown);
	void pl310_LockCacheWay_All_Master(uint8_t way, bool lockdown);

	Spinlock pPL310Lock;
protected:
	size_t getCacheWaySize() {return cCACHE_WAY_SIZE;}

	cycle_t getRAMAccessTimeThreshold() {return cRamAccessTimeThreshold;}

#if !defined(CONFIG_ARMV7_CACHE_COLORING)
	void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration);
	void evictCacheWay(cacheways_t way, cycle_t *duration);
	void evictMemoryRange(uintptr_t start, uint64_t size);
#endif
};

} /* namespace CacheManagement */

#endif /* ARCH_ARMV7_CORTEXA9_CACHEMANAGEMENT_PL310CACHEMANAGEMENT_H_ */
