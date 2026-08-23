/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   paging.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/20 16:48:52 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/23 12:25:15 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "paging.h"
#include "kprintk.h"
#include "frame_allocator.h"
#include "kernel_panic.h"

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
	kernel_panic("PAGE FAULT at 0x%x (error=0x%x): %s, %s, %s",
		fault_addr, error_code,
		(error_code & 0x1) ? "protection violation" : "page not present",
		(error_code & 0x2) ? "write" : "read",
		(error_code & 0x4) ? "user mode" : "kernel mode");
}

/**
 * @brief	Return the page table covering virt_addr, allocating and
 *		zeroing a fresh one via frame_alloc if it is missing and create
 *		is set. The table's own frame is written to directly through its
 *		physical address, so it must fall inside memory already mapped
 *		identity (virt == phys): true today since the kernel only ever
 *		calls this before enough frames have been handed out to leave
 *		the first 4MB identity-mapped range (see paging_build_identity_map).
 *
 * @param	virt_addr Virtual address whose table is requested
 * @param	create Allocate the table if it does not exist yet
 * @return	Pointer to the 1024-entry page table, or NULL if absent
 *		(create == 0) or allocation failed (create == 1)
 */
static t_page_entry	*paging_get_table(uint32_t virt_addr, int create)
{
	uint32_t	dir_index;
	uint32_t	frame;

	dir_index = virt_addr >> 22;
	if (!(page_directory[dir_index] & PAGE_PRESENT))
	{
		if (!create)
			return (NULL);
		frame = frame_alloc();
		if (frame == FRAME_ALLOC_FAILED)
			return (NULL);
		ft_bzero((void *)frame, PAGE_TABLE_SIZE * sizeof(t_page_entry));
		// USER left set on the directory entry: the actual restriction
		// is enforced per-page below, in the table entry itself.
		page_directory[dir_index] = frame | PAGE_PRESENT | PAGE_RW | PAGE_USER;
	}
	return ((t_page_entry *)(page_directory[dir_index] & ~0xFFF));
}

/**
 * @brief	Map a single 4KB page: virt_addr -> phys_addr, creating the
 *		page table for that range if needed.
 *
 * @param	virt_addr Virtual address to map (rounded down to its page)
 * @param	phys_addr Physical address it should point to (rounded down)
 * @param	flags Extra entry flags (e.g. PAGE_RW, PAGE_USER); PAGE_PRESENT
 *		is added automatically
 * @return	0 on success, -1 if the page table could not be created
 */
int	paging_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags)
{
	t_page_entry	*table;
	uint32_t		table_index;

	table = paging_get_table(virt_addr, 1);
	if (!table)
		return (-1);
	table_index = (virt_addr >> 12) & 0x3FF;
	table[table_index] = (phys_addr & ~0xFFF) | (flags & 0xFFF) | PAGE_PRESENT;
	__asm__ volatile ("invlpg (%0)" :: "r"(virt_addr) : "memory");
	return (0);
}

/**
 * @brief	Look up the physical address currently mapped to virt_addr.
 *
 * @param	virt_addr Virtual address to translate
 * @return	The physical address (offset within the page included), or 0
 *		if virt_addr is not currently mapped
 */
uint32_t	paging_get_phys(uint32_t virt_addr)
{
	t_page_entry	*table;
	uint32_t		table_index;

	table = paging_get_table(virt_addr, 0);
	if (!table)
		return (0);
	table_index = (virt_addr >> 12) & 0x3FF;	// index of the page within the table
	if (!(table[table_index] & PAGE_PRESENT))
		return (0);
	return ((table[table_index] & ~0xFFF) | (virt_addr & 0xFFF));
}
