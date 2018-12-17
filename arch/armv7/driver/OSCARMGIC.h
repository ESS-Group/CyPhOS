/*
 * OSCARMGIC.h
 *
 *  Created on: 04.05.2015
 *      Author: hendrik
 */

#ifndef DRIVER_OSCARMGIC_H_
#define DRIVER_OSCARMGIC_H_

#include <component/OSC.h>

#include <driver/ARMGIC.h>

BEGIN_DECLARE_OSC(ARMGIC,OSC_ARMGIC)


public:
	OSC_ARMGIC();
};

END_DECLARE_OSC
#endif /* DRIVER_OSCARMGIC_H_ */
