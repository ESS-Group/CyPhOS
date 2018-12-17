/*
 * CANSimpleTalkerApplication.h
 *
 *  Created on: 18.05.2018
 *      Author: hendrik
 */

#ifndef CANBENCHMARK_COMPONENTS_CANSIMPLETALKERAPPLICATION_H_
#define CANBENCHMARK_COMPONENTS_CANSIMPLETALKERAPPLICATION_H_

#include <testcomponents/ConsoleApplication.h>

#include <eventhandling/Trigger.h>

#include <eventhandling/TimerEvent.h>

BEGIN_DECLARE_OSC_INHERIT(CANSimpleTalkerApplication,CANSimpleTalkerApplication, public ConsoleApplication)

public:

	DECLARE_TRIGGER(start);
	DECLARE_TRIGGER(periodicSend);

	static CANSimpleTalkerApplication mInstance;
	static OSC* trigger_output_deps[];
	static OSC* trigger_deps[];

	static EventHandling::Event messageReady;
private:
	EventHandling::TimerEvent *mPeriodicEvent;

	uint8_t mMessage[8];

	uint64_t mCounter;
};

END_DECLARE_OSC

#endif /* CANBENCHMARK_COMPONENTS_CANSIMPLETALKERAPPLICATION_H_ */
