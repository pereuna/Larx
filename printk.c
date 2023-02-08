#include <larxarg.h>
#include <larxdef.h>
#include <larxstring.h>

static char buf[1024];
static int cursor = 0xb8000;

extern int vsprintf(char *buf, const char *fmt, va_list args, char char_param);

int printk(char char_param, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args, char_param);
    va_end(args);

    __asm__ __volatile__("cld;rep;movsl\n\t"::"D"(cursor), "S"(buf),
			 "c"(i));

    if (cursor < 0xb8000 + 2000)
	cursor += 160;
    else
	memcpy((char *) 0xb8000, (char *) 0xb8000 + 160, 2000);
    return i;
}
