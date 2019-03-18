/*
 * MCP2515SPI.cc
 *
 *  Created on: 10.12.2016
 *      Author: hendrik
 */

#include "MCP2515SPI.h"
#include <common/debug.h>
#include <common/SyncOutput.h>


#include <driver/GenericTimer.h>

#include "IMX6GPIO.h"

#define TAG "MCP2515SPI"

BEGIN_OSC_IMPLEMENTATION(MCP2515SPI, MCP2515SPI)


/**
 * Instances of static trigger objects
 */
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_start  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_start,
		(OSC**)&MCP2515SPI::trigger_deps,EventHandling::Trigger::cMIN_Priority + 100};
#ifdef CONFIG_AUTOLAB
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_startAutolab  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_startAutolab,
		(OSC**)&MCP2515SPI::trigger_deps,EventHandling::Trigger::cMIN_Priority + 100};
#endif
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_PeriodicTask  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_PeriodicTask,
		(OSC**)&MCP2515SPI::trigger_RX_deps,EventHandling::Trigger::cMIN_Priority + 100};
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_stop  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_stop,
		(OSC**)&MCP2515SPI::trigger_deps,EventHandling::Trigger::cMIN_Priority + 100};
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_sendMessage  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_sendMessage,
		(OSC**)&MCP2515SPI::trigger_TX_deps,EventHandling::Trigger::cMIN_Priority + 101};
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_sendMessageFromMemory  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_sendMessageFromMemory,
		(OSC**)&MCP2515SPI::trigger_TX_deps,EventHandling::Trigger::cMIN_Priority + 102};

SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_printRx  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_printRxBuffer,
		(OSC**)&MCP2515SPI::trigger_RX_deps,EventHandling::Trigger::cMIN_Priority + 103};
SECTION_CRITICAL_DATA EventHandling::Trigger MCP2515SPI::trigger_printTx  = {(OSC*)&MCP2515SPI::mInstance,(void (OSC::*)(dword_t))&MCP2515SPI::triggerFunc_printTxBuffer,
		(OSC**)&MCP2515SPI::trigger_TX_deps,EventHandling::Trigger::cMIN_Priority + 104};

SECTION_CRITICAL_DATA DEFINE_TRIGGER(MCP2515SPI, interrupt, MCP2515SPI::mInstance, MCP2515SPI::trigger_RX_deps, CONFIG_PRIORITY_CAN);

/**
 * Instance of trigger dependencies OSC_PREFIX(CanBuffer)::CanBuffer::mTXInstance OSC_PREFIX(CanBuffer)::CanBuffer::mRXInstance
 */
SECTION_CRITICAL_DATA OSC* MCP2515SPI::trigger_deps[] = {nullptr};
SECTION_CRITICAL_DATA OSC* MCP2515SPI::trigger_output_deps[] = {CONSOLE_BUFFER_DEP,nullptr};

SECTION_CRITICAL_DATA OSC* MCP2515SPI::trigger_TX_deps[] = {&OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance,nullptr};
SECTION_CRITICAL_DATA OSC* MCP2515SPI::trigger_RX_deps[] = {&OSC_PREFIX(CanRXBuffer)::CanRXBuffer::mInstance,nullptr};

SECTION_CRITICAL_DATA CANEvent MCP2515SPI::mEventPool[MCP2515SPI::cEventPoolSize];

MCP2515SPI MCP2515SPI::mInstance;


MCP2515SPI::MCP2515SPI() : mTimerEventCan(nullptr) {
	mECSPIBase = ECSPI1_BASE;
	startSPI();
	selectChannel(0);
	selectChannelMode(0, true);
	setClockPreDivider(6);
	selectClockPhaseOperation(0,false);
	selectClockPolarity(0,false);
	selectDataIdle(0,false);
	mode = MCP2515_CONFIGURATION_MODE;

	// Add all static CANEvents to the pool of unregistered events
	for (uint16_t i = 0; i < cEventPoolSize; i++) {
		mUnregisteredEvents.enqueue((QueueElement<CANEvent>*)&mEventPool[i]);
	}


}

void MCP2515SPI::triggerFunc_start(dword_t arg){
	resetController();
	setMode(MCP2515_CONFIGURATION_MODE);
	configRate(CAN_500KBPS, MCP_8MHZ);

	//reset masks and filters
	setFilter(0, 0x0);
	setFilter(1, 0x0);
	setFilter(2, 0x0);
	setFilter(3, 0x0);
	setFilter(4, 0x0);
	setFilter(5, 0x0);
	setMask(0, 0x0);
	setMask(1, 0x0);

	// set filters and masks for curve, lever and gear messages
//	setFilter(0, rxCurveID);
//	setFilter(1, rxGearID);
//	setMask(0, 0x7ff);
//	setFilter(4, rxLeverID);
//	setMask(1, 0x7ff);

	setFilter(0,0x7FF);

	// Enable receive interrupts
	writeRegister(MCP2515_REGISTER_CANINTE, 0x3);

	setMode(MCP2515_NORMAL_MODE);
	canInitialised = true;
	//mTimerEventCan = OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_PeriodicTask, cReceiveInterval);
	SYNC_OUTPUT_STREAM(TAG, "Started Can Receive" << endl);
	EventHandling::EventHandler::pInstance.subscribeEvent(&OSC_PREFIX(GPIO)::IMX6GPIO::EventCAN, &trigger_interrupt);
}


