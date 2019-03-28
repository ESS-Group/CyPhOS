/*
 * CacheColoring.h
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#ifndef CACHEMANAGEMENT_CACHECOLORING_H_
#define CACHEMANAGEMENT_CACHECOLORING_H_

#include "GenericCacheManagement.h"
#include <sync/Spinlock.h>

namespace CacheManagement {

class CacheColoring : public GenericCacheManagement {

public:
	CacheColoring();


protected:
	/**
	 * HW implementations need to define how many hardware cache ways a system has for the shared level cache (LLC)
	 * @return Number of LLC cache ways
	 */
	virtual uint32_t getHWCacheWayCount() = 0;

	/**
	 * HW implementations need to define how large one cache way of the LLC is
	 * @return Size in bytes of the LLC cache ways
	 */
	virtual uint32_t getHWCacheWaySize() = 0;

	/**
	 * Controls how many pages per color the cache coloring uses. This can be used to prevent private
	 * cache starvation. If you use to few the private caches are not used in full capacity.
	 * @return
	 */
	virtual uint32_t getPagesPerColor() = 0;

	/**
	 * Calculates how many colors are available for the given system.
	 * @return Number of cache colors available for the system
	 */
	uint32_t getColorCount();

	/**
	 *
	 * @return Size in bytes of a cache color
	 */
	uint32_t getColorSize();

	/**
	 * Start address where the cache coloring system moves virtual pages to the PHYSICAL memory
	 */
	uintptr_t mColorsStart;

	/**
	 * Preloads a memory range to a given cache way. Needs to be implemented by the architecture
	 * cache management sub class.
	 * @param start Start memory address
	 * @param end End memory address
	 * @param textEnd End of text segment within the whole memory range. (Text at the beginning)
	 * @param way Cache way to be used for the data.
	 */
	virtual void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration);

	/**
	 * Evicts data from a given cache color by returning it back to its physical address
	 * @param way Cache color ("virtual way") to evict data from
	 */
	virtual void evictCacheWay(cacheways_t way, cycle_t *duration);

	/**
	 * Evicts a given memory range from the cache
	 * @param start Start address to evict
	 * @param size End address to evict
	 */
	virtual void evictMemoryRange(uintptr_t start, uint64_t size);

	virtual void printCacheWayInformation();

	/**
	 * Overriden method for the abstract cache management of CyPhOS. This is the size of a cache color for a "virtual" cache management way.
	 * Not to be confused with the hardware cache way size
	 * @return
	 */
	virtual size_t getCacheWaySize();

#ifndef CONFIG_CACHE_CONTROL_EVICT_AFTER_USE
	/**
	 * Overrides the preload method to flush TLB before reusing a cache color.
	 * Otherwise the data TLB data my be incoherent.
	 */
	virtual void preloadSingleOSC(OSC *osc, cycle_t *duration);
#endif
private:

#ifdef CONFIG_SEQUENTIAL_COLORING
	Spinlock mSequentialLock;
#endif
};

}

#endif /* CACHEMANAGEMENT_CACHECOLORING_H_ */
