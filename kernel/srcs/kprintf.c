#include "kernel.h"
#include <stdarg.h>

static int	kputchar_k(char c)
{
	putchar(c);
	return (1);
}

static int	kputstr(char *s)
{
	int	i;

	i = 0;
	if (!s)
	{
		kputchar_k('(');
		kputchar_k('n');
		kputchar_k('u');
		kputchar_k('l');
		kputchar_k('l');
		kputchar_k(')');
		return (6);
	}
	while (s[i])
		kputchar_k(s[i++]);
	return (i);
}

static int	kputnbr(int n)
{
	unsigned int	u;
	char			buf[11];
	int				i;
	int				result;

	result = 0;
	if (n == 0)
		return (kputchar_k('0'));
	if (n < 0)
	{
		kputchar_k('-');
		result++;
		u = (unsigned int)(-(n + 1)) + 1U;
	}
	else
		u = (unsigned int)n;
	i = 0;
	while (u > 0)
	{
		buf[i++] = '0' + (u % 10);
		u /= 10;
	}
	while (--i >= 0)
		result += kputchar_k(buf[i]);
	return (result);
}

static int	kputunbr(unsigned int n)
{
	char			buf[11];
	int				i;
	int				result;

	result = 0;
	if (n == 0)
		return (kputchar_k('0'));
	i = 0;
	while (n > 0)
	{
		buf[i++] = '0' + (n % 10);
		n /= 10;
	}
	while (--i >= 0)
		result += kputchar_k(buf[i]);
	return (result);
}

static char	hex_digit(unsigned long n, int pos, int upper)
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

static int	kputhex(unsigned int n, int upper)
{
	int		digit;
	int		result;
	int		started;

	digit = 8;
	result = 0;
	started = 0;
	while (digit >= 0)
	{
		char c = hex_digit(n, digit--, upper);
		if (c != '0' || started || digit < 0)
		{
			kputchar_k(c);
			started = 1;
			result++;
		}
	}
	return (result);
}

static int	kputptr(void *ptr)
{
	unsigned long	addr;
	int				digit;
	int				result;
	int				started;

	if (!ptr)
	{
		kputchar_k('0');
		kputchar_k('x');
		kputchar_k('0');
		return (3);
	}
	addr = (unsigned long)ptr;
	kputchar_k('0');
	kputchar_k('x');
	result = 2;
	digit = 7;
	started = 0;
	while (digit >= 0)
	{
		char c = hex_digit(addr, digit--, 0);
		if (c != '0' || started || digit < 0)
		{
			kputchar_k(c);
			started = 1;
			result++;
		}
	}
	return (result);
}

static int	kputarg(char c, va_list args)
{
	if (c == 'c')
		return (kputchar_k(va_arg(args, int)));
	if (c == 's')
		return (kputstr(va_arg(args, char *)));
	if (c == 'p')
		return (kputptr(va_arg(args, void *)));
	if (c == 'd' || c == 'i')
		return (kputnbr(va_arg(args, int)));
	if (c == 'u')
		return (kputunbr(va_arg(args, unsigned int)));
	if (c == 'x')
		return (kputhex(va_arg(args, unsigned int), 0));
	if (c == 'X')
		return (kputhex(va_arg(args, unsigned int), 1));
	return (kputchar_k('%'));
}

int	vkprintf(const char *s, va_list args)
{
	int	cursor;
	int	result;
	int	buffer;

	cursor = 0;
	result = 0;
	while (*(s + cursor))
	{
		if (*(s + cursor) == '%' && *(s + cursor + 1) != '%')
			buffer = kputarg(*(s + cursor++ + 1), args);
		else if (*(s + cursor) == '%')
			buffer = kputchar_k(*(s + cursor++));
		else
			buffer = kputchar_k(*(s + cursor));
		result += buffer;
		cursor++;
	}
	return (result);
}

int	kprintf(const char *s, ...)
{
	va_list	args;
	int		result;

	va_start(args, s);
	result = vkprintf(s, args);
	va_end(args);
	return (result);
}
