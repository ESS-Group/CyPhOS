/*

 * GenericIO.h
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#ifndef DRIVER_GENERICIO_H_
#define DRIVER_GENERICIO_H_

#include <component/OSC.h>
#include <component/OSCDependency.h>

#include <eventhandling/Event.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(InputOutput,GenericIO)

public:
	GenericIO(OSC *osc);

	virtual void putc(char c) = 0;

	void triggerFunc_printLine(dword_t lineNum);

	virtual void handleInputInterrupt(dword_t arg) = 0;

	static GenericIO *pInstance;

	static OSC* trigger_printLine_deps[];
	static EventHandling::Trigger trigger_printLine;

	static EventHandling::Event event_charReady;
private:
	OSCDependency mUARTBufferDep;

};


END_DECLARE_OSC
#endif /* DRIVER_GENERICIO_H_ */
