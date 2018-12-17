/*
 * ARMV7PerformanceCounter.cc
 *
 *  Created on: 11.02.2016
 *      Author: hendrik
 */

#include "ARMV7PerformanceCounter.h"

#include <common/types.h>

#include <common/debug.h>
#include <common/bitmanipulation.h>

#define TAG "ARMPerformanceCounter"


/* Static instance */
ARMV7PerformanceCounter ARMV7PerformanceCounter::pInstance;

ARMV7PerformanceCounter::ARMV7PerformanceCounter() {
}

dword_t ARMV7PerformanceCounter::readID_DFR0()
{
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c0, c1, 2;" : "=&r"(regValue));
	return regValue;
}

void ARMV7PerformanceCounter::printInformation()
{
	DEBUG_STREAM(TAG,"ARMv7 Debug helper information: ");
	DEBUG_STREAM(TAG,"ID_DFR0: " << hex << (dword_t)readID_DFR0());
	DEBUG_STREAM(TAG,"PMCR: " << hex << (dword_t)readPMCR());
	DEBUG_STREAM(TAG,"DBGAUTHSTATUS: " << hex << (dword_t)readDBGAUTHSTATUS());
//	kout << "SDER: " << hex << (dword_t)readSDER() ;
	DEBUG_STREAM(TAG,"PMOVSR: " << hex << (dword_t)readPMOVSR());
	//kout << "HDCR: " << hex << (dword_t)readHDCR() ;
	DEBUG_STREAM(TAG,"PMCNTENSET: " << hex << (dword_t)readEventCounterEnable());
}


dword_t ARMV7PerformanceCounter::readEventCounter(byte_t id)
{
	// First set the selection register (PMSELR)
	__asm__ __volatile__("MCR p15, 0, %0, c9, c12, 5;" :: "r"(id):"memory");

	dword_t regValue;
	__asm__ __volatile__("MRC p15, 0, %0, c9, c13, 2;" : "=&r"(regValue)::"memory");
	return regValue;
}


void ARMV7PerformanceCounter::enableEventCounter(byte_t id)
{
	// Read current value
	dword_t PMCNTENSET;
	asm("MRC p15, 0, %0, c9, c12, 1;" : "=&r"(PMCNTENSET));

	PMCNTENSET |= (1 << id);
	asm("MCR p15, 0, %0, c9, c12, 1;" :: "r"(PMCNTENSET));
}

void ARMV7PerformanceCounter::disableEventCounter(byte_t id)
{
	// Read current value
	dword_t PMCNTENCLEAR = 0x0;
//	asm("MRC p15, 0, %0, c9, c12, 2;" : "=&r"(PMCNTENCLEAR));

	PMCNTENCLEAR |= (1 << id);
	asm("MCR p15, 0, %0, c9, c12, 2;" :: "r"(PMCNTENCLEAR));
}

void ARMV7PerformanceCounter::writeEventCounter(dword_t value, byte_t id)
{
	// First set the selection register (PMSELR)
	asm("MCR p15, 0, %0, c9, c12, 5;" :: "r"(id));

	// Write counter register
	asm("MCR p15, 0, %0, c9, c13, 2;" :: "r"(value));
}
ARMV7PerformanceCounter::ARM_event_t ARMV7PerformanceCounter::readEventCounterType(byte_t id)
{
	// First set the selection register (PMSELR)
	asm("MCR p15, 0, %0, c9, c12, 5;" :: "r"(id));

	dword_t regValue;
	asm("MRC p15, 0, %0, c9, c13, 1;" : "=&r"(regValue));
	return (ARMV7PerformanceCounter::ARM_event_t)regValue;
}
void ARMV7PerformanceCounter::writeEventCounterType(ARM_event_t value, byte_t id)
{
	// First set the selection register (PMSELR)
	asm("MCR p15, 0, %0, c9, c12, 5;" :: "r"(id));

	// Write counter event register
	asm("MCR p15, 0, %0, c9, c13, 1;" :: "r"(value));
}

