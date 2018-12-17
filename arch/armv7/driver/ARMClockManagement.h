/*
 * ARMClockManagement.h
 *
 *  Created on: 27.07.2015
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_DRIVER_ARMCLOCKMANAGEMENT_H_
#define ARCH_ARMV7_DRIVER_ARMCLOCKMANAGEMENT_H_



class ARMClockManagement {
public:
	virtual clock_t getCoreClock() = 0;
	virtual clock_t getPeripheralClock() = 0;
};

#endif /* ARCH_ARMV7_DRIVER_ARMCLOCKMANAGEMENT_H_ */
