/*
 * ARMMMU.cpp
 *
 *  Created on: 25.07.2014
 *      Author: hb
 */

#include <driver/ARMMMU.h>

#include <driver/ARMGIC.h>

#include <common/armhelper.h>

/* Debugging */
#include <common/debug.h>
#define TAG "ARMMMU"


#ifndef ARM_MEMORY_MAP
#error "No memory map for the MMU defined"
#define ARM_MEMORY_MAP nullptr
#endif

/* Static instance */
ARMMMU ARMMMU::pInstance;

extern uintptr_t __pagetable_start;
/*
 * This will use the preprocessor macro defined for the specific architecture to fill this memory map with content,
 * during compilation.
 */
memory_map_entry_t ARMMMU::memoryMap[] = ARM_MEMORY_MAP;

ARMMMU::ARMMMU() : GenericMMU() {
	mPagetable = (dword_t*) &__pagetable_start;
}

void ARMMMU::populatePageTable() {
	/* Iterate over the memory map */
	memory_map_entry_t *entry = memoryMap;
	DEBUG_STREAM(TAG,"Memorymap address: " << hex << entry);
	while (entry != nullptr && !(entry->start == 0x0 && entry->length == 0x0)) {
		DEBUG_STREAM(TAG,"Mem_map start:" << hex << entry->start << " length: " << entry->length << " type: " << dec << (unsigned int)entry->type);
		setRangeType(entry->start, entry->start + entry->length - 4, entry->type);
		entry++;
	}
	DSB;
}

unsigned int ARMMMU::getIndexFromAddress(uintptr_t address) {
	// Align address to 1MB bounds (down)
	address = address - (address % 0x100000);
	// calculate index in pagetable
	unsigned int index = address / 0x100000;

	return index;
}


void ARMMMU::dumpPageTable() {
	for (uint32_t i = 0; i < 4096; i++) {
		DEBUG_STREAM(TAG,"Pagetable entry: " << dec << i << ", value:" << hex << *(ARMMMU::mPagetable + i));
	}
}

void ARMMMU::activateMMU() {
	for(int i = 0; i < 10;i++) {
		DEBUG_STREAM(TAG,"Pagetable " << dec << i << " :" << hex <<mPagetable[i] );
	}

	DEBUG_STREAM(TAG,"TTBCR: " << hex << readTTBCR());
	DEBUG_STREAM(TAG,"TTBR0: " << hex << readTTBR0() );DEBUG_STREAM(TAG,"SCTRL: " << hex << readSYSCTRL() );DEBUG_STREAM(TAG,"Activating MMU on core: " << hex << (unsigned int)getCPUID() << " with pagetable at: " << hex << mPagetable);
	__asm__ __volatile__ (
			"mov r0, #0xffffffff\n"
			"mcr p15, 0, r0, c3, c0, 0\n"
			"mcr p15, 0, r0, c7, c5, 0\n"
			"mcr p15, 0, r0, c7, c5, 6\n"
			"mcr p15, 0, r0, c8, c6, 0\n"
			"mov r0,%[tlt]\n"
			"mcr p15, 0, r0, c2, c0, 0\n"
			"MRC    p15, 0, r0, c1, c0, 0\n"
			"ORR    r0, r0, #1 // MMU\n"
			"ORR    r0, r0, #(1<<11) // Branch prediction\n"
			"mcr p15, 0, r0, c1, c0, 0\n"
			"MOV     r0, #0\n"
			"MCR     p15, 0, r0, c8, c7, 0\n     // flush Translation look-aside buffer\n"
			::[tlt]"r"(((uintptr_t)mPagetable)|0x5B):"r0","memory"); // (p. B4-1729 ARM ARM)
	DEBUG_STREAM(TAG,"TTBR0: " << hex << readTTBR0() );DEBUG_STREAM(TAG,"SCTRL: " << hex << readSYSCTRL() );DEBUG_STREAM(TAG,"MMU active");
}

void ARMMMU::flushTLB() {
	__asm__ __volatile__ (
			"ISB\n"
			"DSB\n"
			"MOV     r0, #0\n"
			"MCR p15, 0, r0, c8, c3, 0\n"
			"DSB\n"
			"ISB": : : "memory");
}

void ARMMMU::flushTLBWithAddress(uintptr_t address) {
	// FIXME
	__asm__ __volatile__ (
			"ISB\n"
			"DSB\n"
			"MOV     r0, #0\n"
			"MCR p15, 0, r0, c8, c3, 0\n"
			"DSB\n"
			"ISB": : : "memory");
}

void ARMMMU::flushTLBWithoutBroadcast(){
	__asm__ __volatile__ (
				"ISB\n"
				"DSB\n"
				"MOV     r0, #0\n"
				"MCR p15, 0, r0, c8, c7, 0\n"
				"DSB\n"
				"ISB": : : "memory");
}

void ARMMMU::deactivateMMU() {
	uintptr_t stackpointer = 0;
	uintptr_t linkregister = 0;
	asm ( "mov %0,sp" : "=&r"(stackpointer));
	asm ( "mov %0,lr" : "=&r"(linkregister));
	DEBUG_STREAM(TAG,"SP: " << hex << stackpointer);DEBUG_STREAM(TAG,"LR: " << hex << linkregister);
	for (uintptr_t i = stackpointer; i < stackpointer + (20 * 4); i += 4) {
		DEBUG_STREAM(TAG,"" << hex << i << ":" << *(dword_t*)i);
	}

	DEBUG_STREAM(TAG,"TTBR0: " << hex << readTTBR0() );DEBUG_STREAM(TAG,"SCTRL: " << hex << readSYSCTRL() );DEBUG_STREAM(TAG,"Deactivating MMU on core: " << hex << (unsigned int)getCPUID() << " with pagetable at: " << hex << TRANSLATION_TABLE_BASE);
	__asm__ __volatile__ (
			"mov r0, #0xffffffff\n"
			"mcr p15, 0, r0, c3, c0, 0\n"
			"mcr p15, 0, r0, c7, c5, 0\n"
			"mcr p15, 0, r0, c7, c5, 6\n"
			"mcr p15, 0, r0, c8, c6, 0\n"
			"mov r0,#0x0\n"
			"mcr p15, 0, r0, c2, c0, 0\n"
			"MRC    p15, 0, r0, c1, c0, 0\n"
			"BIC    r0, r0, #1 // MMU\n"
			"MOV     r0, #0\n"
			"MCR     p15, 0, r0, c8, c7, 0\n     // flush Translation look-aside buffer\n"
			"NOP\n" "NOP\n" "NOP\n" "NOP\n" "NOP\n"
			"DSB\n"
			"ISB\n"::[tlt]"r"(TRANSLATION_TABLE_BASE):"r0","memory");
	DEBUG_STREAM(TAG,"TTBR0: " << hex << readTTBR0() );DEBUG_STREAM(TAG,"MMU Not active on core: " << hex << (unsigned int)getCPUID());

	for (uintptr_t i = stackpointer; i < stackpointer + (20 * 4); i += 4) {
		DEBUG_STREAM(TAG,"" << hex << i << ":" << *(dword_t*)i);
	}

}

void ARMMMU::setSectionCacheable(uintptr_t section, bool cacheable) {
	unsigned int index = getIndexFromAddress(section);

	// Make sure the section starts at 1mb boundary
	section = section - (section % 0x100000);

	// Get current translation table entry
	dword_t entry = ARMMMU::mPagetable[index];
	DEBUG_STREAM(TAG,"Setting section: " << hex << section << " to cacheable: " << cacheable << " from current value: " << entry);

	// Set cacheable/bufferable bit [C=3, B=2]
	if (cacheable) {
		entry |= 0xC;
	} else {
		entry = entry & ~(0xC);
	}
	ARMMMU::mPagetable[index] = entry;
	DEBUG_STREAM(TAG,"Setting section: " << hex << section << " to new value: " << entry);
}

void ARMMMU::setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable) {
	// get indices for range
	unsigned int beginIndex = getIndexFromAddress(from);
	unsigned int endIndex = getIndexFromAddress(to);

	DEBUG_STREAM(TAG,"Setting " << dec << (to-from) << " bytes, from address: " << hex << from << " to: " << to << " cacheable " << (uint16_t)cacheable);

	dword_t entry;

	do {
		// Get current entry
		entry = ARMMMU::mPagetable[beginIndex];
		DEBUG_STREAM(TAG,"Setting MMU entry: " << dec << beginIndex << " from old value: " << hex << entry);
		// Set cacheable/bufferable bit [C=3, B=2]
		if (cacheable) {
			entry |= 0xC;
		} else {
			entry = entry & ~(0xC);
		}
		// Save section entry
		ARMMMU::mPagetable[beginIndex] = entry;
		DEBUG_STREAM(TAG,"Setting MMU entry: " << dec << beginIndex << " to new value: " << hex << entry);
		beginIndex++;
	} while (beginIndex <= endIndex);
}

