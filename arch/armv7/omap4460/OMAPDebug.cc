/*
 * OMAPDebug.cc
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#include <common/debug.h>
#include <arch/armv7/omap4460/driver/OMAPUartDriver.h>
#include <arch/armv7/omap4460/OMAPDebug.h>

OMAPDebug OMAPDebug::pInstance;

#ifdef DEBUG_OUT_SYNC
Corelock ExynosDebug::pSyncLock;
#endif

void OMAPDebug::writeToDevice() {
	for(uint32_t i = 0; i < mBufferPosition; i++ ) {
		OSC_PREFIX(InputOutput)::OMAPUart::pInstance.putc(mCharBuffer[i]);
	}
}


OMAPDebug *OMAPDebug::startLine() {
#ifdef DEBUG_OUT_SYNC
	pSyncLock.lock();
#endif
	return &OMAPDebug::pInstance;
}

void OMAPDebug::endLine() {
	OMAPDebug::pInstance << '\n';
#ifdef DEBUG_OUT_SYNC
	pSyncLock.unlock();
#endif
}
