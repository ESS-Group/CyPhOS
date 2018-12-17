/*
 * FifoStrategy.h
 *
 *  Created on: 01.08.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_FIFOSTRATEGY_H_
#define EVENTHANDLING_FIFOSTRATEGY_H_

#include <common/Queue.h>
#include <eventhandling/EventTask.h>

#include <eventhandling/EventQueue.h>

#include <sync/Spinlock.h>

#include "SchedulingStrategy.h"

/**
 * Multi FiFo scheduling
 */
class MultiFifoStrategy : public SchedulingStrategy {
private:
	/**
	 * Queue for normal events between normal software components.
	 */
	EventQueue mFifoQueue[NR_CPUS + 1];

	/**
	 * Queue for events that handle interrupt requests. These are handled before
	 * handling other events.
	 */
	EventQueue mFifoInterruptQueue[NR_CPUS + 1];

	/*
	 * Lock for the queue mFifoQueue
	 */
	Spinlock mQueueLock[NR_CPUS + 1];

	/**
	 * Lock for the queue mFifoInterruptQueue
	 */
	Spinlock mInterruptQueueLock[NR_CPUS + 1];

	/**
	 * Returns a event that is ready for execution specific for one cpu.
	 */
	EventHandling::EventTask *getReadyEvent(cpu_t cpu);

	/**
	 * Returns a event that is ready for execution specific for all cpus.
	 */
	EventHandling::EventTask *getGlobalReadyEvent();

public:
	static MultiFifoStrategy pInstance;
	void enqueueEvent(EventHandling::EventTask *event);

	void enqueueInterruptEvent(EventHandling::EventTask *event);

	EventHandling::EventTask *getEvent(cpu_t cpu);

	uint32_t getPendingEvents();

};

#endif /* EVENTHANDLING_FIFOSTRATEGY_H_ */
