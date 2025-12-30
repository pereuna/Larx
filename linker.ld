OUTPUT_FORMAT(elf32-i386)
OUTPUT_ARCH(i386)
ENTRY(start32)

SECTIONS
{
  /* Must match boot_legacy.s load address: ES=0x200 -> physical 0x2000 */
  . = 0x2000;

  /* Ensure start32 comes first and stays at 0x2000 */
  .text : ALIGN(16)
  {
    *(.text .text.*)
  }

  .rodata : ALIGN(16)
  {
    *(.rodata .rodata.*)
  }

  .data : ALIGN(16)
  {
    *(.data .data.*)
  }

  .bss : ALIGN(16)
  {
    *(COMMON)
    *(.bss .bss.*)
  }

  /DISCARD/ :
  {
    *(.comment)
    *(.note*)
    *(.eh_frame*)
    *(.debug*)
  }
}

