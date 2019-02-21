.section .eventHandling,"ax"
.code 32

#include <common/ARM_common.h>

/* Export symbols globally */
.global activateDispatching
.global dispatchOSCMemberEvent_asm
.global finishEvent_asm
.global saveOSCContext_asm


/**
* This function should provide a safe entry for cores to start event dispatching.
* It should automatically set the stackpointer to the os main stacks with an offset
* for the core number.
* activateDispatching()
*/

activateDispatching:
/* Switch back to system-mode just in case */
	MSR CPSR_c,#(CPU_MODE_SYS | CPSR_I_BIT | CPSR_F_BIT)
/* Set the stack pointer to the start of the OS stack */
// Read the core id
	MRC P15,0,R0,c0,c0,5
// AND with the core id mask
	AND R0,R0,#0x3
// Multiply constant
// FIXME define this offset somewhere
	MOV R1,#512
// Calculate the offset
	MUL R2,R1,R0
// Load the stack start address
	LDR R0,=__runtime_stack_start
// Subtract the actual offset value
	SUB R0,R0,R2
// Set the stackpointer
	MOV sp,R0

/* Stack is prepared, now we can jump back to the c-code */
	B startDispatching

/**
* Dispatches an event by swapping its context
* Changes the stackpointer to the OSC stack and sets the LR to the
* address of the activateDispatching-function, so that the dispatcher
* is called again after the event execution.
* dispatchOSCMemberEvent_asm(void *stackPointer, EventConnection *eventcon, void *arg);
*/
dispatchOSCMemberEvent_asm:
	/* Change the stackpointer to the OSC stack */
	MOV sp,R0
	// Move eventcon to R0 for function call
	MOV R0,R1
	// Load the arg to R1 for function call
	MOV R1,R2
	/* Change the linkregister (return address) to the activateDispatching function.
	 * This guarantees that the dispatcher gets activated again after the event.
	 */
	LDR lr,=finishEvent_asm
	/* Branch to the actual trigger address */
	B contextSwitchOSC

/** This function should call the c++-handler for finished events and pass the stackpointer as an
*   argument to the function
*/
finishEvent_asm:
	/* FIXME: Use SVC later when usermode components are implemented */
	/* Disable interrupts now, because it makes no sense anymore to preempt the OSC */
	/* Save the stackpointer of the OSC */
	MOV R0,SP
	B finishEventHandler

