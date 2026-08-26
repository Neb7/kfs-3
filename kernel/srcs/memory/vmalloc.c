/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vmalloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/23 15:13:25 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/23 15:53:11 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kmalloc.h"
#include "kprintk.h"

static t_heap_state	g_heap_state = {0, 0, NULL, NULL, PAGE_RW | PAGE_USER};

/**
 * @brief	Align n up to the next multiple of align, which must be a
 * 			power of 2.
 * 
 * @param	n 
 * @param	align 
 * @return	uint32_t 
 */
static uint32_t	align_up(uint32_t n, uint32_t align)
{
	return ((n + align - 1) & ~(align - 1));
}

/**
 * @brief	Move the user heap break by increment bytes, mapping any
 *		newly needed pages through frame_alloc + paging_map_page.
 * Growth only maps pages, it never unmaps them: shrinking (increment < 0)
 * just moves the break back down, the already-mapped frames stay
 * reserved and mapped. Freed space below the break is handled by vfree,
 * not by giving pages back to the frame allocator.
 *
 * @param	increment Bytes to grow (>0) or shrink (<0) the break by
 * @return	The break's value before this call, or KBRK_FAILED if growth
 *		could not be satisfied (break left unchanged in that case)
 */
uint32_t	vbrk(int32_t increment)
{
	uint32_t	old_break;
	uint32_t	new_break;
	uint32_t	frame;

	if (g_heap_state.heap_break == 0)
	{
		g_heap_state.heap_break = VHEAP_START;
		g_heap_state.mapped_end = VHEAP_START;
	}
	old_break = g_heap_state.heap_break;
	new_break = old_break + increment;
	if (increment < 0 && new_break < VHEAP_START)
		return (KBRK_FAILED);
	while (new_break > g_heap_state.mapped_end)
	{
		frame = frame_alloc();
		if (frame == FRAME_ALLOC_FAILED)
			return (KBRK_FAILED);
		if (paging_map_page(g_heap_state.mapped_end, frame,
				g_heap_state.page_flags) != 0)
		{
			frame_free(frame);
			return (KBRK_FAILED);
		}
		g_heap_state.mapped_end += PAGE_SIZE;
	}
	g_heap_state.heap_break = new_break;
	return (old_break);
}

/**
 * @brief	Walk the block list for the first free block big enough.
 */
static t_block_header	*find_free_block(uint32_t size)
{
	t_block_header	*block;

	block = g_heap_state.head;
	while (block)
	{
		if (block->free && block->size >= size)
			return (block);
		block = block->next;
	}
	return (NULL);
}

/**
 * @brief	If block has enough room left over past size, carve a new
 *		free block out of the remainder instead of wasting it.
 */
static void	split_block(t_block_header *block, uint32_t size)
{
	t_block_header	*new_block;

	if (block->size < size + MIN_SPLIT_SIZE)
		return ;
	new_block = (t_block_header *)((uint8_t *)block
			+ sizeof(t_block_header) + size);
	new_block->size = block->size - size - sizeof(t_block_header);
	new_block->free = 1;
	new_block->next = block->next;
	block->next = new_block;
	block->size = size;
	if (g_heap_state.tail == block)
		g_heap_state.tail = new_block;
}

/**
 * @brief	Allocate size bytes from the user heap.
 * Reuses a free block if one is large enough (splitting it if it has
 * spare room), otherwise grows the heap with vbrk and appends a new
 * block at its previous end.
 *
 * @param	size Number of bytes requested
 * @return	Pointer to the usable memory, or NULL on failure
 */
void	*vmalloc(uint32_t size)
{
	t_block_header	*block;
	uint32_t		addr;

	size = align_up(size, BLOCK_ALIGN);
	block = find_free_block(size);
	if (block)
		split_block(block, size);
	else
	{
		addr = vbrk((int32_t)(sizeof(t_block_header) + size));
		if (addr == KBRK_FAILED)
			return (NULL);
		block = (t_block_header *)addr;
		block->size = size;
		block->next = NULL;
		if (g_heap_state.tail)
			g_heap_state.tail->next = block;
		else
			g_heap_state.head = block;
		g_heap_state.tail = block;
	}
	block->free = 0;
	return ((void *)(block + 1));
}

/**
 * @brief	Release a pointer previously returned by vmalloc.
 * Merges forward with the next block if it is also free, to limit
 * fragmentation (this is a singly-linked list, so merging backward
 * would need a full list walk: not done here).
 *
 * @param	ptr Pointer previously returned by vmalloc, or NULL (no-op)
 */
void	vfree(void *ptr)
{
	t_block_header	*block;

	if (!ptr)
		return ;
	block = (t_block_header *)ptr - 1;
	block->free = 1;
	while (block->next && block->next->free)
	{
		block->size += sizeof(t_block_header) + block->next->size;
		if (g_heap_state.tail == block->next)
			g_heap_state.tail = block;
		block->next = block->next->next;
	}
}

/**
 * @brief	Return the usable size of a pointer previously returned by
 *		vmalloc (0 for NULL).
 *
 * @param	ptr Pointer previously returned by vmalloc
 */
uint32_t	vsize(void *ptr)
{
	if (!ptr)
		return (0);
	return (((t_block_header *)ptr - 1)->size);
}

/**
 * @brief	Print the user heap's break/mapped_end and its block list
 *		(address, size, free/used), for inspection from the shell.
 */
void	vheap_dump(void)
{
	t_block_header	*block;
	int				i;

	kprintk(KERN_INFO "vheap: break=0x%x mapped_end=0x%x\n",
		g_heap_state.heap_break, g_heap_state.mapped_end);
	block = g_heap_state.head;
	i = 0;
	while (block)
	{
		kprintk(KERN_INFO "  [%d] %p size=%u %s\n", i, (void *)block,
			block->size, block->free ? "free" : "used");
		block = block->next;
		i++;
	}
}

