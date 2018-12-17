/*
 * omap4460_smp.c
 *
 *  Created on: 14.07.2015
 *      Author: hendrik
 */
#include <arch/armv7/omap4460/omap4460_smp.h>

/* Debug helpers */
#define TAG "OMAP_BOOT"

#include <common/memreg.h>

#include "omap4460_smp.h"
#include "omap4460_registers.h"

/* Entry point for secondary cores boot code */
extern dword_t _secondary_boot;

/* Include GIC for wakeup interrupt */
#include <driver/ARMGIC.h>

/* Include bit manipulation tools */
#include <common/bitmanipulation.h>

#include <arch/armv7/cortexa9/boot/initOS.h>

void startup_core(cpu_t core) {
	/* Start setting up secondary cores here */
	void *secondary_entry_point = (void*)&_secondary_boot;

	dword_t auxBootReg0 = READ_REGISTER(REG_AUX_CORE_BOOT_0);

	auxBootReg0 = setbit(auxBootReg0, 3, true);
	auxBootReg0 = setbit(auxBootReg0, 2, true);

	WRITE_REGISTER(REG_AUX_CORE_BOOT_1,(dword_t)secondary_entry_point);
	WRITE_REGISTER(REG_AUX_CORE_BOOT_0,(dword_t)auxBootReg0);

	/* This event signals the core to boot */
	__asm__ __volatile__ (""
			".arch_extension sec\n"
			".arch_extension mp\n"
			"sev;"
			"isb\ndsb\n");
}


void restart_system() {
	/* This will trigger the restart of the SOC by issuing a reset signal */
	WRITE_REGISTER(REG_PRM_RSTCTRL,2);
	DEBUG_STREAM(TAG,"System restarted, LOL nope this shouldn't happen");
}
