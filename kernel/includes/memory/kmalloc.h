/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmalloc.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 08:05:13 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/23 15:53:35 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KMALLOC_H
# define KMALLOC_H

# include "kernel.h"
# include "paging.h"


# include "frame_allocator.h"

// Fixed virtual start of the kernel heap: right after the 4MB identity
// map built at boot (paging_build_identity_map), so kbrk never collides
// with kernel code/data/bss or the frame bitmap.
# define KHEAP_START	0x00400000
# define VHEAP_START	0x01000000

# define BLOCK_ALIGN		4
# define MIN_SPLIT_SIZE	(sizeof(t_block_header) + BLOCK_ALIGN)

typedef struct s_block_header
{
	uint32_t				size;	// usable size, header excluded
	uint8_t					free;
	struct s_block_header	*next;
}	t_block_header;

typedef struct s_heap_state
{
	uint32_t		heap_break;
	uint32_t		mapped_end;
	t_block_header	*head;
	t_block_header	*tail;
	uint32_t         page_flags;
}	t_heap_state;

// 4GB / PAGE_SIZE, expressed directly in frames (not bytes: 4GB itself
// overflows uint32_t by 1, so it can never be represented in bytes here).
# define MAX_FRAMES_SUPPORTED	0x100000
# define MAX_BITMAP_SIZE	(MAX_FRAMES_SUPPORTED / 8)

extern uint32_t	g_total_ram;
extern uint32_t	g_total_frames;
extern uint8_t	g_frame_bitmap[MAX_BITMAP_SIZE];
extern char 	kernel_end; // address of the end of the kernel

# define KBRK_FAILED	((uint32_t)-1)

uint32_t	kbrk(int32_t increment);
void		*kmalloc(uint32_t size);
void		kfree(void *ptr);
uint32_t	ksize(void *ptr);

uint32_t	vbrk(int32_t increment);
void		*vmalloc(uint32_t size);
void		vfree(void *ptr);
uint32_t	vsize(void *ptr);

void		kheap_dump(void);
void		vheap_dump(void);

#endif