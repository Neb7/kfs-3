/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   multiboot.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 11:03:04 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 11:05:31 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// struct taken https://github.com/stevej/osdev/blob/master/kernel/include/multiboot.h


#ifndef MULTIBOOT_H
# define MULTIBOOT_H

# include "kernel.h"

# define MULTIBOOT_HEADER_MAGIC	0x1BADB002

# define MULTIBOOT_FLAG_MEM		0x001
# define MULTIBOOT_FLAG_DEVICE	0x002
# define MULTIBOOT_FLAG_CMDLINE	0x004
# define MULTIBOOT_FLAG_MODS	0x008
# define MULTIBOOT_FLAG_AOUT	0x010
# define MULTIBOOT_FLAG_ELF		0x020
# define MULTIBOOT_FLAG_MMAP	0x040
# define MULTIBOOT_FLAG_CONFIG	0x080
# define MULTIBOOT_FLAG_LOADER	0x100
# define MULTIBOOT_FLAG_APM		0x200
# define MULTIBOOT_FLAG_VBE		0x400

// Truncated right after mmap_addr: nothing past that offset is used here.
struct multiboot
{
	uint32_t	flags;
	uint32_t	mem_lower;
	uint32_t	mem_upper;
	uint32_t	boot_device;
	uint32_t	cmdline;
	uint32_t	mods_count;
	uint32_t	mods_addr;
	uint32_t	num;
	uint32_t	size;
	uint32_t	addr;
	uint32_t	shndx;
	uint32_t	mmap_length;
	uint32_t	mmap_addr;
} __attribute__((packed));

// One entry of the mmap_addr list (variable-length: advance by size + 4).
struct multiboot_mmap_entry
{
	uint32_t	size;
	uint64_t	base_addr;
	uint64_t	length;
	uint32_t	type;
} __attribute__((packed));

#endif
