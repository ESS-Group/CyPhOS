/*
 * initOS.h
 *
 *  Created on: 30.05.2018
 *      Author: hendrik
 */

#ifndef ARCH_X86_BOOT_INITOS_H_
#define ARCH_X86_BOOT_INITOS_H_


#include <common/types.h>

extern "C" {
[[noreturn]] void init_primary_cpu_c();
[[noreturn]] void init_secondary_cpu_c();

	void init_soc();
	void init_soc_secondary();

	void startup_core(cpu_t core);
}

#endif /* ARCH_X86_BOOT_INITOS_H_ */
