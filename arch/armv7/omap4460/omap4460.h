/*
 * omap4460.h
 *
 *  Created on: 30.04.2015
 *      Author: hendrik
 */

#ifndef COMMON_OMAP4460_H_
#define COMMON_OMAP4460_H_

/* Generic configuration options */
#define PERIPHBASE 0x48240000

#define OMAP4460

#define NR_CORES 2
#define NR_CPUS NR_CORES

/* Base addresses for the Generic interrupt controller distributor and
 * core interface
 */
#define GICD_BASE_ADDRESS PERIPHBASE + 0x1000
#define GICC_BASE_ADDRESS PERIPHBASE + 0x100

/* Timer */
#define GLOBAL_TIMER_BASE PERIPHBASE + 0x200

/* Set the PL310 cache controller specific functions here. Every SoC needs a different
 * way to enable the controller because some do it in the monitor mode (SMC call).
 */
#define PL310_ENABLE_FUNC omap4460_smc_enablePL310
#define PL310_DISABLE_FUNC omap4460_smc_disablePL310

/* End of the dynamic memory (1GB-1MB starting from 0x80000000) */
#define DRAM_MEMORY_START 0x80000000
#define DRAM_MEMORY_END 0xBFF00000

#define HARDWARE_PLATFORM "TI OMAP 4460"
#define HARDWARE_BOARD "TI PandaBoard ES"

#define TRANSLATION_TABLE_BASE (DRAM_MEMORY_END - 0x100000)

/* Base address for the pl310 level 2 cache controller */
#define PL310_BASE_ADDRESS (PERIPHBASE + 0x2000)
/* Number of cache-ways of PL310 last level cache */
#define PL310_CACHE_WAYS 16

#define CONFIG_CACHE_WAY_SIZE (64 * 1024) // 64 kB cache ways
#define CONFIG_CACHE_LINE_SIZE (32) // 32 bytes

/* Protect the header against asm include */
#ifndef __ASSEMBLY__

#include <common/config.h>

/* Include the interrupt table definition */
#include "omap4460_interrupt_table.h"

/* Include memory mapped special registers */
#include "omap4460_registers.h"

/* Include the available monitor calls for this SoC */
#include "omap_smc.h"

#ifdef CONFIG_DEBUG
#include <common/armhelper.h>
#include "OMAPDebug.h"
#define DEBUG_STREAM_DEVICE OMAPDebug::pInstance
#endif

#include "driver/OMAPUartDriver.h"
#define CONFIG_UART_CONSOLE_DRIVER &OSC_PREFIX(OMAPUart)::OMAPUart

/* Define the number of interrupts, used for the dispatcher to determine table lengths */
#define NR_INTERRUPTS NR_SW_INTERRUPTS + NR_PPI_INTERRUPTS + NR_HW_INTERRUPTS

#include <arch/armv7/omap4460/driver/OSCOMAP4460ClockManagement.h>
#define ARM_CLOCK_MANAGEMENT &OSC_OMAP4460ClockManagement::pInstance

/* Define the memory map for this platform */
#include <arch/armv7/omap4460/omap4460_mem_map.h>
#define ARM_MEMORY_MAP OMAP4460_MEM_MAP

#endif

#endif /* COMMON_OMAP4460_H_ */
