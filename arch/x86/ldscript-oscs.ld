. = ALIGN(0x80000);
__oscs__start = .;
INCLUDE OSC.ld
__oscs__end = .;	

/* Align to cache way size (512kB) */
. = ALIGN(0x80000);
/* Critical OSC-Stuff */
.critical_osc : 
{
	__critical_osc_start = .;
	/* Include generic critical OSC */
	INCLUDE ldscript-osc-generic.ld

	./build/arch/x86/core/SystemMode.o(".*text*")
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".*text*")
	./build/arch/x86/driver/CATManager.o(".*text*")
	./build/arch/x86/driver/X86MMU.o(".*text*")
	./build/arch/x86/cachemanagement/X86CacheColoring.o(".*text*")
	./build/arch/x86/interrupts/interrupt_handler.o(".*text*")
	./build/arch/x86/driver/SMPManager.o(".*text*")
	./build/arch/x86/driver/LocalAPIC.o(".*text*")
	./build/arch/x86/common/X86System.o(".*text*")
	
	./build/arch/x86/core/SystemMode.o(".*data*")
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".*data*")
	./build/arch/x86/driver/CATManager.o(".*data*")
	./build/arch/x86/driver/X86MMU.o(".*data*")
	./build/arch/x86/cachemanagement/X86CacheColoring.o(".*data*")
	./build/arch/x86/interrupts/interrupt_handler.o(".*data*")
	./build/arch/x86/driver/SMPManager.o(".*data*")
	./build/arch/x86/driver/LocalAPIC.o(".*data*")
	./build/arch/x86/common/X86System.o(".*data*")
	
	./build/arch/x86/cachemanagement/X86CacheManagement.o(".bss")
	./build/arch/x86/driver/CATManager.o(".bss")
	./build/arch/x86/driver/X86MMU.o(".bss")
	./build/arch/x86/cachemanagement/X86CacheColoring.o(".bss")
	./build/arch/x86/core/SystemMode.o(".bss")
	./build/arch/x86/interrupts/interrupt_handler.o(".bss")
	./build/arch/x86/driver/SMPManager.o(".bss")
	./build/arch/x86/driver/LocalAPIC.o(".bss")
	./build/arch/x86/common/X86System.o(".bss")
	__critical_osc_end = .;
}
