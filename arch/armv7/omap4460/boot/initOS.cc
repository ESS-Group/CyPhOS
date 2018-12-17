/*
 * initOS.cc
 *
 *  Created on: 10.04.2015
 *      Author: hendrik
 */

#include <common/debug.h>

/* Include startup message */
#include <common/greeting.h>

/* ARM helper functions */
#include <component/LockedOSC.h>

/* Debug helpers */
#define TAG "INIT"

#include <common/armhelper.h>

#include <common/memreg.h>
#include <common/types.h>

/* Include GIC driver for interrupt control */
#include <driver/ARMGIC.h>

/* Include Cache control driver */
#include <driver/ARMV7CacheControl.h>

/* Include Cortex-A9 generic init code */
#include <arch/armv7/cortexa9/common/cortexa9_generics.h>

/* Include A9 MMU init code */
#include <driver/ARMMMU.h>

/* Include performance monitoring code */
#include <driver/ARMV7PerformanceCounter.h>

/* Event dispatcher */
#include <eventhandling/EventHandler.h>

/* Include omap4460 smp startup code */
#include <arch/armv7/omap4460/omap4460_smp.h>

/* Include the OMAP uart driver for configuration */
#include <arch/armv7/omap4460/driver/OMAPUartDriver.h>

/* Include the OMAP core clock management */
#include <arch/armv7/omap4460/driver/OSCOMAP4460ClockManagement.h>

#include <arch/armv7/cortexa9/CortexA9GlobalTimer.h>

#include <arch/armv7/cortexa9/boot/initOS.h>

/**
 * Initialize the omap specific peripheral devices.
 */
void init_soc() {
	/* Boost clock speed a bit */
	OSC_OMAP4460ClockManagement::pInstance.setCoreClk(OSC_OMAP4460ClockManagement::PANDA_921MHZ);
	BUSY_WAITING_SHORT_NOCACHE
	clock_t sysClk = OSC_OMAP4460ClockManagement::pInstance.getCoreClock();
	DEBUG_STREAM(TAG,"System clock: " << dec << sysClk << " MHz");

	/*
	 * FIXME Move to ARCH specific code.
	 * Enable the UART interrupts of the omap processor.
	 */
	OSC_PREFIX(InputOutput)::OMAPUart::pInstance.setRXInterrupts(true);

	/*
	 * Timer Interrupt
	 * 0x1: core 0
	 */
	OSC_PREFIX(Timer)::CortexA9GlobalTimer::pInstance.initializeTimer();
	OSC_PREFIX(Timer)::CortexA9GlobalTimer::pInstance.dumpStatus();
}

void init_soc_secondary() {

}
