/*
 * idt.h
 *
 *  Created on: 05.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_INTERRUPTS_IDT_H_
#define ARCH_X86_INTERRUPTS_IDT_H_

#include <common/types.h>

struct IDTDescription {
	uint16_t size;
	uint64_t idtAddress;
} __attribute__((packed));

/**
 * Interrupt-Gate and Trap-Gate descriptors in longmode. (Defined on page 93, 24593 - Rev. 3.29 AMD64 Vol.2)
 */
struct IDTEntry {
	uint16_t offset1;
	uint16_t targetSelector;
	uint16_t ist:3,reserved1:5, type:4, zero0:1, dpl:2, p:1;
	uint16_t offset2;
	uint32_t offset3;
	uint32_t reserved2;
} __attribute__((packed));


#endif /* ARCH_X86_INTERRUPTS_IDT_H_ */
