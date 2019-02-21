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

#define CONFIG_WIPE_OLD_PAGE

GenericMMU::GenericMMU() {
	sInstance = this;
}



void GenericMMU::moveVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable) {
#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Move virtual page:" << hex << virtualPage << " to physical page: " << physicalPage);
#endif

	// Get architectural dummy page address that is available as a temporary (virtual) page store
	uintptr_t dummyPageAddress = getDummyPageAddress();

#ifdef CONFIG_WIPE_OLD_PAGE
#endif
	// Get physical address that is mapped to the current virtual address
	uintptr_t originalPhysicalAddress = getPhysicalAddressForVirtual(virtualPage);
	mapVirtualPageToPhysicalAddress(virtualPage, originalPhysicalAddress, !cacheable);

	// If physical page is not mapped to virtual address map a dummy page to it and use this one
	bool dummyPage = getPhysicalAddressForVirtual(physicalPage) != physicalPage;

	// Memory copy variables
	volatile uint64_t *source = (uint64_t*)virtualPage;
	volatile uint64_t *destination = (uint64_t*)physicalPage;

	// Check if physical destination is mapped to its logical counterpart
	// Temporarily map physical location to a dummy location

	if (dummyPage) {
		mapVirtualPageToPhysicalAddress(dummyPageAddress, physicalPage, cacheable);
		destination = (uint64_t*)dummyPageAddress;
	} else {
		mapVirtualPageToPhysicalAddress(physicalPage, physicalPage, cacheable);
	}

	// Copy page content
	size_t pageSize = getPageSize();

	for(uint32_t i = 0; i < pageSize; i += 8) {
		*destination = *source;
		destination++;
		source++;
	}
#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Finished copying");
	DEBUG_STREAM(TAG,"Fix mapping");
#endif

	// Fix the MMU mapping for the requested virtual page
	mapVirtualPageToPhysicalAddress(virtualPage, physicalPage, cacheable);



#ifdef CONFIG_WIPE_OLD_PAGE
	// Map the virtual dummy page to the source physical page, to be able to clean it
	// FIXME remove if system is stable. This is not necessary for a stable system, just
	// to debug data coherence problems
	mapVirtualPageToPhysicalAddress(dummyPageAddress, originalPhysicalAddress, false);
	destination = (uint64_t*)dummyPageAddress;


#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"Clean old physical page: " << hex << originalPhysicalAddress);
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
	mapVirtualPageToPhysicalAddress(0x0,0x0, false);
#else
	// No need to wipe dummy page, just return it
	if (dummyPage) {
		mapVirtualPageToPhysicalAddress(0x0,0x0, false);
	}
#endif

#ifdef CONFIG_DEBUG_MMU
	DEBUG_STREAM(TAG,"All finished");
#endif
}
