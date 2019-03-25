/*
 * first_stage_handler.cc
 *
 *  Created on: 10.04.2015
 *      Author: hendrik
 */

/* Debugging */
#include <common/armhelper.h>
#include <common/baremetal.h>
#include <common/debug.h>

#include <eventhandling/EventHandler.h>
#define TAG "EXCEPTION"

volatile bool exceptionActive = false;

/**
 * ARMv7 exception handling
 * see ARM architecture reference p. B1-1167 for details
 */

//INCLUDE_DECLARATION_IN_LOCKED_OSC(InterruptOSC,dword_t,vector_table[8]);
/*
 * Export exception handlers without c++ name mangling
 */
extern "C" {
void exception_reset_c(void* stackpointer);
void exception_undefined_instruction_c(void* stackpointer);
void exception_supervisor_call_c(void* stackpointer);
void exception_prefetch_abort_c(void* stackpointer);
void exception_data_abort_c(void *stackpointer);
void exception_hypervisor_c(void* stackpointer);
void exception_fiq_interrupt_c();

void restart_system();
}

void printCrashContext(dword_t *sp) {
	exceptionActive = true;
	DEBUG_STREAM(TAG,"PC: " << hex << *sp);
	DEBUG_STREAM(TAG,"SP: " << hex << *(sp+1));
	DEBUG_STREAM(TAG,"LR: " << hex << *(sp+2));

	DEBUG_STREAM(TAG,"R0: " << hex << *(sp+3));
	DEBUG_STREAM(TAG,"R1: " << hex << *(sp+4));
	DEBUG_STREAM(TAG,"R2: " << hex << *(sp+5));
	DEBUG_STREAM(TAG,"R3: " << hex << *(sp+6));
	DEBUG_STREAM(TAG,"R4: " << hex << *(sp+7));
	DEBUG_STREAM(TAG,"R5: " << hex << *(sp+8));
	DEBUG_STREAM(TAG,"R6: " << hex << *(sp+9));
	DEBUG_STREAM(TAG,"R7: " << hex << *(sp+10));
	DEBUG_STREAM(TAG,"R8: " << hex << *(sp+11));
	DEBUG_STREAM(TAG,"R9: " << hex << *(sp+12));
	DEBUG_STREAM(TAG,"R10: " << hex << *(sp+13));
	DEBUG_STREAM(TAG,"R11 " << hex << *(sp+14));
	DEBUG_STREAM(TAG,"R12: " << hex << *(sp+15));

	EventHandling::EventHandler::pInstance.printCurrentEventTask();


	DEBUG_STREAM(TAG,"REBOOTING SYSTEM! :'-(");
	BUSY_WAITING_LONG
	;
}

/**
 * Exception handler for the reset exception
 */
void exception_reset_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "Reset exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the undefined instruction exception
 */
void exception_undefined_instruction_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "undefined instruction exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the supervisor exception
 */
void exception_supervisor_call_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "supervisor call exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the prefetch abort exception
 */
void exception_prefetch_abort_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "prefetch abort exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the data abort exception
 * It will check first, if it was just caused by a access flag fault and can be fixed
 */
void exception_data_abort_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "data abort exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the hypervisor call exception
 */
void exception_hypervisor_c(void* stackpointer) {
	DEBUG_STREAM(TAG, "Hypervisor exception");
	if (!exceptionActive) {
		printCrashContext((dword_t*) stackpointer);
	}
	restart_system();
}

/**
 * Exception handler for the fast interrupt exception
 */
void exception_fiq_interrupt_c() {
	DEBUG_STREAM(TAG, "FIQ exception");

	restart_system();
}
