/*
 * tables.h
 *
 *  Created on: 07.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_ACPITABLES_H_
#define ARCH_X86_DRIVER_ACPITABLES_H_

#include <common/types.h>


struct RSDP2 {
 char signature[8];
 uint8_t checksum;
 char oemID[6];
 uint8_t revision;
 uint32_t rsdtAddress;
 uint32_t length;
 uint64_t xsdtAddress;
 uint8_t extendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed));

struct RSDT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemID[6];
	char oemTableID[8];
	char oemRevision[4];
	char creatorID[4];
	char creatorRevision[4];
	uint32_t entries;
} __attribute__ ((packed));

struct XSDT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemID[6];
	char oemTableID[8];
	char oemRevision[4];
	char creatorID[4];
	char creatorRevision[4];
	uint64_t entries;
} __attribute__ ((packed));


struct MADT {
	char signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oemID[6];
	char oemTableID[8];
	char oemRevision[4];
	char creatorID[4];
	char creatorRevision[4];
	uint32_t localInterruptControllerAddress;
	uint32_t flags;
	uint32_t entries;
} __attribute__ ((packed));


struct MADT_Entry {
	uint8_t type;
	uint8_t length;
} __attribute__ ((packed));

struct MADT_Entry_Processor_Local_APIC {
	MADT_Entry baseEntry;
	uint8_t acpiProcessorID;
	uint8_t apicID;
	uint32_t flags;
} __attribute__ ((packed));

struct MADT_Entry_IOAPIC {
	MADT_Entry baseEntry;
	uint8_t ioAPICID;
	uint8_t reserved;
	uint32_t ioAPICAddress;
	uint32_t globalSystemInterruptBase;
} __attribute__ ((packed));

struct MADT_Entry_NMI {
	MADT_Entry baseEntry;
	uint8_t acpiProcessorID;
	uint16_t flags;
	uint8_t lint;
} __attribute__ ((packed));

struct MADT_Entry_LAPIC_Address_Override {
	MADT_Entry baseEntry;
	uint16_t reserved;
	uint64_t longmodeLAPICAddress;
} __attribute__ ((packed));

#endif /* ARCH_X86_DRIVER_ACPITABLES_H_ */
