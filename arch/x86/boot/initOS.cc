/*
 * initOS.cc
 *
 *  Created on: 30.05.2018
 *      Author: hendrik
 */

/* Include startup message */
#include <common/greeting.h>

#include <core/SystemMode.h>

// Include IDT manager
#include <interrupts/IDTManager.h>

// Include ACPI driver
#include <driver/ACPI.h>

#include <driver/LegacyPIC.h>

// Include LAPIC driver
#include <driver/LocalAPIC.h>

#include <driver/IOAPIC.h>

// Include Keyboard driver to enable interrupt
#include <driver/LegacyKeyboard.h>

// Include UART driver to enable interrupt
#include <driver/UART.h>

#include <core/SystemMode.h>

#include <driver/LegacyKeyboard.h>

#include <eventhandling/EventHandler.h>

#include <eventhandling/StackPool.h>

#include <testcomponents/UARTConsole.h>

#include <driver/GenericTimer.h>

#include <driver/SMPManager.h>
#ifdef CONFIG_AMD64_CAT
#include <driver/CATManager.h>
#endif

#include <cachemanagement/GenericCacheManagement.h>

#include <driver/X86MMU.h>
#include <driver/X86Pagetable.h>

#include <common/cpuid.h>

#define TAG "INIT"

extern "C" {
void init_primary_cpu_c(void *stackpointer);
void init_secondary_cpu_c(void *stackpointer);
}

extern uintptr_t multiboot2_magic_value;
extern uintptr_t multiboot2_information_pointer;

extern uintptr_t __benchmark_results__start;
extern uintptr_t __benchmark_results__end;

/**
 * Symbols for critical OSC
 */
extern uintptr_t __critical_osc_start;
extern uintptr_t __critical_osc_text_end;
extern uintptr_t __critical_osc_end;

extern uintptr_t __oscs__start;
extern uintptr_t __oscs__end;


extern uintptr_t __pagetable_start;
extern uintptr_t __pagetable_end;

#ifdef CONFIG_BAREMETAL_APPLICATION
extern void __attribute__ ((weak)) (*baremetal_entry_points[])();
#endif

void printMemory(uintptr_t start, uint64_t length) {
	while (length != 0) {
		DEBUG_STREAM(TAG, "" << hex << start << ": " << *(uint32_t*)start << " " << (start + 4) << ": " << *(uint32_t*)(start + 4));
		if (length < 8) {
			length = 0;
		} else {
			length -= 8;
		}
		start += 8;
	}
}

/**
 * This is the main entry function for the primary X86 core. It will boot up
 * the secondary cores and initialize the peripheral hardware of the SoC when finished.
 */
