/*
 * ARMv7CacheColoring.h
 *
 *  Created on: 20.02.2019
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_DRIVER_ARMV7CACHECOLORING_H_
#define ARCH_ARMV7_DRIVER_ARMV7CACHECOLORING_H_

#include <cachemanagement/CacheColoring.h>

namespace CacheManagement {

class ARMv7CacheColoring : public CacheColoring {

public:
	ARMv7CacheColoring();

private:
	static constexpr cycle_t cRAMAccessTimeThreshold = 100;
	static constexpr uint32_t cCACHE_COLOR_COUNT = 64 / 4; // LLC cache-way size / pagesize (64kB/4kB)
	static constexpr uint32_t cCACHE_COLOR_SIZE = 16 * 4096; // LLC cache-way count * 4096b (page size)
#ifdef CONFIG_ARMV7_CACHE_COLORING
	static ARMv7CacheColoring sInstance;
#endif

protected:
	virtual uint32_t getColorCount();
	virtual uint32_t getColorSize();

	virtual cycle_t getRAMAccessTimeThreshold();
};

} /* namespace CacheManagement */

#endif /* ARCH_ARMV7_DRIVER_ARMV7CACHECOLORING_H_ */
