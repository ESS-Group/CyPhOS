/* boot.S - bootstrap the kernel */
/* Copyright (C) 1999, 2001, 2010  Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define ASM_FILE	1


#include "multiboot2.h"
#include <interrupts/interrupts.h>

/* C symbol format. HAVE_ASM_USCORE is defined by configure.  */
#ifdef HAVE_ASM_USCORE
# define EXT_C(sym)			_ ## sym
#else
# define EXT_C(sym)			sym
#endif

/* The size of our stack (16KB).  */
#define STACK_SIZE			0x4000

/* The flags for the Multiboot header.  */
#ifdef __ELF__
# define AOUT_KLUDGE 0
#else
# define AOUT_KLUDGE MULTIBOOT_AOUT_KLUDGE
#endif



.text
.code32
	.globl	start, _start
start:
_start:
	jmp	multiboot_entry

	/* Align 64 bits boundary.  */
	.align	8

multiboot_header:
	/* magic */
	.long	MULTIBOOT2_HEADER_MAGIC
	/* ISA: i386 */
	.long   MULTIBOOT_ARCHITECTURE_I386
	/* Header length.  */
	.long	multiboot_header_end - multiboot_header
	/* checksum */
	.long -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))
#ifndef __ELF__
address_tag_start:
	.short MULTIBOOT_HEADER_TAG_ADDRESS
	.short MULTIBOOT_HEADER_TAG_OPTIONAL
	.long address_tag_end - address_tag_start
	/* header_addr */
	.long	multiboot_header
	/* load_addr */
	.long	_start
	/* load_end_addr */
	.long	_edata
	/* bss_end_addr */
	.long	_end
address_tag_end:
entry_address_tag_start:
	.short MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
	.short MULTIBOOT_HEADER_TAG_OPTIONAL
	.long entry_address_tag_end - entry_address_tag_start
	/* entry_addr */
	.long multiboot_entry
entry_address_tag_end:
#endif /* __ELF__ */
	.short MULTIBOOT_HEADER_TAG_END
	.short 0
	.long 8
multiboot_header_end:

.section .data

.align 4
gdt_zero:
	/* Null descriptor */
	.quad 0x0

gdt_32_code:
	.set gdt_32_code_segment, gdt_32_code - gdt_zero
	/* 32-bit code descriptor */
	.word 0xFFFF
	.word 0x0000
	.word 0x9A00
	.word 0x00CF
	// (4GB - (0x100000*0x1000 = 4GB) | base address: 0 | code read/exec | granularity:4096, 386

gdt_64_code:
	.set gdt_64_code_segment, gdt_64_code - gdt_zero
	/* 64-bit code descriptor */
	.word 0xFFFF
	.word 0x0000
	.word 0x9A00
	.word 0x00AF // (4GB - (0x100000*0x1000 = 4GB) | base address: 0 | code read/exec | granularity:4096, 386, long mode

gdt_data:
	/* Data segment descriptor */
	.word 0xFFFF
	.word 0x0000
	.word 0x9200
	.word 0x00CF // (4GB - (0x100000*0x1000 = 4GB) | base address: 0 | data read/write | granularity:4096, 386

gdt_80:
	.word 32
	.quad gdt_zero


.text
multiboot_entry:
	/* Initialize the stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset EFLAGS.  */
	pushl	$0
	popf

	/* Push the pointer to the Multiboot information structure.  */
	movl %ebx, multiboot2_information_pointer
	/* Push the magic value.  */
	movl %eax, multiboot2_magic_value

	/* Disable all interrupts */
	cli

	/* Clear the DF-flag for GCCs string operations */
	cld

	lgdt (gdt_80)

	jmp init_longmode


init_longmode:
/* Use 32 bit mode to be sure */
.code32

	// Enable SSE, amd64 defines it as a requirement
	mov %cr0, %eax
	and $0xFFFB, %ax
	or $0x2, %ax
	mov %eax, %cr0
	mov %cr4, %eax
	or $(3 << 9), %ax
	mov %eax, %cr4

	// activate PAE and pagetable setup
	mov     %cr4,%eax
	or      $(1 << 5), %eax
	mov     %eax, %cr4

	call setup_paging


	// activate long-mode within compatibility-mode
	mov     $0x0C0000080,%ecx        // EFER MSR
	rdmsr
	or      $(1 << 8),%eax
	wrmsr

	// enable paging
	mov     %cr0, %eax
	or      $(1 << 31), %eax
	mov     %eax, %cr0


	// Change to longmode by changing the segment descriptor to (64 bit code)
	ljmp    $gdt_64_code_segment, $longmode_entry



