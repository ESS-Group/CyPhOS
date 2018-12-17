/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_IIR_TACLEIIRCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_IIR_TACLEIIRCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleiirComponent,TacleiirComponent)
public:
	TacleiirComponent();

	DECLARE_TRIGGER(main);

	static TacleiirComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_IIR_TACLEIIRCOMPONENT_H_ */
