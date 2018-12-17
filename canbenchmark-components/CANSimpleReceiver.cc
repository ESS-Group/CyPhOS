/*
 * CANSimpleReceiver.cc
 *
 *  Created on: 18.05.2018
 *      Author: hendrik
 */

#include <canbenchmark-components/CANSimpleReceiver.h>

#include <common/SyncOutput.h>

#include <driver/GenericTimer.h>

#include <arch/armv7/imx6/driver/CanBuffer.h>


#define TAG "CANReceiver"
#define TEST_MESSAGE_ID 0xAC


BEGIN_OSC_IMPLEMENTATION(CANSimpleReceiver,CANSimpleReceiver)

// Singleton instance
CANSimpleReceiver CANSimpleReceiver::mInstance;

SECTION_CRITICAL_DATA OSC* CANSimpleReceiver::trigger_output_deps[] = {CONSOLE_BUFFER_DEP, nullptr};
SECTION_CRITICAL_DATA OSC* CANSimpleReceiver::trigger_deps[] = {&OSC_PREFIX(CanRXBuffer)::CanRXBuffer::mInstance,&OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(CANSimpleReceiver, start, CANSimpleReceiver::mInstance, trigger_output_deps, EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(CANSimpleReceiver, receiveMessage, CANSimpleReceiver::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority);

SECTION_CRITICAL_DATA DEFINE_TRIGGER(CANSimpleReceiver, periodicPrint, CANSimpleReceiver::mInstance, trigger_output_deps, EventHandling::Trigger::cMIN_Priority);

SECTION_CRITICAL_DATA EventHandling::Event CANSimpleReceiver::messageReady;

DEFINE_TRIGGER_FUNC(CANSimpleReceiver, start) {
	mCounter = 0;

	SYNC_OUTPUT_STREAM(TAG, "Started periodic CAN receiver");

	// Connect message ready event with CAN driver
	EventHandling::EventHandler::pInstance.subscribeEvent(&messageReady, &OSC_PREFIX(MCP2515SPI)::MCP2515SPI::mInstance.trigger_sendMessage);

	mCANEvent = OSC_PREFIX(MCP2515SPI)::MCP2515SPI::mInstance.activateCANEvent(0xAB);
	EventHandling::EventHandler::pInstance.subscribeEvent(mCANEvent, &trigger_receiveMessage);

	mPeriodicEvent = OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&CANSimpleReceiver::trigger_periodicPrint, 1000000);
}

DEFINE_TRIGGER_FUNC(CANSimpleReceiver, periodicPrint) {
	SYNC_OUTPUT_STREAM(TAG,"Lost messages:" << dec << mLostMessages << endl);
}

DEFINE_TRIGGER_FUNC(CANSimpleReceiver, receiveMessage) {
	uint8_t recvMessage[8];
	uint8_t length;
	uint16_t id;
	OSC_PREFIX(CanRXBuffer)::CanRXBuffer::mInstance.readMsg(arg, recvMessage, &length, &id);

//	SYNC_OUTPUT_STREAM(TAG, "Recv msg line: " << dec << arg << ": " << hex << id << " length: " << dec << (uint16_t)length << hex << " - " <<
//			(uint16_t)recvMessage[0] << ':' << (uint16_t)recvMessage[1] << ':'  << (uint16_t)recvMessage[2] << ':'  << (uint16_t)recvMessage[3] << ':'  <<
//			(uint16_t)recvMessage[4] << ':'  << (uint16_t)recvMessage[5] << ':'  << (uint16_t)recvMessage[6] << ':'  << (uint16_t)recvMessage[7] << endl);



	uint64_t recvVal = (uint64_t)recvMessage[7] | (uint64_t)recvMessage[6] << 8 | (uint64_t)recvMessage[5] << 16 | (uint64_t)recvMessage[4] << 24 | (uint64_t)recvMessage[3] << 32
			 | (uint64_t)recvMessage[2] << 40 | (uint64_t)recvMessage[1] << 48 | (uint64_t)recvMessage[0] << 56;

//	SYNC_OUTPUT_STREAM(TAG,"Received counter value: " << dec << recvVal << endl);

	if (recvVal != mCounter + 1) {
		// Obviously lost a message, increase counter
//		SYNC_OUTPUT_STREAM(TAG,"Lost message" << endl);
		mLostMessages++;
	}
	mCounter = recvVal;

	mMessage[7] = recvVal & 0xFF;
	mMessage[6] = (recvVal >> 8) & 0xFF;
	mMessage[5] = (recvVal >> 16) & 0xFF;
	mMessage[4] = (recvVal >> 24) & 0xFF;
	mMessage[3] = (recvVal >> 32) & 0xFF;
	mMessage[2] = (recvVal >> 40) & 0xFF;
	mMessage[1] = (recvVal >> 48) & 0xFF;
	mMessage[0] = (recvVal >> 54) & 0xFF;
	// Enter message into shared buffer and save line number for send event
	uint32_t line = OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance.enterMsg(mMessage, 8, TEST_MESSAGE_ID);

	// Emit message ready event with the aforementioned line number
	EventHandling::EventHandler::pInstance.eventTriggered(&messageReady, line);
}

END_OSC_IMPLEMENTATION
