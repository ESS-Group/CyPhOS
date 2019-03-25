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

	./build/arch/armv7/interrupts/*.o(".*text*")
	./build/arch/armv7/interrupts/*.oS(".*text*")
	./build/arch/armv7/eventhandling/*.o(".*text*")
	./build/arch/armv7/eventhandling/*.oS(".*text*")
	./build/arch/armv7/driver/ARMMMU.o(".*text*")
	./build/arch/armv7/cortexa9/cachemanagement/*.o(".*text*")
	./build/arch/armv7/driver/ARMV7CacheControl.o(".*text*")
	./build/arch/armv7/driver/ARMv7CacheColoring.o(".*text*")
	./build/arch/armv7/driver/ARMV7CacheControl.oS(".*text*")

	./build/arch/armv7/interrupts/*.o(".*data*")
	./build/arch/armv7/interrupts/*.oS(".*data*")
	./build/arch/armv7/eventhandling/*.o(".*data*")
	./build/arch/armv7/eventhandling/*.oS(".*data*")
	./build/arch/armv7/driver/ARMMMU.o(".*data*")
	./build/arch/armv7/cortexa9/cachemanagement/*.o(".*data*")
	./build/arch/armv7/driver/ARMV7CacheControl.o(".*data*")
	./build/arch/armv7/driver/ARMv7CacheColoring.o(".*data*")
	./build/arch/armv7/driver/ARMV7CacheControl.oS(".*data*")
		
	./build/arch/armv7/interrupts/*.o(".bss")
	./build/arch/armv7/interrupts/*.oS(".bss")
	./build/arch/armv7/eventhandling/*.o(".bss")
	./build/arch/armv7/eventhandling/*.oS(".bss")
	./build/arch/armv7/driver/ARMMMU.o(".bss")
	./build/arch/armv7/cortexa9/cachemanagement/*.o(".bss")
	./build/arch/armv7/driver/ARMV7CacheControl.o(".bss")
	./build/arch/armv7/driver/ARMv7CacheColoring.o(".bss")
	./build/arch/armv7/driver/ARMV7CacheControl.oS(".bss")
	
	
	. = ALIGN(0x8);
	__irq_stack_end = .;
	. = . + 512 * 4;
	__irq_stack_start = .;
	__critical_osc_end = .;
}
. = ALIGN(0x100000);