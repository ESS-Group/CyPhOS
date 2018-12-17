/*
 * ComponentLock.cc
 *
 *  Created on: 07.02.2017
 *      Author: hendrik
 */

#include <sync/ComponentLock.h>

void ComponentLock::lock() {
	mLock.lock();
}

void ComponentLock::unlock() {
	mLock.unlock();
}
