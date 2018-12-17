/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_ST_TACLESTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_ST_TACLESTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclestComponent,TaclestComponent)
public:
	TaclestComponent();

	DECLARE_TRIGGER(main);

	static TaclestComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_ST_TACLESTCOMPONENT_H_ */
