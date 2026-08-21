/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmalloc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 08:05:13 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 10:44:28 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KMALLOC_H
# define KMALLOC_H

# include "../kernel.h"

extern uint32_t	g_total_ram; 
extern char 	kernel_end; // address of the end of the kernel

#endif