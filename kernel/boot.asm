BITS 32

section .multiboot
align 4

; ----------
; Multiboot header: magic, flags (none requested), checksum
; (magic + flags + checksum must sum to 0 so GRUB accepts this as Multiboot)
dd 0x1BADB002
dd 0
dd -(0x1BADB002)
; ----------

section .bss
align 16
stack_bottom:
    resb 16384
global stack_top
stack_top:

section .text

global start
global keyboard_stub
global page_fault_stub
extern kernel_main
extern keyboard_handler
extern page_fault_handler

start:
    cli
    mov esp, stack_top
	xor ebp, ebp		; EBP = 0
	push ebp			; push 0 onto the stack
	mov ebp, esp		; EBP points to this 0

	push ebx			; GRUB left EAX = multiboot magic, EBX = multiboot_info* : pass both
	push eax			; to kernel_main (cdecl: pushed right-to-left, so EBX first, EAX last)
    call kernel_main

.loop:
    hlt
    jmp .loop

keyboard_stub:
    pusha              ; save eax, ecx, edx, ebx, esp, ebp, esi, edi
    call keyboard_handler
    popa               ; restore registries
    iret               ; return of interrupt (not ret !)

page_fault_stub:
    pusha                  ; save registers (pushes 8 * 4 = 32 bytes)
    mov eax, [esp + 32]    ; CPU-pushed error code, sits above the pusha'd regs
    push eax               ; pass it as page_fault_handler's argument
    call page_fault_handler
    add esp, 4              ; pop the argument (cdecl caller cleanup)
    popa                    ; unreachable in practice: handler halts the CPU
    add esp, 4              ; would discard the CPU-pushed error code
    iret                    ; would return to the faulting instruction