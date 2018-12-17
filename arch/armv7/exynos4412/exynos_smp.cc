/*
 * exynos_smp.c
 *
 *  Created on: 14.07.2015
 *      Author: hendrik
 */
#include "exynos_smp.h"

/* Debug helpers */
#define TAG "EXYNOS_BOOT"

#include <arch/armv7/cortexa9/boot/initOS.h>

#include <common/memreg.h>

#include "exynos_smc.h"

/* Entry point for secondary cores boot code */
extern dword_t _secondary_boot;

/* Include GIC for wakeup interrupt */
#include <driver/ARMGIC.h>


void startup_core(cpu_t core) {


	/* Start setting up secondary cores here */
	void *secondary_entry_point = (void*)&_secondary_boot;

	/* Write the boot address to the right register */
	WRITE_REGISTER(EXYNOS4x12_PA_SYSRAM_NS + 0x1c + 4*core,secondary_entry_point);


	/* Enabling secondary power */
	if(!(READ_REGISTER(S5P_ARM_CORE1_STATUS) & S5P_CORE_LOCAL_PWR_EN)) {
//		DEBUG_STREAM(TAG,"Enabling secondary power quatsch");
		WRITE_REGISTER(S5P_ARM_CORE1_CONFIGURATION,S5P_CORE_LOCAL_PWR_EN);

		while((READ_REGISTER(S5P_ARM_CORE1_STATUS) & S5P_CORE_LOCAL_PWR_EN) != S5P_CORE_LOCAL_PWR_EN) {
		}
	}

	exynos_smc(SMC_CMD_CPU1BOOT,core,0,0);
	__asm__ __volatile__ (".arch_extension sec\n"
			"SEV;isb\ndsb\n");
}


void restart_system() {
	/* This will trigger the restart of the SOC by issuing a reset signal */
	WRITE_REGISTER(0x10020000+0x400,1);

}