#ifdef CONFIG_AUTOLAB
void MCP2515SPI::triggerFunc_startAutolab(dword_t arg) {
	resetController();
	setMode(MCP2515_CONFIGURATION_MODE);
	configRate(CAN_500KBPS, MCP_8MHZ);

	//reset masks and filters
	setFilter(0, 0x0);
	setFilter(1, 0x0);
	setFilter(2, 0x0);
	setFilter(3, 0x0);
	setFilter(4, 0x0);
	setFilter(5, 0x0);
	setMask(0, 0x7ff);
	setMask(1, 0x7ff);

	// set filters and masks for curve, lever and gear messages
	setFilter(0, rxCurveID);
	setFilter(1, rxGearID);
	setMask(0, 0x7ff);
	setFilter(4, rxLeverID);
	setMask(1, 0x7ff);

	setMode(MCP2515_NORMAL_MODE);
	canInitialised = true;
	mTimerEventCan = OSC_PREFIX(Timer)::GenericTimer::pInstance->setPeriodicEvent(&trigger_PeriodicTask, cReceiveInterval);

	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(ApplicationGearSelector)::ApplicationGearSelector::trigger_start,0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(ApplicationPedals)::ApplicationPedals::trigger_start,0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(ApplicationLights)::ApplicationLights::trigger_start,0);
	EventHandling::EventHandler::pInstance.callOSCTrigger(&OSC_PREFIX(ApplicationSteeringColumn)::ApplicationSteeringColumn::trigger_start,0);

	SYNC_OUTPUT_STREAM(TAG, "Started Autolab" << endl);
}
#endif

void MCP2515SPI::triggerFunc_stop(dword_t arg){
	OSC_PREFIX(Timer)::GenericTimer::pInstance->cancelPeriodicTimerEvent(mTimerEventCan);
	SYNC_OUTPUT_STREAM(TAG, "Stopped Can Receive" << endl);
}

void MCP2515SPI::triggerFunc_sendMessage(dword_t arg){
	uint8_t dataLength;
	uint16_t id;
	uint8_t message[8];
	if(OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance.readMsg((uint8_t)arg, message, &dataLength, &id)){
		return;
	}
	if(canInitialised){
		loadTxBuffer(useBuffer, message, dataLength, id);
		if(useBuffer == 0){
			requestToSend(1,0,0);
		}
		else if(useBuffer == 1){
			requestToSend(0,1,0);
		}
		else if(useBuffer == 2){
			requestToSend(0,0,1);
		}
		useBuffer = (useBuffer + 1) % 3;
	}
}

/**
 * The arg has to be a pointer to a can-message struct. Keep in mind to add necessary trigger dependencies if you want to send the message out of the cache
 */
void MCP2515SPI::triggerFunc_sendMessageFromMemory(dword_t arg){
	uint8_t dataLength;
	uint16_t id;
	uint8_t message[8];
	struct can_message *theMessage=(struct can_message *)arg;
	dataLength=theMessage->datalength;
	id=theMessage->id;
	for(uint8_t i=0;i<dataLength;i++){
		message[i]=theMessage->message[i];
	}
	if(canInitialised){
		loadTxBuffer(useBuffer, message, dataLength, id);
		if(useBuffer == 0){
			requestToSend(1,0,0);
		}
		else if(useBuffer == 1){
			requestToSend(0,1,0);
		}
		else if(useBuffer == 2){
			requestToSend(0,0,1);
		}
		useBuffer = (useBuffer + 1) % 3;
	}
}