void ARMMMU::setRangeType(uintptr_t from, uintptr_t to, memory_section_type type) {
	// get indices for range
	unsigned int beginIndex = getIndexFromAddress(from);
	unsigned int endIndex = getIndexFromAddress(to);

	DEBUG_STREAM(TAG,"Setting type from: " << dec << beginIndex << " to: " << endIndex);

	dword_t entry = 0;
	uintptr_t addr = from;
	for (unsigned int i = beginIndex; i <= endIndex; i++) {
		if (type == MEMTYPE_DRAM) {
			entry = addr | DRAM_MEMORY_CONFIGURATION_MASK;
		} else {
			entry = addr | DEVICE_MEMORY_CONFIGURATION_MASK;
		}

		// Save section entry
		ARMMMU::mPagetable[i] = entry;
//		DEBUG_STREAM(TAG,"New entry: " << hex << entry);
		//DEBUG_STREAM(TAG,"Set MMU entry: " << dec << i << " to new value: " << hex << ARMMMU::mPagetable[i]);
		addr += 0x100000;
	}
}

void ARMMMU::resetAllAccessFlags() {
	for (uint32_t i = 0; i < (4096); i++) {
		//			DEBUG_STREAM(TAG, "Setting " << hex << entry << " with old value " << hex << (*entry));
		//Making all parts fully accessable
		//DEBUG_STREAM(TAG, "FROM " << hex << mPagetable[i]);
		//mPagetable[i] |= 0b1 << 10;
		dword_t entry = mPagetable[i];

		entry |= (0b1 << 10);
		mPagetable[i] = entry;
		//DEBUG_STREAM(TAG, "TO " << hex << mPagetable[i]);
	}
//	uintptr_t testcomp=(uintptr_t)&OSC_PREFIX(ApplicationSystemInfo)::ApplicationSystemInfo::mInstance;
//	testcomp=testcomp & 0xFFF00000;
//	int i=getIndexFromAddress(testcomp);
//
//	dword_t entry=mPagetable[i];
//
//				entry &= ~(0b1 << 10);
//				mPagetable[i]=entry;

}

/*
 * The Access Flag is used for the dynamic Memory Management. If DYNAMIC_CACHED is used, the allocated parts will be locked. When
 * the Memory is accessed, the Data Abort Exception will be catched and the corresponding memory will be preloaded.
 */
void ARMMMU::enableAccessFlag() {
	dword_t old = readSYSCTRL();
	//This is Bit 29, the AFE
	old |= (0x20000000);
	writeSYSCTRL(old);
//	flushTLB();
}
void ARMMMU::disableAccessFlag() {
	dword_t old = readSYSCTRL();
	//This is Bit 29, the AFE
	old &= ~(0x20000000);
	writeSYSCTRL(old);
//	flushTLB();
}
void ARMMMU::enableMaintenanceBroadcasting(){
	dword_t actlr=readACTLR();
	//SMP (Bit 6) to 1
	actlr |= (0x1 << 6);
	//FW (Bit 0) to 1
	actlr |= (0x1);
	writeACTLR(actlr);
	DEBUG_STREAM(TAG,"Enabled maintenance broadcasting: ACTLR is " << hex << readACTLR());
}
void ARMMMU::setPagetableEntry(uintptr_t address, dword_t entry) {
	uint32_t index = getIndexFromAddress(address);
	mPagetable[index] = entry;
}

dword_t ARMMMU::readTTBCR() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c2, c0, 2;" : "=&r"(regValue));
	return regValue;
}


dword_t ARMMMU::readTTBR0() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c2, c0, 0;" : "=&r"(regValue));
	return regValue;
}

dword_t ARMMMU::readSYSCTRL() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c1, c0, 0;" : "=&r"(regValue));
	return regValue;
}

void ARMMMU::writeSYSCTRL(dword_t sctrl) {
	asm("MCR p15, 0, %0, c1, c0, 0;" : : "r"(sctrl));
}

dword_t ARMMMU::readACTLR() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c1, c0, 1;" : "=&r"(regValue));
	return regValue;
}

void ARMMMU::writeACTLR(dword_t sctrl) {
	asm("MCR p15, 0, %0, c1, c0, 1;" : : "r"(sctrl));
}

uintptr_t ARMMMU::getPhysicalAddressForVirtual(uintptr_t virtualAddress) {
	// FIXME
	return virtualAddress;
}

void ARMMMU::mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable) {
	// FIXME
}
