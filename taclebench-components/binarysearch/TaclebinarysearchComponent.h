/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_BINARYSEARCH_TACLEBINARYSEARCHCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_BINARYSEARCH_TACLEBINARYSEARCHCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(TaclebinarysearchComponent,TaclebinarysearchComponent)
public:
	TaclebinarysearchComponent();

	DECLARE_TRIGGER(main);

	static TaclebinarysearchComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_BINARYSEARCH_TACLEBINARYSEARCHCOMPONENT_H_ */
