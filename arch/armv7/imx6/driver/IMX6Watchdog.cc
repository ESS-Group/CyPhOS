/*
 * IMX6Watchdog.cc
 *
 *  Created on: 19.02.2018
 *      Author: hendrik
 */

#include "IMX6Watchdog.h"

#include <driver/ConsoleBuffer.h>

#include <driver/GenericTimer.h>

/* Include interrupt controller to control interrupt flags */
#include <driver/ARMGIC.h>
#include <interrupts/InterruptDispatcher.h>


#include <common/memreg.h>

#define TAG "IMX6Watchdog"

BEGIN_OSC_IMPLEMENTATION(IMX6Watchdog, IMX6Watchdog)

/**
 * Static singleton instance
 */
IMX6Watchdog IMX6Watchdog::mInstance;

/**
 * Instances of static trigger objects
 */
SECTION_CRITICAL_DATA DEFINE_TRIGGER(IMX6Watchdog, initializeWatchdog, IMX6Watchdog::mInstance, trigger_deps_output, EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(IMX6Watchdog, interrupt, IMX6Watchdog::mInstance, trigger_deps, EventHandling::Trigger::cMAX_Priority - 10);

/**
 * Instance of trigger dependencies
 */
SECTION_CRITICAL_DATA OSC* IMX6Watchdog::trigger_deps[] = { nullptr };
SECTION_CRITICAL_DATA OSC* IMX6Watchdog::trigger_deps_output[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance, nullptr };

DEFINE_TRIGGER_FUNC(IMX6Watchdog,initializeWatchdog) {
	DEBUG_STREAM(TAG, "Initializing periodic watchdog rewinding");

	InterruptDispatcher::pInstance.registerEvent(HW_INT_WDOG1, &trigger_interrupt);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_WDOG1, 0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_WDOG1, true);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_WDOG1);

	// Configure watchdog parameters

	// First configure interrupt
	uint16_t icrReg = READ_REGISTER_16(WATCHDOG_WICR(WATCHDOG_BASE_0));
	icrReg |= WDOG_WICR_WIE;

	// Clear interrupt bit
	icrReg &= ~WDOG_WICR_WTIS;
	// Clear timeout
	icrReg &= ~WDOG_WICR_WICT_MASK;

	// Set time out to 2 seconds (IMX6 trm p. 5794)
	icrReg |= (4);

	WRITE_REGISTER_16(WATCHDOG_WICR(WATCHDOG_BASE_0), icrReg);


	uint16_t wcrReg = READ_REGISTER_16(WATCHDOG_WCR(WATCHDOG_BASE_0));

	// Set watchdog time out to 5 seconds (0.5 steps) (IMX6 ref. manual p. 5791)
	wcrReg |= (9 << 8);

	// Set enable flag
	wcrReg |= WDOG_WCR_WDE;

	WRITE_REGISTER_16(WATCHDOG_WCR(WATCHDOG_BASE_0), wcrReg);
	// Service the watchdog after configuration
	reloadTimer();
}


DEFINE_TRIGGER_FUNC(IMX6Watchdog,interrupt) {
	reloadTimer();
}


void IMX6Watchdog::restart_system() {
	uint16_t wcr = READ_REGISTER_16(WATCHDOG_WCR(WATCHDOG_BASE_0));
	wcr |= WDOG_WCR_WDE | WDOG_WCR_WT_MASK;
	wcr &= ~(WDOG_WCR_SRS);

	WRITE_REGISTER_16(WATCHDOG_WCR(WATCHDOG_BASE_0), wcr);
}

void IMX6Watchdog::reloadTimer() {
	WRITE_REGISTER_16(WATCHDOG_WSR(WATCHDOG_BASE_0), 0x5555);
	WRITE_REGISTER_16(WATCHDOG_WSR(WATCHDOG_BASE_0), 0xaaaa);

	uint16_t icrReg = READ_REGISTER_16(WATCHDOG_WICR(WATCHDOG_BASE_0));

	// Clear interrupt bit
	icrReg |= WDOG_WICR_WTIS;

	WRITE_REGISTER_16(WATCHDOG_WICR(WATCHDOG_BASE_0), icrReg);

	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_WDOG1);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_WDOG1);
}

void IMX6Watchdog::dumpRegisters() {
	DEBUG_STREAM(TAG, "WCR: " << READ_REGISTER_16(WATCHDOG_WCR(WATCHDOG_BASE_0)));
	DEBUG_STREAM(TAG, "WSR: " << READ_REGISTER_16(WATCHDOG_WSR(WATCHDOG_BASE_0)));
	DEBUG_STREAM(TAG, "WICR: " << READ_REGISTER_16(WATCHDOG_WICR(WATCHDOG_BASE_0)));
}

END_OSC_IMPLEMENTATION
