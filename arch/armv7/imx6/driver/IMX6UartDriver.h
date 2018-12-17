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

#include <eventhandling/Trigger.h>

#define UART_BASE_1 0x02020000

#define UART_URXD(UART_BASE) UART_BASE + 0x0
#define UART_UTXD(UART_BASE) UART_BASE + 0x40
#define UART_UCR1(UART_BASE) UART_BASE + 0x80
#define UART_UCR2(UART_BASE) UART_BASE + 0x84
#define UART_UCR3(UART_BASE) UART_BASE + 0x88
#define UART_UCR4(UART_BASE) UART_BASE + 0x8C
#define UART_UFCR(UART_BASE) UART_BASE + 0x90
#define UART_USR1(UART_BASE) UART_BASE + 0x94
#define UART_USR2(UART_BASE) UART_BASE + 0x98
#define UART_UESC(UART_BASE) UART_BASE + 0x9C
#define UART_UTIM(UART_BASE) UART_BASE + 0xA0
#define UART_UBIR(UART_BASE) UART_BASE + 0xA4
#define UART_UBMR(UART_BASE) UART_BASE + 0xA8
#define UART_UBRC(UART_BASE) UART_BASE + 0xAC
#define UART_ONEMS(UART_BASE) UART_BASE + 0xB0
#define UART_UTS(UART_BASE) UART_BASE + 0xB4
#define UART_UMCR(UART_BASE) UART_BASE + 0xB8


/* Receive ready interrupt enable bit*/
#define UCR1_RRDYEN (1 << 9)

/* UCR2 bits */
/* Software reset */
#define UCR2_SRST (1 << 0)
/* Receiver enable */
#define UCR2_RXEN (1 << 1)
/* Transmitter enable */
#define UCR2_TXEN (1 << 2)
/* Word size (1 = 8 bit) */
#define UCR2_WS (1<< 5)
/* Stop bit 0 = 1 stop bit, 1 = 2 stop bits */
#define UCR2_STPB (1 << 6)
/* Parity odd/even 0=even, 1=odd */
#define UCR2_PROE (1 << 7)
/* Parity enable */
#define UCR2_PREN (1 << 8)
/* Ignore RTS Pin */
#define UCR2_IRTS (1 << 14)

/* UCR3 bits */
#define UCR3_RXDMUXSEL (1 << 2)

/* Receiver Data Ready interrupt enable */
#define UCR4_DREN (1 << 1);

/* Software reset complete */
#define UTS_SOFTRST (1 << 0)

/* Transmitter ready */
#define USR1_TRDY (1 << 13)
/* Receiver ready */
#define USR1_RRDY (1 << 9)

MEMBER_OF_OSC_BEGIN(InputOutput)

class IMX6Uart : public GenericIO {
public:
	IMX6Uart();

	void resetUART();
	void configure();

	char readC();

	void print(char* string, int n);
	void print(char *string);

	void init(void);

	void putc(char c);

	void setRXInterrupts(bool enable);

	void handleInputInterrupt(dword_t arg);

	static IMX6Uart pInstance;
private:

	char mLastChar;

	dword_t mUFCR;
	dword_t mUBIR;
	dword_t mUBMR;

	static OSC* trigger_Interrupt_Deps[];
	static EventHandling::Trigger trigger_Interrupt;
};

MEMBER_OF_OSC_END

#endif /* OMAPUART_H_ */