dword_t ARMV7PerformanceCounter::readCycleCountRegister()
{
	dword_t regValue;
	asm("MRC p15, 0, %0, c9, c13, 0;" : "=&r"(regValue));
	return regValue;
}
void ARMV7PerformanceCounter::writeCycleCountRegister(dword_t value)
{
	asm("MCR p15, 0, %0, c9, c13, 0;" :: "r"(value));
}

void ARMV7PerformanceCounter::enableCycleCount()
{
	/* Read current value */
	dword_t PMCNTENSET;
	asm("MRC p15, 0, %0, c9, c12, 1;" : "=&r"(PMCNTENSET));

	PMCNTENSET |= (1 << 31);
	asm("MCR p15, 0, %0, c9, c12, 1;" :: "r"(PMCNTENSET));
}
void ARMV7PerformanceCounter::disableCycleCount()
{
	/* Read current value */
	dword_t PMCNTENCLEAR;
	asm("MRC p15, 0, %0, c9, c12, 2;" : "=&r"(PMCNTENCLEAR));

	PMCNTENCLEAR &= ~(1 << 31);
	asm("MCR p15, 0, %0, c9, c12, 2;" :: "r"(PMCNTENCLEAR));
}

void ARMV7PerformanceCounter::enableGlobal()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 0;" : "=&r"(pmcr));

	pmcr |= 0x1;
	pmcr &= ~(0x20);
	asm("MCR p15, 0, %0, c9, c12, 0;" :: "r"(pmcr));
}
void ARMV7PerformanceCounter::disableGlobal()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 0;" : "=&r"(pmcr));

	pmcr &= ~(0x1);
	asm("MCR p15, 0, %0, c9, c12, 0;" :: "r"(pmcr));
}

void ARMV7PerformanceCounter::resetGlobal()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 0;" : "=&r"(pmcr));

	pmcr |= ~(0x2);
	asm("MCR p15, 0, %0, c9, c12, 0;" :: "r"(pmcr));
}

dword_t ARMV7PerformanceCounter::readPMCR()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 0;" : "=&r"(pmcr));
	return pmcr;
}

void ARMV7PerformanceCounter::writePMCR(dword_t value)
{
	asm("MCR p15, 0, %0, c9, c12, 0;" : : "r"(value));
}

void  ARMV7PerformanceCounter::set64CycleCounterStep(bool enabled)
{
	dword_t pmcr = readPMCR();
	pmcr = setbit(pmcr,3,enabled);
	writePMCR(pmcr);
}

dword_t ARMV7PerformanceCounter::readPMOVSR()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 3;" : "=&r"(pmcr));
	return pmcr;
}

dword_t ARMV7PerformanceCounter::readHDCR()
{
	dword_t pmcr;
	asm("MRC p15, 4, %0, c1, c1, 1;" : "=&r"(pmcr));
	return pmcr;
}

dword_t ARMV7PerformanceCounter::readSDER()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c1, c1, 1;" : "=&r"(pmcr));
	return pmcr;
}

dword_t ARMV7PerformanceCounter::readDBGAUTHSTATUS()
{
	dword_t pmcr;
	asm("MRC p14, 0, %0, c7, c14, 6;" : "=&r"(pmcr));
	return pmcr;
}

dword_t ARMV7PerformanceCounter::readEventCounterEnable()
{
	dword_t pmcr;
	asm("MRC p15, 0, %0, c9, c12, 1;" : "=&r"(pmcr));
	return pmcr;
}


void ARMV7PerformanceCounter::printCounterValues()
{
	DEBUG_STREAM(TAG,"Cycle counter: " << hex <<  readCycleCountRegister());
	/* get number of implemented counters */
	dword_t ctrlReg = (readPMCR() & (0xF800)) >> 11;
	DEBUG_STREAM(TAG,"Print out " << dec << ctrlReg << " counter registers");
	for ( uint16_t i = 0; i < (uint16_t)ctrlReg; i++) {
		DEBUG_STREAM(TAG,"Counter " << dec << i << ":" << hex << readEventCounter(i));
	}
}
