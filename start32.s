#----------------------------------------------------------------------------
# suojatun tilan kernel
#----------------------------------------------------------------------------

.global start32, scancode, jiffies, unknown_int, gdt, next_stack, last_stack
.text
start32:
        movl $0x10, %eax
        movl %eax, %ds
        movl %eax, %es
        movl %eax,%fs
        movl %eax,%gs
        
	lss stack_start,%esp
	lidt idt_descr
        lgdt gdt_descr
	
	movl $0x10,%eax         # reload all the segment registers
        mov %ax,%ds             # after changing gdt. CS was already
        mov %ax,%es             # reloaded in 'setupgdt'
        mov %ax,%fs
        mov %ax,%gs
	lss stack_start,%esp
        pushl $start_main
	ret
lo:
	jmp lo
.align 2
.word 0
idt_descr:
	.word 256*8-1           # idt contains 256 entries
	.long idt
.align 2
.word 0
gdt_descr:
	.word 256*8-1           # so does gdt (not that that's any
	.long gdt               # magic number, but it works for me :^)
.align


idt:  
        .word ignore_int,0x0008,0x8e00,0x0000 	/* 0x00 div by zero */ 
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x01 */
        .word ignore_int,0x0008,0x8e00,0x0000 	/* 0x02 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x03 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x04 */
        .word ignore_int,0x0008,0x8e00,0x0000 	/* 0x05 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x06 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x07 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x08 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x09 */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0a */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0b */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0c */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0d */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0e */
        .word ignore_int,0x0008,0x8e00,0x0000   /* 0x0f */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x10 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x11 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x12 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x13 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x14 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x15 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x16 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x17 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x18 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x19 */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1a */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1b */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1c */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1d */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1e */
        .word ignore_int,0x0008,0x8e00,0x0000	/* 0x1f */
        .word timer_int ,0x0008,0x8e00,0x0000 	/* 0x20 timer    */
        .word kbd_int	,0x0008,0x8e00,0x0000 	/* 0x21 keyboard */
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
        .word ignore_int,0x0008,0x8e00,0x0000
       	.align

gdt:    .quad 0x0000000000000000        /* NULL descriptor */
	.quad 0x00c09a00000007ff        /* 8Mb */
	.quad 0x00c09200000007ff        /* 8Mb */
	.fill 253,8,0

.align
unknown_int:	.long 0x0
ignore_int:
        incl 0xb8000
	incl unknown_int
	iret           
.align
div_z:	.long 0x0
div_by_zero:
	incl div_z
	iret
.align
scancode: .byte 0x0
kbd_int:
   pushal

   movw	 $0x60, %dx
   inb    %dx,   %al
   movb	 %al,   scancode
   testb  $0x80, %al        # bit7=1 => key release (break)
   jnz    .eoi_and_out      # älä tulosta release-koodeja
   call    kbd
.eoi_and_out:
   movb    $0x20, %al
   outb    %al, $0x20        # EOI master PIC
   popal
   iret
.align
jiffies: .long 0x0742
last_stack: .long 0x0
next_stack: .long 0x0
timer_int:
	movl $0x10, %eax
        movl %eax, %ds
        movl %eax, %es

	movl %esp, %eax
	movl %esp, last_stack

        movb $0x20,%al	/* EOI to interrupt controller */
        outb %al, $0x20
        incl jiffies
	incl 0xb8000+160-4
	call scedule
	
	movl next_stack, %eax
	movl %eax, %esp

	iret
			
