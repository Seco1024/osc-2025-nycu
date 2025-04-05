SECTIONS
{
  . = 0x60000;
  __loader_start = .;
  .text.boot : { KEEP(*(.text.boot)) }
  .text : { *(.text) }

  .rodata : {*(.rodata)}
  .data : {*(.data)}

  . = ALIGN(0x8);
  __bss_begin = .;
  .bss (NOLOAD) : { *(.bss) }
  __bss_end = .;
  __loader_end = .;
}

__bss_size = (__bss_end - __bss_begin) >> 3;
__loader_size = (__loader_end - __loader_start) >> 3;