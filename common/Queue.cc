#include <common/Queue.h>

#include <component/OSC.h>
#include <eventhandling/Event.h>
#include <eventhandling/EventTask.h>
#include <eventhandling/TimerEvent.h>

#include <driver/CANEvent.h>

// Instantiate template class implementations
// Otherwise they are missing and not linked
template class Queue<OSC>;
template class Queue<EventHandling::Event>;
template class Queue<EventHandling::EventTask>;
template class Queue<EventHandling::Trigger>;
template class Queue<EventHandling::TimerEvent>;
template class Queue<CANEvent>;

template<class Objtype>
void Queue<Objtype>::enqueueBefore(QueueElement<Objtype> *object, QueueElement<Objtype> *before) {

	if (nullptr != before->pPrevious) {
		// Set the next-pointer of the previous element to the new element
		before->pPrevious->pNext = object;
	} else {
		// This is the head
		pHead = object;
	}

	// Set the next-pointer of the new element to the compare element
	object->pNext = before;
	object->pPrevious = before->pPrevious;

	// Fix the previous-pointer of the compare element to the new added element
	before->pPrevious = object;

	pSize++;
}

template<class Objtype>
void Queue<Objtype>::enqueue(QueueElement<Objtype> *object) {
#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"enqueue in list with size: " << dec << pSize << " element: " << hex << object);
#endif
	/* Iterate over the list and check if the object is already in it */
	QueueElement<Objtype> *curElement = pHead;
	while (curElement != nullptr) {
		if (curElement == object) {
			/* Object is already in the list -> abort! */
#ifdef LIST_DEBUG
			DEBUG_STREAM(TAG,"Element already in list!");
#endif
			return;
		}
		curElement = curElement->pNext;
	}

	/* Element is not already in the list, insert it at the end */

	/* Empty list */
	if (pHead == nullptr && pTail == nullptr) {
		/* First entry in the list */
		pHead = object;
		pTail = object;

		object->pNext = nullptr;
		object->pPrevious = nullptr;
#ifdef LIST_DEBUG
		DEBUG_STREAM(TAG,"Empty list enqueued");
#endif
	} else {
		object->pNext = nullptr;
		object->pPrevious = pTail;

		pTail->pNext = object;
		pTail = object;
#ifdef LIST_DEBUG
		DEBUG_STREAM(TAG,"Nonempty list enqueued");
#endif
	}

	pSize++;
#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"Enqueue finished");
#endif
}

template<class Objtype>
void Queue<Objtype>::enqueueFast(QueueElement<Objtype> *object) {
#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"enqueue in list with size: " << dec << pSize << " element: " << hex << object);
#endif

	/* Empty list */
	if (pHead == nullptr && pTail == nullptr) {
		/* First entry in the list */
		pHead = object;
		pTail = object;

		object->pNext = nullptr;
		object->pPrevious = nullptr;
#ifdef LIST_DEBUG
		DEBUG_STREAM(TAG,"Empty list enqueued");
#endif
	} else {
		object->pNext = nullptr;
		object->pPrevious = pTail;

		pTail->pNext = object;
		pTail = object;
#ifdef LIST_DEBUG
		DEBUG_STREAM(TAG,"Nonempty list enqueued");
#endif
	}

	pSize++;

#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"Enqueue finished");
#endif
}
template<class Objtype>
QueueElement<Objtype> *Queue<Objtype>::dequeueFirst() {
	if ( nullptr == pHead) {
		return nullptr;
	}
	QueueElement<Objtype> *retVal = nullptr;

	if (pHead == pTail) {
		retVal = pHead;
		pHead = nullptr;
		pTail = nullptr;
	} else {
		retVal = pHead;
		pHead = pHead->pNext;

		pHead->pPrevious = nullptr;

	}
	pSize--;
	if ( nullptr != retVal) {
		retVal->pNext = nullptr;
		retVal->pPrevious = nullptr;
	}
	return retVal;
}

template<class Objtype>
QueueElement<Objtype> *Queue<Objtype>::dequeue(uint32_t pos) {
	/* Check if queue is big enough */
	if (pos >= pSize) {
		return nullptr;
	}

#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"Dequeue start with pos: " << dec << pos);
#endif
	/* Iterate to the right object and return it */
	QueueElement<Objtype> *curElement = pHead;
	while (pos != 0 && curElement != nullptr) {
		curElement = curElement->pNext;
		pos--;
	}
	if (nullptr != curElement) {
		removeElementFromList(curElement);
	}

#ifdef LIST_DEBUG
	DEBUG_STREAM(TAG,"Size now: " << dec << pSize);
	DEBUG_STREAM(TAG,"Dequeue finished with element: " << hex << curElement);
#endif
	return curElement;
}

template<class Objtype>
void Queue<Objtype>::remove(Objtype *element) {
	uint32_t pos = 0;

	/* Iterate to the right object and return it */
	QueueElement<Objtype> *curElement = pHead;
	while (curElement != nullptr && curElement != element) {
		curElement = curElement->pNext;
		pos++;
	}
	if (nullptr != curElement) {
		removeElementFromList(curElement);
	}
}

template<class Objtype>
QueueElement<Objtype> *Queue<Objtype>::get(uint32_t pos) {
	/* Check if queue is big enough */
	if (pos >= pSize) {
		return nullptr;
	}

	/* Iterate to the right object and return it */
	QueueElement<Objtype> *curElement = pHead;
	while (pos != 0 && curElement != nullptr) {
		curElement = curElement->pNext;
		pos--;
	}
	return curElement;
}

template<class Objtype>
void Queue<Objtype>::removeElementFromList(QueueElement<Objtype> *element) {
	/* Repair linkage */
	if (pHead == element) {
		pHead = element->pNext;
	}
	if (pTail == element) {
		pTail = element->pPrevious;
	}

	if (element->pPrevious != nullptr) {
		element->pPrevious->pNext = element->pNext;
	}

	if (element->pNext != nullptr) {
		element->pNext->pPrevious = element->pPrevious;
	}

	pSize--;

	element->pNext = nullptr;
	element->pPrevious = nullptr;
}

template<class Objtype>
void Queue<Objtype>::clearList() {
	while(dequeueFirst()!= nullptr) {

	}

}
