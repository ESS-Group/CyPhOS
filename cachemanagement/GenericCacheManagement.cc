/*
 * GenericCacheManagement.cc
 *
 *  Created on: 17.07.2018
 *      Author: hendrik
 */

#include <cachemanagement/GenericCacheManagement.h>

#include <common/debug.h>
#define TAG "GCacheMgmt"
#include <common/SyncOutput.h>

#include <common/memorytest.h>

#define LRU_MAX_VALUE 0xFF

namespace CacheManagement {

GenericCacheManagement *GenericCacheManagement::sInstance;

GenericCacheManagement::GenericCacheManagement() : mCacheWayCount(0), mCriticalWayPointer(0) {
	CacheManagement::GenericCacheManagement::sInstance = this;

	for(cacheways_t i = 0; i < cMaxCacheWays; i++) {
		mCacheWays[i] = {false,LRU_MAX_VALUE,0x0};
	}
}



void GenericCacheManagement::preloadOSC(OSC* osc, EventHandling::Trigger *trigger, cycle_t *duration) {
	mLock.lock();

	cycle_t duration_sum = 0;
	cycle_t duration_temp = 0;

	/* First get the cache way which is the least recently used one */
	cacheways_t lruCacheWay = getLRUWay(osc);

	/* Determine OSC properties */
	void *oscStart = osc->getOSCStart();
	void *oscEnd = osc->getOSCEnd();
	void *oscTextEnd = osc->getOSCTextEnd();

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG, "OSC start: " << hex << oscStart << " end: " << oscEnd);
	DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)oscEnd - (uintptr_t)oscStart));
	DEBUG_STREAM(TAG, "Preloading OSC: " << hex << (uintptr_t) osc << " in cache way: " << dec << (dword_t)lruCacheWay);
	DEBUG_STREAM(TAG, "OSC start: " << hex << oscStart << " end: " << oscEnd);
	DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)oscEnd - (uintptr_t)oscStart));
#endif


	// Load the data to the specific cache way
	prefetchDataToWay((uintptr_t)oscStart, (uintptr_t)oscEnd, (uintptr_t)oscTextEnd, lruCacheWay,&duration_temp);

#ifdef CONFIG_PROFILING_PRELOAD
	duration_sum += duration_temp;
#endif

#ifdef CONFIG_CACHE_CHECK_MISS_RATE
	uint64_t misses = measureHitRate((uintptr_t)oscStart, (uintptr_t)oscEnd);
	mLastMissRate = ((mLastMissRate * 0.2) + (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)) * 0.8);
//
	if ((misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)) >= 1 ) {
		DEBUG_STREAM(TAG,"Missrate for OSC: " << hex << osc << ": " << dec << (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)));
	}

//	DEBUG_STREAM(TAG,"Misses: " << dec << misses << " access count: " << (((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE));
//	DEBUG_STREAM(TAG,"Missrate: " << dec << (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)) << " for OSC: " << hex << osc);
#endif

	mCacheWays[lruCacheWay].oscID = osc;

	/* Preload dependent OSCs */
	OSC **dep = trigger->pDeps;
	while(*dep != nullptr) {
		lruCacheWay = getLRUWay(*dep);

		/* Determine OSC properties */
		oscStart = (*dep)->getOSCStart();
		oscEnd = (*dep)->getOSCEnd();
		oscTextEnd = (*dep)->getOSCTextEnd();

#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG, "Depending OSC start: " << hex << oscStart << " end: " << oscEnd);
	DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)oscEnd - (uintptr_t)oscStart));

	DEBUG_STREAM(TAG, "Preloading depending OSC: " << hex << (uintptr_t) (*dep) << " in cache way: " << dec << (dword_t)lruCacheWay);
	DEBUG_STREAM(TAG, "OSC start: " << hex << oscStart << " end: " << oscEnd);
	DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)oscEnd - (uintptr_t)oscStart));
#endif


		// Load the data to the specific cache way
		prefetchDataToWay((uintptr_t)oscStart, (uintptr_t)oscEnd, (uintptr_t)oscTextEnd, lruCacheWay, &duration_temp);

#ifdef CONFIG_PROFILING_PRELOAD
		duration_sum += duration_temp;
#endif

#ifdef CONFIG_CACHE_CHECK_MISS_RATE
		misses = measureHitRate((uintptr_t)oscStart, (uintptr_t)oscEnd);

		if ((misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)) >= 1 ) {
			DEBUG_STREAM(TAG,"Missrate for OSC: " << hex << (*dep) << ": " << dec << (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)));
		}
		mLastMissRate = ((mLastMissRate * 0.2) + (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE))* 0.8 );

	// DEBUG_STREAM(TAG,"Misses: " << dec << misses << " access count: " << (((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE));
	//	DEBUG_STREAM(TAG,"Missrate: " << dec << (misses * 100/(((uintptr_t)oscEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE)) << " for OSC: " << hex << osc);
#endif
		mCacheWays[lruCacheWay].oscID = (*dep);


		dep++;
	}

	mLock.unlock();

	*duration = duration_sum;
}



