/*
 * Spinlock.h
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#ifndef SYNC_SPINLOCK_H_
#define SYNC_SPINLOCK_H_

#include <common/types.h>

class Spinlock
{
private:
	volatile bool __attribute__ ((aligned (4))) lockStatus;
public:
	Spinlock() : lockStatus(0) {}

	void lock() {
        	while ( !take_if_free() ) {
			; // spin
		}
	}

	void unlock() {
		__atomic_clear(&lockStatus,__ATOMIC_RELEASE);
	}

	/**
	 * True means successfully set
	 */
	bool take_if_free() {	// returns true if successful, otherwise false
		return ( __atomic_test_and_set(&lockStatus,__ATOMIC_ACQUIRE) == 0 );
	}

};


#endif /* SYNC_SPINLOCK_H_ */
