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

	preloadSingleOSC(osc, duration);
	/* Preload dependent OSCs */
	OSC **dep = trigger->pDeps;
	while(*dep != nullptr) {
		preloadSingleOSC((*dep), &duration_temp);
#ifdef CONFIG_PROFILING_PRELOAD
	duration_sum += duration_temp;
#endif
		dep++;
	}

	mLock.unlock();

	*duration = duration_sum;
}



void GenericCacheManagement::preloadCriticalData(void* start, void* end, void* textEnd) {
	mLock.lock();
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG, "Preloading Critical data from " << hex << start << " to " << end << " text segment ending at: " << textEnd);
#endif
	/* Determine OSC properties */
	uintptr_t oscStart = (uintptr_t) start;
	uintptr_t oscEnd = (uintptr_t) end;

	uintptr_t preloadEnd;

	while (oscStart < oscEnd) {
		// FIXME use config option
		preloadEnd = oscStart + (getCacheWaySize() - 4);
		if (preloadEnd > oscEnd) {
			preloadEnd = oscEnd;
		}
#ifdef CONFIG_CACHE_DEBUG
		DEBUG_STREAM(TAG, "Preloading Critical OSC: " << hex << " in cache way: " << dec << (dword_t)mCriticalWayPointer);
		DEBUG_STREAM(TAG, "OSC start: " << hex << oscStart << " end: " << preloadEnd);
		DEBUG_STREAM(TAG, "OSC size in byte: " << dec << ((uintptr_t)preloadEnd - (uintptr_t)oscStart));
#endif

		// Load the data to the specific cache way
		cycle_t duration;
		prefetchDataToWay((uintptr_t)oscStart, (uintptr_t)preloadEnd, (uintptr_t)textEnd <= preloadEnd ? (uintptr_t)textEnd : (uintptr_t)preloadEnd, mCriticalWayPointer, &duration);
#ifdef CONFIG_CACHE_DEBUG
		uint64_t misses = measureHitRate((uintptr_t)oscStart, (uintptr_t)preloadEnd);
		DEBUG_STREAM(TAG, "Misses: " << dec << misses);
		DEBUG_STREAM(TAG, "Miss rate: " << dec << (misses * 100)/(((uintptr_t)preloadEnd - (uintptr_t)oscStart)/CONFIG_CACHE_LINE_SIZE) << "%");
#endif

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

cacheways_t GenericCacheManagement::getLRUWay(OSC* osc, uintptr_t dataStart) {
	/* Iterate over all cache ways and return least recently used cache way */
	uint8_t maxLRUcount = 0;
	cacheways_t maxCacheWay = 0;

	bool validWay = false;

	// Check if OSC is already in a cache way
	for (cacheways_t way = 0; way < mCacheWayCount; way++) {
		/* Check if OSC is already in way */
		if (mCacheWays[way].oscID == osc && mCacheWays[way].dataStart == dataStart) {
			/* Reset LRU count because it is used */
			mCacheWays[way].lruCount = 0;
			return way;
		}
	}

	for (cacheways_t way = 0; way < mCacheWayCount; way++) {
		/* Get the way and compare with max search */
		uint8_t lruCount = mCacheWays[way].lruCount;
		/* Only check if it is not a permanently locked way */
		// Check if the cache way holds a component Heap, which is not used anymore
		if ((!mCacheWays[way].permanentLocked) && (!mCacheWays[way].inUse)) {
			if (mCacheWays[way].oscID == nullptr) {
				// Always try to use free ways first
				maxCacheWay = way;
				validWay = true;
				break;
			}

			if (lruCount >= maxLRUcount) {
				maxLRUcount = lruCount;
				maxCacheWay = way;
				validWay = true;
			}
			if (mCacheWays[way].lruCount != LRU_MAX_VALUE) {
				mCacheWays[way].lruCount++;
			}
		}
	}

	// Only return a cache way if a valid way is free
	if(validWay) {
		if(mCacheWays[maxCacheWay].oscID != nullptr) {
			cycle_t duration;
			evictCacheWay(maxCacheWay,&duration);
			mEvictionCount++;
		}
		mCacheWays[maxCacheWay].lruCount = 0;
		mCacheWays[maxCacheWay].oscID = nullptr;
		mCacheWays[maxCacheWay].dataStart = 0;
		return maxCacheWay;
	} else {
		// Otherwise return this constant abort execution (SHOULD NOT HAPPEN)
		return cMaxCacheWays;
	}
}

void GenericCacheManagement::lookupAndEvictOSC(OSC *osc, cycle_t *duration) {
#ifdef CONFIG_CACHE_DEBUG
		DEBUG_STREAM(TAG, "Evict OSC: " << hex << osc);
#endif
	for(cacheways_t cacheWay = 0; cacheWay < mCacheWayCount; cacheWay++) {
		if (mCacheWays[cacheWay].oscID == osc) {
#ifdef CONFIG_CACHE_DEBUG
			DEBUG_STREAM(TAG, "Free cache way: " << dec << (uint16_t) cacheWay << " from OSC: " << hex << osc);
#endif

#ifdef	CONFIG_CACHE_CONTROL_EVICT_AFTER_USE
			evictCacheWay(cacheWay,duration);
			mCacheWays[cacheWay].oscID = nullptr;
			mEvictionCount++;
#endif
			mCacheWays[cacheWay].inUse = false;
			mCacheWays[cacheWay].lruCount = 0;
//			DEBUG_STREAM(TAG,
//							"EVCache way " << dec << (dword_t) cacheWay << " permanently locked: " << mCacheWays[cacheWay].permanentLocked
//									<< " with id: " << hex << mCacheWays[cacheWay].oscID << " used: " << dec <<(uint32_t)mCacheWays[cacheWay].inUse );
		}
	}
}


void GenericCacheManagement::printCacheWayInformation() {
//	LOG_OUTPUT_STREAM(TAG,"Cache evictions: " << dec << mEvictionCount << endl);
//	LOG_OUTPUT_STREAM_START(TAG, "Cache ways:" << dec << (uint32_t)mCacheWayCount << endl);
//	LOG_OUTPUT_STREAM_CONTINUE(TAG,"avg. missrate: " << dec << mLastMissRate << endl);
//	for (cacheways_t i = 0; i < mCacheWayCount; i++) {
//		LOG_OUTPUT_STREAM_CONTINUE(TAG,
//				"Cache way " << dec << (dword_t) i << " permanently locked: " << mCacheWays[i].permanentLocked
//						<< " with id: " << hex << mCacheWays[i].oscID << endl);
//	}
//	LOG_OUTPUT_STREAM_END;

	DEBUG_STREAM(TAG,"Cache evictions: " << dec << mEvictionCount << endl);
	DEBUG_STREAM(TAG, "Cache ways:" << dec << (uint32_t)mCacheWayCount << endl);
	DEBUG_STREAM(TAG,"avg. missrate: " << dec << mLastMissRate << endl);
	for (cacheways_t i = 0; i < mCacheWayCount; i++) {
		DEBUG_STREAM(TAG,
				"Cache way " << dec << (dword_t) i << " permanently locked: " << mCacheWays[i].permanentLocked
						<< " with id: " << hex << mCacheWays[i].oscID << " used: " << dec <<(uint32_t)mCacheWays[i].inUse );
	};
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

void GenericCacheManagement::preloadSingleOSC(OSC *osc, cycle_t *duration) {
#ifdef CONFIG_PROFILING_PRELOAD
	cycle_t duration_sum = 0;
#endif
	cycle_t duration_temp = 0;

	/* Determine OSC properties */
	uintptr_t oscStart = (uintptr_t)osc->getOSCStart();
	uintptr_t oscEnd = (uintptr_t)osc->getOSCEnd();
	uintptr_t oscTextEnd = (uintptr_t)osc->getOSCTextEnd();

	uintptr_t preloadEnd;

	while (oscStart < oscEnd) {
		/* First get the cache way which is the least recently used one */
		cacheways_t lruCacheWay = getLRUWay(osc,oscStart);
		if(lruCacheWay == cMaxCacheWays) {
			DEBUG_STREAM(TAG,"LRU did not find a free cache way, aborting execution!");
			printCacheWayInformation();
			while(1);
		}

		// Check if way already contains OSC
		if (mCacheWays[lruCacheWay].oscID == osc) {
			return;
		}

		preloadEnd = oscStart + (getCacheWaySize() - 4);
		if (preloadEnd > oscEnd) {
			preloadEnd = oscEnd;
		}

//		DEBUG_STREAM(TAG,"Preload OSC:" << hex << osc << " to way: " << dec << lruCacheWay);
		// Load the data to the specific cache way
		prefetchDataToWay(oscStart, preloadEnd, oscTextEnd <= preloadEnd ? oscTextEnd : preloadEnd, lruCacheWay, &duration_temp);

		mCacheWays[lruCacheWay].oscID = osc;
		mCacheWays[lruCacheWay].dataStart = oscStart;
		mCacheWays[lruCacheWay].inUse = true;

		oscStart = preloadEnd + 4;
		if (oscStart > oscEnd) {
			oscStart = oscEnd;
		}

#ifdef CONFIG_PROFILING_PRELOAD
		duration_sum += duration_temp;
#endif
	}


#ifdef CONFIG_PROFILING_PRELOAD
	*duration = duration_sum;
#endif
}

} /* namespace CacheManagement */

