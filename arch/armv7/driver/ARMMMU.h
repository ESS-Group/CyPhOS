/*
 * ARMMMU.h
 *
 *  Created on: 25.07.2014
 *      Author: hb
 */

#ifndef ARMMMU_H_
#define ARMMMU_H_

#define DEVICE_MEMORY_CONFIGURATION_MASK 0x10802

/*
 * Bit 16:1 Shareable, 14-12 [TEX]: 001, 2B=1, 3C=1
 */
#define DRAM_MEMORY_CONFIGURATION_MASK 0x11802 // Uncached
//#define DRAM_MEMORY_CONFIGURATION_MASK 0x1180c // cached

#define RESET_VECTOR_MAPPING_VALUE 0xBFD10802

#ifndef TRANSLATION_TABLE_BASE
#error "no TRANSLATION_TABLE_BASE"
#define TRANSLATION_TABLE_BASE 0x0
#endif

#include <common/types.h>

#include <driver/GenericMMU.h>

#include <arch/armv7/driver/memory_map.h>

class ARMMMU : public GenericMMU {
public:
	ARMMMU();
	void activateMMU();
	void deactivateMMU();
	void populatePageTable();

	void setSectionCacheable(uintptr_t section, bool cacheable);
	void setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable);
	void setRangeType(uintptr_t from, uintptr_t to, memory_section_type type);

	void enableAccessFlag();
	void disableAccessFlag();
	void enableMaintenanceBroadcasting();
	void resetAllAccessFlags();
	void setPagetableEntry(uintptr_t address, dword_t entry);
	void flushTLB();
	void flushTLBWithAddress(uintptr_t address);
	void flushTLBWithoutBroadcast();

	void dumpPageTable();

	/** Static instance variable */
	static ARMMMU pInstance;
private:
	size_t cPAGE_SIZE = 1024 * 1024 * 2;
	uintptr_t cDUMMY_PAGE_ADDRESS = 0x0;
	dword_t *mPagetable;

	static memory_map_entry_t memoryMap[];

	unsigned int getIndexFromAddress(uintptr_t address);

	dword_t readTTBCR();
	dword_t readTTBR0();
	dword_t readSYSCTRL();
	void writeSYSCTRL(dword_t sctrl);

	dword_t readACTLR();
	void writeACTLR(dword_t sctrl);

protected:
	size_t getPageSize() {return cPAGE_SIZE;}
	uintptr_t getDummyPageAddress() {return cDUMMY_PAGE_ADDRESS;}

	uintptr_t getPhysicalAddressForVirtual(uintptr_t virtualAddress);
	void mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable);
};

#endif /* ARMMMU_H_ */