longmode_entry:
/* Use 64 bit mode now */
.code64

	// Call constructors
	call _init

	// Move stack to correct one for processor core
	// Get the processor id (Local APIC ID), retval in %rax
	call _getCPUID

// Multiply constant
// FIXME define this offset somewhere
	mov $BOOT_STACK_SIZE,%rbx
// Calculate the offset
	mul %rbx
// Load the stack start address
	mov $__boot_stack_start,%rbx
// Subtract the actual offset value
	sub %rax,%rbx
// Set the stackpointer
	mov %rbx, %rsp

	mov %rsp, %rdi

	// Jump to C++ initialization code
	jmp	EXT_C(init_primary_cpu_c)


.code64

_init:

    mov     $__init_array_start,%rbx
_init_loop:
    cmp     $__init_array_end, %rbx
    je      _init_done
    movq	(%rbx), %rax
    call    *%rax
    add     $8, %rbx
    jmp      _init_loop
_init_done:
	ret

.global low_level_isr_entry_array_start

.balign 8
low_level_isr_entry_array_start:

index = 0
.rept 256
		/*mov $0xa000, %rax
	jmp %rax*/
	pushq %rdi
	mov $index, %rdi
	jmp low_level_isr_generic_handler
/*	mov $0x41,%al
	mov $0x3F8, %dx
	outb %al, %dx*/

	.fill low_level_isr_entry_array_start + (index+1)*ISR_VECTOR_ENTRY_SIZE - ., 1, 0xaa

index = index + 1
.endr

low_level_isr_entry_array_end:


low_level_isr_generic_handler:
	cld

	// Save registers
	pushq %rax
	pushq %rbx
	pushq %rcx
	pushq %rdx
	pushq %rbp
	pushq %rsi
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15
	mov %rsp,%rsi

	call exception_irq_interrupt_c

	// Restore registers
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rsi
	popq %rbp
	popq %rdx
	popq %rcx
	popq %rbx
	popq %rax
	// Restore rdi, saved by entry vector
	popq %rdi

	// Return from interrupt (mind the q here!)
	iretq


.global init_longmode_ap

init_longmode_ap:

	.code32
	cli

	cld

	// Fix data segment
	mov $0x10, %ax
	mov %ax, %ds

	lgdt (gdt_80)

	mov $(3 * 8), %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs

	/* Initialize the stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	// Enable SSE, amd64 defines it as a requirement
	mov %cr0, %eax
	and $0xFFFB, %ax
	or $0x2, %ax
	mov %eax, %cr0
	mov %cr4, %eax
	or $(3 << 9), %ax
	mov %eax, %cr4

	// activate PAE and pagetable setup
	mov     %cr4,%eax
	or      $(1 << 5), %eax
	mov     %eax, %cr4

	call enable_paging


	// activate long-mode within compatibility-mode
	mov     $0x0C0000080,%ecx        // EFER MSR
	rdmsr
	or      $(1 << 8),%eax
	wrmsr

	// enable paging
	mov     %cr0, %eax
	or      $(1 << 31), %eax
	mov     %eax, %cr0

	// Change to longmode by changing the segment descriptor to (64 bit code)
	ljmp    $gdt_64_code_segment, $longmode_entry_ap



longmode_entry_ap:
/* Use 64 bit mode now */
.code64

	// Move stack to correct one for processor core
	// Get the processor id (Local APIC ID), retval in %rax
	call _getCPUID

// Multiply constant
// FIXME define this offset somewhere
	mov $BOOT_STACK_SIZE,%rbx
// Calculate the offset
	mul %rbx
// Load the stack start address
	mov $__boot_stack_start,%rbx
// Subtract the actual offset value
	sub %rax,%rbx
// Set the stackpointer
	mov %rbx, %rsp

	mov %rsp, %rdi

	// Jump to C++ initialization code
	jmp	EXT_C(init_secondary_cpu_c)

.balign 8
	/* Our stack area.  */
	.comm	stack, STACK_SIZE




.data
.global multiboot2_magic_value
.global multiboot2_information_pointer

multiboot2_magic_value:
.long 0x0

multiboot2_information_pointer:
.long 0x0
