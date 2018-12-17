/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_JFDCTINT_TACLEJFDCTINTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_JFDCTINT_TACLEJFDCTINTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclejfdctintComponent,TaclejfdctintComponent)
public:
	TaclejfdctintComponent();

	DECLARE_TRIGGER(main);

	static TaclejfdctintComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_JFDCTINT_TACLEJFDCTINTCOMPONENT_H_ */
