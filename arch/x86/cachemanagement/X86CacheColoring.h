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
	static constexpr uint32_t cCACHE_WAY_COUNT = 20;
	static constexpr uint32_t cCACHE_WAY_SIZE = CONFIG_CACHE_WAY_SIZE;
	static constexpr uint32_t cPAGES_PER_COLOR = 2;
#ifdef CONFIG_AMD64_CACHE_COLORING
	static X86CacheColoring sInstance;
#endif

protected:
	virtual uint32_t getHWCacheWayCount();
	virtual uint32_t getHWCacheWaySize();
	virtual uint32_t getPagesPerColor();

	virtual cycle_t getRAMAccessTimeThreshold();

};

} /* namespace CacheManagement */

#endif /* ARCH_X86_CACHEMANAGEMENT_X86CACHECOLORING_H_ */
