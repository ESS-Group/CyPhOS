/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_BITCOUNT_TACLEBITCOUNTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_BITCOUNT_TACLEBITCOUNTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_BitcountComponent,Tacle_BitcountComponent)
public:
Tacle_BitcountComponent();

	DECLARE_TRIGGER(main);

	static Tacle_BitcountComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_BITCOUNT_TACLEBITCOUNTCOMPONENT_H_ */
