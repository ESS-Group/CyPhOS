/*
 * imx6.h
 *
 *  Created on: 30.04.2015
 *      Author: hendrik
 */

#ifndef COMMON_IMX6_H_
#define COMMON_IMX6_H_
/* Generic defines/config options that are ASM compatible */

#define PERIPHBASE 0x00A00000

/* Base addresses for the Generic interrupt controller distributor and
 * core interface
 */
#define GICD_BASE_ADDRESS PERIPHBASE + 0x1000
#define GICC_BASE_ADDRESS PERIPHBASE + 0x100

/* Base address for the pl310 level 2 cache controller */
#define PL310_BASE_ADDRESS (PERIPHBASE + 0x2000)
/* Number of cache-ways of PL310 last level cache */
#define PL310_CACHE_WAYS 16

#define PL310_LOCKDOWN_BY_LINE

#define CONFIG_CACHE_WAY_SIZE (64 * 1024) // 64 kB cache ways
#define CONFIG_CACHE_LINE_SIZE (32) // 32 bytes

/* Is lockdown by master implemented here */
#define PL310_LOCKDOWN_BY_MASTER
#define PL310_LOCKDOWN_BY_MASTER_LOCK_ALL

#define NR_CORES 4
#define NR_CPUS NR_CORES

/* Timer */
#define GLOBAL_TIMER_BASE PERIPHBASE + 0x200


#define HARDWARE_PLATFORM "Freescale i.MX6"
#define HARDWARE_BOARD "Wandboard"


#define DRAM_MEMORY_START 0x10000000
/* End of the dynamic memory (1GB-1MB starting from 0x80000000) */
#define DRAM_MEMORY_END 0x90000000

#define TRANSLATION_TABLE_BASE (DRAM_MEMORY_END - 0x100000)

/**
 * Set if the boot rom enables level 1 caches
 */
#define CONFIG_EARLY_L1_CACHE_ENABLE

/**
 * Set if the Boot rom enables the L2 cache.
 */
#define CONFIG_EARLY_PL310_ENABLE

#define CONFIG_IMX6_CLOCK_DEBUG

#define CONFIG_CAN

#define CONFIG_IMX6_WATCHDOG

#define CONFIG_IMX6_TEMPMONITOR

/* Protect the header against asm include */
#ifndef __ASSEMBLY__

#include <common/config.h>

// FIXME for now this configuration is only for the Wandboard Quad

#define IMX6

/* Include the interrupt table definition */
#include "imx6_interrupt_table.h"

/* Include memory mapped special registers */
#include "imx6_registers.h"


/* Set the PL310 cache controller specific functions here. Every SoC needs a different
 * way to enable the controller because some do it in the monitor mode (SMC call).
 */
#include "imx6_smc.h"
#define PL310_ENABLE_FUNC imx6_smc_enablePL310
#define PL310_DISABLE_FUNC imx6_smc_disablePL310


// Only enable debug output if DEBUG is defined
//#ifdef CONFIG_DEBUG
#include <common/armhelper.h>
#include <arch/armv7/imx6/IMX6Debug.h>
#define DEBUG_STREAM_DEVICE IMX6Debug::pInstance

#include <common/debug.h>
//#endif

#define CONFIG_UART_CONSOLE_DRIVER &OSC_PREFIX(InputOutput)::IMX6Uart

//#define CONFIG_IMX6_GPIO

#define CONFIG_IMX6_CAN_INTERRUPT

/* Define the number of interrupts, used for the dispatcher to determine table lengths */
#define NR_INTERRUPTS NR_SW_INTERRUPTS + NR_PPI_INTERRUPTS + NR_HW_INTERRUPTS

/* Define the memory map for this platform */
#include <arch/armv7/imx6/imx6_mem_map.h>
#define ARM_MEMORY_MAP IMX6_MEMORY_MAP

#include <arch/armv7/imx6/driver/IMX6ClockManagement.h>
#define ARM_CLOCK_MANAGEMENT &IMX6ClockManagement::mInstance

// no asm part __ASSEMBLY__
#endif

#endif /* COMMON_OMAP4460_H_ */
