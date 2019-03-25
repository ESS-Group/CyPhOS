#/*
 * initOS.cc
 *
 *  Created on: 10.11.2016
 *      Author: hendrik
 */

/* Debug helpers */
#define TAG "A9_INIT"

#include <common/armhelper.h>

#include <common/memreg.h>
#include <common/types.h>

/* Include GIC driver for interrupt control */
#include <driver/ARMGIC.h>

#include <core/SystemMode.h>

/* Include Cache control driver */
#include <driver/ARMV7CacheControl.h>

/* Include PL310 driver */
#include <cachemanagement/PL310CacheManagement.h>

/* Event dispatcher */
#include <eventhandling/EventHandler.h>

/* Include performance monitoring code */
#include <driver/ARMV7PerformanceCounter.h>

/* Include Cortex-A9 generic init code */
#include <arch/armv7/cortexa9/common/cortexa9_generics.h>

#include <eventhandling/StackPool.h>

/* Include A9 MMU init code */
#include <driver/ARMMMU.h>
#include <driver/ARMv7PageTable.h>

/* Include startup message */
#include <common/greeting.h>

/* Include header for Cortex-A9 boot to have prototypes from soc specific functions */
#include <arch/armv7/cortexa9/boot/initOS.h>

#include <testcomponents/UARTConsole.h>

#include <testcomponents/ApplicationSystemInfo.h>

#include "criticalOSCSymbols.h"

extern "C" {
void init_primary_cpu_c();
void init_secondary_cpu_c();
}

volatile int32_t holding_pen;

/**
 * External symbols.
 */
extern dword_t first_jump_table_start;



extern uintptr_t __benchmark_results__start;
extern uintptr_t __benchmark_results__end;

extern uintptr_t __pagestable_region_start;
extern uintptr_t __pagestable_region_end;

extern uintptr_t __oscs__start;
extern uintptr_t __oscs__end;


#ifdef CONFIG_BAREMETAL_APPLICATION
extern void __attribute__ ((weak)) (*baremetal_entry_points[])();
#endif

/**
 * This function waits for the secondary core to finish its initialization boot code.
 * When finished the core sets the holding_pen variable to -1 and the primary core can
 * continue to boot up or initialize other secondary cores.
 */
inline void waitForSecondary() {
//	while ( holding_pen!= -1 ) {
//		//DEBUG_STREAM(TAG,"Wait for secondary: " << holding_pen);
//	}
	__asm__ __volatile__ (
			"MOV r1,#-1 \n"
			"0: LDR r0,[%[hp]] \n"
			"CMP r0,r1 \n"
			"BNE 0b \n"
			: : [hp]"r"(&holding_pen) : "r0","r1","memory");
}

/**
 * This method sets the holding_pen variable to the value of var.
 * It also sends an ARM sev to wakeup possible waiting cores.
 */
inline void setHoldingPen(int32_t val) {
	__asm__ __volatile__ (
			"STR %[val],[%[hp]];"
			"DSB;"
			"ISB;"
			: : [hp]"r"(&holding_pen), [val]"r"(val) : "r0","r1","memory");
}

/**
 * This is the main entry function for the primary cortex-a9 core. It will boot up
 * the secondary cores and initialize the peripheral hardware of the SoC when finished.
 */
[[noreturn]] void init_primary_cpu_c() {
	/* Disable interrupts here to be sure */
	Core::SystemMode::mInstance.disableInterrupts();

	/* Print a friendly greeting message */
	printGreeting();

#ifdef CONFIG_BAREMETAL_APPLICATION
	/* Print warning about baremetal test mode */
	DEBUG_STREAM(TAG,"WARNING: Baremetal test mode is active. No event dispatching will be started!!!");
#endif

	/* Print some core status/control registers for debug purposes */
	DEBUG_STREAM(TAG,"ARM periphalbase address: " << hex << PERIPHBASE);DEBUG_STREAM(TAG,"Stackpointer: " << hex << getStackpointer());DEBUG_STREAM(TAG,"SAC: " << hex << READ_REGISTER(PERIPHBASE+0x50));
	WRITE_REGISTER(PERIPHBASE+0x54, 0xfff);DEBUG_STREAM(TAG,"SNSAC: " << hex << READ_REGISTER(PERIPHBASE+0x54));

	/* Populate the pagetable entries */
	DEBUG_STREAM(TAG,"Initialize the pagetable");
	ARMv7PageTable::sInstance.fillLinear();

	ARMMMU::pInstance.setMemoryMap();

	/* Set certain regions to cacheable */
	DEBUG_STREAM(TAG,"Set OSC region to cacheable");
#if !defined(CONFIG_ARMV7_CACHE_COLORING)
	ARMMMU::pInstance.setRangeCacheable((uintptr_t) &__critical_osc_start, (uintptr_t) &__critical_osc_end, true);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t) &__oscs__start, (uintptr_t) &__oscs__end, true);
