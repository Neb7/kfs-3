/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:48:52 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/20 19:08:40 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "paging.h"
#include "kprintk.h"

extern void	page_fault_stub(void);

// Reserved below 1M by GDT_ADDR (0x800). Aligned on 4096: the low 12 bits
// of their address are 0, so we can safely OR flags into them below.
static t_page_entry	page_directory[PAGE_DIRECTORY_SIZE] __attribute__((aligned(4096)));
static t_page_entry	page_table_0[PAGE_TABLE_SIZE]       __attribute__((aligned(4096)));

/**
 * @brief	Identity-map the first 4MB of physical memory (page_table_0),
 *		then hook that table into the first entry of the directory.
 * Identity map = virtual address == physical address, so that enabling
 * paging does not move anything the CPU is currently executing/reading
 * (kernel code at 1M, VGA buffer at 0xB8000, GDT at 0x800).
 */
static void	paging_build_identity_map(void)
{
	int	i;

	i = 0;
	while (i < PAGE_TABLE_SIZE)
	{
		page_table_0[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
		i++;
	}
	page_directory[0] = ((uint32_t)page_table_0) | PAGE_PRESENT | PAGE_RW;
	i = 1;
	while (i < PAGE_DIRECTORY_SIZE)
	{
		page_directory[i] = 0; // not present: unused for now
		i++;
	}
}

/**
 * @brief	Load CR3 with the directory's physical address, then set
 *		bit 31 (PG) of CR0 to actually turn paging on.
 * 
 */
static void	paging_enable(void)
{
	__asm__ volatile (
		"mov %0, %%cr3\n\t"
		"mov %%cr0, %%eax\n\t"
		"or $0x80000000, %%eax\n\t"
		"mov %%eax, %%cr0\n\t"
		:: "r"(page_directory) : "eax"
	);
}

/**
 * @brief	Build an identity-mapped page directory/table for the first
 *		4MB of physical memory and enable paging.
 */
void	paging_init(void)
{
	uint32_t	cr0;

	paging_build_identity_map();
	paging_enable();
	__asm__ volatile ("mov %%cr0, %0" : "=r"(cr0));
	kprintk(KERN_INFO "Paging enabled, directory at 0x%x (PG=%d)\n",
		(uint32_t)page_directory, (cr0 >> 31) & 1);
	idt_set_gate(14, (uint32_t)page_fault_stub, 0x08, 0x8E);
}

/**
 * @brief	Handle a page fault exception by printing the faulting address*
 * There is no recovery logic yet (no swap, no demand paging), so any
 * page fault is treated as fatal: report it and halt the kernel instead
 * of iret-ing back into an instruction that would just fault again.
 * 
 * @param	error_code The error code pushed by the CPU on the stack
 */
void	page_fault_handler(uint32_t error_code)
{
	uint32_t	fault_addr;

	__asm__ volatile ("mov %%cr2, %0" : "=r"(fault_addr));
	kprintk(KERN_EMERG "PAGE FAULT at 0x%x (error=0x%x)\n",
		fault_addr, error_code);
	kprintk(KERN_EMERG "  %s, %s, %s\n",
		(error_code & 0x1) ? "protection violation" : "page not present",
		(error_code & 0x2) ? "write" : "read",
		(error_code & 0x4) ? "user mode" : "kernel mode");
	kprintk(KERN_EMERG "Kernel halted.\n");
	__asm__ volatile ("cli");
	while (1)
		__asm__ volatile ("hlt");
}
