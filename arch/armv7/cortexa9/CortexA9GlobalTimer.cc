/*
 * CortexA9GlobalTimer.cc
 *
 *  Created on: 30.07.2015
 *      Author: hendrik
 */

#include "CortexA9GlobalTimer.h"

/* Memory mapped registers */
#include <common/memreg.h>
#include <common/bitmanipulation.h>

/* Include exynos clock management */
// FIXME make this more portable
#include <arch/armv7/omap4460/driver/OSCOMAP4460ClockManagement.h>

/* Include interrupt gic driver */
#include <driver/ARMGIC.h>

/* Debugging */
#include <common/debug.h>
#define TAG "GLOBALTIMER"

#ifndef ARM_CLOCK_MANAGEMENT
#error "No clock management defined for this platform"
#define ARM_CLOCK_MANAGEMENT
#endif

#include <eventhandling/EventHandler.h>

/* Include the SystemMode class for entering/leaving the mode */
#include <core/SystemMode.h>

BEGIN_OSC_IMPLEMENTATION(Timer,CortexA9GlobalTimer)

/* Static instance */
CortexA9GlobalTimer CortexA9GlobalTimer::pInstance(ARM_CLOCK_MANAGEMENT);

SECTION_CRITICAL_DATA OSC* GenericTimer::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger GenericTimer::trigger_Interrupt((OSC*) &CortexA9GlobalTimer::pInstance,
		(void (OSC::*)(dword_t))&CortexA9GlobalTimer::handleInterrupt, (OSC**)&CortexA9GlobalTimer::trigger_Interrupt_Deps, CONFIG_PRIORITY_TIMER, 0x00);

CortexA9GlobalTimer::CortexA9GlobalTimer(ARMClockManagement* clockMgr) {
	pClockManagement = clockMgr;

	/* Initialize timer */
	InterruptDispatcher::pInstance.registerEvent(PPI_INT_GLOBAL_TIMER, &trigger_Interrupt);
}

void CortexA9GlobalTimer::setOneshot(uint64_t us) {

	clock_t periphClock = pClockManagement->getPeripheralClock();
	DEBUG_STREAM(TAG, "Peripheral clock: " << dec << periphClock);

	DEBUG_STREAM(TAG, "Oneshot with interval of: " << dec << us << " us requested");
	/* Disable timer first */
	dword_t controlReg = READ_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER);
	controlReg &= ~(0x1);
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);
	DEBUG_STREAM(TAG, "Timer disabled");

	/* Don't use prescale value for now */
	byte_t prescaleValue = 0x00;
	unsigned int preScaleMask = 0 | (prescaleValue << 8);

	DEBUG_STREAM(TAG, "Hz value divided by 1000000:" << dec << ((periphClock/1000000)));
	pTimerValue = ((periphClock / 1000000) * us) - 1;
	DEBUG_STREAM(TAG, "Timer value calculated: " << dec << pTimerValue);

	uint32_t lowBits = (uint32_t) pTimerValue;
	uint32_t highBits = (uint32_t) (pTimerValue >> 32);

	// Set comparator register
	WRITE_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_0, lowBits);
	WRITE_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_1, highBits);

	/* Clear timer registers */
	WRITE_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_0, 0);
	WRITE_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_1, 0);

	DEBUG_STREAM(TAG, "Low bits: " << hex << lowBits);
	DEBUG_STREAM(TAG, "High bits: " << hex << highBits);

	// Clear prescale value
	controlReg = controlReg & ~(0xFF << 8);
	controlReg = controlReg | preScaleMask;
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);
	pMode = ONESHOT;

	/* Enable the timer/comparator, disable autoincrement */
	controlReg |= (0x1) | (0x2);
	controlReg &= ~(0x8);
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);

}

