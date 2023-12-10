MAGIC         equ 0xe85250d6
ARCH          equ 0
HEADER_LENGTH equ header_end - header_start
CHECKSUM      equ  0x100000000 - (MAGIC + HEADER_LENGTH + ARCH)
section .multiboot
header_start:
	dd MAGIC ; multiboot header. The helps the bootloader recognize the format.
	dd ARCH  ; This will enable the protected mode for i386 architecture.
	dd HEADER_LENGTH
    dd CHECKSUM

	; end tag
	dw 0
	dw 0
	dd 8
header_end: