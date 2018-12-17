/*
 * memreg.h
 *
 *  Created on: 14.06.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_COMMON_MEMREG_H_
#define ARCH_X86_COMMON_MEMREG_H_


inline uint32_t readRegister_32(uint32_t *address) {
	return *((volatile uint32_t*)(address));
}

inline void writeRegister_32(uint32_t *address, uint32_t value) {
	*((volatile uint32_t*)(address)) = (uint32_t)(value);
}

inline uint64_t readRegister_64(uint64_t *address) {
	return *((volatile uint64_t*)(address));
}

inline void writeRegister_64(uint64_t *address, uint64_t value) {
	*((volatile uint64_t*)(address)) = (uint64_t)(value);
}


#endif /* ARCH_X86_COMMON_MEMREG_H_ */
