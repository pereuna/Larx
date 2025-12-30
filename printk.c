// Simple VGA text mode printk implementation
// Copyright (c) 2018-2024 J. M. Spivey		
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by	
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// ChatGPT: Modified to add char_param to printk for attribute control 

#include <larxarg.h>
#include <larxdef.h>
#include <larxstring.h>

static char buf[1024];

/* VGA text mode memory */
#define VGA_BASE   0xB8000
#define VGA_COLS   80
#define VGA_ROWS   25
#define VGA_CELL   2
#define VGA_LINE_BYTES (VGA_COLS * VGA_CELL)
#define VGA_SIZE_BYTES (VGA_COLS * VGA_ROWS * VGA_CELL)

static unsigned char *cursor = (unsigned char *)VGA_BASE;

extern int vsprintf(char *buf, const char *fmt, va_list args, char char_param);

static void vga_scroll(char char_param)
{
    /* move lines 1..24 to 0..23 */
    memmove((void *)VGA_BASE,
            (void *)(VGA_BASE + VGA_LINE_BYTES),
            VGA_SIZE_BYTES - VGA_LINE_BYTES);

    /* clear last line */
    {
        unsigned char *p = (unsigned char *)(VGA_BASE + VGA_SIZE_BYTES - VGA_LINE_BYTES);
        int i;
        for (i = 0; i < VGA_COLS; ++i) {
            *p++ = ' ';
            *p++ = (unsigned char)char_param;
        }
    }

    cursor = (unsigned char *)(VGA_BASE + VGA_SIZE_BYTES - VGA_LINE_BYTES);
}

int printk(char char_param, const char *fmt, ...)
{
    va_list args;
    int bytes;

    va_start(args, fmt);
    bytes = vsprintf(buf, fmt, args, char_param);  /* BYTES (char,attr) pairs */
    va_end(args);

    /* copy BYTES to VGA using byte moves */
    __asm__ __volatile__("cld; rep movsb"
                         :
                         : "D"(cursor), "S"(buf), "c"(bytes)
                         : "memory");

    /* original semantics: advance one line per printk() */
    if (cursor < (unsigned char *)(VGA_BASE + VGA_SIZE_BYTES - VGA_LINE_BYTES)) {
        cursor += VGA_LINE_BYTES;
    } else {
        vga_scroll(char_param);
    }

    return bytes;
}

