/*
 * IMX6PMURegisters.h
 *
 *  Created on: 15.11.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_IMX6_DRIVER_IMX6PMUREGISTERS_H_
#define ARCH_ARMV7_IMX6_DRIVER_IMX6PMUREGISTERS_H_

/**
 * Register addresses of the power management unit of the i.MX6.
 * Defined in i.MX6 TRM p. 4459.
 */
#define PMU_REG_1P1 0x020C8110
#define PMU_REG_3P0 0x020C8120
#define PMU_REG_2P5 0x020C8130

#define PMU_REG_CORE 0x020C8140
#define PMU_REG_MISC0 0x020C8150
#define PMU_REG_MISC1 0x020C8160
#define PMU_REG_MISC1_SET 0x020C8164
#define PMU_REG_MISC1_CLR 0x020C8168
#define PMU_REG_MISC1_TOG 0x020C816C

#define PMU_REG_MISC2 0x020C8170
#define PMU_REG_MISC2_SET 0x020C8174
#define PMU_REG_MISC2_CLR 0x020C8178
#define PMU_REG_MISC2_TOG 0x020C817C

#endif /* ARCH_ARMV7_IMX6_DRIVER_IMX6PMUREGISTERS_H_ */
