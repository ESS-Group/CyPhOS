/*
 * IDTManager.cc
 *
 *  Created on: 05.06.2018
 *      Author: hendrik
 */

#include <arch/x86/interrupts/IDTManager.h>

#include "interrupts.h"

#include <common/debug.h>

#define TAG "IDTManager"

IDTManager IDTManager::mInstance;

extern uintptr_t low_level_isr_entry_array_start;

IDTManager::IDTManager() {
	fillIDT();
}

void IDTManager::fillIDT() {

	for (uint32_t i = 0; i < cIDTSize; i++) {
		// Calculate vector entry point
		uintptr_t entryAddress = (uintptr_t)(&low_level_isr_entry_array_start) + (i * ISR_VECTOR_ENTRY_SIZE);
		mIDT[i].offset1 = 0xFFFF & entryAddress;
		mIDT[i].offset2 = 0xFFFF & (entryAddress >> 16);
		mIDT[i].offset3 = 0xFFFFFFFF & (entryAddress >> 32);

		// FIXME save segment target in a global symbol from startup (GDT init code) and reuse it from there instead of hard coding
		mIDT[i].targetSelector = 0x10;

		mIDT[i].ist = 0x0;
		mIDT[i].type = 0xE;
		mIDT[i].p = 1;
		mIDT[i].reserved2 = 0x0;
		mIDT[i].zero0 = 0;
	}

//	printIDT();
	loadIDT();
//	printIDTInfo();
}

void IDTManager::printIDT() {
	for (uint32_t i = 0; i < cIDTSize; i++) {
//		DEBUG_STREAM(TAG,"IDT[" << dec << (uint32_t)i <<"] offset1 :" << hex << mIDT[i].offset1);
//		DEBUG_STREAM(TAG,"IDT[" << dec << (uint32_t)i <<"] offset2 :" << hex << mIDT[i].offset2);
//		DEBUG_STREAM(TAG,"IDT[" << dec << (uint32_t)i <<"] offset3 :" << hex << mIDT[i].offset3);
		DEBUG_STREAM(TAG,"IDT[" << dec << (uint32_t)i <<"] offset:" << hex << (uint64_t)(mIDT[i].offset1 | ((uint64_t)mIDT[i].offset2 << 16) | ((uint64_t)mIDT[i].offset3 << 32) ));
		DEBUG_STREAM(TAG,"IDT[" << dec << (uint32_t)i <<"] target:" << hex << (uint16_t)mIDT[i].targetSelector);
	}
}

void IDTManager::loadIDT() {
	mIDTInfo.size = (cIDTSize * 16);
	mIDTInfo.idtAddress = (uint64_t)&mIDT;
	__asm__ __volatile__("lidt %0\n "::"m"(mIDTInfo):"memory");
}

void IDTManager::printIDTInfo() {
	IDTDescription info;
	__asm__ __volatile__("sidt %0\n"::"m"(info):"memory");
	DEBUG_STREAM(TAG,"IDTInfo size: " << dec << info.size << " address: " << hex << info.idtAddress);
	if (info.idtAddress != (uintptr_t)&mIDT) {
		DEBUG_STREAM(TAG,"IDT MISMATCH. SYSTEM ERROR :-(");
	}
}
