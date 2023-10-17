global start
extern long_mode_start

section .text
bits 32
start:
	mov esp, stack_top

	call check_multiboot
	call check_cpuid
	call check_long_mode

	; from above we have check if long mode is supported or not. But it still needs some work. we need to set up paging.
	; for this we will need to implement virtual memory so that we can enter 64 bit mode.
	; we will now create paging table which will help us ma virtual memory address to real address.
	; a single page is 4kb of memory.

	call setup_page_tables ; we will now go to .bss section to secure memory or create page tables.
	call enable_paging

	lgdt [gdt64.pointer]
	jmp gdt64.code_segment: long_mode_start

	hlt

check_multiboot:
	cmp eax, 0x36d76289
	jne .no_multiboot
	ret
.no_multiboot:
	mov al, "M"
	jmp error

check_cpuid:
	pushfd
	pop eax
	mov ecx, eax
	xor eax, 1 << 21
	push eax
	popfd
	pushfd
	pop eax
	push ecx
	popfd
	cmp eax, ecx
	je .no_cpuid
	ret
.no_cpuid:
	mov al, "C"
	jmp error

check_long_mode:
	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001
	jb .no_long_mode
	
	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29
	jz .no_long_mode
	ret
.no_long_mode:
	mov al, "L"
	jmp error
setup_page_tables:
	mov eax, page_table_l3
	;now math part. log2 of 4096 gives us 12. Thus, the first 6bits i.e. 12 characters should be 0. And the cpu uses these bit sto store flags instead.
	or eax, 0b11 ; present , writable.
	mov [page_table_l4], eax

	mov eax, page_table_l2
	or eax, 0b11 ; present , writable.
	mov [page_table_l3], eax

	; now we will fill in all 512 entries of level 2 table. and each entries is 2 mb each. which will give us 1 gb of entry map
	mov ecx, 0 ; counter
.loop:

	mov eax, 0x200000 ; 2Mib
	mul ecx
	or eax, 0b10000011 ; present, writable, and a huge page.
	mov [page_table_l2 + ecx * 8], eax

	inc ecx ; increment counter
	cmp ecx, 512 ; checks if the whole thing is mapped.
	jne .loop
	ret

enable_paging:
	; PASS page table address to the cpu.
	mov eax, page_table_l4
	mov cr3, eax

	; enable PAE
	mov eax, cr4
	or eax, 1 << 5
	mov cr4, eax

	; now we finally enable long mode.
	mov ecx, 0xC0000080
	rdmsr ; read model specific register instructions. this will load the value of the efer register into the eax register.
	or eax, 1 << 8
	wrmsr

	; enable paging now.
	mov eax, cr0
	or eax, 1 << 31
	mov cr0, eax

	ret
	; Now even though we have managed to enter the long mode, we are still in 32 bit compatible mode.
	; Thus we will create a Global descriptor table. It doesn't server any neccessary purpose as we are utilizing paging, still we need it.

error:
	; print "ERR: X" where X is the error code.
	mov dword [0xb8000], 0x4f524f45
	mov dword [0xb8004], 0x4f3a4f52
	mov dword [0xb8008], 0x4f204f20
	mov byte [0xb800a], al
	hlt

section .bss
align 4096
page_table_l4:
	resb 4096
page_table_l3:
	resb 4096
page_table_l2:
	resb 4096
stack_bottom:
	resb 4096 * 4
stack_top:

section .rodata
gdt64:
	dq 0 ; zero entry
.code_segment: equ $ - gdt64
	dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; code segment
.pointer:
	dw $ - gdt64 - 1
	dq gdt64

