/*
 * BenchmarkCore0.h
 *
 *  Created on: 25.09.2017
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_BENCHMARKCORE0_H_
#define TESTCOMPONENTS_BENCHMARKCORE0_H_

#include <component/OSC.h>

#include "ConsoleApplication.h"

#include <eventhandling/Trigger.h>
#include <eventhandling/Event.h>

BEGIN_DECLARE_OSC_INHERIT(BenchmarkCore0,BenchmarkCore0, public ConsoleApplication)

public:
	/**
	* Trigger objects to react to outside events
	*/
	DECLARE_TRIGGER(start);

	DECLARE_TRIGGER(benchmarkRun);

	static EventHandling::Event mBenchmarkEvent;

	/**
	 * Dependencies for the triggers
	 */
	static OSC* trigger_deps[];
	static OSC* trigger_deps_output[];

	static BenchmarkCore0 mInstance;

private:
	static constexpr uint32_t cMemoryRequests = 32;
	static constexpr uint32_t cBenchmarkCalls = 32768;
	static constexpr uint32_t cBenchmarkMemAccesses = 32768;
	static constexpr uint32_t cMemoryRangeSize = 32 * 1024; // 32kBytes

	uint32_t *mAccessTimes;

	dword_t *mMemory[cMemoryRequests];
};


END_DECLARE_OSC

#endif /* TESTCOMPONENTS_BENCHMARKCORE0_H_ */
