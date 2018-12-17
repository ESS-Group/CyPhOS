/*
 * CanBuffer.h
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_CANBUFFER_H_
#define ARCH_ARMV7_IMX6_DRIVER_CANBUFFER_H_

#include <component/OSC.h>

#include <common/SyncOutput.h>

class CanBuffer {


public:
	struct CanBufferMsg{
		uint16_t id;
		uint8_t msg[8];
		uint8_t dataLength;
		bool isUsed;
	};

	inline int enterMsg(uint8_t *message, uint8_t dataLength, uint16_t messageID) {
		if(dataLength > 8) return -1;

		// Reserve a line
		uint32_t lineNum = (mWritePosition + 1) % cLineCount;
		uint16_t counter = 0;
		while (mBuffer[lineNum].isUsed && (lineNum != mWritePosition)) {
			lineNum = (lineNum + 1) % cLineCount;
			counter ++;
			if(counter > cLineCount+1){
				return -1;
			}
		}
		mBuffer[lineNum].isUsed = true;

		// copy message
		for(uint8_t i = 0; i<dataLength; i++){
			mBuffer[lineNum].msg[i] = message[i];
		}

		// save id and message length
		mBuffer[lineNum].id = messageID;
		mBuffer[lineNum].dataLength = dataLength;

		mWritePosition = lineNum;

		return lineNum;
	}


	inline int8_t readMsg(uint32_t lineNum, uint8_t *message, uint8_t *dataLength, uint16_t *messageID) {
		if(lineNum >= cLineCount){
			return -1;
		}
		// check if lineNum contains valid data
		if (mBuffer[lineNum].isUsed) {
			// copy message buffer
			for (uint8_t i = 0; i < mBuffer[lineNum].dataLength; i++) {
				message[i] = mBuffer[lineNum].msg[i];
			}
			*dataLength = mBuffer[lineNum].dataLength;
			*messageID = mBuffer[lineNum].id;
			freeLine(lineNum);
			return 0;
		}
		else{
			return -1;
		}
	}

//	void print();

	inline void freeLine(uint32_t lineNum) {
		mBuffer[lineNum].isUsed = false;
	}

	inline uint8_t removeMsgWithID(uint16_t id) {
		uint8_t counter = 0;

		for(uint8_t i=0; i < cLineCount; i++){
			if(mBuffer[i].isUsed && mBuffer[i].id == id){
				freeLine(i);
				counter++;
			}
		}

		return counter;
	}

	inline void print() {
		SYNC_OUTPUT_STREAM_START("_________________________________" << endl) ;

		for(uint8_t i=0; i < cLineCount; i++){
			if(mBuffer[i].isUsed){
				SYNC_OUTPUT_STREAM_CONTINUE( "id: " << (uint16_t) mBuffer[i].id << " message[0]: " << (uint16_t) mBuffer[i].msg[0] << " length: " << (uint16_t) mBuffer[i].dataLength) ;
				SYNC_OUTPUT_STREAM_CONTINUE(" - " << (uint16_t)mBuffer[i].msg[0] << ':'  << (uint16_t)mBuffer[i].msg[1] << ':'  << (uint16_t)mBuffer[i].msg[2] << ':'  << (uint16_t)mBuffer[i].msg[3] << ':'
						 << (uint16_t)mBuffer[i].msg[4] << ':'  << (uint16_t)mBuffer[i].msg[5] << ':'  << (uint16_t)mBuffer[i].msg[6] << ':'  << (uint16_t)mBuffer[i].msg[7] << endl)
			}
		}

		SYNC_OUTPUT_STREAM_END;
	}


private:
	constexpr static int cLineCount = 10;

	uint32_t mWritePosition;

	CanBufferMsg mBuffer[cLineCount];

};



#endif /* ARCH_ARMV7_IMX6_DRIVER_CANBUFFER_H_ */
