/*
 * StackPool.cc
 *
 *  Created on: 03.08.2016
 *      Author: hendrik
 */

#include "StackPool.h"

#define STACKPOINTER_FROM_ARRAY(addr) ((void*)( ((uintptr_t)addr) + ((uintptr_t)STACK_SIZE)  ))

#define ARRAY_FROM_STACKPOINTER(addr) ((void*)( ((uintptr_t)addr) - ((uintptr_t)STACK_SIZE)  ))

#include <common/debug.h>

#define TAG "StackPool"

StackPool StackPool::pInstance;

StackPool::StackPool() {
	mUsedPointer = 0;
}

void *StackPool::getFreeStack() {
	mLock.lock();
	uint32_t lastPointer = mUsedPointer;
	/* Iterate of the list of free stacks and return the first free one
	 * after setting the used bool to false.
	 */
	do {
		if ( mUsedStacks[mUsedPointer] == false ) {
			mUsedStacks[mUsedPointer] = true;
			void *sp = STACKPOINTER_FROM_ARRAY(&mStacks[mUsedPointer]);
			//DEBUG_STREAM(TAG,"Found stack: " << hex << sp);
			mLock.unlock();
			return sp;
		}
		mUsedPointer = (mUsedPointer + 1) % cStackPoolSize;
	} while ( mUsedPointer != lastPointer );
	mLock.unlock();
	DEBUG_STREAM(TAG,"No stacks available");
	/* If no free stack is available return nullptr. */
	return nullptr;
}

void StackPool::setStackFree(void *stackpointer) {
	/* Check to which index the stackpointer maps */
	mLock.lock();
	for (uint32_t i = 0; i < cStackPoolSize; i++) {
		if ( &(mStacks[i]) == ARRAY_FROM_STACKPOINTER(stackpointer) ) {
			mUsedStacks[i] = false;
//			DEBUG_STREAM(TAG,"Returning stack: " << hex << stackpointer);
			mLock.unlock();
			return;
		}
	}
	DEBUG_STREAM(TAG,"Invalid stack: " << hex << stackpointer);
	mLock.unlock();
}


void StackPool::dumpStacks() {
	for (uint16_t i = 0; i < cStackPoolSize; i++ ) {
		DEBUG_STREAM(TAG,"Stack " << dec << i << hex << " begins at :" << STACKPOINTER_FROM_ARRAY(&mStacks[i]));
	}
}
