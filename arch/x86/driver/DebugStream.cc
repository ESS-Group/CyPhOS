/*
 * DebugStream.cc
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/DebugStream.h>

#include <driver/UART.h>

#include <driver/CGAScreen.h>
DebugStream DebugStream::mInstance;


void DebugStream::writeToDevice() {
	//write to Screen
	OSC_PREFIX(InputOutput)::CGAScreen::mInstance.print(mCharBuffer,mBufferPosition,15);
	//write to COM1
	// FIXME
	OSC_PREFIX(UART)::UART::mInstances[0].print(mCharBuffer,mBufferPosition);
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


