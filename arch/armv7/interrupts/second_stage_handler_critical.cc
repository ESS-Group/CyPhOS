/*
 * second_stage_handler_critical.cc
 *
 *  Created on: 30.01.2017
 *      Author: hendrik
 */

/* Debugging */
#include <common/armhelper.h>
#include <common/baremetal.h>
#include <common/debug.h>
#define TAG "EXCEPTION"


/* Include the SystemMode class to check if in system mode during preemption */
#include <core/SystemMode.h>

/* Include the interrupt controller (needed for intNo, acknowledgment ) */
#include <driver/ARMGIC.h>

/* Include the interrupt event dispatcher */
#include <interrupts/InterruptDispatcher.h>

/* Include the event management system for context saving */
#include <eventhandling/EventHandler.h>

/* FIXME ARCH CODE, Register count * word width (in bytes) */
#define CONTEXT_SAVE_SIZE 16 * 4

SECTION_CRITICAL_DATA static Spinlock isrLock;

extern "C" {
int exception_irq_interrupt_c();
}

/**
 * Exception handler for the normal interrupt exception.
 * Parameter is the old stackpointer of the running OSC (if any).
 * This stackpointer needs to be saved for the OSC otherwise a resume of
 * an preempted eventhandler is not possible.
 * Returns true if an OSC was preempted.
 * false if the system was interrupted
 */
int exception_irq_interrupt_c() {
	dword_t gicIAR = 0;
	dword_t intNo = 0;

	/*
	 * Short lock needed around the reading of the Int-No.
	 * S. page 3-41 of ARM GIC reference.
	 */
	isrLock.lock();
	gicIAR = OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.getNextInterrupt();
	isrLock.unlock();

	/* Filter only the IRQ-no */
	intNo = gicIAR & SPURIOUS_INT;

	/* Check if this interrupt is spurious. This
	 * could mean that it is handled by another cpu core. Just acknowledge and then return
	 */
	if (intNo == 1023 || intNo == 1022) {
		/* Do not preempt here because it was a sporadic int */
		return 0;
	}

	EventHandling::EventTask *event = EventHandling::EventHandler::pInstance.getSelf();

#ifdef CONFIG_DEBUG
	// Check if no event handler is running. Non-preemptive scheduling prohibits interruption during task execution!
	if (event != nullptr ) {
		DEBUG_STREAM(TAG,"Interrupt during EventTask execution. SHOULD NOT HAPPEN!");
	}
#endif

	do {

		/* Check what type the interrupt is */
		if (intNo < NR_INTERRUPTS) {
#ifdef EXCEPTION_DEBUGGING
				DEBUG_STREAM(TAG,"Dispatch interrupt: " << intNo);
#endif
			/*
			 * Handle interrupt with the specific number here. It is disabled until handled.
			 * Corresponding OSC needs to reactivate the interrupt after acknowledging it.
			 */
			OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.disableInterrupt(intNo);
			InterruptDispatcher::pInstance.dispatchInterrupt(intNo);
		}

		/* Acknowledge the interrupt to the controller */
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.endOfInterrupt(gicIAR);

		isrLock.lock();
		gicIAR = OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.getNextInterrupt();
		isrLock.unlock();

		/* Filter only the IRQ-no */
		intNo = gicIAR & SPURIOUS_INT;

	} while (intNo != 1023 && intNo != 1022);
//	}
#ifdef EXCEPTION_DEBUGGING
	DEBUG_STREAM(TAG,"Exception IRQ end: " << dec << ((uint32_t)(EventHandling::EventHandler::pInstance.inSYSMode()) ? 0 : 1));
#endif

	/* Check if interrupted during system call then it should finish otherwise the event dispatcher is called again */
	return 0;
}
