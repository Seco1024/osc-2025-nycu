.global switch_to
switch_to:
    stp x19, x20, [x0, 16 * 0]
    stp x21, x22, [x0, 16 * 1]
    stp x23, x24, [x0, 16 * 2]
    stp x25, x26, [x0, 16 * 3]
    stp x27, x28, [x0, 16 * 4]
    stp fp, lr, [x0, 16 * 5]
    mov x9, sp
    str x9, [x0, 16 * 6]

    ldp x19, x20, [x1, 16 * 0]
    ldp x21, x22, [x1, 16 * 1]
    ldp x23, x24, [x1, 16 * 2]
    ldp x25, x26, [x1, 16 * 3]
    ldp x27, x28, [x1, 16 * 4]
    ldp fp, lr, [x1, 16 * 5]
    ldp x9, x10, [x1, 16*6]
    mov sp, x9
    msr tpidr_el1, x1

    dsb ish
    msr ttbr0_el1, x10
    tlbi vmalle1is
    dsb ish
    isb

    ret

.global get_current
get_current:
    mrs x0, tpidr_el1
    ret

.global restore_context
restore_context:
  ldp x19, x20, [x0, 16*0]
  ldp x21, x22, [x0, 16*1]
  ldp x23, x24, [x0, 16*2]
  ldp x25, x26, [x0, 16*3]
  ldp x27, x28, [x0, 16*4]
  ldp fp, lr, [x0, 16*5]
  ldr x9, [x0, 16*6]
  mov sp, x9
  ret