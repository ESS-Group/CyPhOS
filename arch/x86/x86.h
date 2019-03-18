#ifndef X86_H
#define X86_H


#define NR_CORES 4
#define NR_CPUS NR_CORES

#define CONFIG_CACHE_WAY_SIZE (512 * 1024) // 512 kB cache ways
#define CONFIG_CACHE_LINE_SIZE (64) // 64 bytes

#define HARDWARE_PLATFORM "Generic AMD64"
#define HARDWARE_BOARD "undefined"

#define CONFIG_UART_CONSOLE_DRIVER &OSC_PREFIX(InputOutput)::X86Uart

#define BOOT_STACK_SIZE (0x1000)

/* Define the number of interrupts, used for the dispatcher to determine table lengths */
#define NR_INTERRUPTS 256

#define CONFIG_AMD64

//#include <arch/x86/driver/PIT.h>
#define GENERIC_TIMER OSC_PREFIX(Timer)::PIT::mInstance


/* Protect the header against asm include */
#ifndef __ASSEMBLY__

#include <common/config.h>



#include <arch/x86/driver/DebugStream.h>
#define DEBUG_STREAM_DEVICE DebugStream::mInstance

#ifdef CONFIG_CACHE_CONTROL
#ifdef CONFIG_PAGE_COLORING
#define CONFIG_AMD64_CACHE_COLORING
#else
#define CONFIG_AMD64_CAT
#endif
#else
// FIXME if no cache management is active use CAT as it robust enough to work on systems without CAT
// Change to empty x86 cache management in the future
#define CONFIG_AMD64_CAT
#endif

// DEBUG DEFINES
#define CONFIG_DEBUG_ACPI
#ifdef CONFIG_DEBUG_MMU
#define CONFIG_X86_DEBUG_MMU
#endif
#define CONFIG_X86_PRINT_MADT_ENTRIES
#define CONFIG_X86_DEBUG_IOAPIC
//#define CONFIG_X86_DEBUG_CAT


#define CONFIG_X86_DISCARD_SMI_PROFILING


#endif // __ASSEMBLY__
#endif
