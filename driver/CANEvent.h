/*
 * CANEvent.h
 *
 *  Created on: 18.04.2018
 *      Author: hendrik
 */

#ifndef DRIVER_CANEVENT_H_
#define DRIVER_CANEVENT_H_

#include <common/types.h>

#include <eventhandling/Event.h>



/**
 * Class to manage CAN events. Emitted by a CAN driver on receiving CAN messages
 */
class CANEvent : public EventHandling::Event, public QueueElement<CANEvent> {

public:
	CANEvent() : messageID(0) {};

	uint16_t messageID;
};

#endif /* DRIVER_CANEVENT_H_ */
