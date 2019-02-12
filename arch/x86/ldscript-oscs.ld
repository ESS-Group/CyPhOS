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

	./build/eventhandling/EventQueue.o(".text*")
	./build/eventhandling/EventPriorityQueue.o(".text*")
	./build/eventhandling/EventHandler.o(".text")

	./build/eventhandling/StackPool.o(".text*")
	./build/interrupts/InterruptDispatcher.o(".text*")

 	./build/eventhandling/MultiQueuePriorityStrategy.o(".text")
	./build/common/RandomGenerator.o(".text")
	./build/arch/x86/core/SystemMode.o(".text")
	
	/* Cache management */
	./build/cachemanagement/GenericCacheManagement.o(".text")
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".text")
	./build/arch/x86/driver/CATManager.o(".text")
	
	/* Arch specific interrupt handler */
	./build/arch/x86/interrupts/interrupt_handler.o(".text")
	
	./build/common/baremetal.o(".text")
	__critical_osc_text_end = .;
	
	./build/eventhandling/EventHandler.o(".*data*")
	
	./build/eventhandling/StackPool.o(".*data*")

	./build/interrupts/InterruptDispatcher.o(".data*")
	./build/arch/x86/core/SystemMode.o(".*data*")
 	
 	./build/eventhandling/MultiQueuePriorityStrategy.o(".*data*")

	./build/common/RandomGenerator.o(".data")
	./build/cachemanagement/GenericCacheManagement.o(".data")
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".data")
	./build/arch/x86/driver/CATManager.o(".data")

	./build/arch/x86/interrupts/interrupt_handler.o(".data")

	__NS_CRITICAL_OSC__bss_start = .;
	./build/eventhandling/EventHandler.o(".bss")
	./build/interrupts/InterruptDispatcher.o(".bss")
	./build/eventhandling/StackPool.o(".bss")
	./build/eventhandling/MultiQueuePriorityStrategy.o(".bss")
	
	./build/common/RandomGenerator.o(".bss")

	./build/cachemanagement/GenericCacheManagement.o(".bss")
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".bss")
	./build/arch/x86/driver/CATManager.o(".bss")


	./build/arch/x86/core/SystemMode.o(".bss")
	./build/core/SystemMode.o(".bss")
	./build/arch/x86/interrupts/interrupt_handler.o(".bss")
	__NS_CRITICAL_OSC__bss_end = .;
	
	*(".critical_data")

	/* Full-descending stack for cores (please be cautious with offsets) */
    . = ALIGN(0x1000);
	__runtime_stack_end = .;
	. = . + 0x1000 * 4;
	__runtime_stack_start = .;
	__critical_osc_end = .;
	/* ASSERT( . - __critical_osc_start <= 0x10000 , "Permanent preloading section >64k");*/
}
