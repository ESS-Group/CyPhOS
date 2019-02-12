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
	// Force (downwards) alignment of component
	start = start & ~(0xFFF);
	uintptr_t currentPage = start;
	uintptr_t colorOffset = 0;

	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		// Calculates the correct position within the coloring space
		uintptr_t colorAdditionOffset = (way * pageSize) + (colorOffset * pageSize * colorCount);

		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, mColorsStart + colorAdditionOffset, true);

		currentPage += pageSize;
		colorOffset++;
	}
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {
	// Return data from color to its original location

	size_t pageSize = GenericMMU::sInstance->getPageSize();

	// Force (downwards) alignment of component
	uintptr_t start = (uintptr_t)mCacheWays[way].dataStart;
	uintptr_t end = start + getColorSize();

	uintptr_t currentPage = start;

	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, currentPage, false);
		currentPage += pageSize;
	}
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

void CacheColoring::distributeDataOverColor(uintptr_t start, uintptr_t end, cacheways_t color) {

}

}

