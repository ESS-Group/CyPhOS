.section .text,"ax"

#include <component/OSC.h>

OSC_FUNC_ASM(CacheControl,asm_enable_instruction_cache)
			DSB
			ISB
			STMFD	sp!, {r4-r11, lr}
			// Read the SCTRL register out of coprocessor cp15
			MRC p15, 0, r0, c1, c0, 0
			// Set the i(nstruction-cache)-bit
			ORR r0,r0,#(1 << 12)
			// Write the SCTRL register out of coprocessor cp15
			MCR p15, 0, r0, c1, c0, 0
			dsb
			isb
			LDMFD	sp!, {r4-r11, pc}


OSC_FUNC_ASM(CacheControl,asm_disable_instruction_cache)
			DSB
			ISB
			STMFD	sp!, {r4-r11, lr}
			/* Read the SCTRL register out of coprocessor cp15 */
			MRC p15, 0, r0, c1, c0, 0
			/* Clear the i(nstruction-cache)-bit */
			BIC r0,r0,#(1 << 12)
			/* Write the SCTRL register out of coprocessor cp15	 */
			MCR p15, 0, r0, c1, c0, 0
			DSB
			ISB
			LDMFD	sp!, {r4-r11, pc}


OSC_FUNC_ASM(CacheControl,asm_invalidate_instruction_cache)
			DSB
			ISB
			STMFD	sp!, {r4-r11, lr}
			MOV r1, #0
			MCR p15, 0, r1, c7, c5, 0
			DSB
			ISB
			LDMFD	sp!, {r4-r11, pc}


OSC_FUNC_ASM(CacheControl,asm_enable_unified_cache)
			DSB
			ISB
			STMFD	sp!, {r4-r11, lr}
			/* Read the SCTRL register out of coprocessor cp15 */
			MRC p15, 0, r0, c1, c0, 0
			/* Set the cache-bit */
			ORR r0,r0,#(1 << 2)
			/* Write the SCTRL register out of coprocessor cp15 */
			MCR p15, 0, r0, c1, c0, 0
			DSB
			ISB
			LDMFD	sp!, {r4-r11, pc}


OSC_FUNC_ASM(CacheControl,asm_disable_unified_cache)
			DSB
			ISB
			STMFD	sp!, {r4-r11, lr}
			/* Read the SCTRL register out of coprocessor cp15 */
			MRC p15, 0, r0, c1, c0, 0
			/* Set the cache-bit */
			BIC r0,r0,#(1 << 2)
			/* Write the SCTRL register out of coprocessor cp15 */
			MCR p15, 0, r0, c1, c0, 0
			DSB
			ISB
			LDMFD	sp!, {r4-r11, pc}


/**
  * Cache maintainence as described in ARMv7-A ARM (p. B2-1286)
  *
**/

OSC_FUNC_ASM(CacheControl,asm_clean_and_invalidate_data_caches)
	STMFD	sp!, {r4-r11, lr}
	DSB
	MRC p15, 1, R0, c0, c0, 1 // Read CLIDR into R0
	ANDS R3, R0, #0x07000000
	MOV R3, R3, LSR #23  // Cache level value (naturally aligned)
	BEQ 5f
	MOV R10, #0
1:
	ADD R2, R10, R10, LSR #1 // Work out 3 x cachelevel
	MOV R1, R0, LSR R2 // bottom 3 bits are the Cache type for this level
	AND R1, R1, #7 // get those 3 bits alone
	CMP R1, #2
	BLT 4f  // no cache or only instruction cache at this level
	MCR p15, 2, R10, c0, c0, 0 // write CSSELR from R10
	ISB // ISB to sync the change to the CCSIDR
	MRC p15, 1, R1, c0, c0, 0  // read current CCSIDR to R1
	AND R2, R1, #7 // extract the line length field
	ADD R2, R2, #4 // add 4 for the line length offset (log2 16 bytes)
	LDR R4, =0x3FF
	ANDS R4, R4, R1, LSR #3 // R4 is the max number on the way size (right aligned)
	CLZ R5, R4 // R5 is the bit position of the way size increment
	MOV R9, R4 // R9 working copy of the max way size (right aligned)
2:
	LDR R7, =0x00007FFF
	ANDS R7, R7, R1, LSR #13 // R7 is the max number of the index size (right aligned)
3:
	ORR R11, R10, R9, LSL R5 // factor in the way number and cache number into R11
	ORR R11, R11, R7, LSL R2 // factor in the index number
	MCR p15, 0, R11, c7, c14, 2 // DCCISW, clean and invalidate by set/way
	SUBS R7, R7, #1 // decrement the index
	BGE 3b
	SUBS R9, R9, #1 // decrement the way number
	BGE 2b
