/*
 * ResultPrintApplication.h
 *
 *  Created on: 07.03.2017
 *      Author: hendrik
 */

#ifndef BENCHMARKING_RESULTPRINTAPPLICATION_H_
#define BENCHMARKING_RESULTPRINTAPPLICATION_H_

#include <testcomponents/ConsoleApplication.h>

#include "ResultContainer.h"

BEGIN_DECLARE_OSC_INHERIT(ResultPrintApplication,ResultPrintApplication, public ConsoleApplication)
public:
	ResultPrintApplication();

	static OSC* trigger_deps[];
	static EventHandling::Trigger trigger_PrintResultContainer;

	static ResultPrintApplication mInstance;

	void triggerFunc_printResultContainter(dword_t arg);

};

END_DECLARE_OSC

#endif /* BENCHMARKING_RESULTPRINTAPPLICATION_H_ */
