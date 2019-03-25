/*
 * initOS.cc
 *
 *  Created on: 10.04.2015
 *      Author: hendrik
 */

#include <common/debug.h>

/* Debug helpers */
#define TAG "EXYNOS_INIT"

#include <common/armhelper.h>

#include <common/memreg.h>
#include <common/types.h>

/* Include GIC driver for interrupt control */
#include <driver/ARMGIC.h>

/* Include startup code for secondary cores */
#include <arch/armv7/exynos4412/exynos_smp.h>

/* Include GlobalTimer code */
#include <arch/armv7/exynos4412/driver/ExynosMultiCoreTimer.h>

/* Include ExynosUART */
#include <arch/armv7/exynos4412/driver/ExynosUartDriver.h>

/* FIXME: make it platform specific */
/* Include Exynos 4412 clock management */
#include <arch/armv7/exynos4412/driver/ExynosClockManagement.h>

#include <arch/armv7/cortexa9/boot/initOS.h>

void init_soc() {
#ifdef CONFIG_DEBUG
#ifdef CONFIG_CLOCK_DIAGNOSIS
	ExynosClockManagement::mInstance.dumpAPLLStatus();
	ExynosClockManagement::mInstance.dumpCMUTopStatus();
	ExynosClockManagement::mInstance.dumpCMURightbusStatus();
#endif
#endif

	/*
	 * Enable the UART_1 interrupts of the exynos processor.
	 */
	OSC_PREFIX(InputOutput)::ExynosUart::pInstance.setRXInterrupts(true);

#ifdef CONFIG_ENABLE_EXYNOS_MCT
	DEBUG_STREAM(TAG,"Initialize GenericTimer provided by Exynos MCT");
	OSC_PREFIX(Timer)::ExynosMultiCoreTimer::pInstance.initializeTimer();
	DEBUG_STREAM(TAG,"Timer should be running");
#endif
}

void init_soc_secondary() {

}


