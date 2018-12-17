.section .eventHandling,"ax"

.global activateDispatching
.global dispatchOSCMemberEvent_asm
.global resumeOSC_asm
.global finishEvent_asm
.global saveOSCContext_asm


// This function should provide a safe entry for cores to start event dispatching.
// It should automatically set the stackpointer to the os main stacks with an offset
// for the core number.
// activateDispatching()

activateDispatching:
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

// Stack is prepared, now we can jump back to the c-code
	jmp startDispatching


// Dispatches an event by swapping its context
// Changes the stackpointer to the OSC stack and sets the return address to the
// address of the activateDispatching function, so that the dispatcher
// is called again after the event execution.
// dispatchOSCMemberEvent_asm(void *stackPointer, EventTask *eventcon, void *arg);

dispatchOSCMemberEvent_asm:
// Change the stackpointer to the OSC stack
	mov %rdi, %rsp
// Save the second Parameter (eventcon) in RDI (first parameter)
	mov %rsi, %rdi
// Save the third Parameter (Args) in RSI (second parameter)
	mov %rdx, %rsi


// Change the return address to the activateDispatching function.
// This guarantees that the dispatcher gets activated again after the event.
	push $finishEvent_asm

// Branch to the actual trigger address
	jmp contextSwitchOSC

finishEvent_asm:
// FIXME: Use SVC later when usermode components are implemented
// Save the stackpointer of the OSC
	mov %rsp, %rdi
	call finishEventHandler
// no need to cleanup the stack, because finishEventHandler will not return and starts DispatcherLoop

