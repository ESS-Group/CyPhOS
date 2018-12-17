/*
 * FifoStrategy.h
 *
 *  Created on: 01.08.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_FIFOSTRATEGY_H_
#define EVENTHANDLING_FIFOSTRATEGY_H_

#include <common/Queue.h>

#include <eventhandling/EventQueue.h>

#include <eventhandling/EventTask.h>

#include <sync/Spinlock.h>

#include "SchedulingStrategy.h"

/**
 * Global FiFo scheduling
 */
class FifoStrategy : public SchedulingStrategy {
private:
	/**
	 * Queue for normal events between normal software components.
	 */
	EventQueue mFifoQueue;

	/**
	 * Queue for events that handle interrupt requests. These are handled before
	 * handling other events.
	 */
	EventQueue mFifoInterruptQueue;

	/*
	 * Lock for the queue mFifoQueue
	 */
	Spinlock mQueueLock;

	/**
	 * Lock for the queue mFifoInterruptQueue
	 */
	Spinlock mInterruptQueueLock;


	/**
	 * Returns a event that is ready for execution.
	 */
	EventHandling::EventTask *getReadyEvent(cpu_t cpu);

	bool lockEventOSC(EventHandling::EventTask *event);

public:
	static FifoStrategy pInstance;
	void enqueueEvent(EventHandling::EventTask *event);

	void enqueueInterruptEvent(EventHandling::EventTask *event);

	EventHandling::EventTask *getEvent(cpu_t cpu);

	uint32_t getPendingEvents();

};

#endif /* EVENTHANDLING_FIFOSTRATEGY_H_ */
