/*
 * CacheColoring.h
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#ifndef CACHEMANAGEMENT_CACHECOLORING_H_
#define CACHEMANAGEMENT_CACHECOLORING_H_

#include "GenericCacheManagement.h"

namespace CacheManagement {

class CacheColoring : public GenericCacheManagement {

public:
	CacheColoring();


protected:
	virtual uint32_t getColorCount() = 0;
	virtual uint32_t getColorSize() = 0;

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
	 * Evicts data from a given cache way
	 * @param way Way to evict data from
	 */
	virtual void evictCacheWay(cacheways_t way, cycle_t *duration);

	/**
	 * Evicts a given memory range from the cache
	 * @param start Start address to evict
	 * @param size End address to evict
	 */
	virtual void evictMemoryRange(uintptr_t start, uint64_t size);

	virtual void printCacheWayInformation();

	virtual size_t getCacheWaySize();

private:
	void distributeDataOverColor(uintptr_t start, uintptr_t end);
};

}

#endif /* CACHEMANAGEMENT_CACHECOLORING_H_ */
