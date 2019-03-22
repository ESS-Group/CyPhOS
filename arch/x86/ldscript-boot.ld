    .boot :
    {
	./build/arch/x86/boot/startup-64.oS("*")
	./build/arch/x86/boot/startup_ap-64.oS("*")
	./build/arch/x86/boot/paging.oS("*")
	./build/arch/x86/boot/initOS.o("*")

	/* Full-descending stack for cores (please be cautious with offsets) */
    . = ALIGN(0x1000);
	__boot_stack_end = .;
	. = . + 0x1000 * 4;
	__boot_stack_start = .;
	}
	

