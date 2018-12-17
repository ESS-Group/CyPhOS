/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_FFT_TACLEFFTCOMPONENT_H_
#define TACLEBENCH_COMPONENTS_FFT_TACLEFFTCOMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Tacle_FftComponent,Tacle_FftComponent)
public:
	Tacle_FftComponent();

	DECLARE_TRIGGER(main);

	static Tacle_FftComponent mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_FFT_TACLECOUNTNEGATIVECOMPONENT_H_ */
