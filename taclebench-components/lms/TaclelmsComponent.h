/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_LMS_TACLELMSCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_LMS_TACLELMSCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclelmsComponent,TaclelmsComponent)
public:
	TaclelmsComponent();

	DECLARE_TRIGGER(main);

	static TaclelmsComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_LMS_TACLELMSCOMPONENT_H_ */
