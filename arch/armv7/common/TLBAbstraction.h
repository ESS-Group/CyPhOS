/*
 * TLBAbstraction.h
 *
 *  Created on: 25.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_COMMON_TLBABSTRACTION_H_
#define ARCH_ARMV7_COMMON_TLBABSTRACTION_H_

#include <driver/ARMMMU.h>

namespace MMU {

inline void flushTLB() {
	ARMMMU::pInstance.flushTLBWithoutBroadcast();
}

inline void flushTLBBroadcast() {
	ARMMMU::pInstance.flushTLB();
}

inline void enableAccessFlag() {
	ARMMMU::pInstance.enableAccessFlag();
}

inline void disableAccessFlag() {
	ARMMMU::pInstance.disableAccessFlag();
}

}

#endif /* ARCH_ARMV7_COMMON_TLBABSTRACTION_H_ */
