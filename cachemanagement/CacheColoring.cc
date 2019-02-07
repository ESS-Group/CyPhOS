/*
 * CacheColoring.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <cachemanagement/CacheColoring.h>

namespace CacheManagement {

CacheColoring::CacheColoring() : GenericCacheManagement() {

}

void CacheColoring::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t* duration) {
}

void CacheColoring::evictCacheWay(cacheways_t way, cycle_t* duration) {
}

void CacheColoring::evictMemoryRange(uintptr_t start, uint64_t size) {
}

}

