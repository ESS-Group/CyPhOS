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
	/**
	 * Cache coloring for the PL310 LLC cache
	 */
	static constexpr uint32_t cCACHE_WAY_COUNT = PL310_CACHE_WAYS;
	/**
	 * Cache way size of the hardware board that is used
	 */
	static constexpr uint32_t cCACHE_WAY_SIZE = CONFIG_CACHE_WAY_SIZE;
	/**
	 * Use more than one page per color to enable more use of the private cache
	 */
	static constexpr uint32_t cPAGES_PER_COLOR = 4;

#ifdef CONFIG_ARMV7_CACHE_COLORING
	/**
	 * Singleton instance only enabled if cache coloring is enabled
	 */
	static ARMv7CacheColoring sInstance;
#endif

protected:
	virtual uint32_t getHWCacheWayCount();
	virtual uint32_t getHWCacheWaySize();
	virtual uint32_t getPagesPerColor();

	virtual cycle_t getRAMAccessTimeThreshold();
};

} /* namespace CacheManagement */

#endif /* ARCH_ARMV7_DRIVER_ARMV7CACHECOLORING_H_ */
