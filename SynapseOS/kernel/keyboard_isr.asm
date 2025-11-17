; kernel/keyboard_isr.asm — NASM syntax
; Minimal stub for keyboard interrupt service routine

BITS 32
section .text
global keyboard_isr

keyboard_isr:
    iret
