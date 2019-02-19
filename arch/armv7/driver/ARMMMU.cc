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



void ARMMMU::dumpPageTable() {
	for (uint32_t i = 0; i < 4096; i++) {
		DEBUG_STREAM(TAG,"Pagetable entry: " << dec << i << ", value:" << hex << *(ARMMMU::mPagetable + i));
	}
}

void ARMMMU::activateMMU() {
	uintptr_t pageTable = (uintptr_t)ARMv7PageTable::sInstance.getFirstLevelBaseAddress();
	DEBUG_STREAM(TAG,"TTBCR: " << hex << readTTBCR());
	DEBUG_STREAM(TAG,"TTBR0: " << hex << readTTBR0() );DEBUG_STREAM(TAG,"SCTRL: " << hex << readSYSCTRL() );
	DEBUG_STREAM(TAG,"Activating MMU on core: " << hex << (unsigned int)getCPUID() << " with pagetable at: " << hex << pageTable);
//	// Do sanity check
//	for (uintptr_t i = 0; i < 0xFFFFFFFF; i+=cPAGE_SIZE) {
//		uintptr_t physicalPage = getPhysicalAddressForVirtual(i);
//		if (i != physicalPage) {
//			DEBUG_STREAM(TAG,"Sanity check failed at: " << hex << i << " with physical address: " << physicalPage);
//		}
//	}
//	DEBUG_STREAM(TAG, "Pagetable passed sanity check");



	uint32_t *ptStart = (uint32_t*)pageTable;

	for(int i = 0;i < 10; i++) {
		DEBUG_STREAM(TAG,"Pagetable[" << dec << i << "]: " << hex << *ptStart);
		ptStart++;
	}

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
			::[tlt]"r"((uintptr_t)pageTable):"r0","memory"); // (p. B4-1729 ARM ARM)
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

void ARMMMU::setRangeCacheable(uintptr_t from, uintptr_t to, bool cacheable) {
	ARMv7PageTable::secondLevelDescriptor_t *pageEntry = getSecondLevelPageTableEntryFromAddress(from);

	while (from < to) {
		pageEntry->c = cacheable;
		pageEntry->b = cacheable;

		flushTLBWithAddress(from);

		from += cPAGE_SIZE;
	}
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
	return (getSecondLevelPageTableEntryFromAddress(virtualAddress)->smallPageBaseAddress << 12) | (virtualAddress & 0xFFF);
}

ARMv7PageTable::secondLevelDescriptor_t* ARMMMU::getSecondLevelPageTableEntryFromAddress(uintptr_t address) {
	uintptr_t index = (address >> 20);
//	DEBUG_STREAM(TAG, "First index: " << dec << index);

	// Manual pagetable walk
	ARMv7PageTable::firstLevelDescriptor_t *firstEntry = &ARMv7PageTable::sInstance.getFirstLevelBaseAddress()[index];

//	DEBUG_STREAM(TAG,"First entry: " << hex << *(uint32_t*)firstEntry);

	index = (address >> 12) & 0xFF;
//	DEBUG_STREAM(TAG, "Second index: " << dec << index);


	// Get second level entry
//	DEBUG_STREAM(TAG, "Second table at: " << hex << (firstEntry->secondLevelPageTableBaseAddress << 10));
	ARMv7PageTable::secondLevelDescriptor_t *secondEntry = &((ARMv7PageTable::secondLevelDescriptor_t *)(firstEntry->secondLevelPageTableBaseAddress << 10))[index];
//	DEBUG_STREAM(TAG,"Second entry: " << hex << *(uint32_t*)secondEntry);
	return secondEntry;
}

void ARMMMU::mapVirtualPageToPhysicalAddress(uintptr_t virtualPage, uintptr_t physicalPage, bool cacheable) {
	// FIXME
}
