/*
 * X86CacheManagement.cc
 *
 *  Created on: 16.07.2018
 *      Author: hendrik
 */

#include <arch/x86/cachemanagement/X86CacheManagement.h>

#include <common/debug.h>
#include <common/cpuid.h>

#include <common/cyclecounter.h>

#define TAG "X86CacheMgmt"

namespace CacheManagement {

X86CacheManagement::X86CacheManagement() : GenericCacheManagement() {
	// FIXME this is a placeholder for systems without CAT
	mCacheWayCount = 16;

	uint32_t eax = 0x1; // AMD64 Architecture Programmer's Manual Vol.3 - p. 632
	uint32_t ebx = 0x0;
	uint32_t ecx = 0x0;
	uint32_t edx = 0x0;

	// Read clflush size
	readCPUID(&eax, &ebx, &ecx, &edx);

	mCLFlushSize = ((ebx >> 8) & 0xFF) * 8; // Size defined in "quadwords"
}

void X86CacheManagement::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration) {
#ifdef CONFIG_PROFILING_PRELOAD
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif
	__asm__ __volatile__ ("0: \n"
			"movq (%[start]),%%rax\n"
			"add %[flushSize],%[start]\n"
			"cmp %[end],%[start]\n"
			"jb 0b\n"
			"mfence\n"::[start]"r"(start), [end]"r"(end), [flushSize]"r"((uint64_t)mCLFlushSize):"memory","rax");
#ifdef CONFIG_PROFILING_PRELOAD
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Prefetched from: " << hex << start << " to: " << end);
#endif
}

void X86CacheManagement::evictCacheWay(cacheways_t way, cycle_t *duration) {
	uintptr_t oscStart = (uintptr_t)mCacheWays[way].dataStart;
	uintptr_t oscEnd = (uintptr_t)mCacheWays[way].dataEnd;
#ifdef CONFIG_PROFILING_WRITEBACK
	cycle_t before = 0, after = 0;
	RESET_READ_CYCLE_COUNTER(before);
#endif
	__asm__ __volatile__ (
			"mfence\n"
			"0: \n"
			"clflush (%[start])\n"
			"add %[flushSize],%[start]\n"
			"cmp %[end],%[start]\n"
			"jb 0b\n"
			"mfence\n"::[start]"r"(oscStart), [end]"r"(oscEnd), [flushSize]"r"((uint64_t)mCLFlushSize):"memory");
#ifdef CONFIG_PROFILING_WRITEBACK
	READ_CYCLE_COUNTER(after);
	*duration = (after-before);
#endif
#ifdef CONFIG_CACHE_DEBUG
	DEBUG_STREAM(TAG,"Evicted OSC: " << hex << oscStart << " with clflush size: " << mCLFlushSize);
#endif
}

void X86CacheManagement::evictMemoryRange(uintptr_t start, uint64_t size) {
	__asm__ __volatile__ ("mfence\n"
			"0: \n"
			"clflush (%[start])\n"
			"add %[flushSize],%[start]\n"
			"cmp %[end],%[start]\n"
			"jb 0b\n"
			"mfence\n"::[start]"r"(start), [end]"r"((uintptr_t)start + size), [flushSize]"r"((uint64_t)mCLFlushSize):"memory");
}

}
