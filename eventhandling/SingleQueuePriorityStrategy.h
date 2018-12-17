#ifndef EVENTHANDLING_SINGLEQUEUEPRIORITYSTRATEGY_H_
#define EVENTHANDLING_SINGLEQUEUEPRIORITYSTRATEGY_H_

#include <common/Queue.h>
#include <eventhandling/EventTask.h>

#include <eventhandling/EventPriorityQueue.h>

#include <sync/Spinlock.h>

#include "SchedulingStrategy.h"

/**
 * Single FiFo priority scheduling
 */
class SingleQueuePriorityStrategy : public SchedulingStrategy {
private:
	/**
	 * Queue for normal events between normal software components.
	 */
	EventPriorityQueue mFifoQueue;


	/*
	 * Lock for the queue mFifoQueue
	 */
	Spinlock mQueueLock;


	/**
	 * Returns a event that is ready for execution specific for one cpu.
	 */
	EventHandling::EventTask *getReadyEvent(cpu_t cpu);

	/**
	 * Returns a event that is ready for execution specific for all cpus.
	 */
	EventHandling::EventTask *getGlobalReadyEvent();

public:
	static SingleQueuePriorityStrategy pInstance;

	SingleQueuePriorityStrategy();

	void enqueueEvent(EventHandling::EventTask *event);

	void enqueueInterruptEvent(EventHandling::EventTask *event);

	EventHandling::EventTask *getEvent(cpu_t cpu);

	uint32_t getPendingEvents();

};

#endif /* EVENTHANDLING_SINGLEQUEUEPRIORITYSTRATEGY_H_ */
