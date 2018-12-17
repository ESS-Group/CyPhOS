/*
 * CanTXBuffer.h
 *
 *  Created on: 22.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_CANTXBUFFER_H_
#define ARCH_ARMV7_IMX6_DRIVER_CANTXBUFFER_H_

#include "CanBuffer.h"

BEGIN_DECLARE_OSC_INHERIT(CanTXBuffer,CanTXBuffer, public CanBuffer)

public:
	static CanTXBuffer mInstance;

};

END_DECLARE_OSC

#endif /* ARCH_ARMV7_IMX6_DRIVER_CANTXBUFFER_H_ */
