/*
 * Queue.h
 *
 *  Created on: 21.07.2015
 *      Author: hendrik
 */

#ifndef COMMON_QUEUE_H_
#define COMMON_QUEUE_H_

#include <common/types.h>

#include "QueueElement.h"

/* Debugging */
#include <common/debug.h>

#ifndef TAG
#define TAG "Queue"
#define QUEUETAG
#endif

//#define LIST_DEBUG

template<class Objtype> class Queue {

public:
	Queue<Objtype>() :
			pHead(nullptr), pTail(nullptr) {
		pSize = 0;
	}

	void enqueueBefore(QueueElement<Objtype> *object, QueueElement<Objtype> *before);

	/**
	 * Enqueues an object if it is not already in the list
	 */
	void enqueue(QueueElement<Objtype> *object);

	/**
	 * Enqueues an object regardless if it is already in. (End of list)
	 */
	void enqueueFast(QueueElement<Objtype> *object);

	/**
	 * Optimized method to return the first object of the list.
	 * @return Head element of the list or NULL if not available
	 */
	QueueElement<Objtype> *dequeueFirst();

	/**
	 * Dequeues an element at pos and returns it
	 */
	QueueElement<Objtype> *dequeue(uint32_t pos);

	/**
	 * Removes an element from the list.
	 * @param element Element to remove from the list. If it is NULL nothing is done.
	 * If it is not part of the list, the element stays untouched.
	 */
	void remove(Objtype *element);

	/**
	 * Returns the element at pos. The element
	 * remains in the list
	 * @return Element at the position, NULL if not available
	 */
	QueueElement<Objtype> *get(uint32_t pos);

	/**
	 * Removes an element from the list with respect to the linkage
	 * USE WITH CARE! No check if element is part of the list.
	 * @param element Element to remove from the list. No NULL check!
	 */
	void removeElementFromList(QueueElement<Objtype> *element);

	/**
	 * Returns the size of the list;
	 */
	uint32_t size() {
		return pSize;
	}

	void clearList();


	// FIXME introduce an iterator
	QueueElement<Objtype> *pHead;
	QueueElement<Objtype> *pTail;

protected:
	uint32_t pSize;

};

#ifdef QUEUETAG
#undef TAG
#undef QUEUETAG
#endif

#endif /* COMMON_QUEUE_H_ */
