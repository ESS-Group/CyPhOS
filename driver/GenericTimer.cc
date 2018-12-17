/*
 * GenericTimer.cc
 *
 *  Created on: 07.10.2016
 *      Author: hendrik
 */

#include "GenericTimer.h"

#include <eventhandling/EventHandler.h>

//#define CONFIG_GENERIC_TIMER_DEBUG

#ifdef CONFIG_GENERIC_TIMER_DEBUG
#include <common/debug.h>
#endif
#define TAG "GenericTimer"

/* Include the SystemMode class for entering/leaving the mode */
#include <core/SystemMode.h>

#include <common/SyncOutput.h>


MEMBER_OF_OSC_BEGIN(Timer)
GenericTimer* GenericTimer::pInstance;




GenericTimer::GenericTimer() :  mTimerInterval(1), mTicks(0) {
	for ( uint32_t i = 0; i< cTimerPoolSize; i++) {
		mTimerConnections[i].pType = EventHandling::Event::TIMER_EVENT;
		mOneshotEventPool.enqueue(&mTimerConnections[i]);
		mPeriodicEventPool.enqueue(&mTimerConnections[i + (cTimerPoolSize)]);
	}
	GenericTimer::pInstance = this;
}

EventHandling::TimerEvent *GenericTimer::setPeriodicEvent(EventHandling::Trigger *trigger, uint32_t interval) {
	EventHandling::TimerEvent *event = getPeriodicEventFromPool();
	if ( event == nullptr) {
		return nullptr;
	}

#ifdef CONFIG_GENERIC_TIMER_DEBUG
	DEBUG_STREAM(TAG,"Registering periodic event for trigger: " << hex << trigger);
#endif


	/*
	 * Initialize the event
	 */
	event->mTimerEventType = EventHandling::TimerEvent::PERIODIC;

	// Calculate the interval of this event in required ticks
	event->mInterval = interval/mTimerInterval;
	event->mRemainingTicks = event->mInterval;
	event->addSubscriber(trigger);

	// Enqueue this to the queue

	mPeriodicLock.lock();
	mPeriodicQueue.enqueueFast(event);
	mPeriodicLock.unlock();

#ifdef CONFIG_GENERIC_TIMER_DEBUG
	DEBUG_STREAM(TAG, "Event enqueued as periodic: " << event << ", interval in jiffies: " << event->mInterval << ", remaining jiffies:" << event->mRemainingTicks << ", periodic event count: " << dec << mPeriodicQueue.size());
#endif
	return event;
}

EventHandling::TimerEvent *GenericTimer::setOneshotEvent(EventHandling::Trigger *trigger,
		uint32_t delay) {
	EventHandling::TimerEvent *event = getOneshotEventFromPool();
	if ( event == nullptr) {
		return nullptr;
	}

	/*
	 * Initialize the event
	 */
	event->mTimerEventType = EventHandling::TimerEvent::ONESHOT;
	event->addSubscriber(trigger);


	mOneshotLock.lock();
	/*
	 * Check if oneshot events are already scheduled.
	 */
	if ( mOneshotQueue.size() == 0 ) {
		/*
		 * If not, schedule the new event with the given delay as first and only
		 * element to the queue.
		 */
		event->mRemainingTicks = delay/mTimerInterval;
		mOneshotQueue.enqueueFast(event);
#ifdef CONFIG_GENERIC_TIMER_DEBUG
		DEBUG_STREAM(TAG, "Oneshot queue empty, just enqueue");
#endif
	} else {
		// Required ticks for the event to be enqueued
		uint32_t requiredTicks = delay/mTimerInterval;

		// Iteration event
		QueueElement<EventHandling::TimerEvent> *compEvent = mOneshotQueue.get(0);

		// Sum of the delay until the iteration event is due
		uint32_t sumDelay = 0;
		while ( compEvent != nullptr ) {
			sumDelay += ((EventHandling::TimerEvent*)compEvent)->mRemainingTicks;

#ifdef CONFIG_GENERIC_TIMER_DEBUG
		DEBUG_STREAM(TAG, "Required ticks: " << dec << requiredTicks << ", found delay: " << sumDelay);
#endif

			if (requiredTicks < sumDelay) {
				// Calculate the remaining ticks for the new event
				event->mRemainingTicks = requiredTicks - (sumDelay - ((EventHandling::TimerEvent*)compEvent)->mRemainingTicks);

				// Reduce following element
				((EventHandling::TimerEvent*)compEvent)->mRemainingTicks -= event->mRemainingTicks;

				// We found the position where the new event needs to be enqueued
				mOneshotQueue.enqueueBefore(event, compEvent);

				// Finish
				break;
			}
			// Move to the next element
			compEvent = compEvent->getNext();
		}

		if ( compEvent == nullptr ) {
			// At this point we did not found the point to enter the event. Add at the end

			// Calculate the remaining ticks
			event->mRemainingTicks = requiredTicks - sumDelay;

			mOneshotQueue.enqueueFast(event);
		}
	}

	mOneshotLock.unlock();

#ifdef CONFIG_GENERIC_TIMER_DEBUG
	DEBUG_STREAM(TAG, "Event enqueued as periodic: " << event << ", interval in jiffies: " << event->mInterval << ", remaining jiffies:" << event->mRemainingTicks << ", oneshot event count: " << dec << mOneshotQueue.size());
#endif

	return event;
}

