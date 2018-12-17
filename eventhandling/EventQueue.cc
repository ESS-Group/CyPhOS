/*
#include <eventhandling/EventTask.h>
 * EventQueue.cc
 *
 *  Created on: 21.11.2016
 *      Author: hendrik
 */

#include "EventQueue.h"
#include "EventHandler.h"

#define TAG "EventQueue"

EventQueue::EventQueue() : Queue() {

}

/**
 * Returns the first EventConnection for which a lock could be acquired.
 * @return The EventConnection that is now locked and ready to be executed.
 */
EventHandling::EventTask *EventQueue::dequeueFirstLocked() {
	QueueElement<EventHandling::EventTask> *event = pHead;
	while ( nullptr != event ) {
#ifdef CONFIG_DEBUG_EVENT_QUEUE
			DEBUG_STREAM(TAG,"Try lock for OSC:" << hex << ((EventConnection*)event)->pDestinationOSC);
#endif
		if ( (EventHandling::EventHandler::pInstance.tryTaskLock((EventHandling::EventTask*)event)) ) {
			/* If a lock for the destination OSC is acquired the EventConnection is removed
			 * from the queue and returned.
			 */
#ifdef CONFIG_DEBUG_EVENT_QUEUE
			DEBUG_STREAM(TAG,"Got lock for OSC:" << hex << ((EventConnection*)event)->pDestinationOSC);
#endif
			removeElementFromList(event);
			return (EventHandling::EventTask*)event;
		}
		event = event->pNext;
	}

	/* No OSC can be locked or nothing is ready to be executed */
	return nullptr;
}
