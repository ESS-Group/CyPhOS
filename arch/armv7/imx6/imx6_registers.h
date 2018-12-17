/*
 * omap4460_registers.h
 *
 *  Created on: 26.04.2016
 *      Author: hendrik
 */

#ifndef ARCH_ARMV7_OMAP4460_OMAP4460_REGISTERS_H_
#define ARCH_ARMV7_OMAP4460_OMAP4460_REGISTERS_H_

/* Registers of the system reset control unit (SRC) */

#define SRC_SCR 0x020D8000
#define SRC_SBMR1 0x020D8004
#define SRC_SRSR 0x020D8008
#define SRC_SISR 0x020D8014
#define SRC_SIMR 0x020D8018
#define SRC_SBMR2 0x020D801C

#define SRC_GPR1 0x020D8020
#define SRC_GPR2 0x020D8024
#define SRC_GPR3 0x020D8028
#define SRC_GPR4 0x020D802C
#define SRC_GPR5 0x020D8030
#define SRC_GPR6 0x020D8034
#define SRC_GPR7 0x020D8038
#define SRC_GPR8 0x020D803C
#define SRC_GPR9 0x020D8040
#define SRC_GPR10 0x020D8044

#define SRC_CORE_0_RST (1 << 13)
#define SRC_CORE_1_RST (1 << 14)
#define SRC_CORE_2_RST (1 << 15)
#define SRC_CORE_3_RST (1 << 16)

#define SRC_CORE_0_DBG_RST (1 << 17)
#define SRC_CORE_1_DBG_RST (1 << 18)
#define SRC_CORE_2_DBG_RST (1 << 19)
#define SRC_CORE_3_DBG_RST (1 << 20)

#define SRC_CORE_DBG_RST (1 << 21)

#define SRC_CORE_1_ENABLE (1 << 22)
#define SRC_CORE_2_ENABLE (1 << 23)
#define SRC_CORE_3_ENABLE (1 << 24)




#endif /* ARCH_ARMV7_OMAP4460_OMAP4460_REGISTERS_H_ */
