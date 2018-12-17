/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_QUICKSORT_TACLEQUICKSORTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_QUICKSORT_TACLEQUICKSORTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclequicksortComponent,TaclequicksortComponent)
public:
	TaclequicksortComponent();

	DECLARE_TRIGGER(main);

	static TaclequicksortComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_QUICKSORT_TACLEQUICKSORTCOMPONENT_H_ */
