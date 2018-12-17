/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_FILTERBANK_TACLEFILTERBANKCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_FILTERBANK_TACLEFILTERBANKCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclefilterbankComponent,TaclefilterbankComponent)
public:
	TaclefilterbankComponent();

	DECLARE_TRIGGER(main);

	static TaclefilterbankComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_FILTERBANK_TACLEFILTERBANKCOMPONENT_H_ */
