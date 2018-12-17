/*
 * OMAPUart.cpp
 *
 *  Created on: 21.07.2014
 *      Author: hb
 */

#include <common/memreg.h>
#include <common/bitmanipulation.h>
#include "OMAPUartDriver.h"


/* Include interrupt dispatcher for self registering */
#include <interrupts/InterruptDispatcher.h>

/* ARM Helper macros */
#include <common/armhelper.h>

/* Include interrupt controller to control interrupt flags */
#include <driver/ARMGIC.h>


#include <eventhandling/EventHandler.h>

MEMBER_OF_OSC_BEGIN(InputOutput)
OMAPUart OMAPUart::pInstance;

SECTION_CRITICAL_DATA OSC* OMAPUart::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger OMAPUart::trigger_Interrupt((OSC*) &OMAPUart::pInstance, (void (OSC::*)(dword_t))&OMAPUart::handleInputInterrupt, (OSC**)&OMAPUart::trigger_Interrupt_Deps, CONFIG_PRIORITY_UART);



OMAPUart::OMAPUart() : GenericIO(this) {
	init();
	InterruptDispatcher::pInstance.registerEvent(HW_INT_UART3,&trigger_Interrupt);
}

void OMAPUart::resetUART() {
	// Set the SOFTRESET to 1
	dword_t systemReg = READ_REGISTER(UART3_SYSC);
	systemReg = setbit(systemReg, 1, true);
	WRITE_REGISTER(UART3_SYSC, systemReg);

	// Wait for reset acknowledgment (RESETDONE)
	volatile char *systemStatusReg = (char*) UART3_SYSS;
	while (!testbit(*systemStatusReg, 0))
		;
	// FIXME
	WRITE_REGISTER(0x48020070, 0x0);
}

void OMAPUart::configure() {
	// See omap ref. p. 4711
	// Enable FIFO
	dword_t uart3_lcrBackup = READ_REGISTER(UART3_LCR);

	// enable configuration mode b
	WRITE_REGISTER(UART3_LCR, 0x00BF);
	// Enable register submode TCL_TLR with ENHANCED_EN bit
	bool uart3_efrBackup = testbit(READ_REGISTER(UART3_EFR), ENHANCED_EN);

	dword_t efrVal = setbit(READ_REGISTER(UART3_EFR), ENHANCED_EN, true);
	WRITE_REGISTER(UART3_EFR, efrVal);

	// enable configuration mode a
	WRITE_REGISTER(UART3_LCR, 0x0080);
	bool uart3_tcr_TLRBackup = testbit(READ_REGISTER(UART3_MCR), 6);
	dword_t tlrVal = READ_REGISTER(UART3_MCR);
	tlrVal = setbit(tlrVal, 6, true);
	WRITE_REGISTER(UART3_MCR, tlrVal);

	// Set FIFO Triggers to 8 characters (see ref. p. 4731)
	// RX FIFO
	dword_t fcrVal = 0x0;
	fcrVal = setbit(fcrVal, 7, false);
	fcrVal = setbit(fcrVal, 6, false);

	// TX FIFO
	fcrVal = setbit(fcrVal, 5, true);
	fcrVal = setbit(fcrVal, 4, true);

	// FIFO_EN enables FIFO
	fcrVal = setbit(fcrVal, 0, false);
	fcrVal = setbit(fcrVal, 3, false);
	// Disable DMA for now
	WRITE_REGISTER(UART3_FCR, fcrVal);

	// enable configuration mode b
	WRITE_REGISTER(UART3_LCR, 0x00BF);

	// Restore UART_EFR ENHANCED_EN
	efrVal = setbit(efrVal, ENHANCED_EN, uart3_efrBackup);
	WRITE_REGISTER(UART3_EFR, efrVal);

	// enable configuration mode a
	WRITE_REGISTER(UART3_LCR, 0x0080);

	tlrVal = setbit(tlrVal, 6, uart3_tcr_TLRBackup);
	WRITE_REGISTER(UART3_MCR, tlrVal);
	WRITE_REGISTER(UART3_LCR, uart3_lcrBackup);
}

