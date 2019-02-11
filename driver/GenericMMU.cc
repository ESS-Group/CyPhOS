/*
 * GenericMMU.cc
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#include <driver/GenericMMU.h>

#define TAG "GenericMMU"
#include <common/debug.h>

GenericMMU *GenericMMU::sInstance;

GenericMMU::GenericMMU() {
	sInstance = this;
}

void GenericMMU::moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage) {
#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Move virtual page:" << hex << virtualPage << " to physical page: " << physicalPage);
#endif

	// Get architectural dummy page address that is available as a temporary (virtual) page store
	uintptr_t dummyPageAddress = getDummyPageAddress();

	// Get physical address that is mapped to the current virtual address
	uintptr_t originalPhysicalAddress = getPhysicalAddressForVirtual(virtualPage);

	// If physical page is not mapped to virtual address map a dummy page to it and use this one
	bool dummyPage = getPhysicalAddressForVirtual(physicalPage) != physicalPage;

	// Memory copy variables
	volatile uint64_t *source = (uint64_t*)virtualPage;
	volatile uint64_t *destination = (uint64_t*)physicalPage;

	// Check if physical destination is mapped to its logical counterpart
	// Temporarily map physical location to a dummy location
	if (dummyPage) {
		mapVirtualPageToPhysicalAddress(dummyPageAddress, physicalPage);
		destination = (uint64_t*)dummyPageAddress;
	}

	// Copy page content
	size_t pageSize = getPageSize();
	for(uint32_t i = 0; i < pageSize; i += 8) {
		*destination = *source;
//		DEBUG_STREAM(TAG,"Source: " << hex << (uintptr_t)source << " dest: " << (uintptr_t)destination);
//		DEBUG_STREAM(TAG,"Copy:" << hex << *source << " to:" << *destination);

		destination++;
		source++;
	}
#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Finished copying");
	DEBUG_STREAM(TAG,"Fix mapping");
#endif

	// Fix the MMU mapping for the requested virtual page
	mapVirtualPageToPhysicalAddress(virtualPage, physicalPage);

#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Clean old physical page: " << hex << originalPhysicalAddress);
#endif

	// Map the virtual dummy page to the source physical page, to be able to clean it
	// FIXME remove if system is stable. This is not necessary for a stable system, just
	// to debug data coherence problems
	mapVirtualPageToPhysicalAddress(dummyPageAddress, originalPhysicalAddress);
	destination = (uint64_t*)dummyPageAddress;

#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Start cleaning");
#endif
	for(uint32_t i = 0; i < pageSize; i += 8) {
//		DEBUG_STREAM(TAG,"Cleaning address: " << (uintptr_t)destination);
		*destination = 0;
		destination++;
	}

#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Clean finished");
#endif

	// Return dummy page to its original mapping
	mapVirtualPageToPhysicalAddress(0x0,0x0);

#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"All finished");
#endif
}
