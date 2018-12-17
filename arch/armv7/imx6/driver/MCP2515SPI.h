/*
 * MCP2515SPI.h
 *
 *  Created on: 10.12.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_MCP2515SPI_H_
#define ARCH_ARMV7_IMX6_DRIVER_MCP2515SPI_H_

#include <driver/IMX6ECSPI.h>

#include <driver/CanTXBuffer.h>
#include <driver/CanRXBuffer.h>

#include <eventhandling/Trigger.h>
#include <driver/CANEvent.h>

#include <component/OSC.h>

#define MCP2515_INSTRUCTION_RESET 		0xC0
#define MCP2515_INSTRUCTION_READ 		0x3
#define MCP2515_INSTRUCTION_WRITE 		0x2
#define MCP2515_INSTRUCTION_READ_RX 	0x90
#define MCP2515_INSTRUCTION_LOAD_TX 	0x40
#define MCP2515_INSTRUCTION_RTS 		0x80
#define MCP2515_INSTRUCTION_READ_STATUS 0xA0
#define MCP2515_INSTRUCTION_RX_STATUS 	0xB0
#define MCP2515_INSTRUCTION_BIT_MODIFY 	0x5

#define MCP2515_REGISTER_TXB0CTRL 0x30
#define MCP2515_REGISTER_TXB0SIDH 0x31
#define MCP2515_REGISTER_TXB0SIDL 0x32
#define MCP2515_REGISTER_TXB0DLC  0x35
#define MCP2515_REGISTER_TXB0D0	  0x36

#define MCP2515_REGISTER_TXB1CTRL 0x40
#define MCP2515_REGISTER_TXB1SIDH 0x41
#define MCP2515_REGISTER_TXB1SIDL 0x42
#define MCP2515_REGISTER_TXB1DLC  0x45
#define MCP2515_REGISTER_TXB1D0	  0x46

#define MCP2515_REGISTER_TXB2CTRL 0x50
#define MCP2515_REGISTER_TXB2SIDH 0x51
#define MCP2515_REGISTER_TXB2SIDL 0x52
#define MCP2515_REGISTER_TXB2DLC  0x55
#define MCP2515_REGISTER_TXB2D0	  0x56

#define MCP2515_REGISTER_RXB0CTRL 0x60
#define MCP2515_REGISTER_RXB0SIDH 0x61
#define MCP2515_REGISTER_RXB0SIDL 0x62
#define MCP2515_REGISTER_RXB0DLC  0x65
#define MCP2515_REGISTER_RXB0D0	  0x66

#define MCP2515_REGISTER_RXB1CTRL 0x70
#define MCP2515_REGISTER_RXB1SIDH 0x71
#define MCP2515_REGISTER_RXB1SIDL 0x72
#define MCP2515_REGISTER_RXB1DLC  0x75
#define MCP2515_REGISTER_RXB1D0	  0x76

#define MCP2515_REGISTER_RXF0SIDH 0x00
#define MCP2515_REGISTER_RXF0SIDL 0x01
#define MCP2515_REGISTER_RXF1SIDH 0x04
#define MCP2515_REGISTER_RXF1SIDL 0x05
#define MCP2515_REGISTER_RXF2SIDH 0x08
#define MCP2515_REGISTER_RXF2SIDL 0x09
#define MCP2515_REGISTER_RXF3SIDH 0x10
#define MCP2515_REGISTER_RXF3SIDL 0x11
#define MCP2515_REGISTER_RXF4SIDH 0x14
#define MCP2515_REGISTER_RXF4SIDL 0x15
#define MCP2515_REGISTER_RXF5SIDH 0x18
#define MCP2515_REGISTER_RXF5SIDL 0x19

#define MCP2515_REGISTER_RXM0SIDH 0x20
#define MCP2515_REGISTER_RXM0SIDL 0x21
#define MCP2515_REGISTER_RXM1SIDH 0x24
#define MCP2515_REGISTER_RXM1SIDL 0x25

#define MCP2515_REGISTER_CANCTRL 0x0F
#define MCP2515_REGISTER_CANSTAT 0x0E

#define MCP2515_REGISTER_CANINTE 0x2B
#define MCP2515_REGISTER_CANINTF 0x2C

#define MCP2515_REGISTER_TEC 0x1C
#define MCP2515_REGISTER_REC 0x1D

#define MCP2515_NORMAL_MODE 		0x0
#define MCP2515_SLEEP_MODE			0x1
#define MCP2515_LOOPBACK_MODE		0x2
#define MCP2515_LISTEN_ONLY_MODE	0x3
#define MCP2515_CONFIGURATION_MODE	0x4

#define MCP2515_RXMODE_NO_MASK_NO_FILTERS	0x3
#define MCP2515_RXMODE_EXTENDED_IDS			0x2
#define MCP2515_RXMODE_STANDARD_IDS			0x1
#define MCP2515_RXMODE_ALL_IDS				0x0

/*
 *  Speed 8M
 */
#define MCP_8MHz_1000kBPS_CFG1 (0x00)
#define MCP_8MHz_1000kBPS_CFG2 (0x80)
#define MCP_8MHz_1000kBPS_CFG3 (0x80)

#define MCP_8MHz_500kBPS_CFG1 (0x00)
#define MCP_8MHz_500kBPS_CFG2 (0x90)
#define MCP_8MHz_500kBPS_CFG3 (0x82)

#define MCP_8MHz_250kBPS_CFG1 (0x00)
#define MCP_8MHz_250kBPS_CFG2 (0xB1)
#define MCP_8MHz_250kBPS_CFG3 (0x85)

#define MCP_8MHz_200kBPS_CFG1 (0x00)
#define MCP_8MHz_200kBPS_CFG2 (0xB4)
#define MCP_8MHz_200kBPS_CFG3 (0x86)

#define MCP_8MHz_125kBPS_CFG1 (0x01)
#define MCP_8MHz_125kBPS_CFG2 (0xB1)
#define MCP_8MHz_125kBPS_CFG3 (0x85)

#define MCP_8MHz_100kBPS_CFG1 (0x01)
#define MCP_8MHz_100kBPS_CFG2 (0xB4)
#define MCP_8MHz_100kBPS_CFG3 (0x86)

#define MCP_8MHz_80kBPS_CFG1 (0x01)
#define MCP_8MHz_80kBPS_CFG2 (0xBF)
#define MCP_8MHz_80kBPS_CFG3 (0x87)

#define MCP_8MHz_50kBPS_CFG1 (0x03)
#define MCP_8MHz_50kBPS_CFG2 (0xB4)
#define MCP_8MHz_50kBPS_CFG3 (0x86)

#define MCP_8MHz_40kBPS_CFG1 (0x03)
#define MCP_8MHz_40kBPS_CFG2 (0xBF)
#define MCP_8MHz_40kBPS_CFG3 (0x87)

#define MCP_8MHz_33k3BPS_CFG1 (0x47)
#define MCP_8MHz_33k3BPS_CFG2 (0xE2)
#define MCP_8MHz_33k3BPS_CFG3 (0x85)

#define MCP_8MHz_31k25BPS_CFG1 (0x07)
#define MCP_8MHz_31k25BPS_CFG2 (0xA4)
#define MCP_8MHz_31k25BPS_CFG3 (0x84)

#define MCP_8MHz_20kBPS_CFG1 (0x07)
#define MCP_8MHz_20kBPS_CFG2 (0xBF)
#define MCP_8MHz_20kBPS_CFG3 (0x87)

#define MCP_8MHz_10kBPS_CFG1 (0x0F)
#define MCP_8MHz_10kBPS_CFG2 (0xBF)
#define MCP_8MHz_10kBPS_CFG3 (0x87)

#define MCP_8MHz_5kBPS_CFG1 (0x1F)
#define MCP_8MHz_5kBPS_CFG2 (0xBF)
#define MCP_8MHz_5kBPS_CFG3 (0x87)

/*
 *  speed 16M
 */
#define MCP_16MHz_1000kBPS_CFG1 (0x00)
#define MCP_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP_16MHz_1000kBPS_CFG3 (0x82)

#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)

#define MCP_16MHz_250kBPS_CFG1 (0x41)
#define MCP_16MHz_250kBPS_CFG2 (0xF1)
#define MCP_16MHz_250kBPS_CFG3 (0x85)

#define MCP_16MHz_200kBPS_CFG1 (0x01)
#define MCP_16MHz_200kBPS_CFG2 (0xFA)
#define MCP_16MHz_200kBPS_CFG3 (0x87)

#define MCP_16MHz_125kBPS_CFG1 (0x03)
#define MCP_16MHz_125kBPS_CFG2 (0xF0)
#define MCP_16MHz_125kBPS_CFG3 (0x86)

#define MCP_16MHz_100kBPS_CFG1 (0x03)
#define MCP_16MHz_100kBPS_CFG2 (0xFA)
#define MCP_16MHz_100kBPS_CFG3 (0x87)

