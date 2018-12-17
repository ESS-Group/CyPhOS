/*
 * IMX6CCMRegisters.h
 *
 *  Created on: 15.11.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6CCMREGISTERS_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6CCMREGISTERS_H_

/**
 * Register addresses of the clock controller module of the i.MX6.
 * Defined in i.MX6 TRM p. 835.
 */
#define CCM_ANALOG_PLL_ARM 0x020C8000

#define CCM_ANALOG_PLL_USB1 0x020C8010
#define CCM_ANALOG_PLL_USB2 0x020C8020
#define CCM_ANALOG_PLL_SYS 0x020C8030


/* Clock gating registers */
#define CCM_CCGR_BASE 0x20C4000
#define CCM_CCGR1 CCM_CCGR_BASE + 0x6C
#define CCM_CCOSR CCM_CCGR_BASE + 0x60

#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6CCMREGISTERS_H_ */