void GenericCacheManagement::preloadCriticalData(void* start, void* end, void* textEnd) {
	mLock.lock();
	DEBUG_STREAM(TAG, "Preloading Critical data from " << hex << start << " to " << end << " text segment ending at: " << textEnd);
	/* Determine OSC properties */
	uintptr_t oscStart = (uintptr_t) start;
	uintptr_t oscEnd = (uintptr_t) end;

	uintptr_t preloadEnd;

	while (oscStart < oscEnd) {
		// FIXME use config option
		preloadEnd = oscStart + (CONFIG_CACHE_WAY_SIZE - 4);
		if (preloadEnd > oscEnd) {
			preloadEnd = oscEnd;
		}
		DEBUG_STREAM(TAG, "Preloading Critical OSC: " << hex << " in cache way: " << dec << (dword_t)mCriticalWayPointer);
		DEBUG_STREAM(TAG, "OSC start: " << hex << oscStart << " end: " << preloadEnd);
		DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)preloadEnd - (uintptr_t)oscStart));

		// Load the data to the specific cache way
		cycle_t duration;
		prefetchDataToWay((uintptr_t)oscStart, (uintptr_t)preloadEnd, (uintptr_t)textEnd <= preloadEnd ? (uintptr_t)textEnd : (uintptr_t)preloadEnd, mCriticalWayPointer, &duration);
		uint64_t misses = measureHitRate((uintptr_t)oscStart, (uintptr_t)preloadEnd);
		DEBUG_STREAM(TAG, "Misses: " << dec << misses);
		DEBUG_STREAM(TAG, "Miss rate: " << dec << (misses * 100)/(((uintptr_t)preloadEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE) << "%");

		mCacheWays[mCriticalWayPointer].oscID = 0;
		mCacheWays[mCriticalWayPointer].permanentLocked = true;

		mCriticalWayPointer++;
		oscStart = preloadEnd + 4;
		if (oscStart > oscEnd) {
			oscStart = oscEnd;
		}
	}

	mLock.unlock();
}


void GenericCacheManagement::evictOSC(EventHandling::EventTask* eventtask, cycle_t *duration) {
	mLock.lock();

	cycle_t duration_temp = 0, duration_sum = 0;

	// Evict main OSC
	lookupAndEvictOSC(eventtask->trigger->pDestinationOSC, &duration_temp);

#ifdef CONFIG_PROFILING_WRITEBACK
	duration_sum += duration_temp;
#endif

	// Evict dependent OSCs
	OSC **dep = eventtask->trigger->pDeps;
	while(*dep != nullptr) {
		lookupAndEvictOSC((*dep), &duration_temp);

#ifdef CONFIG_PROFILING_WRITEBACK
	duration_sum += duration_temp;
#endif

		dep++;
	}

	*duration = duration_sum;
	mLock.unlock();
}

cacheways_t GenericCacheManagement::getLRUWay(OSC* osc) {
	/* Iterate over all cache ways and return least recently used cache way */
	uint8_t maxLRUcount = 0;
	uint8_t maxCacheWay = 0;


	for (uint8_t way = 0; way < mCacheWayCount; way++) {
		/* Check if OSC is already in way */
		if (mCacheWays[way].oscID == osc) {
			/* Reset LRU count because it is used */
			mCacheWays[way].lruCount = 0;
			return way;
		}


		/* Get the way and compare with max search */
		uint8_t lruCount = mCacheWays[way].lruCount;
		/* Only check if it is not a permanently locked way */
		// FIXME only evict OSCs that have refCount==0
		// Check if the cache way holds a component Heap, which is not used anymore
		if ((!mCacheWays[way].permanentLocked) && ((mCacheWays[way].oscID == nullptr))) {
			if (lruCount > maxLRUcount) {
				maxLRUcount = lruCount;
				maxCacheWay = way;
			}
			if (mCacheWays[way].lruCount != LRU_MAX_VALUE) {
				mCacheWays[way].lruCount++;
			}
		}
	}

	mCacheWays[maxCacheWay].lruCount = 0;
	return maxCacheWay;

}

void GenericCacheManagement::lookupAndEvictOSC(OSC *osc, cycle_t *duration) {
#ifdef CONFIG_CACHE_DEBUG
			DEBUG_STREAM(TAG, "Evict OSC: " << hex << osc);
#endif
	for(cacheways_t cacheWay = 0; cacheWay < mCacheWayCount; cacheWay++) {
		if (mCacheWays[cacheWay].oscID ==osc) {
#ifdef CONFIG_CACHE_DEBUG
			DEBUG_STREAM(TAG, "Free cache way: " << dec << (uint16_t) cacheWay << " from OSC: " << hex << osc);
#endif

			evictCacheWay(cacheWay,duration);
			mCacheWays[cacheWay].oscID = nullptr;
			mCacheWays[cacheWay].lruCount = 0;
			return;
		}
	}
}


void GenericCacheManagement::printCacheWayInformation() {
	LOG_OUTPUT_STREAM_START(TAG, "Cache ways:" << dec << (uint32_t)mCacheWayCount << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG,"avg. missrate: " << dec << mLastMissRate << endl);
	for (cacheways_t i = 0; i < mCacheWayCount; i++) {
		LOG_OUTPUT_STREAM_CONTINUE(TAG,
				"Cache way " << dec << (dword_t) i << " permanently locked: " << mCacheWays[i].permanentLocked
						<< " with id: " << hex << mCacheWays[i].oscID << endl);
	}
	LOG_OUTPUT_STREAM_END;
}

uint64_t GenericCacheManagement::measureHitRate(uintptr_t start, uintptr_t end) {
	uint64_t retVal = 0;

	cycle_t ramThreshold = getRAMAccessTimeThreshold();
	for (uintptr_t address = start; address < end; address += CONFIG_CACHE_LINE_SIZE) {
		cycle_t accessTime = 0;
		MEASURE_MEMORY_ACCESS(address, accessTime);
		if (accessTime >= ramThreshold) {
			retVal++;
		}
	}
	return retVal;
}

} /* namespace CacheManagement */