[[noreturn]] void init_primary_cpu_c(void *stackpointer) {
	Core::SystemMode::mInstance.disableInterrupts();
	DEBUG_STREAM(TAG, "Stackpointer: " << hex << stackpointer);
	/* Print a friendly greeting message */
	printGreeting();

	char vendor[] = "ABCDEFGHIJKL";
	uint32_t code = 0;
	readCPUID(&code, (uint32_t*) (vendor), (uint32_t*) (vendor + 8), (uint32_t*) (vendor + 4));
	DEBUG_STREAM(TAG, "Vendor: " << vendor);


	X86Pagetable::sInstances[0].fillLinear();
	X86MMU::mInstance.printInformation();
	X86MMU::mInstance.activatePagetable(X86Pagetable::sInstances[0].getBaseAddress());

	/* Set certain regions to cacheable */
	DEBUG_STREAM(TAG, "Set OSC region to cacheable");
	X86MMU::mInstance.setRangeCacheable((uintptr_t) &__critical_osc_start, (uintptr_t) &__critical_osc_end, true);
	X86MMU::mInstance.setRangeCacheable((uintptr_t) &__oscs__start, (uintptr_t) &__oscs__end, false);

	X86MMU::mInstance.setRangeCacheable((uintptr_t) &__benchmark_results__start, (uintptr_t) &__benchmark_results__end, false);
	X86MMU::mInstance.setRangeCacheable((uintptr_t) &__pagetable_start, (uintptr_t) &__pagetable_end, false);

	DEBUG_STREAM(TAG, "Multiboot2 magic value: " << hex << (uint32_t)*&multiboot2_magic_value);
	DEBUG_STREAM(TAG, "Multiboot2 information pointer: " << hex << (uint32_t)*&multiboot2_information_pointer);

	uint32_t *information_start = (uint32_t*) (uintptr_t) ((uint32_t) (*&multiboot2_information_pointer));
	uint32_t information_size = *information_start;

	information_start += 2;
	information_size -= 8;

	uint32_t count = 0;
	while (information_size != 0) {
		uint32_t type = *information_start;
		uint32_t size = *(information_start + 1);
		// Print all available information tags
		DEBUG_STREAM(TAG, "Found information type: " << dec << type << " with size: " << hex << size);

		// FIXME use values from multiboot2.h (not compilable with c++ at the moment)
		// Try to use RSDP table from grub here because otherwise it is missing on qemu with kvm enabled
		if (type == 14 || type == 15) {
			printMemory((uintptr_t) information_start, size);
			DEBUG_STREAM(TAG, "Found RSDP copy from multiboot, try to use it");
			ACPI::mInstances[0].registerExternalRSDP((RSDP2*) (((uintptr_t) information_start) + 8));
		}

		information_size -= size;
		information_start = (uint32_t*) ((uintptr_t) information_start + size);

		// Check 8 byte alignment
		uintptr_t misalignment = 8 - ((uintptr_t) information_start) % 8;
		if (misalignment != 8) {
			information_start = (uint32_t*) ((uintptr_t) information_start + misalignment);
			information_size -= misalignment;
		}

		count++;
	}

	// Initialize the Local APIC
	LocalAPIC::mInstance.initialize();
	LocalAPIC::mInstance.printRegisters();
	LocalAPIC::mInstance.printDebug();

	// Initialize ACPI management
	ACPI::mInstances[0].initACPI();

	// Initialize & calibrate periodic hardware timer
	OSC_PREFIX(Timer)::GenericTimer::pInstance->initializeTimer();

#ifdef CONFIG_AMD64_CAT
	// Initialize CAT (if available)
	CacheManagement::CATManager::sInstance.initialize();
#endif

	/* If OS-controlled cache management is enabled, make sure that the critical parts (interrupt,eventhandling) are
	 * locked & loaded in the cache.
	 */
#ifdef CONFIG_CACHE_CONTROL
	DEBUG_STREAM(TAG, "CyPhOS cache management is active");
	/* Load critical OSC with eventhandler, IRQ dispatching ... to cache */
	CacheManagement::GenericCacheManagement::sInstance->preloadCriticalData((void*) &__critical_osc_start, (void*) &__critical_osc_end, (void*) &__critical_osc_text_end);
#else
	DEBUG_STREAM(TAG,"CyPhOS cache management is inactive");
#endif

	// Boot secondary processors
#ifdef CONFIG_BOOT_MULTICORE
	SMPManager::sInstance.initializeSMP();
#endif

#ifdef CONFIG_BAREMETAL_APPLICATION
	void (*entry_func)(void);
	entry_func = baremetal_entry_points[getCPUID()];
	(*entry_func)();
#endif

	// Enable keyboard interrupts
	OSC_PREFIX(InputOutput)::LegacyKeyboard::mInstance.setInterruptEnabled(true);

	// Reset & enable UART interrupts
	OSC_PREFIX(InputOutput)::UART::mInstances[0].resetController();

	// Start initial UARTConsole
	OSC_PREFIX(UARTConsole)::UARTConsole::pInstance.start();

	// Start event dispatching
	activateDispatching();
}

[[noreturn]] void init_secondary_cpu_c(void *stackpointer) {
	Core::SystemMode::mInstance.disableInterrupts();
	DEBUG_STREAM(TAG, "Stackpointer: " << hex << stackpointer);

//	X86Pagetable::sInstances[getCPUID()].copyFromPagetable(&X86Pagetable::sInstances[0]);
	X86MMU::mInstance.activatePagetable(X86Pagetable::sInstances[0].getBaseAddress());

	LegacyPIC::mInstance.disable();

	// Initialize the Local APIC
	LocalAPIC::mInstance.initialize();
	DEBUG_STREAM(TAG, "secondary LocalAPIC initialized");

#ifdef CONFIG_AMD64_CAT
	CacheManagement::CATManager::sInstance.initialize();
#endif

	IDTManager::mInstance.loadIDT();

	X86MMU::mInstance.printInformation();

	SMPManager::sInstance.confirmStartup(getCPUID());

#ifdef CONFIG_BAREMETAL_APPLICATION
	void (*entry_func)(void);
	entry_func = baremetal_entry_points[getCPUID()];
	(*entry_func)();
#endif

	Core::SystemMode::mInstance.enableInterrupts();
	// Start event dispatching
	activateDispatching();
}
