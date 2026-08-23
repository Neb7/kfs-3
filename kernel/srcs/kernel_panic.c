/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_panic.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 16:08:04 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/23 16:19:30 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel_panic.h"
#include "kprintk.h"
#include "gdt.h"

# define KPANIC_BUF_SIZE	256

/**
 * @brief	Print a fatal error message and halt the kernel for good.
 * Formats fmt/args into a buffer first so the whole panic message (tag
 * included) prints through a single kprintk call, at KERN_EMERG level
 * (logged to dmesg, shown in the emergency color). Follows with a stack
 * trace for context, then disables interrupts and spins on hlt: this
 * function never returns.
 *
 * @param	fmt printf-style format string describing what went wrong
 */
void	kernel_panic(const char *fmt, ...)
{
	va_list	args;
	char	buf[KPANIC_BUF_SIZE];

	va_start(args, fmt);
	vsnprintf(buf, KPANIC_BUF_SIZE, fmt, args);
	va_end(args);
	kprintk(KERN_EMERG "KERNEL PANIC: %s\n", buf);
	print_stack(10);
	__asm__ volatile ("cli");
	while (1)
		__asm__ volatile ("hlt");
}


