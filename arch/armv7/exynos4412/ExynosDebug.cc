/*
 * OMAPDebug.cc
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */
//#ifdef CONFIG_DEBUG
#include <common/debug.h>
#include "ExynosDebug.h"
#include <arch/armv7/exynos4412/driver/ExynosUartDriver.h>

ExynosDebug ExynosDebug::pInstance __attribute__ ((init_priority (1000)));

#ifdef DEBUG_OUT_SYNC
Corelock ExynosDebug::pSyncLock;
#endif

void ExynosDebug::writeToDevice() {
	for(uint32_t i = 0; i < mBufferPosition; i++ ) {
		OSC_PREFIX(InputOutput)::ExynosUart::pInstance.putc(mCharBuffer[i]);
	}
}


ExynosDebug *ExynosDebug::startLine() {
#ifdef DEBUG_OUT_SYNC
	pSyncLock.lock();
#endif
	return &ExynosDebug::pInstance;
}

void ExynosDebug::endLine() {
	ExynosDebug::pInstance << '\n';
#ifdef DEBUG_OUT_SYNC
	pSyncLock.unlock();
#endif
}

//#endif
