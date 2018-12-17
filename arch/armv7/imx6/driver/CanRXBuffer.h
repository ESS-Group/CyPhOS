/*
 * CanRXBuffer.h
 *
 *  Created on: 22.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_CANRXBUFFER_H_
#define ARCH_ARMV7_IMX6_DRIVER_CANRXBUFFER_H_

#include "CanBuffer.h"

BEGIN_DECLARE_OSC_INHERIT(CanRXBuffer,CanRXBuffer, public CanBuffer)

public:
	static CanRXBuffer mInstance;
};

END_DECLARE_OSC

#endif /* ARCH_ARMV7_IMX6_DRIVER_CANRXBUFFER_H_ */
