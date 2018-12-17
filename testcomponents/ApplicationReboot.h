/*
 * ApplicationReboot.h
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_APPLICATIONREBOOT_H_
#define TESTCOMPONENTS_APPLICATIONREBOOT_H_

#include <component/OSC.h>

#include "ConsoleApplication.h"

BEGIN_DECLARE_OSC_INHERIT(ApplicationReboot,ApplicationReboot, public ConsoleApplication)
public:
	ApplicationReboot();

	void triggerFunc_start(dword_t);
	static ApplicationReboot mInstance;

	static OSC* trigger_deps[];
	static EventHandling::Trigger trigger_Reboot;
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_APPLICATIONREBOOT_H_ */
