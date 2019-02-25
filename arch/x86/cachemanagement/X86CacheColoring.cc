/*
 * X86CacheColoring.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <arch/x86/cachemanagement/X86CacheColoring.h>

extern uintptr_t __cache_coloring_start;
namespace CacheManagement {

#ifdef CONFIG_AMD64_CACHE_COLORING
X86CacheColoring X86CacheColoring::sInstance;
#endif



X86CacheColoring::X86CacheColoring() : CacheColoring() {
	mCacheWayCount = getColorCount();
	mColorsStart = (uintptr_t)&__cache_coloring_start;
}

uint32_t X86CacheColoring::getHWCacheWayCount() {
	return cCACHE_WAY_COUNT;
}

uint32_t X86CacheColoring::getHWCacheWaySize() {
	return cCACHE_WAY_SIZE;
}

uint32_t X86CacheColoring::getPagesPerColor() {
	return cPAGES_PER_COLOR;
}

cycle_t X86CacheColoring::getRAMAccessTimeThreshold() {
	return cRAMAccessTimeThreshold;
}

} /* namespace CacheManagement */
