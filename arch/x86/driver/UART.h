/*
 * UART.h
 *
 *  Created on: 15.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_UART_H_
#define ARCH_X86_DRIVER_UART_H_

#include <component/OSC.h>

#include <driver/GenericIO.h>

#include <common/IOPort.h>

BEGIN_DECLARE_OSC(InputOutput,UART)

public:
	struct IER_t // Interrupt Enable Register
	{
		uint8_t receivedDataInterrupt :1,
				transmitterHoldingRegisterEmptyInterrupt :1,
				receiverLineStatusInterrupt :1, modemStatusInterrupt :1,
				sleepMode :1, lowPowerMode :1, reserved :2;
	}__attribute__((packed));

	struct IIR_t // Interrupt Identification Register
	{
		uint8_t interruptPending :1, receivedInterrupt :3, reserved :1,
				fifo64byte :1, fifo :2;
	}__attribute__((packed));

	struct FCR_t // FIFO Control Register
	{
		uint8_t enableFifo :1, clearReceiveFifo :1, clearTransmitFifo :1,
				dmaSelectMode :1, reserved :1, enable64byteFifo :1,
				triggerLevel :2;
	}__attribute__((packed));

	struct LCR_t // Line Control Register
	{
		uint8_t wordLength :2, stopBit :1, parity :3, breakEnable :1,
				divisorLatchAccess :1;
	}__attribute__((packed));

	struct MCR_t // Modem Control Register
	{
		uint8_t dataTerminalReady :1, requestToSend :1,
				auxiliaryOutput_1 :1, auxiliaryOutput_2 :1, loopBackMode :1,
				autoFlowControlEnabled :1, reserved :2;
	}__attribute__((packed));

	struct LSR_t // Line Status Register
	{
		uint8_t dataReady :1, overrunError :1, parityError :1,
				framingError :1, breakInterrupt :1,
				emptyTransmitterHoldingRegisters :1,
				emptyDataHoldingRegisters :1, errorInFifo :1;
	}__attribute__((packed));

	struct MSR_t // Modem Status Register
	{
		uint8_t deltaDataClearToSend :1, deltaDataSetReady :1,
				trailingEdgeRingIndicator :1, deltaDataCarrierDetect :1,
				clearToSend :1, dataSetReady :1, ringIndicator :1,
				carrierDetect :1;
	}__attribute__((packed));

	/**
	 * Constructor.
	 * @param portBase IOPort base from constexpr
	 */
	UART(uint16_t portBase);

	/**
	 * Outputs a char to the given UART device
	 * @param c
	 */
	void putc(char c);

	/**
	 * Prints a string with length n
	 * @param string String to print
	 * @param n Length limitiation
	 */
	void print(char* string, int n);

	/**
	 * Prints a null terminated string
	 * @param string to print
	 */
	void print(char *string);

	void handleInputInterrupt(dword_t arg);

	/**
	 * Static instances
	 */
	static UART mInstances[4];

	void resetController();
private:
	/**
	 * IOPort base addresses
	 */
	static constexpr uint16_t cBaseCOM1 = 0x3F8;
	static constexpr uint16_t cBaseCOM2 = 0x2F8;
	static constexpr uint16_t cBaseCOM3 = 0x3E8;
	static constexpr uint16_t cBaseCOM4 = 0x2E8;

	/**
	 * IO register offsets
	 */
	static constexpr uint8_t cDataRegisterLowBaudOffset = 0x0;
	static constexpr uint8_t cInterruptEnableHighBaudOffset = 0x1;
	static constexpr uint8_t cInterruptIdentificationRegisterOffset = 0x2;
	static constexpr uint8_t cFifoControlRegisterOffset = 0x2;
	static constexpr uint8_t cLineControlRegisterOffset = 0x3;
	static constexpr uint8_t cModemControlRegisterOffset = 0x4;
	static constexpr uint8_t cLineStatusRegisterOffset = 0x5;
	static constexpr uint8_t cModemStatusRegisterOffset = 0x6;

	/**
	 * IOPorts for control
	 */
	const IOPort mDataLowBaudPort;
	const IOPort mInterruptEnableHighBaudPort;
	const IOPort mInterruptIdentificationFifoControlPort;
	const IOPort mLineControlPort;
	const IOPort mLineStatusPort;
	const IOPort mModemControlPort;
	const IOPort mModemStatusPort;

	uint8_t readC();


	DECLARE_TRIGGER(handleInterrupt);
	static OSC* trigger_Interrupt_Deps[];
};

END_DECLARE_OSC

#endif /* ARCH_X86_DRIVER_UART_H_ */
