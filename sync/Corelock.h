/*
 * Corelock.h
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#ifndef SYNC_CORELOCK_H_
#define SYNC_CORELOCK_H_

#include <common/armhelper.h>

class Corelock
{
private:
	volatile unsigned char __attribute__ ((aligned (8))) lock_status ;
	volatile unsigned int pCPUID;
	volatile unsigned int pCount;
public:
	inline Corelock() : lock_status(0), pCPUID(0) {
	}

	inline void lock()
	{
		if (pCPUID == getCPUID() ) {
			pCount++;
			return;
		}
 		__asm__ __volatile__ (
				"ldr r1,=0x1\n"
				"0: LDREX r0,[%[LOCK]]\n"
				"CMP r0,#0x0\n"
				"STREXEQ R0,R1,[%[LOCK]]\n"
				"CMPEQ R0,#0x0\n"
				"WFENE \n"
				"BNE 0b\n"
				"DSB\n" ::[LOCK] "r"(&lock_status):"r0","r1","memory");
 		pCount++;
 		pCPUID = getCPUID();
	}
	inline void unlock()
	{
		if ( pCPUID == getCPUID() ) {
			pCount--;

		}
		if ( pCount != 0) {
			return;
		}
		__asm__ __volatile__ (
				"MOV R0,#0x0\n"
				"DMB\n"
				"STR R0,[%[LOCK]]\n"
				"DSB \n"
				"SEV \n"
				::[LOCK] "r"(&lock_status):"r0","memory");

	}
	inline unsigned int get_status()
	{
		return lock_status;
	}
	inline bool take_if_free()
	{
		return ( __sync_lock_test_and_set(&lock_status,1) == 0 );
	}

};


#endif /* SYNC_SPINLOCK_H_ */
