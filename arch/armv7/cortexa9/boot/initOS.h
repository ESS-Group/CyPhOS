/*
 * initOS.h
 *
 *  Created on: 10.11.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_CORTEXA9_BOOT_INITOS_H_
#define ARCH_ARMV7_CORTEXA9_BOOT_INITOS_H_

#include <common/types.h>

extern "C" {
[[noreturn]] void init_primary_cpu_c();
[[noreturn]] void init_secondary_cpu_c();

	void init_soc();
	void init_soc_secondary();

	void startup_core(cpu_t core);
}

#endif /* ARCH_ARMV7_CORTEXA9_BOOT_INITOS_H_ */
