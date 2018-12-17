/*
 * IOPort.h
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_IOPORT_H_
#define ARCH_X86_COMMON_IOPORT_H_

#include <common/types.h>

class IOPort {
	uint16_t mAddress;
public:
	IOPort(int a) : mAddress(a) {};

	IOPort() : mAddress(0) {};

	inline void writeByte(uint8_t val) const {
		__asm__ __volatile__ ("outb %0, %1"::"a"(val),"Nd"(mAddress):"memory");
	}

	inline void writeWord(uint16_t val) const {
		__asm__ __volatile__ ("out %%dx,%%ax"::"d"(mAddress),"a"(val):);
	}

	inline uint8_t readByte() const {
		uint8_t value;
		__asm__ __volatile__ ("inb %1, %0"
                : "=a"(value)
                : "Nd"(mAddress): "memory");
		return value;
	}

	inline uint16_t readWord() const {
		uint32_t value;
		__asm__ __volatile__ ("xor	%%eax,%%eax\n\t"
				"in	%%ax,%%dx\n\t"
				"mov %0, %%eax"
				:"=r"(value):"d"(mAddress):"%rax");
		return value;
	}
};




#endif /* ARCH_X86_COMMON_IOPORT_H_ */
