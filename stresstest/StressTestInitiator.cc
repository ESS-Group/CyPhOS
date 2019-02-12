/*
 * StressTestInitiator.cc
 *
 *  Created on: 09.03.2017
 *      Author: hendrik
 */

#include <stresstest/StressTestInitiator.h>

#include <eventhandling/EventHandler.h>
#include <driver/GenericTimer.h>


BEGIN_OSC_IMPLEMENTATION(StressTestInitiator,StressTestInitiator)

StressTestInitiator StressTestInitiator::mInstance;

SECTION_CRITICAL_DATA OSC* StressTestInitiator::trigger_deps[] = {CONSOLE_BUFFER_DEP, nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestInitiator::trigger_Start = {(OSC*)&StressTestInitiator::mInstance,(void (OSC::*)(dword_t))&StressTestInitiator::func_Start,
				(OSC**)&StressTestInitiator::trigger_deps, EventHandling::Trigger::cMIN_Priority + 2400};

SECTION_CRITICAL_DATA EventHandling::Trigger StressTestInitiator::trigger_Periodic = {(OSC*)&StressTestInitiator::mInstance,(void (OSC::*)(dword_t))&StressTestInitiator::func_Periodic,
				(OSC**)&StressTestInitiator::trigger_deps, EventHandling::Trigger::cMIN_Priority + 2401};

StressTestInitiator::StressTestInitiator() {
}

void StressTestInitiator::func_Start(dword_t arg) {
//	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_subscribe, 0);
	//OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_Periodic, 1 * 1000 * 1000);
}

void StressTestInitiator::func_Start2(dword_t arg) {
	//EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_selfCall, 0);
	//OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_Periodic, 1 * 1000 * 1000);
}


void StressTestInitiator::func_Periodic(dword_t arg) {
	//EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(StressTestComponent1)::StressTestComponent1::trigger_printOut, 0);
}
END_OSC_IMPLEMENTATION

