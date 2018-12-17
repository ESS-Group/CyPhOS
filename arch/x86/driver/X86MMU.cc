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

// External symbols from global pagetable fields
extern uint64_t global_pml4;
extern uint64_t global_pdpe;
extern uint64_t global_pde;

X86MMU::X86MMU() {
	// Link global pagetables to class attributes for easy access
	mGlobalPML4 = (pml4Entry_t*)&global_pml4;
	mGlobalPDPE = (pdpeEntry_t*)&global_pdpe;
	mGlobalPDE = (pdeEntry_t*)&global_pde;
}

void X86MMU::setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable) {
#ifdef CONFIG_X86_DEBUG_MMU
	DEBUG_STREAM(TAG,"setRangeCacheable: from: " << hex << from << " to: " << to << " cacheable: " << (uint16_t)cacheable);
#endif
	do {
		pdeEntry_t *entry = getPDEEntryFromAddress(from);
#ifdef CONFIG_X86_DEBUG_MMU
		DEBUG_STREAM(TAG,"Changing PDE entry:" << hex << entry << ": " << *(uint64_t*)entry << " for address: " << from);
#endif

		entry->pcd = !cacheable;

		flushTLBWithAddress(from);

		// Increase from pointer by page size (2MB)
		from += 0x200000;
	} while (from < to);

	// Finish by flushing the TLB
	flushTLB();
}


uintptr_t X86MMU::virtualToPhysical(uintptr_t address) {
//	DEBUG_STREAM(TAG,"Translating virtual address manually: " << hex << address);
//	DEBUG_STREAM(TAG,"PML4 address: " << hex << &global_pml4);
	uint64_t *pml4 = (uint64_t*)&global_pml4;
	uintptr_t physicalAddress = 0x0;

	// Manual pagetable walk

	// Get index to PML4
	uint16_t index = (address >> 39) & 0x1FF;
	// Entry of PML4
	uint64_t pml4_entry = pml4[index];

//	DEBUG_STREAM(TAG,"PML4 index: " << hex << index << " entry: " << pml4_entry);

	uintptr_t pdpe_address = (pml4_entry & 0xFFFFFFFFFF000);
	// Get index in PDPE
	index = (address >> 30) & 0x1FF;
	uint64_t pdpe_entry = ((uint64_t*)pdpe_address)[index];

//	DEBUG_STREAM(TAG,"PDPE index: " << hex << index << " entry: " << pdpe_entry);

	uintptr_t pde_address = (pdpe_entry & 0xFFFFFFFFFF000);
	// Get index in PDPE
	index = (address >> 21) & 0x1FF;
	uint64_t pde_entry = ((uint64_t*)pde_address)[index];

//	DEBUG_STREAM(TAG,"PDE index: " << hex << index << " entry: " << pde_entry);

	physicalAddress = (address & 0x1FFFFF) | (pde_entry & 0xFFFFFFFFFF000);


	return physicalAddress;
}


X86MMU::pdeEntry_t *X86MMU::getPDEEntryFromAddress(uintptr_t address) {
	uint64_t *pml4 = (uint64_t*)&global_pml4;
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
	// Get index in PDPE
	index = (address >> 21) & 0x1FF;


	return (pdeEntry_t*)&((uint64_t*)pde_address)[index];
}


void X86MMU::flushTLB() {
	__asm__ __volatile__ (
			"MFENCE\n"
			"WBINVD\n"
			"mov %%cr3, %%rax\n"
			"mov %%rax, %%cr3\n":::"memory","rax");
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

	for (uint16_t i = 0; i < 10; i++) {
		DEBUG_STREAM(TAG,"Entry " << dec << i << " :" << hex << *(uint64_t*)&mGlobalPDE[i]);
	}
}
