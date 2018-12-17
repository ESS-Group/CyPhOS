/*
 * ARMV7PerformanceCounter.h
 *
 *  Created on: 11.02.2016
 *      Author: hendrik
 */

#ifndef DRIVER_ARMV7PERFORMANCECOUNTER_H_
#define DRIVER_ARMV7PERFORMANCECOUNTER_H_


class ARMV7PerformanceCounter {
public:
	/* Event type values */
	enum class ARM_event_t {
		EVENT_SOFTWARE_INCREMENT = 0x00,
		EVENT_INSTRUCTION_CACHE_MISS = 0x01,
		EVENT_INSTRUCTION_MICRO_TLB_MISS = 0x02,
		EVENT_DATA_CACHE_MISS = 0x03,
		EVENT_DATA_CACHE_ACCESS = 0x04,
		EVENT_DATA_CACHE_MICRO_TLB_MISS = 0x05,
		EVENT_DATA_READ = 0x06,
		EVENT_DATA_WRITE = 0x07,
		EVENT_EXCEPTION_TAKEN = 0x09,
		EVENT_EXCEPTION_RETURN = 0x0A,
		EVENT_WRITE_CONTEXT_ID = 0x0B,
		EVENT_SOFTWARE_CHANGE_OF_PC = 0x0C,
		EVENT_IMMEDIATE_BRANCH = 0x0D,
		EVENT_UNALIGNED_LOAD_STORE = 0x0F,
		EVENT_BRANCH_MISPREDICTED_NOTPREDICTED = 0x10,
		EVENT_CYCLE_COUNT = 0x11,
		EVENT_PREDICTABLE_BRANCHES = 0x12,
		EVENT_MEMORY_ACCESS = 0x13,
		EVENT_LEVEL_1_INSTRUCTION_ACCESS = 0x14,
		EVENT_LEVEL_1_DATA_WRITEBACK = 0x15,
		EVENT_LEVEL_2_DATA_ACCESS = 0x16,
		EVENT_LEVEL_2_DATA_REFILL = 0x17,
		EVENT_LEVEL_2_DATA_WRITEBACK = 0x18,
		EVENT_BUS_ACCESS = 0x19,
		EVENT_MEMORY_ERRORY = 0x1A,
		EVENT_INSTRUCTION_SPECULATIVELY_EXECUTED = 0x1B,
		EVENT_WRITE_TO_TTBR = 0x1C,
		EVENT_BUS_CYCLES = 0x1D,
		EVENT_ISSUE_NOT_DISPATCHING = 0x66,
		EVENT_ISSUE_IS_EMPTY = 0x67,
		EVENT_PREFETCHED_CACHE_LINES = 0x6B,
		EVENT_DATA_LINEFILLS = 0x69,
		EVENT_MAIN_EXECUTION_UNIT_INS_COUNT = 0x70,
		EVENT_SECOND_EXECUTION_UNIT_INS_COUNT = 0x71,
		EVENT_LOAD_STORE_INSTRUCTIONS = 0x72,
	};


	ARMV7PerformanceCounter();

	/**
	 * Dumps some identification information
	 */
	void printInformation();

	/**
	 * Read the programmable event counter with id.
	 */
	dword_t readEventCounter(byte_t id);
	/**
	 * Writes a value to the event counter with id.
	 */
	void writeEventCounter(dword_t value, byte_t id);
	/**
	 * Enables the event counter with id.
	 */
	void enableEventCounter(byte_t id);

	/**
	 * Disables the event counter with id.
	 */
	void disableEventCounter(byte_t id);

	/**
	 * Reads the type to count for event counter with id.
	 */
	ARMV7PerformanceCounter::ARM_event_t readEventCounterType(byte_t id);

	/**
	 * Writes the type to count for event counter with id.
	 */
	void writeEventCounterType(ARM_event_t value, byte_t id);

	/**
	 * Reads the cycle count register. Core banked.
	 */
	dword_t readCycleCountRegister();

	/**
	 * Writes the cycle count register.
	 */
	void writeCycleCountRegister(dword_t value);

	/**
	 * Set if the cycle counter should only count every 64th step to make
	 * room for more cycles.
	 */
	void set64CycleCounterStep(bool enabled);

	/**
	 * Enables the cycle counter. Banked per core
	 */
	void enableCycleCount();

	/**
	 * Disables the cycle counter
	 *  */
	void disableCycleCount();

	/**
	 * Enable the global PMU
	 */
	void enableGlobal();

	/**
	 * Disable the global PMU
	 */
	void disableGlobal();

	/**
	 * Resets the PMU
	 */
	void resetGlobal();


	/**
	 * Prints all the counter values.
	 */
	void printCounterValues();



	// Static instance
	static ARMV7PerformanceCounter pInstance;
private:
	dword_t readID_DFR0();
	dword_t readPMOVSR();
	dword_t readPMCR();
	void writePMCR(dword_t value);
	dword_t readHDCR();
	dword_t readSDER();
	dword_t readEventCounterEnable();
	dword_t readDBGAUTHSTATUS();

};

#endif /* DRIVER_ARMV7PERFORMANCECOUNTER_H_ */
