/*
 * CATManager.cc
 *
 *  Created on: 04.07.2018
 *      Author: hendrik
 */

#include <arch/x86/driver/CATManager.h>

#include <common/cpuid.h>
#include <common/debug.h>

#include <common/SyncOutput.h>

#include <common/msr.h>

#define TAG "CAT"

namespace CacheManagement {

#ifdef CONFIG_AMD64_CAT
CATManager CATManager::sInstance;
#endif

CATManager::CATManager() : X86CacheManagement() {
}

void CATManager::initialize() {
	// If cache management is globally disabled, abort here regardless of CAT support
#ifndef CONFIG_CACHE_CONTROL
	mNoCATAvailable = true;
	return;
#endif
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Initialize" );
#endif
	if (getCPUID() == 0) {
		// Only enumerate capabilities once on BSP
		enumerateCapabilities();
		if (!mNoCATAvailable) {
			createWayMapping();
		}
	}
	enableProcessorCAT();
}

void CATManager::enumerateCapabilities() {
	uint32_t eax = cCPUID_REG_EAX_AVAILABLE_RESOURCE_IDENTIFICATION;
	uint32_t ebx = 0x0;
	uint32_t ecx = cCPUID_REG_ECX_AVAILABLE_RESOURCE_IDENTIFICATION;
	uint32_t edx = 0x0;

	// Read resource type identification (17-57 Vol.3b)
	readCPUID(&eax, &ebx, &ecx, &edx);
	mAvailableResources = ebx;

#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"CAT capabilities: L3: " << dec << (uint16_t)getL3CATAvailable() << " L2: " << (uint16_t)getL2CATAvailable() << " MBA: " << (uint16_t)getMBAAvailable());
#endif

	if (getL3CATAvailable()) {
		eax = 0x10;
		ecx = 0x1; //
		ebx = 0x0;
		edx = 0x0;
	} else if (getL2CATAvailable()) {
		eax = 0x10;
		ecx = 0x2; //
		ebx = 0x0;
		edx = 0x0;
	} else {
		mNoCATAvailable = true;
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"No CAT available");
#endif
		return;
	}

	readCPUID(&eax, &ebx, &ecx, &edx);

	mCOSMax = (0xFFFF & edx) + 1;
	mSharedBitMask = ebx;
	mBitmaskMaxLength = (0x1F & eax) + 1;

	mCodeDataPriorizationAvailable = 0x4 & ecx;


#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"CAT capabilities: COS max: " << dec << (uint16_t)mCOSMax << " Shared bit mask " << hex << mSharedBitMask << " max bitmask length: " << (uint16_t)mBitmaskMaxLength);
#endif
}

void CATManager::createWayMapping() {
	mCacheWayCount = 0;


	cpu_t cpu = 0;
	for (uint32_t i = 0; i < mBitmaskMaxLength; i++) {
		// Check if cache way is identical to the shared bit mask. If so, do not use it as a regular cache way.
		if (!(mSharedBitMask & (0x1 << i))) {
			// First assign each core a standby mask. Then use remaining for general OSC use
			if (cpu != NR_CPUS) {
#ifdef CONFIG_X86_DEBUG_CAT
				DEBUG_STREAM(TAG,"CAT bit: " << dec << i << " mapped as stand-by way for cpu: " << (uint32_t)cpu);
#endif
				mStandbyMask[cpu] = (0x1 << i);
				cpu++;
			} else {
				mWayBitMapping[mCacheWayCount] = i;
#ifdef CONFIG_X86_DEBUG_CAT
				DEBUG_STREAM(TAG,"CAT bit: " << dec << i << " mapped to \"way\": " << mCacheWayCount);
#endif
				mCacheWayCount++;
			}
		}
	}
}

