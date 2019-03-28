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

#include <common/cyclecounter.h>

#include <driver/GenericMMU.h>

namespace CacheManagement {

CacheColoring::CacheColoring() : GenericCacheManagement() {

}

void CacheColoring::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t* duration) {
#ifdef CONFIG_SEQUENTIAL_COLORING
	mSequentialLock.lock();
#endif
#ifdef CONFIG_PROFILING_PRELOAD
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Copy data to way: " << hex << way << " from: " << start << " to: " << end);
#endif
	// Move all pages to corresponding color
	size_t pageSize = GenericMMU::sInstance->getPageSize();
	uint32_t colorCount = getColorCount();
	uint32_t pagesPerColor = getPagesPerColor();
	// Force (downwards) alignment of component
	start = start & ~(pageSize - 1);
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
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, mColorsStart + colorAdditionOffset, true);

		// Increase source page pointer
		currentPage += pageSize;

		// Increase page counter
		colorOffset++;

		if (colorOffset % pagesPerColor == 0) {
			// If pages per color is exhausted move to next space of the cache color
			pageOffset += pageSize * pagesPerColor * colorCount;
		} else {
			// A contiguous page is still available for the color.
			pageOffset += pageSize;
		}
	}
#ifdef CONFIG_PROFILING_PRELOAD
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
#ifdef CONFIG_SEQUENTIAL_COLORING
	mSequentialLock.unlock();
#endif
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {
#ifdef CONFIG_SEQUENTIAL_COLORING
	mSequentialLock.lock();
#endif
#ifdef CONFIG_PROFILING_WRITEBACK
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif
	// Return data from color to its original location

	size_t pageSize = GenericMMU::sInstance->getPageSize();

	// Force (downwards) alignment of component
	uintptr_t start = (uintptr_t)mCacheWays[way].dataStart;
	uintptr_t end = (uintptr_t)mCacheWays[way].dataEnd;

	uintptr_t currentPage = start;

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Evict way: " << dec << (uint32_t)way << " with OSC: " << hex << " from: " << hex << start << " to: " << end);
#endif
	// Distribute all OSC pages back to their original physical address (virtual address == physical address)
	while (currentPage < end) {
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, currentPage, false);
		currentPage += pageSize;
	}
#ifdef CONFIG_PROFILING_WRITEBACK
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Finished evict");
#endif
#ifdef CONFIG_SEQUENTIAL_COLORING
	mSequentialLock.unlock();
#endif
}

void CacheColoring::evictMemoryRange(uintptr_t start, uint64_t size) {
}

void CacheColoring::printCacheWayInformation() {
	GenericCacheManagement::printCacheWayInformation();
	LOG_OUTPUT_STREAM_START(TAG, "Cache coloring information:" << endl);
#ifdef CONFIG_CACHE_CONTROL
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "SW-based page coloring cache management" << endl);
#else
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "no SW-based page coloring cache management" << endl);
#endif
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Color size: " << hex << getColorSize() << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Color count: " << hex << getColorCount() << endl);
	LOG_OUTPUT_STREAM_END
}

size_t CacheColoring::getCacheWaySize() {
	// The size of a "virtual" cache way for th ecache management abstraction
	return getColorSize();
}

#ifndef CONFIG_CACHE_CONTROL_EVICT_AFTER_USE
void CacheColoring::preloadSingleOSC(OSC* osc, cycle_t* duration) {
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"preloadSingleOSC:" << hex << osc);
#endif
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
	// Divide the LLC cache into colors
	return getHWCacheWaySize() / (GenericMMU::sInstance->getPageSize() * getPagesPerColor());
}

uint32_t CacheColoring::getColorSize() {
	// Calculate the size of a cache color.
	return GenericMMU::sInstance->getPageSize() * getPagesPerColor() * getHWCacheWayCount();
}

}


