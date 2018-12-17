/*
 * ExynosTimer.cc
 *
 *  Created on: 30.07.2015
 *      Author: hendrik
 */

#include "ExynosMultiCoreTimer.h"

#ifdef MCT_DEBUG
#include <common/debug.h>
#define TAG "MCT"
#endif

#include <common/memreg.h>

#include <common/armhelper.h>

/* Include clock management */
#include <arch/armv7/exynos4412/driver/ExynosClockManagement.h>

/* Include GIC driver */
#include <driver/ARMGIC.h>

/* Include interrupt dispatcher to register self */
#include <interrupts/InterruptDispatcher.h>


/* Include EventHandler for speed measurement */
#include <eventhandling/EventHandler.h>

/* Debug include for performance counter */
#include <driver/ARMV7PerformanceCounter.h>

#include <benchmarking/ResultContainer.h>

// #define EXYNOS_MCT_EVAL


BEGIN_OSC_IMPLEMENTATION(Timer,ExynosMultiCoreTimer)
/* Static instance */
ExynosMultiCoreTimer ExynosMultiCoreTimer::pInstance;

SECTION_CRITICAL_DATA OSC* GenericTimer::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger GenericTimer::trigger_Interrupt((OSC*) &ExynosMultiCoreTimer::pInstance,
		(void (OSC::*)(dword_t))&ExynosMultiCoreTimer::handleInterrupt, (OSC**)&trigger_Interrupt_Deps, EventHandling::Trigger::cMAX_Priority, 0x0);

ExynosMultiCoreTimer::ExynosMultiCoreTimer()
{
	pLastEventCount = 0;
	InterruptDispatcher::pInstance.registerEvent(HW_INT_G0_IRQ,&trigger_Interrupt);
}

void ExynosMultiCoreTimer::setPeriodic(uint64_t us) {
	disableGlobal();

	/* Calculate the needed cycles for the interval us.
	 * For this we need the clock at which the timer is running
	 *  */
	/* The timer seems to be connected directly to the external clock input (24MHz) */
	clock_t timerClk = FREQ_INPUT;//ExynosClockManagement::pInstance.getMCTClk();
#ifdef MCT_DEBUG
	DEBUG_STREAM(TAG, "Timer clock is: " << dec << timerClk << " Hz");
#endif

	/* Caluclate the needed cycles */
	DEBUG_STREAM(TAG,"Hz value divided by 1000000:"  << dec << ((timerClk/1000000)));
	pTimerValue = ((timerClk/1000000) * us);
	DEBUG_STREAM(TAG,"Timer value calculated: " << dec << pTimerValue);

	/* Set the auto increment cycles */
	writeMCTRegister(MCT_GLOBAL_COMPARATOR0_ADD_INCREMENT,(dword_t)pTimerValue);

	/* Enable the auto increment & comparator mode */
	dword_t globalCon = READ_REGISTER(MCT_GLOBAL_TIMER_CONFIG);
	globalCon |= (MCT_GLOBAL_TCON_COMP0_AUTO_INC | MCT_GLOBAL_TCON_COMP0_ENABLE | MCT_GLOBAL_TCON_START);

	uint32_t lowBits = (uint32_t)pTimerValue;
	uint32_t highBits = (uint32_t)(pTimerValue >> 32);

	/* Set the comparator value */
	writeMCTRegister(MCT_GLOBAL_COMPARATOR0_LOWER,lowBits);
	writeMCTRegister(MCT_GLOBAL_COMPARATOR0_UPPER,highBits);

	/* Set the timer value to 0 */
	writeMCTRegister(MCT_GLOBAL_COUNT_LOWER,0x0);
	writeMCTRegister(MCT_GLOBAL_COUNT_UPPER,0x0);

	/* Enable the interrupts */
	writeMCTRegister(MCT_GLOBAL_TIMER_INTERRUPT_ENABLE,0x1);

	/* Write config register */
	writeMCTRegister(MCT_GLOBAL_TIMER_CONFIG,globalCon);


	/* Debug output of corresponding register set */
	DEBUG_STREAM(TAG,"MCT_GLOBAL_TIMER_CONFIG:" << hex << READ_REGISTER(MCT_GLOBAL_TIMER_CONFIG));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COMPARATOR0_LOWER:" << hex << READ_REGISTER(MCT_GLOBAL_COMPARATOR0_LOWER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COMPARATOR0_UPPER:" << hex << READ_REGISTER(MCT_GLOBAL_COMPARATOR0_UPPER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COUNT_LOWER:" << hex << READ_REGISTER(MCT_GLOBAL_COUNT_LOWER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COUNT_UPPER:" << hex << READ_REGISTER(MCT_GLOBAL_COUNT_UPPER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_TIMER_INTERRUPT_ENABLE:" << hex << READ_REGISTER(MCT_GLOBAL_TIMER_INTERRUPT_ENABLE));

	mTimerInterval = us;
}

void ExynosMultiCoreTimer::setOneshot(uint64_t us) {
	// FIXME not implemented
}

//void ExynosMultiCoreTimer::setGlobalOneshot(uint64_t us) {
//}

void ExynosMultiCoreTimer::disableGlobal() {
	/* Stop global timer */
	dword_t globalCon = READ_REGISTER(MCT_GLOBAL_TIMER_CONFIG);
	/* Disable corresponding bits */
	globalCon &= ~(MCT_GLOBAL_TCON_COMP0_AUTO_INC|MCT_GLOBAL_TCON_COMP0_ENABLE);
	writeMCTRegister(MCT_GLOBAL_TIMER_CONFIG,globalCon);
	/* Disable interrupts */
	writeMCTRegister(MCT_GLOBAL_TIMER_INTERRUPT_ENABLE,0x0);
}

void ExynosMultiCoreTimer::writeMCTRegister(uintptr_t reg, dword_t val) {
	uintptr_t stat_addr;
	uint32_t mask;
	uint32_t i;

	WRITE_REGISTER(reg, val);

	if (reg >= MCT_LOCAL_BASE) {
		if ( reg == MCT_LOCAL_TIMER_CONFIG ) {
			stat_addr = MCT_LOCAL_WSTAT;
			mask = 1 << 3; /* MCT_LOCAL_TIMER_CONFIG write status */
		} else if ( reg == MCT_LOCAL_I_COUNTER ) {
			stat_addr = MCT_LOCAL_WSTAT;
			mask = 1 << 1; /* MCT_LOCAL_I_COUNTER write status */
		} else if ( reg == MCT_LOCAL_T_COUNTER ) {
			stat_addr = MCT_LOCAL_WSTAT;
			mask = 1 << 0; /* MCT_LOCAL_T_COUNTER write status */
		}
		else {
			return;
		}
	} else {
		switch ((uint32_t) reg) {
		case (uint32_t) MCT_GLOBAL_TIMER_CONFIG:
			stat_addr = MCT_GLOBAL_WSTAT;
			mask = 1 << 16; /* G_TCON write status */
			break;
		case (uint32_t) MCT_GLOBAL_COMPARATOR0_LOWER:
			stat_addr = MCT_GLOBAL_WSTAT;
			mask = 1 << 0; /* G_COMP0_L write status */
			break;
		case (uint32_t) MCT_GLOBAL_COMPARATOR0_UPPER:
			stat_addr = MCT_GLOBAL_WSTAT;
			mask = 1 << 1; /* G_COMP0_U write status */
			break;
		case (uint32_t) MCT_GLOBAL_COMPARATOR0_ADD_INCREMENT:
			stat_addr = MCT_GLOBAL_WSTAT;
			mask = 1 << 2; /* G_COMP0_ADD_INCR w status */
			break;
		case (uint32_t) MCT_GLOBAL_COUNT_LOWER:
			stat_addr = MCT_GLOBAL_COUNT_WSTAT;
			mask = 1 << 0; /* G_CNT_L write status */
			break;
		case (uint32_t) MCT_GLOBAL_COUNT_UPPER:
			stat_addr = MCT_GLOBAL_COUNT_WSTAT;
			mask = 1 << 1; /* G_CNT_U write status */
			break;
		default:
			return;
		}
	}

	/* Wait maximum 1 ms until written values are applied */
	for (i = 0; i < (1<<12) / 1000 * 100; i++)
		if (READ_REGISTER(stat_addr) & mask) {
			WRITE_REGISTER(stat_addr,mask);
			return;
		}

	DEBUG_STREAM(TAG,"MCT hangs after writing " << hex << val << " at address: " << reg);
}

void ExynosMultiCoreTimer::ackInterrupt() {
	/* Write 1 to CSTAT */
	writeMCTRegister(MCT_GLOBAL_TIMER_INTERRUPT_CSTAT,0x1);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_G0_IRQ);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_G0_IRQ);
}

