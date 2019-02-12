/*
 * ApplicationCacheMissTest.cc
 *
 *  Created on: 16.11.2016
 *      Author: hendrik
 */

#include "ApplicationCacheMissTest.h"

#ifdef CONFIG_ARMV7
#include <arch/armv7/driver/ARMV7CacheControl.h>

#include <arch/armv7/driver/ARMV7PerformanceCounter.h>

#define MEASURE_MEMORY_ACCESS(address, cycles) __asm__ __volatile__ ( \
				"ISB;" \
				"DSB;" \
				"MRC p15, 0, r1, c9, c13, 0;" \
				"LDR r0,[%[addr]];" \
				"ISB;" \
				"DSB;" \
				"MRC p15, 0, %[cycles], c9, c13, 0;" \
				"SUB %[cycles],%[cycles], R1" \
				:[cycles]"=r"(cycles):[addr]"r"(address):"memory","r0","r1");

#endif

#ifdef CONFIG_AMD64
#include <driver/CATManager.h>

#define MEASURE_MEMORY_ACCESS(address, cycles) __asm__ __volatile__ ( \
				"CPUID\n" \
				"RDTSC\n" \
				"SHL $32,%%rdx\n" \
				"OR %%rdx,%%rax\n" \
				"MOVQ %%rax,%%r8\n" \
				"MOVQ (%[addr]),%%rax\n" \
				"RDTSCP\n" \
				"SHL $32,%%rdx\n" \
				"OR %%rdx,%%rax\n" \
				"SUB %%r8,%%rax\n" \
				"MOVQ %%rax,%[cycles]\n" \
				"CPUID\n" \
				:[cycles]"=r"(cycles):[addr]"r"(address):"memory","%rax","%rbx","%rcx","%rdx","r8");
#endif

#include <eventhandling/EventHandler.h>

#include <common/SyncOutput.h>

#include <driver/GenericTimer.h>

#include <common/greeting.h>

#include <common/RandomGenerator.h>

#define TAG "SystemInfo"

extern uintptr_t __critical_osc_start;
extern uintptr_t __critical_osc_end;


BEGIN_OSC_IMPLEMENTATION(ApplicationCacheMissTest,ApplicationCacheMissTest)

ApplicationCacheMissTest ApplicationCacheMissTest::mInstance;

SECTION_CRITICAL_DATA OSC* ApplicationCacheMissTest::trigger_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationCacheMissTest,verifyCache,ApplicationCacheMissTest::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 605);


DEFINE_TRIGGER_FUNC(ApplicationCacheMissTest,verifyCache) {
	uint32_t min = 0xFFFFFFFF,max = 0;

	cycle_t cycles_sum = 0;

#ifdef CONFIG_ARMV7
	const uint32_t ram_threshold = 100;
	const uint32_t l2_threshold = 35;
	const uint32_t l3_threshold = 0xFFFF;
#endif
#ifdef CONFIG_AMD64
	// Values for the Xeon E5-1620 v4
	const uint32_t ram_threshold = 100;
	const uint32_t l3_threshold = 36;
	const uint32_t l2_threshold = 32;
#endif
	uint32_t ramCount = 0;
	uint32_t l2Count = 0;
	uint32_t l3Count = 0;
	uint32_t l1Count = 0;

	uint32_t cacheHitRate_before = 0, cacheReq_before = 0;
	uint32_t cacheHitRate_after = 0, cacheReq_after = 0;

	uint32_t memoryAccess = 0;

	uint32_t count = 0;//critical_size / 4;
#ifdef CONFIG_ARMV7
	CacheManagement::ARMV7CacheControl::pInstance.cleanAndInvalidate_Data_Caches();
	DEBUG_STREAM(TAG,"L1 Cache invalidated");
	cacheHitRate_before = CacheManagement::ARMV7CacheControl::readPerformanceCounter(0);
	cacheReq_before = CacheManagement::ARMV7CacheControl::readPerformanceCounter(1);

	ARMV7PerformanceCounter::pInstance.writeEventCounterType(ARMV7PerformanceCounter::ARM_event_t::EVENT_MEMORY_ACCESS, 0x0);
	ARMV7PerformanceCounter::pInstance.writeEventCounterType(ARMV7PerformanceCounter::ARM_event_t::EVENT_DATA_LINEFILLS, 0x1);
	ARMV7PerformanceCounter::pInstance.writeEventCounter(0,0);
	ARMV7PerformanceCounter::pInstance.writeEventCounter(0,1);
	ARMV7PerformanceCounter::pInstance.enableEventCounter(0);
	ARMV7PerformanceCounter::pInstance.enableEventCounter(1);
	ARMV7PerformanceCounter::pInstance.printCounterValues();
#endif
	count = 0;
	for (uintptr_t i = (uintptr_t)mTestData; i < (uintptr_t)mTestData + cTestSize; i+=CONFIG_CACHE_LINE_SIZE) {
		cycle_t cycles = 0;

		MEASURE_MEMORY_ACCESS(i,cycles);
		cycles_sum += cycles;
		if ( cycles < min) {
			min = cycles;
		}
		if (cycles > max) {
			max = cycles;
		}

		if (cycles > ram_threshold) {
//			DEBUG_STREAM(TAG,"RAM hit at address: " << hex << i << " with cycles: " << dec << cycles);
			ramCount++;
		} else if (cycles > l3_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			l3Count++;
		} else if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			l2Count++;
		} else {
//			DEBUG_STREAM(TAG,"L1 HIT at address: " << hex << i << " with cycles: " << dec << cycles);
			l1Count++;
		}


		count++;
	}
