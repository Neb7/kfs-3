/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frame_allocator.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 10:44:48 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 11:13:03 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kmalloc.h"
#include "multiboot.h"

uint32_t g_total_ram = 0;

/**
 * @brief	Walk the multiboot mmap entries and set g_total_ram to the
 *		highest physical address reported, across ALL entries (not just
 *		type == available) so reserved holes still get a bitmap slot later.
 *
 * @param	mmap_addr Physical address of the first mmap entry
 * @param	mmap_length Total size in bytes of the mmap entry list
 */
void	get_total_ram(uint32_t mmap_addr, uint32_t mmap_length)
{
	struct multiboot_mmap_entry	*entry;
	uint32_t						offset;
	uint64_t						top;

	offset = 0;
	while (offset < mmap_length)
	{
		entry = (struct multiboot_mmap_entry *)(mmap_addr + offset);
		top = entry->base_addr + entry->length;
		if (top > g_total_ram)
			g_total_ram = (uint32_t)top;
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
	}
	else
	{
		// Handle the case where mmap is not available
		// or at least display info.
	}
}