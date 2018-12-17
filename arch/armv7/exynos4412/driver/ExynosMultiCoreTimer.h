/*
 * ExynosTimer.h
 *
 *  Created on: 30.07.2015
 *      Author: hendrik
 */

#ifndef DRIVER_EXYNOSMULTICORETIMER_H_
#define DRIVER_EXYNOSMULTICORETIMER_H_

/* Timer registers */
#define MCT_BASE_ADDRESS 0x10050000

/* Global registers */
#define MCT_GLOBAL_COUNT_LOWER (uintptr_t)(MCT_BASE_ADDRESS + 0x100)
#define MCT_GLOBAL_COUNT_UPPER (uintptr_t)(MCT_BASE_ADDRESS + 0x104)

#define MCT_GLOBAL_COUNT_WSTAT (uintptr_t)(MCT_BASE_ADDRESS + 0x110)

#define MCT_GLOBAL_COMPARATOR0_LOWER (uintptr_t)(MCT_BASE_ADDRESS + 0x200)
#define MCT_GLOBAL_COMPARATOR0_UPPER (uintptr_t)(MCT_BASE_ADDRESS + 0x204)
#define MCT_GLOBAL_COMPARATOR0_ADD_INCREMENT (uintptr_t)(MCT_BASE_ADDRESS + 0x208)

#define MCT_GLOBAL_TIMER_CONFIG (uintptr_t)(MCT_BASE_ADDRESS + 0x240)

#define MCT_GLOBAL_TIMER_INTERRUPT_CSTAT (uintptr_t)(MCT_BASE_ADDRESS + 0x244)
#define MCT_GLOBAL_TIMER_INTERRUPT_ENABLE (uintptr_t)(MCT_BASE_ADDRESS + 0x248)
#define MCT_GLOBAL_WSTAT (uintptr_t)(MCT_BASE_ADDRESS + 0x24C)


/* Local registers
 * Those seem to have an offset of 0x100 * core
 */
#define MCT_LOCAL_BASE (uintptr_t)(MCT_BASE_ADDRESS + 0x300 + (0x100 *getCPUID()))
#define MCT_LOCAL_T_COUNTER (uintptr_t)(MCT_LOCAL_BASE + 0x00)
#define MCT_LOCAL_I_COUNTER (uintptr_t)(MCT_LOCAL_BASE + 0x08)
#define MCT_LOCAL_TIMER_CONFIG (uintptr_t)(MCT_LOCAL_BASE + 0x20)

#define MCT_LOCAL_INTERRUPT_CSTAT (uintptr_t)(MCT_LOCAL_BASE + 0x30)
#define MCT_LOCAL_INTERRUPT_ENABLE (uintptr_t)(MCT_LOCAL_BASE + 0x34)
#define MCT_LOCAL_WSTAT (uintptr_t)(MCT_LOCAL_BASE + 0x40)

/* Bit fields */
#define MCT_GLOBAL_TCON_START		(1 << 8)
#define MCT_GLOBAL_TCON_COMP0_AUTO_INC	(1 << 1)
#define MCT_GLOBAL_TCON_COMP0_ENABLE		(1 << 0)

#define MCT_LOCAL_TCON_INTERVAL_MODE	(1 << 2)
#define MCT_LOCAL_TCON_INT_START		(1 << 1)
#define MCT_LOCAL_TCON_TIMER_START		(1 << 0)

#define MCT_DEBUG

#include <driver/GenericTimer.h>
#include <component/InterruptOSC.h>

BEGIN_DECLARE_INTERRUPTOSC_INHERIT(Timer,ExynosMultiCoreTimer,public GenericTimer)

public:
	ExynosMultiCoreTimer();
	enum TimerMode {
		ONESHOT,
		PERIODIC
	};

	/**
	 * Interrupt trigger function
	 */
	void handleInterrupt(dword_t arg);

	/**
	 * Set periodic interval in us
	 */
	void setPeriodic(uint64_t us);

	void setOneshot(uint64_t us);

	void initializeTimer();

	/**
	 * Set oneshot interval in us
	 */
//	void setGlobalOneshot(uint64_t us);

	/**
	 * Disable global timer
	 */
	void disableGlobal();


	void setInterrupt(bool enabled);
	void dumpGlobal();

	uint64_t getHardwareTicks();

	/* Static instance */
	static ExynosMultiCoreTimer pInstance;
private:

	void writeMCTRegister(uintptr_t reg,dword_t val);

	void ackInterrupt();

	uint64_t pTimerValue;


	/* Temporary event counter value to measure speed of events per second */
	uint32_t pLastEventCount;

};

END_DECLARE_OSC

#endif /* DRIVER_EXYNOSMULTICORETIMER_H_ */
