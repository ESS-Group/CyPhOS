/*
 * CortexA9GlobalTimer.h
 *
 *  Created on: 30.07.2015
 *      Author: hendrik
 */

#ifndef DRIVER_CORTEXA9GLOBALTIMER_H_
#define DRIVER_CORTEXA9GLOBALTIMER_H_

/* Include ARM clock management for frequency calculation */
#include <component/InterruptOSC.h>

/* Include interrupt dispatcher for self registering */
#include <interrupts/InterruptDispatcher.h>

#include <driver/GenericTimer.h>
#include "../driver/ARMClockManagement.h"

// See cortex-A9 MPcore reference p. 18
#ifndef GLOBAL_TIMER_BASE
#error no global timer base
#define GLOBAL_TIMER_BASE 0
#endif

#define GLOBAL_TIMER_COUNTER_REGISTER_0 (GLOBAL_TIMER_BASE + 0x00)
#define GLOBAL_TIMER_COUNTER_REGISTER_1 (GLOBAL_TIMER_BASE + 0x04)
#define GLOBAL_TIMER_COMPARATOR_REGISTER_0 (GLOBAL_TIMER_BASE + 0x10)
#define GLOBAL_TIMER_COMPARATOR_REGISTER_1 (GLOBAL_TIMER_BASE + 0x14)
#define GLOBAL_TIMER_CONTROL_REGISTER (GLOBAL_TIMER_BASE + 0x08)
#define GLOBAL_TIMER_INTERRUPT_STATUS_REGISTER (GLOBAL_TIMER_BASE + 0x0C)
#define GLOBAL_TIMER_AUTOINCREMENT_REGISTER (GLOBAL_TIMER_BASE + 0x18)


BEGIN_DECLARE_INTERRUPTOSC_INHERIT(Timer,CortexA9GlobalTimer,public GenericTimer)
public:
	CortexA9GlobalTimer(ARMClockManagement *clockMgr);

	void setOneshot(uint64_t us);


	void setPeriodic(uint64_t us);
	uint64_t getInterval();

	void ackInterrupt();

	/**
	 * enables / disables the interrupt bit of the global timer
	 * Interrupt ID 27
	 * CAUTION: This bit is banked between the Cortex-A9 cores
	 */
	void setInterrupt(bool enable);

	static CortexA9GlobalTimer *getInstance();

	void dumpStatus();

	void handleInterrupt(dword_t arg);

	void initializeTimer();

	virtual uint64_t getHardwareTicks();

	/* Static instance variable */
	static CortexA9GlobalTimer pInstance;
private:
	enum GlobalTimerMode {
		ONESHOT,
		PERIODIC
	};

	ARMClockManagement *pClockManagement;

	uint64_t pTimerValue;
	GlobalTimerMode pMode;

};

END_DECLARE_OSC

#endif /* DRIVER_CORTEXA9GLOBALTIMER_H_ */
