/*
 * interrupt_handler.cc
 *
 *  Created on: 05.06.2018
 *      Author: hendrik
 */

#include <common/types.h>

#include <common/debug.h>

#include <interrupts/InterruptDispatcher.h>

#include <eventhandling/EventHandler.h>

#include <driver/LocalAPIC.h>

#define TAG "IRQHandler"

extern "C" {
	void exception_irq_interrupt_c(uint64_t irqNo, uintptr_t stackpointer);
	void restart_system();
}

/**
 * Global variable to indicate the active handling of an exception. Prevents
 * sync output stream from writing into error debug output.
 */
volatile bool exceptionActive = false;

void printStackError(uintptr_t stackpointer, bool errorCode) {
	if(errorCode) {
		DEBUG_STREAM(TAG,"SS: " << hex << *(((uint64_t*)stackpointer) + 20));
		DEBUG_STREAM(TAG,"RSP: " << hex << *(((uint64_t*)stackpointer) + 19));
		DEBUG_STREAM(TAG,"RFLAGS: " << hex << *(((uint64_t*)stackpointer) + 18));
		DEBUG_STREAM(TAG,"RCS: " << hex << *(((uint64_t*)stackpointer) + 17));
		DEBUG_STREAM(TAG,"RIP: " << hex << *(((uint64_t*)stackpointer) + 16));
		DEBUG_STREAM(TAG,"ErrorCode: " << hex << *(uint32_t*)(((uint64_t*)stackpointer) + 15));
		DEBUG_STREAM(TAG,"Faulting index if selector fault: " << dec << ((*(uint32_t*)(((uint64_t*)stackpointer) + 15))>>3));
	} else {
		DEBUG_STREAM(TAG,"SS: " << hex << *(((uint64_t*)stackpointer) + 19));
		DEBUG_STREAM(TAG,"RSP: " << hex << *(((uint64_t*)stackpointer) + 18));
		DEBUG_STREAM(TAG,"RFLAGS: " << hex << *(((uint64_t*)stackpointer) + 17));
		DEBUG_STREAM(TAG,"RCS: " << hex << *(((uint64_t*)stackpointer) + 16));
		DEBUG_STREAM(TAG,"RIP: " << hex << *(((uint64_t*)stackpointer) + 15));
	}

	DEBUG_STREAM(TAG,"RDI: " << hex << *(((uint64_t*)stackpointer) + 14));
	DEBUG_STREAM(TAG,"RAX: " << hex << *(((uint64_t*)stackpointer) + 13));
	DEBUG_STREAM(TAG,"RBX: " << hex << *(((uint64_t*)stackpointer) + 12));
	DEBUG_STREAM(TAG,"RCX: " << hex << *(((uint64_t*)stackpointer) + 11));
	DEBUG_STREAM(TAG,"RDX: " << hex << *(((uint64_t*)stackpointer) + 10));
	DEBUG_STREAM(TAG,"RBP: " << hex << *(((uint64_t*)stackpointer) + 9));
	DEBUG_STREAM(TAG,"RSI: " << hex << *(((uint64_t*)stackpointer) + 8));
	DEBUG_STREAM(TAG,"R8: " << hex << *(((uint64_t*)stackpointer) + 7));
	DEBUG_STREAM(TAG,"R9: " << hex << *(((uint64_t*)stackpointer) + 6));
	DEBUG_STREAM(TAG,"R10: " << hex << *(((uint64_t*)stackpointer) + 5));
	DEBUG_STREAM(TAG,"R11: " << hex << *(((uint64_t*)stackpointer) + 4));
	DEBUG_STREAM(TAG,"R12: " << hex << *(((uint64_t*)stackpointer) + 3));
	DEBUG_STREAM(TAG,"R13: " << hex << *(((uint64_t*)stackpointer) + 2));
	DEBUG_STREAM(TAG,"R14: " << hex << *(((uint64_t*)stackpointer) + 1));
	DEBUG_STREAM(TAG,"R15: " << hex << *(((uint64_t*)stackpointer) + 0));
}

void exception_irq_interrupt_c(uint64_t irqNo, uintptr_t stackpointer) {
	if (irqNo < 32) {
		exceptionActive = true;
		DEBUG_STREAM(TAG,"TRAP: " << hex << irqNo);
		printStackError(stackpointer, irqNo >= 8 ? true : false);
		EventHandling::EventHandler::pInstance.printCurrentEventTask();
		// Reboot the system
		restart_system();
	}


	// Call the dispatcher
	InterruptDispatcher::pInstance.dispatchInterrupt((uint32_t)irqNo);

	// Signal the end of interrupt to the LocalAPIC
	LocalAPIC::mInstance.sendEOI();
	exceptionActive = false;
}


