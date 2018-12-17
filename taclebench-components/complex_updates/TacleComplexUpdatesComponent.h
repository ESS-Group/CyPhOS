/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_COMPLEX_UPDATES_TACLECOMPLEX_UPDATESCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_COMPLEX_UPDATES_TACLECOMPLEX_UPDATESCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_ComplexUpdatesComponent,Tacle_ComplexUpdatesComponent)
public:
	Tacle_ComplexUpdatesComponent();

	DECLARE_TRIGGER(main);

	static Tacle_ComplexUpdatesComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_COMPLEX_UPDATES_TACLECOMPLEX_UPDATESCOMPONENT_H_ */
