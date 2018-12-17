#include <driver/ARMGIC.h>

#include <common/armhelper.h>

#include <common/memreg.h>
#include <common/bitmanipulation.h>

#include <common/debug.h>

#define TAG "GIC"

MEMBER_OF_OSC_BEGIN(ARMGIC)

// Static instance
ARMGIC ARMGIC::pInstance;

ARMGIC::ARMGIC() : mNumLines(0) {}

void ARMGIC::init(void) {
	distributorInit();
	dumpStatus();
}

void ARMGIC::distributorInit(void) {
	// Disable distributor
	WRITE_REGISTER(GICD_CTLR, 0x00000000);
	DEBUG_STREAM(TAG,"Initializing GIC-distributor");
	DEBUG_STREAM(TAG,"GICD_CTLR: " << hex << READ_REGISTER(GICD_CTLR));
	dword_t gicd_typer = READ_REGISTER(GICD_TYPER);
	DEBUG_STREAM(TAG,"GICD_TYPER: " << hex << gicd_typer);
	DEBUG_STREAM(TAG,"GICD_IIDR: " << hex << *((uint32_t *) (GICD_IIDR)));

	mNumLines = 0x1F & gicd_typer;
	DEBUG_STREAM(TAG,"Number int lines: " << hex << mNumLines);
	DEBUG_STREAM(TAG,"Resulting interrupts: " << dec << (mNumLines + 1) * 32);

	uint32_t numInts = (mNumLines + 1) * 32;

	DEBUG_STREAM(TAG,"initializing interrupt controller");

	// Set interrupts to level triggered
	for (uint32_t i = 32; i < numInts; i++) {
		configureInterruptTrigger(i, true);
		configureInterruptHandling(i, false);
		//DEBUG_STREAM(TAG,"Configured int: " << i << '\n';
	}

	for (uint32_t i = 32; i < numInts; i++) {
		setInterruptTarget(i, 0x0);
		//DEBUG_STREAM(TAG,"Configured int target: " << i << '\n';
	}

	for (uint32_t i = 0; i < numInts; i++) {
		setInterruptPriority(i, 0xa0);
		//DEBUG_STREAM(TAG,"Configured int priority: " << i << '\n';
	}

	for (uint32_t i = 0; i < numInts; i++) {
		disableInterrupt(i);
	}

	// clear pending
	for (uint32_t i = 0; i < numInts; i++) {
		clearPending(i);
	}

	/*for ( uint32_t i = 0; i < numInts; i++ ) {
		setInterruptGroup(i,1);
	}*/


}

void ARMGIC::coreInterfaceInit() {
	DEBUG_STREAM(TAG,"Initializing GIC-coreInterface");
	// Disable
	WRITE_REGISTER(GICC_CTRL, 0x00000000);
	// clear banked pending clear register
	WRITE_REGISTER(GICD_ICPENDRn(0), 0xFFFFFFFF);

	// Set binary point register for preemption
	WRITE_REGISTER(GICC_BPR, 0x00000000);

	// Clear pending
	dword_t pendingInt;
	do {
		pendingInt = READ_REGISTER(GICC_IAR);
		DEBUG_STREAM(TAG,"pendingInt: " << hex << pendingInt);
		if (pendingInt == SPURIOUS_INT) {
			break;
		}
	} while (true);

	for ( unsigned int i = 0 ; i < 32; i++ ) {
		setInterruptPriority(i,0xa0);
		setInterruptTarget(i,(1 << getCPUID()));
	}
	setProcessorPriorityMask(0xF0);
//	int numInts = (mNumLines + 1) * 32;
}

bool ARMGIC::enableInterrupt(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = interrupt % 32;
	dword_t regVal =  (0x1 << bitNumber);
	WRITE_REGISTER(GICD_ISENABLERn(rowNumber), regVal);
//	DEBUG_STREAM(TAG,"Enabled int: " << dec << interrupt);
//	for (unsigned int i = 0; i < mNumLines + 1; i++) {
//		DEBUG_STREAM(TAG,"GICD_ISENABLER[" << dec << i << "]: " << hex
//				<< READ_REGISTER(GICD_ISENABLERn(i)));
//	}
	return true;
}

bool ARMGIC::disableInterrupt(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = interrupt % 32;
	dword_t regVal =  (0x1 << bitNumber);
	WRITE_REGISTER(GICD_ICENABLERn(rowNumber), regVal);
//	DEBUG_STREAM(TAG,"Disabled int: " << dec << interrupt);
//	for (unsigned int i = 0; i < mNumLines + 1; i++) {
//			DEBUG_STREAM(TAG,"GICD_ISENABLER[" << dec << i << "]: " << hex
//					<< READ_REGISTER(GICD_ISENABLERn(i)));
//		}
	return true;
}

