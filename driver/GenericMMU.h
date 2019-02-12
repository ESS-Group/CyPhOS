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
	GenericMMU();

	virtual void mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) = 0;

	void moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage);

	virtual uintptr_t getPhysicalAddressForVirtual(uintptr_t virtualPage) = 0;

	virtual size_t getPageSize() = 0;

	virtual void flushTLB() = 0;
	virtual void flushTLBWithAddress(uintptr_t address) = 0;

	static GenericMMU *sInstance;
protected:
	virtual uintptr_t getDummyPageAddress() = 0;
};

#endif /* DRIVER_GENERICMMU_H_ */
