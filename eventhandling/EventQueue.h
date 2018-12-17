/*
 * EventQueue.h
 *
 *  Created on: 21.11.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_EVENTQUEUE_H_
#define EVENTHANDLING_EVENTQUEUE_H_

#include <eventhandling/EventTask.h>
#include <common/Queue.h>

/**
 * Queue extension that provides a method to get the first connection in the list
 * for which a lock on the destination OSC could be be acquired.
 *
 * This class is useful if the CONFIG_LOCK_OSC option is set and OSCs events are only
 * executed when the OSC is not used by another processor.
 */
class EventQueue : public Queue<EventHandling::EventTask>{
public:
	EventQueue();

	EventHandling::EventTask *dequeueFirstLocked();
};

#endif /* EVENTHANDLING_EVENTQUEUE_H_ */
