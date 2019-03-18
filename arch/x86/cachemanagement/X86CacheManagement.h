/*
 * X86CacheManagement.h
 *
 *  Created on: 16.07.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_CACHEMANAGEMENT_X86CACHEMANAGEMENT_H_
#define ARCH_X86_CACHEMANAGEMENT_X86CACHEMANAGEMENT_H_

#include <common/types.h>
#include <eventhandling/EventTask.h>

#include <cachemanagement/GenericCacheManagement.h>

namespace CacheManagement {
class X86CacheManagement : public GenericCacheManagement {
public:
	X86CacheManagement();
	void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration);
	void evictCacheWay(cacheways_t way, cycle_t *duration);

	void evictMemoryRange(uintptr_t start, uint64_t size);
	cycle_t getRAMAccessTimeThreshold() { return cRAMAccessTimeThreshold; }

private:
	static constexpr cycle_t cRAMAccessTimeThreshold = 100;
	uint32_t mCLFlushSize;
};

}
#endif /* ARCH_X86_CACHEMANAGEMENT_X86CACHEMANAGEMENT_H_ */
