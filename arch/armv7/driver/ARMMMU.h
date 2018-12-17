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

#include <arch/armv7/driver/memory_map.h>

class ARMMMU {
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
	void flushTLBWithoutBroadcast();

	void dumpPageTable();

	/** Static instance variable */
	static ARMMMU pInstance;
private:

	dword_t *mPagetable;

	static memory_map_entry_t memoryMap[];

	unsigned int getIndexFromAddress(uintptr_t address);

	dword_t readTTBCR();
	dword_t readTTBR0();
	dword_t readSYSCTRL();
	void writeSYSCTRL(dword_t sctrl);

	dword_t readACTLR();
	void writeACTLR(dword_t sctrl);
};

#endif /* ARMMMU_H_ */
