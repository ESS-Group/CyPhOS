/*
 * APICTimer.h
 *
 *  Created on: 02.07.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_DRIVER_APICTIMER_H_
#define ARCH_X86_DRIVER_APICTIMER_H_

#include <driver/GenericTimer.h>
#include <component/OSC.h>


BEGIN_DECLARE_OSC_INHERIT(Timer, APICTimer, public GenericTimer)

public:
	APICTimer();

	DECLARE_TRIGGER(handleInterrupt);
	static APICTimer pInstance;
protected:
	void setInterrupt(bool enable);

	void setPeriodic(uint64_t us);
	void setOneshot(uint64_t us);

	uint64_t getHardwareTicks();

private:
};

END_DECLARE_OSC

#endif /* ARCH_X86_DRIVER_APICTIMER_H_ */
