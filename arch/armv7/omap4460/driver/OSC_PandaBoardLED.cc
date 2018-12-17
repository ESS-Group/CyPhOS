/*
 * PandaBoardLED.cc
 *
 *  Created on: 26.08.2014
 *      Author: hb
 */
#include "OSC_PandaBoardLED.h"

#include <common/memreg.h>

BEGIN_OSC_IMPLEMENTATION(PandaBoardLED,PandaBoardLED)

PandaBoardLED PandaBoardLED::pInstance;

PandaBoardLED::PandaBoardLED() {
	// Enable GPIO modules
	initializeGPIO4();
	turnLEDD1(true);
	turnLEDD2(true);
}

void PandaBoardLED::initializeGPIO4()
{
	// Enable GPIO modules

	// Enable GPIO4 modules
	dword_t regVal = READ_REGISTER(GPIO4_SYSCONFIG);
	regVal |= 0x2;
	WRITE_REGISTER(GPIO4_SYSCONFIG,regVal);
	regVal = READ_REGISTER(GPIO4_SYSSTATUS);
	while ( !(regVal & 0x1) ) {
		regVal = READ_REGISTER(GPIO4_SYSSTATUS);
	}
	// set idle mode
	regVal = READ_REGISTER(GPIO4_SYSCONFIG);
	regVal &= ~(0x19);
	regVal |= 0x8;
	WRITE_REGISTER(GPIO4_SYSCONFIG,regVal);

	// Set clockgating-ratio
	regVal = READ_REGISTER(GPIO4_CTRL);
	regVal &= ~(0x7);
	WRITE_REGISTER(GPIO4_CTRL,regVal);
	// read output enable register
	regVal = READ_REGISTER(GPIO4_OE);
	regVal &= ~(0x4000); // enable bit 14 for GPIO4_GPIO_110
	WRITE_REGISTER(GPIO4_OE,regVal);

	// Set mux to output
	regVal = READ_REGISTER(CONTROL_CORE_PAD0_SDMMC1_DAT7_PAD1_ABE_MCBSP2_CLKX);
	regVal &= ~(0x70000);
	regVal |= 0x30000;
	WRITE_REGISTER(CONTROL_CORE_PAD0_SDMMC1_DAT7_PAD1_ABE_MCBSP2_CLKX,regVal);

	// Enable GPIO1 modules
	regVal = READ_REGISTER(GPIO1_SYSCONFIG);
	regVal |= 0x2;
	WRITE_REGISTER(GPIO1_SYSCONFIG,regVal);
	regVal = READ_REGISTER(GPIO1_SYSSTATUS);
	while ( !(regVal & 0x1) ) {
		regVal = READ_REGISTER(GPIO1_SYSSTATUS);
	}
	// set idle mode
	regVal = READ_REGISTER(GPIO1_SYSCONFIG);
	regVal &= ~(0x19);
	regVal |= 0x8;
	WRITE_REGISTER(GPIO1_SYSCONFIG,regVal);

	// Set clockgating-ratio
	regVal = READ_REGISTER(GPIO1_CTRL);
	regVal &= ~(0x7);
	WRITE_REGISTER(GPIO1_CTRL,regVal);
	// read output enable register
	regVal = READ_REGISTER(GPIO1_OE);
	regVal &= ~(0x100); // enable bit 8 for GPIO1_GPIO_8
	WRITE_REGISTER(GPIO1_OE,regVal);

	// Set mux to output
	regVal = READ_REGISTER(CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K);
	regVal &= ~(0x7);
	regVal |= 0x3;
	WRITE_REGISTER(CONTROL_WKUP_PAD0_FREF_CLK4_OUT_PAD1_SYS_32K,regVal);

}

void PandaBoardLED::turnLEDD1(bool state)
{
	dword_t clearSetMask = 0x4000;
	dword_t regVal = READ_REGISTER(GPIO4_DATAOUT);
	if ( state ) {
		regVal |= 0x4000; // bit 14 again
		WRITE_REGISTER(GPIO4_SETDATAOUT,clearSetMask);
	} else {
		regVal &= ~(0x4000); // bit 14 again
		WRITE_REGISTER(GPIO4_CLEARDATAOUT,clearSetMask);
	}
	WRITE_REGISTER(GPIO4_DATAOUT,regVal);
}

void PandaBoardLED::turnLEDD2(bool state)
{
	dword_t clearSetMask = 0x100;
	dword_t regVal = READ_REGISTER(GPIO1_DATAOUT);
	if ( state ) {
		regVal |= clearSetMask; // bit 8 again
		WRITE_REGISTER(GPIO1_SETDATAOUT,clearSetMask);
	} else {
		regVal &= ~(clearSetMask); // bit 8 again
		WRITE_REGISTER(GPIO1_CLEARDATAOUT,clearSetMask);
	}
	WRITE_REGISTER(GPIO1_DATAOUT,regVal);
}

END_OSC_IMPLEMENTATION
