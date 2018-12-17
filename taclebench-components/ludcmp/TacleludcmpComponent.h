/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_LUDCMP_TACLELUDCMPCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_LUDCMP_TACLELUDCMPCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleludcmpComponent,TacleludcmpComponent)
public:
	TacleludcmpComponent();

	DECLARE_TRIGGER(main);

	static TacleludcmpComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_LUDCMP_TACLELUDCMPCOMPONENT_H_ */
