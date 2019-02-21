	. = ALIGN(4);
	
	.boot :
	{
		KEEP (*(.boot))
	}
	
	.sboot :
	{
		KEEP (*(.sboot))
	}
	
	/* Full-descending stack for cores (please be cautious with offsets) */
    . = ALIGN(0x8);
	__boot_stack_end = .;
	. = . + 512 * 4;
	__boot_stack_start = .;
	
	

