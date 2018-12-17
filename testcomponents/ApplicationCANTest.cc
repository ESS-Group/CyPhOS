/*
 * ApplicationCANTest.cc
 *
 *  Created on: 10.12.2016
 *      Author: hendrik
 */

#include "ApplicationCANTest.h"

#include <driver/MCP2515SPI.h>

#include <driver/IMX6GPIO.h>

#include <common/SyncOutput.h>

#define TAG "CANTest"

BEGIN_OSC_IMPLEMENTATION(ApplicationCANTest,ApplicationCANTest)

ApplicationCANTest ApplicationCANTest::mInstance;

ApplicationCANTest::ApplicationCANTest() {
	// TODO Auto-generated constructor stub

}

void ApplicationCANTest::triggerFunc_start(dword_t arg) {
//	MCP2515SPI::mInstance.resetController();
}


void ApplicationCANTest::triggerFunc_resetCANController(dword_t arg) {
//	MCP2515SPI::mInstance.resetController();
}

void ApplicationCANTest::triggerFunc_printRegister(dword_t arg) {
//	uint8_t regVal = MCP2515SPI::mInstance.readRegister(0xE);
//	SYNC_OUTPUT_STREAM(TAG, "MCP2515 register: " << hex <<(uint32_t) regVal);
//	uint8_t registers [20];
//	registers[0] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_TXB0CTRL);
//	registers[1] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_TXB0D0);
//	registers[2] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXB0CTRL);
//	registers[3] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXB0D0);
//	registers[4] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXB0SIDH);
//	registers[5] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXB0SIDL);
//	registers[6] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXF0SIDH);
//	registers[7] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXF0SIDL);
//	registers[8] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXM0SIDH);
//	registers[9] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_RXM0SIDL);
//	registers[10] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_CANCTRL);
//	registers[11] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_CANSTAT);
//	registers[12] = MCP2515SPI::mInstance.readRegister(MCP2515_REGISTER_TXB0DLC);
//
//	SYNC_OUTPUT_STREAM(TAG, "txb0ctrl: " << hex <<(uint32_t) registers[0] << " txb0d0: "<< hex <<(uint32_t) registers[1]);
//	SYNC_OUTPUT_STREAM(TAG, "rxb0ctrl: " << hex <<(uint32_t) registers[2] << " rxb0d0: "<< hex <<(uint32_t) registers[3]);
//	SYNC_OUTPUT_STREAM(TAG, "rxb0sidh: " << hex <<(uint32_t) registers[4] << " rxb0sidl: "<< hex <<(uint32_t) registers[5]);
//	SYNC_OUTPUT_STREAM(TAG, "rxf0sidh: " << hex <<(uint32_t) registers[6] << " rxf0sidl: "<< hex <<(uint32_t) registers[7]);
//	SYNC_OUTPUT_STREAM(TAG, "rxm0sidh: " << hex <<(uint32_t) registers[8] << " rxm0sidl: "<< hex <<(uint32_t) registers[9]);
//	SYNC_OUTPUT_STREAM(TAG, "canctrl: " << hex <<(uint32_t) registers[10] << " canstat: "<< hex <<(uint32_t) registers[11]);
//	SYNC_OUTPUT_STREAM(TAG, "txb0dlc: " << hex <<(uint32_t) registers[12]);
}


void ApplicationCANTest::triggerFunc_periodicReceive(dword_t arg){
//	GENERIC_TIMER.setPeriodicEvent(this,(void (OSC::*)(dword_t))&ApplicationCANTest::triggerFunc_receive, 100000 );
}

void ApplicationCANTest::triggerFunc_transmit(dword_t arg) {
//	uint8_t message[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
//	uint16_t messageID = 0x111;
//	if(MCP2515SPI::mInstance.loadTxBuffer(0,message,8,messageID) == MCP2515_TX_BUFFER_BUSY){
//		SYNC_OUTPUT_STREAM(TAG, "Error: Transmit Buffer Busy!")
//				return;
//	}
//
//	MCP2515SPI::mInstance.requestToSend(1,0,0);
//	SYNC_OUTPUT_STREAM(TAG, "Transmit done!");

}

