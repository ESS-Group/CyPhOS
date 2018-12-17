/*
 * ApplicationSystemInfo.cc
 *
 *  Created on: 16.11.2016
 *      Author: hendrik
 */

#include "ApplicationSystemInfo.h"


#include <common/cyclecounter.h>

#include <eventhandling/EventHandler.h>

#include <common/SyncOutput.h>

#include <driver/GenericTimer.h>

#include <common/greeting.h>

#include <common/RandomGenerator.h>

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

#include <cachemanagement/GenericCacheManagement.h>

#define TAG "SystemInfo"

extern uintptr_t __critical_osc_start;
extern uintptr_t __critical_osc_end;

extern uintptr_t __benchmark_results__start;
extern uintptr_t __benchmark_results__end;
extern volatile int32_t holding_pen;


BEGIN_OSC_IMPLEMENTATION(ApplicationSystemInfo,ApplicationSystemInfo)

ApplicationSystemInfo ApplicationSystemInfo::mInstance;

SECTION_CRITICAL_DATA OSC* ApplicationSystemInfo::trigger_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,printEventDispatcherStats,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 600);

SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,floatTest,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 603);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,printUptime,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 603);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,printVersion,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 604);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,printCacheManagementStats,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 601);

SECTION_CRITICAL_DATA DEFINE_TRIGGER_AFFINITY(ApplicationSystemInfo,crashTest,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 606, 0x1);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,verifyCritical,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 605);
#ifdef CONFIG_ARMV7
SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,printPMUStats,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 602);


SECTION_CRITICAL_DATA DEFINE_TRIGGER(ApplicationSystemInfo,multiCore,ApplicationSystemInfo::mInstance,trigger_deps, EventHandling::Trigger::cMIN_Priority + 606);
#endif

DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,start) {

}

ApplicationSystemInfo::ApplicationSystemInfo(): mTimerEvent(nullptr){
	trigger_crashTest.pAffinity = 0x0;
}

DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,printEventDispatcherStats) {
	EventHandling::EventHandler::pInstance.dumpDispatchStatistic();
}



DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,printUptime) {
	SYNC_OUTPUT_STREAM(TAG, "System uptime: " << dec << OSC_PREFIX(Timer)::GenericTimer::pInstance->getTimeInMS() << " ms" << endl);
	OSC_PREFIX(Timer)::GenericTimer::pInstance->dumpTimerStatus();
}


DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,floatTest) {

//	DEBUG_STREAM(TAG,"Floating pointt test: " << test);
	SYNC_OUTPUT_STREAM(TAG, "System uptime: " << 8 << endl);
//	SYNC_OUTPUT_STREAM(TAG, "Floating point test: " <<  endl);
}

DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,printVersion) {
	SYNC_OUTPUT_STREAM(TAG, "System version: " << cyphos_greeting);
}
DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,printCacheManagementStats) {
//	CacheManagement::GenericCacheManagement::sInstance->printCacheInformation();
	CacheManagement::GenericCacheManagement::sInstance->printCacheWayInformation();
}



DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,crashTest) {
	DEBUG_STREAM(TAG, "Crashing the system");
//	int a = 0;
//	int b = 42;
//	int *c = &a;
//	int d = (b / *c);
//	DEBUG_STREAM(TAG,"Crash: " << dec << d);

//	int *a = (int*)0xFFFFFFFF00004711;
//	DEBUG_STREAM(TAG,"Int a: " << dec << *a);
#ifdef CONFIG_ARMV7
	__asm__ __volatile__ (// Trigger undefined instruction".word 0xf7f0a000;"
			"MOV R0, #0;"
			"MOV R1, #1;"
			"MOV R2, #2;"
			"MOV R3, #3;"
			"MOV R4, #4;"
			"MOV R5, #5;"
			"MOV R6, #6;"
			"MOV R7, #7;"
			"MOV R8, #8;"
			"MOV R9, #9;"
			"MOV R10, #10;"
			"MOV R11, #11;"
			"MOV R12, #12;"
			// Trigger data abort here (write to 0x0)
			"STR R0,[R0];":::"memory");
#endif

#ifdef CONFIG_AMD64
	__asm__ __volatile__ (""
			"MOVQ $0xFFFF4711,%%RAX;"
			"MOVQ $1,%%RBX;"
			"MOVQ $2,%%RCX;"
			"MOVQ $3,%%RDX;"
			"MOVQ $8,%%R8;"
			"MOVQ $9,%%R9;"
			"MOVQ $10,%%R10;"
			"MOVQ $11,%%R11;"
			"MOVQ $12,%%R12;"
			"MOVQ $13,%%R13;"
			"MOVQ $14,%%R14;"
			"MOVQ $15,%%R15;"
			"MOVQ $0,(%%RAX);"
			"JMP %%RAX;":::"memory");

#endif
}

DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,multiCore) {

	SYNC_OUTPUT_STREAM(TAG,"Released holding pen for multicore applications" << endl);
}

#ifdef CONFIG_ARMV7


DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,printPMUStats) {
	ARMV7PerformanceCounter::pInstance.printInformation();
	ARMV7PerformanceCounter::pInstance.printCounterValues();
}
#endif

