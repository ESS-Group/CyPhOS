/*
 * X86Pagetable.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <arch/x86/driver/X86Pagetable.h>


#define TAG "PAGETABLE"
#include <common/debug.h>

#include <common/baremetal.h>

X86Pagetable X86Pagetable::sInstances[NR_CPUS];

void X86Pagetable::fillLinear() {
	// Set first entry of PML4 list to start of pdpe list
	uint64_t *tempPointer = (uint64_t*)&pml4[0];
	*tempPointer = (uintptr_t)pdpe | cPML4_FLAGS;

	// Set all other entries to non present
	for (uint32_t i = 1; i < pml4Entries; i++) {
		tempPointer = (uint64_t*)&pml4[i];
		*tempPointer = cPML4_FLAGS;
		pml4[i].p = 0;
	}

	// Fill pdpe entries
	for (uint32_t i = 0; i < cMAX_MEMORY_SIZE; i++) {
		tempPointer = (uint64_t*)&pdpe[i];
		*tempPointer = (((uintptr_t)&pde[0]) + (i * 0x1000))|cPDPE_FLAGS;
	}

	// Fill pdpe entries
	for (uint32_t i = cMAX_MEMORY_SIZE; i < pdpeEntries; i++) {
		tempPointer = (uint64_t*)&pdpe[i];
		*tempPointer = cPDPE_FLAGS & ~(0x1); // Without present bit
	}

	// Fill pde entries
	for (uint32_t i = 0; i < pdeEntries; i++) {
		tempPointer = (uint64_t*)&pde[i];
		*tempPointer = (((uintptr_t)&pte[0]) + (i * 0x1000))|cPDE_FLAGS;
	}

	// Fill pte entries
	for (uint32_t i = 0; i < pteEntries; i++) {
		tempPointer = (uint64_t*)&pte[i];
		// Pagesize 4kb (4096 = 0x1000) increments
		*tempPointer = (0x1000*i)|cPTE_FLAGS;
	}
}

void X86Pagetable::printTablesStart() {
	uint64_t *tempPointer = (uint64_t*)&pml4[0];
	for(uint32_t i = 0; i < 10; i++) {
		tempPointer = (uint64_t*)&pml4[i];
		DEBUG_STREAM(TAG,"PML4[" << dec << i << "]: " << hex << *tempPointer);
	}

	for(uint32_t i = 0; i < 10; i++) {
		tempPointer = (uint64_t*)&pdpe[i];
		DEBUG_STREAM(TAG,"PDPE[" << dec << i << "]: " << hex << *tempPointer);
	}

	for(uint32_t i = 0; i < 10; i++) {
		tempPointer = (uint64_t*)&pde[i];
		DEBUG_STREAM(TAG,"PDE[" << dec << i << "]: " << hex << *tempPointer);
	}

	for(uint32_t i = 0; i < 10; i++) {
		tempPointer = (uint64_t*)&pte[i];
		DEBUG_STREAM(TAG,"PTE[" << dec << i << "]: " << hex << *tempPointer);
	}
}

void X86Pagetable::copyFromPagetable(X86Pagetable *srcTable) {
	memcpy((void*)pml4, (void*)srcTable->pml4, sizeof(pml4Entry_t) * pml4Entries);
	memcpy((void*)pdpe, (void*)srcTable->pdpe, sizeof(pdpeEntry_t) * pdpeEntries);
	memcpy((void*)pde, (void*)srcTable->pde, sizeof(pdeEntry_t) * pdeEntries);
	memcpy((void*)pte, (void*)srcTable->pte, sizeof(pteEntry_t) * pteEntries);
}
