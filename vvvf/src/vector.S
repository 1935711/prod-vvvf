.section ".text"

.global vector_ins
.global vector_start
.global vector_end

vector_ins:
	ldral	pc, [pc, #72]			@ 56 = 64 - 8. 8 because PC points 2 instructions ahead.
vector_start:
	@ 0x00 = Reset.
	@ 0x04 = Undefined instruction.
	@ 0x08 = SWI.
	@ 0x0C = Prefetch abort.
	@ 0x10 = Data abort.
	.word	irq_reset
	.word	irq_insundef
	.word	irq_swi
	.word	irq_prefetchabort
	.word	irq_dataabort
vector_end:
