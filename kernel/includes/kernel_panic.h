/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel_panic.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 16:08:04 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/23 16:08:04 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_PANIC_H
# define KERNEL_PANIC_H

// kernel_panic never returns: it prints the message and halts the CPU
// for good. Only call it for conditions with no recovery path (a CPU
// exception we can't resume from, corrupted internal state...).
// For anything recoverable (e.g. an allocation failure), just log with
// kprintk(KERN_ERR/KERN_WARNING, ...) and let the caller decide what to
// do instead: not every error is a kernel_panic.
void	kernel_panic(const char *fmt, ...);

#endif