void OMAPUart::setBaudrate(int baudrate) {
	// Disable UART
	dword_t mdrVal = READ_REGISTER(UART3_MDR1);
	mdrVal = setbit(mdrVal, 0, true);
	mdrVal = setbit(mdrVal, 1, true);
	mdrVal = setbit(mdrVal, 2, true);
	WRITE_REGISTER(UART3_MDR1, mdrVal);

//	// Configuration mode b
	WRITE_REGISTER(UART3_LCR, 0x00BF);

	bool enhanced_en = testbit(READ_REGISTER(UART3_EFR), ENHANCED_EN);
	WRITE_REGISTER(UART3_EFR,
			setbit(READ_REGISTER(UART3_EFR), ENHANCED_EN, true));

	// register operational mode
	WRITE_REGISTER(UART3_LCR, 0x0000);

	// Clear UART_IER
	WRITE_REGISTER(UART3_IER, 0x0000);

	// Configuration mode b
	WRITE_REGISTER(UART3_LCR, 0x00BF);

	// Set clock values (115.2kbps for now)
	// DLLL 0x1A
	WRITE_REGISTER(UART3_DLL, 0x1A);
	// DLH 0x00
	WRITE_REGISTER(UART3_DLH, 0x00);

	// register operational mode
	WRITE_REGISTER(UART3_LCR, 0x0000);

	// TODO set interrupts here
	dword_t ierVal = 0;
	ierVal = setbit(ierVal, THR_IT, false);
	ierVal = setbit(ierVal, RHR_IT, true);
	ierVal = setbit(ierVal, RTS_IT, false);
	ierVal = setbit(ierVal, MODEM_STS_IT, false);
	WRITE_REGISTER(UART3_IER, ierVal);

//	 Configuration mode b
	WRITE_REGISTER(UART3_LCR, 0x00BF);

	// Restore ENHANCED_EN
	WRITE_REGISTER(UART3_EFR,
			setbit(READ_REGISTER(UART3_EFR), ENHANCED_EN, enhanced_en));

	WRITE_REGISTER(UART3_LCR, 0x0000);
//	REGISTER(UART3_LCR) = 0x3;
//	setbit(REGISTER(UART3_LCR), DIV_EN, false);
//	setbit(REGISTER(UART3_LCR), BREAK_EN, false);
//	// Parity (see ref. p. 4734)
//	setbit(REGISTER(UART3_LCR), PARITY_TYPE_2, false);
//	setbit(REGISTER(UART3_LCR), PARITY_TYPE_1, false);
//	setbit(REGISTER(UART3_LCR), PARITY_EN, false);
//	// Stop bit ( 1 bit)
//	setbit(REGISTER(UART3_LCR), NB_STOP, false);
//	// char length ( 8 bit)
//	setbit(REGISTER(UART3_LCR), CHAR_LENGTH_0, true);
//	setbit(REGISTER(UART3_LCR), CHAR_LENGTH_1, true);
	// Set mode to UART 16x mode
	WRITE_REGISTER(UART3_MDR1, 0x00);
	WRITE_REGISTER(UART3_LCR, 0x3);

	if (baudrate) {
	}
}

void OMAPUart::putc(char c) {
	while ( READ_REGISTER(UART3_TXFIFO_LVL) != 0x00) {
	}
	WRITE_REGISTER(UART3_THR, c);
	while ( READ_REGISTER(UART3_TXFIFO_LVL) != 0x00) {
	}

	// TODO fix fifo level
	if (c == '\n') {
		WRITE_REGISTER(UART3_THR, '\r');
		while ( READ_REGISTER(UART3_TXFIFO_LVL) != 0x00) {
		}
	}
}

void OMAPUart::print(char* string, int n) {
	for (int i = 0; i < n; i++) {
		putc(string[i]);
	}
}

void OMAPUart::print(char* string) {
	while(*(string) != 0) {
		putc(*string++);
	}
}

void OMAPUart::init() {
	resetUART();
	configure();
	setBaudrate(0);
}

char OMAPUart::readC() {
	while ( READ_REGISTER(UART3_RXFIFO_LVL) == 0x00) {
	}
	return (char) *(char*) (UART3_RHR);
}

void OMAPUart::setRXInterrupts(bool enable) {
	dword_t ierVal = 0;
	ierVal = setbit(ierVal, RHR_IT, enable);
	WRITE_REGISTER(UART3_IER, ierVal);
	if ( enable ) {
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_UART3,0xFF);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_UART3,true);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_UART3);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART3);
	} else {
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.disableInterrupt(HW_INT_UART3);

	}
}

void OMAPUart::handleInputInterrupt(dword_t arg) {
	char c = readC();

//	ackRXInterrupt();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_UART3);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART3);

	// Triggers the event of the super class
	EventHandling::EventHandler::pInstance.eventTriggered(&event_charReady, c);
}

MEMBER_OF_OSC_END