void MCP2515SPI::triggerFunc_PeriodicTask(dword_t arg) {
	//uint8_t counter = 0;
	uint8_t status = rxStatus();
	uint8_t txError = readRegister(MCP2515_REGISTER_TEC);
	uint8_t rxError = readRegister(MCP2515_REGISTER_REC);

	uint8_t canInt = OSC_PREFIX(GPIO)::IMX6GPIO::mInstance.readGPIO(GPIO3_DR, 27);

	writeRegister(MCP2515_REGISTER_TEC,0);
	writeRegister(MCP2515_REGISTER_REC,0);

	SYNC_OUTPUT_STREAM(TAG, "rxStatus: " << (uint16_t) rxStatus() << " REC: " << dec << (uint16_t)rxError
			<< " TEC: " << dec << (uint16_t)txError  << "Int: " << (uint16_t)canInt << endl);
	while ((status & (1 << 6)) || (status & (1 << 7)) ) {
		/*checking for new can message in RXBuffer0*/
		uint16_t messageID = receivedMessageID(0);
		if (messageID) {
			emitCANEvent(messageID, 0);
			bitModify(MCP2515_REGISTER_CANINTF, 0x1, 0);
		}

		/*checking for new can message in RXBuffer1*/
		messageID = receivedMessageID(1);
		if (messageID) {
			emitCANEvent(messageID, 1);
			bitModify(MCP2515_REGISTER_CANINTF, 0x2, 0);
		}

	/*	if(counter > 0){
			SYNC_OUTPUT_STREAM(TAG, "counter: " << (uint16_t) counter << endl);
		}*/
		status = rxStatus();
	//	counter++;

	}
}

bool MCP2515SPI::emitCANEvent(uint16_t messageID, uint8_t bufferID) {
	// Check registered events for messageID
	QueueElement<CANEvent> *queueElement = mRegisteredEvents.pHead;

	// 8 Byte buffer for the CAN message
	uint8_t buffer[8];

	bool success = false;

	while (nullptr != queueElement) {
		CANEvent *event = ((CANEvent*)queueElement);
		if (event->messageID == messageID) {
			// Found a CANEvent with the right messageID
			uint8_t dataLength = getRxDataLength(bufferID);

			// Read message from controller
			receive(bufferID, buffer, dataLength);

			// Enter the received message in the RX buffer
			uint32_t line = OSC_PREFIX(CanRXBuffer)::CanRXBuffer::mInstance.enterMsg(buffer, dataLength, messageID);

			// Emit event within the EventHandler
			EventHandling::EventHandler::pInstance.eventTriggered(event, line);

			success = true;
		}
		queueElement = queueElement->pNext;
	}
	return success;
}


CANEvent *MCP2515SPI::activateCANEvent(uint16_t messageID) {
	QueueElement<CANEvent> *retVal = mUnregisteredEvents.dequeueFirst();

	// Check if CANEvents are available in the pool
	if (nullptr == retVal) {
		return nullptr;
	}

	((CANEvent*)retVal)->messageID = messageID;

	mRegisteredEvents.enqueue(retVal);

	return (CANEvent*)retVal;
}

void MCP2515SPI::deactivateCANEvent(CANEvent *event) {
	mRegisteredEvents.remove(event);
	mUnregisteredEvents.enqueue((QueueElement<CANEvent>*)event);
}

/**
 * Resets the controller by sending the right SPI instruction.
 */
void MCP2515SPI::resetController() {
//	DEBUG_STREAM(TAG,"Resetting MCP2515");
	/* Prepare the output buffer the SPI driver */
	mOutputBuffer[0] = MCP2515_INSTRUCTION_RESET;

	/* Send the output over the SPI */
	writeOutputBuffer(8);

	/* clear receive flags */
	bitModify(MCP2515_REGISTER_CANINTF,0x1,0);
	bitModify(MCP2515_REGISTER_CANINTF,0x2,0);

	uint8_t status = setMode(MCP2515_CONFIGURATION_MODE);
	configRate(CAN_500KBPS, MCP_8MHZ);

	if (status == MCP2515_NOT_ANSWERING) {
		DEBUG_STREAM(TAG,"MCP2515 not answering after controller reset");
	}
}

/*
 *  Configures the speed of the MCP2515
 *  code from: https://github.com/coryjfowler/MCP_CAN_lib/blob/master/mcp_can.cpp
 */
