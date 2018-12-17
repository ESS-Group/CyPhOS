/*
 * ResultPrintApplication.cc
 *
 *  Created on: 07.03.2017
 *      Author: hendrik
 */

#include <benchmarking/ResultPrintApplication.h>

#include <common/debug.h>

#define TAG "ResultPrintApplication"

BEGIN_OSC_IMPLEMENTATION(ResultPrintApplication,ResultPrintApplication)

ResultPrintApplication ResultPrintApplication::mInstance;


SECTION_CRITICAL_DATA OSC* ResultPrintApplication::trigger_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger ResultPrintApplication::trigger_PrintResultContainer = {(OSC*)&ResultPrintApplication::mInstance,(void (OSC::*)(dword_t))&ResultPrintApplication::triggerFunc_printResultContainter,
				(OSC**)&ResultPrintApplication::trigger_deps, EventHandling::Trigger::cMIN_Priority + 90};

ResultPrintApplication::ResultPrintApplication() {
}


void ResultPrintApplication::triggerFunc_printResultContainter(dword_t arg) {
	BUSY_WAITING_SHORT
	DEBUG_STREAM(TAG,"Clear");
	// Print out every CPUs ResultContainer
	for (cpu_t cpu = 0; cpu < NR_CPUS;cpu++) {
		ResultContainer::pInstance[cpu].printOutResults(cpu);
	}
}

END_OSC_IMPLEMENTATION