void GenericTimer::handleTick() {

#ifdef CONFIG_GENERIC_TIMER_DEBUG
//	DEBUG_STREAM(TAG,"HandleTick");
#endif

	// Lock the oneshot queue
	mOneshotLock.lock();

	// Handle oneshot events first for now
	if ( mOneshotQueue.size() != 0 ) {
		EventHandling::TimerEvent *event = (EventHandling::TimerEvent*)mOneshotQueue.get(0);

		// Decrement ticks
		event->mRemainingTicks--;
		if ( event->mRemainingTicks == 0 ) {
			mOneshotQueue.dequeueFirst();
			EventHandling::EventHandler::pInstance.eventTriggered(event,0);
		}

		// Check if the next elements need exactly the same count of ticks
		while ( mOneshotQueue.size() != 0 ) {
			event = (EventHandling::TimerEvent*)mOneshotQueue.get(0);
			if ( event->mRemainingTicks == 0 ) {
				mOneshotQueue.dequeueFirst();
				EventHandling::EventHandler::pInstance.eventTriggered(event,0);
			} else {
				break;
			}
		}
	}

	// Lock the oneshot queue, lock the periodic queue
	mOneshotLock.unlock();
	mPeriodicLock.lock();

	// Handle periodic events
	if ( mPeriodicQueue.size() != 0 ) {
		// Iterate over all and check if they should fire
		// FIXME, put them in buckets or so

		EventHandling::TimerEvent *event = (EventHandling::TimerEvent*)mPeriodicQueue.get(0);

		while ( nullptr != event ) {
			event->mRemainingTicks--;
			if ( event->mRemainingTicks == 0 ) {
#ifdef CONFIG_GENERIC_TIMER_DEBUG
				DEBUG_STREAM(TAG, "Event: " << hex << event << " run down");
#endif
				EventHandling::EventHandler::pInstance.eventTriggered(event,0);
				event->mRemainingTicks = event->mInterval;
			}
			event = (EventHandling::TimerEvent*)((QueueElement<EventHandling::TimerEvent>*)event)->getNext();
		}
	}

	// Unlock the periodic queue
	mPeriodicLock.unlock();

	// Increase global ticks counter
	mTicks++;
}

void GenericTimer::initializeTimer() {
#ifdef CONFIG_GENERIC_TIMER_DEBUG
	DEBUG_STREAM(TAG,"Initialize generic timer with interval: " << dec << cTimerInterval << " us");
#endif
	setPeriodic(cTimerInterval);
#ifdef CONFIG_GENERIC_TIMER_DEBUG
	DEBUG_STREAM(TAG,"Timer initialized with interval in us: " << dec << mTimerInterval);
#endif
}

void GenericTimer::cancelPeriodicTimerEvent(EventHandling::TimerEvent* event) {
	// Lock the list
	mPeriodicLock.lock();

	// Search the event
	QueueElement<EventHandling::TimerEvent> *compareEvent = mPeriodicQueue.get(0);
	while ( compareEvent != nullptr && compareEvent != event) {
		compareEvent = compareEvent->getNext();
	}

	mPeriodicQueue.removeElementFromList(compareEvent);

	// Requeue the element to the pool of available TimerEvents
	mPeriodicPoolLock.lock();
	event->mSubscribers.clearList();
	mPeriodicEventPool.enqueueFast(event);
	mPeriodicPoolLock.unlock();

	// Unlock the list
	mPeriodicLock.unlock();

}

