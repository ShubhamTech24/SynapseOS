ENTRY(_start)

OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)

SECTIONS
{
  . = 0x00100000; /* link at 1MB */

  .multiboot_header ALIGN(8) : {
    KEEP(*(.multiboot_header))
  }

  .text ALIGN(16) : {
    *(.text*)
    *(.rodata*)
  }

  .data ALIGN(16) : {
    *(.data*)
  }

  .bss ALIGN(16) : {
    *(COMMON)
    *(.bss*)
  }

  /DISCARD/ : { *(.note.GNU-stack) }
}

