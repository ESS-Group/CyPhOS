/*
 * CANSimpleReceiver.h
 *
 *  Created on: 18.05.2018
 *      Author: hendrik
 */

#ifndef CANBENCHMARK_COMPONENTS_CANSIMPLERECEIVER_H_
#define CANBENCHMARK_COMPONENTS_CANSIMPLERECEIVER_H_

#include <testcomponents/ConsoleApplication.h>

#include <eventhandling/Trigger.h>

#include <eventhandling/TimerEvent.h>
#include <arch/armv7/imx6/driver/MCP2515SPI.h>

BEGIN_DECLARE_OSC_INHERIT(CANSimpleReceiver,CANSimpleReceiver, public ConsoleApplication)


	DECLARE_TRIGGER(start);
	DECLARE_TRIGGER(receiveMessage);


	DECLARE_TRIGGER(periodicPrint);

	static CANSimpleReceiver mInstance;
	static OSC* trigger_output_deps[];
	static OSC* trigger_deps[];

	static EventHandling::Event messageReady;
private:
	EventHandling::TimerEvent *mPeriodicEvent;
	CANEvent *mCANEvent;

	uint8_t mMessage[8];

	uint64_t mCounter;

	uint32_t mLostMessages;
};


END_DECLARE_OSC

#endif /* CANBENCHMARK_COMPONENTS_CANSIMPLERECEIVER_H_ */
