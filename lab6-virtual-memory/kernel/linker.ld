SECTIONS
{
  . = 0xffff000000000000;
  . += 0x80000;
  .text.boot : { KEEP(*(.text.boot)) }
  .text : { *(.text) }

  .rodata : {*(.rodata)}
  .data : {*(.data)}

  . = ALIGN(0x8);
  __bss_begin = .;
  .bss (NOLOAD) : { *(.bss) }
  __bss_end = .;
}

__bss_size = (__bss_end - __bss_begin) >> 3;
