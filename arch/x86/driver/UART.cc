/*
 * UART.cc
 *
 *  Created on: 15.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/UART.h>


#include <interrupts/InterruptDispatcher.h>
#include <interrupt_table.h>

#include <driver/InputOutputAggregator.h>

#include <driver/IOAPIC.h>

#define TAG "UART"

BEGIN_OSC_IMPLEMENTATION(UART,UART)

// Static instances for "com1-com4"
UART UART::mInstances[4] = { UART(cBaseCOM1), UART(cBaseCOM2), UART(cBaseCOM3),
		UART(cBaseCOM4) };


/**
 * Static trigger dependencies
 */
SECTION_CRITICAL_DATA OSC* UART::trigger_Interrupt_Deps[] = {&OSC_PREFIX(InputOutput)::InputOutputAggregator::pInstance, nullptr };

/**
 * Static triggers
 */
SECTION_CRITICAL_DATA DEFINE_TRIGGER(UART, handleInterrupt, UART::mInstances[0], trigger_Interrupt_Deps, CONFIG_PRIORITY_UART - 1);

UART::UART(uint16_t portBase):  mDataLowBaudPort(
				portBase + cDataRegisterLowBaudOffset), mInterruptEnableHighBaudPort(
				portBase + cInterruptEnableHighBaudOffset), mInterruptIdentificationFifoControlPort(
				portBase + cInterruptIdentificationRegisterOffset), mLineControlPort(
				portBase + cLineControlRegisterOffset), mLineStatusPort(
				portBase + cLineStatusRegisterOffset), mModemControlPort(
				portBase + cModemControlRegisterOffset), mModemStatusPort(
				portBase + cModemStatusRegisterOffset) {
	InterruptDispatcher::pInstance.registerEvent(HW_INT_UART_1, &trigger_handleInterrupt);
}

DEFINE_TRIGGER_FUNC(UART, handleInterrupt) {
	// Read control port
	uint8_t regVal = mInterruptIdentificationFifoControlPort.readByte();

	// Check if interrupt is pending
	if (!((IIR_t*)&regVal)->interruptPending) {
		// Read line status register
		regVal = mLineStatusPort.readByte();
		OSC_PREFIX(InputOutput)::InputOutputAggregator::pInstance.handleInputInterrupt(readC());
	}
}

uint8_t UART::readC() {
	uint8_t regVal = mLineStatusPort.readByte();
	while (((LSR_t*) &regVal)->dataReady == 0x0) {
		regVal = (uint8_t) mLineStatusPort.readByte();
	}
	return mDataLowBaudPort.readByte();


}

void UART::print(char* string, int n) {
	for (int i = 0; i < n; i++) {
		putc(string[i]);
	}
}

void UART::print(char* string) {
	while (*(string) != 0) {
		putc(*string++);
	}
}

void UART::putc(char c) {
	int reg = mLineStatusPort.readByte();
	LSR_t* lsr = (LSR_t*) &reg;
	while (!(lsr->emptyTransmitterHoldingRegisters == 0x1)) {
		reg = (uint8_t) mLineStatusPort.readByte();
	}
	mDataLowBaudPort.writeByte((uint8_t) c);

	if(c == '\n') {
		putc('\r');
	}
}

void UART::resetController() {
	// Disable all interrupts
	IER_t ier_reset;
	ier_reset.lowPowerMode = 0x0;
	ier_reset.modemStatusInterrupt = 0x0;
	ier_reset.receivedDataInterrupt = 0x0;
	ier_reset.receiverLineStatusInterrupt = 0x0;
	ier_reset.sleepMode = 0x0;
	ier_reset.transmitterHoldingRegisterEmptyInterrupt = 0x0;
	mDataLowBaudPort.writeByte(*(uint8_t*)&ier_reset);

	// Disable DLA
	LCR_t lcr_reset;
	lcr_reset.breakEnable = 0x0;
	lcr_reset.divisorLatchAccess = 0x1;
	lcr_reset.parity = 0x0;
	lcr_reset.stopBit = 0x0;
	lcr_reset.wordLength = 0x0;
	mLineControlPort.writeByte(*(uint8_t*)&lcr_reset);

	// Disable and clear FIFO leftovers
	FCR_t fcr_reset;
	fcr_reset.enableFifo = 0x0;
	fcr_reset.clearReceiveFifo = 0x1;
	fcr_reset.clearTransmitFifo = 0x1;
	fcr_reset.dmaSelectMode = 0x0;
	fcr_reset.enable64byteFifo = 0x0;
	fcr_reset.triggerLevel = 0x0; // 14 byte
	mInterruptIdentificationFifoControlPort.writeByte(*(uint8_t*)&fcr_reset);

	// Disable IRQ, RTS and DTR
	MCR_t mcr_reset;
	mcr_reset.dataTerminalReady = 0x0;
	mcr_reset.requestToSend = 0x0;
	mcr_reset.auxiliaryOutput_1 = 0x0;
	mcr_reset.auxiliaryOutput_2 = 0x0;
	mcr_reset.loopBackMode = 0x0;
	mcr_reset.autoFlowControlEnabled = 0x0;
	mModemControlPort.writeByte(*(uint8_t*)&mcr_reset);

	// Configure baud rate
	LCR_t lcr_init;
	lcr_init.breakEnable = 0x0;
	lcr_init.divisorLatchAccess = 0x1;
	lcr_init.parity = 0x0;
	lcr_init.stopBit = 0x0;
	lcr_init.wordLength = 0x0;
	mLineControlPort.writeByte(*(uint8_t*)&lcr_init);

	// Set baud rate
	mDataLowBaudPort.writeByte((uint8_t) 0x01); // baud rate 115200
	mInterruptEnableHighBaudPort.writeByte((uint8_t) 0x00);

	// Set word length = 8 bits
	lcr_init.divisorLatchAccess = 0x0;
	lcr_init.wordLength = 0x3;
	mLineControlPort.writeByte(*(uint8_t*)&lcr_init);

	// Enable and clear FIFO
	FCR_t fcr_init;
	fcr_init.enableFifo = 0x1;
	fcr_init.clearReceiveFifo = 0x1;
	fcr_init.clearTransmitFifo = 0x1;
	fcr_init.dmaSelectMode = 0x0;
	fcr_init.enable64byteFifo = 0x0;
	fcr_init.triggerLevel = 0x3; // 14 byte
	mInterruptIdentificationFifoControlPort.writeByte(*(uint8_t*)&fcr_init);

	// Enable IRQ, RTS and DTR
	MCR_t mcr_init;
	mcr_init.dataTerminalReady = 0x1;
	mcr_init.requestToSend = 0x1;
	mcr_init.auxiliaryOutput_1 = 0x0;
	mcr_init.auxiliaryOutput_2 = 0x1;
	mcr_init.loopBackMode = 0x0;
	mcr_init.autoFlowControlEnabled = 0x0;
	mModemControlPort.writeByte(*(uint8_t*)&mcr_init);

	// Enable DataAvailableInterrupt
	IER_t ier_init;
	ier_init.receivedDataInterrupt = 0x1;
	mInterruptEnableHighBaudPort.writeByte(*(uint8_t*)&ier_init);

	IOAPIC::mInstances[0].setInterruptMask(IOAPIC::INTERRUPT_INPUT_UART_1, IOAPIC::UNMASKED);
	DEBUG_STREAM(TAG,"UART IRQ enabled");
}

END_OSC_IMPLEMENTATION
