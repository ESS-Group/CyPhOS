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

uint32_t X86CacheColoring::getColorCount() {
	return cCACHE_COLOR_COUNT;
}

uint32_t X86CacheColoring::getColorSize() {
	return cCACHE_COLOR_SIZE;
}

cycle_t X86CacheColoring::getRAMAccessTimeThreshold() {
	return cRAMAccessTimeThreshold;
}

} /* namespace CacheManagement */
