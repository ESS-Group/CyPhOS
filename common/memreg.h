/*
 * memreg.h
 *
 *  Created on: 20.03.2015
 *      Author: hendrik
 */

#ifndef COMMON_MEMREG_H_
#define COMMON_MEMREG_H_
#include <common/types.h>

/* Helper macros to access memory mapped registers */
#define READ_REGISTER(a)		*((volatile dword_t*)(a))
#define WRITE_REGISTER(reg,value) *((volatile dword_t*)(reg)) = (dword_t)(value)

/**
 * 16bit register manipulation
 */
#define READ_REGISTER_16(a) *((volatile uint16_t*)(a))
#define WRITE_REGISTER_16(reg,value) *((volatile uint16_t*)(reg)) = (uint16_t)(value)

#endif /* COMMON_MEMREG_H_ */