#ifdef CONFIG_ARMV7
	cacheHitRate_after = CacheManagement::ARMV7CacheControl::readPerformanceCounter(0);
	cacheReq_after = CacheManagement::ARMV7CacheControl::readPerformanceCounter(1);

	memoryAccess = ARMV7PerformanceCounter::pInstance.readEventCounter(0);
#endif
	SYNC_OUTPUT_STREAM_START(TAG << "Access count: " << dec << count << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Run on CPU: " << dec << (uint32_t)getCPUID() << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("L3 misses (estimated): " << dec << ramCount << " rate: " << (ramCount*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("L2 misses (estimated): " << dec << l3Count << " rate: " << (l3Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("L1 misses (estimated): " << dec << l2Count << " rate: " << (l2Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("L1 hits (estimated): " << dec << l1Count << " rate: " << (l1Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Cache hit before: " << dec << cacheHitRate_before << " after " << cacheHitRate_after << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Cache Requests before: " << dec << cacheReq_before << " after " << cacheReq_after << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Cache requests: " << dec << cacheReq_after- cacheReq_before << " hits: " << cacheHitRate_after- cacheHitRate_before<< endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Memory access counter: " << dec << memoryAccess << endl);
	SYNC_OUTPUT_STREAM_END;
#ifdef CONFIG_ARMV7
	CacheManagement::ARMV7CacheControl::pInstance.clean_Data_Caches();
#endif
//	DEBUG_STREAM(TAG,"Flushing caches from: " << hex << mTestData << " to: " << ((uintptr_t)mTestData + cTestSize) );
#ifdef CONFIG_AMD64_CAT
	CacheManagement::CATManager::sInstance.evictMemoryRange((uintptr_t)mTestData, cTestSize);
#endif
//	DEBUG_STREAM(TAG,"L1 Cache cleaned from: " << hex << mTestData << " to: " << ((uintptr_t)mTestData + cTestSize));
	count = 0;
	l1Count = 0;
	l2Count = 0;
	l3Count = 0;
	ramCount = 0;
	cycles_sum = 0;

	min = 0xFFFFFFFF;
	max = 0;

	DEBUG_STREAM(TAG,"mTestData: " << hex << mTestData << " cTestSize: " << (uintptr_t)mTestData + cTestSize);

	for (volatile uintptr_t i = (uintptr_t)mTestData; i < (uintptr_t)mTestData + cTestSize; i+=CONFIG_CACHE_LINE_SIZE) {
		cycle_t cycles = 0;

		MEASURE_MEMORY_ACCESS(i,cycles);

		cycles_sum += cycles;
		if ( cycles < min) {
			min = cycles;
		}
		if (cycles > max) {
			max = cycles;
		}

		if (cycles > ram_threshold) {
//			DEBUG_STREAM(TAG,"RAM hit at address: " << hex << i << " with cycles: " << dec << cycles);
			ramCount++;
		} else if (cycles > l3_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			l3Count++;
		} else if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			l2Count++;
		} else {
//			DEBUG_STREAM(TAG,"L1 HIT at address: " << hex << i << " with cycles: " << dec << cycles);
			l1Count++;
		}


//		DEBUG_STREAM(TAG,"Access time: " << dec << cycles);

		count++;
	}
#ifdef CONFIG_ARMV7
	ARMV7PerformanceCounter::pInstance.printCounterValues();
#endif
	SYNC_OUTPUT_STREAM_START(TAG << "2nd_Access count: " << dec << count << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_L3 misses (estimated): " << dec << ramCount << " rate: " << (ramCount*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_L2 misses (estimated): " << dec << l3Count << " rate: " << (l3Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_L1 misses (estimated): " << dec << l2Count << " rate: " << (l2Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_L1 hits (estimated): " << dec << l1Count << " rate: " << (l1Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_Cache hit before: " << dec << cacheHitRate_before << " after " << cacheHitRate_after << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_Cache Requests before: " << dec << cacheReq_before << " after " << cacheReq_after << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_Cache requests: " << dec << cacheReq_after- cacheReq_before << " hits: " << cacheHitRate_after- cacheHitRate_before<< endl);
	SYNC_OUTPUT_STREAM_CONTINUE("2nd_Memory access counter: " << dec << memoryAccess << endl);
	SYNC_OUTPUT_STREAM_END;
}

END_OSC_IMPLEMENTATION
