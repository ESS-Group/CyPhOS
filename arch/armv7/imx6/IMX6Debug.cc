/*
 * IMX6Debug.cc
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */
#include <common/debug.h>
#include <arch/armv7/imx6/driver/IMX6UartDriver.h>
#include <arch/armv7/imx6/IMX6Debug.h>

IMX6Debug IMX6Debug::pInstance;

#ifdef DEBUG_OUT_SYNC
Corelock ExynosDebug::pSyncLock;
#endif

void IMX6Debug::writeToDevice() {
	for(uint32_t i = 0; i < mBufferPosition; i++ ) {
		OSC_PREFIX(InputOutput)::IMX6Uart::pInstance.putc(mCharBuffer[i]);
	}
}


IMX6Debug *IMX6Debug::startLine() {
#ifdef DEBUG_OUT_SYNC
	pSyncLock.lock();
#endif
	return &IMX6Debug::pInstance;
}

void IMX6Debug::endLine() {
	IMX6Debug::pInstance << '\n';
#ifdef DEBUG_OUT_SYNC
	pSyncLock.unlock();
#endif
}
