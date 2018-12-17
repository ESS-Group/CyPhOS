/*
 * ApplicationSystemInfo.h
 *
 *  Created on: 16.11.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_APPLICATIONCACHEMISSTEST_H_
#define TESTCOMPONENTS_APPLICATIONCACHEMISSTEST_H_

#include "ConsoleApplication.h"
#include <eventhandling/TimerEvent.h>

BEGIN_DECLARE_OSC_INHERIT(ApplicationCacheMissTest,ApplicationCacheMissTest, public ConsoleApplication)

public:
	static ApplicationCacheMissTest mInstance;

	DECLARE_TRIGGER(verifyCache);

	static OSC* trigger_deps[];
private:
	EventHandling::TimerEvent *mTimerEvent;

	static constexpr uint32_t cTestSize = 0.8 * CONFIG_CACHE_WAY_SIZE;

	__attribute__ ((aligned(8))) uint8_t mTestData[cTestSize];
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_APPLICATIONCACHEMISSTEST_H_ */