uint8_t MCP2515SPI::configRate(uint8_t canSpeed, uint8_t canClock)
{
	uint8_t set, cfg1, cfg2, cfg3;

	// Default configuration
	cfg1 = MCP_8MHz_5kBPS_CFG1;
	cfg2 = MCP_8MHz_5kBPS_CFG2;
	cfg3 = MCP_8MHz_5kBPS_CFG3;

    set = 1;
    switch (canClock)
    {
        case (MCP_8MHZ):
        switch (canSpeed)
        {
            case (CAN_5KBPS):                                               //   5KBPS
            cfg1 = MCP_8MHz_5kBPS_CFG1;
            cfg2 = MCP_8MHz_5kBPS_CFG2;
            cfg3 = MCP_8MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10KBPS
            cfg1 = MCP_8MHz_10kBPS_CFG1;
            cfg2 = MCP_8MHz_10kBPS_CFG2;
            cfg3 = MCP_8MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20KBPS
            cfg1 = MCP_8MHz_20kBPS_CFG1;
            cfg2 = MCP_8MHz_20kBPS_CFG2;
            cfg3 = MCP_8MHz_20kBPS_CFG3;
            break;

            case (CAN_31K25BPS):                                            //  31.25KBPS
            cfg1 = MCP_8MHz_31k25BPS_CFG1;
            cfg2 = MCP_8MHz_31k25BPS_CFG2;
            cfg3 = MCP_8MHz_31k25BPS_CFG3;
            break;

            case (CAN_33K3BPS):                                             //  33.33KBPS
            cfg1 = MCP_8MHz_33k3BPS_CFG1;
            cfg2 = MCP_8MHz_33k3BPS_CFG2;
            cfg3 = MCP_8MHz_33k3BPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_8MHz_40kBPS_CFG1;
            cfg2 = MCP_8MHz_40kBPS_CFG2;
            cfg3 = MCP_8MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_8MHz_50kBPS_CFG1;
            cfg2 = MCP_8MHz_50kBPS_CFG2;
            cfg3 = MCP_8MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_8MHz_80kBPS_CFG1;
            cfg2 = MCP_8MHz_80kBPS_CFG2;
            cfg3 = MCP_8MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_8MHz_100kBPS_CFG1;
            cfg2 = MCP_8MHz_100kBPS_CFG2;
            cfg3 = MCP_8MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_8MHz_125kBPS_CFG1;
            cfg2 = MCP_8MHz_125kBPS_CFG2;
            cfg3 = MCP_8MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_8MHz_200kBPS_CFG1;
            cfg2 = MCP_8MHz_200kBPS_CFG2;
            cfg3 = MCP_8MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_8MHz_250kBPS_CFG1;
            cfg2 = MCP_8MHz_250kBPS_CFG2;
            cfg3 = MCP_8MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_8MHz_500kBPS_CFG1;
            cfg2 = MCP_8MHz_500kBPS_CFG2;
            cfg3 = MCP_8MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_8MHz_1000kBPS_CFG1;
            cfg2 = MCP_8MHz_1000kBPS_CFG2;
            cfg3 = MCP_8MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
	    return MCP2515_WRONG_PARAMETER;
            break;
        }
        break;

        case (MCP_16MHZ):
        switch (canSpeed)
        {
            case (CAN_5KBPS):                                               //   5Kbps
            cfg1 = MCP_16MHz_5kBPS_CFG1;
            cfg2 = MCP_16MHz_5kBPS_CFG2;
            cfg3 = MCP_16MHz_5kBPS_CFG3;
            break;

            case (CAN_10KBPS):                                              //  10Kbps
            cfg1 = MCP_16MHz_10kBPS_CFG1;
            cfg2 = MCP_16MHz_10kBPS_CFG2;
            cfg3 = MCP_16MHz_10kBPS_CFG3;
            break;

            case (CAN_20KBPS):                                              //  20Kbps
            cfg1 = MCP_16MHz_20kBPS_CFG1;
            cfg2 = MCP_16MHz_20kBPS_CFG2;
            cfg3 = MCP_16MHz_20kBPS_CFG3;
            break;

            case (CAN_33K3BPS):                                              //  20Kbps
            cfg1 = MCP_16MHz_33k3BPS_CFG1;
            cfg2 = MCP_16MHz_33k3BPS_CFG2;
            cfg3 = MCP_16MHz_33k3BPS_CFG3;
            break;

            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_16MHz_40kBPS_CFG1;
            cfg2 = MCP_16MHz_40kBPS_CFG2;
            cfg3 = MCP_16MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg2 = MCP_16MHz_50kBPS_CFG2;
            cfg3 = MCP_16MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_16MHz_80kBPS_CFG1;
            cfg2 = MCP_16MHz_80kBPS_CFG2;
            cfg3 = MCP_16MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_16MHz_100kBPS_CFG1;
            cfg2 = MCP_16MHz_100kBPS_CFG2;
            cfg3 = MCP_16MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_16MHz_125kBPS_CFG1;
            cfg2 = MCP_16MHz_125kBPS_CFG2;
            cfg3 = MCP_16MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_16MHz_200kBPS_CFG1;
            cfg2 = MCP_16MHz_200kBPS_CFG2;
            cfg3 = MCP_16MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_16MHz_250kBPS_CFG1;
            cfg2 = MCP_16MHz_250kBPS_CFG2;
            cfg3 = MCP_16MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_16MHz_500kBPS_CFG1;
            cfg2 = MCP_16MHz_500kBPS_CFG2;
            cfg3 = MCP_16MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_16MHz_1000kBPS_CFG1;
            cfg2 = MCP_16MHz_1000kBPS_CFG2;
            cfg3 = MCP_16MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
	    return MCP2515_WRONG_PARAMETER;
            break;
        }
        break;

        case (MCP_20MHZ):
        switch (canSpeed)
        {
            case (CAN_40KBPS):                                              //  40Kbps
            cfg1 = MCP_20MHz_40kBPS_CFG1;
            cfg2 = MCP_20MHz_40kBPS_CFG2;
            cfg3 = MCP_20MHz_40kBPS_CFG3;
            break;

            case (CAN_50KBPS):                                              //  50Kbps
            cfg1 = MCP_20MHz_50kBPS_CFG1;
            cfg2 = MCP_20MHz_50kBPS_CFG2;
            cfg3 = MCP_20MHz_50kBPS_CFG3;
            break;

            case (CAN_80KBPS):                                              //  80Kbps
            cfg1 = MCP_20MHz_80kBPS_CFG1;
            cfg2 = MCP_20MHz_80kBPS_CFG2;
            cfg3 = MCP_20MHz_80kBPS_CFG3;
            break;

            case (CAN_100KBPS):                                             // 100Kbps
            cfg1 = MCP_20MHz_100kBPS_CFG1;
            cfg2 = MCP_20MHz_100kBPS_CFG2;
            cfg3 = MCP_20MHz_100kBPS_CFG3;
            break;

            case (CAN_125KBPS):                                             // 125Kbps
            cfg1 = MCP_20MHz_125kBPS_CFG1;
            cfg2 = MCP_20MHz_125kBPS_CFG2;
            cfg3 = MCP_20MHz_125kBPS_CFG3;
            break;

            case (CAN_200KBPS):                                             // 200Kbps
            cfg1 = MCP_20MHz_200kBPS_CFG1;
            cfg2 = MCP_20MHz_200kBPS_CFG2;
            cfg3 = MCP_20MHz_200kBPS_CFG3;
            break;

            case (CAN_250KBPS):                                             // 250Kbps
            cfg1 = MCP_20MHz_250kBPS_CFG1;
            cfg2 = MCP_20MHz_250kBPS_CFG2;
            cfg3 = MCP_20MHz_250kBPS_CFG3;
            break;

            case (CAN_500KBPS):                                             // 500Kbps
            cfg1 = MCP_20MHz_500kBPS_CFG1;
            cfg2 = MCP_20MHz_500kBPS_CFG2;
            cfg3 = MCP_20MHz_500kBPS_CFG3;
            break;

            case (CAN_1000KBPS):                                            //   1Mbps
            cfg1 = MCP_20MHz_1000kBPS_CFG1;
            cfg2 = MCP_20MHz_1000kBPS_CFG2;
            cfg3 = MCP_20MHz_1000kBPS_CFG3;
            break;

            default:
            set = 0;
            return MCP2515_WRONG_PARAMETER;
            break;
        }
        break;

        default:
        set = 0;
	return MCP2515_WRONG_PARAMETER;
        break;
    }

    if (set) {
    	writeRegister(MCP2515_REGISTER_CNF1, cfg1);
    	writeRegister(MCP2515_REGISTER_CNF2, cfg2);
    	writeRegister(MCP2515_REGISTER_CNF3, cfg3);
        return MCP2515_OK;
    }

    return MCP2515_WRONG_PARAMETER;
}

