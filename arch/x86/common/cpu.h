/*
 * cpu.h
 *
 *  Created on: 28.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_CPU_H_
#define ARCH_X86_COMMON_CPU_H_

#include <common/types.h>

#include <driver/SMPManager.h>
#include <driver/LocalAPIC.h>

static inline cpu_t getCPUID() {
	// FIXME return actual cpuID
	return SMPManager::sInstance.translateAPICIDToCPUID(LocalAPIC::mInstance.getAPICID());
}

static inline void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    __asm__ __volatile__ ( "outb %%al, $0x80" : : "a"(0) );
    /* %%al instead of %0 makes no difference.  TODO: does the register need to be zeroed? */
}

static inline uint64_t native_save_fl(void)
{
	uint64_t flags;

     /*
      * "=rm" is safe here, because "pop" adjusts the stack before
      * it evaluates its effective address -- this is part of the
      * documented behavior of the "pop" instruction.
      */
     __asm__ __volatile__("# __raw_save_flags\n\t"
              "pushf ; pop %0"
              : "=rm" (flags)
              : /* no input */
              : "memory");

     return flags;
}

// FIXME with real implementation
#define SEND_WAKE __asm__ __volatile__ ("NOP;")
#define WAIT_EVENT __asm__ __volatile__ ("NOP;")

#define BUSY_WAITING_SHORT_NOCACHE for ( volatile unsigned int i = 0; i< 200000;i++) {}
#define BUSY_WAITING_LONG_NOCACHE for ( volatile unsigned int i = 0; i< 10000000;i++) {}

#define BUSY_WAITING_SHORT for ( volatile unsigned int i = 0; i< 2500000;i++) {}
#define BUSY_WAITING_LONG for ( volatile unsigned int i = 0; i< 200000000;i++) {}


#endif /* ARCH_X86_COMMON_CPU_H_ */
