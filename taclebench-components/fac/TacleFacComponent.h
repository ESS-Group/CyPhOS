/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_FAC_TACLEFACCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_FAC_TACLEFACCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_FacComponent,Tacle_FacComponent)
public:
	Tacle_FacComponent();

	DECLARE_TRIGGER(main);

	static Tacle_FacComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_FAC_TACLECOUNTNEGATIVECOMPONENT_H_ */
