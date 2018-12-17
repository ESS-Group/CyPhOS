/*
 * debug.h
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#ifndef COMMON_DEBUG_H_
#define COMMON_DEBUG_H_

#include <common/cpu.h>

#ifdef CONFIG_DEBUG
#ifndef DEBUG_STREAM_DEVICE
#define DEBUG_STREAM_DEVICE
#error "Debug enabled but no debug out put stream defined"
#endif

#define DEBUG_STREAM(TAG,...)  DEBUG_STREAM_DEVICE << hex << (unsigned int)getCPUID() << ":" << TAG << ':' <<  __VA_ARGS__; DEBUG_STREAM_DEVICE.endLine()

#else
#define DEBUG_STREAM(TAG,...)

/* Empty defines for modifiers, otherwise the preprocessor will abort compilation with debugging disabled */
#endif

#define OUTPUT_STREAM DEBUG_STREAM_DEVICE

#define printMemoryAscending(start,range) { \
uintptr_t endPtr = (uintptr_t)start + (uintptr_t)range; \
	for ( uintptr_t itPtr = (uintptr_t)start; itPtr < endPtr; itPtr+=4) { \
		DEBUG_STREAM(TAG,hex << (void*)itPtr << ':' << *((uint32_t*)itPtr) ); \
	}\
	}


#define printMemoryDescending(start,range) { \
	uintptr_t endPtr = (uintptr_t)start - (uintptr_t)range; \
	for ( uintptr_t itPtr = (uintptr_t)start; itPtr > endPtr; itPtr-=4) { \
		DEBUG_STREAM(TAG,hex << (void*)itPtr << ':' << *((uint32_t*)itPtr) ); \
	} \
}


#endif /* COMMON_DEBUG_H_ */
