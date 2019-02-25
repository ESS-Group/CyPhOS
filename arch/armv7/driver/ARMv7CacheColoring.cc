/*
 * ARMv7CacheColoring.cc
 *
 *  Created on: 20.02.2019
 *      Author: hendrik
 */

#include <arch/armv7/driver/ARMv7CacheColoring.h>

extern uintptr_t __cache_coloring_start;
namespace CacheManagement {



#ifdef CONFIG_ARMV7_CACHE_COLORING
ARMv7CacheColoring ARMv7CacheColoring::sInstance;
#endif


ARMv7CacheColoring::ARMv7CacheColoring() : CacheColoring() {
	mCacheWayCount = getColorCount();
	mColorsStart = (uintptr_t)&__cache_coloring_start;
}


uint32_t ARMv7CacheColoring::getHWCacheWayCount() {
	return cCACHE_WAY_COUNT;
}

uint32_t ARMv7CacheColoring::getHWCacheWaySize() {
	return cCACHE_WAY_SIZE;
}

uint32_t ARMv7CacheColoring::getPagesPerColor() {
	return cPAGES_PER_COLOR;
}

cycle_t ARMv7CacheColoring::getRAMAccessTimeThreshold() {
	return cRAMAccessTimeThreshold;
}


} /* namespace CacheManagement */
