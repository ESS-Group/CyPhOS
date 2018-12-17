/*
 * barrier.h
 *
 *  Created on: 02.10.2018
 *      Author: hendrik
 */

#ifndef COMMON_BARRIER_H_
#define COMMON_BARRIER_H_

#include <sync/Spinlock.h>

#include <common/debug.h>

class Barrier {
public:
	Barrier(unsigned int size) {
            mSize = size;
            mNotReady = size;
            mWaiting = 0;
    }

    inline void await() {
            int val;
            mReadyLock.lock();
            val = mNotReady--;
            if (val == 1) {
            	// Wait that all other waiting threads finish waiting, then reset
            	while (mWaiting > 0) {}
            	mNotReady = mSize;
            	mReadyLock.unlock();
            } else {
            	// Not all finished yet, spin

            	mWaitingLock.lock();
            	mWaiting++;
            	mWaitingLock.unlock();

            	mReadyLock.unlock();

            	while (mNotReady > 0) {}

            	// Signal end of waiting by decreasing mWaiting
            	mWaitingLock.lock();
            	mWaiting--;
            	mWaitingLock.unlock();
            }
    }

private:
    unsigned int mSize;
    volatile unsigned int mNotReady;
    volatile unsigned int mWaiting;

    Spinlock mReadyLock;
    Spinlock mWaitingLock;
};



#endif /* COMMON_BARRIER_H_ */
