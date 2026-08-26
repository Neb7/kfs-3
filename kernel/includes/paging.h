/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:48:55 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/20 17:06:45 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PAGING_H
# define PAGING_H

# include "kernel.h"

# define PAGE_DIRECTORY_SIZE	1024
# define PAGE_TABLE_SIZE	1024
# define PAGE_SIZE	0x1000
# define PAGE_PRESENT	0x1
# define PAGE_RW	0x2
# define PAGE_USER	0x4

// Recursive page directory mapping: page_directory's last entry points to
// itself, so every page table (wherever its physical frame actually is)
// becomes reachable at a fixed virtual address, without ever needing to
// dereference a raw physical address as if it were virtual.
// Source: https://wiki.osdev.org/Page_Tables (section "Recursive mapping")
# define PAGE_DIR_INDEX	(PAGE_DIRECTORY_SIZE - 1)
# define PAGE_DIR_VADDR	0xFFFFF000
# define PAGE_TABLE_VADDR(dir_index)	(0xFFC00000 + ((dir_index) << 12))

// deliberately outside the 4MB identity-mapped range, for testing #PF
# define PAGE_TEST_UNMAPPED_ADDR	0x00500000

typedef uint32_t t_page_entry;

void		paging_init(void);
void		page_fault_handler(uint32_t error_code);
int			paging_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);
uint32_t	paging_get_phys(uint32_t virt_addr);

#endif
