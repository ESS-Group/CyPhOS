/*
 * ACPI.cc
 *
 *  Created on: 07.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/ACPI.h>

#include <common/debug.h>
#include <common/strcmp.h>

// Include IOAPIC driver for registration of MADT data
#include <driver/IOAPIC.h>

#include <driver/SMPManager.h>

#define TAG "ACPI"

#define GET_RSDT_FROM_RSDP(RSDP) ((RSDT*)(uintptr_t)RSDP->rsdtAddress)
#define GET_XSDT_FROM_RSDP(RSDP) ((XSDT*)(uintptr_t)RSDP->xsdtAddress)

ACPI ACPI::mInstances[NR_CPUS];

extern uint64_t *global_pml4;
extern uint64_t *global_pdpe;
extern uint64_t *global_pde;

ACPI::ACPI() : mRSDP(0), mRSDT(0), mXSDT(0) {

}


static void printMemory(uintptr_t start, uint64_t length) {
	uint64_t offset = 0;
	while (length != 0) {
		DEBUG_STREAM(TAG,"" << hex << start << "[" << offset << "]: " << *(uint32_t*)start << " " << (start + 4) << "[" << offset + 4 << "]: "  << *(uint32_t*)(start + 4));
		if (length < 8) {
			length = 0;
		} else {
			length -= 8;
		}
		start += 8;
		offset += 8;
	}
}

void ACPI::initACPI() {
	if (mRSDP == 0) {
		mRSDP = (RSDP2*)findRSDP();
	}

	if (mRSDP == 0) {
		DEBUG_STREAM(TAG,"Found no valid RSDP. System will probably not work correctly! :-(");
	}
	else {
		if (mRSDP->revision > 1) {
			mXSDT = GET_XSDT_FROM_RSDP(mRSDP);

			mMADT = (MADT*)findTablefromXSDT(mXSDT, madt_signature);
		} else {
			mRSDT = GET_RSDT_FROM_RSDP(mRSDP);

			mMADT = (MADT*)findTablefromRSDT(mRSDT, madt_signature);
		}
#ifdef CONFIG_DEBUG_ACPI
		DEBUG_STREAM(TAG,"RSDP table: " << hex << mRSDP);
		DEBUG_STREAM(TAG,"RSDT table: " << hex << mRSDT);
		DEBUG_STREAM(TAG,"XSDT table: " << hex << mXSDT);
		DEBUG_STREAM(TAG,"MADT table: " << hex << mMADT);
#endif
	}

	if (mMADT != 0 ) {
		MADT_Entry *entry = getNextEntry((MADT_Entry*)mMADT);

		while(entry != 0) {
#ifdef CONFIG_DEBUG_ACPI
#ifdef CONFIG_X86_PRINT_MADT_ENTRIES
			DEBUG_STREAM(TAG,"MADT_Entry: " << hex << entry << " type: " << (uint16_t)entry->type << " length: " << (uint16_t)entry->length);
#endif
#endif

			if (entry->type == ACPI::MADT_ENTRY_IOAPIC ) {
				MADT_Entry_IOAPIC *ioapicEntry = (MADT_Entry_IOAPIC*)entry;
				// For each IOAPIC entry register one instance with the IOAPIC driver
				uintptr_t baseAddress = ioapicEntry->ioAPICAddress;
				uint8_t ioapicID = ioapicEntry->ioAPICID;
				uint32_t globalInterruptBase = ioapicEntry->globalSystemInterruptBase;

				IOAPIC::configureNewInstance(baseAddress, ioapicID, globalInterruptBase);
			} else if (entry->type == ACPI::MADT_ENTRY_LOCAL_APIC) {
				MADT_Entry_Processor_Local_APIC *lAPICEntry = (MADT_Entry_Processor_Local_APIC*)entry;
				// Found an entry for a local APIC. Register it with the SMPManager if it is defined as enabled (ACPI Spec 6.2a p. 154)
				if (lAPICEntry->flags & 0x1) {
					SMPManager::sInstance.addAPICID(lAPICEntry->apicID, lAPICEntry->acpiProcessorID, (lAPICEntry->flags & 0x1) );
				}
			}

			entry = getNextEntry(entry);
		}
	}
}


uintptr_t ACPI::findRSDP() {
	RSDP2 *rsdp = (RSDP2*)rsdp2_base_address;
	if (checkRSDPValidity(rsdp)) {
		return (uintptr_t)rsdp;
	}

	DEBUG_STREAM(TAG,"0x40e: " << (uintptr_t)(void*) ((uint64_t) ((*( (uint16_t*) ( (0x40<<4)+0x0e) ))<<4)));
	rsdp = (RSDP2*) findRSDP((uintptr_t)(void*) ((uint64_t) ((*( (uint16_t*) ( (0x40<<4)+0x0e) ))<<4)), 0x400);
	if (rsdp != 0){
		return (uintptr_t)rsdp;
	}
	// Search through BIOS read-only memory (ACPI spec. p 119 (6.2.a)
	rsdp = (RSDP2*) findRSDP(0xE0000, 0x1FFFF);

	if (rsdp != 0){
		return (uintptr_t)rsdp;
	}

	return (uintptr_t)0;
}



uintptr_t ACPI::findRSDP(uintptr_t start, uint64_t length) {

	// Search through BIOS read-only memory (ACPI spec. p 119 (6.2.a)
	while (length > 0) {
		bool success = checkRSDPValidity((RSDP2*)start);
		if (success) {
			return start;
		}

		// Table is always aligned to 16 bytes
		if (length < 0x10) {
			length = 0;
		} else {
			length -= 0x10;
		}
		start += 0x10;
	}
	return (uintptr_t)0;
}


bool ACPI::checkRSDPValidity(RSDP2 *rsdp) {
//#ifdef CONFIG_DEBUG_ACPI
//	DEBUG_STREAM(TAG,"Looking for RSDP table at: " << hex << (uintptr_t)rsdp);
//	DEBUG_STREAM(TAG,"Data at this point" << *(uint64_t*)rsdp);
//#endif

	char *sigChar = (char*)rsdp->signature;
	char *cmpChar = (char*)rsdp_signature;

	bool rsdpSigFound = strncmp(sigChar,cmpChar, 8);

	bool sdtSigFound;
	if (rsdp->revision > 1) {
		sdtSigFound = strncmp((char*)xsdt_signature,GET_XSDT_FROM_RSDP(rsdp)->signature, 4);
	} else {
		sdtSigFound = strncmp((char*)rsdt_signature,GET_RSDT_FROM_RSDP(rsdp)->signature, 4);
	}

	return rsdpSigFound && sdtSigFound;
}



void ACPI::registerExternalRSDP(RSDP2 *rdsp) {
	if (checkRSDPValidity(rdsp)) {
		mRSDP = rdsp;
	}
}



uintptr_t ACPI::findTablefromRSDT(RSDT *rsdt, const char *tableSignature) {
	uintptr_t entriesStart = (uintptr_t)&(rsdt->entries);
	uintptr_t entriesEnd = entriesStart + ((uintptr_t)rsdt->length - 36);

	printMemory((uintptr_t)rsdt, rsdt->length);

	DEBUG_STREAM(TAG,"RSDT with entries from:" << hex << entriesStart << " to: " << entriesEnd);

	while (entriesStart < entriesEnd) {
		if (strncmp((char*)tableSignature, (char*)(uintptr_t)(*(uint32_t*)(uintptr_t)entriesStart), 4)) {
			DEBUG_STREAM(TAG,"Found table: " << tableSignature << " at: " << (*(uint32_t*)(uintptr_t)entriesStart));
			return (*(uint32_t*)(uintptr_t)entriesStart);
		}
		entriesStart += 4;
	}
	return 0;
}


uintptr_t ACPI::findTablefromXSDT(XSDT *xsdt, const char *tableSignature) {
	uintptr_t entriesStart = (uintptr_t)&(xsdt->entries);
	uintptr_t entriesEnd = entriesStart + ((uintptr_t)xsdt->length - 36);

	printMemory((uintptr_t)xsdt, xsdt->length);

	DEBUG_STREAM(TAG,"XSDT with entries from:" << hex << entriesStart << " to: " << entriesEnd);

	while (entriesStart < entriesEnd) {
		if (strncmp((char*)tableSignature, (char*)(uintptr_t)(*(uint64_t*)(uintptr_t)entriesStart), 4)) {
			DEBUG_STREAM(TAG,"Found table: " << tableSignature << " at: " << (*(uint64_t*)(uintptr_t)entriesStart));
			return (*(uint64_t*)(uintptr_t)entriesStart);
		}
		entriesStart += 8;
	}
	return 0;
}

MADT_Entry *ACPI::getNextEntry(MADT_Entry *entry) {
	MADT_Entry *retVal = 0;

	// Check if given entry is the MADT itself, then return the first MADT_entry
	if(entry == (MADT_Entry*)mMADT) {
		return (MADT_Entry*)((uintptr_t)mMADT + 44);
	}

	retVal = (MADT_Entry*)((uintptr_t)entry + entry->length);
	if (retVal < (MADT_Entry*)((uintptr_t)mMADT+mMADT->length)) {
		return retVal;
	} else {
		return 0;
	}
}
