#ifndef __ARMINTERRUPTCONTROLLER_H__
#define __ARMINTERRUPTCONTROLLER_H__

/* Include OSC based design header */
#include <component/OSC.h>

#include <common/types.h>

#include <common/ARM_common.h>

// ARM GIC Stuff
#ifndef GICD_BASE_ADDRESS
#define GICD_BASE_ADDRESS 0x0
#error No GICD_BASE_ADDRESS
#endif

#ifndef GICC_BASE_ADDRESS
#define GICC_BASE_ADDRESS 0x0
#error No GICC_BASE_ADDRESS
#endif


#ifndef GIC_NO_BANKED_REGISTERS
#define CORE_BANK_OFFSET 0x0
#else
/* Exynos 4412 specific offset, check for other platforms */
#define CORE_BANK_OFFSET (getCPUID() * 0x4000)
#endif

#define TLB_FLUSH_INTERRUPT_ID 2

// Distributor registers
#define GICD_CTLR GICD_BASE_ADDRESS + 0x000
#define GICD_TYPER GICD_BASE_ADDRESS + 0x004
#define GICD_IIDR GICD_BASE_ADDRESS + 0x008
#define GICD_SGIR GICD_BASE_ADDRESS + 0xF00

#define GICD_IGROUPRn(a) GICD_BASE_ADDRESS + 0x084 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ISENABLERn(a) GICD_BASE_ADDRESS + 0x100 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ICENABLERn(a) GICD_BASE_ADDRESS + 0x180 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ISPENDRn(a) GICD_BASE_ADDRESS + 0x200 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ICPENDRn(a) GICD_BASE_ADDRESS + 0x280 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ICFGRn(a) GICD_BASE_ADDRESS + 0xC00 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_IPRIORITYRn(a) GICD_BASE_ADDRESS + 0x400 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ITARGETSRn(a) GICD_BASE_ADDRESS + 0x800 + (0x4 * a) + CORE_BANK_OFFSET
#define GICD_ICSPISRn(a) GICD_BASE_ADDRESS + 0xD04 + (0x4 * a) + CORE_BANK_OFFSET

// Core registers
#define GICC_CTRL GICC_BASE_ADDRESS + 0x00 + CORE_BANK_OFFSET
#define GICC_PMR  GICC_BASE_ADDRESS + 0x04 + CORE_BANK_OFFSET
#define GICC_IIDR GICC_BASE_ADDRESS + 0xFC + CORE_BANK_OFFSET
#define GICC_BPR  GICC_BASE_ADDRESS + 0x08 + CORE_BANK_OFFSET
#define GICC_IAR  GICC_BASE_ADDRESS + 0x0C + CORE_BANK_OFFSET
#define GICC_EOIR GICC_BASE_ADDRESS + 0x10 + CORE_BANK_OFFSET
#define GICC_RPR GICC_BASE_ADDRESS + 0x14 + CORE_BANK_OFFSET
#define GICC_HPPIR GICC_BASE_ADDRESS + 0x18 + CORE_BANK_OFFSET
#define GICC_AIAR GICC_BASE_ADDRESS + 0x20 + CORE_BANK_OFFSET
#define GICC_DIR GICC_BASE_ADDRESS + 0x1000 + CORE_BANK_OFFSET

#define SPURIOUS_INT 0x3ff

MEMBER_OF_OSC_BEGIN(ARMGIC)

class ARMGIC {
private:
	ARMGIC(const ARMGIC &copy);

	uint32_t mNumLines;


public:
	/**
	 * Constructor for ARM GIC driver
	 */
	ARMGIC();

	void init(void);
	/**
	 * Initializes the main distributor interface
	 */
	void distributorInit(void);

	/**
	 * Initializes the core specific interface
	 */
	void coreInterfaceInit(void);

	/**
	 * Enables a specific interrupt id globally inside the distributor
	 */
	bool enableInterrupt(uint32_t interrupt);
	/**
	 * Disables a specific interrupt id globally inside the distributor
	 */
	bool disableInterrupt(uint32_t interrupt);

	dword_t getNextInterrupt(void);
	dword_t getGICCAIAR();

	void setProcessorPriorityMask(dword_t mask);

	void setCoreControl(bool enable);

	void setDistributorControl(bool enable);

	void testInterrupt(uint32_t interrupt);

	void configureInterruptTrigger(uint32_t interrupt, bool levelTriggered);
	void configureInterruptHandling(uint32_t interrupt, bool nton);
	void setInterruptPriority(uint32_t interrupt, byte_t priority);
	void setInterruptTarget(uint32_t interrupt, byte_t target);
	byte_t getInterruptTarget(uint32_t interrupt);
	void setInterruptGroup(uint32_t interrupt, byte_t group);
	byte_t getInterruptGroup(uint32_t interrupt);
	void enableInterruptTarget(uint32_t interrupt, byte_t target);
	void disableInterruptTarget(uint32_t interrupt, byte_t target);
	void clearPending(uint32_t interrupt);
	void printPending();

	void endOfInterrupt(uint32_t interrupt);

	void dumpStatus();
	void dumpCoreStatus();


	void generateSoftwareInterrupt(byte_t id, byte_t target);
	void generateSoftwareInterruptToAll(byte_t id);

	/** Static instance variable */
	static ARMGIC pInstance;
};

MEMBER_OF_OSC_END

#endif // __ARMINTERRUPTCONTROLLER_H__
