/*
 * InterruptDispatcher.h
 *
 *  Created on: 23.07.2015
 *      Author: hendrik
 */

#ifndef INTERRUPTS_INTERRUPTDISPATCHER_H_
#define INTERRUPTS_INTERRUPTDISPATCHER_H_

#ifndef NR_INTERRUPTS
#define NR_INTERRUPTS 0
#error "No interrupt count defined"
#endif


#include <component/InterruptOSC.h>

#include <eventhandling/EventTask.h>
#include <eventhandling/Trigger.h>
#include <eventhandling/Event.h>

class InterruptDispatcher {

public:
	InterruptDispatcher();

	/**
	 * Dispatch interrupts through the event system
	 */
	void dispatchInterrupt(uint32_t nr);


	/**
	 * Register HW/SW events within the interrupt dispatcher
	 */
	void registerEvent(uint32_t nr, EventHandling::Trigger *trigger);


	/* Static instance variable */
	static InterruptDispatcher pInstance;
private:
	EventHandling::Event pInterrupts[NR_INTERRUPTS];


};

#endif /* INTERRUPTS_INTERRUPTDISPATCHER_H_ */
