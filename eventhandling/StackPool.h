/*
 * StackPool.h
 *
 *  Created on: 03.08.2016
 *      Author: hendrik
 */

#ifndef EVENTHANDLING_STACKPOOL_H_
#define EVENTHANDLING_STACKPOOL_H_

#include <common/types.h>

#include <sync/Spinlock.h>

class StackPool {

	struct stack_t {
		byte_t __attribute__ ((aligned (16))) data[STACK_SIZE];
	};

public:
	StackPool();

	void *getFreeStack();

	void setStackFree(void *stackpointer);

	static StackPool pInstance;

	void dumpStacks();
private:
	static constexpr uint32_t cStackPoolSize = NR_CPUS;

	// Secure retrieving/free of stack operations
	Spinlock mLock;

	// Manages which stacks are available for use
	bool mUsedStacks[cStackPoolSize];

	// Stack data
	stack_t __attribute__ ((aligned (16))) mStacks[cStackPoolSize];

	// Points to last used stack
	uint32_t mUsedPointer;

};



#endif /* EVENTHANDLING_STACKPOOL_H_ */