void GenericTimer::cancelOneshotTimerEvent(EventHandling::TimerEvent* event) {


	// Lock the list
	mOneshotLock.lock();

	// Search the event
	QueueElement<EventHandling::TimerEvent> *compareEvent = mOneshotQueue.get(0);
	while ( compareEvent != nullptr && compareEvent != event) {
		compareEvent = compareEvent->getNext();
	}

	EventHandling::TimerEvent *nextEvent = (EventHandling::TimerEvent*)compareEvent->getNext();
	if ( compareEvent != nullptr &&  nextEvent != nullptr) {
		nextEvent->mRemainingTicks += ((EventHandling::TimerEvent*)compareEvent)->mRemainingTicks;
	}

	mOneshotQueue.removeElementFromList(compareEvent);

	// Requeue the element to the pool of available TimerEvents
	mOneshotPoolLock.lock();
	event->mSubscribers.clearList();
	mOneshotEventPool.enqueueFast(event);
	mOneshotPoolLock.unlock();

	// Unlock the list
	mOneshotLock.unlock();
}

EventHandling::TimerEvent *GenericTimer::getOneshotEventFromPool() {

	// Lock the pool lock
	mOneshotPoolLock.lock();
	// Get an event from the queue
	EventHandling::TimerEvent *event = (EventHandling::TimerEvent*)mOneshotEventPool.dequeueFirst();
	mOneshotPoolLock.unlock();

	return event;
}

EventHandling::TimerEvent *GenericTimer::getPeriodicEventFromPool() {

	// Lock the pool lock
	mPeriodicPoolLock.lock();
	// Get an event from the queue
	EventHandling::TimerEvent *event = (EventHandling::TimerEvent*)mPeriodicEventPool.dequeueFirst();
	mPeriodicPoolLock.unlock();

	return event;
}

void GenericTimer::returnOneshotEventToPool(EventHandling::TimerEvent *event) {
	// Lock the pool lock
	mOneshotPoolLock.lock();
	// Requeue the event
	event->mSubscribers.clearList();
	mOneshotEventPool.enqueueFast(event);
	mOneshotPoolLock.unlock();

}

void GenericTimer::returnPeriodicEventToPool(EventHandling::TimerEvent *event) {

	// Lock the pool lock
	mPeriodicPoolLock.lock();
	// Requeue the event
	event->mSubscribers.clearList();
	mPeriodicEventPool.enqueueFast(event);
	mPeriodicPoolLock.unlock();
}

uint64_t GenericTimer::getTimeInMS() {
	return (mTicks * (mTimerInterval))/1000;
}

void GenericTimer::dumpTimerStatus() {
	SYNC_OUTPUT_STREAM_START("Timer interval in us: " << dec << mTimerInterval << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Timer frequency in Hz: " << dec << 1000000/mTimerInterval << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Ticks: " << dec << mTicks << endl);
	uint64_t hwTicks = getHardwareTicks();
	SYNC_OUTPUT_STREAM_CONTINUE("Hardware ticks: " << dec << hwTicks << endl);
	SYNC_OUTPUT_STREAM_CONTINUE("Drift: " << dec << (hwTicks - mTicks) << " ticks" << endl);
	SYNC_OUTPUT_STREAM_END;
}

void GenericTimer::dumpOneshotQueue() {
	DEBUG_STREAM(TAG,"Oneshot queue:");
	for(unsigned int i = 0; i < mOneshotQueue.size(); i++ ) {
		QueueElement<EventHandling::TimerEvent> *queueEvent = mOneshotQueue.get(i);
		EventHandling::TimerEvent *event = (EventHandling::TimerEvent*)queueEvent;
		DEBUG_STREAM(TAG,"Event: " << dec << i << " addr: " << hex << event << " queueEvent: " << queueEvent << " Remaining ticks: " << dec << event->mRemainingTicks );
	}
}

MEMBER_OF_OSC_END
