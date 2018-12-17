/*
 * SystemMode.cc
 *
 *  Created on: 24.08.2017
 *      Author: hendrik
 */

#include <core/SystemMode.h>

namespace Core {

SystemMode SystemMode::mInstance;

SystemMode::SystemMode() {
	for(cpu_t cpu = 0; cpu < NR_CPUS; cpu++) {
		mSYSModeCounter[cpu] = 0;
		mIRQCounter[cpu] = 0;
	}
}


} /* namespace Core */