void CortexA9GlobalTimer::setPeriodic(uint64_t us) {
	/* Disable timer first */
	dword_t controlReg = READ_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER);
	controlReg &= ~(0x1);
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);

	/* Don't use prescale value for now */
	byte_t prescaleValue = 0x00;
	unsigned int preScaleMask = 0 | (prescaleValue << 8);

	clock_t periphClock = pClockManagement->getPeripheralClock();
	pTimerValue = ((periphClock / 1000000) * us) - 1;

	DEBUG_STREAM(TAG, "Peripheral clock: " << dec << periphClock);
	DEBUG_STREAM(TAG, "Timer value calculated: " << dec << pTimerValue);

	uint32_t lowBits = (uint32_t) pTimerValue;
	uint32_t highBits = (uint32_t) (pTimerValue >> 32);

	// Set comparator register
	WRITE_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_0, lowBits);
	WRITE_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_1, highBits);

	DEBUG_STREAM(TAG, "Low bits: " << hex << lowBits);
	DEBUG_STREAM(TAG, "High bits: " << hex << highBits);

	/* Set auto increment value */
	WRITE_REGISTER(GLOBAL_TIMER_AUTOINCREMENT_REGISTER, lowBits);

	/* Clear timer registers */
	WRITE_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_0, 0);
	WRITE_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_1, 0);

	// Clear prescale value
	controlReg = controlReg & ~(0xFF << 8);
	controlReg = controlReg | preScaleMask;
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);
	pMode = PERIODIC;

	/* Enable the timer/comparator, enable autoincrement */
	controlReg |= (0x1) | (0x2) | (0x8);
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);

	mTimerInterval = us;
}

uint64_t CortexA9GlobalTimer::getInterval() {
	return pTimerValue;
}

void CortexA9GlobalTimer::ackInterrupt() {
	dword_t isrReg = READ_REGISTER(GLOBAL_TIMER_INTERRUPT_STATUS_REGISTER);
	isrReg = setbit(isrReg, 0, true);
	WRITE_REGISTER(GLOBAL_TIMER_INTERRUPT_STATUS_REGISTER, isrReg);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(PPI_INT_GLOBAL_TIMER);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(PPI_INT_GLOBAL_TIMER);
}

void CortexA9GlobalTimer::setInterrupt(bool enable) {
	/* Set or unset the interrupt enable bit (pos 2) */
	dword_t controlReg = READ_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER);
	controlReg = setbit(controlReg, 2, enable);
	WRITE_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER, controlReg);
}

void CortexA9GlobalTimer::dumpStatus() {
	DEBUG_STREAM(TAG, "Timer base addr: " << hex << GLOBAL_TIMER_BASE);
	DEBUG_STREAM(TAG, "COUNT0: " << hex << READ_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_0));
	DEBUG_STREAM(TAG, "COUNT1: " << hex << READ_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_1));
	DEBUG_STREAM(TAG, "COMPARATOR0: " << hex << READ_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_0));
	DEBUG_STREAM(TAG, "COMPARATOR1: " << hex << READ_REGISTER(GLOBAL_TIMER_COMPARATOR_REGISTER_1));
	DEBUG_STREAM(TAG, "CONTROL: " << hex << READ_REGISTER(GLOBAL_TIMER_CONTROL_REGISTER));
	DEBUG_STREAM(TAG, "INTERRUPTSTATUS: " << hex << READ_REGISTER(GLOBAL_TIMER_INTERRUPT_STATUS_REGISTER));
	DEBUG_STREAM(TAG, "AUTOINCREMENT: " << hex << READ_REGISTER(GLOBAL_TIMER_AUTOINCREMENT_REGISTER));
}

void CortexA9GlobalTimer::handleInterrupt(dword_t arg) {
	handleTick();
//	dumpStatus();
	ackInterrupt();
}

void CortexA9GlobalTimer::initializeTimer() {
	// Call generic initialization
	GenericTimer::initializeTimer();

	/*
	 * Enable the PPI timer interrupts
	 */
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(PPI_INT_GLOBAL_TIMER, 0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(PPI_INT_GLOBAL_TIMER);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(PPI_INT_GLOBAL_TIMER, true);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(PPI_INT_GLOBAL_TIMER);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(PPI_INT_GLOBAL_TIMER);

	setInterrupt(true);
	ackInterrupt();
}

uint64_t CortexA9GlobalTimer::getHardwareTicks() {
	uint64_t retVal = READ_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_0) | ((uint64_t) (READ_REGISTER(GLOBAL_TIMER_COUNTER_REGISTER_1)) << 32);
	return retVal / pTimerValue;
}

END_OSC_IMPLEMENTATION
