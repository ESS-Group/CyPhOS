#ifndef EVENTHANDLING_MULTIQUEUEPRIORITYSTRATEGY_H_
#define EVENTHANDLING_MULTIQUEUEPRIORITYSTRATEGY_H_

#include <common/Queue.h>
#include <eventhandling/EventTask.h>

#include <eventhandling/EventPriorityQueue.h>

#include <sync/Spinlock.h>

#include "SchedulingStrategy.h"

/**
 * Multi FiFo priority scheduling
 */
class MultiQueuePriorityStrategy : public SchedulingStrategy {
private:
	/**
	 * Queue for normal events between normal software components.
	 */
	EventPriorityQueue mFifoQueue[NR_CPUS + 1];

	/*
	 * Lock for the queue mFifoQueue
	 */
	Spinlock mQueueLock[NR_CPUS + 1];

	/**
	 * Returns a event that is ready for execution specific for one cpu.
	 */
	EventHandling::EventTask *getReadyEvent(cpu_t cpu);

	/**
	 * Returns a event that is ready for execution specific for all cpus.
	 */
	EventHandling::EventTask *getGlobalReadyEvent();

public:
	static MultiQueuePriorityStrategy pInstance;

	MultiQueuePriorityStrategy();

	void enqueueEvent(EventHandling::EventTask *event);

	void enqueueInterruptEvent(EventHandling::EventTask *event);

	EventHandling::EventTask *getEvent(cpu_t cpu);

	uint32_t getPendingEvents();

};

#endif /* EVENTHANDLING_MULTIQUEUEPRIORITYSTRATEGY_H_ */