dword_t ARMGIC::getNextInterrupt(void) {

	dword_t pendingInt = READ_REGISTER(GICC_IAR);
	return pendingInt;
}

dword_t ARMGIC::getGICCAIAR() {
	dword_t pendingInt = READ_REGISTER(GICC_AIAR);
	return pendingInt;
}

void ARMGIC::setProcessorPriorityMask(dword_t mask) {
//	DEBUG_STREAM(TAG,"Set processor priority mask: " << hex << mask << '\n';
	WRITE_REGISTER(GICC_PMR, mask);
}

void ARMGIC::setCoreControl(bool enable) {
//	DEBUG_STREAM(TAG,"Enabling core interrupts: " << enable << '\n';
	dword_t regVal = READ_REGISTER(GICC_CTRL);
	/* Group0 bit enable */
	regVal = setbit(regVal, 0, enable);
	/* Group1 bit enable */
	regVal = setbit(regVal, 1, enable);
	WRITE_REGISTER(GICC_CTRL, regVal);
}

void ARMGIC::setDistributorControl(bool enable) {
//	DEBUG_STREAM(TAG,"Enabling distributor interrupts: " << enable << '\n';
	dword_t regVal = READ_REGISTER(GICD_CTLR);
	regVal = setbit(regVal, 0, enable);
	regVal = setbit(regVal, 1, enable);
	WRITE_REGISTER(GICD_CTLR, regVal);
}

void ARMGIC::testInterrupt(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = (interrupt % 32);

	dword_t regVal = READ_REGISTER(GICD_ISPENDRn(rowNumber));
	regVal = setbit(regVal, bitNumber, true);
	WRITE_REGISTER(GICD_ISPENDRn(rowNumber), regVal);
}

void ARMGIC::configureInterruptTrigger(uint32_t interrupt, bool levelTriggered) {
	uint32_t rowNumber = interrupt / 16;
	uint32_t bitNumber = (interrupt % 16) + 1;

	dword_t confRegVal = READ_REGISTER(GICD_ICFGRn(rowNumber));
	confRegVal = setbit(confRegVal, bitNumber, !levelTriggered);
	WRITE_REGISTER(GICD_ICFGRn(rowNumber), confRegVal);
}

void ARMGIC::configureInterruptHandling(uint32_t interrupt, bool nton) {
	uint32_t rowNumber = interrupt / 16;
	uint32_t bitNumber = (interrupt % 16);

	dword_t confRegVal = READ_REGISTER(GICD_ICFGRn(rowNumber));
	confRegVal = setbit(confRegVal, bitNumber, !nton);
	WRITE_REGISTER(GICD_ICFGRn(rowNumber), confRegVal);
}

void ARMGIC::setInterruptTarget(uint32_t interrupt, byte_t target) {
	uint32_t rowNumber = interrupt / 4;
	uint32_t bitNumber = (interrupt % 4);

	uint32_t prioMask = 0 | (target << (bitNumber * 8));

	dword_t regVal = READ_REGISTER(GICD_ITARGETSRn(rowNumber));
	regVal = (regVal & ~(0xFF << bitNumber * 8));
	regVal = regVal | prioMask;

	WRITE_REGISTER(GICD_ITARGETSRn(rowNumber), regVal);
}

byte_t ARMGIC::getInterruptTarget(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 4;
	uint32_t bitNumber = (interrupt % 4);

	dword_t regVal = READ_REGISTER(GICD_ITARGETSRn(rowNumber));
	regVal = (regVal & (0xFF << bitNumber * 8));
	byte_t target = regVal >> bitNumber * 8;
	return target;
}

void ARMGIC::setInterruptPriority(uint32_t interrupt, byte_t priority) {
	uint32_t rowNumber = interrupt / 4;
	uint32_t bitNumber = (interrupt % 4);

	uint32_t prioMask = 0 | (priority << (bitNumber * 8));

	dword_t regVal = READ_REGISTER(GICD_IPRIORITYRn(rowNumber));
//	DEBUG_STREAM(TAG,"Priority value before: " << hex << regVal << '\n';
	regVal = (regVal & ~(0xF << (bitNumber * 8)));
	regVal = regVal | prioMask;
//	DEBUG_STREAM(TAG,"Priority value now: " << hex << regVal << '\n';
	WRITE_REGISTER(GICD_IPRIORITYRn(rowNumber), regVal);
}

void ARMGIC::clearPending(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = (interrupt % 32);

	dword_t pendVal = READ_REGISTER(GICD_ICPENDRn(rowNumber));
	pendVal = setbit(pendVal, bitNumber, true);
}

