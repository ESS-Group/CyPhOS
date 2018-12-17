/*
 * DebugStream.cc
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/DebugStream.h>

#include <driver/UART.h>

DebugStream DebugStream::mInstance;


void DebugStream::writeToDevice() {
	//write to Screen
	print(mCharBuffer,mBufferPosition,15);
	//write to COM1
	// FIXME
	OSC_PREFIX(InputOutput)::UART::mInstances[0].print(mCharBuffer,mBufferPosition);
}

DebugStream* DebugStream::startLine() {
#ifdef DEBUG_OUT_SYNC
	pSyncLock.lock();
#endif
	return &DebugStream::mInstance;
}

void DebugStream::endLine() {
	DebugStream::mInstance << '\n';
}


