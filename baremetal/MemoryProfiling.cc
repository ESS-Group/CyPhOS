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

#define MEMORY_PROFILE_TEST_SIZE 262144

static __attribute__ ((aligned (0x100000))) uint8_t testarray_0[MEMORY_PROFILE_TEST_SIZE];
static __attribute__ ((aligned (0x100000))) uint8_t testarray_1[MEMORY_PROFILE_TEST_SIZE];
static __attribute__ ((aligned (0x100000))) uint8_t testarray_2[MEMORY_PROFILE_TEST_SIZE];
static __attribute__ ((aligned (0x100000))) uint8_t testarray_3[MEMORY_PROFILE_TEST_SIZE];

static uint32_t holding_pen = 0;

void entry_func_0() {
//	DEBUG_STREAM(TAG, "entry_func_0 started");
	uintptr_t start = (uintptr_t)testarray_0;
	uintptr_t end = (uintptr_t)testarray_0 + MEMORY_PROFILE_TEST_SIZE;
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)&__text_segment__start, (uintptr_t)&__text_segment__end, true);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)testarray_0, (uintptr_t)testarray_0 + MEMORY_PROFILE_TEST_SIZE, true);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)testarray_1, (uintptr_t)testarray_1 + MEMORY_PROFILE_TEST_SIZE, true);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)testarray_2, (uintptr_t)testarray_2 + MEMORY_PROFILE_TEST_SIZE, true);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t)testarray_3, (uintptr_t)testarray_3 + MEMORY_PROFILE_TEST_SIZE, true);
	DSB;
	ARMV7CacheControl::pInstance.clean_Data_Caches();
	ARMV7CacheControl::pInstance.pl310_cleanSharedCache();
	ARMMMU::pInstance.flushTLB();

//	// Unlock all cache-ways
//	for (uint8_t i = 0; i < PL310_CACHE_WAYS; i++) {
//		ARMV7CacheControl::pInstance.pl310_LockCacheWay(i, false);
//	}
//
//	// Lock all, resets data structure of cache management
//	for (uint8_t i = 0; i < PL310_CACHE_WAYS; i++) {
//		ARMV7CacheControl::pInstance.pl310_LockCacheWay(i, true);
//	}
//	ARMV7CacheControl::pInstance.preloadCriticalData((void*)testarray_0, (void*)((uintptr_t)testarray_0 + MEMORY_PROFILE_TEST_SIZE),(void*)testarray_0);
//	ARMV7CacheControl::pInstance.preloadCriticalData((void*)testarray_1, (void*)((uintptr_t)testarray_1 + MEMORY_PROFILE_TEST_SIZE),(void*)testarray_1);
//	ARMV7CacheControl::pInstance.preloadCriticalData((void*)testarray_2, (void*)((uintptr_t)testarray_2 + MEMORY_PROFILE_TEST_SIZE),(void*)testarray_2);
//	ARMV7CacheControl::pInstance.preloadCriticalData((void*)testarray_3, (void*)((uintptr_t)testarray_3 + MEMORY_PROFILE_TEST_SIZE),(void*)testarray_3);

	for (uint8_t i = 0; i < NR_CPUS; i++) {
		holding_pen++;
		__asm__ __volatile__ ("SEV;":::"memory");
		BUSY_WAITING_SHORT_NOCACHE;
	}
	while(1) {
		uint32_t before,after;
		dword_t *address = (dword_t*)start;
		for (uint32_t i = 0; i < ((end-start)/4); i++) {
#ifdef SEQUENTIAL_ACCESS
			address = (dword_t*)((uintptr_t)address + 4);
#else
			address = (dword_t*)(start + RandomGenerator::mInstance[0].random32() % MEMORY_PROFILE_TEST_SIZE);

			// Align to word size
			address = (dword_t*)((uintptr_t)address - ((uintptr_t)address % 4));
#endif
			READ_CYCLE_COUNTER(before);
			__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
			READ_CYCLE_COUNTER(after);
			DEBUG_STREAM(TAG,"" << dec << (after-before));
//			DEBUG_STREAM(TAG,"Address:" << hex << address << " - " << dec << (after-before));
		}

//		__asm__ __volatile__ ("WFE;":::"memory");
	}
}