4:
	ADD R10, R10, #2 // increment the cache number
	CMP R3, R10
	BGT 1b
	DSB
	ISB
5:
	LDMFD	sp!, {r4-r11, pc}



OSC_FUNC_ASM(CacheControl,asm_invalidate_data_caches)
	STMFD	sp!, {r4-r11, lr}
	DSB
	MRC p15, 1, R0, c0, c0, 1 // Read CLIDR into R0
	ANDS R3, R0, #0x07000000
	MOV R3, R3, LSR #23  // Cache level value (naturally aligned)
	BEQ 5f
	MOV R10, #0
1:
	ADD R2, R10, R10, LSR #1 // Work out 3 x cachelevel
	MOV R1, R0, LSR R2 // bottom 3 bits are the Cache type for this level
	AND R1, R1, #7 // get those 3 bits alone
	CMP R1, #2
	BLT 4f  // no cache or only instruction cache at this level
	MCR p15, 2, R10, c0, c0, 0 // write CSSELR from R10
	ISB // ISB to sync the change to the CCSIDR
	MRC p15, 1, R1, c0, c0, 0  // read current CCSIDR to R1
	AND R2, R1, #7 // extract the line length field
	ADD R2, R2, #4 // add 4 for the line length offset (log2 16 bytes)
	LDR R4, =0x3FF
	ANDS R4, R4, R1, LSR #3 // R4 is the max number on the way size (right aligned)
	CLZ R5, R4 // R5 is the bit position of the way size increment
	MOV R9, R4 // R9 working copy of the max way size (right aligned)
2:
	LDR R7, =0x00007FFF
	ANDS R7, R7, R1, LSR #13 // R7 is the max number of the index size (right aligned)
3:
	ORR R11, R10, R9, LSL R5 // factor in the way number and cache number into R11
	ORR R11, R11, R7, LSL R2 // factor in the index number
	MCR p15, 0, R11, c7, c6, 2 // DCISW, invalidate by set/way
	SUBS R7, R7, #1 // decrement the index
	BGE 3b
	SUBS R9, R9, #1 // decrement the way number
	BGE 2b
4:
	ADD R10, R10, #2 // increment the cache number
	CMP R3, R10
	BGT 1b
	DSB
	ISB
5:
	LDMFD	sp!, {r4-r11, pc}

OSC_FUNC_ASM(CacheControl,asm_clean_data_caches)
	STMFD	sp!, {r4-r11, lr}
	DSB
	MRC p15, 1, R0, c0, c0, 1 // Read CLIDR into R0
	ANDS R3, R0, #0x07000000
	MOV R3, R3, LSR #23  // Cache level value (naturally aligned)
	BEQ 5f
	MOV R10, #0
1:
	ADD R2, R10, R10, LSR #1 // Work out 3 x cachelevel
	MOV R1, R0, LSR R2 // bottom 3 bits are the Cache type for this level
	AND R1, R1, #7 // get those 3 bits alone
	CMP R1, #2
	BLT 4f  // no cache or only instruction cache at this level
	MCR p15, 2, R10, c0, c0, 0 // write CSSELR from R10
	ISB // ISB to sync the change to the CCSIDR
	MRC p15, 1, R1, c0, c0, 0  // read current CCSIDR to R1
	AND R2, R1, #7 // extract the line length field
	ADD R2, R2, #4 // add 4 for the line length offset (log2 16 bytes)
	LDR R4, =0x3FF
	ANDS R4, R4, R1, LSR #3 // R4 is the max number on the way size (right aligned)
	CLZ R5, R4 // R5 is the bit position of the way size increment
	MOV R9, R4 // R9 working copy of the max way size (right aligned)
2:
	LDR R7, =0x00007FFF
	ANDS R7, R7, R1, LSR #13 // R7 is the max number of the index size (right aligned)
3:
	ORR R11, R10, R9, LSL R5 // factor in the way number and cache number into R11
	ORR R11, R11, R7, LSL R2 // factor in the index number
	MCR p15, 0, R11, c7, c10, 2 // DCCSW, clean by set/way
	SUBS R7, R7, #1 // decrement the index
	BGE 3b
	SUBS R9, R9, #1 // decrement the way number
	BGE 2b
4:
	ADD R10, R10, #2 // increment the cache number
	CMP R3, R10
	BGT 1b
	DSB
	ISB
5:
	LDMFD	sp!, {r4-r11, pc}
