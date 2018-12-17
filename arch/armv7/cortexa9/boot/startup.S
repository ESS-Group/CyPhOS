

#include <arch/armv7/armcpubits.asm>
#include <component/OSC.h>

.global _boot_first_stage

#ifndef PERIPHBASE
#error "No periphbase set"
#endif

.global global_arm_periphbase

.section .boot,"ax"
.align 0x1
_boot_first_stage:
	/**
	  * Set the stackpointers here, to allow save jump to c-functions and back
	 */

	/* Set the stackpointer of the interrupt mode */
	/* Switch to interrupt mode */
	MSR CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
	LDR sp,=__irq_stack_start

	MSR CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
	LDR sp,=__irq_stack_start

	MSR CPSR_c,#(UND_MODE | I_BIT | F_BIT)
	LDR sp,=__irq_stack_start

	MSR CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
	LDR sp,=__irq_stack_start

	/* Switch back to system-mode and reset stackpointer
	just in case */
	MSR CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
	LDR sp,=__boot_stack_start

	/* Check if cpuid == 0 */
	MRC P15,0,R0,c0,c0,5 // read Multiprocessor ID register
	AND R0,R0,#0x3
	CMP R0,#0x1
	BCS _halt_non_boot

	/* Disable SMP bit in Aux control register
	   to get a save state after the boot rom which is always the same.
	 */
	MRC     p15, 0, r0, c1, c0, 1
	BIC     r0, #(1 << 6) //  SMP bit
	MCR  p15, 0, r0, c1, c0, 1 //  write Aux Control Register

	// Disable alignment check
	MRC p15, 0, R0, c1, c0, 0
	BIC r0, #0x2
	MCR p15, 0, R0, c1, c0, 0



#ifdef CONFIG_EARLY_PL310_ENABLE
	/**
	 * Disable the L2 PL310 cache controller (boot rom enables it)
	 */
	BL armv7_disable_pl310
#endif

#ifdef CONFIG_EARLY_L1_CACHE_ENABLE
	/**
	  * Disable L1 Data/instruction caches.
	  */
    BL OSC_FUNC_ASM_SYM(CacheControl,asm_clean_data_caches)
	BL OSC_FUNC_ASM_SYM(CacheControl,asm_disable_unified_cache)
    BL OSC_FUNC_ASM_SYM(CacheControl,asm_invalidate_instruction_cache)
	BL OSC_FUNC_ASM_SYM(CacheControl,asm_disable_instruction_cache)
#endif

	/* Read the arm periphbase address */
	LDR r0,global_arm_periphbase

/*
   Cortex-A9 bringup acording to A9 MPCore TRM p. 81
   Disable the SCU (in case it is already active). Invalidate it.
   Invalidate the data cache
   Enable the SCU
   Enable the data cache
   Enable SMP mode
   */

	/* Enable the SCU (Snoop control unit) */
	// Load the SCU Control register (PERIPHBASE + 0x0)
	LDR r1,[r0]
	// r1 = SCU control register, bit0=1 => enabled
    BIC r1,r1,#0x1
    // Save the SCU control register (with SCU disabled)
	STR r1,[r0]

	// Enable unsecure access for all cores to the SCU (including global,private timers)
	LDR r1,[r0,#0x54]
	LDR R2,=0xFFF
	ORR r1,r1,r2
	STR R1,[R0,#0x54]

	// Invalidate SCU
	MOV r1,#0x0
	STR r1,[r0,#0x0c]
	LDR R1, =#0xFFFF
	STR r1,[r0,#0x0c]

	// reenable SCU
	LDR r1,[r0,#0x0]
	ORR	r1,r1,#0x1
	STR r1,[r0,#0x0]

	/* Enable SMP bit in Aux control register */
	MRC     p15, 0, r0, c1, c0, 1
	//  Set SMP bit to 1
	ORR   r0, r0, #(1 << 6) //  Set SMP bit in aux control register
	MCR   p15, 0, r0, c1, c0, 1 //  write Aux Control Register

	MRC p15,0,r0,c1,c0,2    // Read CP Access register
	ORR r0,r0,#0x00f00000   // Enable full access to NEON/VFP (Coprocessors 10 and 11)
	MCR p15,0,r0,c1,c0,2    // Write CP Access register
	ISB

	MOV r3, #0x40000000 //  enable FPU in FP exception register
	//  VMSR FPEXC, r3    # assembler bug
	.long 0xeee83a10



	/* Clear (fill with zeros) the .bss segment */
_boot_clear_bss:
    MOV r0,#0x0
    LDR r1,=__bss_start
    LDR r2,=__bss_end

_boot_clear_bss_loop:
    CMP r1,r2
    STRLO r0,[r1]
    ADDLO r1,r1,#0x4
    BLO _boot_clear_bss_loop

/**
  * Call C++ constructors here.
  */
_boot_constructor:
    LDR r4,=__init_array_start
    LDR r5,=__init_array_end
_boot_constructor_loop:
    CMP r4,r5
    BHS _boot_constructor_done
    LDR r6,[r4], #0x4
    LDR r7,=_boot_constructor_loop
    MOV lr,r7
    BLX r6
    B       _boot_constructor_loop
_boot_constructor_done:

/**
  * This jumps to the c based operating system code.
  */
_boot_c_initOS:
	B init_primary_cpu_c

	/* Branch to endless loop if not core 0 */
_halt_non_boot:
	WFE
	B _halt_non_boot


global_arm_periphbase:
	.word PERIPHBASE
