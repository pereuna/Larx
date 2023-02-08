#include "larx.h"

//inline void setup_PIC(void)
void setup_PIC(void)
{
    outb(0x11, PORT_8259M);	/* start 8259 initialization */
    outb(0x11, PORT_8259S);
    outb(0x20, PORT_8259M + 1);	/* master base interrupt vector */
    outb(0x28, PORT_8259S + 1);	/* slave base interrupt vector */
    outb(1 << 2, PORT_8259M + 1);	/* bitmask for cascade on IRQ2 */
    outb(2, PORT_8259S + 1);	/* cascade on IRQ2 */
    outb(1, PORT_8259M + 1);	/* finish 8259 initialization */
    outb(1, PORT_8259S + 1);
    outb(0xFC, PORT_8259M + 1);	/* enable timer ans keyboard (master) */
    outb(0xFF, PORT_8259S + 1);	/* disable all (slave) */
}
//inline void setup_timer(void)
void setup_timer(void)
{
    outb(0x36, 0x43);		/* binary, mode 3, LSB/MSB, ch 0 */
    outb(11000 & 0xff, 0x40);	/* LSB */
    outb(11000 >> 8, 0x40);	/* MSB */
    outb(inb(0x21) & ~0x01, 0x21);
}
