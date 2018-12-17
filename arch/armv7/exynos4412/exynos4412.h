/*
 * exynos4412.h
 *
 *  Created on: 30.04.2015
 *      Author: hendrik
 */

#ifndef COMMON_EXYNOS4412_H_
#define COMMON_EXYNOS4412_H_

#define HARDWARE_PLATFORM "Samsung Exynos 4412"
#define HARDWARE_BOARD "Hardkernel ODroid-U3"
/* Generic defines/config options that are ASM compatible */
#define PERIPHBASE 0x10500000

#define NR_CORES 4
#define NR_CPUS NR_CORES

/* Base addresses for the Generic interrupt controller distributor and
 * core interface
 */
#define GICD_BASE_ADDRESS 0x10490000
#define GICC_BASE_ADDRESS 0x10480000

/* Peripheral base address + 0x200
 * s. cortex-a9 mp-core trm p. 18 */
#define GLOBAL_TIMER_BASE 0x139D0000

#define GIC_NO_BANKED_REGISTERS

/* Base address for the pl310 level 2 cache controller */
#define PL310_BASE_ADDRESS 0x10502000
/* Number of cache-ways of PL310 last level cache */
#define PL310_CACHE_WAYS 16

#define CONFIG_CACHE_WAY_SIZE (64 * 1024) // 64 kB cache ways
#define CONFIG_CACHE_LINE_SIZE (32) // 32 bytes

/* End of the dynamic memory (2GB-1MB starting from 0x40000000) */
#define DRAM_MEMORY_END 0xBFF00000

#define TRANSLATION_TABLE_BASE (DRAM_MEMORY_END - 0x100000)

/**
 * Set if the boot rom enables level 1 caches
 */
#define CONFIG_EARLY_L1_CACHE_ENABLE

/**
 * Enables the exynos multicore timer as a timer
 */
#define CONFIG_ENABLE_EXYNOS_MCT

#ifdef CONFIG_DEBUG
/* Define to include clock management diagnosis code */
#define CONFIG_CLOCK_DIAGNOSIS
#endif

#define EXYNOS4412

/* Protect the header against asm include */
#ifndef __ASSEMBLY__



// Only enable debug output if DEBUG is defined

#include <common/armhelper.h>

#include <common/config.h>



/* Interrupt handling defines */
#include "exynos_interrupt_table.h"

/* See linux: arch/arm/plat-samsung/include/plat/map-base.h */
#define S3C_ADDR_BASE	0xF6000000
#define S3C_ADDR(x)	(S3C_ADDR_BASE + (x))

#define EXYNOS4_PA_SYSRAM0		0x02025000
#define EXYNOS4_PA_SYSRAM1		0x02020000
#define EXYNOS4x12_PA_SYSRAM_NS 0x0204F000


#define S5P_PA_PMU		0x10020000
#define S5P_PMUREG(x)				(S5P_PA_PMU + (x))
#define S5P_ARM_CORE1_STATUS			S5P_PMUREG(0x2084)
#define S5P_ARM_CORE1_CONFIGURATION		S5P_PMUREG(0x2080)
#define S5P_CORE_LOCAL_PWR_EN			0x3



#include "ExynosDebug.h"
#define DEBUG_STREAM_DEVICE ExynosDebug::pInstance

#include <common/debug.h>

#include "driver/ExynosUartDriver.h"
#define CONFIG_UART_CONSOLE_DRIVER &OSC_PREFIX(InputOutput)::ExynosUart

/* Define the number of interrupts, used for the dispatcher to determine table lengths */
#define NR_INTERRUPTS NR_SW_INTERRUPTS + NR_PPI_INTERRUPTS + NR_HW_INTERRUPTS

/* Set the PL310 cache controller specific functions here. Every SoC needs a different
 * way to enable the controller because some do it in the monitor mode (SMC call).
 */
#include "exynos_smc.h"
#define PL310_ENABLE_FUNC exynos_smc_enablePL310
#define PL310_DISABLE_FUNC exynos_smc_disablePL310

#define PL310_LOCKDOWN_BY_LINE

#include <arch/armv7/exynos4412/driver/ExynosClockManagement.h>
#define ARM_CLOCK_MANAGEMENT &ExynosClockManagement::mInstance

/* Define the memory map for this platform */
#include <arch/armv7/exynos4412/exynos_mem_map.h>
#define ARM_MEMORY_MAP EXYNOS4412_MEM_MAP


#endif

#endif /* COMMON_EXYNOS4412_H_ */
