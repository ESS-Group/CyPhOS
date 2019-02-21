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
	__pagetable_start = .;
	. = . + 4096 * 4;
	__pagetable_end = .;
    	

	./build/arch/armv7/interrupts/arm_jumptable.oS("*")

	./build/arch/armv7/interrupts/first_stage_handler_critical.oS("*")

  	./build/arch/armv7/interrupts/first_stage_handler.o(".text")
	./build/common/Queue.o(".text*")

	./build/eventhandling/EventQueue.o(".text*")
	./build/eventhandling/EventPriorityQueue.o(".text*")
	./build/eventhandling/EventHandler.o(".text")

	./build/eventhandling/StackPool.o(".text*")
	./build/cachemanagement/GenericCacheManagement.o(".text")
	./build/interrupts/InterruptDispatcher.o(".text*")

 	./build/eventhandling/MultiQueuePriorityStrategy.o(".text")
	./build/common/RandomGenerator.o(".text")
	./build/arch/armv7/driver/ARMMMU.o(".text")
	./build/core/SystemMode.o(".text")
	
	__critical_osc_text_end = .;
	
	./build/arch/armv7/interrupts/first_stage_handler.o(".data")
		
	./build/eventhandling/EventHandler.o(".*data*")
	./build/arch/armv7/eventhandling/EventHandler.oS("*")
	
	./build/eventhandling/StackPool.o(".*data*")
	
	./build/cachemanagement/GenericCacheManagement.o(".*data*")
	./build/arch/armv7/driver/ARMMMU.o(".*data*")
	
	./build/interrupts/InterruptDispatcher.o(".data*")
	./build/core/SystemMode.o(".*data*")

 	
 	./build/eventhandling/MultiQueuePriorityStrategy.o(".*data*")

	./build/common/RandomGenerator.o(".data")

	__NS_CRITICAL_OSC__bss_start = .;
	./build/eventhandling/EventHandler.o(".bss")
	./build/arch/armv7/driver/ARMMMU.o(".bss")
	./build/interrupts/InterruptDispatcher.o(".bss")
	./build/arch/armv7/interrupts/first_stage_handler.o(".bss")
	./build/eventhandling/StackPool.o(".bss")
	./build/eventhandling/MultiQueuePriorityStrategy.o(".bss")
	
	./build/cachemanagement/GenericCacheManagement.o(".bss")
	./build/common/RandomGenerator.o(".bss")

	./build/core/SystemMode.o(".bss")
	__NS_CRITICAL_OSC__bss_end = .;
	
	*(".critical_data")
	*(".text")
	
	. = ALIGN(0x8);
	__irq_stack_end = .;
	. = . + 512 * 4;
	__irq_stack_start = .;
	
	/* Full-descending stack for cores (please be cautious with offsets) */
    . = ALIGN(0x8);
	__boot_stack_end = .;
	. = . + 512 * 4;
	__boot_stack_start = .;

	
	__critical_osc_end = .;
	/* ASSERT( . - __critical_osc_start <= 0x10000 , "Permanent preloading section >64k");*/
	
}
. = ALIGN(0x100000);