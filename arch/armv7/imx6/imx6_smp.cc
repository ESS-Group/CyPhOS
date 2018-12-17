
#include "imx6_smp.h"

/* Debug helpers */
#define TAG "IMX6_BOOT"

#include <arch/armv7/cortexa9/boot/initOS.h>

#include <common/memreg.h>

#include "imx6_smp.h"
#include "imx6_registers.h"

#include "driver/IMX6Watchdog.h"

/* Entry point for secondary cores boot code */
extern dword_t _secondary_boot;

/* Include GIC for wakeup interrupt */
#include <driver/ARMGIC.h>

/* Include bit manipulation tools */
#include <common/bitmanipulation.h>

void startup_core(cpu_t core) {
	// Set the actual address of the boot code
	// Set the cores entrypoint registers
	switch (core) {
	case 1:
		WRITE_REGISTER(SRC_GPR3,(uintptr_t)(void*)&_secondary_boot);
		WRITE_REGISTER(SRC_GPR4,0);
		break;
	case 2:
		WRITE_REGISTER(SRC_GPR5,(uintptr_t)(void*)&_secondary_boot);
		WRITE_REGISTER(SRC_GPR6,0);
		break;
	case 3:
		WRITE_REGISTER(SRC_GPR7,(uintptr_t)(void*)&_secondary_boot);
		WRITE_REGISTER(SRC_GPR8,0);
		break;
	}

	// Reset the specific core
	dword_t scr = READ_REGISTER(SRC_SCR);

	switch (core) {
	// Set the enable(reset?) bit
	case 1:
		scr |= (SRC_CORE_1_ENABLE | SRC_CORE_1_DBG_RST | SRC_CORE_1_RST);
		break;
	case 2:
		scr |= (SRC_CORE_2_ENABLE | SRC_CORE_2_DBG_RST | SRC_CORE_2_RST);
		break;
	case 3:
		scr |= (SRC_CORE_3_ENABLE | SRC_CORE_3_DBG_RST | SRC_CORE_3_RST);
		break;
	}
//	DEBUG_STREAM(TAG,"Setting SCR: "  << hex << scr);
	DEBUG_STREAM(TAG,"Waking CPU: " << dec << (uint32_t)core);


	// Write the control register, this should reset the core
	WRITE_REGISTER(SRC_SCR, scr);
	ISB;DSB;
//	dump_src_registers();
}

/**
 * Helper function to dump all registers of the system reset control
 */
void dump_src_registers() {
	DEBUG_STREAM(TAG,"SRC_SCR: " << hex << READ_REGISTER(SRC_SCR));
	DEBUG_STREAM(TAG,"SRC_SBMR1: " << hex << READ_REGISTER(SRC_SBMR1));
	DEBUG_STREAM(TAG,"SRC_SRSR: " << hex << READ_REGISTER(SRC_SRSR));
	DEBUG_STREAM(TAG,"SRC_SISR: " << hex << READ_REGISTER(SRC_SISR));
	DEBUG_STREAM(TAG,"SRC_SIMR: " << hex << READ_REGISTER(SRC_SIMR));
	DEBUG_STREAM(TAG,"SRC_SBMR2: " << hex << READ_REGISTER(SRC_SBMR2));

	DEBUG_STREAM(TAG,"SRC_GPR1: " << hex << READ_REGISTER(SRC_GPR1));
	DEBUG_STREAM(TAG,"SRC_GPR2: " << hex << READ_REGISTER(SRC_GPR2));
	DEBUG_STREAM(TAG,"SRC_GPR3: " << hex << READ_REGISTER(SRC_GPR3));
	DEBUG_STREAM(TAG,"SRC_GPR4: " << hex << READ_REGISTER(SRC_GPR4));
	DEBUG_STREAM(TAG,"SRC_GPR5: " << hex << READ_REGISTER(SRC_GPR5));
	DEBUG_STREAM(TAG,"SRC_GPR6: " << hex << READ_REGISTER(SRC_GPR6));
	DEBUG_STREAM(TAG,"SRC_GPR7: " << hex << READ_REGISTER(SRC_GPR7));
	DEBUG_STREAM(TAG,"SRC_GPR8: " << hex << READ_REGISTER(SRC_GPR8));
	DEBUG_STREAM(TAG,"SRC_GPR9: " << hex << READ_REGISTER(SRC_GPR9));
	DEBUG_STREAM(TAG,"SRC_GPR10: " << hex << READ_REGISTER(SRC_GPR10));}

void restart_system() {
	OSC_PREFIX(IMX6Watchdog)::IMX6Watchdog::mInstance.restart_system();
}

