/*
 * cortexa9_generics.cc
 *
 *  Created on: 16.07.2015
 *      Author: hendrik
 */


void setVBAR(dword_t vbar_val)
{
	asm ( "MCR p15, 0, %0, c12, c0, 0;" : : "r"(vbar_val));
}

dword_t readVBAR() {
	dword_t regValue;
	asm ( "MRC p15, 0, %0, c12, c0, 0;" : "=&r"(regValue));
	return regValue;
}


