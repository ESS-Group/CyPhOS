/*
 * GenericCacheManagement.h
 *
 *  Created on: 17.07.2018
 *      Author: hendrik
 */

#ifndef CACHEMANAGEMENT_GENERICCACHEMANAGEMENT_H_
#define CACHEMANAGEMENT_GENERICCACHEMANAGEMENT_H_

#include <common/types.h>

#include <component/OSC.h>
#include <eventhandling/Trigger.h>
#include <eventhandling/EventTask.h>

#include <sync/Spinlock.h>

namespace CacheManagement {

typedef uint32_t cacheways_t;

class GenericCacheManagement {
protected:
	struct CACHE_WAY_STATE_t {
			bool permanentLocked;
			uint8_t lruCount;
			OSC *oscID;
			uintptr_t dataStart;
			bool inUse;
		};

public:
	GenericCacheManagement();

	/**
	 * Pointer to static instance of the architectures active cache management
	 */
	static GenericCacheManagement *sInstance;

	/**
	 * Preloads the data of a component to the cache
	 * @param osc Component to be preloaded
	 * @param trigger executing Trigger of the component, used to determine dependent components
	 */
	void preloadOSC(OSC *osc, EventHandling::Trigger *trigger, cycle_t *duration);

	/**
	 * Preloads data that will be marked as critical (no eviction after preloading) to the cache
	 * @param start Start address of the data range
	 * @param end End address of the data range
	 * @param textEnd End address of a text segment, must be within the data range
	 */
	void preloadCriticalData(void *start, void *end, void *textEnd);

	/**
	 * Evicts the OSC and all dependent components of the given EventTask from the cache
	 * @param eventtask EventTask of running trigger
	 */
	void evictOSC(EventHandling::EventTask *eventtask, cycle_t *duration);


	/**
	 * Virtual method (to be implemented by architecture) that prints information
	 * of the cache usage for debugging.
	 */
	virtual void printCacheWayInformation();
protected:
	/**
	 * Defined maximum of cache ways in a system
	 */
	static constexpr cacheways_t cMaxCacheWays = 20;

	/**
	 * Actual count of cache ways in the running system
	 */
	cacheways_t mCacheWayCount;

	/**
	 * Pointer to the last used cache way for critical data (used during initialization)
	 */
	cacheways_t mCriticalWayPointer;

	/**
	 * Array of status variables for all the available cache ways of a system
	 */
	CACHE_WAY_STATE_t mCacheWays[cMaxCacheWays];

	/**
	 * Spinlock used to ensure only one processor at a time utilizes/changes the cache state
	 */
	Spinlock mLock;

	/**
	 * Preloads a memory range to a given cache way. Needs to be implemented by the architecture
	 * cache management sub class.
	 * @param start Start memory address
	 * @param end End memory address
	 * @param textEnd End of text segment within the whole memory range. (Text at the beginning)
	 * @param way Cache way to be used for the data.
	 */
	virtual void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration) = 0;

	/**
	 * Evicts data from a given cache way
	 * @param way Way to evict data from
	 */
	virtual void evictCacheWay(cacheways_t way, cycle_t *duration) = 0;

	/**
	 * Evicts a given memory range from the cache
	 * @param start Start address to evict
	 * @param size End address to evict
	 */
	virtual void evictMemoryRange(uintptr_t start, uint64_t size) = 0;

	uint64_t measureHitRate(uintptr_t start, uintptr_t end);

	/**
	 * Determines which cache way to use for the next preload component
	 * @param osc Component to be preloaded to the cache.
	 * @return
	 */
	cacheways_t getLRUWay(OSC *osc, uintptr_t dataStart);

	virtual cycle_t getRAMAccessTimeThreshold() = 0;

private:
	uint64_t mLastMissRate = 0;

	uint64_t mEvictionCount = 0;

	void lookupAndEvictOSC(OSC *osc, cycle_t *duration);

	void preloadSingleOSC(OSC *osc, cycle_t *duration);
};

} /* namespace CacheManagement */

#endif /* CACHEMANAGEMENT_GENERICCACHEMANAGEMENT_H_ */
