/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_FIR2DIM_TACLEFIR2DIMCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_FIR2DIM_TACLEFIR2DIMCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Taclefir2dimComponent,Taclefir2dimComponent)
public:
	Taclefir2dimComponent();

	DECLARE_TRIGGER(main);

	static Taclefir2dimComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_FIR2DIM_TACLEFIR2DIMCOMPONENT_H_ */
