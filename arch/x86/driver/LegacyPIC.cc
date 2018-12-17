/*
 * LegacyPIC.cc
 *
 *  Created on: 11.06.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/LegacyPIC.h>

#include <common/cpu.h>

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#include <common/debug.h>

#define TAG "LegacyPIC"

LegacyPIC LegacyPIC::mInstance;

LegacyPIC::LegacyPIC() : mMasterCommand(0x0020), mMasterData(0x0021), mSlaveCommand(0x00A0), mSlaveData(0x00A1) {
	disable();
}


void LegacyPIC::remapToOffset(uint32_t offset) {
	// Start reinitialization process
	mMasterCommand.writeByte(ICW1_INIT + ICW1_ICW4);
	io_wait();
	mSlaveCommand.writeByte(ICW1_INIT + ICW1_ICW4);
	io_wait();

	// Program vector offsets
	mMasterData.writeByte(offset);
	io_wait();
	mSlaveData.writeByte(offset + 8);
	io_wait();

	// Program master/slave configuration
	mMasterData.writeByte(4);
	io_wait();
	mSlaveData.writeByte(2);
	io_wait();

//	// Enable 8086-mode
	mMasterData.writeByte(ICW4_8086);
	io_wait();
	mSlaveData.writeByte(ICW4_8086);
	io_wait();

//	DEBUG_STREAM(TAG,"LegacyPIC IRQs remapped to: " << dec << offset);
}

void LegacyPIC::maskIRQLine(uint8_t line, bool masked) {
	IOPort *port;
	if (line < 8) {
		port = &mMasterData;
	} else {
		port = &mSlaveData;
		line -= 8;
	}

	uint8_t tmpVal = port->readByte();

	if (masked) {
		tmpVal |= (1 << line);
	} else {
		tmpVal &= ~(1 << line);
	}
	port->writeByte(tmpVal);
	io_wait();
}

void LegacyPIC::disable() {
	remapToOffset(32);
	// Mask all IRQ lines
	mMasterData.writeByte(0xFF);
	mSlaveData.writeByte(0xFF);

//	DEBUG_STREAM(TAG,"LegacyPIC masked");
}
