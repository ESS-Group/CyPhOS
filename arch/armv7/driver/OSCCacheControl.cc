/*
 * OSCCacheControl.cc
 *
 *  Created on: 09.04.2015
 *      Author: hendrik
 */

#include <driver/OSCCacheControl.h>

BEGIN_LOCKED_OSC_IMPLEMENTATION(CacheControl)

ARMV7CacheControl OSCCacheControl::pCacheControl;

OSCCacheControl::OSCCacheControl() {
//	TRIGGER_enableCaches = {0,(void*)&FUNC_enableCaches,this};
}

void OSCCacheControl::FUNC_enableCaches() {

}

END_LOCKED_OSC_IMPLEMENTATION
