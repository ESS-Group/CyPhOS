/*
 * Spinlock.h
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#ifndef SYNC_SPINLOCK_H_
#define SYNC_SPINLOCK_H_

#include <common/types.h>

class Spinlock {
private:
	volatile uint32_t __attribute__ ((aligned (4))) available;
	volatile uint32_t __attribute__ ((aligned (4))) active;
public:
	Spinlock() :
			available(0), active(0) {
	}

	void lock() {
		uint32_t myTicket = __atomic_fetch_add(&available, 1, __ATOMIC_ACQUIRE);
		while (__atomic_load_n(&active, __ATOMIC_ACQUIRE) != myTicket) {
		}
	}

	void unlock() {
		__atomic_fetch_add(&active, 1, __ATOMIC_RELEASE);
	}

	/**
	 * True means successfully set
	 */
	bool take_if_free() {   // returns true if successful, otherwise false
		uint32_t myTicket = __atomic_fetch_add(&available, 1, __ATOMIC_ACQUIRE);
		if (__atomic_load_n(&active, __ATOMIC_ACQUIRE) != myTicket) {
			__atomic_fetch_sub(&available,1, __ATOMIC_ACQUIRE);
			return false;
		}
		return true;
	}

};

#endif /* SYNC_SPINLOCK_H_ */
