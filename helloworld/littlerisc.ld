ENTRY(_intc00);

SECTIONS
{
	. = 0x0;
	.text : { 
		*(.text.prologue);
		*(.text); 
	}
	_edata = .;
	_end = .;
	
	. = 0x4000;
	.data : { *(.data) }
	.bss : { *(.bss) }
	__global_pointer$ = .;

}
