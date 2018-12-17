/*
 * IOAPIC.h
 *
 *  Created on: 14.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_IOAPIC_H_
#define ARCH_X86_DRIVER_IOAPIC_H_

#include <common/types.h>

class IOAPIC {
public:
	struct IOAPIC_ID_REGISTER_t {
		uint32_t reserved_1 :24, ioapicID :4, reserved_2 :4;
	}__attribute__((packed));

	struct IOAPIC_VERSION_REGISTER_t {
		uint32_t version :8, reserved_1 :8, maximumRedirectionEntries :8,
				reserved_2 :8;
	}__attribute__((packed));

	struct IOAPIC_ARBITRATION_REGISTER_t {
		uint32_t reserved_1 :24, ioapicID :4, reserved_2 :4;
	}__attribute__((packed));

	struct IOAPIC_ARBITRATION_ENTRY_t {
		uint32_t interruptVector :8, deliveryMode :3, destinationMode :1,
				deliveryStatus :1, inputPinPolarity :1,remoteIRR:1, triggerMode :1,
				interruptMask :1, reserved :15;
		union {
			struct {
				uint32_t reserved :24, destinationAPICID :4, reserved_2 :4;
			} physicalMode __attribute__((packed));
			struct {
				uint32_t reserved :24, logicalDestination :8;
			} logicalMode __attribute__((packed));
		} destination;
	}__attribute__((packed));

	enum DELIVERY_MODE_t {
		DELIVERY_MODE_FIXED,
		DELIVERY_MODE_LOWEST_PRIORITY,
		DELIVERY_MODE_SMI,
		DELIVERY_MODE_RESERVED,
		DELIVERY_MODE_NMI,
		DELIVERY_MODE_INIT,
		DELIVERY_MODE_RESERVED_2,
		DELIVERY_MODE_EXT_INT
	};

	enum DESTINATION_MODE_t {
		DESTINATION_PHYSICAL = 0x0,
		DESTINATION_LOGICAL = 0x1
	};

	enum INTERRUPT_INPUT_LINES {
		// Legacy interrupt 1
		INTERRUPT_INPUT_KEYBOARD = 0x1,
		// UART 1
		INTERRUPT_INPUT_UART_1 = 0x4,
	};

	enum INTERRUPT_MASK_t {
		UNMASKED = 0x0,
		MASKED = 0x1
	};

	enum TRIGGER_MODE_t {
		TRIGGER_EDGE_SENSITIVE = 0x0,
		TRIGGER_LEVEL_SENSITIVE = 0x1,
	};

	enum INTERRUPT_PIN_POLARITY_t {
		POLARITY_HIGH_ACTIVE = 0x0,
		POLARITY_LOW_ACTIVE = 0x1,
	};
private:

	/**
	 * Defines a maximum of IOAPIC instances. There are probably never more IOAPICs than CPU cores
	 */
	static constexpr uint32_t cMAX_INSTANCES = NR_CPUS;

	/**
	 * IOAPICs id.
	 */
	uint8_t mID;

	/**
	 * Base address of an IOAPIC instance
	 */
	uintptr_t mBaseAddress;

	/**
	 * Starting point of the interrupt number the IOAPIC handles
	 */
	uint32_t mGlobalInterruptBase;

	/**
	 * Number of discovered and configured IOAPIC units
	 */
	static uint32_t sRegisteredIOAPICs;

	/**
	 * Reads a 32 bit register from the IOAPIC
	 * @param offset Register address WITHIN the IOAPIC
	 * @return Value of the IOAPIC internal register
	 */
	uint32_t readIOAPICRegister(uint16_t offset);

	/**
	 * Writes an internal IOAPIC register
	 * @param offset Internal address of the IOAPIC register
	 * @param value to write to the IOAPIC internal register
	 */
	void writeIOAPICRegister(uint16_t offset, uint32_t value);

	/*
	 * Register getter methods
	 */
	IOAPIC_ID_REGISTER_t getIDRegister();
	IOAPIC_VERSION_REGISTER_t getVersionRegister();
	IOAPIC_ARBITRATION_REGISTER_t getArbitrationRegister();

	/*
	 * Register setter methods
	 */
	void setIDRegister(IOAPIC::IOAPIC_ID_REGISTER_t regVal);

	/**
	 *
	 * @return Length of the redirection entry table
	 */
	uint8_t getMaximumRedirectionEntryCount();

	/**
	 * Returns redirection entry with given index
	 * @param index Offset in table
	 * @return Entry at the offset in the table
	 */
	IOAPIC_ARBITRATION_ENTRY_t getRedirectionEntry(uint8_t index);

	/**
	 * Sets the redirection entry at index to the given entry
	 * @param index Offset in table
	 * @param entry Entry to set
	 */
	void setRedirectionEntry(uint8_t index, IOAPIC::IOAPIC_ARBITRATION_ENTRY_t &entry);

	/**
	 * Private initialization method. Resets all redirection entries
	 */
	void initialize();

	void setID(uint8_t ioapicID);
public:
	/**
	 * Static instances
	 */
	static IOAPIC mInstances[cMAX_INSTANCES];

	/**
	 * Register offsets
	 */
	static constexpr uint16_t cREG_ID = 0x00;
	static constexpr uint16_t cREG_VERSION = 0x01;
	static constexpr uint16_t cREG_ARB = 0x02;
	static constexpr uint16_t cREG_REDIRECTION_TABLE_START = 0x10;

	/**
	 * MMIO register offsets
	 */
	static constexpr uint16_t cREG_SEL = 0x00;
	static constexpr uint16_t cREG_WIN = 0x10;

	IOAPIC();

	/**
	 * Called from ACPI parsing of the MADT table entries. For each discovered IOAPIC this method
	 * should be called.
	 * @param baseAddress IOAPIC address in memory space
	 * @param ioapicID Unique IOAPIC ID
	 * @param globalInterruptBase Starting number of interrupt inputs where this IOAPIC handles interrupts from
	 */
	static void configureNewInstance(uintptr_t baseAddress, uint8_t ioapicID,
			uint32_t globalInterruptBase);

	void setInterruptMask(uint8_t slot, IOAPIC::INTERRUPT_MASK_t mask);

	void printRegisters();
};

#endif /* ARCH_X86_DRIVER_IOAPIC_H_ */
