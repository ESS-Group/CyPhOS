/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_COUNTNEGATIVE_TACLECOUNTNEGATIVECOMPONENT_H_
#define TACLEBENCH_COMPONENTS_COUNTNEGATIVE_TACLECOUNTNEGATIVECOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_CountnegativeComponent,Tacle_CountnegativeComponent)
public:
	Tacle_CountnegativeComponent();

	DECLARE_TRIGGER(main);

	static Tacle_CountnegativeComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_COUNTNEGATIVE_TACLECOUNTNEGATIVECOMPONENT_H_ */
