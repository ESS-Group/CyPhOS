/*
 * EventQueue.h
 *
 *  Created on: 15.03.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_EVENTPRIORITYQUEUE_H_
#define EVENTHANDLING_EVENTPRIORITYQUEUE_H_

/* Include the baseclass */
#include <eventhandling/EventQueue.h>

/* Include the object to store */
#include <eventhandling/EventTask.h>

/* Include spinlock */
#include <sync/Spinlock.h>

class EventPriorityQueue : public Queue<EventHandling::EventTask>{
public:
	EventPriorityQueue() : Queue() {}

	/** This method enqueues an EventConnection object to
	 * the position according to its priority. This makes sure the list stays sorted.
	 */
	void enqueue(EventHandling::EventTask *event);

	/**
	 * Returns the highest priority of the queue.
	 * Because the list is sorted this should be the priority of the first element of the list.
	 */
	priority_t getHighestPriority();

	/**
	 * Returns the element with the highest priority which should just be the first one.
	 * Return NULL if no element is in the queue!
	 */
	EventHandling::EventTask *getHighestPriorityElement();

	/**
	 * Dequeues highest priority element which is free and not locked by another core (the OSC of the event) and returns it.
	 */
	EventHandling::EventTask *dequeueHighestPriority();



	EventHandling::EventTask *dequeueHead();
};

#endif /* EVENTHANDLING_EVENTPRIORITYQUEUE_H_ */
