/*
 * EventMapping.h
 *
 *  Created on: 10.02.2017
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_EVENTMAPPING_H_
#define EVENTHANDLING_EVENTMAPPING_H_

#include <component/OSC.h>

#include <eventhandling/Trigger.h>
#include <eventhandling/Event.h>

namespace EventHandling {

/**
 * This class is a micro Task that actually handles an emitted Event
 * by calling the Trigger with a small stack. For every emitted Event
 * a new EventTask instance is created.
 */
class EventTask : public QueueElement<EventTask> {
public:
	/**
	 * Event that this task handles.
	 */
	Event *event;

	/**
	 * Trigger that is handling the event (destination OSC and destination method)
	 */
	Trigger *trigger;

	/**
	 * Timestamp (increasing counter) of the Task. Can be used to prevent starvation.
	 */
	uint64_t pTimestamp;

	/**
	 * Set if the task is enqueued to a ready list. Is used to prevent multiple enqueueing.
	 */
	bool pEnqueued;

	/**
	 * Argument that is passed to the Trigger method call.
	 */
	dword_t mArg;

	/**
	 * Stackpointer for the micro stack used to handle the event.
	 */
	void *pStackpointer;
};

}
#endif /* EVENTHANDLING_EVENTTASK_H_ */
