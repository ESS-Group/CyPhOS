/*
 * APICTimer.cc
 *
 *  Created on: 02.07.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/APICTimer.h>

#include <interrupts/InterruptDispatcher.h>

#include <interrupt_table.h>

#include "LocalAPIC.h"

BEGIN_OSC_IMPLEMENTATION(Timer,APICTimer)

SECTION_CRITICAL_DATA OSC* GenericTimer::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger GenericTimer::trigger_Interrupt((OSC*) &APICTimer::pInstance,
		(void (OSC::*)(dword_t))&APICTimer::triggerFunc_handleInterrupt, (OSC**)&APICTimer::trigger_Interrupt_Deps, CONFIG_PRIORITY_TIMER, 0x00);

APICTimer APICTimer::pInstance;

APICTimer::APICTimer() : GenericTimer(this) {
	InterruptDispatcher::pInstance.registerEvent(HW_INT_LOCAL_APIC_TIMER, &trigger_Interrupt);
}

void APICTimer::setInterrupt(bool enable) {

}

void APICTimer::setPeriodic(uint64_t us) {
	LocalAPIC::mInstance.windupPeriodic(us);
	mTimerInterval = us;
}
void APICTimer::setOneshot(uint64_t us) {

}

uint64_t APICTimer::getHardwareTicks() {
	return 0;
}

DEFINE_TRIGGER_FUNC(APICTimer, handleInterrupt) {
	handleTick();
}

END_OSC_IMPLEMENTATION
