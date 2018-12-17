/*
 * TacleBenchInitiator.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_TACLEBENCHINITIATOR_H_
#define TACLEBENCH_COMPONENTS_TACLEBENCHINITIATOR_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

#include <testcomponents/UARTConsole.h>

BEGIN_DECLARE_OSC_INHERIT(TacleBenchInitiator,TacleBenchInitiator,  public ConsoleApplication)

public:
	TacleBenchInitiator();

	DECLARE_TRIGGER(startBenchmarks);

	static TacleBenchInitiator mInstance;
	static OSC* trigger_deps[];
};

END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_TACLEBENCHINITIATOR_H_ */
