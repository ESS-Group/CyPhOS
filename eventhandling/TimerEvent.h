/*
 * TimerEvent.h
 *
 *  Created on: 07.10.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_TIMEREVENT_H_
#define EVENTHANDLING_TIMEREVENT_H_

#include "EventTask.h"

#include <common/QueueElement.h>

namespace EventHandling {

/**
 * Specialized Event type that is used by the software timer GenericTimer
 * to save if it is a periodic or oneshot event. Also saves the timing information.
 */
class TimerEvent : public Event, public QueueElement<TimerEvent> {
public:
	/**
	 * Interval if this is a periodic event
	 */
	uint32_t mInterval;

	/**
	 * Remaining ticks until this events needs emitting
	 */
	uint32_t mRemainingTicks;

	/**
	 * Periodic or oneshot element.
	 */
	enum TimerEventType {
		ONESHOT,//!< ONESHOT emitted only once and gets returned to the pool after use
		PERIODIC//!< PERIODIC emitted every time the interval passed
	};

	inline TimerEventType getTimerType() {return mTimerEventType;}
	TimerEventType mTimerEventType;
private:
};

}


#endif /* EVENTHANDLING_TIMEREVENT_H_ */
