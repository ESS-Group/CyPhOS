/*
 * startup_ap-64.S
 *
 *  Created on: 20.06.2018
 *      Author: hendrik
 */


.global startup_ap_start
.global startup_ap_end

.balign 4096
startup_ap_start:

// Needs to be aligned to 12bit boundary because only 8bit of the 20bit address
// is given as the vector during SIPI
.text
.code16

	// Disable interrupts
	cli
//	mov $0x4, %al
//	outb %al, $0x20 // Test code to panic bochs
	mov %cs, %ax
	mov %ax, %ds

	lidtl ap_idt - startup_ap_start
	lgdtl ap_gdt_80 - startup_ap_start

	// Activate protected mode
	mov	$0x1, %ax
	lmsw %ax
	jmp flush_instructions

flush_instructions:
	ljmpl $0x8,$init_longmode_ap


.set ap_gdt_offset, ap_gdt_zero - startup_ap_start
ap_gdt_zero:
	/* Null descriptor */
	.quad 0x0

ap_gdt_32_code:
	/* 32-bit code descriptor */
	.word 0xFFFF
	.word 0x0000
	.word 0x9A00
	.word 0x00CF
	// (4GB - (0x100000*0x1000 = 4GB) | base address: 0 | code read/exec | granularity:4096, 386

ap_gdt_data:
	/* Data segment descriptor */
	.word 0xFFFF
	.word 0x0000
	.word 0x9200
	.word 0x00CF // (4GB - (0x100000*0x1000 = 4GB) | base address: 0 | data read/write | granularity:4096, 386

ap_gdt_80:
	.word 24
	.long 0x40000 + ap_gdt_offset

ap_idt:
	.word 0x0
	.word 0x0
startup_ap_end:


