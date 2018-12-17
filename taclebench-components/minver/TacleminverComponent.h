/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_MINVER_TACLEMINVERCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_MINVER_TACLEMINVERCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleminverComponent,TacleminverComponent)
public:
	TacleminverComponent();

	DECLARE_TRIGGER(main);

	static TacleminverComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_MINVER_TACLEMINVERCOMPONENT_H_ */
