/*
 * config.h
 *
 *  Created on: 04.03.2016
 *      Author: hendrik
 */

#ifndef COMMON_CONFIG_H_
#define COMMON_CONFIG_H_

#define STATIC_INTERRUPT_MAPPING

#define CONFIG_DEBUG

//#define CONFIG_GENERIC_TIMER_DEBUG

//#define MEM_EVAL_PRELOAD_BENCH

//#define CONFIG_PRIO_TEST

//#define CONFIG_PREEMPT

/*
 * This enables/disables the CyPhOS controlled cache management.
 * Only disable it for benchmarking purposes!
 */
#define CONFIG_CACHE_CONTROL

/**
 * Defines if OSCs are directly evicted after use.
 */

#ifdef CONFIG_CACHE_CONTROL
//#define CONFIG_CACHE_CONTROL_EVICT_AFTER_USE

// DO NOT DISABLE THIS UNLESS YOU KNOW WHAT YOU ARE DOING.
// PLI IS NOT WORKING CORRECTLY AS TEST HAVE SHOWN
#define CONFIG_CACHE_PLDW_FOR_INSTRUCTION

// Enable SW-based page coloring
#define CONFIG_PAGE_COLORING

#ifdef CONFIG_PAGE_COLORING
//#define CONFIG_SET_ALL_PAGES_CACHEABLE
#endif
#endif

//#define CONFIG_ARM_STANDBY_CACHE_WAYS

/**
 * Set if either no lockdown by master is available or if performance suffers
 * from multiple prefetching operations.
 */
#define CONFIG_CACHE_CONTROL_LOCK_PRELOAD
//#define CONFIG_SEQUENTIAL_COLORING

//#define CONFIG_CACHE_DEBUG
//#define CONFIG_DISPATCHER_DEBUG
//#define CONFIG_DEBUG_LOCKING
//#define CONFIG_DEBUG_MMU
//#define CONFIG_DEBUG_EVENT_QUEUE

//#define CONFIG_GENERIC_TIMER_DEBUG

//#define EXCEPTION_DEBUGGING

// Enable multicore support
#define CONFIG_BOOT_MULTICORE

#ifdef CONFIG_BOOT_MULTICORE
#define CONFIG_AUTOMATIC_MULTICORE_BOOT
#endif

#define STACK_SIZE 16384

//#define CONFIG_SCHEDULER_SINGLE_FIFO
//#define CONFIG_SCHEDULER_MULTI_FIFO
#define CONFIG_SCHEDULER_PRIORITY_MULTI

#define CONFIG_IMX6_CLOCK_DEBUG
//#define CONFIG_DEBUG_SPI

//#define CONFIG_DEBUG_EVENT_QUEUE
//#define CONFIG_FORCE_CPUID_OUTPUT

#define CONFIG_PROFILING


#ifdef CONFIG_PROFILING // Don't remove to prevent half of profiling code inclusion
//#define CONFIG_PROFILING_EXECUTION
//#define CONFIG_PROFILING_PRELOAD
//#define CONFIG_PROFILING_PRELOAD_OVERHEAD
//#define CONFIG_PROFILING_WRITEBACK


#endif

#ifdef CONFIG_DYNAMICMEMORY_VERBOSE
	#define CONFIG_DYNAMICMEMORY_EXCEPTION_VERBOSE
#endif

//#define CONFIG_MEASURE_IDLE

//#define CONFIG_CACHE_DEBUG

/**
 * Don't enable unless you want to run an application without the OS
 */
//#define CONFIG_BAREMETAL_APPLICATION

//#define CONFIG_DEBUG_SYSTEMMODE


/**
 * Default priorities
 */
#define CONFIG_PRIORITY_TIMER EventHandling::Trigger::cMAX_Priority - 5
#define CONFIG_PRIORITY_UART EventHandling::Trigger::cMAX_Priority - 200
#define CONFIG_PRIORITY_CAN EventHandling::Trigger::cMAX_Priority - 1000

#endif /* COMMON_CONFIG_H_ */
