/*
 * ARMv7PageTable.h
 *
 *  Created on: 14.02.2019
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_DRIVER_ARMV7PAGETABLE_H_
#define ARCH_ARMV7_DRIVER_ARMV7PAGETABLE_H_

class ARMv7PageTable {
	friend class ARMMMU;
public:
	struct firstLevelDescriptor_t {
		uint32_t one_1:1, zero_1:1, pxn:1, ns:1, sbz_1:1, domain:4, impl:1, secondLevelPageTableBaseAddress:22;
	}__attribute__((packed));

	struct secondLevelDescriptor_t {
			uint32_t xn:1, one_1:1, b:1, c:1, ap_1:2, tex:3, ap_2:1, s:1, nG:1, smallPageBaseAddress:20;
	}__attribute__((packed));

	firstLevelDescriptor_t *getFirstLevelBaseAddress() {
		return firstLevelTable;
	}

	secondLevelDescriptor_t *getSecondLevelBaseAddress() {
		return secondLevelTable;
	}

	void fillLinear();
	static ARMv7PageTable sInstance;
private:
	static constexpr uint32_t cFIRST_LEVEL_ENTRIES= 4096;
	static constexpr uint32_t cSECOND_LEVEL_ENTIRES = 256;
	static constexpr uint32_t cFIRST_LEVEL_FLAGS = 0x1;
	static constexpr uint32_t cSECOND_LEVEL_FLAGS = 0x402;
	firstLevelDescriptor_t __attribute__ ((aligned (0x10000))) firstLevelTable[cFIRST_LEVEL_ENTRIES];
	secondLevelDescriptor_t __attribute__ ((aligned (0x10000))) secondLevelTable[cSECOND_LEVEL_ENTIRES * cFIRST_LEVEL_ENTRIES];

};

#endif /* ARCH_ARMV7_DRIVER_ARMV7PAGETABLE_H_ */
