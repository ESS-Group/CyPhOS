/*
 * OSC_OMAPUart.h
 *
 *  Created on: 21.07.2014
 *      Author: hb
 */

#ifndef OSC_OMAPUART_H_
#define OSC_OMAPUART_H_

#include <common/types.h>

#include <component/InterruptOSC.h>

#include <driver/GenericIO.h>

// Exynos UART Registers ( see operational modes, Exynos 4412 Reference p. 696)

/* UART 1 is the output of the odroid board */
#define EXYNOS_UART_BASE 0x13800000
#define EXYNOS_UART_ADDRESS 0x10000 + EXYNOS_UART_BASE

// Line control
#define ULCON EXYNOS_UART_ADDRESS + 0x00
// Control
#define UCON EXYNOS_UART_ADDRESS + 0x04
// FIFO control
#define UFCON EXYNOS_UART_ADDRESS + 0x08
// Modem control
#define UMCON EXYNOS_UART_ADDRESS + 0x0C
// Tx/Rx status
#define UTRSTAT EXYNOS_UART_ADDRESS + 0x10
// Rx error status
#define UERSTAT EXYNOS_UART_ADDRESS + 0x14
// FIFO status
#define UFSTAT EXYNOS_UART_ADDRESS + 0x18
// Modem status
#define UMSTAT EXYNOS_UART_ADDRESS + 0x1C
// Transmit buffer
#define UTXH EXYNOS_UART_ADDRESS + 0x20
// Receive buffer
#define URXH EXYNOS_UART_ADDRESS + 0x24
// Baud rate divisor
#define UBRDIV EXYNOS_UART_ADDRESS + 0x28
// Divisor fractional value
#define UFRACVAL EXYNOS_UART_ADDRESS + 0x2C
// Interrupt pending
#define UINTP EXYNOS_UART_ADDRESS + 0x30
// Interrupt source pending
#define UINTSP EXYNOS_UART_ADDRESS + 0x34
// Interrupt mask
#define UINTM EXYNOS_UART_ADDRESS + 0x38

#define EXYNOS_UART_BUFFER_SIZE 64


/* Bitmasks */
#define ULCON_INFRARED_MODE 1 << 6
#define ULCON_PARITY_MODE 1 << 3
#define ULCON_NO_STOP_BITS 1 << 2
#define ULCON_WORD_LENGTH 1 << 0

#define UTRSTAT_RECEIVE_BUFFER_READY 0x1
#define UTRSTAT_TRANSMITTER_EMPTY 0x4


MEMBER_OF_OSC_BEGIN(InputOutput)
class ExynosUart : public GenericIO {
public:
	ExynosUart();

	void resetUART();
	void configure();
	void setBaudrate(uint32_t baudrate);
	void init(void);
	void setRXInterrupts(bool enabled);
	void ackRXInterrupt();

	char readC();

	void print(char* string, int n);
	void print(char *string);


//	static void printDebug(char *tag, char *text);
	void putc(char c);

	void handleInputInterrupt(dword_t arg);

	/* Static variable instance */
	static ExynosUart pInstance;

	static OSC* trigger_Interrupt_Deps[];
	static EventHandling::Trigger trigger_Interrupt;
private:

	char pBuffer[EXYNOS_UART_BUFFER_SIZE];
	uint32_t pBufferWritePointer;
	uint32_t pBufferReadPointer;
};

MEMBER_OF_OSC_END

#endif /* OMAPUART_H_ */
