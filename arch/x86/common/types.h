/*
 * types.h
 *
 *  Created on: 20.03.2015
 *      Author: hendrik
 */

#ifndef COMMON_TYPES_H_
#define COMMON_TYPES_H_

typedef char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef long long int int64_t;
typedef long long unsigned int uint64_t;

typedef uint64_t cpu_affinity_t;

typedef uint32_t deadline_t;


typedef long unsigned int size_t;
typedef uint32_t dword_t;

#ifdef CONFIG_AMD64
typedef uint64_t uintptr_t;
#else
typedef uint32_t uintptr_t;
#endif

typedef unsigned char byte_t;

typedef uint32_t clock_t;

typedef byte_t cpu_t;

typedef uint64_t cycle_t;

typedef uint32_t priority_t;

#define NULL 0

#endif /* COMMON_TYPES_H_ */