/**
 * Reads an internal register of the MCP2515
 * @param address address of the register to be read
 * @return The value of the read register
 */
uint8_t MCP2515SPI::readRegister(uint8_t address) {
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0] = (MCP2515_INSTRUCTION_READ << 16) | (address << 8);

	/* Send the output of the SPI */
	uint32_t readLength = writeOutputBuffer(24);
	if ( readLength == 0) {
		return 0;
	}
	uint8_t retVal = readInputBuffer();
	return retVal;
}

/**
 * Writes a byte to an internal register of the MCP2515
 * @param address address of the register
 * @param data Byte to be sent
 * @return 0 if successful
 */
uint8_t MCP2515SPI::writeRegister(uint8_t address, uint8_t data){
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0]=(MCP2515_INSTRUCTION_WRITE << 16) | (address << 8) | data;

	writeOutputBuffer(24);

	return MCP2515_OK;
}

/**
 * Reads a receive buffer of the MCP2515
 * @param buffer to be read
 * @message pointer to the buffer for the received data
 * @dataLength amount of data to be read
 * @return 0 if successful
 */
uint8_t MCP2515SPI::receive(uint8_t buffer, uint8_t *message, uint8_t dataLength){
	if(buffer > 1 || dataLength > 8 || dataLength == 0){
		return MCP2515_WRONG_PARAMETER;
	}
	uint8_t rxbndm;
	if (!buffer) {
//		buffer = 0x1; /*start at RXB0D0*/
		rxbndm = MCP2515_REGISTER_RXB0D0;
	} else {
//		buffer = 0x3; /*start at RXB1D0*/
		rxbndm = MCP2515_REGISTER_RXB1D0;
	}
	/*read message*/
	for(uint8_t i = 0; i < dataLength; i++){
		message[i] = readRegister(rxbndm + i);
	}
	/*clear flag*/
	if(!buffer){
		bitModify(MCP2515_REGISTER_CANINTF,0x1,0);
	}
	else{
		bitModify(MCP2515_REGISTER_CANINTF,0x2,0);
	}

	return MCP2515_OK;
}

