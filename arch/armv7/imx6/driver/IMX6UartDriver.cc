/*
 * OMAPUart.cpp
 *
 *  Created on: 21.07.2014
 *      Author: hb
 */

#include <common/memreg.h>
#include <common/bitmanipulation.h>
#include <interrupts/InterruptDispatcher.h>

/* ARM Helper macros */
#include <common/armhelper.h>

/* Include interrupt controller to control interrupt flags */
#include <driver/ARMGIC.h>

#include "IMX6UartDriver.h"

#include <eventhandling/EventHandler.h>

#include <testcomponents/UARTConsole.h>

MEMBER_OF_OSC_BEGIN(InputOutput)

SECTION_CRITICAL_DATA OSC* IMX6Uart::trigger_Interrupt_Deps[] = { nullptr };
SECTION_CRITICAL_DATA EventHandling::Trigger IMX6Uart::trigger_Interrupt((OSC*) &IMX6Uart::pInstance, (void (OSC::*)(dword_t))&IMX6Uart::handleInputInterrupt, (OSC**)&IMX6Uart::trigger_Interrupt_Deps, CONFIG_PRIORITY_UART);

IMX6Uart IMX6Uart::pInstance;

IMX6Uart::IMX6Uart() :
		GenericIO(this), mLastChar(0) {
//
//	DEBUG_STREAM_DEVICE<< "Hello UART" << endl;
//
//	DEBUG_STREAM_DEVICE << "UCR1: " << hex << READ_REGISTER(UART_UCR1(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UCR2: " << hex << READ_REGISTER(UART_UCR2(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UCR3: " << hex << READ_REGISTER(UART_UCR3(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UCR4: " << hex << READ_REGISTER(UART_UCR4(UART_BASE_1)) << endl;
//
//	DEBUG_STREAM_DEVICE << "UFCR: " << hex << READ_REGISTER(UART_UFCR(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "USR1: " << hex << READ_REGISTER(UART_USR1(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "USR2: " << hex << READ_REGISTER(UART_USR2(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UESC: " << hex << READ_REGISTER(UART_UESC(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UTIM: " << hex << READ_REGISTER(UART_UTIM(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UBIR: " << hex << READ_REGISTER(UART_UBIR(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UBMR: " << hex << READ_REGISTER(UART_UBMR(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UBRC: " << hex << READ_REGISTER(UART_UBRC(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "ONEMS: " << hex << READ_REGISTER(UART_ONEMS(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UTS: " << hex << READ_REGISTER(UART_UTS(UART_BASE_1)) << endl;
//	DEBUG_STREAM_DEVICE << "UMCR: " << hex << READ_REGISTER(UART_UMCR(UART_BASE_1)) << endl;

	mUFCR = READ_REGISTER(UART_UFCR(UART_BASE_1));
	mUBIR = READ_REGISTER(UART_UBIR(UART_BASE_1));
	mUBMR = READ_REGISTER(UART_UBMR(UART_BASE_1));

	init();
	InterruptDispatcher::pInstance.registerEvent(HW_INT_UART1, &trigger_Interrupt);
}

void IMX6Uart::resetUART() {
	// Set software reset
	dword_t ucr2 = READ_REGISTER(UART_UCR2(UART_BASE_1));
	ucr2 &= ~(UCR2_SRST);
	WRITE_REGISTER(UART_UCR2(UART_BASE_1), ucr2);

	// Wait for reset complete
	while ( READ_REGISTER(UART_UTS(UART_BASE_1)) & UTS_SOFTRST) {
	}
}

void IMX6Uart::configure() {
	// Enable the UART controller
	dword_t ucr1 = 0x1;
	WRITE_REGISTER(UART_UCR1(UART_BASE_1), ucr1);

	dword_t ucr2 = UCR2_SRST | UCR2_RXEN | UCR2_TXEN | UCR2_WS | UCR2_IRTS;
	WRITE_REGISTER(UART_UCR2(UART_BASE_1), ucr2);

	dword_t ucr3 = UCR3_RXDMUXSEL;
	WRITE_REGISTER(UART_UCR3(UART_BASE_1), ucr3);

	/* Set the baudrate to 115200kbps */
	WRITE_REGISTER(UART_UFCR(UART_BASE_1), mUFCR);
	WRITE_REGISTER(UART_UBIR(UART_BASE_1), mUBIR);
	WRITE_REGISTER(UART_UBMR(UART_BASE_1), mUBMR);
}

void IMX6Uart::putc(char c) {
	// Check if TRDY is set
	while (!(READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_TRDY)) {
	}

	// Send the char to the register
	WRITE_REGISTER(UART_UTXD(UART_BASE_1), c);

	// Wait for it to clear again.
	while (!(READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_TRDY)) {
	}

	// If a newline is send, also send return
	if (c == '\n') {
		WRITE_REGISTER(UART_UTXD(UART_BASE_1), '\r');
		while (!(READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_TRDY)) {
		}
	}

}

void IMX6Uart::print(char* string, int n) {
	for (int i = 0; i < n; i++) {
		putc(string[i]);
	}
}

void IMX6Uart::print(char* string) {
	while (*(string) != 0) {
		putc(*string++);
	}
}

void IMX6Uart::init() {
	resetUART();
	configure();
}

char IMX6Uart::readC() {
	while (!(READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_RRDY)) {
	}

	return READ_REGISTER(UART_URXD(UART_BASE_1));
}

void IMX6Uart::setRXInterrupts(bool enable) {
	if (enable) {
		while ( READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_RRDY) {
			READ_REGISTER(UART_URXD(UART_BASE_1));
		}

		dword_t ucr4 = READ_REGISTER(UART_UCR4(UART_BASE_1));
		ucr4 |= (1 << 10) | 0x1;
		WRITE_REGISTER(UART_UCR4(UART_BASE_1), ucr4);

		/* Set rx trigger level */
		dword_t ufcr = READ_REGISTER(UART_UFCR(UART_BASE_1));
		ufcr |= 0x1;
		WRITE_REGISTER(UART_UFCR(UART_BASE_1), ufcr);

		dword_t ucr1 = READ_REGISTER(UART_UCR1(UART_BASE_1));
		ucr1 |= UCR1_RRDYEN;
		WRITE_REGISTER(UART_UCR1(UART_BASE_1), ucr1);

		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.setInterruptTarget(HW_INT_UART1, 0xF);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART1);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.configureInterruptHandling(HW_INT_UART1, true);

		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_UART1);
		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART1);
	} else {
		dword_t ucr1 = READ_REGISTER(UART_UCR1(UART_BASE_1));
		ucr1 &= ~UCR1_RRDYEN;
		WRITE_REGISTER(UART_UCR1(UART_BASE_1), ucr1);

		OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.disableInterrupt(HW_INT_UART1);
	}
}

void IMX6Uart::handleInputInterrupt(dword_t arg) {
	while ((READ_REGISTER(UART_USR1(UART_BASE_1)) & USR1_RRDY)) {
		char c = readC();
		EventHandling::EventHandler::pInstance.eventTriggered(&event_charReady, c);
	}

//	ackRXInterrupt();
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.clearPending(HW_INT_UART1);
	OSC_PREFIX(ARMGIC)::ARMGIC::pInstance.enableInterrupt(HW_INT_UART1);
}

MEMBER_OF_OSC_END
