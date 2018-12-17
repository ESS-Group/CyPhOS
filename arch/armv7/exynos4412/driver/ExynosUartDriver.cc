/*
 * OMAPUart.cpp
 *
 *  Created on: 21.07.2014
 *      Author: hb
 */
#include "ExynosUartDriver.h"

#include <common/memreg.h>
#include <common/bitmanipulation.h>

/* Include clock management of exynos */
#include <arch/armv7/exynos4412/driver/ExynosClockManagement.h>

/* Include exynos_smp for soc restarting */
#include <arch/armv7/exynos4412/exynos_smp.h>

/* Include interrupt dispatcher for self registering */
#include <interrupts/InterruptDispatcher.h>


/* ARM Helper macros */
#include <common/armhelper.h>

/* Include interrupt controller to control interrupt flags */
#include <driver/ARMGIC.h>

#include <testcomponents/UARTConsole.h>

#include <arch/armv7/exynos4412/driver/ExynosMultiCoreTimer.h>


#include <arch/armv7/driver/ARMV7PerformanceCounter.h>
#include <arch/armv7/exynos4412/driver/ExynosMultiCoreTimer.h>

#include <arch/armv7/driver/ARMGIC.h>

#include <eventhandling/EventHandler.h>


MEMBER_OF_OSC_BEGIN(InputOutput)
ExynosUart ExynosUart::pInstance;

SECTION_CRITICAL_DATA OSC* ExynosUart::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger ExynosUart::trigger_Interrupt((OSC*) &ExynosUart::pInstance, (void (OSC::*)(dword_t))&ExynosUart::handleInputInterrupt, (OSC**)&ExynosUart::trigger_Interrupt_Deps, CONFIG_PRIORITY_UART);


ExynosUart::ExynosUart() : GenericIO(this), pBufferWritePointer(0),pBufferReadPointer(0) {
	init();
	InterruptDispatcher::pInstance.registerEvent(HW_INT_UART_1,&trigger_Interrupt);
}

void ExynosUart::resetUART() {
	/*
	 * Set all registers to their reset value.
	 * This way we get a nicely initialized uart controller without
	 * interference from u-boot.
	 */
	WRITE_REGISTER(ULCON,0x00000000);
	WRITE_REGISTER(UCON,0x00003000);
	WRITE_REGISTER(UFCON,0x00000000);
	WRITE_REGISTER(UMCON,0x00000000);
	WRITE_REGISTER(UTRSTAT,0x00006000);
	WRITE_REGISTER(UERSTAT,0x00000000);
	WRITE_REGISTER(UFSTAT,0x00000000);
	WRITE_REGISTER(UMSTAT,0x00000000);
	WRITE_REGISTER(URXH,0x00000000);
	WRITE_REGISTER(UBRDIV,0x00000000);
	WRITE_REGISTER(UFRACVAL,0x00000000);
	WRITE_REGISTER(UINTP,0x00000000);
	WRITE_REGISTER(UINTSP,0x00000000);
	WRITE_REGISTER(UINTM,0xF);
}

void ExynosUart::configure() {
	// Set ULCON line control register
	WRITE_REGISTER(ULCON,0x3);
	// Set control register
	WRITE_REGISTER(UCON,0x105);
	// Set FIFO control register (disable for now)
	WRITE_REGISTER(UFCON,0x0);
	// Mask interrupts for now
	WRITE_REGISTER(UINTM,0xF);
}

void ExynosUart::setBaudrate(uint32_t baudrate) {
	clock_t uartClk = ExynosClockManagement::mInstance.getUARTClk();
//	/* Integer part (ETRM p. 711) */
	uint32_t divVal = (uartClk / (baudrate * 16)) - 1;
	WRITE_REGISTER(UBRDIV,divVal);

	/* FRAC value */
	uint32_t fracVal = (uartClk / ((baudrate * 16)/100)) - 100;
	fracVal = (((fracVal - divVal*100) * 16)/100) + 1;

	WRITE_REGISTER(UFRACVAL,fracVal);
}

void ExynosUart::setRXInterrupts(bool enabled) {
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_UART_1,0xF);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_UART_1,true);
	if ( enabled) {
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART_1);
	} else {
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.disableInterrupt(HW_INT_UART_1);
	}

	/* Clear pending interrupts */
	ackRXInterrupt();
	/* Read the interrupt mask register */
	dword_t intMaskreg = READ_REGISTER(UINTM);
	/* Inverse the receive interrupt mask bit */
	intMaskreg = setbit(intMaskreg,0,!enabled) ;
	WRITE_REGISTER(UINTM,intMaskreg);
}

void ExynosUart::ackRXInterrupt() {
	/* Clear the pending bit */
	dword_t pendInt = READ_REGISTER(UINTP);
    pendInt = 0xF;
    WRITE_REGISTER(UINTP,pendInt);
//    WRITE_REGISTER(UINTSP,pendInt);
}

void ExynosUart::putc(char c) {
	// Check if the transmitter is empty
	while (!((READ_REGISTER(UTRSTAT)) & UTRSTAT_TRANSMITTER_EMPTY) ) {}
	WRITE_REGISTER(UTXH,c);
//	while (!((READ_REGISTER(UTRSTAT)) & UTRSTAT_TRANSMITTER_EMPTY) ) {}

	// Always return for newlines
	if(c=='\n') {
		putc('\r');
	}
}

void ExynosUart::print(char* string, int n) {
	for (int i = 0; i < n; i++) {
		putc(string[i]);
	}
}

void ExynosUart::print(char* string) {
	while(*(string) != 0) {
		putc(*string++);
	}
}

void ExynosUart::init() {
	resetUART();
	configure();
	setBaudrate(460800);
}

char ExynosUart::readC() {
	dword_t utrstat = READ_REGISTER(UTRSTAT);
	dword_t uerstat = READ_REGISTER(UERSTAT);

	if ( !(uerstat & (0x1)) && ((utrstat & (0x1))) ) {
		dword_t val = READ_REGISTER(URXH);
		return (char)val;
	}

	return 0;
}


void ExynosUart::handleInputInterrupt(dword_t arg) {
	char c = readC();

	// Triggers the event of the super class
	EventHandling::EventHandler::pInstance.eventTriggered(&event_charReady, c);

	// Acknowledges the interrupt to the UART controller
	ackRXInterrupt();

	// Renables the interrupt in the GIC
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_UART_1);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART_1);

}
MEMBER_OF_OSC_END

