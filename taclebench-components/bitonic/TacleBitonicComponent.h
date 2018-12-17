/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_BITONIC_TACLEBITONICCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_BITONIC_TACLEBITONICCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_BitonicComponent,Tacle_BitonicComponent)
public:
	Tacle_BitonicComponent();

	DECLARE_TRIGGER(main);

	static Tacle_BitonicComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_BITONIC_TACLEBITONICCOMPONENT_H_ */
