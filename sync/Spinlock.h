/*
 * Spinlock.h
 *
 *  Created on: 28.07.2015
 *      Author: hendrik
 */

#ifndef SYNC_SPINLOCK_H_
#define SYNC_SPINLOCK_H_

#include <common/types.h>

#include <common/cyclecounter.h>

#ifndef TAG
#define RED_TAG
#define TAG "Spinlock"
#endif

//#define CONFIG_SPINLOCK_DEADLOCK_DETECTION

class Spinlock {
private:
	volatile uint32_t __attribute__ ((aligned (4))) available;
	volatile uint32_t __attribute__ ((aligned (4))) active;
public:
	Spinlock() :
			available(0), active(0) {
	}

	void lock() {
#ifdef CONFIG_SPINLOCK_DEADLOCK_DETECTION
		cycle_t before,after;
		READ_CYCLE_COUNTER(before);
#endif
		uint32_t myTicket = __atomic_fetch_add(&available, 1, __ATOMIC_ACQUIRE);
		while (__atomic_load_n(&active, __ATOMIC_ACQUIRE) != myTicket) {
#ifdef CONFIG_SPINLOCK_DEADLOCK_DETECTION
			READ_CYCLE_COUNTER(after);
			if(after-before > 100000000) {
				DEBUG_STREAM(TAG,"Deadlock?");
			}
#endif
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

#ifdef RED_TAG
#undef TAG
#undef RED_TAG
#endif

#endif /* SYNC_SPINLOCK_H_ */
