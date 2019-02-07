/*
 * X86MMU.cc
 *
 *  Created on: 13.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/X86MMU.h>

#include <common/debug.h>

// To read PAT registers
#include <common/msr.h>

// For identifying PAT,MTRR support
#include <common/cpuid.h>

#define TAG "X86MMU"

// Static singleton
X86MMU X86MMU::mInstance;


X86MMU::X86MMU() {
}

void X86MMU::setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable) {
#ifdef CONFIG_X86_DEBUG_MMU
	DEBUG_STREAM(TAG,"setRangeCacheable: from: " << hex << from << " to: " << to << " cacheable: " << (uint16_t)cacheable);
#endif
	do {
		X86Pagetable::pteEntry_t *entry = getPTEEntryFromAddress(from);

		entry->pcd = !cacheable;

//		flushTLBWithAddress(from);

		// Increase from pointer by page size (4kB)
		from += 0x1000;
	} while (from < to);

	// Finish by flushing the TLB
	flushTLB();
}


uintptr_t X86MMU::virtualToPhysical(uintptr_t address) {
	X86Pagetable::pteEntry_t *pteEntry = getPTEEntryFromAddress(address);

	uintptr_t physicalAddress = (pteEntry->physicalPageBaseAddress << 12) | (address & 0x1FF);

	return physicalAddress;
}


X86Pagetable::pteEntry_t *X86MMU::getPTEEntryFromAddress(uintptr_t address) {
	uint64_t *pml4 = (uint64_t*)X86Pagetable::sInstances[getCPUID()].getBaseAddress();
	// Manual pagetable walk

	// Get index to PML4
	uint16_t index = (address >> 39) & 0x1FF;
	// Entry of PML4
	uint64_t pml4_entry = pml4[index];

	uintptr_t pdpe_address = (pml4_entry & 0xFFFFFFFFFF000);
	// Get index in PDPE
	index = (address >> 30) & 0x1FF;
	uint64_t pdpe_entry = ((uint64_t*)pdpe_address)[index];


	uintptr_t pde_address = (pdpe_entry & 0xFFFFFFFFFF000);
	// Get index in PDE
	index = (address >> 21) & 0x1FF;
	uint64_t pde_entry = ((uint64_t*)pde_address)[index];

	// Get PTE
	uintptr_t pte_address = (pde_entry & 0xFFFFFFFFFF000);
	// Get index in PTE
	index = (address >> 12) & 0x1FF;

	return (X86Pagetable::pteEntry_t*)&((uint64_t*)pte_address)[index];
}


void X86MMU::flushTLB() {
	__asm__ __volatile__ (
			"MFENCE\n"
			"WBINVD\n"
			"mov %%cr3, %%rax\n"
			"mov %%rax, %%cr3\n":::"memory","rax");
}

void X86MMU::moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) {
	DEBUG_STREAM(TAG,"Move virtual page:" << hex << virtualPage << " to physical page: " << physicalPage);
	// Memory move
	uint64_t *source = (uint64_t*)virtualPage;
	uint64_t *destination = (uint64_t*)physicalPage;
	for(uint32_t i = 0; i < 4096; i += 8) {
		*destination++ = *source;

		// Clear old memory
		*source++ = 0x0;
	}


	mapVirtualPageToPhysicalAddress(virtualPage, physicalPage);
}

void X86MMU::flushTLBWithAddress(uintptr_t address) {
	__asm__ __volatile__ (
			"MFENCE\n"
			"WBINVD\n"
			"INVLPG (%0)"::"r"(address):"memory");
}

void X86MMU::printInformation() {
	uint64_t cr4 = 0;
	__asm__ __volatile__ ("mov %%cr4, %0\n":"=r"(cr4)::"memory");
	DEBUG_STREAM(TAG,"CR4 register: " << hex << cr4);

	// Check if PAT support is available
	uint32_t eax = cCPUPID_PAT_SUPPORT,ebx = 0,ecx = 0,edx = 0;
	readCPUID(&eax, &ebx, &ecx, &edx);
	if (cCPUID_PAT_SUPPORT_BIT_MASK_EDX & edx) {
		DEBUG_STREAM(TAG,"MSR PAT: " << hex << readMSR(cMSR_PAT));
	} else {
		DEBUG_STREAM(TAG, "No PAT support");
	}
}

void X86MMU::activatePagetable(uintptr_t table) {
	DEBUG_STREAM(TAG,"Activating page table " << hex << table);
	__asm__ __volatile__ (
			"MFENCE\n"
			"WBINVD\n"
			"mov %0, %%cr3\n"::"r"(table):"memory");

	DEBUG_STREAM(TAG,"Finished");
}

void X86MMU::mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) {
	X86Pagetable::pteEntry_t *pte = getPTEEntryFromAddress(virtualPage);

	pte->physicalPageBaseAddress = (physicalPage & 0xFFFFFFFFFF000) >> 12;

	flushTLBWithAddress(physicalPage);
}
