/*
 * X86MMU.h
 *
 *  Created on: 13.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_X86MMU_H_
#define ARCH_X86_DRIVER_X86MMU_H_

#include <common/types.h>

class X86MMU {
public:
	struct pml4Entry_t {
		uint64_t nx :1, available :11, pageDirectoryPointerBaseAddress :40,
				avl :3, mbz :2, ign :1, a :1, pcd :1, pwt :1, us :1, rw :1,
				p :1;
	}__attribute__((packed));

	struct pdpeEntry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, ign :1, zero :1,
				mbz :1, avl :3, pageDirectoryBaseAddress :40, available :11,
				nx :1;
	}__attribute__((packed));

	struct pdeEntry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, d :1, one :1, g :1,
				avl :3, pat :1, reservered :8, physicalPageBaseAddress :31,
				available :11, nx :1;
	}__attribute__((packed));

	X86MMU();

	static X86MMU mInstance;

	void setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable);

	uintptr_t virtualToPhysical(uintptr_t address);

	void printInformation();
private:
	static constexpr uint16_t cMSR_PAT = 0x277;
	static constexpr uint32_t cCPUPID_PAT_SUPPORT = 0x1;
	static constexpr uint32_t cCPUID_PAT_SUPPORT_BIT_MASK_EDX = (0x1 << 16);
	pml4Entry_t *mGlobalPML4;
	pdpeEntry_t *mGlobalPDPE;
	pdeEntry_t *mGlobalPDE;


	pdeEntry_t *getPDEEntryFromAddress(uintptr_t address);
	void flushTLB();
	void flushTLBWithAddress(uintptr_t address);
};

#endif /* ARCH_X86_DRIVER_X86MMU_H_ */
