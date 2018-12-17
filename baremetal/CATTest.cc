#include "MemoryProfiling.h"

#include <common/debug.h>

#define TAG "CATTest"

#include <common/types.h>


#include <common/RandomGenerator.h>

#include <eventhandling/EventHandler.h>

#define WAY_COUNT 18

#include <sync/barrier.h>
#ifdef CONFIG_AMD64
#include <driver/CATManager.h>

static constexpr uint32_t cAccessCount = 50000;
static constexpr uint32_t cAccessRange = 1024 * 512;

static constexpr uint8_t cCacheWayDistance = 1;

extern "C" {
	void restart_system();
}


#define WARM_UP_RANGE(rangeStart, rangeEnd) \
	DEBUG_STREAM(TAG,"Warming up from: " << hex << rangeStart << " to: " << rangeEnd); \
		__asm__ __volatile__ ( "WBINVD\n" \
					"0: \n" \
					"prefetchw (%[start])\n" \
					"add %[flushSize],%[start]\n" \
					"cmp %[end],%[start]\n" \
					"jb 0b\n" \
					"mfence\n":[start]"=r"(rangeStart):[end]"r"(rangeEnd), [flushSize]"r"((uint64_t)64):"memory", "r14");


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

#define READ_CYCLE_COUNTER(cycles) __asm__ __volatile__ ( \
		"CPUID\n" \
		"RDTSC\n" \
		"SHL $32,%%rdx\n" \
		"OR %%rdx,%%rax\n" \
		"MOVQ %%rax,%[asmCycles]\n" \
		:[asmCycles]"=r"(cycles)::"memory","%rax","%rbx","%rcx","%rdx");

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


/*
 * Entry points used by startup code if baremetal testing is enabled.
 */
void (*baremetal_entry_points[])() = {
	&entry_func_0,
	&entry_func_1,
	&entry_func_2,
	&entry_func_3
};

extern uintptr_t profiling_space;
static Barrier barrier(4);


inline void warm_up_way_range(CacheManagement::cacheways_t way) {
	cycle_t cycles_before, cycles_after;
	READ_CYCLE_COUNTER(cycles_before);
	uintptr_t start = (uintptr_t)&profiling_space + (way * cAccessRange);
	uintptr_t end = (uintptr_t)&profiling_space + ((way + 1) * cAccessRange);
	CacheManagement::CATManager::sInstance.prefetchDataToWay(start,end,start,way);
	READ_CYCLE_COUNTER(cycles_after);
	DEBUG_STREAM(TAG,"Warmup," << dec << (uint16_t)way<< "," << (cycles_after-cycles_before));
}


inline void load_way_range(uint8_t way) {
	uint8_t cpuID = getCPUID();
	uintptr_t start = (uintptr_t)&profiling_space + (way * cAccessRange);
//	uintptr_t end = (uintptr_t)profiling_space + ((way + 1) * cAccessRange);
	uintptr_t address;
	for (uint32_t i = 0; i < cAccessCount; i++) {
		address = start + (RandomGenerator::mInstance[cpuID].random32() % cAccessRange);
		cycle_t cycles = 0;

		MEASURE_MEMORY_ACCESS(address, cycles);
	}
}

inline void evict_way_range(CacheManagement::cacheways_t way) {
	cycle_t cycles_before, cycles_after;
	READ_CYCLE_COUNTER(cycles_before);
	uintptr_t start = (uintptr_t)&profiling_space + (way * cAccessRange);
	uintptr_t end = (uintptr_t)&profiling_space + ((way + 1) * cAccessRange);
	CacheManagement::CATManager::sInstance.evictMemoryRange(start, (end - start));
	READ_CYCLE_COUNTER(cycles_after);
	DEBUG_STREAM(TAG,"Eviction," << dec << (uint16_t)way << "," << (cycles_after-cycles_before));
}



inline void measure_range(uintptr_t start, uintptr_t end) {
	cycle_t maxCycles = 0, minCycles = 0xFFFFFFFF;
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

	uintptr_t address;
	uint8_t cpuID = getCPUID();
	for (uint32_t i = 0; i < cAccessCount; i++) {
		address = start + RandomGenerator::mInstance[cpuID].random32() % (end - start);
		cycle_t cycles = 0;
		MEASURE_MEMORY_ACCESS(address, cycles);
//		DEBUG_STREAM(TAG,"Cycles: " << dec << cycles);
		cycles_sum += cycles;
		if (cycles < minCycles) {
			minCycles = cycles;
		}
		if (cycles > maxCycles) {
			maxCycles = cycles;
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

//	DEBUG_STREAM(TAG, "Average random access time: " << dec << (cycles_sum / cAccessCount) << " min: " << minCycles << " max: " << maxCycles);
//	DEBUG_STREAM(TAG, "L3 misses (estimated): " << dec << ramCount << " rate: " << (ramCount*100 / cAccessCount) << " %");
//	DEBUG_STREAM(TAG, "L2 misses (estimated): " << dec << l3Count << " rate: " << (l3Count*100 / cAccessCount) << " %");
//	DEBUG_STREAM(TAG, "L1 misses (estimated): " << dec << l2Count << " rate: " << (l2Count*100 / cAccessCount) << " %");
}

inline void measure_way_range(uint8_t way) {
	cycle_t cycles_before, cycles_after;
	READ_CYCLE_COUNTER(cycles_before);
	uintptr_t start = (uintptr_t)&profiling_space + (way * cAccessRange);
	uintptr_t end = (uintptr_t)&profiling_space + ((way + 1) * cAccessRange);
//	DEBUG_STREAM(TAG,"Measure way: " << dec << (uint16_t)way);
	measure_range(start, end);
	READ_CYCLE_COUNTER(cycles_after);
	DEBUG_STREAM(TAG,"Measure," << dec << (uint16_t)way<< "," << (cycles_after-cycles_before));
}

inline void secondary_cores() {
	uint8_t way = 0 + getCPUID();
	barrier.await();
	while (1) {

		load_way_range(way);
//		DEBUG_STREAM(TAG, "Continue");
		barrier.await();
		way = (way + 1) % WAY_COUNT;
//		CacheManagement::CATManager::sInstance.enableWayExclusive(way);
	}
}

inline void measure_core() {
	uint32_t localCount = 0;
	CacheManagement::CATManager::sInstance.printCacheWayInformation();
	uint8_t way = 0 + getCPUID();
	for (CacheManagement::cacheways_t initWay = 0; initWay < WAY_COUNT; initWay++) {
		warm_up_way_range(initWay);
		measure_way_range(initWay);
	}
	barrier.await();
	while (1) {
//		DEBUG_STREAM(TAG, "Warming up cache way: " << dec << (uint16_t)way);
		measure_way_range(way);

		evict_way_range(way);
		measure_way_range(way);

		warm_up_way_range(way);
		measure_way_range(way);

		barrier.await();
		way = (way + 1) % WAY_COUNT;
//		CacheManagement::CATManager::sInstance.enableWayExclusive(way);
//		// End execution
//		if (way == 0) {
//			while(1);
//		}
		localCount++;
		if (localCount == 1000) {
			restart_system();
		}
	}
}

void entry_func_0() {
	measure_core();
//	secondary_cores();
}

void entry_func_1() {
	secondary_cores();
//	measure_core();
}

void entry_func_2() {
	secondary_cores();
//	measure_core();
}

void entry_func_3() {
	secondary_cores();
}
