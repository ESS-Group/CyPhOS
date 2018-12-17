/*
 * ConsoleApplication.h
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_CONSOLEAPPLICATION_H_
#define TESTCOMPONENTS_CONSOLEAPPLICATION_H_

#include <component/OSC.h>

#include <eventhandling/Trigger.h>

class ConsoleApplication {
public:
	struct ConsoleApplicationEntry {
		EventHandling::Trigger *trigger;

		const char *mCommandName;
	};

};


#endif /* TESTCOMPONENTS_CONSOLEAPPLICATION_H_ */
