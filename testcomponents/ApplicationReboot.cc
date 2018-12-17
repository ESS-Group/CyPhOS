/*
 * ApplicationReboot.cc
 *
 *  Created on: 14.11.2016
 *      Author: hendrik
 */

#include "ApplicationReboot.h"

extern "C" {
	void restart_system();
}


BEGIN_OSC_IMPLEMENTATION(ApplicationReboot,ApplicationReboot)

ApplicationReboot ApplicationReboot::mInstance;

SECTION_CRITICAL_DATA OSC* ApplicationReboot::trigger_deps[] = {nullptr};

SECTION_CRITICAL_DATA EventHandling::Trigger ApplicationReboot::trigger_Reboot = {(OSC*)&ApplicationReboot::mInstance,(void (OSC::*)(dword_t))&ApplicationReboot::triggerFunc_start,
				(OSC**)&ApplicationReboot::trigger_deps, EventHandling::Trigger::cMAX_Priority};

ApplicationReboot::ApplicationReboot() {
	// TODO Auto-generated constructor stub

}

void ApplicationReboot::triggerFunc_start(dword_t arg) {
	restart_system();
}

END_OSC_IMPLEMENTATION
