/*
 * StressTestComponent__THISCOMPONENT__.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestComponent___THISCOMPONENT__.h>

#include <common/SyncOutput.h>

#include "StressTestComponent___NEXTCOMPONENT__.h"

#define TAG "StressTestComponent__THISCOMPONENT__"

#include <arch/armv7/driver/ARMV7PerformanceCounter.h>

#include <common/RandomGenerator.h>

BEGIN_OSC_IMPLEMENTATION(StressTestComponent__THISCOMPONENT__,StressTestComponent__THISCOMPONENT__)

StressTestComponent__THISCOMPONENT__ StressTestComponent__THISCOMPONENT__::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestComponent__THISCOMPONENT__::trigger_deps_doStress[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_doStress = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_doStress,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + __THISCOMPONENT__ + 1};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_subscribe = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_subscribe,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + __THISCOMPONENT__ + 2};

SECTION_CRITICAL_DATA OSC* StressTestComponent__THISCOMPONENT__::trigger_deps_printOut[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger StressTestComponent__THISCOMPONENT__::trigger_printOut = {(OSC*)&StressTestComponent__THISCOMPONENT__::mInstance,(void (OSC::*)(dword_t))&StressTestComponent__THISCOMPONENT__::func_printOut,
				(OSC**)&StressTestComponent__THISCOMPONENT__::trigger_deps_printOut, EventHandling::Trigger::cMIN_Priority + 21000 + __THISCOMPONENT__ + 3};

SECTION_CRITICAL_DATA EventHandling::Event StressTestComponent__THISCOMPONENT__::event_Continue;

SECTION_CRITICAL_DATA DEFINE_TRIGGER(StressTestComponent__THISCOMPONENT__,selfCall,StressTestComponent__THISCOMPONENT__::mInstance,trigger_deps_doStress, EventHandling::Trigger::cMIN_Priority + 2003);


StressTestComponent__THISCOMPONENT__::StressTestComponent__THISCOMPONENT__() : mIterations(0), mMin(0xFFFFFFFF), mMax(0) {
//	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(StressTestComponent__NEXTCOMPONENT__)::StressTestComponent__NEXTCOMPONENT__::event_Continue, &trigger_doStress);
}

void StressTestComponent__THISCOMPONENT__::func_doStress(dword_t arg) {
//
//	uint32_t before,after;

	uint32_t cycles_sum = 0;

	const uint32_t count = 10000;

	const uint32_t ram_threshold = 100;
	const uint32_t l2_threshold = 25;

#define RANDOM_ACCESS
//#define SEQUENTIAL_ACCESS
#ifdef RANDOM_ACCESS
	//uint32_t before,after;
	//READ_CYCLE_COUNTER(before);
	mIterations++;
	cpu_t cpu = getCPUID();
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < count; i++) {
		uint32_t cycles;
		address = &mTestData[RandomGenerator::mInstance[cpu].random32() % cArraySize];
//		READ_CYCLE_COUNTER(before);
		__asm__ __volatile__ (
				//"MOV r0,#0x0;"
				//"MCR p15, 0, r0, c9, c13, 0;"
				"ISB;"
				"DSB;"
				"MRC p15, 0, r1, c9, c13, 0;"
				"LDR r0,[%[address]];"
				"ISB;"
				"DSB;"
				"MRC p15, 0, %[cycles], c9, c13, 0;"
				"SUB %[cycles],%[cycles], R1"
				:[cycles]"=r"(cycles):[address]"r"(address):"memory","r0","r1");
//		__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
//		READ_CYCLE_COUNTER(after);
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
	mLastAverage = cycles_sum / count;
//	SYNC_OUTPUT_STREAM(TAG,"Average random access time: " << dec << (cycles_sum / count) << " min: " << min << " max: " << max << endl);
	EventHandling::EventHandler::pInstance.eventTriggered(&event_Continue,0);
	//READ_CYCLE_COUNTER(after);
	//SYNC_OUTPUT_STREAM(TAG,"Execution duration on CPU: " << (uint16_t)getCPUID() << " :" << dec << (after-before) << endl);
#endif
#ifdef SEQUENTIAL_ACCESS
	//uint32_t before,after;
	//READ_CYCLE_COUNTER(before);
	mIterations++;
	dword_t *address = (dword_t*)mTestData;
	for (uint32_t i = 0; i < count; i++) {
		uint32_t cycles;
		address = (dword_t*)((uintptr_t)address + 4);
		__asm__ __volatile__ (
				"ISB;"
				"DSB;"
				"MRC p15, 0, r1, c9, c13, 0;"
				"LDR r0,[%[address]];"
				"ISB;"
				"DSB;"
				"MRC p15, 0, %[cycles], c9, c13, 0;"
				"SUB %[cycles],%[cycles], R1"
				:[cycles]"=r"(cycles):[address]"r"(address):"memory","r0","r1");
//		__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
//		READ_CYCLE_COUNTER(after);
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

	mLastAverage = cycles_sum / count;
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
