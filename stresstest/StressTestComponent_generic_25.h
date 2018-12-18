/*
 * StressTestComponent25.h
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#ifndef STRESSTEST_StressTestComponent25_H_
#define STRESSTEST_StressTestComponent25_H_

#include <eventhandling/Event.h>

#include <testcomponents/ConsoleApplication.h>

BEGIN_DECLARE_OSC_INHERIT(StressTestComponent25,StressTestComponent25, public ConsoleApplication)
public:
	StressTestComponent25();

	static OSC* trigger_deps_doStress[];
	static EventHandling::Trigger trigger_doStress;

	static EventHandling::Trigger trigger_subscribe;

	static OSC* trigger_deps_printOut[];
	static EventHandling::Trigger trigger_printOut;

	static EventHandling::Event event_Continue;

	void func_subscribe(dword_t arg);

	void func_doStress(dword_t arg);
	void func_printOut(dword_t arg);

	DECLARE_TRIGGER(selfCall);

	static StressTestComponent25 mInstance;

#if !defined(CONFIG_CACHE_WAY_SIZE)
#error "Your architecture lacks the definition of the cache way size"
#endif
	static constexpr uint32_t cArraySize = (CONFIG_CACHE_WAY_SIZE * 0.9) / 4;
	static constexpr uint32_t cAccessCount = 8192;
private:
	uint32_t mIterations;
	uint32_t mTestData[cArraySize];


	uint64_t mCountSum;
	uint64_t mRamCount;
	uint64_t mL2Count;
	uint64_t mL3Count;

	uint32_t mMin;
	uint32_t mMax;
	uint32_t mLastAverage;
	uint32_t mLastCount;
	uint64_t mCycles;
};


END_DECLARE_OSC

#endif /* STRESSTEST_StressTestComponent25_H_ */
