/*
 * StressTestInitiator.h
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#ifndef STRESSTEST_STRESSTESTINITIATOR_H_
#define STRESSTEST_STRESSTESTINITIATOR_H_

#include <testcomponents/ConsoleApplication.h>

BEGIN_DECLARE_OSC_INHERIT(StressTestInitiator,StressTestInitiator, public ConsoleApplication)

public:
	StressTestInitiator();

	static OSC* trigger_deps[];
	static EventHandling::Trigger trigger_Start;

	static EventHandling::Trigger trigger_Periodic;

	void func_Start(dword_t arg);
	void func_Start2(dword_t arg);
	void func_Periodic(dword_t arg);

	static StressTestInitiator mInstance;
};


END_DECLARE_OSC

#endif /* STRESSTEST_STRESSTESTINITIATOR_H_ */
