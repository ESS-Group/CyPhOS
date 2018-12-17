/*
 * ComponentLock.h
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#ifndef SYNC_COMPONENTLOCK_H_
#define SYNC_COMPONENTLOCK_H_

#include <sync/Spinlock.h>

/**
 * Lock to be used in shared components. This lock can be an spinlock if no
 * preemption is active because spinning would be quite short.
 *
 * If preemption is enabled (CONFIG_PREEMPT) a queue of suspended eventhandlers
 * (EventConnection) objects is necessary to keep book of waiting handlers.
 * NOT IMPLEMENTED FOR NOW
 */
class ComponentLock {
private:
	Spinlock mLock;

public:
	void lock();
	void unlock();
};

#endif /* SYNC_COMPONENTLOCK_H_ */
