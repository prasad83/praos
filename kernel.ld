/* NOTE: No leading underscore for symbols handled in asm: */

ENTRY(entry)
LS_PHYSICAL = 0x100000; /* 1M = load (physical) address */
LS_VIRTUAL  = 0x100000; /* 1M = virtual address */

SECTIONS 
{
	.text LS_VIRTUAL : AT(LS_PHYSICAL)
	{
		LS_CODE = .;
		/* symbols to mark start of code segment */
		code = .; _code = .;
		/* kernel code */
		*(.text)
		/* .rodata is the ELF constant data section */
		*(.rodata*)
		. = ALIGN(4096);
	}
	
	.data : AT(LS_PHYSICAL + (LS_DATA - LS_CODE))
	{
		LS_DATA = .;
		/* symbols to mark start of data segment */
		data = .; _data = .;
		/* kernel data */
		*(.data)
		. = ALIGN(4096);
	}
	
	.bss : AT(LS_PHYSICAL + (LS_BSS - LS_CODE))
	{
		LS_BSS = .;
		/* symbols to mark start of BSS segment */
		bss = .; _bss = .;
		/* kernel bss */
		*(.bss)
		*(COMMON) /* "common" variables */
		. = ALIGN(4096);
	}
	
	/* symbols to mark end of kernel */
	end = .; _end = .;
}
