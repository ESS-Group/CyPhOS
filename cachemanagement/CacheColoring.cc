/*
 * CacheColoring.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <cachemanagement/CacheColoring.h>


#define TAG "CacheColoring"
#include <common/debug.h>
#include <common/SyncOutput.h>

#include <driver/GenericMMU.h>

namespace CacheManagement {

CacheColoring::CacheColoring() : GenericCacheManagement() {

}

void CacheColoring::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t* duration) {
	// Move all pages to corresponding color
	size_t pageSize = GenericMMU::sInstance->getPageSize();
	uint32_t colorCount = getColorCount();
	uint32_t pagesPerColor = getPagesPerColor();
	// Force (downwards) alignment of component
	start = start & ~(0xFFF);
	uintptr_t currentPage = start;

	uintptr_t colorOffset = 0;
	uintptr_t pageOffset = 0;

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Preload from: " << hex << start << " to:" << end << " in way: " << dec << (uint32_t)way);
#endif
	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		// Calculates the correct position within the coloring space
		// (way * pageSize * pagesPerColor) : constant offset for virtual "way" (color)
		// (colorOffset * pageSize * colorCount) : offset in color
		uintptr_t colorAdditionOffset = (way * pageSize * pagesPerColor) + pageOffset;
//		DEBUG_STREAM(TAG,"Calculated offset: " << hex << colorAdditionOffset);
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, mColorsStart + colorAdditionOffset, true);

		currentPage += pageSize;
		colorOffset++;

		if (colorOffset % pagesPerColor == 0) {
			pageOffset += pageSize * pagesPerColor * colorCount;
		} else {
			pageOffset += pageSize;
		}
	}
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {

	// Return data from color to its original location

	size_t pageSize = GenericMMU::sInstance->getPageSize();

	// Force (downwards) alignment of component
	uintptr_t start = (uintptr_t)mCacheWays[way].dataStart;
	uintptr_t end = (uintptr_t)mCacheWays[way].dataEnd;

	uintptr_t currentPage = start;

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Evict way: " << dec << (uint32_t)way << " with OSC: " << hex << " from: " << hex << start << " to: " << end);
#endif
	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, currentPage, false);
		currentPage += pageSize;
	}
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Finished evict");
#endif
}

void CacheColoring::evictMemoryRange(uintptr_t start, uint64_t size) {
}

void CacheColoring::printCacheWayInformation() {
	GenericCacheManagement::printCacheWayInformation();
	LOG_OUTPUT_STREAM_START(TAG, "Cache coloring information:" << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Color size: " << hex << getColorSize() << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Color count: " << hex << getColorCount() << endl);
	LOG_OUTPUT_STREAM_END
}

size_t CacheColoring::getCacheWaySize() {
	return getColorSize();
}

#ifndef CONFIG_CACHE_CONTROL_EVICT_AFTER_USE
void CacheColoring::preloadSingleOSC(OSC* osc, cycle_t* duration) {
	// This should only be necessary if no evict after use is enabled because the TLB of one processor
	// could be incoherent to another ones.
	// Flush all TLB entries of OSC to ensure data coherence
	GenericMMU::sInstance->flushTLBWithAddress((uintptr_t)osc);

	uintptr_t oscPAGE = (uintptr_t)osc->getOSCStart();
	uintptr_t oscEnd = (uintptr_t)osc->getOSCEnd();

	size_t pageSize = GenericMMU::sInstance->getPageSize();
	while (oscPAGE < oscEnd) {
		GenericMMU::sInstance->flushTLBWithAddress(oscPAGE);
		oscPAGE += pageSize;
	}
	GenericCacheManagement::preloadSingleOSC(osc,duration);
}
#endif

uint32_t CacheColoring::getColorCount() {
	return getHWCacheWaySize() / (GenericMMU::sInstance->getPageSize() * getPagesPerColor());
}

uint32_t CacheColoring::getColorSize() {
	return GenericMMU::sInstance->getPageSize() * getPagesPerColor() * getHWCacheWayCount();
}

}


