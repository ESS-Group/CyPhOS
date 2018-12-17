/*
 * IMX6Watchdog.h
 *
 *  Created on: 19.02.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6WATCHDOG_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6WATCHDOG_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>
#include <eventhandling/TimerEvent.h>

/* Watchdog control register */
#define WATCHDOG_BASE_0 0x20BC000
#define WATCHDOG_WCR(BASE) BASE + 0x0
#define WATCHDOG_WSR(BASE) BASE + 0x2

#define WATCHDOG_WICR(BASE) BASE + 0x6

#define WDOG_WICR_WIE (1 << 15)
#define WDOG_WICR_WTIS (1 << 14)
#define WDOG_WICR_WICT_MASK (0xff)

#define WDOG_WCR_WDE (1 << 2)
#define WDOG_WCR_WT_MASK (2 << 8)

#define WDOG_WCR_SRS (1 << 4)


BEGIN_DECLARE_OSC(IMX6Watchdog, IMX6Watchdog)

public:

	/**
	 * Singleton instance
	 */
	static IMX6Watchdog mInstance;

	/**
	 * Dependencies for the triggers
	 */
	static OSC* trigger_deps[];
	static OSC* trigger_deps_output[];

	DECLARE_TRIGGER(initializeWatchdog);

	DECLARE_TRIGGER(interrupt);

	void restart_system();

private:
	EventHandling::TimerEvent *mPeriodicRewindEvent;

	void reloadTimer();

	void dumpRegisters();
};
END_DECLARE_OSC

#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6WATCHDOG_H_ */
