/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_PM_TACLEPMCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_PM_TACLEPMCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclepmComponent,TaclepmComponent)
public:
	TaclepmComponent();

	DECLARE_TRIGGER(main);

	static TaclepmComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_PM_TACLEPMCOMPONENT_H_ */
