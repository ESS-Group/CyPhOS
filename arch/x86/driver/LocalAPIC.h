/*
 * LocalAPIC.h
 *
 *  Created on: 11.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_LOCALAPIC_H_
#define ARCH_X86_DRIVER_LOCALAPIC_H_

#include <common/types.h>
#include <common/memreg.h>

extern "C" {
	uint8_t getLocalAPICID();
}

class LocalAPIC {
public:
	static constexpr uintptr_t cAPIC_REG_ID = 0x20;
	static constexpr uintptr_t cAPIC_REG_VERSION = 0x30;
	static constexpr uintptr_t cAPIC_REG_TASK_PRIORITY = 0x80;
	static constexpr uintptr_t cAPIC_REG_ARBITRATION_PRIORITY = 0x90;
	static constexpr uintptr_t cAPIC_REG_PROCESS_PRIORITY = 0xA0;
	static constexpr uintptr_t cAPIC_REG_END_OF_INTERRUPT = 0xB0;
	static constexpr uintptr_t cAPIC_REG_REMOTE_READ = 0xC0;
	static constexpr uintptr_t cAPIC_REG_LOCAL_DESTINATION = 0xD0;
	static constexpr uintptr_t cAPIC_REG_DESTINATION_FORMAT = 0xE0;
	static constexpr uintptr_t cAPIC_REG_SPURIOUS_INTERRUPT_VECTOR = 0xF0;

	static constexpr uintptr_t cAPIC_REG_ERROR_STATUS = 0x280;
	static constexpr uintptr_t cAPIC_REG_INTERRUPT_COMMAND_LOW = 0x300;
	static constexpr uintptr_t cAPIC_REG_INTERRUPT_COMMAND_HIGH = 0x310;

	static constexpr uintptr_t cAPIC_REG_TIMER_LOCAL_VECTOR = 0x320;
	static constexpr uintptr_t cAPIC_REG_TIMER_CURRENT_COUNT = 0x390;
	static constexpr uintptr_t cAPIC_REG_TIMER_INITIAL_COUNT = 0x380;
	static constexpr uintptr_t cAPIC_REG_TIMER_DIVIDER_CONFIGURATION = 0x3E0;

	static constexpr uint32_t cMSR_APIC_BASE_ADDRESS_REGISTER = 0x0000001B;

	struct APIC_BASE_REGISTER_t {
		uint64_t reserved_3:8, bsc:1, reserved_2:2, ae:1, aba:40, reserved:12;
	} __attribute__((packed));

	struct APIC_VERSION_REGISTER_t {
		uint32_t version:8, reserved_1:8,mle:8,reserved_2:7, eas:1;
	} __attribute__((packed));

	struct APIC_SPURIOUS_VECTOR_REGISTER_t {
		uint32_t vector:8,ase:1,fcc:1, reserved:22;
	} __attribute__((packed));

	struct APIC_ICR_REGISTER_LOW_t {
		uint32_t vector :8, messageType :3, destinationMode :1, deliveryStatus :1, reservedZero :1, level :1, triggerMode :1, remoteReadStatus :2, destinationShortHand :2, reserved_2 :12;
	}__attribute__((packed));

	struct APIC_ICR_REGISTER_HIGH_t {
		uint32_t reserved :24, destination :8;
	}__attribute__((packed));

	struct APIC_TIMER_LVT_REGISTER_t {
		uint32_t vec:8,reserved_1:4, ds:1,reserved_2:3, m:1, tmm:1, reserved_3:14;
	}__attribute__((packed));

	enum DESTINATION_MODE_t {
		DESTINATION_MODE_CLUSTER = 0x0,
		DESTINATION_MODE_FLAT = 0xF
	};

	enum MESSAGE_TYPE_t {
		MESSAGE_TYPE_FIXED = 0x0,
		MESSAGE_TYPE_LOWEST_PRIORITY = 0x1,
		MESSAGE_TYPE_SMI = 0x2,
		MESSAGE_TYPE_REMOTE_READ = 0x3,
		MESSAGE_TYPE_NMI = 0x4,
		MESSAGE_TYPE_INIT = 0x5,
		MESSAGE_TYPE_STARTUP = 0x6,
		MESSAGE_TYPE_EXTERNAL_INTERRUPT = 0x7
	};

	enum ICR_TRIGGER_MODE_t {
		ICR_TRIGGER_EDGE = 0x0,
		ICR_TRIGGER_LEVEL = 0x1
	};

	enum ICR_LEVEL_MODE_t {
		ICR_LEVEL_DEASSERT = 0x0,
		ICR_LEVEL_ASSERT = 0x1
	};

	enum TIMER_DIVIDER_VALUE_t {
		DIVIDE_BY_2 = 0x0,
		DIVIDE_BY_4 = 0x1,
		DIVIDE_BY_8 = 0x2,
		DIVIDE_BY_16 = 0x3,
		DIVIDE_BY_32 = 0x4,
		DIVIDE_BY_64 = 0x5,
		DIVIDE_BY_128 = 0x6,
		DIVIDE_BY_1 = 0x7
	};

	enum TIMER_MODE_VALUE_t {
		TIMER_MODE_ONESHOT = 0x0,
		TIMER_MODE_PERIODIC = 0x1
	};

	LocalAPIC();

	/**
	 * Sends an End of interrupt to the LocalAPIC. Acknowledges the interrupt
	 */
	void sendEOI();

	/**
	 * Prints debug information about the LocalAPIC
	 */
	void printDebug();

	/**
	 * Prints the current state of important LocalAPIC registers
	 */
	void printRegisters();


	/**
	 * Sets the destination mode as logical or physical
	 * @param mode
	 */
	void setDestinationMode(DESTINATION_MODE_t mode);

	/**
	 * Initializes a core local APIC
	 */
	void initialize();

	/**
	 * Can be used to send software controlled interrupts to other processors
	 * or the own processor.
	 * @param destination Destination for the interrupt
	 * @param icrReg Parameters for the interrupt to send
	 */
	void sendIPI(uint8_t destination, APIC_ICR_REGISTER_LOW_t icrReg);

	/**
	 * Returns the unique processor APIC ID
	 * @return
	 */
	inline uint8_t getAPICID() {
		return (readRegister_32((uint32_t*)(mBaseAddress + cAPIC_REG_ID)) >> 24);
	}

	uint8_t getIPIDeliveryStatus();

	/**
	 * Sets the hardware timer divider value
	 * @param dividerVal
	 */
	void setTimerDivider(TIMER_DIVIDER_VALUE_t dividerVal);


	/**
	 * Sets the timebase parameter used for timing calculations
	 * Determined at startup via the PIC.
	 * FIXME Use a more precise calibration
	 * @param timeBase
	 */
	void setTimebase(uint64_t timebase);

	/**
	 * Configures the local APIC to fire periodic interrupts with an interval of
	 * @param interval Interval in us
	 */
	void windupPeriodic(uint64_t interval);

	void setTimerInterrupt(bool enable);

	/**
	 * Static instance. Can be used across multiple cores because the class is pretty much
	 * stateless.
	 */
	static LocalAPIC mInstance;
private:
	uintptr_t mBaseAddress;

	/**
	 * Time of one tick in ps. Can be used to set timer interval
	 */
	uint64_t mTimeBase;

	void setTaskPriority(uint8_t taskPriority, uint8_t taskPrioritySubClass);

	void calibrate();
};

#endif /* ARCH_X86_DRIVER_LOCALAPIC_H_ */

