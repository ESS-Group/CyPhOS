#include <eventhandling/StackPool.h>

#include <common/debug.h>
#include <eventhandling/MultiQueuePriorityStrategy.h>

#define TAG "PriorityStrategy"

//#define MULTI_FIFO_DEBUG

MultiQueuePriorityStrategy::MultiQueuePriorityStrategy() {}

void MultiQueuePriorityStrategy::enqueueEvent(EventHandling::EventTask *eventtask) {
	if ( nullptr == eventtask ) {
		return;
	}

	cpu_t cpuNo = eventtask->trigger->pAffinity;
	if ( cpuNo >= NR_CPUS ) {
		cpuNo = NR_CPUS;
	}

	eventtask->pEnqueued = true;



	mQueueLock[cpuNo].lock();
	mFifoQueue[cpuNo].enqueue(eventtask);
#ifdef MULTI_FIFO_DEBUG
		DEBUG_STREAM(TAG,"Eventtask: " << hex << eventtask << "enqueued for queue: " << dec << (uint16_t)cpuNo);
		DEBUG_STREAM(TAG,"Queue size: " << dec << (uint32_t)mFifoQueue[cpuNo].size());
#endif
	mQueueLock[cpuNo].unlock();
}

void MultiQueuePriorityStrategy::enqueueInterruptEvent(EventHandling::EventTask *event) {
	enqueueEvent(event);
}




/**
 * This actually defines how the strategy schedules its events
 */
EventHandling::EventTask *MultiQueuePriorityStrategy::getReadyEvent(cpu_t cpu) {
	EventHandling::EventTask *eventtask = nullptr;

	/* Check for normal software events */
	mQueueLock[cpu].lock();
	eventtask = (EventHandling::EventTask*)mFifoQueue[cpu].dequeueHighestPriority();
	if ( nullptr != eventtask ) {
		mQueueLock[cpu].unlock();
#ifdef MULTI_FIFO_DEBUG
		DEBUG_STREAM(TAG,"eventtask: " << hex << eventtask << " for OSC: " << eventtask->trigger->pDestinationOSC);
#endif
		return eventtask;
	} else {
		mQueueLock[cpu].unlock();
//		DEBUG_STREAM(TAG,"No regular event");
	}


#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"NO EVENTS, queue size: " << dec << (uint32_t)mFifoQueue[cpu].size());
#endif

	return eventtask;
}

EventHandling::EventTask *MultiQueuePriorityStrategy::getGlobalReadyEvent() {
	EventHandling::EventTask *eventtask;

	/* Check for normal software events */
	mQueueLock[NR_CPUS].lock();
	eventtask = (EventHandling::EventTask*)mFifoQueue[NR_CPUS].dequeueHighestPriority();
	if ( nullptr != eventtask ) {
		mQueueLock[NR_CPUS].unlock();
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"Global event: " << hex << eventtask);
#endif
		return eventtask;
	} else {
		mQueueLock[NR_CPUS].unlock();
	}

#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"Global event: " << hex << eventtask << " queue size:" << dec << (uint32_t)mFifoQueue[NR_CPUS].size());
#endif
	return eventtask;
}

EventHandling::EventTask *MultiQueuePriorityStrategy::getEvent(cpu_t cpu) {
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

uint32_t MultiQueuePriorityStrategy::getPendingEvents() {
	uint32_t retVal = 0;
	for ( cpu_t cpu = 0; cpu <= NR_CPUS; cpu++ ) {
		retVal += (mFifoQueue[cpu].size());
	}
	return retVal;
}

