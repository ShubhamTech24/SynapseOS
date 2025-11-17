; boot/boot.asm — Fully working Multiboot2 entry for SynapseOS
; Assemble with:
;   nasm -f elf32 boot/boot.asm -o build/boot.o

BITS 32

; ------------------------------------------------------------
; Multiboot2 Header
; ------------------------------------------------------------
section .multiboot_header
align 8
header_start:
    dd 0xE85250D6              ; magic number for multiboot2
    dd 0                       ; architecture (0 = i386)
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))  ; checksum

; --- Framebuffer Tag (request 1024x768x32) ------------------
    align 8
    dw 5                       ; type = framebuffer
    dw 0                       ; flags = 0
    dd 20                      ; size of this tag
    dd 1024                    ; framebuffer width
    dd 768                     ; framebuffer height
    dd 32                      ; bits per pixel

; --- End Tag ------------------------------------------------
    align 8
    dw 0                       ; type = end
    dw 0
    dd 8                       ; size = 8 bytes
header_end:

; ------------------------------------------------------------
; Kernel Entry Point
; ------------------------------------------------------------
section .text
global _start
global multiboot2_magic
global multiboot2_info
extern kernel_main

_start:
    cli

    ; Save GRUB-passed registers for kernel
    mov [multiboot2_magic], eax
    mov [multiboot2_info], ebx

    ; Call kernel_main()
    call kernel_main

.hang:
    hlt
    jmp .hang

; ------------------------------------------------------------
; Global variables for Multiboot2 handoff
; ------------------------------------------------------------
section .bss
align 4
multiboot2_magic:   resd 1
multiboot2_info:    resd 1