#define MCP_16MHz_80kBPS_CFG1 (0x03)
#define MCP_16MHz_80kBPS_CFG2 (0xFF)
#define MCP_16MHz_80kBPS_CFG3 (0x87)

#define MCP_16MHz_50kBPS_CFG1 (0x07)
#define MCP_16MHz_50kBPS_CFG2 (0xFA)
#define MCP_16MHz_50kBPS_CFG3 (0x87)

#define MCP_16MHz_40kBPS_CFG1 (0x07)
#define MCP_16MHz_40kBPS_CFG2 (0xFF)
#define MCP_16MHz_40kBPS_CFG3 (0x87)

#define MCP_16MHz_33k3BPS_CFG1 (0x4E)
#define MCP_16MHz_33k3BPS_CFG2 (0xF1)
#define MCP_16MHz_33k3BPS_CFG3 (0x85)

#define MCP_16MHz_20kBPS_CFG1 (0x0F)
#define MCP_16MHz_20kBPS_CFG2 (0xFF)
#define MCP_16MHz_20kBPS_CFG3 (0x87)

#define MCP_16MHz_10kBPS_CFG1 (0x1F)
#define MCP_16MHz_10kBPS_CFG2 (0xFF)
#define MCP_16MHz_10kBPS_CFG3 (0x87)

#define MCP_16MHz_5kBPS_CFG1 (0x3F)
#define MCP_16MHz_5kBPS_CFG2 (0xFF)
#define MCP_16MHz_5kBPS_CFG3 (0x87)

/*
 *  speed 20M
 */
#define MCP_20MHz_1000kBPS_CFG1 (0x00)
#define MCP_20MHz_1000kBPS_CFG2 (0xD9)
#define MCP_20MHz_1000kBPS_CFG3 (0x82)

#define MCP_20MHz_500kBPS_CFG1 (0x00)
#define MCP_20MHz_500kBPS_CFG2 (0xFA)
#define MCP_20MHz_500kBPS_CFG3 (0x87)

#define MCP_20MHz_250kBPS_CFG1 (0x41)
#define MCP_20MHz_250kBPS_CFG2 (0xFB)
#define MCP_20MHz_250kBPS_CFG3 (0x86)

#define MCP_20MHz_200kBPS_CFG1 (0x01)
#define MCP_20MHz_200kBPS_CFG2 (0xFF)
#define MCP_20MHz_200kBPS_CFG3 (0x87)

#define MCP_20MHz_125kBPS_CFG1 (0x03)
#define MCP_20MHz_125kBPS_CFG2 (0xFA)
#define MCP_20MHz_125kBPS_CFG3 (0x87)

#define MCP_20MHz_100kBPS_CFG1 (0x04)
#define MCP_20MHz_100kBPS_CFG2 (0xFA)
#define MCP_20MHz_100kBPS_CFG3 (0x87)

#define MCP_20MHz_80kBPS_CFG1 (0x04)
#define MCP_20MHz_80kBPS_CFG2 (0xFF)
#define MCP_20MHz_80kBPS_CFG3 (0x87)

#define MCP_20MHz_50kBPS_CFG1 (0x09)
#define MCP_20MHz_50kBPS_CFG2 (0xFA)
#define MCP_20MHz_50kBPS_CFG3 (0x87)

#define MCP_20MHz_40kBPS_CFG1 (0x09)
#define MCP_20MHz_40kBPS_CFG2 (0xFF)
#define MCP_20MHz_40kBPS_CFG3 (0x87)

#define MCP_20MHZ    0
#define MCP_16MHZ    1
#define MCP_8MHZ     2

#define CAN_4K096BPS 0
#define CAN_5KBPS    1
#define CAN_10KBPS   2
#define CAN_20KBPS   3
#define CAN_31K25BPS 4
#define CAN_33K3BPS  5
#define CAN_40KBPS   6
#define CAN_50KBPS   7
#define CAN_80KBPS   8
#define CAN_100KBPS  9
#define CAN_125KBPS  10
#define CAN_200KBPS  11
#define CAN_250KBPS  12
#define CAN_500KBPS  13
#define CAN_1000KBPS 14

#define MCP2515_REGISTER_CNF3 	0x28
#define MCP2515_REGISTER_CNF2	0x29
#define MCP2515_REGISTER_CNF1 	0x2A


/* ERROR Defines*/
#define MCP2515_OK							0x0
#define MCP2515_TIMOUT_ERROR				0x1
#define MCP2515_TX_BUFFER_BUSY				0x2
#define MCP2515_WRONG_PARAMETER				0x3
#define MCP2515_CRC_FAIL					0x4
#define MCP2515_NOT_ANSWERING				0x5

