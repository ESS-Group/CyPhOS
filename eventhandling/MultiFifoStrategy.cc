/*
 * FifoStrategy.cc
 *
 *  Created on: 01.08.2016
 *      Author: hendrik
 */

#include <eventhandling/MultiFifoStrategy.h>

#include <eventhandling/StackPool.h>

#include <common/debug.h>

#define TAG "MultiFIFOStrategy"

//#define MULTI_FIFO_DEBUG

void MultiFifoStrategy::enqueueEvent(EventHandling::EventTask *eventtask) {
	if ( nullptr == eventtask ) {
		return;
	}

	cpu_t cpuNo = eventtask->trigger->pAffinity;
	if ( cpuNo >= NR_CPUS ) {
		cpuNo = NR_CPUS;
	}

	eventtask->pEnqueued = true;

	if ( eventtask->event->pType == EventHandling::Event::INTERRUPT_EVENT ) {
			mInterruptQueueLock[cpuNo].lock();
			mFifoInterruptQueue[cpuNo].enqueue(eventtask);
			mInterruptQueueLock[cpuNo].unlock();
			return;
	}


	// Not an interrupt event, enqueue normally
	mQueueLock[cpuNo].lock();
	mFifoQueue[cpuNo].enqueue(eventtask);
	mQueueLock[cpuNo].unlock();
}

void MultiFifoStrategy::enqueueInterruptEvent(EventHandling::EventTask *event) {
	enqueueEvent(event);
}




/**
 * This actually defines how the strategy schedules its events
 */
EventHandling::EventTask *MultiFifoStrategy::getReadyEvent(cpu_t cpu) {
#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getReadyEvent");
#endif
	EventHandling::EventTask *event = nullptr;

	/* Check interrupt queue first */
	mInterruptQueueLock[cpu].lock();
	event = (EventHandling::EventTask*)mFifoInterruptQueue[cpu].dequeueFirstLocked();
	if ( nullptr != event ) {
		mInterruptQueueLock[cpu].unlock();
		return event;
	} else {
		mInterruptQueueLock[cpu].unlock();
	}

#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getReadyEvent no ISRs");
#endif

	/* Check for normal software events */
	mQueueLock[cpu].lock();
	event = (EventHandling::EventTask*)mFifoQueue[cpu].dequeueFirstLocked();
	if ( nullptr != event ) {
		mQueueLock[cpu].unlock();
//		DEBUG_STREAM(TAG,"regular event: " << hex << event << " for OSC: " << event->pDestinationOSC);
		return event;
	} else {
		mQueueLock[cpu].unlock();
//		DEBUG_STREAM(TAG,"No regular event");
	}


#ifdef MULTI_FIFO_DEBUG
	DEBUG_STREAM(TAG,"getReadyEvent no preempted events");
#endif




	return event;
}

EventHandling::EventTask *MultiFifoStrategy::getGlobalReadyEvent() {
	EventHandling::EventTask *eventtask;

	/* Check interrupt queue first */
	mInterruptQueueLock[NR_CPUS].lock();
	eventtask = (EventHandling::EventTask*)mFifoInterruptQueue[NR_CPUS].dequeueFirstLocked();
	if ( nullptr != eventtask ) {
		mInterruptQueueLock[NR_CPUS].unlock();
		return eventtask;
	} else {
		mInterruptQueueLock[NR_CPUS].unlock();
	}

	/* Check for normal software events */
	mQueueLock[NR_CPUS].lock();
	eventtask = (EventHandling::EventTask*)mFifoQueue[NR_CPUS].dequeueFirstLocked();
	if ( nullptr != eventtask ) {
		mQueueLock[NR_CPUS].unlock();
		return eventtask;
	} else {
		mQueueLock[NR_CPUS].unlock();
	}


	return eventtask;
}

EventHandling::EventTask *MultiFifoStrategy::getEvent(cpu_t cpu) {
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

uint32_t MultiFifoStrategy::getPendingEvents() {
	uint32_t retVal = 0;
	for ( cpu_t cpu = 0; cpu <= NR_CPUS; cpu++ ) {
		retVal += (mFifoQueue[cpu].size() + mFifoInterruptQueue[cpu].size());
	}
	return retVal;
}

