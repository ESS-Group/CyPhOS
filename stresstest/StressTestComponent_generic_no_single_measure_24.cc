/*
 * StressTestComponent24.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestComponent_generic_no_single_measure_24.h>

#include <common/SyncOutput.h>

#include "StressTestComponent_generic_no_single_measure_25.h"

#define TAG "StressTestComponent24"

#include <common/cyclecounter.h>
#include <benchmarking/ResultContainer.h>

#include <cachemanagement/GenericCacheManagement.h>

#ifdef CONFIG_ARMV7
#include <arch/armv7/driver/ARMV7CacheControl.h>

#include <arch/armv7/driver/ARMV7PerformanceCounter.h>

				//"LDR r0,[%[addr]];"
#define MEASURE_MEMORY_ACCESS(address) __asm__ __volatile__ ( \
				"LDR r0,[%[addr]];" \
				::[addr]"r"(address):"memory","r0");

#endif

#ifdef CONFIG_AMD64
#include <driver/CATManager.h>

#define MEASURE_MEMORY_ACCESS(address) __asm__ __volatile__ ( \
				"MOVQ (%[addr]),%%rax\n" \
				::[addr]"r"(address):"memory","%rax");
#endif

BEGIN_OSC_IMPLEMENTATION(StressTestComponent24,StressTestComponent24)

StressTestComponent24 StressTestComponent24::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestComponent24::trigger_deps_doStress[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent24::trigger_doStress = {(OSC*)&StressTestComponent24::mInstance,(void (OSC::*)(dword_t))&StressTestComponent24::func_doStress,
				(OSC**)&StressTestComponent24::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 0 + 1};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent24::trigger_subscribe = {(OSC*)&StressTestComponent24::mInstance,(void (OSC::*)(dword_t))&StressTestComponent24::func_subscribe,
				(OSC**)&StressTestComponent24::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 0 + 2};

SECTION_CRITICAL_DATA OSC* StressTestComponent24::trigger_deps_printOut[] = {CONSOLE_BUFFER_DEP,nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent24::trigger_printOut = {(OSC*)&StressTestComponent24::mInstance,(void (OSC::*)(dword_t))&StressTestComponent24::func_printOut,
				(OSC**)&StressTestComponent24::trigger_deps_printOut, EventHandling::Trigger::cMIN_Priority + 21000 + 0 + 3};

SECTION_CRITICAL_DATA EventHandling::Event StressTestComponent24::event_Continue;

SECTION_CRITICAL_DATA DEFINE_TRIGGER(StressTestComponent24,selfCall,StressTestComponent24::mInstance,trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 2003);


StressTestComponent24::StressTestComponent24() : mIterations(0), mMin(0xFFFFFFFF), mMax(0) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::event_Continue, &trigger_doStress);
}

void StressTestComponent24::func_doStress(dword_t arg) {
	mCurrentRandomSeed = cRandomSeed;
//	cycle_t enqueue_before, enqueue_after;
//	uint32_t count = 0;
//	READ_CYCLE_COUNTER(enqueue_before);
#define RANDOM_ACCESS
//#define SEQUENTIAL_ACCESS
#ifdef RANDOM_ACCESS
	mIterations++;
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < cAccessCount; i++) {
		address = &mTestData[random32() % cArraySize];
		MEASURE_MEMORY_ACCESS(address);
	}
#endif
#ifdef SEQUENTIAL_ACCESS
	mIterations++;
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < cAccessCount; i++) {
		address = (dword_t*)((uintptr_t)address + (CONFIG_CACHE_LINE_SIZE));
		// Contain address within bounds
		if ((uintptr_t)address > ((uintptr_t)mTestData + (uintptr_t)cArraySize)) {
			address = mTestData;
		}
		MEASURE_MEMORY_ACCESS(address);
//		count++;
	}
#endif
//	READ_CYCLE_COUNTER(enqueue_after);
//	cycle_t diff = (enqueue_after-enqueue_before);
//	if ((diff) > 50000) {
//		DEBUG_STREAM(TAG,"Outlier: " << dec << diff);
//		DEBUG_STREAM(TAG,"Count: " << dec << count);
////		for (uint16_t i = 0; i < GenericCacheManagement::sInstance->mCacheWayCount) {
////
////		}
//	}
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
}

void StressTestComponent24::func_printOut(dword_t arg) {
	SYNC_OUTPUT_STREAM_START(TAG << ": iterations: " << dec << mIterations << endl);
	SYNC_OUTPUT_STREAM_END;
}

void StressTestComponent24::func_subscribe(dword_t arg) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::event_Continue, &trigger_doStress);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
}

DEFINE_TRIGGER_FUNC(StressTestComponent24,selfCall) {
	mIterations++;
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_selfCall, NULL);
}

uint32_t StressTestComponent24::random32() {
	// XOR-Shift
	/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
	uint32_t x = mCurrentRandomSeed;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	mCurrentRandomSeed = x;

	return (uint32_t) x;
}

END_OSC_IMPLEMENTATION