void ApplicationCANTest::triggerFunc_receive(dword_t arg) {
	/*checking for new message in rxbuffer0*/
//	uint16_t messageID = MCP2515SPI::mInstance.receivedMessageID(0);
//	if(messageID){ /* if new message received*/
//		uint8_t dataLength = MCP2515SPI::mInstance.getRxDataLength(0);
//		uint8_t message[8];
//
//		if(MCP2515SPI::mInstance.receive(0, message, dataLength) == MCP2515_OK){ //read message
//
//			SYNC_OUTPUT_STREAM(TAG, ">>>>Receiving message ID Buffer 0");
//			SYNC_OUTPUT_STREAM(TAG, "datalength: " << hex <<(uint32_t) dataLength);
//			SYNC_OUTPUT_STREAM(TAG, "message ID: " << hex <<(uint32_t) messageID);
//			for(uint8_t i = 0; i< dataLength; i++){
//				uint32_t regVal = message[i];
//				SYNC_OUTPUT_STREAM(TAG, "message: " << hex <<(uint32_t) regVal);
//			}
//		}
//		else{
//			SYNC_OUTPUT_STREAM(TAG, "Error while reading CAN rxbuffer");
//		}
//
//	}
//	else{
//	//	SYNC_OUTPUT_STREAM(TAG, "No new message");
//	}
}

void ApplicationCANTest::triggerFunc_readStatus(dword_t arg){
//	uint8_t status = MCP2515SPI::mInstance.readStatus();
//	SYNC_OUTPUT_STREAM(TAG, "status: " << hex <<(uint32_t) status);
}

void ApplicationCANTest::triggerFunc_rxStatus(dword_t arg){
//	uint8_t status = MCP2515SPI::mInstance.rxStatus();
//	SYNC_OUTPUT_STREAM(TAG, "rx status: " << hex <<(uint32_t) status);
}

void ApplicationCANTest::triggerFunc_getCanMode(dword_t arg) {
//	uint8_t mode = MCP2515SPI::mInstance.getMode();
//	SYNC_OUTPUT_STREAM(TAG, "mode: " << hex <<(uint32_t) mode);
}

void ApplicationCANTest::triggerFunc_setRightFilter(dword_t arg){
//	MCP2515SPI::mInstance.setMode(MCP2515_CONFIGURATION_MODE);
//	MCP2515SPI::mInstance.setFilter(0,0x190);
//	MCP2515SPI::mInstance.setMask(0,0x7ff);
//	MCP2515SPI::mInstance.setMode(MCP2515_LOOPBACK_MODE);
}

void ApplicationCANTest::triggerFunc_setWrongFilter(dword_t arg){
//	MCP2515SPI::mInstance.setMode(MCP2515_CONFIGURATION_MODE);
//	MCP2515SPI::mInstance.setFilter(0,0x1ef);
//	MCP2515SPI::mInstance.setMask(0,0x7ff);
//	MCP2515SPI::mInstance.setMode(MCP2515_LOOPBACK_MODE);
}

void ApplicationCANTest::triggerFunc_canNoFilter(dword_t arg){
//	MCP2515SPI::mInstance.setRxMode(0, MCP2515_RXMODE_NO_MASK_NO_FILTERS);
}

void ApplicationCANTest::triggerFunc_canLoopback(dword_t arg) {
//	MCP2515SPI::mInstance.setMode(MCP2515_LOOPBACK_MODE);
}
void ApplicationCANTest::triggerFunc_canNormalMode(dword_t arg){
//	MCP2515SPI::mInstance.setMode(MCP2515_NORMAL_MODE);
}

void ApplicationCANTest::triggerFunc_configRate(dword_t arg){
//	MCP2515SPI::mInstance.setMode(MCP2515_CONFIGURATION_MODE);
//	MCP2515SPI::mInstance.configRate(CAN_500KBPS,MCP_8MHZ);
//	MCP2515SPI::mInstance.setMode(MCP2515_NORMAL_MODE);
}

void ApplicationCANTest::triggerFunc_GPIOInit(dword_t arg){
	OSC_PREFIX(GPIO)::IMX6GPIO::mInstance.configurePorts();
}

void ApplicationCANTest::triggerFunc_GPIORead(dword_t arg){
	uint8_t val = OSC_PREFIX(GPIO)::IMX6GPIO::mInstance.readGPIO(GPIO1_DR, 5);

	SYNC_OUTPUT_STREAM(TAG, "GPIO Port 1 Pin 5: " << hex << val);
}

END_OSC_IMPLEMENTATION

