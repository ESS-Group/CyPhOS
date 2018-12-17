/*
 * ApplicationCANTest.h
 *
 *  Created on: 10.12.2016
 *      Author: hendrik
 */

#ifndef TESTCOMPONENTS_APPLICATIONCANTEST_H_
#define TESTCOMPONENTS_APPLICATIONCANTEST_H_

#include "ConsoleApplication.h"

BEGIN_DECLARE_OSC_INHERIT(ApplicationCANTest,ApplicationCANTest, public ConsoleApplication)

public:
	ApplicationCANTest();

	void triggerFunc_start(dword_t arg);

	void triggerFunc_resetCANController(dword_t arg);
	void triggerFunc_printRegister(dword_t arg);
	void triggerFunc_periodicReceive(dword_t arg);
	void triggerFunc_transmit(dword_t arg);
	void triggerFunc_readStatus(dword_t arg);
	void triggerFunc_rxStatus(dword_t arg);
	void triggerFunc_getCanMode(dword_t arg);
	void triggerFunc_setRightFilter(dword_t arg);
	void triggerFunc_setWrongFilter(dword_t arg);
	void triggerFunc_canLoopback(dword_t arg);
	void triggerFunc_receive(dword_t arg);
	void triggerFunc_canNoFilter(dword_t arg);
	void triggerFunc_canNormalMode(dword_t arg);
	void triggerFunc_configRate(dword_t arg);

	void triggerFunc_GPIOInit(dword_t arg);
	void triggerFunc_GPIORead(dword_t arg);

	static ApplicationCANTest mInstance;
};

END_DECLARE_OSC

#endif /* TESTCOMPONENTS_APPLICATIONCANTEST_H_ */
