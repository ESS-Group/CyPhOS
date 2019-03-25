/*
 * ApplicationPeriodicTest.h
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_APPLICATIONPERIODICTEST_H_
#define TESTCOMPONENTS_APPLICATIONPERIODICTEST_H_

#include "ConsoleApplication.h"

#include <component/OSC.h>

#include <eventhandling/TimerEvent.h>

BEGIN_DECLARE_OSC_INHERIT(ApplicationPeriodicTest,ApplicationPeriodicTest, public ConsoleApplication)
public:
	ApplicationPeriodicTest();


	DECLARE_TRIGGER(startPeriodic);
	DECLARE_TRIGGER(stopPeriodic);
	DECLARE_TRIGGER(periodicCall);

	DECLARE_TRIGGER(oneshotTest);
	DECLARE_TRIGGER(oneshotTestTimerTrigger);

	DECLARE_TRIGGER(endlessTest);


	static OSC* trigger_deps[];

	static ApplicationPeriodicTest mInstance;

private:

	uint8_t mTestArray[60000];
	uint32_t mCounter;

	EventHandling::TimerEvent *mTimerEvent;
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_APPLICATIONPERIODICTEST_H_ */
