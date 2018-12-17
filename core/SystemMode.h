/*
 * SystemMode.h
 *
 *  Created on: 24.08.2017
 *      Author: hendrik
 */

#ifndef CORE_SYSTEMMODE_H_
#define CORE_SYSTEMMODE_H_

#include <common/types.h>

namespace Core {

class SystemMode {
public:
	SystemMode();

	void disableInterrupts();
	void enableInterrupts();

	static SystemMode mInstance;
private:

	volatile uint32_t mIRQCounter[NR_CPUS];
	volatile uint32_t mSYSModeCounter[NR_CPUS];

};

} /* namespace Core */

#endif /* CORE_SYSTEMMODE_H_ */
