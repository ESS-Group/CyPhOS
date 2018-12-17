/*
 * InterruptDispatcher.cc
 *
 *  Created on: 23.07.2015
 *      Author: hendrik
 */

#include <interrupts/InterruptDispatcher.h>

/* Debugging */
#include <common/debug.h>
#define TAG "INTERRUPT_DISPATCH"

#include <eventhandling/EventHandler.h>

/* Static instance */
InterruptDispatcher InterruptDispatcher::pInstance;

InterruptDispatcher::InterruptDispatcher() {
}

void InterruptDispatcher::dispatchInterrupt(uint32_t nr) {
	EventHandling::Event *intEvent = &pInterrupts[nr];
	/* Check if an interrupt is defined for this interrupt number */
	if (intEvent->mSubscribers.size() == 0) {
		DEBUG_STREAM(TAG, "No interrupt event defined for: " << hex << nr);
		return;
	}
//	DEBUG_STREAM(TAG,"Dispatch event:" << hex << (unsigned int)intEvent);
	EventHandling::EventHandler::pInstance.interruptEventTriggered(intEvent, 0);

//	DEBUG_STREAM(TAG,"Dispatch finished");
}

void InterruptDispatcher::registerEvent(uint32_t nr, EventHandling::Trigger *trigger) {
	/* Check if a null event is given */
	if (trigger == nullptr || (nr > NR_INTERRUPTS)) {
		return;
	}
//	DEBUG_STREAM(TAG, "Registering event for INT: " << nr << " with trigger: " << hex << trigger);
	pInterrupts[nr].addSubscriber(trigger);
	pInterrupts[nr].pType = EventHandling::Event::INTERRUPT_EVENT;
}