void ARMGIC::printPending() {
	for (uint32_t i = 0; i < mNumLines + 1; i++) {
		DEBUG_STREAM(TAG,"GICD_ISPENDR[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_ISPENDRn(i)));
	}
}

void ARMGIC::dumpStatus() {
	DEBUG_STREAM(TAG,"GICD_CTLR: " << READ_REGISTER(GICD_CTLR));
	DEBUG_STREAM(TAG,"GICD_TYPER: " << READ_REGISTER(GICD_TYPER));
	DEBUG_STREAM(TAG,"GICD_IIDR: " << READ_REGISTER(GICD_IIDR));
	for (unsigned int i = 0; i < mNumLines + 1; i++) {
		DEBUG_STREAM(TAG,"GICD_IGROUPR[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_IGROUPRn(i)));
	}
	for (unsigned int i = 0; i < 7; i++) {
		DEBUG_STREAM(TAG,"GICD_ICSPIR[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_ICSPISRn(i)));
	}
	for (unsigned int i = 0; i < mNumLines + 1; i++) {
		DEBUG_STREAM(TAG,"GICD_ISENABLER[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_ISENABLERn(i)));
	}
	for (unsigned int i = 0; i < mNumLines + 1; i++) {
		DEBUG_STREAM(TAG,"GICD_ICENABLER[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_ICENABLERn(i)));
	}
	printPending();
	for (unsigned int i = 0; i < 8 * (mNumLines + 1); i++) {
		DEBUG_STREAM(TAG,"GICD_ITARGETSR[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_ITARGETSRn(i)));
	}
	for (unsigned int i = 0; i < 8*(mNumLines + 1); i++) {
		DEBUG_STREAM(TAG,"GICD_IPRIORITYR[" << dec << i << "]: " << hex
				<< READ_REGISTER(GICD_IPRIORITYRn(i)));
	}

	/* Core specific registers */
	DEBUG_STREAM(TAG,"GICC_BPR: " << hex << READ_REGISTER(GICC_BPR));
}

void ARMGIC::endOfInterrupt(uint32_t interrupt) {
	WRITE_REGISTER(GICC_EOIR, interrupt);
}

void ARMGIC::generateSoftwareInterrupt(byte_t id, byte_t target) {
	// Generate a software interrupt for target id with interupt id
	dword_t sgiRegVal = 0 | target << 16;
	sgiRegVal |= (0xF & id);
	DEBUG_STREAM(TAG,"Setting SGI Reg to: " << hex << sgiRegVal);
	WRITE_REGISTER(GICD_SGIR, sgiRegVal);
}

void ARMGIC::generateSoftwareInterruptToAll(byte_t id) {
	// Generate a software interrupt for target id with interupt id
	dword_t sgiRegVal = 0x01000000;
	sgiRegVal |= (0xF & id);
	DEBUG_STREAM(TAG,"Setting SGI Reg to: " << hex << sgiRegVal);
	WRITE_REGISTER(GICD_SGIR, sgiRegVal);
}

void ARMGIC::enableInterruptTarget(uint32_t interrupt, byte_t target) {
	byte_t currentTarget = getInterruptTarget(interrupt);
	currentTarget |= target;
	setInterruptTarget(interrupt, currentTarget);
}

void ARMGIC::disableInterruptTarget(uint32_t interrupt, byte_t target) {
	byte_t currentTarget = getInterruptTarget(interrupt);
	currentTarget &= ~target;
	setInterruptTarget(interrupt, currentTarget);
}

void ARMGIC::setInterruptGroup(uint32_t interrupt, byte_t group) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = interrupt % 32;
	dword_t regVal = READ_REGISTER(GICD_IGROUPRn(rowNumber));
	regVal = setbit(regVal, bitNumber, group == 0 ? false : true);
	WRITE_REGISTER(GICD_IGROUPRn(rowNumber), regVal);
}

byte_t ARMGIC::getInterruptGroup(uint32_t interrupt) {
	uint32_t rowNumber = interrupt / 32;
	uint32_t bitNumber = interrupt % 32;
	dword_t regVal = READ_REGISTER(GICD_IGROUPRn(rowNumber));
	return (regVal & (1 << bitNumber)) >> bitNumber;
}

void ARMGIC::dumpCoreStatus() {
	DEBUG_STREAM(TAG,"GICC_CTRL:" << hex << READ_REGISTER(GICC_CTRL));
	DEBUG_STREAM(TAG,"GICC_PMR:" << hex << READ_REGISTER(GICC_PMR));
	DEBUG_STREAM(TAG,"GICC_BPR:" << hex << READ_REGISTER(GICC_BPR));
	DEBUG_STREAM(TAG,"GICC_IAR:" << hex << READ_REGISTER(GICC_IAR));
	DEBUG_STREAM(TAG,"GICC_RPR:" << hex << READ_REGISTER(GICC_RPR));
	DEBUG_STREAM(TAG,"GICC_HPPIR:" << hex << READ_REGISTER(GICC_HPPIR));


}

MEMBER_OF_OSC_END


