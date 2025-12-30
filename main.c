#include "larx.h"
#define ST 1023
extern char scancode;
extern long jiffies;

long stack[3][1024];

struct {
    long *a;
    short b;
} stack_start = {&stack[0][1023], 0x10};

unsigned long task_esp[3];
unsigned long current=0;
extern unsigned long last_stack;
extern unsigned long next_stack;
extern void setup_PIC(void);
extern void setup_timer(void);
extern int printk(char char_param, const char *fmt, ...);

void task1(void);
void task2(void);
void scedule(void);
void add_task(long eip, int n);

void start_main(void)
{

    setup_PIC();
    setup_timer();
    printk(4, "[ Larx Operating System %c ]", 0x1);
    add_task((long) &task1, 1);
    add_task((long) &task2, 2);
    __asm__("sti");
    while (1) {
    };
}
void scedule(void)
{
    task_esp[current] = last_stack;
    
    if(!(jiffies % 1000)){
    	current = 1;
	next_stack = task_esp[1];
    }else{
	current = 2;
	next_stack = task_esp[2];
    }
}
void add_task(long eip, int n)
{
    stack[n][ST - 1] = 0x202L;	/* EFLAGS (ints enabled) */
    stack[n][ST - 2] = 0x8;	/* CS */
    stack[n][ST - 3] = eip;	/* EIP */
    stack[n][ST - 4] = 0x10;	/* DS */
    stack[n][ST - 5] = 0x10;	/* ES */
    stack[n][ST - 6] = 0x10;	/* FS */
    stack[n][ST - 7] = 0x10;	/* GS */
    stack[n][ST - 8] = 0x0;	/* EAX */
    stack[n][ST - 9] = 0x0;	/* ECX */
    stack[n][ST - 10] = 0x0;	/* EDX */
    stack[n][ST - 11] = 0x0;	/* EBX */
    stack[n][ST - 12] = (long) &stack[n][ST-3];	/* ESP */
    						/* others uninitialized */
    task_esp[n] = (long) &stack[n][ST-3];
}

void task1(void)
{
#ifdef O
    unsigned char a=0;
    unsigned short *x;
    long start = 0xb8000+640+160;
    x = (unsigned short *)start;
    while (1){
	    (long)*x = 0x0900 | a;
	    if((int)x < (start + 0x160*5))
	    	x++;
	    else x = (unsigned short *)start;
	    a++;
    }
#endif
    printk(4, "task1:%x\n", task1);
    while (1)
	__asm__("incl 0xb8000+640");
}

void task2(void)
{
    while (1)
	__asm__("incl 0xb8000+640+160");
}

// Keyboard handler written by ChatGPT
void kbd(void) {
    static unsigned short *x = (unsigned short *)(0xb8000 + 640 + 160 + 160);

    unsigned char sc = (unsigned char)scancode;

    /* US keyboard set 1, ilman shiftiä (minimi) */
    static const unsigned char us_noshift[128] = {
        0,  27, '1','2','3','4','5','6','7','8','9','0','+', 0, '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l','o','a','\'', 0, '\\',
        'z','x','c','v','b','n','m',',','.','-', 0,  '*', 0,  ' ',
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    if (sc < 128) {
        unsigned char ch = us_noshift[sc];
        if (ch) {
            *x = 0x0700 | ch;     /* attribute 0x07, ASCII char */
            x++;
        }
    }
}

// My old keyboard handler
/*	
void kbd(void){
#define START 0xb8000+640+160+160 
    static unsigned short *x = (unsigned short *)START;
    
    //(long)*x = 0x0900 | scancode;
    *x = 0x0900 | scancode;
    if((int)x < (START + 160*10))
	x++;
    else x = (unsigned short *)START;
}
*/
