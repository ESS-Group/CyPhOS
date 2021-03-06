/*
 * StressTestComponent__THISCOMPONENT__.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestComponent_generic___THISCOMPONENT__.h>

#include <common/SyncOutput.h>

#include "StressTestComponent_generic___NEXTCOMPONENT__.h"

#define TAG "StressTestComponent__THISCOMPONENT__"

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

#include <common/RandomGenerator.h>

BEGIN_OSC_IMPLEMENTATION(StressTestComponent__THISCOMPONENT__,StressTestComponent__THISCOMPONENT__)

StressTestComponent__THISCOMPONENT__ StressTestComponent__THISCOMPONENT__::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestComponent__THISCOMPONENT__::trigger_deps_doStress[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_doStress = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_doStress,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + __THISCOMPONENT__ + 1};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_subscribe = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_subscribe,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + __THISCOMPONENT__ + 2};

SECTION_CRITICAL_DATA OSC* StressTestComponent__THISCOMPONENT__::trigger_deps_printOut[] = {CONSOLE_BUFFER_DEP,nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_printOut = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_printOut,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_printOut, EventHandling::Trigger::cMIN_Priority + 21000 + __THISCOMPONENT__ + 3};

SECTION_CRITICAL_DATA EventHandling::Event StressTestComponent__THISCOMPONENT__::event_Continue;

SECTION_CRITICAL_DATA DEFINE_TRIGGER(StressTestComponent__THISCOMPONENT__,selfCall,StressTestComponent__THISCOMPONENT__::mInstance,trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 2003);


StressTestComponent__THISCOMPONENT__::StressTestComponent__THISCOMPONENT__() : mIterations(0), mMin(0xFFFFFFFF), mMax(0) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent__NEXTCOMPONENT__)::StressTestComponent__NEXTCOMPONENT__::event_Continue, &trigger_doStress);
}

void StressTestComponent__THISCOMPONENT__::func_doStress(dword_t arg) {
	uint32_t cycles_sum = 0;

	const uint32_t ram_threshold = 100;
	const uint32_t l2_threshold = 25;

#define RANDOM_ACCESS
//#define SEQUENTIAL_ACCESS
#ifdef RANDOM_ACCESS
	mIterations++;
	cpu_t cpu = getCPUID();
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < cAccessCount; i++) {
		cycle_t cycles = 1;
		address = &mTestData[RandomGenerator::mInstance[cpu].random32() % cArraySize];
		MEASURE_MEMORY_ACCESS(address,cycles);

		cycles_sum += cycles;

		if ( cycles < mMin) {
			mMin = cycles;
		}
		if (cycles > mMax) {
			mMax = cycles;
		}

		if (cycles > ram_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mRamCount++;
		}
		if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL2Count++;
		}
		mCountSum++;
	}
	mLastAverage = cycles_sum / cAccessCount;
//	SYNC_OUTPUT_STREAM(TAG,"Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
	//READ_CYCLE_COUNTER(after);
	//SYNC_OUTPUT_STREAM(TAG,"Execution duration on CPU: " << (uint16_t)getCPUID() << " :" << dec << (after-before) << endl);
#endif
#ifdef SEQUENTIAL_ACCESS
	mIterations++;
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < cAccessCount; i++) {
		cycle_t cycles;
		address = (dword_t*)((uintptr_t)address + 4);
		MEASURE_MEMORY_ACCESS(address,cycles);
//		DEBUG_STREAM(TAG,"" << dec << cycles);


		cycles_sum += cycles;

		if ( cycles < mMin) {
			mMin = cycles;
		}
		if (cycles > mMax) {
			mMax = cycles;
		}

		if (cycles > ram_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mRamCount++;
		}
		if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL2Count++;
		}
		mCountSum++;
	}

	mLastAverage = cycles_sum / cAccessCount;
//	SYNC_OUTPUT_STREAM(TAG,"Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
	//READ_CYCLE_COUNTER(after);
	//SYNC_OUTPUT_STREAM(TAG,"Execution duration on CPU: " << (uint16_t)getCPUID() << " :" << dec <<  (after-before) << endl);
#endif
}

void StressTestComponent__THISCOMPONENT__::func_printOut(dword_t arg) {
	SYNC_OUTPUT_STREAM_START(TAG << ": L2 misses (estimated): " << dec << mRamCount << " rate: " << (mRamCount*100 / mCountSum) << " % ");
	SYNC_OUTPUT_STREAM_CONTINUE("L1 misses (estimated): " << dec << mL2Count << " rate: " << (mL2Count*100 / mCountSum) << " % ");
	SYNC_OUTPUT_STREAM_CONTINUE("Min : " << dec << mMin << " Max: " << mMax << " average: " << mLastAverage << endl);
	SYNC_OUTPUT_STREAM_END;
	mCountSum = 0;
	mRamCount = 0;
	mLastAverage = 0;
	mL2Count = 0;
	mMax = 0;
	mMin = 0xFFFFFFFF;
}

void StressTestComponent__THISCOMPONENT__::func_subscribe(dword_t arg) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent__NEXTCOMPONENT__)::StressTestComponent__NEXTCOMPONENT__::event_Continue, &trigger_doStress);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
}

DEFINE_TRIGGER_FUNC(StressTestComponent__THISCOMPONENT__,selfCall) {
	mIterations++;
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_selfCall, NULL);
}

END_OSC_IMPLEMENTATION
