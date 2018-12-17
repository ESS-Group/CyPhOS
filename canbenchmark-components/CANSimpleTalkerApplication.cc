/*
 * CANSimpleTalkerApplication.cc
 *
 *  Created on: 18.05.2018
 *      Author: hendrik
 */

#include <canbenchmark-components/CANSimpleTalkerApplication.h>

#include <common/SyncOutput.h>

#include <driver/GenericTimer.h>

#include <arch/armv7/imx6/driver/CanBuffer.h>
#include <arch/armv7/imx6/driver/MCP2515SPI.h>

#define TAG "CANTalker"

#define TEST_MESSAGE_ID 0xAB

BEGIN_OSC_IMPLEMENTATION(CANSimpleTalkerApplication,CANSimpleTalkerApplication)

// Singleton instance
CANSimpleTalkerApplication CANSimpleTalkerApplication::mInstance;

SECTION_CRITICAL_DATA OSC* CANSimpleTalkerApplication::trigger_output_deps[] = {CONSOLE_BUFFER_DEP, nullptr};
SECTION_CRITICAL_DATA OSC* CANSimpleTalkerApplication::trigger_deps[] = {&OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(CANSimpleTalkerApplication, start, CANSimpleTalkerApplication::mInstance, trigger_output_deps, EventHandling::Trigger::cMIN_Priority);
SECTION_CRITICAL_DATA DEFINE_TRIGGER(CANSimpleTalkerApplication, periodicSend, CANSimpleTalkerApplication::mInstance, trigger_deps, EventHandling::Trigger::cMIN_Priority);

SECTION_CRITICAL_DATA EventHandling::Event CANSimpleTalkerApplication::messageReady;

DEFINE_TRIGGER_FUNC(CANSimpleTalkerApplication, start) {
	mCounter = 0;

	SYNC_OUTPUT_STREAM(TAG, "Started periodic CAN talker");
	// Create periodic event for message sending
	mPeriodicEvent = OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&CANSimpleTalkerApplication::trigger_periodicSend, 1000);

	// Connect message ready event with CAN driver
	EventHandling::EventHandler::pInstance.subscribeEvent(&messageReady, &OSC_PREFIX(MCP2515SPI)::MCP2515SPI::mInstance.trigger_sendMessage);
}

DEFINE_TRIGGER_FUNC(CANSimpleTalkerApplication, periodicSend) {
	mMessage[7] = mCounter & 0xFF;
	mMessage[6] = (mCounter >> 8) & 0xFF;
	mMessage[5] = (mCounter >> 16) & 0xFF;
	mMessage[4] = (mCounter >> 24) & 0xFF;
	mMessage[3] = (mCounter >> 32) & 0xFF;
	mMessage[2] = (mCounter >> 40) & 0xFF;
	mMessage[1] = (mCounter >> 48) & 0xFF;
	mMessage[0] = (mCounter >> 54) & 0xFF;

	mCounter++;
	// Enter message into shared buffer and save line number for send event
	uint32_t line = OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance.enterMsg(mMessage, 8, TEST_MESSAGE_ID);

	// Emit message ready event with the aforementioned line number
	EventHandling::EventHandler::pInstance.eventTriggered(&messageReady, line);
}

END_OSC_IMPLEMENTATION
