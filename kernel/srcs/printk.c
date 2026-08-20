/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printk.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:54:42 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/07/07 13:54:44 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "vga.h"
#include "kprintk.h"
#include "dmesg.h"
#include <stdarg.h>

#define KPRINTK_BUF 256

/* ── vsnprintf engine ────────────────────────────────────────────────────── */

typedef struct s_sbuf
{
    char    *buf;
    int     size;
    int     pos;
}   t_sbuf;

static void sb_putchar(t_sbuf *sb, char c)
{
    if (sb->pos < sb->size - 1)
        sb->buf[sb->pos] = c;
    sb->pos++;
}

static void sb_putstr(t_sbuf *sb, char *s)
{
    if (!s)
    {
        sb_putchar(sb, '(');
        sb_putchar(sb, 'n');
        sb_putchar(sb, 'u');
        sb_putchar(sb, 'l');
        sb_putchar(sb, 'l');
        sb_putchar(sb, ')');
        return ;
    }
    while (*s)
        sb_putchar(sb, *s++);
}

static void sb_putnbr(t_sbuf *sb, int n)
{
    unsigned int    u;
    char            tmp[11];
    int             i;

    if (n == 0)
    {
        sb_putchar(sb, '0');
        return ;
    }
    if (n < 0)
    {
        sb_putchar(sb, '-');
        u = (unsigned int)(-(n + 1)) + 1U;
    }
    else
        u = (unsigned int)n;
    i = 0;
    while (u > 0)
    {
        tmp[i++] = '0' + (u % 10);
        u /= 10;
    }
    while (--i >= 0)
        sb_putchar(sb, tmp[i]);
}

static void sb_putunbr(t_sbuf *sb, unsigned int n)
{
    char    tmp[11];
    int     i;

    if (n == 0)
    {
        sb_putchar(sb, '0');
        return ;
    }
    i = 0;
    while (n > 0)
    {
        tmp[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (--i >= 0)
        sb_putchar(sb, tmp[i]);
}

static char hex_digit(unsigned long n, int pos, int upper)
{
    while (pos)
    {
        n = n / 16;
        pos--;
    }
    n = n % 16;
    if (n <= 9)
        return (n + '0');
    if (upper)
        return (n - 10 + 'A');
    return (n - 10 + 'a');
}

static void sb_puthex(t_sbuf *sb, unsigned int n, int upper)
{
    int     digit;
    int     started;

    digit = 8;
    started = 0;
    while (digit >= 0)
    {
        char c = hex_digit(n, digit--, upper);
        if (c != '0' || started || digit < 0)
        {
            sb_putchar(sb, c);
            started = 1;
        }
    }
}

static void sb_putptr(t_sbuf *sb, void *ptr)
{
    unsigned long   addr;
    int             digit;
    int             started;

    if (!ptr)
    {
        sb_putchar(sb, '0');
        sb_putchar(sb, 'x');
        sb_putchar(sb, '0');
        return ;
    }
    addr = (unsigned long)ptr;
    sb_putchar(sb, '0');
    sb_putchar(sb, 'x');
    digit = 7;
    started = 0;
    while (digit >= 0)
    {
        char c = hex_digit(addr, digit--, 0);
        if (c != '0' || started || digit < 0)
        {
            sb_putchar(sb, c);
            started = 1;
        }
    }
}

static void sb_putarg(t_sbuf *sb, char c, va_list *args)
{
    if (c == 'c')
        sb_putchar(sb, (char)va_arg(*args, int));
    else if (c == 's')
        sb_putstr(sb, va_arg(*args, char *));
    else if (c == 'p')
        sb_putptr(sb, va_arg(*args, void *));
    else if (c == 'd' || c == 'i')
        sb_putnbr(sb, va_arg(*args, int));
    else if (c == 'u')
        sb_putunbr(sb, va_arg(*args, unsigned int));
    else if (c == 'x')
        sb_puthex(sb, va_arg(*args, unsigned int), 0);
    else if (c == 'X')
        sb_puthex(sb, va_arg(*args, unsigned int), 1);
    else
        sb_putchar(sb, '%');
}

static int  vsnprintf(char *buf, int size, const char *fmt, va_list args)
{
    t_sbuf  sb;
    int     cursor;

    sb.buf  = buf;
    sb.size = size;
    sb.pos  = 0;
    cursor  = 0;
    while (fmt[cursor])
    {
        if (fmt[cursor] == '%' && fmt[cursor + 1] != '%')
            sb_putarg(&sb, fmt[cursor++ + 1], &args);
        else if (fmt[cursor] == '%')
            sb_putchar(&sb, fmt[cursor++]);
        else
            sb_putchar(&sb, fmt[cursor]);
        cursor++;
    }
    if (size > 0)
        buf[sb.pos < size ? sb.pos : size - 1] = '\0';
    return (sb.pos);
}

/* ── kprintk ─────────────────────────────────────────────────────────────── */

static uint8_t g_level_colors[8] = {
    0x4F,  // 0 EMERG   - fond rouge, texte blanc
    0x0C,  // 1 ALERT   - rouge vif
    0x04,  // 2 CRIT    - rouge
    0x0C,  // 3 ERR     - rouge vif
    0x0E,  // 4 WARNING - jaune
    0x0B,  // 5 NOTICE  - cyan vif
    0x0F,  // 6 INFO    - blanc
    0x08,  // 7 DEBUG   - gris sombre
};

static int  parse_level(const char **fmt)
{
    if ((*fmt)[0] == '<'
        && (*fmt)[1] >= '0' && (*fmt)[1] <= '7'
        && (*fmt)[2] == '>')
    {
        int level = (*fmt)[1] - '0';
        *fmt += 3;
        return (level);
    }
    return (-1);
}

int kprintk(const char *fmt, ...)
{
    va_list args;
    char    buf[KPRINTK_BUF];
    int     level;
    uint8_t saved_color;
    int     ret;
    int     i;

    level = parse_level(&fmt);
    va_start(args, fmt);
    ret = vsnprintf(buf, KPRINTK_BUF, fmt, args);
    va_end(args);
    if (level >= 0)
        dmesg_write(buf);
    saved_color = g_screens[g_cur].color;
    if (level >= 0)
        g_screens[g_cur].color = g_level_colors[level];
    i = 0;
    while (buf[i])
        putchar(buf[i++]);
    g_screens[g_cur].color = saved_color;
    return (ret);
}
