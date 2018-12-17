#include "MemoryProfiling.h"

#include <common/debug.h>

#define TAG "MemoryProfiling"

#include <common/types.h>

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

#define MEMORY_PROFILE_TEST_SIZE (1024 * 1024 * 10)

//static __attribute__ ((aligned (0x200000))) uint8_t profiling_space[MEMORY_PROFILE_TEST_SIZE];

extern uintptr_t profiling_space;

static volatile uint32_t holding_pen = 0;

#define CACHE_LINE_SIZE 64

// Size to fool the prefetcher
#define MEASURE_STEP_SIZE 512

#ifdef CONFIG_AMD64
//DEBUG_STREAM(TAG,"Warming up from: " << hex << rangeStart << " to: " << rangeEnd);

#define WARM_UP_RANGE(rangeStart, rangeEnd) \
		__asm__ __volatile__ ("0: \n" \
					"movq (%[start]),%%r14\n" \
					"add %[flushSize],%[start]\n" \
					"cmp %[end],%[start]\n" \
					"jb 0b\n" \
					"mfence\n":[start]"=r"(rangeStart):[end]"r"(rangeEnd), [flushSize]"r"((uint64_t)CACHE_LINE_SIZE):"memory", "r14");

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

#ifdef CONFIG_ARMV7

#define WARM_UP_RANGE(rangeStart, rangeEnd) \
		__asm__ __volatile__ ("" \
				".arch_extension mp\n" \
				"mov r0,%[start]\n" \
				"0: PLDW [r0]; B 3f;" \
				"ADD r0, #32\n" \
				"CMP %[end],r0\n" \
				"BGE 0b\n" \
				""::[start]"r"(rangeStart),[oscEnd]"r"(rangeEnd):"r0","memory");

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

void entry_func_0() {
	DEBUG_STREAM(TAG, "entry_func_0 started");

	while (1) {
		for (uint64_t size = 32768; size < 1024 * 1024 * 32; size = size * 2) {
			DEBUG_STREAM(TAG, "Size: " << dec << size);
			uintptr_t start = (uintptr_t) &profiling_space;
			uintptr_t end = (uintptr_t) &profiling_space + size;

			cycle_t maxCycles = 0, minCycles = 0xFFFFFFFF;
			cycle_t cycles_sum = 0;

			uint64_t count = 0;

			WARM_UP_RANGE(start, end);
			WARM_UP_RANGE(start, end);
			start = (uintptr_t) &profiling_space;
			for (uintptr_t i = start; i < end; i += MEASURE_STEP_SIZE) {
				cycle_t cycles = 0;

				MEASURE_MEMORY_ACCESS(i, cycles);
				cycles_sum += cycles;
				if (cycles < minCycles) {
					minCycles = cycles;
				}
				if (cycles > maxCycles) {
					maxCycles = cycles;
				}

				DEBUG_STREAM(TAG, "CYCLES: " << dec << cycles);
				count++;
			}
			DEBUG_STREAM(TAG, "Access count: " << dec << count);
			DEBUG_STREAM(TAG, "Average random access time: " << dec << (cycles_sum / count) << " min: " << minCycles << " max: " << maxCycles);
		}
		DEBUG_STREAM(TAG, "Finished run");
	}
}

void entry_func_1() {
	DEBUG_STREAM(TAG, "entry_func_1 started");
	while (1) {
		while ((holding_pen % 4) != getCPUID()) {

		}
	}
}

void entry_func_2() {
	DEBUG_STREAM(TAG, "entry_func_2 started");
	while (1) {
		while ((holding_pen % 4) != getCPUID()) {

		}
	}
}

void entry_func_3() {
	DEBUG_STREAM(TAG, "entry_func_3 started");
	while (1) {
		while ((holding_pen % 4) != getCPUID()) {

		}
	}
}
