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

namespace CacheManagement {

CacheColoring::CacheColoring() : GenericCacheManagement() {

}

void CacheColoring::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t* duration) {
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {
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

}

