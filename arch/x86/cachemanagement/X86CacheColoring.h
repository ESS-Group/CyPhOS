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
	/**
	 * 20 cache ways for (Intel Xeon E5-1620 v4)
	 */
	static constexpr uint32_t cCACHE_WAY_COUNT = 20;
	/**
	 * Use architecture defined cache way size
	 */
	static constexpr uint32_t cCACHE_WAY_SIZE = CONFIG_CACHE_WAY_SIZE;
	/**
	 * Use more than one page per color to enable more use of the private cache
	 */
	static constexpr uint32_t cPAGES_PER_COLOR = 2;

#ifdef CONFIG_AMD64_CACHE_COLORING
	/**
	 * Singleton instance only enabled if cache coloring is enabled
	 */
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
