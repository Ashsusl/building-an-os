MAGIC         equ 0xe85250d6
ARCH          equ 0
HEADER_LENGTH equ header_end - header_start
CHECKSUM      equ  -(MAGIC + HEADER_LENGTH + ARCH)
section .multiboot_header
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
global long_mode_start
extern kernel_main
section .text
bits 64
long_mode_start:
    ;load null into all the data segment registers.
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_main
    
    hlt
