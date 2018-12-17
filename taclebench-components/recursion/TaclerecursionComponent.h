/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_RECURSION_TACLERECURSIONCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_RECURSION_TACLERECURSIONCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclerecursionComponent,TaclerecursionComponent)
public:
	TaclerecursionComponent();

	DECLARE_TRIGGER(main);

	static TaclerecursionComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_RECURSION_TACLERECURSIONCOMPONENT_H_ */
