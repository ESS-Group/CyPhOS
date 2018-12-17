/*
 * ResultContainer.cc
 *
 *  Created on: 12.05.2016
 *      Author: hendrik
 */

#include "ResultContainer.h"

#include <common/debug.h>

#include <eventhandling/EventHandler.h>

#define TAG "RESULT"

__attribute__ ((section (".benchmark_results"))) ResultContainer ResultContainer::pInstance[NR_CPUS];

SECTION_CRITICAL_DATA bool ResultContainer::full;

ResultContainer::ResultContainer() {
	pCurrentPointer = 0;
}

void ResultContainer::printOutResults(cpu_t cpu) {
#ifdef CONFIG_X86_DISCARD_SMI_PROFILING
	EventHandling::EventHandler::pInstance.printSMIDiscards();
#endif

	DEBUG_STREAM(cTAG, "RESULT BEGIN:CPU integer;result_sequence integer;id integer;type text;duration integer;sequence integer");
	for ( uint32_t i = 0; i < pCurrentPointer; i++) {
		DEBUG_STREAM(TAG, "" << (int) cpu << ";" << dec << i << ';' << hex << pStorage[i].id << ';' <<  convertTypeToString(pStorage[i].type) << ';'
				<< dec << pStorage[i].duration << ';' << pStorage[i].sequence);

	}
	DEBUG_STREAM(cTAG,"RESULT_END");
}

const char *ResultContainer::convertTypeToString(ResultType type) {
	switch ( type ) {
	case OSC_PRELOAD: return (const char*)"OSC_PRELOAD";
	case OSC_WRITEBACK: return (const char*)"OSC_WRITEBACK";
	case TRIGGER_EXECUTION_TIME: return (const char*)"TRIGGER_EXECUTION_TIME";
	case INTERRUPT_DISPATCH: return (const char*)"INTERRUPT_DISPATCH";
	case CONTEXT_SWITCH: return (const char*)"CONTEXT_SWITCH";
	case INPUT_LATENCY: return (const char*)"INPUT_LATENCY";
	case BENCHMARK_EXECUTION: return (const char*)"BENCHMARK_EXECUTION";
	case BENCHMARK_HORSING_AROUND: return (const char*)"BENCHMARK_HORSING_AROUND";
	case INTERRUPT_START_TIMER: return (const char*)"INTERRUPT_START_TIMER";
	case UART_END_TIMER: return  (const char*)"UART_END_TIMER";
	case TIMER_END_TIMER: return  (const char*)"TIMER_END_TIMER";
	default: return (const char*)"DEFAULT_RESUlT_SHOULD_NOT_BE_USED";
	}
	return 0;
}
