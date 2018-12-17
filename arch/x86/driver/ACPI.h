/*
 * ACPI.h
 *
 *  Created on: 07.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_ACPI_H_
#define ARCH_X86_DRIVER_ACPI_H_

#include <arch/x86/driver/ACPItables.h>
#include <common/types.h>


class ACPI {
public:
	ACPI();
	void initACPI();

	void registerExternalRSDP(RSDP2 *rdsp);

	static ACPI mInstances[NR_CPUS];

	uintptr_t getIOAPICAddress();
private:
	enum MADT_ENTRY_TYPES_T {
		MADT_ENTRY_LOCAL_APIC = 0x0,
		MADT_ENTRY_IOAPIC = 0x1,
		MADT_ENTRY_INTERRUPT_SOURCE_OVERRIDE = 0x2,
		MADT_ENTRY_NMI_SOURCE = 0x3,
		MADT_ENTRY_LOCAL_APIC_NMI = 0x4,
		MADT_ENTRY_LOCAL_APIC_OVERRIDE = 0x5,
		MADT_ENTRY_IO_SAPIC = 0x6,
		MADT_ENTRY_LOCAL_SAPIC = 0x7,
		MADT_ENTRY_PLATFORM_INTERRUPT_SOURCES = 0x8
	};

	/* ACPI constants */
	const char *rsdp_signature = "RSD PTR ";
	const char *rsdt_signature = "RSDT";
	const char *xsdt_signature = "XSDT";
	const char *madt_signature = "APIC";

	const RSDP2 *rsdp2_base_address = (RSDP2*)0x400e;

	uintptr_t findRSDP();

	uintptr_t findRSDP(uintptr_t start, uint64_t length);

	uintptr_t findTablefromRSDT(RSDT *rsdt, const char *tableSignature);
	uintptr_t findTablefromXSDT(XSDT *xsdt, const char *tableSignature);

	RSDP2 *mRSDP;
	RSDT *mRSDT;
	XSDT *mXSDT;

	MADT *mMADT;

	bool checkRSDPValidity(RSDP2 *rsdp);



	/**
	 *
	 * @return
	 */
	MADT_Entry *getNextEntry(MADT_Entry *entry);

};

#endif /* ARCH_X86_DRIVER_ACPI_H_ */
