/*
 * OSCCacheControl.h
 *
 *  Created on: 09.04.2015
 *      Author: hendrik
 */

#ifndef DRIVER_OSCCACHECONTROL_H_
#define DRIVER_OSCCACHECONTROL_H_

#include <component/LockedOSC.h>


// include cache control driver
#include <driver/ARMV7CacheControl.h>

BEGIN_DECLARE_LOCKED_OSC(CacheControl)

class OSCCacheControl : public LockedOSC {

public:
//	static Trigger TRIGGER_enableCaches;

	OSCCacheControl();

private:
	static ARMV7CacheControl pCacheControl;

	static void FUNC_enableCaches();


};

END_DECLARE_LOCKED_OSC

#endif /* DRIVER_OSCCACHECONTROL_H_ */
