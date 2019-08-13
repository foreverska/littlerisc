.global _intc00
.section ".text.prologue"

_intc00:
	addi sp, zero, 0x600
	call _start
