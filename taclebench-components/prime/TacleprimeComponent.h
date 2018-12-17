/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_PRIME_TACLEPRIMECOMPONENT_H_
#define TACLEBENCH_COMPONENTS_PRIME_TACLEPRIMECOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TacleprimeComponent,TacleprimeComponent)
public:
	TacleprimeComponent();

	DECLARE_TRIGGER(main);

	static TacleprimeComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_PRIME_TACLEPRIMECOMPONENT_H_ */
