/*
 * exynos_mem_map.h
 *
 *  Created on: 16.07.2015
 *      Author: hendrik
 */

#ifndef ARCH_IMX6_MEM_MAP_H_
#define ARCH_IMX6_MEM_MAP_H_

#include <arch/armv7/driver/memory_map.h>

// 	{0x0,0x00017FFF,MEMTYPE_BLOCKED,NONCACHEABLE,"Boot ROM"},

#define IMX6_MEMORY_MAP {\
		{0x10000000,0x80000000,MEMTYPE_DRAM,CACHEABLE,"MMDC - DDR"}, \
		{0x00000000,0x0FFFFFFF,MEMTYPE_DEVICE_MEMORY,NONCACHEABLE,"i.MX6 Device Memory"}, \
		{0x0,0x0,MEMTYPE_MEMORY_SECTION_TYPE_LAST,NONCACHEABLE,"LISTEND"} \
}

#endif /* ARCH_IMX6_MEM_MAP_H_ */
