/*
 * GenericMMU.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <driver/GenericMMU.h>

#define TAG "GenericMMU"
#include <common/debug.h>

void GenericMMU::moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) {
	DEBUG_STREAM(TAG,"Move virtual page:" << hex << virtualPage << " to physical page: " << physicalPage);
	// Memory move
	uint64_t *source = (uint64_t*)virtualPage;
	uint64_t *destination = (uint64_t*)physicalPage;
	for(uint32_t i = 0; i < 4096; i += 8) {
		*destination++ = *source;

		// Clear old memory
		*source++ = 0x0;
	}

	// Fix the MMU mapping
	mapVirtualPageToPhysicalAddress(virtualPage, physicalPage);
}
