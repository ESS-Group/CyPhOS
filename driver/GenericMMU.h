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

	/**
	 * This maps a virtual page address to the given physical address. Needs to be implemented for the specific hardware architecture
	 * @param virtualPage Virtual address of a memory page
	 * @param physicalPage Physical destination address of the memory page
	 * @param cacheable If the cacheable attribute should be set or not
	 */
	virtual void mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable) = 0;

	/**
	 * Moves a virtual memory page to a physical memory address. Sets the page table afterwards so that the operation is transparent
	 * @param virtualPage Virtual page to move
	 * @param physicalPage Physical destination address
	 * @param cacheable If the cacheable attribute for the page (destination) should be set afterwards
	 */
	void moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable);

	/**
	 * Manually translates a virtual address to a physical address
	 * (Page table walk)
	 * @param virtualPage Virtual address to translate
	 * @return Corresponding physical address for the virtual one
	 */
	virtual uintptr_t getPhysicalAddressForVirtual(uintptr_t virtualPage) = 0;

	/**
	 * Hardware-specific parameter for a implementing architecture
	 * @return
	 */
	virtual size_t getPageSize() = 0;

	/**
	 * Hardware-specific method to flush the whole TLB
	 */
	virtual void flushTLB() = 0;

	/**
	 * Hardware-specific metod to flush the TLB for a given address.
	 * (If the destination architecture does not support it, implement a full TLB flush here)
	 * @param address Virtual address the TLB should be flushed for
	 */
	virtual void flushTLBWithAddress(uintptr_t address) = 0;

	/**
	 * Pointer to the hardware-specific singleton instance (set by the constructor of this class, needs to be called by the subclass)
	 */
	static GenericMMU *sInstance;
protected:
	/**
	 * A hardware implementation must provide at least space for one page for temporary storage
	 * @return Physical memory address for temporary storage for a page
	 */
	virtual uintptr_t getDummyPageAddress() = 0;
};

#endif /* DRIVER_GENERICMMU_H_ */
