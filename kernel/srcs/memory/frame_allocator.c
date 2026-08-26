/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frame_allocator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 10:44:48 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 14:02:50 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kmalloc.h"
#include "multiboot.h"
#include "frame_allocator.h"
#include "kprintk.h"

uint32_t	g_total_ram = 0;
uint32_t	g_total_frames = 0;
uint8_t		g_frame_bitmap[MAX_BITMAP_SIZE];

/**
 * @brief	Walk the multiboot mmap entries and set g_total_ram to the
 *		highest physical address reported by an available (type == 1)
 *		entry. Reserved entries are ignored here: near the 4GB mark they
 *		can push base_addr + length to exactly 0x100000000, which would
 *		overflow and wrap to 0 once truncated to uint32_t. Reserved holes
 *		are still marked in the bitmap separately, by mark_reserved_regions.
 *
 * @param	mmap_addr Physical address of the first mmap entry
 * @param	mmap_length Total size in bytes of the mmap entry list
 */
void	get_total_ram(uint32_t mmap_addr, uint32_t mmap_length)
{
	struct multiboot_mmap_entry	*entry;
	uint32_t					offset;
	uint64_t					top;

	offset = 0;
	while (offset < mmap_length)
	{
		entry = (struct multiboot_mmap_entry *)(mmap_addr + offset);
		if (entry->type == 1)
		{
			top = entry->base_addr + entry->length;
			if (top > g_total_ram)
				g_total_ram = (uint32_t)top;
		}
		offset += entry->size + sizeof(entry->size);
	}
}

/**
 * @brief	Mark a single frame as used in the bitmap. Silently ignores
 *		indices past MAX_FRAMES_SUPPORTED instead of writing out of bounds.
 *
 * @param	frame_index Index of the frame to mark (address / PAGE_SIZE)
 */
void	set_frame_used(uint32_t frame_index)
{
	if (frame_index >= MAX_FRAMES_SUPPORTED)
		return ;
	g_frame_bitmap[frame_index / 8] |= (1 << (frame_index % 8));
}

/**
 * @brief	Mark [0, kernel_end) as used, in one contiguous pass. Since
 *		g_frame_bitmap is static (lives in .bss), this also covers the
 *		bitmap's own storage for free: kernel_end sits right after .bss.
 */
void	mark_kernel_reserved(void)
{
	uint32_t	frame;
	uint32_t	kernel_frames;

	kernel_frames = ((uint32_t)&kernel_end + PAGE_SIZE - 1) / PAGE_SIZE;
	frame = 0;
	while (frame < kernel_frames)
	{
		set_frame_used(frame);
		frame++;
	}
}

/**
 * @brief	Walk the mmap a second time and mark every frame covered by a
 *		non-available (type != 1) entry as used (ACPI, MMIO, etc.).
 *		Must run after the bitmap has been zeroed, not before.
 *
 * @param	mmap_addr Physical address of the first mmap entry
 * @param	mmap_length Total size in bytes of the mmap entry list
 */
void	mark_reserved_regions(uint32_t mmap_addr, uint32_t mmap_length)
{
	struct multiboot_mmap_entry	*entry;
	uint32_t						offset;
	uint32_t						frame;
	uint32_t						start_frame;
	uint32_t						end_frame;

	offset = 0;
	while (offset < mmap_length)
	{
		entry = (struct multiboot_mmap_entry *)(mmap_addr + offset);
		if (entry->type != 1)
		{
			start_frame = (uint32_t)(entry->base_addr / PAGE_SIZE);
			end_frame = (uint32_t)((entry->base_addr + entry->length
						+ PAGE_SIZE - 1) / PAGE_SIZE);
			frame = start_frame;
			while (frame < end_frame)
			{
				set_frame_used(frame);
				frame++;
			}
		}
		offset += entry->size + sizeof(entry->size);
	}
}

void	frame_allocator_init(uint32_t mb_info_addr)
{
	struct multiboot *mb_info = (struct multiboot *)mb_info_addr;

	// check mb_info_struct integrity
	if (mb_info->flags & MULTIBOOT_FLAG_MMAP)
	{
		get_total_ram(mb_info->mmap_addr, mb_info->mmap_length);
		g_total_frames = g_total_ram / PAGE_SIZE;
		if (g_total_frames > MAX_FRAMES_SUPPORTED)
			g_total_frames = MAX_FRAMES_SUPPORTED;
		ft_bzero(g_frame_bitmap, sizeof(g_frame_bitmap));
		mark_kernel_reserved();
		mark_reserved_regions(mb_info->mmap_addr, mb_info->mmap_length);
		kprintk(KERN_INFO "frame_allocator_init: total RAM %u bytes, %u frames\n",
			g_total_ram, g_total_frames);
	}
	else
	{
		kprintk(KERN_ERR "frame_allocator_init: multiboot mmap not available\n");
		// Handle the case where mmap is not available
		// or at least display info.
	}
}

/**
 * @brief	Clear a single frame's bit in the bitmap (mark it free).
 *		Mirrors set_frame_used's bound check.
 *
 * @param	frame_index Index of the frame to clear (address / PAGE_SIZE)
 */
static void	clear_frame_used(uint32_t frame_index)
{
	if (frame_index >= MAX_FRAMES_SUPPORTED)
		return ;
	g_frame_bitmap[frame_index / 8] &= ~(1 << (frame_index % 8));
}

/**
 * @brief	Return 1 if the given frame is marked used, 0 if free.
 */
static int	frame_is_used(uint32_t frame_index)
{
	return ((g_frame_bitmap[frame_index / 8] >> (frame_index % 8)) & 1);
}

/**
 * @brief	Find the first free frame within the RAM actually reported by
 *		multiboot (g_total_frames), mark it used, and return its
 *		physical address.
 *
 * @return	The physical address of the newly reserved frame, or
 *		FRAME_ALLOC_FAILED if no frame is free.
 */
uint32_t	frame_alloc(void)
{
	uint32_t	frame;

	frame = 0;
	while (frame < g_total_frames)
	{
		if (!frame_is_used(frame))
		{
			set_frame_used(frame);
			return (frame * PAGE_SIZE);
		}
		frame++;
	}
	return (FRAME_ALLOC_FAILED);
}

/**
 * @brief	Mark the frame containing this physical address as free again.
 *
 * @param	addr Physical address previously returned by frame_alloc
 */
void	frame_free(uint32_t addr)
{
	clear_frame_used(addr / PAGE_SIZE);
}

/**
 * @brief	Print a summary of the frame allocator's current state: total
 *		RAM detected, and how many of the frames within it are used/free.
 */
void	frame_allocator_dump(void)
{
	uint32_t	frame;
	uint32_t	used;

	used = 0;
	frame = 0;
	while (frame < g_total_frames)
	{
		if (frame_is_used(frame))
			used++;
		frame++;
	}
	kprintk(KERN_INFO
		"meminfo: %u bytes RAM, %u frames total, %u used, %u free\n",
		g_total_ram, g_total_frames, used, g_total_frames - used);
}
