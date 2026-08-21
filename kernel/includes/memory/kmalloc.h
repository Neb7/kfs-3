/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmalloc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 08:05:13 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 11:45:44 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KMALLOC_H
# define KMALLOC_H

# include "kernel.h"
# include "paging.h"

// 4GB / PAGE_SIZE, expressed directly in frames (not bytes: 4GB itself
// overflows uint32_t by 1, so it can never be represented in bytes here).
# define MAX_FRAMES_SUPPORTED	0x100000
# define MAX_BITMAP_SIZE	(MAX_FRAMES_SUPPORTED / 8)

extern uint32_t	g_total_ram;
extern uint32_t	g_total_frames;
extern uint8_t	g_frame_bitmap[MAX_BITMAP_SIZE];
extern char 	kernel_end; // address of the end of the kernel

#endif