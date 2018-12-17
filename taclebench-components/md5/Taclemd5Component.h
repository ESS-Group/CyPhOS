/*
 * TacleBinarySearchComponent.h
 *
 *  Created on: 03.05.2018
 *      Author: hendrik
 */

#ifndef TACLEBENCH_COMPONENTS_MD5_TACLEMD5COMPONENT_H_
#define TACLEBENCH_COMPONENTS_MD5_TACLEMD5COMPONENT_H_

#include <component/OSC.h>
#include <eventhandling/Trigger.h>

BEGIN_DECLARE_OSC(Taclemd5Component,Taclemd5Component)
public:
	Taclemd5Component();

	DECLARE_TRIGGER(main);

	static Taclemd5Component mInstance;
	static OSC* trigger_deps[];
};
END_DECLARE_OSC

#endif /* TACLEBENCH_COMPONENTS_MD5_TACLEMD5COMPONENT_H_ */
