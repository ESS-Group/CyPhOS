/*
 class OSC;

 * OSCDependency.h
 *
 *  Created on: 06.02.2017
 *      Author: hendrik
 */

#ifndef COMPONENT_OSCDEPENDENCY_H_
#define COMPONENT_OSCDEPENDENCY_H_


#include <common/QueueElement.h>
#include <component/OSC.h>

class OSC;
class OSCDependency : public QueueElement<OSCDependency> {
	public:
	inline OSCDependency(OSC *osc) {
		pDependency = osc;
	}

	OSC *pDependency = nullptr;
};


#endif /* COMPONENT_OSCDEPENDENCY_H_ */
