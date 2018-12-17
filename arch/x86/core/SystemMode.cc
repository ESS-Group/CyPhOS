/*
 * SystemMode.cc
 *
 *  Created on: 24.08.2017
 *      Author: hendrik
 */

#include <common/types.h>
#include <core/SystemMode.h>

#include <common/debug.h>

#define TAG "SystemMode"

namespace Core {


void SystemMode::disableInterrupts() {
	cpu_t cpuID = getCPUID();
	if(mIRQCounter[cpuID] == 0 ) {
#ifdef CONFIG_DEBUG_SYSTEMMODE
		DEBUG_STREAM(TAG,"Disable interrupts");
#endif
    	__asm__ __volatile__ ("cli"
    	    				:
    	    				:
    	    				: );
	}
	mIRQCounter[cpuID]++;
}

void SystemMode::enableInterrupts() {
	cpu_t cpuID = getCPUID();
	mIRQCounter[cpuID]--;
	if(mIRQCounter[cpuID] == 0 ) {
#ifdef CONFIG_DEBUG_SYSTEMMODE
		DEBUG_STREAM(TAG,"Enable interrupts");
#endif
    	__asm__ __volatile__ ("sti"
    				:
    				:
    				: );
	}
}

} /* namespace Core */
