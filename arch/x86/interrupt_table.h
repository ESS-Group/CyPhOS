/*
 * interrupt_table.h
 *
 *  Created on: 29.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_INTERRUPT_TABLE_H_
#define ARCH_X86_INTERRUPT_TABLE_H_

enum HW_INTERRUPTS {
	HW_INT_KEYBOARD = 0x21,
	HW_INT_UART_1 = 0x24,
	HW_INT_LOCAL_APIC_TIMER = 0x42,
};



#endif /* ARCH_X86_INTERRUPT_TABLE_H_ */
