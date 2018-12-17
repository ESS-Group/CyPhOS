/*
 * DebugStream.h
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_DEBUGSTREAM_H_
#define ARCH_X86_DRIVER_DEBUGSTREAM_H_

#include <driver/CGAScreen.h>
#include <common/OutputStream.h>


class DebugStream : public OutputStream, public CGAScreen {

protected:
	void writeToDevice();

public:
	static DebugStream mInstance;
	static DebugStream *startLine();
	static void endLine();
};

#endif /* ARCH_X86_DRIVER_DEBUGSTREAM_H_ */
