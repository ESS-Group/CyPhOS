/*
 * ApplicationSystemInfo.h
 *
 *  Created on: 16.11.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_APPLICATIONSYSTEMINFO_H_
#define TESTCOMPONENTS_APPLICATIONSYSTEMINFO_H_

#include "ConsoleApplication.h"
#include <eventhandling/TimerEvent.h>

BEGIN_DECLARE_OSC_INHERIT(ApplicationSystemInfo,ApplicationSystemInfo, public ConsoleApplication)

public:
	static ApplicationSystemInfo mInstance;

	ApplicationSystemInfo();

	DECLARE_TRIGGER(printUptime);
	DECLARE_TRIGGER(start);
	DECLARE_TRIGGER(printEventDispatcherStats);
	DECLARE_TRIGGER(printCacheManagementStats);
	DECLARE_TRIGGER(printPMUStats);

	DECLARE_TRIGGER(verifyCritical);

	DECLARE_TRIGGER(crashTest);

	DECLARE_TRIGGER(floatTest);

	DECLARE_TRIGGER(multiCore);


	DECLARE_TRIGGER(printVersion);

	static OSC* trigger_deps[];
private:
	EventHandling::TimerEvent *mTimerEvent;
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_APPLICATIONSYSTEMINFO_H_ */
