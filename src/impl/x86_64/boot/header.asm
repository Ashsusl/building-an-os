section .multiboot_header
header_start:
	; magic number
	dd 0xe85250d6 ; multiboot2. This helps the bootloader recognize the header format.
	; architecture
	dd 0 ; it indicates protected mode for i386 architecture.
	; header length
	dd header_end - header_start ; this indicates the size of the Multiboot header.
	; checksum
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start)) ; this is going to be used for validation and integration checks.

	; end tag
	dw 0
	dw 0
	dd 8
header_end:
