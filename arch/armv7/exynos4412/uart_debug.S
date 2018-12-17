.global _exynos_print_hex

/* Prints hex value */
_exynos_print_hex:
	stmfd	sp!, {r4-r11, lr}
	LDR r0,uart_out_address
	mov r1,#0x42
	str r1,[r0]
	ldmfd	sp!, {r4-r11, pc}

uart_out_address:
	.word 0x13810020