DEFINE_TRIGGER_FUNC(ApplicationSystemInfo,verifyCritical) {
	uint32_t min = 0xFFFFFFFF,max = 0;

	uint32_t before = 0,after = 0;

//	uintptr_t address = 0;

//	uint32_t critical_size =  &__critical_osc_end - &__critical_osc_start;

	uint32_t cycles_sum = 0;

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



	uint32_t count = 20000;//critical_size / 4;
#ifdef CONFIG_ARMV7
	uint32_t cacheHitRate_before = 0, cacheReq_before = 0;
	uint32_t cacheHitRate_after = 0, cacheReq_after = 0;

	uint32_t memoryAccess = 0;

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


	count = ((uintptr_t)&__critical_osc_end - (uintptr_t)&__critical_osc_start) / CONFIG_CACHE_LINE_SIZE;
	for (uintptr_t i = (uintptr_t)&__critical_osc_start; i < (uintptr_t)&__critical_osc_end; i+=CONFIG_CACHE_LINE_SIZE) {
		cycle_t cycles = after - before;
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
		}

	}

#ifdef CONFIG_ARMV7
	cacheHitRate_after = CacheManagement::ARMV7CacheControl::readPerformanceCounter(0);
	cacheReq_after = CacheManagement::ARMV7CacheControl::readPerformanceCounter(1);

	memoryAccess = ARMV7PerformanceCounter::pInstance.readEventCounter(0);
#endif

	SYNC_OUTPUT_STREAM(TAG,"Critical section access count: " << dec << count << endl);
	SYNC_OUTPUT_STREAM(TAG,"Critical section average access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Critical section L3 misses (estimated): " << dec << ramCount << " rate: " << (ramCount*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Critical section L2 misses (estimated): " << dec << l3Count << " rate: " << (l3Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Critical section L1 misses (estimated): " << dec << l2Count << " rate: " << (l2Count*100 / count) << " %" << endl);
#ifdef CONFIG_ARMV7
	SYNC_OUTPUT_STREAM(TAG,"Cache hit before: " << dec << cacheHitRate_before << " after " << cacheHitRate_after << endl);
	SYNC_OUTPUT_STREAM(TAG,"Cache Requests before: " << dec << cacheReq_before << " after " << cacheReq_after << endl);
	SYNC_OUTPUT_STREAM(TAG,"Cache requests: " << dec << cacheReq_after- cacheReq_before << " hits: " << cacheHitRate_after- cacheHitRate_before<< endl);
	SYNC_OUTPUT_STREAM(TAG,"Memory access counter: " << dec << memoryAccess << endl);
#endif

#ifdef CONFIG_ARMV7
	CacheManagement::ARMV7CacheControl::pInstance.cleanAndInvalidate_Data_Caches();
	DEBUG_STREAM(TAG,"L1 Cache invalidated");
#endif
	ramCount = 0;
	l2Count = 0;
	l3Count = 0;
	count = ((uintptr_t)&__critical_osc_end - (uintptr_t)&__critical_osc_start) / 4;
	for (uintptr_t i = (uintptr_t)&__critical_osc_start; i < (uintptr_t)&__critical_osc_end; i+=4) {
		cycle_t cycles = after - before;
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
		}

	}
#ifdef CONFIG_ARMV7
	ARMV7PerformanceCounter::pInstance.printCounterValues();
#endif
	// Check if benchmark region is not cached after multiple access
	cycles_sum = 0;
	min = 0xFFFFFFFF;
	max = 0x0;
	ramCount = 0;
	l2Count = 0;
	l3Count = 0;
	count = ((uintptr_t)&__benchmark_results__end - (uintptr_t)&__benchmark_results__start) / 4;
	for (uintptr_t i = (uintptr_t)&__benchmark_results__start; i < (uintptr_t)&__benchmark_results__end; i+=4) {
			cycle_t cycles = after - before;
			MEASURE_MEMORY_ACCESS(i,cycles);
			cycles_sum += cycles;
			if ( cycles < min) {
				min = cycles;
			}
			if (cycles > max) {
				max = cycles;
			}

			if (cycles > ram_threshold) {
//				DEBUG_STREAM(TAG,"RAM hit at address: " << hex << i << " with cycles: " << dec << cycles);
				ramCount++;
			} else if (cycles > l3_threshold) {
//				DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
				l3Count++;
			} else if (cycles > l2_threshold) {
	//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
				l2Count++;
			} else {
	//			DEBUG_STREAM(TAG,"L1 HIT at address: " << hex << i << " with cycles: " << dec << cycles);
			}

	}
	SYNC_OUTPUT_STREAM(TAG,"Benchmark section access count: " << dec << count << endl);
	SYNC_OUTPUT_STREAM(TAG,"Benchmark section average access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Benchmark L3 misses (estimated): " << dec << ramCount << " rate: " << (ramCount*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Benchmark L2 misses (estimated): " << dec << l3Count << " rate: " << (l3Count*100 / count) << " %" << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Benchmark L1 misses (estimated): " << dec << l2Count << " rate: " << (l2Count*100 / count) << " %" << endl);
}


END_OSC_IMPLEMENTATION
