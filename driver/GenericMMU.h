/*
 * GenericMMU.h
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#ifndef DRIVER_GENERICMMU_H_
#define DRIVER_GENERICMMU_H_

class GenericMMU {
public:
	virtual void mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) = 0;

	void moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage);
};

#endif /* DRIVER_GENERICMMU_H_ */
