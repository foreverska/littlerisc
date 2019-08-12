SECTIONS
{

	. = 0x0;
	.test : { *(.text) }
	_edata = .;
	_end = .;
	
	. = 0x600;
	.data : { *(.data) }
	.bss : { *(.bss) }
	__global_pointer$ = .;

}
