/*
 * memory_map.h
 *
 *  Created on: 16.07.2015
 *      Author: hendrik
 */

#ifndef ARCH_MEMORY_MAP_H_
#define ARCH_MEMORY_MAP_H_

enum memory_section_type {
	MEMTYPE_ROM,
	MEMTYPE_SRAM,
	MEMTYPE_DRAM,
	MEMTYPE_BLOCKED,
	MEMTYPE_ICACHE,
	MEMTYPE_DEVICE_MEMORY,
	MEMTYPE_SOC_SPECIFIC,
	MEMTYPE_MEMORY_SECTION_TYPE_LAST
};

enum memory_section_cacheable {
	NONCACHEABLE,
	CACHEABLE
};

typedef struct {
	uintptr_t start;
	uintptr_t length;
	memory_section_type type;
	memory_section_cacheable cacheable;
	const char *name;
} memory_map_entry_t;



#endif /* ARCH_MEMORY_MAP_H_ */
