	. = ALIGN(4);
	
	.boot :
	{
		KEEP (*(.boot))
	}
	
	.sboot :
	{
		KEEP (*(.sboot))
	}
	

