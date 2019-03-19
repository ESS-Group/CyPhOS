. = ALIGN(0x200000);
__oscs__start = .;
INCLUDE OSC.ld
__oscs__end = .;	

/* Align to page granularity */
. = ALIGN(0x200000);
/* Critical OSC-Stuff */
.critical_osc : 
{
	__critical_osc_start = .;

	
	./build/common/Queue.o(".text*")

	./build/eventhandling/*.o("*")

	./build/eventhandling/StackPool.o(".text*")
	./build/interrupts/InterruptDispatcher.o(".text*")

 	./build/eventhandling/MultiQueuePriorityStrategy.o(".text")
	./build/common/RandomGenerator.o(".text")
	./build/arch/x86/core/SystemMode.o(".text")
	
	/* Cache management */
	./build/arch/x86/driver/CATManager.o(".text")
	./build/arch/x86/driver/X86MMU.o(".text")
	./build/driver/GenericMMU.o(".text")
	./build/cachemanagement/CacheColoring.o(".text")
	
	/* Arch specific interrupt handler */
	
	./build/common/baremetal.o(".text")
	__critical_osc_text_end = .;
	
	./build/arch/x86/driver/SMPManager.o("*")
	./build/arch/x86/common/X86System.o("*")


	./build/interrupts/InterruptDispatcher.o(".data*")
	./build/arch/x86/core/SystemMode.o(".*data*")
 	

	./build/common/RandomGenerator.o(".data")

	./build/arch/x86/driver/CATManager.o(".data*")
	./build/arch/x86/driver/X86MMU.o(".data*")
	./build/driver/GenericMMU.o(".data*")

	./build/arch/x86/interrupts/*.o("*")

	__NS_CRITICAL_OSC__bss_start = .;
	./build/interrupts/InterruptDispatcher.o(".bss")

	./build/common/RandomGenerator.o(".bss")

	./build/arch/x86/driver/CATManager.o(".bss")
	./build/arch/x86/driver/X86MMU.o(".bss")
	./build/driver/GenericMMU.o(".bss")


	./build/arch/x86/cachemanagement/*.o("*")
	./build/cachemanagement/*.o("*")
	
	./build/arch/x86/core/SystemMode.o(".bss")
	./build/core/SystemMode.o(".bss")
	__NS_CRITICAL_OSC__bss_end = .;
	
	*(".critical_data")
	*(".eventHandling*")

	/* Full-descending stack for cores (please be cautious with offsets) */
    . = ALIGN(0x1000);
	__runtime_stack_end = .;
	. = . + 0x1000 * 4;
	__runtime_stack_start = .;
	__critical_osc_end = .;
	/* ASSERT( . - __critical_osc_start <= 0x10000 , "Permanent preloading section >64k");*/
}
