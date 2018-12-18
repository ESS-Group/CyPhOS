/*
 * StressTestComponent0.h
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#ifndef STRESSTEST_StressTestComponent101_H_
#define STRESSTEST_StressTestComponent101_H_

#include <eventhandling/Event.h>

#include <testcomponents/ConsoleApplication.h>

BEGIN_DECLARE_OSC_INHERIT(StressTestComponent101,StressTestComponent101, public ConsoleApplication)
public:
	StressTestComponent101();

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

	static StressTestComponent101 mInstance;

#if !defined(CONFIG_CACHE_WAY_SIZE)
#error "Your architecture lacks the definition of the cache way size"
#endif
	static constexpr uint32_t cArraySize = (CONFIG_CACHE_WAY_SIZE * 0.9) / 4;
//	static constexpr uint32_t cAccessCount = ((cArraySize * 4) / CONFIG_CACHE_LINE_SIZE) * 4;
	static constexpr uint32_t cAccessCount = CONFIG_CACHE_WAY_SIZE * 0.5;
private:
	uint32_t mIterations;
	uint32_t mTestData[cArraySize];


	uint32_t mCountSum;
	uint32_t mRamCount;
	uint32_t mL2Count;

	uint32_t mMin;
	uint32_t mMax;
	uint32_t mLastAverage;

	static constexpr uint32_t cRandomSeed = 0xBADC0FFE;
	uint32_t mCurrentRandomSeed;

	uint32_t random32();
};


END_DECLARE_OSC

#endif /* STRESSTEST_StressTestComponent101_H_ */
