#include <common/ARM_common.h>

.text


.global exception_irq_interrupt



exception_irq_interrupt:
	/* Subtract offset 4 for ARM-mode (0 for thumb mode) p.B1-1220 ARM ARM */
	SUB lr,lr,#4

	/* Store the old context */
	STMFD sp!,{r0-r12,lr}
	/* Branch to interrupt dispatcher. */
	BL exception_irq_interrupt_c

	/* Jump back. */
	LDMFD sp!,{r0-r12,pc}^

