/*
 * MemoryProfilingSpace.h
 *
 *  Created on: 29.03.2017
 *      Author: hendrik
 */

#ifndef BAREMETAL_MEMORYPROFILINGSPACE_H_
#define BAREMETAL_MEMORYPROFILINGSPACE_H_

#include <common/types.h>

class MemoryProfilingSpace {
private:
	static constexpr size_t cDataRange = 64 * 1024;

public:
	MemoryProfilingSpace();


	byte_t mData[cDataRange];

	static MemoryProfilingSpace mInstances[NR_CPUS];



};

#endif /* BAREMETAL_MEMORYPROFILINGSPACE_H_ */