void CATManager::printCacheWayInformation() {
	GenericCacheManagement::printCacheWayInformation();
	LOG_OUTPUT_STREAM_START(TAG, "CAT information:" << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "Cache ways available: " << dec << mCacheWayCount << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "CAT capabilities: COS max: " << dec << (uint16_t)mCOSMax << " Shared bit mask " << hex << mSharedBitMask << " max bitmask length: " << (uint16_t)mBitmaskMaxLength << endl);
	LOG_OUTPUT_STREAM_CONTINUE(TAG, "CAT capabilities: L3: " << dec << (uint16_t)getL3CATAvailable() << " L2: " << (uint16_t)getL2CATAvailable() << " MBA: " << (uint16_t)getMBAAvailable() << endl);
	LOG_OUTPUT_STREAM_END
}

void CATManager::prefetchDataToWay(uintptr_t start, uintptr_t end, uintptr_t textEnd, cacheways_t way, cycle_t *duration) {
	// Enable correct way
	enableWayExclusive(way);
	// Call super class prefetching
	X86CacheManagement::prefetchDataToWay(start, end, textEnd, way, duration);
	// Disable way
	disableAllWays();
}

void CATManager::evictCacheWay(cacheways_t way, cycle_t *duration) {
	// Enable correct way
	enableWayExclusive(way);
	// Call super class eviction
	X86CacheManagement::evictCacheWay(way, duration);
	// Disable way
	disableAllWays();
}


void CATManager::enableWaysInverse(cacheways_t way) {
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Enabling all ways except: " << way);
#endif
	uint32_t mask = 0;
	for (cacheways_t loopWay = 0; loopWay < mCacheWayCount; loopWay++) {
		if (loopWay == way) {
			continue;
		}
		mask &= (0x1 << mWayBitMapping[loopWay]);
	}
	setMask(getProcessorCOS(), mask);
}

void CATManager::enableWayExclusive(cacheways_t way) {
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Enabling way: " << way);
#endif
	setMask(getProcessorCOS(), (0x1 << mWayBitMapping[way]));
}

void CATManager::disableAllWays() {
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Disable all ways");
#endif

	/*
	 * Set processor core to standby mask (which should be a shared mask)
	 * because a all zero mask is not allowed and will result in a GPF.
	 */
	setMask(getProcessorCOS(), mStandbyMask[getCPUID()]);
}

void CATManager::enableAllWays() {
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Disable all ways");
#endif

	uint32_t mask = 0;

	for (cacheways_t way = 0;  way < mBitmaskMaxLength; way++) {
		mask &= (0x1 << way);
	}

	//setMask(getProcessorCOS(), mask);
}


void CATManager::setMask(uint16_t cos, uint32_t mask) {
	if(mNoCATAvailable) {
		// Do not try to configure CAT if it is not available
		return;
	}

#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Set mask: " << mask << " for cos: " << cos);
#endif

	uint64_t regValue;
	if (getL3CATAvailable() ) {
		regValue = readMSR((cMSR_L3_COS_BASE + cos));
		regValue &= ((uint64_t)0xFFFFFFFF << 32);
		regValue |= mask;
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Writing MSR: " << (cMSR_L3_COS_BASE + cos)<< "with value: " << regValue);
#endif
		writeMSR(cMSR_L3_COS_BASE + cos, (uint64_t)mask);
	} else if (getL2CATAvailable()) {
		regValue = readMSR((cMSR_L2_COS_BASE + cos));
		regValue &= ((uint64_t)0xFFFFFFFF << 32);
		regValue |= mask;
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Writing MSR: " << (cMSR_L2_COS_BASE + cos)<< "with value: " << regValue);
#endif
		writeMSR(cMSR_L2_COS_BASE + cos, (uint64_t)mask);
	}
}

void CATManager::enableProcessorCAT() {
	if(mNoCATAvailable) {
		// Do not try to enable CAT if it is not available
		return;
	}
	uint64_t regValue = readMSR(cMSR_IA32_PQR_ASSOC);

	regValue &= ((uint64_t)0xFFFFFFFF << 32);
	regValue |= ((uint64_t)getProcessorCOS() << 32);
#ifdef CONFIG_X86_DEBUG_CAT
	DEBUG_STREAM(TAG,"Writing MSR: " << cMSR_IA32_PQR_ASSOC << "with value: " << regValue);
#endif
	writeMSR(cMSR_IA32_PQR_ASSOC, regValue);

	disableAllWays();
}

}