void ExynosMultiCoreTimer::handleInterrupt(dword_t arg) {

	handleTick();


	ackInterrupt();
#ifdef EXYNOS_MCT_EVAL
	ResultContainer::pInstance.enterResult(TIMER_END_TIMER,getGlobalCount());
#endif
}


void ExynosMultiCoreTimer::setInterrupt(bool enabled) {
	/* Write 1 to CSTAT */
	writeMCTRegister(MCT_GLOBAL_TIMER_INTERRUPT_CSTAT,0x1);
	/* Enable the interrupts */
	writeMCTRegister(MCT_GLOBAL_TIMER_INTERRUPT_ENABLE,(enabled ? 1 : 0));
}


void ExynosMultiCoreTimer::dumpGlobal() {
	/* Debug output of corresponding register set */
	DEBUG_STREAM(TAG,"MCT_GLOBAL_TIMER_CONFIG:" << hex << READ_REGISTER(MCT_GLOBAL_TIMER_CONFIG));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COMPARATOR0_LOWER:" << hex << READ_REGISTER(MCT_GLOBAL_COMPARATOR0_LOWER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COMPARATOR0_UPPER:" << hex << READ_REGISTER(MCT_GLOBAL_COMPARATOR0_UPPER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COUNT_LOWER:" << hex << READ_REGISTER(MCT_GLOBAL_COUNT_LOWER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_COUNT_UPPER:" << hex << READ_REGISTER(MCT_GLOBAL_COUNT_UPPER));
	DEBUG_STREAM(TAG,"MCT_GLOBAL_TIMER_INTERRUPT_ENABLE:" << hex << READ_REGISTER(MCT_GLOBAL_TIMER_INTERRUPT_ENABLE));
}

void ExynosMultiCoreTimer::initializeTimer() {
	// Call generic initialization
	GenericTimer::initializeTimer();
	/*
	 * Enable the exynos multicore timer interrupts.
	 */
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_G0_IRQ,0x1);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_G0_IRQ);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_G0_IRQ,true);
}

uint64_t ExynosMultiCoreTimer::getHardwareTicks() {
	uint64_t retVal = ((uint64_t)READ_REGISTER(MCT_GLOBAL_COUNT_LOWER)) | (((uint64_t)READ_REGISTER(MCT_GLOBAL_COUNT_UPPER)) << 32);//(((uint64_t)READ_REGISTER(MCT_GLOBAL_COUNT_UPPER)) << 32);// || READ_REGISTER(MCT_GLOBAL_COUNT_LOWER);
	return retVal / pTimerValue;
}

END_OSC_IMPLEMENTATION

