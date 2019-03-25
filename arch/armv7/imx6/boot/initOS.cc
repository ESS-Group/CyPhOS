/*
 * initOS.cc
 *
 *  Created on: 10.04.2015
 *      Author: hendrik
 */

#include <common/debug.h>

/* Include startup message */
#include <common/greeting.h>


/* Debug helpers */
#define TAG "IMX6_INIT"

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

/* Include i.mx6 smp startup code */
#include <arch/armv7/imx6/imx6_smp.h>

/* Include the OMAP uart driver for configuration */
#include <arch/armv7/imx6/driver/IMX6UartDriver.h>

/* Include the i.MX6 core clock management */
#include <arch/armv7/imx6/driver/IMX6ClockManagement.h>

/* Include the i.MX6 uart driver */
#include <arch/armv7/imx6/driver/IMX6UartDriver.h>

/* Include the i.MX6 gpio driver */
#include <arch/armv7/imx6/driver/IMX6GPIO.h>

/* Include the i.MX6 Clock/Power management unit */
#include <arch/armv7/imx6/driver/IMX6ClockManagement.h>

#include <arch/armv7/cortexa9/CortexA9GlobalTimer.h>

#include <arch/armv7/imx6/driver/MCP2515SPI.h>

#include <arch/armv7/imx6/driver/IMX6Watchdog.h>

#include <arch/armv7/imx6/driver/IMX6TempMonitor.h>

extern "C" {
	void init_soc();
	void init_soc_secondary();
}



void init_soc()
{
	OSC_PREFIX(InputOutput)::IMX6Uart::pInstance.setRXInterrupts(true);
	OSC_PREFIX(GPIO)::IMX6GPIO::mInstance.configureInterrupts();

	/*
	 * Set the core frequency before setting the timers, as the core frequency influences the timer
	 * interval.
	 */
	IMX6ClockManagement::mInstance.setCoreFrequencyVoltage(IMX6ClockManagement::mFrequencies[4]);
	IMX6ClockManagement::mInstance.dumpPMURegisters();
	IMX6ClockManagement::mInstance.dumpCCMRegisters();

	/*
	 * Timer Interrupt
	 * 0x1: core 0
	 */

	OSC_PREFIX(Timer)::CortexA9GlobalTimer::pInstance.initializeTimer();
	OSC_PREFIX(Timer)::CortexA9GlobalTimer::pInstance.dumpStatus();
#ifdef CONFIG_CAN
	OSC_PREFIX(MCP2515SPI)::MCP2515SPI::mInstance.resetController();
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(MCP2515SPI)::MCP2515SPI::trigger_start, 0);
#endif

#ifdef CONFIG_IMX6_WATCHDOG
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(IMX6Watchdog)::IMX6Watchdog::trigger_initializeWatchdog, 0);
#endif

#ifdef CONFIG_IMX6_TEMPMONITOR
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(IMX6TempMonitor)::IMX6TempMonitor::trigger_initialize, 0);
#endif
}


void init_soc_secondary() {
	//OSC_PREFIX(Timer)::CortexA9GlobalTimer::pInstance.setInterrupt(true);
}


