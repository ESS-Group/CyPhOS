/*
 * IDTManager.h
 *
 *  Created on: 05.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_INTERRUPTS_IDTMANAGER_H_
#define ARCH_X86_INTERRUPTS_IDTMANAGER_H_

#include "idt.h"

class IDTManager {

public:
	IDTManager();
	void fillIDT();
	static IDTManager mInstance;

	void printIDT();
	void loadIDT();
private:
	static uint32_t constexpr cIDTSize = 256;
	IDTEntry __attribute__ ((aligned(8))) mIDT[cIDTSize];
	IDTDescription __attribute__((aligned(8))) mIDTInfo;


	void printIDTInfo();
};

#endif /* ARCH_X86_INTERRUPTS_IDTMANAGER_H_ */
