/*
 * GenericIO.cc
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#include <driver/GenericIO.h>

#include <driver/ConsoleBuffer.h>

#include <common/debug.h>

extern bool exceptionActive;

BEGIN_OSC_IMPLEMENTATION(InputOutput, GenericIO)

GenericIO *GenericIO::pInstance;


SECTION_CRITICAL_DATA OSC* GenericIO::trigger_printLine_deps[] = {&OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance, nullptr};
SECTION_CRITICAL_DATA EventHandling::Trigger GenericIO::trigger_printLine = {(OSC*)GenericIO::pInstance,(void (OSC::*)(dword_t))&GenericIO::triggerFunc_printLine,
				(OSC**)&GenericIO::trigger_printLine_deps, CONFIG_PRIORITY_UART};

SECTION_CRITICAL_DATA EventHandling::Event GenericIO::event_charReady;

GenericIO::GenericIO(OSC *osc)  {
	pInstance = this;
	trigger_printLine.pDestinationOSC = this;
}

void GenericIO::triggerFunc_printLine(dword_t lineNum) {
	if (exceptionActive) {
		return;
	}
	// Get line buffer from ConsoleBuffer component
	const char *buf = OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.readLine(lineNum);

	while (*buf != '\0' ) {
		putc(*buf++);
	}

	OSC_PREFIX(ConsoleBuffer)::ConsoleBuffer::mInstance.freeLine(lineNum);
}

END_OSC_IMPLEMENTATION

