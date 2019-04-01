/*
 * InputOutputAggregator.cc
 *
 *  Created on: 19.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/InputOutputAggregator.h>

#include <driver/UART.h>

#include <driver/CGAScreen.h>

#include <eventhandling/EventHandler.h>

MEMBER_OF_OSC_BEGIN(InputOutput)

InputOutputAggregator InputOutputAggregator::pInstance;

InputOutputAggregator::InputOutputAggregator() : GenericIO(this) {

}

void InputOutputAggregator::putc(char c) {
	OSC_PREFIX(UART)::UART::mInstances[0].putc(c);
	CGAScreen::mInstance.mInstance.print(&c,1,15);
}

void InputOutputAggregator::handleInputInterrupt(dword_t arg) {
	EventHandling::EventHandler::pInstance.eventTriggered(&event_charReady, arg);
}

MEMBER_OF_OSC_END
