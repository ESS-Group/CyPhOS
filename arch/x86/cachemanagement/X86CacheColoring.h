/*
 * X86CacheColoring.h
 *
 *  Created on: 07.02.2019
 *      Author: hendrik
 */

#ifndef ARCH_X86_CACHEMANAGEMENT_X86CACHECOLORING_H_
#define ARCH_X86_CACHEMANAGEMENT_X86CACHECOLORING_H_

#include <cachemanagement/CacheColoring.h>

namespace CacheManagement {

class X86CacheColoring : public CacheColoring {
public:
	X86CacheColoring();

private:
	static constexpr cycle_t cRAMAccessTimeThreshold = 100;
	static constexpr uint32_t cCACHE_COLOR_COUNT = 512 / 4; // LLC cache-way size / pagesize (512kB/4kB)
	static constexpr uint32_t cCACHE_COLOR_SIZE = 20 * 4096; // LLC cache-way count * 4096b (page size)
#ifdef CONFIG_AMD64_CACHE_COLORING
	static X86CacheColoring sInstance;
#endif

protected:
	virtual uint32_t getColorCount();
	virtual uint32_t getColorSize();

	virtual cycle_t getRAMAccessTimeThreshold();

};

} /* namespace CacheManagement */

#endif /* ARCH_X86_CACHEMANAGEMENT_X86CACHECOLORING_H_ */