/* Define a CAN Message struct */
struct can_message{
	uint16_t id;
	uint8_t datalength;
	uint8_t message[8];
};

#define SEND_CAN_MESSAGE_WITH_MEMORY(buffer,mMessage,length,mId) \
	(buffer)->id=(mId); \
	(buffer)->datalength=(length); \
	for(uint8_t i=0;i<(length);i++){ \
		(buffer)->message[i]=(mMessage)[i]; \
	} \
	EventHandling::EventHandler::pInstance.callOSCTrigger(&NS_OSC_MCP2515SPI_OSC::MCP2515SPI::mInstance.trigger_sendMessageFromMemory, (dword_t)(buffer));



BEGIN_DECLARE_OSC_INHERIT(MCP2515SPI, MCP2515SPI, public IMX6ECSPI)

public:
	MCP2515SPI();

	static MCP2515SPI mInstance;

	void triggerFunc_start(dword_t arg);

#ifdef CONFIG_AUTOLAB
	void triggerFunc_startAutolab(dword_t arg);
#endif

	void triggerFunc_stop(dword_t arg);

	void triggerFunc_PeriodicTask(dword_t arg);

	void triggerFunc_sendMessage(dword_t arg);

	void triggerFunc_sendMessageFromMemory(dword_t arg);

	void resetController();

	uint8_t configRate(uint8_t canSpeed, uint8_t canClock);

	uint8_t readRegister(uint8_t address);

	uint8_t writeRegister(uint8_t address, uint8_t data);

	uint8_t receive(uint8_t buffer, uint8_t *message, uint8_t dataLength);

	uint8_t loadTxBuffer(uint8_t buffer, uint8_t *message, uint8_t dataLength, uint16_t messageID);

	uint8_t requestToSend(uint8_t buffer0, uint8_t buffer1, uint8_t buffer2);

	uint8_t readStatus();

	uint8_t rxStatus();

	uint8_t bitModify(uint8_t address, uint8_t mask, uint8_t data);

	uint8_t setMode(uint8_t newMode);

	uint8_t getMode();

	uint8_t setFilter(uint8_t filterid, uint16_t filter);

	uint8_t setMask(uint8_t maskid, uint16_t mask);

	uint8_t setRxMode(uint8_t buffer, uint8_t rxmode);

	uint16_t receivedMessageID(uint8_t buffer);

	uint8_t getRxDataLength(uint8_t buffer);

	void triggerFunc_printRxBuffer();

	void triggerFunc_printTxBuffer();

	/**
	 * Size of the CANEvent pool
	 */
	static constexpr uint32_t cEventPoolSize = 16;

	/**
	 * Static pool of events applications can use to receive CAN messages
	 */
	static CANEvent mEventPool[cEventPoolSize];

	/**
	 * Queue with events that are registered to receive CAN messages
	 */
	Queue<CANEvent> mRegisteredEvents;

	/**
	 * Queue with events that are NOT registered to receive CAN messages
	 */
	Queue<CANEvent> mUnregisteredEvents;

	CANEvent *activateCANEvent(uint16_t messageID);
	void deactivateCANEvent(CANEvent *event);


	/**
	 * Trigger objects to react to outside events
	 */
	static EventHandling::Trigger trigger_start;
	static EventHandling::Trigger trigger_startAutolab;
	static EventHandling::Trigger trigger_PeriodicTask;
	static EventHandling::Trigger trigger_stop;
	static EventHandling::Trigger trigger_sendMessage;
	static EventHandling::Trigger trigger_sendMessageFromMemory;
	static EventHandling::Trigger trigger_printRx;
	static EventHandling::Trigger trigger_printTx;

	DECLARE_TRIGGER(interrupt);

	/**
	 * Dependencies for the triggers
	 */
	static OSC* trigger_deps[];

	static OSC* trigger_output_deps[];

	static OSC* trigger_TX_deps[];
	static OSC* trigger_RX_deps[];

	bool enableLightsRx = false;
	bool enableGearRx = false;


private:
	static constexpr uint32_t cReceiveInterval = 1000 * 1000; //1 s

	uint8_t mode;

	uint8_t useBuffer = 0;

	uint16_t countWheelMsgs = 0;

	bool canInitialised = false;

	EventHandling::TimerEvent *mTimerEventCan;

	bool emitCANEvent(uint16_t messageID, uint8_t bufferID);


};
END_DECLARE_OSC

#endif /* ARCH_ARMV7_IMX6_DRIVER_MCP2515SPI_H_ */
