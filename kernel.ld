ENTRY (_start)

SECTIONS {
	. = 0x10000;

	.text : {
		*(.text)
	}
	.data ALIGN (0x1000) : {
		*(.data)
		*(.rodata)
	}
	.bss ALIGN (0x1000) : {
		*(COMMON)
		*(.bss)
	}
	end_kernel = .;
	.ramdisk ALIGN (0x1000) : {
		*(.ramdisk)
	}
}
