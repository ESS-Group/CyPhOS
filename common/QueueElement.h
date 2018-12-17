/*
 * QueueElement.h
 *
 *  Created on: 21.07.2015
 *      Author: hendrik
 */

#ifndef COMMON_QUEUEELEMENT_H_
#define COMMON_QUEUEELEMENT_H_


template<class Objtype> class QueueElement {
/* Allow private access from the queue */
	template <class T> friend class Queue;
	friend class EventQueue;
	friend class EventPriorityQueue;

public:
	QueueElement<Objtype>() {}

	inline QueueElement<Objtype> *getNext() {
		return pNext;
	}

	QueueElement<Objtype> *pNext;
	QueueElement<Objtype> *pPrevious;
};

#endif /* COMMON_QUEUEELEMENT_H_ */
