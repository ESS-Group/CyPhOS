/*
 * DynamicMemoryTest.h
 *
 *  Created on: 02.03.2017
 *      Author: christian
 */

#ifndef TESTCOMPONENTS_TLBINFO_H_
#define TESTCOMPONENTS_TLBINFO_H_

#include <component/OSC.h>

#include "ConsoleApplication.h"

BEGIN_DECLARE_OSC_INHERIT(TLBInfo,TLBInfo, public ConsoleApplication)
public:
TLBInfo();

	void triggerFunc_start(dword_t);
	static TLBInfo mInstance;

	static OSC* trigger_deps[];
	static EventHandling::Trigger trigger_show;
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_DYNAMICMEMORYTEST_H_ */