/**
 * Writes a byte to the selected transmit buffer of the MCP2515
 * @param buffer transmit buffer to be written to (between 0 and 2)
 * @param message data to be sent
 * @param dataLength amount of Bytes to be sent
 * @param messageID CAN-ID
 * @return 0 if successful
 */
uint8_t MCP2515SPI::loadTxBuffer(uint8_t buffer, uint8_t *message, uint8_t dataLength, uint16_t messageID){
	uint8_t dlc_register, data_register;
	uint8_t sidl_register, sidh_register;
//	uint8_t sidh;
	uint8_t regVal;
	uint8_t txreq;
	uint16_t timeout = 0;
//	uint8_t txBuf0, txBuf1, txBuf2 = 0;

	/*set register values for correct buffer*/
	if(buffer > 2 || dataLength > 8 || dataLength == 0 || messageID > 0x7ff){
		return MCP2515_WRONG_PARAMETER;
	}
	if(!buffer){
		buffer = 0x1; /*start at TXB0D0*/
//		sidh = 0x0; /*start at TXB0SIDH*/
		data_register = MCP2515_REGISTER_TXB0D0;
		sidh_register = MCP2515_REGISTER_TXB0SIDH;
		sidl_register = MCP2515_REGISTER_TXB0SIDL;
		dlc_register = MCP2515_REGISTER_TXB0DLC;
		txreq = 0x4;
	//	txBuf0 = 1;

	}
	else if(buffer == 1){
		buffer = 0x3; /*start at TXB1D0*/
//		sidh = 0x2; /*start at TXB1SIDH*/
		data_register = MCP2515_REGISTER_TXB1D0;
		sidh_register = MCP2515_REGISTER_TXB1SIDH;
		sidl_register = MCP2515_REGISTER_TXB1SIDL;
		dlc_register = MCP2515_REGISTER_TXB1DLC;
		txreq = 0x10;
		//txBuf1 = 1;
	}
	else{
		buffer = 0x5; /*start at TXB2D0*/
//		sidh = 0x4; /*start at TXB2SIDH*/
		data_register = MCP2515_REGISTER_TXB2D0;
		sidh_register = MCP2515_REGISTER_TXB2SIDH;
		sidl_register = MCP2515_REGISTER_TXB2SIDL;
		dlc_register = MCP2515_REGISTER_TXB2DLC;
		txreq = 0x40;
	//	txBuf2 = 1;
	}
	/*waiting until tx buffer is ready*/
	while((readStatus() & txreq)){
		timeout++;
		if(timeout > 50){
			return MCP2515_TX_BUFFER_BUSY;
		}
	}


	// set message-IDH
	regVal = (uint8_t)(messageID >> 3);
	writeRegister(sidh_register, regVal);
//	mOutputBuffer[0]=((MCP2515_INSTRUCTION_LOAD_TX | sidh) << 8) | regVal;
//	writeOutputBuffer(16);

	// set message-IDL
	messageID &= 0x7; /*get bit 2-0*/
	regVal = readRegister(sidl_register);
	regVal &= 0x17; /*clear bit 7-5 and bit 3(->message will transmit standard identifier)*/
	regVal |= (uint8_t) (messageID << 5);
	writeRegister(sidl_register, regVal);

	//send dataLength
	regVal = dataLength;
	writeRegister(dlc_register, regVal);

	//load output buffer
	for(uint8_t i = 0; i < dataLength; i++){
		writeRegister(data_register + i, message[i]);
	}
	//requestToSend(txBuf0,txBuf1,txBuf2);
	return MCP2515_OK;
}

/**
 * Sends a request-to-send instruction to the MCP2515
 * @param buffer0: 1 if request-to-send for TXBO else 0
 * @param buffer1: 1 if request-to-send for TXB1 else 0
 * @param buffer2: 1 if request-to-send for TXB2 else 0
 * @return 0 if successful
 */
uint8_t MCP2515SPI::requestToSend(uint8_t buffer0, uint8_t buffer1, uint8_t buffer2){
	if(buffer0 > 1 || buffer1 > 1 || buffer2 > 1){
		return MCP2515_WRONG_PARAMETER;
	}
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0]= MCP2515_INSTRUCTION_RTS | (buffer2 << 2) | (buffer1 << 1) | buffer0;

	writeOutputBuffer(8);
	return MCP2515_OK;
}

/*
 * Reads status bits for transmit and receive functions
 * @return the received status bits
 */
