; kernel/idt_load.asm — NASM syntax
; Simple stub for IDT load (compiles with nasm -f elf32)

BITS 32
section .text
global load_idt_stub

; Dummy function — does nothing, just returns
load_idt_stub:
    ret
