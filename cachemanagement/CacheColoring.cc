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
//	DEBUG_STREAM(TAG,"Move data from: " << hex << start << " to color: " << dec << way);
	// Move all pages to corresponding color
	size_t pageSize = GenericMMU::sInstance->getPageSize();
//	DEBUG_STREAM(TAG,"Pages necessary:" << dec << (end-start) / pageSize);
	uint32_t colorCount = getColorCount();
	// Force (downwards) alignment of component
	start = start & ~(0xFFF);
	uintptr_t currentPage = start;
	uintptr_t colorOffset = 0;

	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		uintptr_t colorAdditionOffset = (way * pageSize) + (colorOffset * pageSize * colorCount);
//		DEBUG_STREAM(TAG,"Calculated offset: " << hex << colorAdditionOffset);
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, mColorsStart + colorAdditionOffset);
		currentPage += pageSize;
		colorOffset++;
	}

//	DEBUG_STREAM(TAG,"Finished");
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {
//	DEBUG_STREAM(TAG,"Free color: " << dec << way);
	// Return data from color to its original location

	size_t pageSize = GenericMMU::sInstance->getPageSize();
	// Force (downwards) alignment of component
	uintptr_t start = (uintptr_t)mCacheWays[way].oscID->getOSCStart();
	uintptr_t end = (uintptr_t)mCacheWays[way].oscID->getOSCEnd();

	uintptr_t currentPage = start;
	uintptr_t colorOffset = 0;


	// Distribute all OSC pages to color pages
	while (currentPage < end) {
		GenericMMU::sInstance->moveVirtualPageToPhysicalAddress(currentPage, currentPage);
		currentPage += pageSize;
		colorOffset++;
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

