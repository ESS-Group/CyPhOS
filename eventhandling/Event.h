/*

 * Event.h
 *
 *  Created on: 15.11.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_EVENT_H_
#define EVENTHANDLING_EVENT_H_

#include <common/Queue.h>
#include <common/QueueElement.h>

#include <component/OSC.h>
#include "Trigger.h"


namespace EventHandling {


/**
 * Class that models events. OSCs can emit events to which another component
 * can connect with a Trigger object and react to it. Events can be handled with a
 * given priority and a processor affinity.
 */
class Event : public QueueElement<Event> {
public:


	/**
	 * Adds a subscriber to the Event.
	 * @param con Trigger object to react to this event
	 */
	inline void addSubscriber(Trigger *con ) {
		mSubscribers.enqueue(con);
	}

	/**
	 * Removes a subscriber to the Event.
	 * @param con Trigger object no to react to this event anymore
	 */
	inline void removeSubscriber(Trigger *con) {
		mSubscribers.remove(con);
	}

	enum event_type_t{
		SOFTWARE_EVENT,
		INTERRUPT_EVENT,
		TIMER_EVENT,
		DUMMY_CALL_EVENT,
		EVENT_TYPE_COUNT
	};

	/**
	 * Specify the event type because some events need special care at some points.
	 * Interrupt events for example get different EventTasks to speed up queue handling
	 */
	event_type_t pType;

	/**
	 * List of subscribers of this event
	 */
	Queue<Trigger> mSubscribers;

	Spinlock mSubscriberLock;
private:


};

}

#endif /* EVENTHANDLING_EVENT_H_ */
