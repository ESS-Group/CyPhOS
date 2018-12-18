/*
 * StressTestComponent3.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestComponent_generic_3.h>

#include <common/SyncOutput.h>

#include "StressTestComponent_generic_4.h"

#define TAG "StressTestComponent3"

#include <common/cyclecounter.h>

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
				"RDTSCP\n" \
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

BEGIN_OSC_IMPLEMENTATION(StressTestComponent3,StressTestComponent3)

StressTestComponent3 StressTestComponent3::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestComponent3::trigger_deps_doStress[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent3::trigger_doStress = {(OSC*)&StressTestComponent3::mInstance,(void (OSC::*)(dword_t))&StressTestComponent3::func_doStress,
				(OSC**)&StressTestComponent3::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 3 + 1,0x0};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent3::trigger_subscribe = {(OSC*)&StressTestComponent3::mInstance,(void (OSC::*)(dword_t))&StressTestComponent3::func_subscribe,
				(OSC**)&StressTestComponent3::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 3 + 2};

SECTION_CRITICAL_DATA OSC* StressTestComponent3::trigger_deps_printOut[] = {CONSOLE_BUFFER_DEP,nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent3::trigger_printOut = {(OSC*)&StressTestComponent3::mInstance,(void (OSC::*)(dword_t))&StressTestComponent3::func_printOut,
				(OSC**)&StressTestComponent3::trigger_deps_printOut, EventHandling::Trigger::cMIN_Priority + 21000 + 3 + 3};

SECTION_CRITICAL_DATA EventHandling::Event StressTestComponent3::event_Continue;

SECTION_CRITICAL_DATA DEFINE_TRIGGER(StressTestComponent3,selfCall,StressTestComponent3::mInstance,trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 2003);


StressTestComponent3::StressTestComponent3() : mIterations(0), mMin(0xFFFFFFFF), mMax(0) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::event_Continue, &trigger_doStress);
}

void StressTestComponent3::func_doStress(dword_t arg) {
	uint32_t cycles_sum = 0;

	cycle_t measure_before = 0, measure_after = 0;

	RESET_READ_CYCLE_COUNTER(measure_before);
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
//			DEBUG_STREAM(TAG,"RAM hit at address: " << hex << i << " with cycles: " << dec << cycles);
			mRamCount++;
		} else if (cycles > l3_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL3Count++;
		} else if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL2Count++;
		} else {
//			DEBUG_STREAM(TAG,"L1 HIT at address: " << hex << i << " with cycles: " << dec << cycles);
		}
		mCountSum++;
	}
	mLastAverage = cycles_sum / cAccessCount;
//	SYNC_OUTPUT_STREAM(TAG,"Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);

	// Every 100 iterations print out status
	if ((mIterations  % 100) == 0) {
		EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_printOut, 0);
	}
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
//			DEBUG_STREAM(TAG,"RAM hit at address: " << hex << i << " with cycles: " << dec << cycles);
			mRamCount++;
		} else if (cycles > l3_threshold) {
//			DEBUG_STREAM(TAG,"L2 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL3Count++;
		} else if (cycles > l2_threshold) {
//			DEBUG_STREAM(TAG,"L1 MISS at address: " << hex << i << " with cycles: " << dec << cycles);
			mL2Count++;
		} else {
//			DEBUG_STREAM(TAG,"L1 HIT at address: " << hex << i << " with cycles: " << dec << cycles);
		}
		mCountSum++;
	}

	mLastAverage = cycles_sum / cAccessCount;
//	SYNC_OUTPUT_STREAM(TAG,"Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);

	// Every 100 iterations print out status
	if ((mIterations  % 100) == 0) {
		EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_printOut, 0);
	}
	//READ_CYCLE_COUNTER(after);
	//SYNC_OUTPUT_STREAM(TAG,"Execution duration on CPU: " << (uint16_t)getCPUID() << " :" << dec <<  (after-before) << endl);
#endif
	READ_CYCLE_COUNTER(measure_after);
	mLastCount++;
	mCycles += (measure_after - measure_before);
}

void StressTestComponent3::func_printOut(dword_t arg) {
	SYNC_OUTPUT_STREAM_START(TAG << ": L3 misses : " << dec << mRamCount << " rate: " << (mRamCount*1000000 / mCountSum) << " ppm ");
	SYNC_OUTPUT_STREAM_CONTINUE("L2 misses : " << dec << mL3Count << " rate: " << (mL3Count*1000000 / mCountSum) << " ppm ");
	SYNC_OUTPUT_STREAM_CONTINUE("L1 misses : " << dec << mL2Count << " rate: " << (mL2Count*1000000 / mCountSum) << " ppm ");
	SYNC_OUTPUT_STREAM_CONTINUE("Min : " << dec << mMin << " Max: " << mMax << " average: " << mLastAverage);
	SYNC_OUTPUT_STREAM_CONTINUE(" Avg runtime : " << dec << (mCycles/mLastCount) << " count: " << dec << mCountSum << endl);
	SYNC_OUTPUT_STREAM_END;
	mCountSum = 0;
	mRamCount = 0;
	mLastAverage = 0;
	mL2Count = 0;
	mL3Count = 0;
	mMax = 0;
	mMin = 0xFFFFFFFF;
	mLastCount = 0;
	mCycles = 0;
}

void StressTestComponent3::func_subscribe(dword_t arg) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent4)::StressTestComponent4::event_Continue, &trigger_doStress);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
}

DEFINE_TRIGGER_FUNC(StressTestComponent3,selfCall) {
	mIterations++;
	EventHandling::EventHandler::pInstance.callOSCTrigger(&trigger_selfCall, NULL);
}

END_OSC_IMPLEMENTATION
