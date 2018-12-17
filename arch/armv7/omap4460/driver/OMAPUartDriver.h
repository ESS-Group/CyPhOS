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

// PandaBoards UART Registers ( see operational modes, OMAP Reference p. 4695)

// Transmit register
#define UART3_THR 0x48020000
// Receive register
#define UART3_RHR 0x48020000

// LSB for baudrate divisor
#define UART3_DLL 0x48020000
// MSB for baudrate divisor
#define UART3_DLH 0x48020004
#define UART3_IIR 0x48020008
// Line-control register
#define UART3_LCR 0x4802000C
// Modem control register
#define UART3_MCR 0x48020010
// Line status register
#define UART3_LSR 0x48020014
#define UART3_MSR 0x48020018
#define UART3_TLR 0x4802001C
// Mode control register
#define UART3_MDR1 0x48020020
#define UART3_MDR2 0x48020024


// Interrupt enable register
#define UART3_ISR 0x48020004
// FIFO control register
#define UART3_FCR 0x48020008
// Enhanced feature register
#define UART3_EFR 0x48020008


// System configuration register
#define UART3_SYSC 0x48020054
// System status register
#define UART3_SYSS 0x48020058

// Interrupt enable register
#define UART3_IER 0x48020004

// FIFO Lvls
#define UART3_RXFIFO_LVL 0x48020064
#define UART3_TXFIFO_LVL 0x48020068

// Bit definitions
#define SOFTRESET 1
#define RESETDONE 0
#define ENHANCED_EN 4
#define TCR_TLR 6

#define RX_FIFO_TRIG_0 6
#define RX_FIFO_TRIG_1 7
#define RX_FIFO_TRIG_DMA_0 4
#define RX_FIFO_TRIG_DMA_1 5
#define RX_FIFO_TRIG_DMA_2 6
#define RX_FIFO_TRIG_DMA_3 7
#define TX_FIFO_TRIG_0 4
#define TX_FIFO_TRIG_1 5
#define TX_FIFO_TRIG_DMA_0 0
#define TX_FIFO_TRIG_DMA_1 1
#define TX_FIFO_TRIG_DMA_2 2
#define DMA_MODE 3
#define FIFO_ENABLE 0
#define RX_TRIG_GRANU1 7
#define TX_TRIG_GRANU1 6
#define DMA_MODE_2_0 1
#define DMA_MODE_2_1 2
#define DMA_MODE_CTRL 0

#define MODE_SELECT_0 0
#define MODE_SELECT_1 1
#define MODE_SELECT_2 2



enum IER_BITS {
	RHR_IT = 0,
	THR_IT,
	LINE_STS_IT,
	MODEM_STS_IT,
	SLEEP_MODE,
	XOFF_IT,
	RTS_IT,
	CTS_IT
};

enum LCR_BITS {
	CHAR_LENGTH_0 = 0,
	CHAR_LENGTH_1,
	NB_STOP,
	PARITY_EN,
	PARITY_TYPE_1,
	PARITY_TYPE_2,
	BREAK_EN,
	DIV_EN
};



MEMBER_OF_OSC_BEGIN(InputOutput)

class OMAPUart : public GenericIO {

public:
	OMAPUart();

	void resetUART();
	void configure();
	void setBaudrate(int baudrate);

	char readC();

	void print(char* string, int n);
	void print(char *string);

	void init(void);

	void putc(char c);

	void setRXInterrupts(bool enable);

	void handleInputInterrupt(dword_t arg);


	static OMAPUart pInstance;

	static OSC* trigger_Interrupt_Deps[];
	static EventHandling::Trigger trigger_Interrupt;
private:
	char mLastChar;
};

MEMBER_OF_OSC_END

#endif /* OMAPUART_H_ */
