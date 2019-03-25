/*
 * CATManager.h
 *
 *  Created on: 04.07.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_CATMANAGER_H_
#define ARCH_X86_DRIVER_CATMANAGER_H_

#include <common/types.h>

#include <cachemanagement/X86CacheManagement.h>

namespace CacheManagement {
class CATManager : public X86CacheManagement {
private:
	static constexpr uint32_t cCPUID_REG_EAX_AVAILABLE_RESOURCE_IDENTIFICATION = 0x10;
	static constexpr uint32_t cCPUID_REG_ECX_AVAILABLE_RESOURCE_IDENTIFICATION = 0x00;

	static constexpr uint32_t cMSR_L3_COS_BASE = 0xC90;
	static constexpr uint32_t cMSR_L2_COS_BASE = 0xD10;
	static constexpr uint32_t cMSR_IA32_PQR_ASSOC = 0xC8F;

	static constexpr uint8_t cCOSMaxStatic = 20;
	static constexpr uint8_t cWaysMax = 32;
public:
	CATManager();

	/**
	 * Initializes CAT. On the BSP this enumerates the capabilities. On all secondary processors this
	 * will only enable CAT (if available)
	 */
	void initialize();

	/**
	 * Method to print information of the current state of the managed cache.
	 */
	virtual void printCacheWayInformation();

	void prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration);
	void evictCacheWay(cacheways_t way, cycle_t *duration);

	/**
	 * Enable all but the specified cache way. Can be used to measure certain cache
	 * performance metrics.
	 * @param way Way that should be disabled
	 */
	void enableWaysInverse(cacheways_t way);

	/**
	 * Enables cache allocation for one specific cache way.
	 * @param way Way to allow allocation to
	 */
	void enableWayExclusive(cacheways_t way);

	/**
	 * Disable allocation to all cache ways for the calling processor
	 */
	void disableAllWays();

	/**
	 * Enable allocation to all cache ways for the calling processor
	 */
	void enableAllWays();

	/**
	 * Singleton instance
	 */
	static CATManager sInstance;
private:
	static constexpr size_t cCACHE_WAY_SIZE = 512 * 1024;
	/**
	 * Enumerates the capabilities for Intel CAT on the running machine. Sets all the properties
	 * regarding CAT
	 */
	void enumerateCapabilities();

	/**
	 * Creates a linear mapping of available cache way to COS bitmasks. This is necessary because
	 * it is possible that not all bits are available as an exclusive cache way. Some are shared with
	 * IGP and other uncore components.
	 */
	void createWayMapping();

	/**
	 * Sets the cache access mask for a given COS
	 * @param cos COS to set mask for
	 * @param mask to set for the COS
	 */
	void setMask(uint16_t cos, uint32_t mask);

	/**
	 * Enables CAT on the calling processor by setting the COS to the specific one for
	 * the hardware thread. See getProcessorCOS().
	 */
	void enableProcessorCAT();

	inline bool getL3CATAvailable() {
		return mAvailableResources & 0x2;
	}

	inline bool getL2CATAvailable() {
		return mAvailableResources & 0x4;
	}

	inline bool getMBAAvailable() {
		return mAvailableResources & 0x8;
	}

	/**
	 * Provides a mapping of CPU number to Class of Service number
	 * @return COS for the calling processor
	 */
	inline uint16_t getProcessorCOS() {
		return getCPUID() + 1;
	}

	/**
	 * Bit mask of that describes the CAT capabilities
	 */
	uint32_t mAvailableResources;
	/**
	 * Number of available classes of service
	 */
	uint16_t mCOSMax;

	/**
	 * Maximum number of manageable cache partitions
	 */
	uint8_t mBitmaskMaxLength;

	/**
	 * Bitmask of shared resources (e.g. IGP)
	 */
	uint32_t mSharedBitMask;

	/**
	 * Set if code/data priorization is available on the machine. Not used at the moment.
	 */
	bool mCodeDataPriorizationAvailable;

	/**
	 * Set if the running machine does not present the CAT feature.
	 */
	bool mNoCATAvailable;


	/**
	 * Stores an linear cache way -> Cache bit mapping
	 */
	uint8_t mWayBitMapping[cCOSMaxStatic];

	uint32_t mStandbyMask[NR_CPUS];

	Spinlock mCATLock;

protected:
	virtual size_t getCacheWaySize();

};
} /* namespace CacheManagement */
#endif /* ARCH_X86_DRIVER_CATMANAGER_H_ */
