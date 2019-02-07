	
	/* Align to pagesize (1MB) */
	. = ALIGN(0x100000); 
    .text :
    {
    __text_segment__start = .;
      *(".text*")
    __text_segment__end = .;
      KEEP(*(".fini"))
      KEEP(*(".gnu.linkonce.*"))
    }


   .data :
    {
      *(".data")
      *(".data$")
      *(".rodata")
      *(".rodata.*")
      *(".got")
      *(".got.plt")
      *(".eh_frame")
      *(".eh_fram")
      *(".jcr")
      *(".note.*")
    }
    	    
	/* BSS */
	.bss : 
    {
      __bss_start = .;
      *(".bss")
      *(".bss.*")
      __bss_end  = .;
    }
    	

    /* Uncache benchmark results */
	/* Align to pagesize (1MB) */
	. = ALIGN(0x100000); 
	.benchmark_results : 
	{
		__benchmark_results__start = .;
		./build/benchmarking/ResultContainer.o(.benchmark_results);
		./build/benchmarking/ResultContainer.o(.text);
		./build/benchmarking/ResultContainer.o(.data);
	}
	
	.benchmark_results_data : 
	{
		./build/benchmarking/ResultContainer.o(.bss);
		__benchmark_results__end = .;
	}
	
	. = ALIGN(0x1000);
	__osc_move_test = .;
	
	/* 
	 * Necessary for CATTest baremetal application 
	 *. = ALIGN(0x200000);
	 * profiling_space = .;
	 */
	