/* Text segments */
. = ALIGN(0x1000);
./build/cachemanagement/GenericCacheManagement.o(".*text*")
./build/cachemanagement/GenericCacheManagement.o(".*data*")
./build/cachemanagement/GenericCacheManagement.o(".bss")

*(".eventHandling*")
./build/eventhandling/EventHandler.o(".text*")
./build/eventhandling/EventPriorityQueue.o(".text*")
./build/eventhandling/MultiQueuePriorityStrategy.o(".text*")
./build/common/Queue.o(".text*")
./build/common/OutputStream.o(".*text*")
./build/eventhandling/EventQueue.o(".text*")
./build/eventhandling/StackPool.o(".text*")
./build/interrupts/InterruptDispatcher.o(".text*")
./build/core/SystemMode.o(".text*")


./build/common/RandomGenerator.o(".*text*")



/* Cache management */
./build/driver/GenericMMU.o(".*text*")
./build/cachemanagement/CacheColoring.o(".*text*")

./build/common/baremetal.o(".*text*")

./build/benchmarking/ResultContainer.o(".*text*")

__critical_osc_text_end = .;

./build/eventhandling/EventHandler.o(".*data*")

./build/eventhandling/StackPool.o(".*data*")

./build/interrupts/InterruptDispatcher.o(".*data*")

./build/eventhandling/MultiQueuePriorityStrategy.o(".*data*")

./build/common/RandomGenerator.o(".*data*")
./build/common/OutputStream.o(".*data*")

./build/driver/GenericMMU.o(".*data*")
./build/cachemanagement/CacheColoring.o(".*data*")
./build/core/SystemMode.o(".*data*")


__NS_CRITICAL_OSC__bss_start = .;
./build/eventhandling/EventHandler.o(".bss")
./build/interrupts/InterruptDispatcher.o(".bss")
./build/eventhandling/StackPool.o(".bss")
./build/eventhandling/MultiQueuePriorityStrategy.o(".bss")

./build/common/RandomGenerator.o(".bss")


./build/driver/GenericMMU.o(".bss")
./build/common/OutputStream.o(".bss")
./build/cachemanagement/CacheColoring.o(".bss")


./build/core/SystemMode.o(".bss")
__NS_CRITICAL_OSC__bss_end = .;

/* Include static triggers and events (SECTION_CRITICAL_DATA macro) */
*(".critical_section")

/* Full-descending stack for cores (please be cautious with offsets) */
. = ALIGN(0x10);
__runtime_stack_end = .;
. = . + 0x1000 * 4;
__runtime_stack_start = .;
