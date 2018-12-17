/*

 * SchedulingStrategy.h
 *
 *  Created on: 01.08.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_SCHEDULINGSTRATEGY_H_
#define EVENTHANDLING_SCHEDULINGSTRATEGY_H_

#include <eventhandling/EventTask.h>

/**
 * Abstract class to define scheduling strategies by subclassing it.
 */
class SchedulingStrategy {


public:
	/**
	 * Enqueues an event in the queues of the strategy.
	 * This function is called from the software context and needs to be
	 * protected against interrupts. To prevent deadlocks this function
	 * should not use locks that are also used by the enqueueInterruptEvent
	 * function.
	 */
	virtual void enqueueEvent(EventHandling::EventTask *event) = 0;

	/**
	 * This function is called from the interrupt context.
	 * This means, that the implementation of this function only needs
	 * synchronization againt other processor cores.
	 */
	virtual void enqueueInterruptEvent(EventHandling::EventTask *event) = 0;

	/**
	 * Returns the event ready for execution on the cpu.
	 * The strategy needs to ensure, that the EventConnection that
	 * is returned has a stack associated. So use the StackPool.
	 */
	virtual EventHandling::EventTask *getEvent(cpu_t cpu) = 0;



	/**
	 * Returns the number of events that are ready to be executed.
	 */
	virtual uint32_t getPendingEvents() = 0;

};


#endif /* EVENTHANDLING_SCHEDULINGSTRATEGY_H_ */
