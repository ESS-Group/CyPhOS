#include "MemoryProfiling.h"

#include <common/debug.h>

#define TAG "MemoryProfiling"

#include <common/types.h>
#include <arch/armv7/driver/ARMV7PerformanceCounter.h>
#include <arch/armv7/driver/ARMV7CacheControl.h>
#include <arch/armv7/driver/ARMMMU.h>

#include <common/armhelper.h>
#include <common/RandomGenerator.h>

//#define SEQUENTIAL_ACCESS

/*
 * Entry points used by startup code if baremetal testing is enabled.
 */
void (*baremetal_entry_points[])() = {
		&entry_func_0,
		&entry_func_1,
		&entry_func_2,
		&entry_func_3
};
extern uintptr_t __critical_osc_start;
extern uintptr_t __critical_osc_end;

extern uintptr_t __text_segment__start;
extern uintptr_t __text_segment__end;

#define MEMORY_PROFILE_TEST_SIZE (262144 * 4)
//
//static __attribute__ ((aligned (0x100000))) uint8_t testarray_0[MEMORY_PROFILE_TEST_SIZE];
//static __attribute__ ((aligned (0x100000))) uint8_t testarray_1[MEMORY_PROFILE_TEST_SIZE];
//static __attribute__ ((aligned (0x100000))) uint8_t testarray_2[MEMORY_PROFILE_TEST_SIZE];
//static __attribute__ ((aligned (0x100000))) uint8_t testarray_3[MEMORY_PROFILE_TEST_SIZE];

static volatile uint32_t holding_pen = 0;

static __attribute__ ((aligned (0x100000))) volatile uint32_t testarray[MEMORY_PROFILE_TEST_SIZE];

static void fillArray() {
	for (uint32_t i = 0; i < MEMORY_PROFILE_TEST_SIZE; i++) {
		testarray[i] = (((i << 2) &~(0x3))|(3));
	}
}

static int32_t checkCoherency(uint32_t checkCPU, uint32_t currentCPU) {
	DEBUG_STREAM(TAG,"Check coherency for CPU: " << currentCPU << " and check CPU:" << checkCPU);
	uint32_t currentValue = 0;
	uint32_t checkValue = 0;
	for (uint32_t i = 0; i < MEMORY_PROFILE_TEST_SIZE; i++) {
		currentValue = testarray[i];
		checkValue = (((i << 2) &~(0x3))|(checkCPU));
		if (currentValue != checkValue) {
			DEBUG_STREAM(TAG,"Coherency error at: " << dec << i << " with checkCPU: " << dec << checkCPU << " and current CPU: " << currentCPU);
			DEBUG_STREAM(TAG,"Check value: " << hex << checkValue << " current value: " << currentValue);
			return 0;
		}
		testarray[i] = (((i << 2) &~(0x3))|(currentCPU));
	}
	return 1;
}

void entry_func_0() {
	DEBUG_STREAM(TAG, "entry_func_0 started");
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)testarray, (uintptr_t)testarray + MEMORY_PROFILE_TEST_SIZE, true);
	fillArray();

	while (1) {
		while((holding_pen % 4) != getCPUID()) {

		}
		if(checkCoherency(3,0)) {
			holding_pen++;
		}
	}
}

void entry_func_1() {
	DEBUG_STREAM(TAG, "entry_func_1 started");
	while (1) {
		while((holding_pen % 4) != getCPUID()) {

		}
		if (checkCoherency(0,1)) {
			holding_pen++;
		}
	}
}

void entry_func_2() {
	DEBUG_STREAM(TAG, "entry_func_2 started");
	while (1) {
		while((holding_pen % 4) != getCPUID()) {

		}
		if (checkCoherency(1,2)) {
			holding_pen++;
		}
	}
}

void entry_func_3() {
	DEBUG_STREAM(TAG, "entry_func_3 started");
	while (1) {
		while((holding_pen % 4) != getCPUID()) {

		}
		if (checkCoherency(2,3)) {
			holding_pen++;
		}
	}
}