void entry_func_1() {
	while (holding_pen != 1) {
		__asm__ __volatile__ ("WFE;":::"memory");
	}

		DEBUG_STREAM(TAG, "entry_func_1 started");
		uintptr_t start = (uintptr_t)testarray_1;
		uintptr_t end = (uintptr_t)testarray_1 + MEMORY_PROFILE_TEST_SIZE;
//		while(1){__asm__ __volatile__ ("WFE;":::"memory");}
		while(1) {
			uint32_t before,after;
			dword_t *address = (dword_t*)start;
			for (uint32_t i = 0; i < ((end-start)/4); i++) {
#ifdef SEQUENTIAL_ACCESS
			address = (dword_t*)((uintptr_t)address + 4);
#else
			address = (dword_t*)(start + RandomGenerator::mInstance[0].random32() % MEMORY_PROFILE_TEST_SIZE);

			// Align to word size
			address = (dword_t*)((uintptr_t)address - ((uintptr_t)address % 4));
#endif
				READ_CYCLE_COUNTER(before);
				__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
				READ_CYCLE_COUNTER(after);
//				DEBUG_STREAM(TAG,"" << dec << (after-before));
			}

	//		__asm__ __volatile__ ("WFE;":::"memory");
		}
}

void entry_func_2() {
	while (holding_pen != 2) {
		__asm__ __volatile__ ("WFE;":::"memory");
	}

		DEBUG_STREAM(TAG, "entry_func_2 started");
		uintptr_t start = (uintptr_t)testarray_2;
		uintptr_t end = (uintptr_t)testarray_2 + MEMORY_PROFILE_TEST_SIZE;
//		while(1){__asm__ __volatile__ ("WFE;":::"memory");}
		while(1) {
			uint32_t before,after;
			dword_t *address = (dword_t*)start;
			for (uint32_t i = 0; i < ((end-start)/4); i++) {
#ifdef SEQUENTIAL_ACCESS
			address = (dword_t*)((uintptr_t)address + 4);
#else
			address = (dword_t*)(start + RandomGenerator::mInstance[0].random32() % MEMORY_PROFILE_TEST_SIZE);

			// Align to word size
			address = (dword_t*)((uintptr_t)address - ((uintptr_t)address % 4));
#endif
				READ_CYCLE_COUNTER(before);
				__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
				READ_CYCLE_COUNTER(after);
//				DEBUG_STREAM(TAG,"" << dec << (after-before));
			}

	//		__asm__ __volatile__ ("WFE;":::"memory");
		}
}

void entry_func_3() {
	while (holding_pen != 3) {
		__asm__ __volatile__ ("WFE;":::"memory");
	}

		DEBUG_STREAM(TAG, "entry_func_3 started");
		uintptr_t start = (uintptr_t)testarray_3;
		uintptr_t end = (uintptr_t)testarray_3 + MEMORY_PROFILE_TEST_SIZE;
//		while(1){__asm__ __volatile__ ("WFE;":::"memory");}
		while(1) {
			uint32_t before,after;
			dword_t *address = (dword_t*)start;
			for (uint32_t i = 0; i < ((end-start)/4); i++) {
#ifdef SEQUENTIAL_ACCESS
			address = (dword_t*)((uintptr_t)address + 4);
#else
			address = (dword_t*)(start + RandomGenerator::mInstance[0].random32() % MEMORY_PROFILE_TEST_SIZE);

			// Align to word size
			address = (dword_t*)((uintptr_t)address - ((uintptr_t)address % 4));
#endif
				READ_CYCLE_COUNTER(before);
				__asm__ __volatile__ ("LDR r0,[%[address]];"::[address]"r"(address):"memory","r0");
				READ_CYCLE_COUNTER(after);
//				DEBUG_STREAM(TAG,"" << dec << (after-before));
			}

	//		__asm__ __volatile__ ("WFE;":::"memory");
		}
}
