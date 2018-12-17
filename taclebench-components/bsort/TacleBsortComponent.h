/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_BSORT_TACLEBSORTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_BSORT_TACLEBSORTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_BsortComponent,Tacle_BsortComponent)
public:
	Tacle_BsortComponent();

	DECLARE_TRIGGER(main);

	static Tacle_BsortComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_BSORT_TACLEBSORTCOMPONENT_H_ */
