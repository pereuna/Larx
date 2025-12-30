/* vsprintf.c -- Lars Wirzenius & Linus Torvalds. */
/*
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 * ChatGPT 5.2 fixed it again.
 */

#include <larxarg.h>
#include <larxstring.h>

/* we use this so that we can do without the ctype library */
#define is_digit(c) ((c) >= '0' && (c) <= '9')
void __stack_chk_fail(void) {}

static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD  1   /* pad with zero */
#define SIGN     2   /* signed conversion */
#define PLUS     4   /* show plus */
#define SPACE    8   /* space if plus */
#define LEFT     16  /* left justified */
#define SPECIAL  32  /* 0x / 0 */
#define SMALL    64  /* use 'abcdef' instead of 'ABCDEF' */

/* n must be an unsigned 32-bit quantity for divl */
#define do_div(n, base) ({                                \
    unsigned long __n = (n);                               \
    unsigned long __base = (base);                         \
    unsigned long __rem;                                   \
    __asm__("divl %4"                                      \
            : "=a" (__n), "=d" (__rem)                     \
            : "0" (__n), "1" (0UL), "r" (__base));         \
    (n) = __n;                                             \
    (int)__rem;                                            \
})

static char *number(char *str,
                    unsigned long num,
                    int base,
                    int size,
                    int precision,
                    int type,
                    char char_param,
                    char sign_char) /* 0 if none, otherwise '+', ' ', '-' */
{
    char c, tmp[36];
    const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;

    if (type & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (type & LEFT)
        type &= ~ZEROPAD;

    if (base < 2 || base > 36)
        return 0;

    c = (type & ZEROPAD) ? '0' : ' ';

    if (sign_char)
        size--;

    if (type & SPECIAL) {
        if (base == 16)
            size -= 2; /* 0x */
        else if (base == 8)
            size -= 1; /* 0 */
    }

    i = 0;
    if (num == 0) {
        tmp[i++] = '0';
    } else {
        while (num != 0) {
            int rem = do_div(num, (unsigned long)base);
            tmp[i++] = digits[rem];
        }
    }

    if (i > precision)
        precision = i;

    size -= precision;

    /* leading spaces (if any) */
    if (!(type & (ZEROPAD | LEFT))) {
        while (size-- > 0) {
            *str++ = ' ';
            *str++ = char_param;
        }
    }

    /* sign */
    if (sign_char) {
        *str++ = sign_char;
        *str++ = char_param;
    }

    /* prefix */
    if (type & SPECIAL) {
        if (base == 8) {
            *str++ = '0';
            *str++ = char_param;
        } else if (base == 16) {
            *str++ = '0';
            *str++ = char_param;
            *str++ = (type & SMALL) ? 'x' : 'X';
            *str++ = char_param;
        }
    }

    /* zero padding (if any) */
    if (!(type & LEFT)) {
        while (size-- > 0) {
            *str++ = c;
            *str++ = char_param;
        }
    }

    /* precision zeros */
    while (i < precision--) {
        *str++ = '0';
        *str++ = char_param;
    }

    /* digits */
    while (i-- > 0) {
        *str++ = tmp[i];
        *str++ = char_param;
    }

    /* trailing spaces */
    while (size-- > 0) {
        *str++ = ' ';
        *str++ = char_param;
    }

    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args, char char_param)
{
    int len, i;
    char *str;
    char *s;
    int *ip;

    int flags;          /* flags to number() */
    int field_width;    /* width of output field */
    int precision;      /* min # of digits for integers; max chars for strings */
    int qualifier;      /* 'h', 'l', or 'L' for integer fields */

    str = buf;

    for (; *fmt; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            *str++ = char_param;
            continue;
        }

        /* process flags */
        flags = 0;
repeat:
        ++fmt; /* skip '%' or next flag */
        switch (*fmt) {
        case '-': flags |= LEFT;    goto repeat;
        case '+': flags |= PLUS;    goto repeat;
        case ' ': flags |= SPACE;   goto repeat;
        case '#': flags |= SPECIAL; goto repeat;
        case '0': flags |= ZEROPAD; goto repeat;
        default: break;
        }

        /* get field width */
        field_width = -1;
        if (is_digit(*fmt)) {
            field_width = skip_atoi(&fmt);
        } else if (*fmt == '*') {
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
            ++fmt;
        }

        /* get precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;
            if (is_digit(*fmt)) {
                precision = skip_atoi(&fmt);
            } else if (*fmt == '*') {
                precision = va_arg(args, int);
                ++fmt;
            }
            if (precision < 0)
                precision = 0;
        }

        /* get conversion qualifier */
        qualifier = 0;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        switch (*fmt) {
        case 'c':
            if (!(flags & LEFT)) {
                while (--field_width > 0) {
                    *str++ = ' ';
                    *str++ = char_param;
                }
            }
            *str++ = (unsigned char)va_arg(args, int);
            *str++ = char_param;
            while (--field_width > 0) {
                *str++ = ' ';
                *str++ = char_param;
            }
            break;

        case 's':
            s = va_arg(args, char *);
            if (!s)
                s = "(null)";
            len = strlen(s);
            if (precision < 0)
                precision = len;
            else if (len > precision)
                len = precision;

            if (!(flags & LEFT)) {
                while (len < field_width--) {
                    *str++ = ' ';
                    *str++ = char_param;
                }
            }
            for (i = 0; i < len; ++i) {
                *str++ = *s++;
                *str++ = char_param;
            }
            while (len < field_width--) {
                *str++ = ' ';
                *str++ = char_param;
            }
            break;

        case 'o': {
            unsigned long v = (qualifier == 'l') ? va_arg(args, unsigned long)
                                                 : (unsigned long)va_arg(args, unsigned int);
            str = number(str, v, 8, field_width, precision, flags, char_param, 0);
            break;
        }

        case 'p': {
            unsigned long v = (unsigned long)va_arg(args, void *);
            if (field_width == -1) {
                field_width = 8;
                flags |= ZEROPAD;
            }
            str = number(str, v, 16, field_width, precision, flags, char_param, 0);
            break;
        }

        case 'x':
            flags |= SMALL;
            /* fall through */
        case 'X': {
            unsigned long v = (qualifier == 'l') ? va_arg(args, unsigned long)
                                                 : (unsigned long)va_arg(args, unsigned int);
            str = number(str, v, 16, field_width, precision, flags, char_param, 0);
            break;
        }

        case 'd':
        case 'i': {
            long sv = (qualifier == 'l') ? va_arg(args, long)
                                         : (long)va_arg(args, int);
            unsigned long mag;
            char sign_char = 0;

            flags |= SIGN;

            if (sv < 0) {
                sign_char = '-';
                /* avoid UB on LONG_MIN */
                mag = (unsigned long)(-(sv + 1)) + 1;
            } else {
                if (flags & PLUS) sign_char = '+';
                else if (flags & SPACE) sign_char = ' ';
                mag = (unsigned long)sv;
            }

            /* PLUS/SPACE handled via sign_char; clear to avoid double-sign logic */
            flags &= ~(PLUS | SPACE);

            str = number(str, mag, 10, field_width, precision, flags, char_param, sign_char);
            break;
        }

        case 'u': {
            unsigned long v = (qualifier == 'l') ? va_arg(args, unsigned long)
                                                 : (unsigned long)va_arg(args, unsigned int);
            str = number(str, v, 10, field_width, precision, flags, char_param, 0);
            break;
        }

        case 'n':
            ip = va_arg(args, int *);
            /* bytes written so far */
            *ip = (int)(str - buf);
            break;

        case '%':
            *str++ = '%';
            *str++ = char_param;
            break;

        default:
            /* unknown specifier: print it literally */
            *str++ = '%';
            *str++ = char_param;
            if (*fmt) {
                *str++ = *fmt;
                *str++ = char_param;
            } else {
                --fmt;
            }
            break;
        }
    }

    /* Return BYTES written. We do not include terminator in the count. */
    i = (int)(str - buf);

    /* Optional terminator pair (does not affect printk(), which copies by returned length) */
    *str++ = '\0';
    *str++ = char_param;

    return i;
}

