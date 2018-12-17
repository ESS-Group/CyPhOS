/*
 * EventQueue.cc
 *
 *  Created on: 17.03.2016
 *      Author: hendrik
 */

#include <component/OSC.h>
#include <eventhandling/EventHandler.h>
#include <eventhandling/EventPriorityQueue.h>

#define TAG "EventQueue"

void EventPriorityQueue::enqueue(EventHandling::EventTask *eventtask) {
	/* Check if event is not nullptr.
	 * FIXME remove this check for performance reasons?
	 */
	if (nullptr == eventtask) {
		return;
	}

	/* Priority of the event to queue */
	priority_t prio = eventtask->trigger->pPriority;

	/* Check if the list is empty */
	if (pHead == nullptr) {
		pHead = eventtask;
		pTail = eventtask;

		/* Set the element pointers for linkage */
		eventtask->pNext = nullptr;
		eventtask->pPrevious = nullptr;

		pSize++;

		/* Finished the enqueue */
		return;
	}

	/* Temp element to iterate over the queue */
	QueueElement<EventHandling::EventTask> *curElement = pHead;

	/* Iterate over the queue elements until the priority of the current element is lower than
	 * the one of the element to enqueue.
	 */
	while ((nullptr != curElement) && ((EventHandling::EventTask*) curElement)->trigger->pPriority >= prio) {
		/* Move to the next element and repeat*/
		curElement = curElement->pNext;
	}

	/* curElement now is either nullptr which means that the end of the list is reached or
	 * is != nullptr which means that the event to enqueue has a higher priority.
	 */
	if (nullptr == curElement) {
		/* We reached the end of the queue during the search for a lower priority,
		 * enqueue the element at the end of the queue. */

		/* Set the next pointer to nothing and the pPrevious pointer to the current tail-element */
		eventtask->pNext = nullptr;
		eventtask->pPrevious = pTail;

		/* Set the next pointer of the current last element to the next element */
		pTail->pNext = eventtask;
		/* Set the tail to new tail */
		pTail = eventtask;
	} else {
		/* We found an element with lower priority so insert the event before this element */
		if (curElement == pHead) {
			/* We have to change the head pointer */
			pHead = eventtask;
			curElement->pPrevious = eventtask;

			/* Set pointer of new element */
			eventtask->pNext = curElement;
			eventtask->pPrevious = nullptr;
		} else {
			/* Insert event before the curElement */
			eventtask->pNext = curElement;
			eventtask->pPrevious = curElement->pPrevious;

			eventtask->pPrevious->pNext = eventtask;
			curElement->pPrevious = eventtask;

		}
	}

	pSize++;
}

EventHandling::EventTask *EventPriorityQueue::dequeueHighestPriority() {
	QueueElement<EventHandling::EventTask> *retEl = pHead;
	if (nullptr == retEl) {
		/* Empty queue. Abort here and return nullptr */
		return nullptr;
	}


	/* This iterates over all sorted events and tries to get the lock. This prevents that all but one core
	 * is doing work because of locks around a OSC. This happens if multiple events of _ONE_ OSC are
	 * queued.
	 * The loop also tries to lock all the allocated Memory. This synchronizes memory access to shared memory regions
	 */
	while (nullptr != retEl ) {
#ifdef CONFIG_DEBUG_EVENT_QUEUE
		DEBUG_STREAM(TAG,"Try to lock EventTask: " << hex << retEl);
#endif

		// If no lock could be acquired jump to next priority level
		if (!(EventHandling::EventHandler::pInstance.tryTaskLock((EventHandling::EventTask*) retEl))) {
			priority_t curPriority = ((EventHandling::EventTask*)retEl)->trigger->pPriority;

			while (nullptr != retEl && ((EventHandling::EventTask*)retEl)->trigger->pPriority == curPriority ) {
				retEl = retEl->pNext;
			}

			// Skip next jump and go directly to above if again
			continue;
		} else {
#ifdef CONFIG_DEBUG_EVENT_QUEUE
			DEBUG_STREAM(TAG,"Lock acquired");
#endif

			// Task lock acquired, break loop
			break;
		}
		retEl = retEl->pNext;
	}

	/* Check if retEl is nullptr, this means we couldn't get any lock */
	if (retEl == nullptr) {
		/* Abort */
		return nullptr;
	}

	removeElementFromList(retEl);

	return (EventHandling::EventTask*) retEl;
}

priority_t EventPriorityQueue::getHighestPriority() {
	/* Check if an element is in the queue and if return the priority of
	 * the first element which should be the highest.
	 */
	if (pHead != NULL) {
		return ((EventHandling::EventTask*) pHead)->trigger->pPriority;
	}

	/* If no Element is in the queue return the INVALID priority. */
	return EventHandling::Trigger::cInvalid_Priority;
}

EventHandling::EventTask *EventPriorityQueue::getHighestPriorityElement() {
	return (EventHandling::EventTask*) pHead;
}

EventHandling::EventTask *EventPriorityQueue::dequeueHead() {
	EventHandling::EventTask *retEl = (EventHandling::EventTask*) pHead;

	if (pHead != NULL) {
		pHead = pHead->pNext;

		/* Make the old second element the new head element. */
		if (pHead != NULL) {
			pHead->pPrevious = NULL;
		}
		pSize--;
	}
	return retEl;
}
