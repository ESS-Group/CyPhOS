/*
 * ARMv7PageTable.cc
 *
 *  Created on: 14.02.2019
 *      Author: hendrik
 */

#include <arch/armv7/driver/ARMv7PageTable.h>

#define TAG "ARMPT"
#include <common/debug.h>

__attribute__ ((section (".pagetable"))) ARMv7PageTable ARMv7PageTable::sInstance;

void ARMv7PageTable::fillLinear() {
	// Create first table linkage to second level tables
	uint32_t *firstLevelEntry = (uint32_t*)getFirstLevelBaseAddress();
	uintptr_t secondLevelStartAddress = (uintptr_t)getSecondLevelBaseAddress();
	for (uint32_t i = 0; i < cFIRST_LEVEL_ENTRIES; i++) {
		*firstLevelEntry = secondLevelStartAddress | cFIRST_LEVEL_FLAGS;
		secondLevelStartAddress += (sizeof(secondLevelDescriptor_t) * cSECOND_LEVEL_ENTIRES);
		firstLevelEntry++;
	}

	// Create second level page tables with linear mapping
	uint32_t *secondLevelEntry = (uint32_t*)getSecondLevelBaseAddress();
	uintptr_t pageAddress = 0x0;
	for (uint32_t i = 0; i < cFIRST_LEVEL_ENTRIES * cSECOND_LEVEL_ENTIRES; i++) {
		*secondLevelEntry = pageAddress | cSECOND_LEVEL_FLAGS;
		// Pagesize (4k)
		pageAddress += 4096;
		if (i < 10 ) {
			DEBUG_STREAM(TAG,"2nd entry: " << hex << *secondLevelEntry);
		}
		secondLevelEntry++;

	}

}
