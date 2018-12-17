/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_MATRIX1_TACLEMATRIX1COMPONENT_H_
#define TACLEBENCH_COMPONENTS_MATRIX1_TACLEMATRIX1COMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Taclematrix1Component,Taclematrix1Component)
public:
	Taclematrix1Component();

	DECLARE_TRIGGER(main);

	static Taclematrix1Component mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_MATRIX1_TACLEMATRIX1COMPONENT_H_ */
