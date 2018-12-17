/*
 * FifoStrategy.cc
 *
 *  Created on: 01.08.2016
 *      Author: hendrik
 */

#include <eventhandling/FifoStrategy.h>

#include <eventhandling/StackPool.h>

#include <eventhandling/EventHandler.h>

#include <common/debug.h>

#define TAG "FIFOStrategy"

void FifoStrategy::enqueueEvent(EventHandling::EventTask *eventtask) {
	if ( nullptr == eventtask ) {
		return;
	}

	eventtask->pEnqueued = true;

	if ( eventtask->event->pType == EventHandling::Event::INTERRUPT_EVENT ) {
			DEBUG_STREAM(TAG,"Enqueue interrupt event:" << eventtask);
			mInterruptQueueLock.lock();
			mFifoInterruptQueue.enqueue(eventtask);
			mInterruptQueueLock.unlock();
			return;
	}

	// Not an interrupt event, enqueue normally
	mQueueLock.lock();
	mFifoQueue.enqueue(eventtask);
	mQueueLock.unlock();
}

void FifoStrategy::enqueueInterruptEvent(EventHandling::EventTask *event) {
	enqueueEvent(event);
}




/**
 * This actually defines how the strategy schedules its events
 */
EventHandling::EventTask *FifoStrategy::getReadyEvent(cpu_t cpu) {
	EventHandling::EventTask *event;

	/* Check interrupt queue first */
	mInterruptQueueLock.lock();
	event = (EventHandling::EventTask*)mFifoInterruptQueue.dequeueFirstLocked();
	if ( nullptr != event ) {
		mInterruptQueueLock.unlock();
		return event;
	} else {
		mInterruptQueueLock.unlock();
	}

	/* Check for normal software events */
	mQueueLock.lock();
	event = (EventHandling::EventTask*)mFifoQueue.dequeueFirstLocked();
	mQueueLock.unlock();

	return event;
}

EventHandling::EventTask *FifoStrategy::getEvent(cpu_t cpu) {
	EventHandling::EventTask *event = getReadyEvent(cpu);

	if ( nullptr == event ) {
		return nullptr;
	}

	if ( nullptr == event->pStackpointer ) {
		event->pStackpointer = StackPool::pInstance.getFreeStack();
	}

	if ( nullptr == event->pStackpointer) {
		// Requeue the event
		enqueueEvent(event);
		return nullptr;
	}

	event->pEnqueued = false;
	return event;
}

uint32_t FifoStrategy::getPendingEvents() {
	return mFifoQueue.size() + mFifoInterruptQueue.size();
}

bool FifoStrategy::lockEventOSC(EventHandling::EventTask *eventtask) {
	return (EventHandling::EventHandler::pInstance.tryTaskLock((EventHandling::EventTask*)eventtask));
}

