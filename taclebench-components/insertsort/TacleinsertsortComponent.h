/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_INSERTSORT_TACLEINSERTSORTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_INSERTSORT_TACLEINSERTSORTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleinsertsortComponent,TacleinsertsortComponent)
public:
	TacleinsertsortComponent();

	DECLARE_TRIGGER(main);

	static TacleinsertsortComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_INSERTSORT_TACLEINSERTSORTCOMPONENT_H_ */
