#include <arch/armv7/armcpubits.asm>
#include <component/OSC.h>

#define IRQ_STACK_SIZE 512

/* Note the section name here, it is different from primary boot */
.section .sboot,"ax"
.align 0x1
.global _secondary_boot

/**
 * Entry point for secondary cpus. They'll start their execution right here after being woken up
 * the bootrom initializes their caches and jumps here. This means we should deactivate the caches
   again to get a predicatable setup.
 */
_secondary_boot:
	/* Disable SMP bit in Aux control register */
	MRC     p15, 0, r0, c1, c0, 1
	BIC     r0, #(1 << 6) //  SMP bit
	MCR  p15, 0, r0, c1, c0, 1 //  write Aux Control Register

	// Disable alignment check
	MRC p15, 0, R0, c1, c0, 0
	BIC r0, #0x2
	MCR p15, 0, R0, c1, c0, 0

	/* Calculate stack offset (corenum*STACK_SIZE)*/
	/* FIXME STACK_SIZE constant consistent between linker script and this code */
	/* Set the stackpointer of the interrupt mode */
	/* Switch to interrupt mode */
	MSR CPSR_c,#(IRQ_MODE | I_BIT | F_BIT)
	/* Read the core number */
	MRC P15,0,R0,c0,c0,5
	/* Mask the bits for the core number, discards everything else */
	AND R0,R0,#0x3
	/* Load the stack size constant */
	MOV R1,#IRQ_STACK_SIZE
	/* Calculate offset within stack space */
    MUL R2,R1,R0
    /* Load stack start */
    LDR R0,=__irq_stack_start
    /* Decreasing stack so subtract offset */
    SUB R0,R0,R2
    /* Set stackpointer */
	MOV sp,R0

	MSR CPSR_c,#(ABT_MODE | I_BIT | F_BIT)
	/* Read the core number */
	MRC P15,0,R0,c0,c0,5
	/* Mask the bits for the core number, discards everything else */
	AND R0,R0,#0x3
	/* Load the stack size constant */
	MOV R1,#IRQ_STACK_SIZE
	/* Calculate offset within stack space */
    MUL R2,R1,R0
    /* Load stack start */
    LDR R0,=__irq_stack_start
    /* Decreasing stack so subtract offset */
    SUB R0,R0,R2
    /* Set stackpointer */
	MOV sp,R0

	MSR CPSR_c,#(UND_MODE | I_BIT | F_BIT)
	/* Read the core number */
	MRC P15,0,R0,c0,c0,5
	/* Mask the bits for the core number, discards everything else */
	AND R0,R0,#0x3
	/* Load the stack size constant */
	MOV R1,#IRQ_STACK_SIZE
	/* Calculate offset within stack space */
    MUL R2,R1,R0
    /* Load stack start */
    LDR R0,=__irq_stack_start
    /* Decreasing stack so subtract offset */
    SUB R0,R0,R2
    /* Set stackpointer */
	MOV sp,R0

	MSR CPSR_c,#(SVC_MODE | I_BIT | F_BIT)
	/* Read the core number */
	MRC P15,0,R0,c0,c0,5
	/* Mask the bits for the core number, discards everything else */
	AND R0,R0,#0x3
	/* Load the stack size constant */
	MOV R1,#IRQ_STACK_SIZE
	/* Calculate offset within stack space */
    MUL R2,R1,R0
    /* Load stack start */
    LDR R0,=__irq_stack_start
    /* Decreasing stack so subtract offset */
    SUB R0,R0,R2
    /* Set stackpointer */
	MOV sp,R0

	/* Switch back to system-mode and reset stackpointer
	just in case */
	MSR CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
	MRC P15,0,R0,c0,c0,5
	AND R0,R0,#0x3
	MOV R1,#512
    MUL R2,R1,R0
    LDR R0,=__boot_stack_start
    SUB R0,R0,R2
	MOV sp,R0

#ifdef CONFIG_EARLY_L1_CACHE_ENABLE
	// Disable both caches (data/instruction)
	BL OSC_FUNC_ASM_SYM(CacheControl,asm_disable_unified_cache)
	BL OSC_FUNC_ASM_SYM(CacheControl,asm_disable_instruction_cache)
#endif

	MRC p15,0,r0,c1,c0,2    // Read CP Access register
	ORR r0,r0,#0x00f00000   // Enable full access to NEON/VFP (Coprocessors 10 and 11)
	MCR p15,0,r0,c1,c0,2    // Write CP Access register
	ISB

	MOV r3, #0x40000000 //  enable FPU in FP exception register
	//  VMSR FPEXC, r3    # assembler bug
	.long 0xeee83a10


	/* Jump to c++- secondary core entry */
	B init_secondary_cpu_c

	/* Should not be reached */
halt_secondary:
	WFE
	B halt_secondary
