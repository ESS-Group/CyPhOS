/*
 * ResultContainer.h
 *
 *  Created on: 12.05.2016
 *      Author: hendrik
 */

#ifndef BENCHMARKING_RESULTCONTAINER_H_
#define BENCHMARKING_RESULTCONTAINER_H_

#include <common/types.h>

#include <common/SyncOutput.h>

enum ResultType{
	OSC_PRELOAD,
	OSC_WRITEBACK,
	TRIGGER_EXECUTION_TIME,
	INTERRUPT_DISPATCH,
	INTERRUPT_START_TIMER,
	CONTEXT_SWITCH,
	INPUT_LATENCY,
	UART_END_TIMER,
	TIMER_END_TIMER,
	BENCHMARK_HORSING_AROUND,
	BENCHMARK_EXECUTION,
};

enum CounterType {
	CYCLE_COUNTER,
	SYSTEM_TIMER,
};

struct BenchmarkResult {
	ResultType type;
	CounterType ctype;
	cycle_t duration;
	uint32_t id;
	uint32_t sequence;
};

class ResultContainer {
private:
	static constexpr uint32_t cResultStorageSize = 500000;
	const char *convertTypeToString(ResultType type);


	BenchmarkResult pStorage[cResultStorageSize];

	uint32_t pCurrentPointer = 1;

	 static bool full;

public:
	ResultContainer();
	static constexpr char *cTAG = "RESULT";
	/**
	 * Method receives the benchmark result of type and value.
	 * This should be inlined, to prevent cache misses
	 */
	inline void enterResult(ResultType type, CounterType counterType, cycle_t value, uint32_t id, uint32_t sequence) {
		if(pCurrentPointer == cResultStorageSize || full) {
			if(!full){
				full = true;
				DEBUG_STREAM("RESULTCONTAINER", "Buffer full!" << endl);
			}
			return;
		}
		pStorage[pCurrentPointer].duration = value;
		pStorage[pCurrentPointer].id = id;
		pStorage[pCurrentPointer].ctype = counterType;
		pStorage[pCurrentPointer].type = type;
		pStorage[pCurrentPointer++].sequence = sequence;
	}

	void printOutResults(cpu_t cpu);

	static ResultContainer pInstance[NR_CPUS];



};

#endif /* BENCHMARKING_RESULTCONTAINER_H_ */
