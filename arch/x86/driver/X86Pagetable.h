/*
 * X86Pagetable.h
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_X86PAGETABLE_H_
#define ARCH_X86_DRIVER_X86PAGETABLE_H_

class X86Pagetable {
public:
	struct pml4Entry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, ign :1, mbz:2,
		avl :3, pageDirectoryPointerBaseAddress :40, available :11,
		nx :1;
	}__attribute__((packed));

	struct pdpeEntry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, ign :1, zero :1,
				ign_1 :1, avl :3, pageDirectoryBaseAddress :40, available :11,
				nx :1;
	}__attribute__((packed));

	struct pdeEntry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, ign :1, zero :1, ign_1 :1,
				avl :3, pageTableBaseAddress :40, available :11,
				nx :1;
	}__attribute__((packed));

	struct pteEntry_t {
		uint64_t p :1, rw :1, us :1, pwt :1, pcd :1, a :1, d :1, pat :1, g :1,
				avl :3, physicalPageBaseAddress:40, available:11, nx:1;
	}__attribute__((packed));


	void fillLinear();

	uintptr_t getBaseAddress() {
		return (uintptr_t)pml4;
	}

	static X86Pagetable sInstances[NR_CPUS];

	void printTablesStart();

	void copyFromPagetable(X86Pagetable *srcTable);
private:
	static constexpr uint32_t cMAX_MEMORY_SIZE = 4; // Maximum size in gigabyte (max. 512)

	static constexpr uint32_t pml4Entries = 512;
	static constexpr uint32_t pdpeEntries = 512;
	static constexpr uint32_t pdeEntries = 512 * cMAX_MEMORY_SIZE;
	static constexpr uint32_t pteEntries = 512 * 512 * cMAX_MEMORY_SIZE; // Enough for 1GB of 4k pages
	pml4Entry_t __attribute__ ((aligned (0x1000))) pml4[pml4Entries];
	pdpeEntry_t __attribute__ ((aligned (0x1000))) pdpe[pdpeEntries];
	pdeEntry_t __attribute__ ((aligned (0x1000))) pde[pdeEntries];
	pteEntry_t __attribute__ ((aligned (0x1000))) pte[pteEntries];

	static constexpr uint32_t cPML4_FLAGS = 0x17;
	static constexpr uint32_t cPDPE_FLAGS = 0x17;
	static constexpr uint32_t cPDE_FLAGS = 0x17;
	static constexpr uint32_t cPTE_FLAGS = 0x7;
};

#endif /* ARCH_X86_DRIVER_X86PAGETABLE_H_ */