#endif

	ARMMMU::pInstance.setRangeCacheable((uintptr_t) &__pagestable_region_start, (uintptr_t) &__pagestable_region_end, false);
	ARMMMU::pInstance.setRangeCacheable((uintptr_t) &__benchmark_results__start, (uintptr_t) &__benchmark_results__end, false);

	/* Set the vbar address which defines where the exception vector table is located */
	/* See first_stage_handler.* */
	DEBUG_STREAM(TAG,"Setting VBAR (vector base address register) to: " << hex << &first_jump_table_start);
	setVBAR((dword_t) &first_jump_table_start);
	DEBUG_STREAM(TAG,"VBAR now: " << hex << readVBAR());

	/* Now init the Memory Management*/
//	DynamicMemory::MemoryAllocator::mInstance.init();

	/* Activate the pagetable */
	DEBUG_STREAM(TAG,"Initialized the pagetable");
	ARMMMU::pInstance.activateMMU();

	//ARMMMU::pInstance.flushTLB();

	/* Enable cycle counter */
	ARMV7PerformanceCounter::pInstance.enableGlobal();
	ARMV7PerformanceCounter::pInstance.enableCycleCount();
	ARMV7PerformanceCounter::pInstance.writeEventCounterType(ARMV7PerformanceCounter::ARM_event_t::EVENT_MEMORY_ACCESS, 0x0);
	ARMV7PerformanceCounter::pInstance.printInformation();

	/* Enable the level 1 private caches */
	DEBUG_STREAM(TAG, "L1 Cache enabling");
	CacheManagement::ARMV7CacheControl::pInstance.enableL1Caches();

	/* Enable the SMP mode to allow coherence later. */
	DEBUG_STREAM(TAG, "SMP mode enabled");
	CacheManagement::ARMV7CacheControl::pInstance.enableSMPMode();

	DEBUG_STREAM(TAG, "Prefetching enabled");
	CacheManagement::ARMV7CacheControl::pInstance.enablePrefetching();

	/* Enable the level 2 shared cache controller */
	DEBUG_STREAM(TAG, "L2 Cache enabling");
	CacheManagement::PL310CacheManagement::sInstance.setEnabled(true);
	DEBUG_STREAM(TAG, "Cache enabled");
	CacheManagement::ARMV7CacheControl::pInstance.printCacheInformation();


	/* Finalize MemoryAllocator initialization */
//	DynamicMemory::MemoryAllocator::mInstance.init_last();

	DEBUG_STREAM(TAG,"SCTLR: " << hex << getSCTLR() << " ACTLR: " << getACTLR());


	/* If os-controlled cache management is enabled, make sure that the critical parts (interrupt,eventhandling) are
	 * locked & loaded in the cache.
	 */
#ifdef CONFIG_CACHE_CONTROL
	DEBUG_STREAM(TAG,"CyPhOS cache management is active: " << hex << CacheManagement::GenericCacheManagement::sInstance);
	/* Load critical OSC with eventhandler, IRQ dispatching ... to cache */
	CacheManagement::GenericCacheManagement::sInstance->preloadCriticalData((void*)&__critical_osc_start,(void*)&__critical_osc_end,(void*)&__critical_osc_text_end);

	/* Load/lock benchmarking handler to cache */
	//ARMV7CacheControl::pInstance.preloadCriticalData((void*)&__benchmark_osc_start,(void*)&__benchmark_osc_end);
#else
	DEBUG_STREAM(TAG,"CyPhOS cache management is inactive");
#endif


	/* Iterate of available cores and boot them up */
#ifdef CONFIG_BOOT_MULTICORE
	for (uint8_t core = 1; core < NR_CPUS; core++) {
		setHoldingPen(core);
		startup_core(core);
		/* Wait for the secondary core to finish its initialization */
		waitForSecondary();
	}
	DEBUG_STREAM(TAG,"All cores should be up now :-)");
