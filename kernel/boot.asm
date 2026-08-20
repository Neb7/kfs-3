BITS 32

section .multiboot
align 4

dd 0x1BADB002
dd 0
dd -(0x1BADB002)

section .bss
align 16
stack_bottom:
    resb 16384
global stack_top
stack_top:

section .text

global start
global keyboard_stub
extern kernel_main
extern keyboard_handler

start:
    cli
    mov esp, stack_top
	xor ebp, ebp      ; EBP = 0
	push ebp          ; empile 0 sur la stack
	mov ebp, esp      ; EBP pointe sur ce 0

    call kernel_main

.loop:
    hlt
    jmp .loop

keyboard_stub:
    pusha              ; save eax, ecx, edx, ebx, esp, ebp, esi, edi
    call keyboard_handler
    popa               ; restore registries
    iret               ; return of interrupt (not ret !)