uint8_t MCP2515SPI::readStatus(){
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0] = MCP2515_INSTRUCTION_READ_STATUS << 16;
	/* Send the output of the SPI */
	uint32_t readLength = writeOutputBuffer(24);
	if (readLength == 0) {
		return MCP2515_NOT_ANSWERING;
	}
	uint16_t read = readInputBuffer();
	/*compare data out and repeat data out*/
	uint8_t retVal = read >> 8;
	read = read & 0xff;
	if(read != retVal){
		return MCP2515_CRC_FAIL;
	}
	else{
		return retVal;
	}

}

/*
 * Reads status bits for received messages
 * @return the received status bits
 */
uint8_t MCP2515SPI::rxStatus(){
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0] = MCP2515_INSTRUCTION_RX_STATUS << 16;
	/* Send the output of the SPI */
	uint32_t readLength = writeOutputBuffer(24);
	if (readLength == 0) {
		return MCP2515_NOT_ANSWERING;
	}

	uint16_t read = readInputBuffer();
	/*compare data out and repeat data out*/
	uint8_t retVal = read >> 8;

	read = read & 0xff;
	if(read != retVal){
		return MCP2515_CRC_FAIL;
	}
	else{
		return retVal;
	}

}

/*
 * Clears individual bits in a particular register
 * @param address
 * @param mask
 * @param data
 * @return 0 if successful
 */
uint8_t MCP2515SPI::bitModify(uint8_t address, uint8_t mask, uint8_t data){
	/* Prepare the output buffer to the right instruction format */
	mOutputBuffer[0] = (MCP2515_INSTRUCTION_BIT_MODIFY << 24) | (address << 16) | (mask << 8) | data;

	writeOutputBuffer(32);

	return 1;
}

/*
 * Sets the mode of the MCP2515
 * @param mode mode to be set
 * @return 1 if changing the mode was successful else 0
 */
uint8_t MCP2515SPI::setMode(uint8_t newMode){
	uint16_t timeout = 0;
	if(newMode > 4){
		return MCP2515_WRONG_PARAMETER;
	}
	bitModify(MCP2515_REGISTER_CANCTRL, 0xE0, (newMode << 5));
	while (getMode() != newMode){
		timeout++;
		if(timeout > 10000){
			return MCP2515_NOT_ANSWERING;
		}
	}
	mode = newMode;

	return MCP2515_OK;
}
/*
 * Reads the mode of the MCP2515
 * @return current mode
 */
uint8_t MCP2515SPI::getMode(){
	return (readRegister(MCP2515_REGISTER_CANSTAT) >> 5);
}
/*
 * Sets the receive filter
 * @param filterid which filter to be set (filter 0 and 1 for rxbuffer0 and filter 2-5 for rxbuffer1)
 * @param mask mask to be set
 * @return 0 if successful
 */
uint8_t MCP2515SPI::setFilter(uint8_t filterid, uint16_t filter){
	uint8_t sidh_register, sidl_register, filterl, filterh, rxctrl;

	if(filterid > 5 || filter > 0x7ff){
		return MCP2515_WRONG_PARAMETER;
	}
	/*set SIDH and SIDL registers*/
	switch (filterid){
		case 0:
			sidh_register = MCP2515_REGISTER_RXF0SIDH;
			sidl_register = MCP2515_REGISTER_RXF0SIDL;
			break;
		case 1:
			sidh_register = MCP2515_REGISTER_RXF1SIDH;
			sidl_register = MCP2515_REGISTER_RXF1SIDL;
			break;
		case 2:
			sidh_register = MCP2515_REGISTER_RXF2SIDH;
			sidl_register = MCP2515_REGISTER_RXF2SIDL;
			break;
		case 3:
			sidh_register = MCP2515_REGISTER_RXF3SIDH;
			sidl_register = MCP2515_REGISTER_RXF3SIDL;
			break;
		case 4:
			sidh_register = MCP2515_REGISTER_RXF4SIDH;
			sidl_register = MCP2515_REGISTER_RXF4SIDL;
			break;
		case 5:
			sidh_register = MCP2515_REGISTER_RXF5SIDH;
			sidl_register = MCP2515_REGISTER_RXF5SIDL;
			break;
		default:
			return MCP2515_WRONG_PARAMETER;
	}
	/* save mask high and low values */
	filterh = (uint8_t) (filter >> 3);
	filterl = readRegister(sidl_register);
	filterl &= 0x1f; /*clear bit 7-5 */
	filterl |= (uint8_t) (filter << 5);
	filterl &= ~(1 << 3); /*clear bit 3 to use standard frames*/

	/*write filter ID*/
	writeRegister(sidh_register, filterh);
	writeRegister(sidl_register, filterl);

	/*turn masks and filter on for standard identifiers and chose filter*/
	rxctrl = (1 << 5) | filterid;
	if(filterid < 2){
		writeRegister(MCP2515_REGISTER_RXB0CTRL, rxctrl);
	}
	else{
		writeRegister(MCP2515_REGISTER_RXB1CTRL, rxctrl);
	}
	return MCP2515_OK;
}
/*
 * Sets the receive mask
 * @param maskid mask 1 or 0
 * @param mask mask to be set
 * @return 0 if successful
 */