#endif


	/* Initialize the ARM generic interrupt controller distributor & core interface. */
	DEBUG_STREAM(TAG,"Start initializing GIC");
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.init();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.coreInterfaceInit();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setDistributorControl(true);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.dumpStatus();
	DEBUG_STREAM(TAG,"GIC initialized");


	/* Enable interrupts for the primary core in the GIC */
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setCoreControl(true);

	// FIXME enable when cortex-a9 SW/PPI interrupt tables are unified
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(SW_INT_STACKTRACE, 0xF);
//	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(SW_INT_STACKTRACE);

	/* Call SoC specific initialization here. This can be used to initialize
	 * peripheral devices.
	 */
	init_soc();

	// Print initial cache information
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::trigger_printCacheManagementStats,0x0);

	OSC_PREFIX(UARTConsole)::UARTConsole::pInstance.start();

//	StackPool::pInstance.dumpStacks();
	DEBUG_STREAM(TAG,"Boot up complete. Have fun ;-)");
	/* This sets all the secondary cores free, which are idling in a loop. */
#ifdef CONFIG_AUTOMATIC_MULTICORE_BOOT
	holding_pen = 4711 + NR_CPUS;
	SEND_WAKE;
#endif




	//MemoryAllocator::mInstance.printPagetable();
#ifdef CONFIG_BAREMETAL_APPLICATION
	void (*entry_func)(void);
	entry_func = baremetal_entry_points[getCPUID()];
	(*entry_func)();
#endif

	/* This function call will never return */
	activateDispatching();
}

[[noreturn]] void init_secondary_cpu_c() {
	/* Disable interrupts here to be sure */
	Core::SystemMode::mInstance.disableInterrupts();

	DEBUG_STREAM(TAG,"Booting up secondary CPU");

	cpu_t cpuNr = getCPUID();

	/* Print some debug information */
	DEBUG_STREAM(TAG,"Stackpointer: "<< getStackpointer());
	DEBUG_STREAM(TAG,"SCU_Control_Reg: " << hex << READ_REGISTER(PERIPHBASE + 0x0));
	DEBUG_STREAM(TAG,"SCU_Configuration Reg:" << hex << READ_REGISTER(PERIPHBASE + 0x4));

	/* Enable the SMP mode to allow coherence later. */
	CacheManagement::ARMV7CacheControl::pInstance.enableSMPMode();
	DEBUG_STREAM(TAG, "SMP mode enabled");

	DEBUG_STREAM(TAG, "Prefetching enabled");
	CacheManagement::ARMV7CacheControl::pInstance.enablePrefetching();

	/* Enable the level 1 private caches */
	CacheManagement::ARMV7CacheControl::pInstance.enableL1Caches();
	DEBUG_STREAM(TAG, "L1 Cache enabled");


	/* Enable virtual memory mapping */
	ARMMMU::pInstance.activateMMU();

	DEBUG_STREAM(TAG, "Enable cycle counter");
	/* Enable cycle counter */
	ARMV7PerformanceCounter::pInstance.enableGlobal();
	ARMV7PerformanceCounter::pInstance.enableCycleCount();

	/* See first_stage_handler.* */
	DEBUG_STREAM(TAG,"Setting VBAR (vector base address register) to: " << hex << &first_jump_table_start);
	/* Exception handling */
	/* Set the vbar address which defines where the vector table is located */
	setVBAR((dword_t) &first_jump_table_start);
	DEBUG_STREAM(TAG,"VBAR now: " << hex << readVBAR());

	/* Enable core interface for interrupt handling */
	DEBUG_STREAM(TAG,"Start initializing GIC");
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.dumpStatus();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.dumpCoreStatus();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.coreInterfaceInit();
	DEBUG_STREAM(TAG,"GIC initialized");


	/* Enable interrupts for this secondary core in the GIC */
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setCoreControl(true);

	DEBUG_STREAM(TAG,"SCTLR: " << hex << getSCTLR() << " ACTLR: " << getACTLR());

	/* Release the holding pen, so another core can boot up */
	DEBUG_STREAM(TAG,"Core is running...");

	/* Allows primary core to proceed, need coherence setup here. */
	setHoldingPen(-1);

	/*
	 * It is critical that everything for interrupt handling is setup
	 * correctly here because the eventhandler will enable the interrupts for this core.
	 */

	/* Wait until the primary core gives the go to handle events. */
	while (holding_pen < (4711 + cpuNr)) {
		WAIT_EVENT;
	}


	/* Call SoC specific initialization here. This can be used to initialize
	 * peripheral devices.
	 */
	init_soc_secondary();

#ifdef CONFIG_BAREMETAL_APPLICATION
	void (*entry_func)(void);
	entry_func = baremetal_entry_points[getCPUID()];
	(*entry_func)();
#endif

	enableInterruptBit();

	/* This function call will never return */
	activateDispatching();
}

