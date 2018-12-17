/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_SHA_TACLESHACOMPONENT_H_
#define TACLEBENCH_COMPONENTS_SHA_TACLESHACOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleshaComponent,TacleshaComponent)
public:
	TacleshaComponent();

	DECLARE_TRIGGER(main);

	static TacleshaComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_SHA_TACLESHACOMPONENT_H_ */
