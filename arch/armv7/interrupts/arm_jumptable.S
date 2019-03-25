.text
/*
 * Labels to get start and end address for later memmove
 */
.global first_jump_table_start
.global first_jump_table_end

/*
 *  ldr pc,[pc,#0x18] is equivalent to an jump forward in
 *  pc address
 */

first_jump_table_start:
	ldr pc,[pc,#0x18]	/*	Reset (only in secure mode)	*/
	ldr pc,[pc,#0x18]	/*	Undefined Instruction	*/
	ldr pc,[pc,#0x18]	/*	Supervisor call	*/
	ldr pc,[pc,#0x18]	/*	Prefetch Abort	*/
	ldr pc,[pc,#0x18]	/*	Data Abort	*/
	ldr pc,[pc,#0x18]	/*	HypTrap	*/
	ldr pc,[pc,#0x18]	/*	IRQ	*/
	ldr pc,[pc,#0x18]	/*	FIQ	*/
first_jump_table_end:

/** Addresses of the c-functions
*  to which the interrupt handler jumps
*/
second_jump_table_start:
	.word exception_reset
	.word exception_undefined_instruction
	.word exception_supervisor_call
	.word exception_prefetch_abort
	.word exception_data_abort
	.word exception_hypervisor
	.word exception_irq_interrupt
	.word exception_fiq_interrupt
second_jump_table_end:
