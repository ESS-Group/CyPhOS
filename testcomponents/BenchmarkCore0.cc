/*
 * BenchmarkCore0.cc
 *
 *  Created on: 25.09.2017
 *      Author: hendrik
 */

#include "BenchmarkCore0.h"

#include <common/SyncOutput.h>

#include <dynamicmemory/MemoryAllocator.h>

#include <common/RandomGenerator.h>

#include <arch/armv7/driver/ARMV7PerformanceCounter.h>

BEGIN_OSC_IMPLEMENTATION(BenchmarkCore0,BenchmarkCore0)

#define TAG "BenchmarkCore0"

BenchmarkCore0 BenchmarkCore0::mInstance;

/**
 * Instance of trigger dependencies
 */
SECTION_CRITICAL_DATA OSC* BenchmarkCore0::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA OSC* BenchmarkCore0::trigger_deps_output[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(BenchmarkCore0,start,BenchmarkCore0::mInstance,trigger_deps_output, EventHandling::Trigger::cMIN_Priority + 900);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(BenchmarkCore0,benchmarkRun,BenchmarkCore0::mInstance,trigger_deps_output, EventHandling::Trigger::cMIN_Priority + 901);

SECTION_CRITICAL_DATA EventHandling::Event BenchmarkCore0::mBenchmarkEvent;

DEFINE_TRIGGER_FUNC(BenchmarkCore0, start) {
	SYNC_OUTPUT_STREAM(TAG,"Starting benchmark" << endl);

	mAccessTimes = (uint32_t*)DynamicMemory::MemoryAllocator::mInstance.allocate(this,sizeof(uint32_t) * cBenchmarkCalls * cBenchmarkMemAccesses,DynamicMemory::MemoryAllocator::NOT_CACHED);
	if ( mAccessTimes == nullptr) {
		SYNC_OUTPUT_STREAM(TAG,"Error allocating the benchmark result space" << endl);
	} else {
		SYNC_OUTPUT_STREAM(TAG, "Benchmark result space allocated at: " << hex << mAccessTimes << endl);
	}

	for(uint8_t i = 0; i < cMemoryRequests; i++) {
		mMemory[i] = (dword_t*)DynamicMemory::MemoryAllocator::mInstance.allocate(this,cMemoryRangeSize,DynamicMemory::MemoryAllocator::DYNAMIC_CACHED);
//		SYNC_OUTPUT_STREAM(TAG,"Got memory at" << hex << mMemory[i] << endl);
	}

	EventHandling::EventHandler::pInstance.subscribeEvent(&mBenchmarkEvent, &trigger_benchmarkRun);
	EventHandling::EventHandler::pInstance.eventTriggered(&mBenchmarkEvent, 0x0);
}

DEFINE_TRIGGER_FUNC(BenchmarkCore0, benchmarkRun) {
	uint32_t before,after;

	uint32_t memRange;
	for(uint32_t access = 0; access < cBenchmarkMemAccesses; access++) {
		memRange = RandomGenerator::random32() % cMemoryRequests;
		void *address = ((char*)mMemory[memRange]) + RandomGenerator::random32() % (cMemoryRangeSize - 4);
		READ_CYCLE_COUNTER(before);
		__asm__ __volatile__("LDR R0,[%[address]]"::[address]"r"(address):"r0","memory");
		READ_CYCLE_COUNTER(after);
		DEBUG_STREAM(TAG,"Access time: " << dec << (after-before));
	}
}

END_OSC_IMPLEMENTATION
