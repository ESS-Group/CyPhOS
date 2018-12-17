#include <common/ARM_common.h>

.code 32
.section .irq_handler,"ax"

.global exception_reset
.global exception_undefined_instruction
.global exception_supervisor_call
.global exception_prefetch_abort
.global exception_data_abort
.global exception_hypervisor
.global exception_fiq_interrupt

exception_reset:
	/* Just branch to C reset handler and stop execution */
	B exception_reset_c

exception_undefined_instruction:
	STMFD sp!,{r0-r12}

	/* The LR of the usr/sys mode and the SPSR still needs saving */
	/* Change to system mode to get the LR and SP */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)

	MOV R0,LR
	MOV R1,SP

	MSR CPSR_c,#(CPU_MODE_UNDEFINED | CPSR_I_BIT | CPSR_F_BIT)

	/* Save old linkregister */
	PUSH {R0}

	/* Save old stackpointer */
	PUSH {R1}

	/*  Save the return address (where the code was interrupted) */
	SUB LR,LR,#8
	/* Push the register to the user-mode stack */
	PUSH {LR}

	/* Save the stackpointer for the exception handling debug output */
	MOV R0,SP

	/* Just branch to C undefined instruction handler and stop execution */
	B exception_undefined_instruction_c;

exception_supervisor_call:
	/* Save general registers */
	STMFD sp!,{r0-r12}

	/* The LR of the usr/sys mode and the SPSR still needs saving */
	/* Change to system mode to get the LR and SP */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)

	MOV R0,LR
	MOV R1,SP

	MSR CPSR_c,#(CPU_MODE_SUPERVISOR | CPSR_I_BIT | CPSR_F_BIT)

	/* Save old linkregister */
	PUSH {R0}

	/* Save old stackpointer */
	PUSH {R1}

	/*  Save the return address (where the code was interrupted) */
	SUB LR,LR,#8
	/* Push the register to the user-mode stack */
	PUSH {LR}

	/* Save the stackpointer for the exception handling debug output */
	MOV R0,SP

	/* Just branch to C supervisor call handler and stop execution */
	B exception_supervisor_call_c;

exception_prefetch_abort:
	/* Save general registers */
	STMFD sp!,{r0-r12}

	/* The LR of the usr/sys mode and the SPSR still needs saving */
	/* Change to system mode to get the LR and SP */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)

	MOV R0,LR
	MOV R1,SP

	MSR CPSR_c,#(CPU_MODE_ABORT | CPSR_I_BIT | CPSR_F_BIT)

	/* Save old linkregister */
	PUSH {R0}

	/* Save old stackpointer */
	PUSH {R1}

	/*  Save the return address (where the code was interrupted) */
	SUB LR,LR,#8
	/* Push the register to the user-mode stack */
	PUSH {LR}

	/* Save the stackpointer for the exception handling debug output */
	MOV R0,SP

	/* Just branch to C prefetch abort handler and stop execution */
	B exception_prefetch_abort_c;

exception_data_abort:
	/* Save general registers */
	STMFD sp!,{r0-r12}

	/* The LR of the usr/sys mode and the SPSR still needs saving */
	/* Change to system mode to get the LR and SP */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)

	MOV R0,LR
	MOV R1,SP

	MSR CPSR_c,#(CPU_MODE_ABORT | CPSR_I_BIT | CPSR_F_BIT)

	/* Save old linkregister */
	PUSH {R0}

	/* Save old stackpointer */
	PUSH {R1}

	/*  Save the return address (where the code was interrupted) */
	SUB LR,LR,#8
	/* Push the register to the user-mode stack */
	PUSH {LR}

	/* Save the stackpointer for the exception handling debug output */
	MOV R0,SP


	/* Just branch to C hypervisor call handler and stop execution */
	B exception_data_abort_c;

exception_hypervisor:
	/* Save general registers */
	STMFD sp!,{r0-r12}

	/* The LR of the usr/sys mode and the SPSR still needs saving */
	/* Change to system mode to get the LR and SP */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)

	MOV R0,LR
	MOV R1,SP

	MSR CPSR_c,#(CPU_MODE_HYP | CPSR_I_BIT | CPSR_F_BIT)

	/* Save old linkregister */
	PUSH {R0}

	/* Save old stackpointer */
	PUSH {R1}

	/*  Save the return address (where the code was interrupted) */
	SUB LR,LR,#8
	/* Push the register to the user-mode stack */
	PUSH {LR}

	/* Save the stackpointer for the exception handling debug output */
	MOV R0,SP


	/* Just branch to C hypervisor call handler and stop execution */
	B exception_hypervisor_c;


exception_fiq_interrupt:
