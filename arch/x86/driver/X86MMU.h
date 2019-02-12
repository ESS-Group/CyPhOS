/*
 * X86MMU.h
 *
 *  Created on: 13.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_X86MMU_H_
#define ARCH_X86_DRIVER_X86MMU_H_

#include <common/types.h>

#include "X86Pagetable.h"

#include <driver/GenericMMU.h>

class X86MMU : public GenericMMU {
public:

	X86MMU();

	static X86MMU mInstance;

	void setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable);

	uintptr_t virtualToPhysical(uintptr_t address);

	void printInformation();

	void activatePagetable(uintptr_t table);
	uintptr_t getPhysicalAddressForVirtual(uintptr_t virtualPage);

	virtual void flushTLB();

	virtual void flushTLBWithAddress(uintptr_t address);
protected:
	virtual uintptr_t getDummyPageAddress() {return cDUMMY_PAGE;};

private:
	static constexpr uintptr_t cDUMMY_PAGE = 0x0;
	static constexpr size_t cPAGESIZE = 4096;
	static constexpr uint16_t cMSR_PAT = 0x277;
	static constexpr uint32_t cCPUPID_PAT_SUPPORT = 0x1;
	static constexpr uint32_t cCPUID_PAT_SUPPORT_BIT_MASK_EDX = (0x1 << 16);



	X86Pagetable::pteEntry_t *getPTEEntryFromAddress(uintptr_t address);


	virtual void mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage);
	virtual size_t getPageSize() { return cPAGESIZE;};
};

#endif /* ARCH_X86_DRIVER_X86MMU_H_ */
