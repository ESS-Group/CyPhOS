/*
 * UARTOutputOSC.h
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_UARTCONSOLE_H_
#define TESTCOMPONENTS_UARTCONSOLE_H_

#include <component/OSC.h>

#include "ConsoleApplication.h"

#define UART_CONSOLE_BUFFER_SIZE 160

BEGIN_DECLARE_OSC(UARTConsole,UARTConsole)
public:
	UARTConsole();

	void triggerFunc_handleKey(dword_t key);
	void start();

	static UARTConsole pInstance;
private:
	uint32_t mInputBufferPointer;
	char mInputBuffer[UART_CONSOLE_BUFFER_SIZE];

	void clearBuffer();

	void parseCommand();

	bool compareCommand(const char *cmd1, const char *cmd2);

	void printHelp();


	static ConsoleApplication::ConsoleApplicationEntry mApplications[];

	static OSC* trigger_receiveChar_deps[];
	static EventHandling::Trigger trigger_receiveChar;

protected:

};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_UARTCONSOLE_H_ */
