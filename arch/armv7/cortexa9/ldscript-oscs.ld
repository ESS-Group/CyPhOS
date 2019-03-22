. = ALIGN(0x100000);
__oscs__start = .;
INCLUDE OSC.ld
__oscs__end = .;	

/* Align to page granularity */
. = ALIGN(0x100000);
/* Critical OSC-Stuff */
.critical_osc : 
{
	__critical_osc_start = .;
	/* Include generic critical OSC */
	INCLUDE ldscript-osc-generic.ld

	./build/arch/armv7/interrupts/*.o("*")
	./build/arch/armv7/interrupts/*.oS("*")
	./build/arch/armv7/eventhandling/*.o("*")
	./build/arch/armv7/eventhandling/*.oS("*")
	./build/arch/armv7/driver/ARMMMU.o("*")
	./build/arch/armv7/cortexa9/cachemanagement/*.o("*")
	./build/arch/armv7/driver/ARMV7CacheControl.o("*")
	./build/arch/armv7/driver/ARMv7CacheColoring.o("*")
	./build/arch/armv7/driver/ARMV7CacheControl.oS("*")

	
	. = ALIGN(0x8);
	__irq_stack_end = .;
	. = . + 512 * 4;
	__irq_stack_start = .;
	__critical_osc_end = .;
}
. = ALIGN(0x100000);