uint8_t MCP2515SPI::setMask(uint8_t maskid, uint16_t mask){
	uint8_t sidh_register;
	uint8_t sidl_register;
	uint8_t maskh;
	uint8_t maskl;
	if(maskid > 1 || mask > 0x7ff){
		return MCP2515_WRONG_PARAMETER;
	}
	/*set SIDH and SIDL registers*/
	if(!maskid){
		sidh_register = MCP2515_REGISTER_RXM0SIDH;
		sidl_register = MCP2515_REGISTER_RXM0SIDL;
	}
	else{
		sidh_register = MCP2515_REGISTER_RXM1SIDH;
		sidl_register = MCP2515_REGISTER_RXM1SIDL;
	}

	/* save mask high and low values */
	maskh = (uint8_t) (mask >> 3);
	maskl = readRegister(sidl_register);
	maskl &= 0x1f; /*clear bit 7-5 */
	maskl |= (uint8_t) (mask << 5);

	writeRegister(sidh_register, maskh);
	writeRegister(sidl_register, maskl);

	return MCP2515_OK;

}
/*
 * Sets the receive mode of the MCP2515
 * @param buffer buffer to be set
 * @param rxmode receive mode to be set
 * @return 0 if successful
 */
uint8_t MCP2515SPI::setRxMode(uint8_t buffer, uint8_t rxmode){
	uint8_t ctrlbuffer;
	if(rxmode > 4 || buffer > 1){
		return MCP2515_WRONG_PARAMETER;
	}
	/*set ctrlbuffer*/
	if(buffer){
		ctrlbuffer = MCP2515_REGISTER_RXB1CTRL;
	}
	else{
		ctrlbuffer = MCP2515_REGISTER_RXB0CTRL;
	}
	/*write receive mode*/
	bitModify(ctrlbuffer, 0x60, (rxmode << 5));
	/*check receive mode*/
	if ((readRegister(ctrlbuffer) >> 5) == rxmode){
		return MCP2515_OK;
	}
	else{
		return MCP2515_CRC_FAIL;
	}
}

/*
 * checks for new messages and return the CAN-ID
 * @param buffer receive buffer to be checked
 * @return CAN-ID for the received message (0 if there is no new message)
 */
uint16_t MCP2515SPI::receivedMessageID(uint8_t buffer){
	uint16_t messageID = 0;
	if(buffer>1) return 0;
	/*read rxstatus*/
	uint8_t status = rxStatus();
	if(!status) return 0;
	if(buffer == 0){
		if(!(status & 0x40)) return 0; //if no new message in buffer 0
		messageID = readRegister(MCP2515_REGISTER_RXB0SIDH) << 3;
		messageID |= readRegister(MCP2515_REGISTER_RXB0SIDL) >> 5;
	}
	else{
		if(!(status & 0x80)) return 0; //if no new message in buffer 1
		messageID = readRegister(MCP2515_REGISTER_RXB1SIDH) << 3;
		messageID |= readRegister(MCP2515_REGISTER_RXB1SIDL) >> 5;
	}
	return messageID;

}
/*
 * reads the data length of a received message
 * @param buffer receive buffer
 * @return CAN-ID data length in bytes
 */
uint8_t MCP2515SPI::getRxDataLength(uint8_t buffer){
	uint8_t dataLength = 0;
	if(buffer>1) return 0;
	if(!buffer){
		dataLength = readRegister(MCP2515_REGISTER_RXB0DLC);
	}
	else{
		dataLength = readRegister(MCP2515_REGISTER_RXB1DLC);
	}

	dataLength &= 0xf;
	return dataLength;
}

void MCP2515SPI::triggerFunc_printRxBuffer(){
	OSC_PREFIX(CanRXBuffer)::CanRXBuffer::mInstance.print();
}

void MCP2515SPI::triggerFunc_printTxBuffer(){
	OSC_PREFIX(CanTXBuffer)::CanTXBuffer::mInstance.print();
}

DEFINE_TRIGGER_FUNC(MCP2515SPI, interrupt) {
	uint8_t status = rxStatus();

	while ((status & (1 << 6)) || (status & (1 << 7)) ) {
		/*checking for new can message in RXBuffer0*/
		uint16_t messageID = receivedMessageID(0);
		if (messageID) {
			emitCANEvent(messageID, 0);
			bitModify(MCP2515_REGISTER_CANINTF, 0x1, 0);
		}

		/*checking for new can message in RXBuffer1*/
		messageID = receivedMessageID(1);
		if (messageID) {
			emitCANEvent(messageID, 1);
			bitModify(MCP2515_REGISTER_CANINTF, 0x2, 0);
		}

		status = rxStatus();


	}
}

END_OSC_IMPLEMENTATION

