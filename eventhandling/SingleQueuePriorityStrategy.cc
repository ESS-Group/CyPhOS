#include <eventhandling/StackPool.h>

#include <common/debug.h>
#include <eventhandling/SingleQueuePriorityStrategy.h>

#define TAG "PriorityStrategy"

//#define MULTI_FIFO_DEBUG

SingleQueuePriorityStrategy::SingleQueuePriorityStrategy() {}

void SingleQueuePriorityStrategy::enqueueEvent(EventHandling::EventTask *eventtask) {
	if ( nullptr == eventtask ) {
		return;
	}

	eventtask->pEnqueued = true;

	mQueueLock.lock();
	mFifoQueue.enqueue(eventtask);
	mQueueLock.unlock();
}

void SingleQueuePriorityStrategy::enqueueInterruptEvent(EventHandling::EventTask *event) {
	enqueueEvent(event);
}


/**
 * This actually defines how the strategy schedules its events
 */
EventHandling::EventTask *SingleQueuePriorityStrategy::getReadyEvent(cpu_t cpu) {
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getReadyEvent");
#endif
	EventHandling::EventTask *eventtask = nullptr;

	/* Check for normal software events */
	mQueueLock.lock();
	eventtask = (EventHandling::EventTask*)mFifoQueue.dequeueHighestPriority();
	if ( nullptr != eventtask ) {
		mQueueLock.unlock();
//		DEBUG_STREAM(TAG,"regular event: " << hex << event << " for OSC: " << event->pDestinationOSC);
		return eventtask;
	} else {
		mQueueLock.unlock();
//		DEBUG_STREAM(TAG,"No regular event");
	}


#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getReadyEvent no preempted events");
#endif

	return eventtask;
}

EventHandling::EventTask *SingleQueuePriorityStrategy::getGlobalReadyEvent() {
	EventHandling::EventTask *eventtask;


	/* Check for normal software events */
	mQueueLock.lock();
	eventtask = (EventHandling::EventTask*)mFifoQueue.dequeueHighestPriority();
	if ( nullptr != eventtask ) {
		mQueueLock.unlock();
		return eventtask;
	} else {
		mQueueLock.unlock();
	}

	return eventtask;
}

EventHandling::EventTask *SingleQueuePriorityStrategy::getEvent(cpu_t cpu) {
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getEvent");
#endif
	EventHandling::EventTask *eventtask = getReadyEvent(cpu);

	if ( nullptr == eventtask ) {
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getEvent no local event found");
#endif

	eventtask = getGlobalReadyEvent();
		if ( nullptr == eventtask ) {
			return nullptr;
		}
	}
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getEvent no event found");
#endif

	if ( nullptr == eventtask->pStackpointer ) {
		eventtask->pStackpointer = StackPool::pInstance.getFreeStack();
	}

	if ( nullptr == eventtask->pStackpointer) {
		// Requeue the event
		enqueueEvent(eventtask);
		return nullptr;
	}

	eventtask->pEnqueued = false;
	return eventtask;
}

uint32_t SingleQueuePriorityStrategy::getPendingEvents() {
	uint32_t retVal = 0;

	retVal += (mFifoQueue.size());

	return retVal;
}

