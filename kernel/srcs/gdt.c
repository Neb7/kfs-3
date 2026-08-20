/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 14:55:44 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/30 15:45:36 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gdt.h"
#include "kernel.h"

// The GDT will be placed by the linker at GDT_ADDR (0x800)
static t_gdt_entry gdt[GDT_COUNT + 1] __attribute__((section(".gdt")));
static t_gdt_ptr    gdtp;

/*
** access byte:
**   bit 7   : present (1)
**   bits 5-6: DPL (0=kernel, 3=user)
**   bit 4   : type descriptor (1=code/data)
**   bit 3   : executable (1=code, 0=data/stack)
**   bit 2   : direction/conforming
**   bit 1   : readable(code) / writable(data)
**   bit 0   : accessed (set to 1 by CPU)
**
** flags_limit (high byte):
**   bit 7 (G) : granularity (1=4KB, 0=1B)
**   bit 6 (D/B): 32-bit (1) or 16-bit (0)
**   bit 5 (L) : 64-bit (0 in x86-32)
**   bit 4     : reserved
**   bits 0-3  : limit bits 16-19
*/

static void gdt_set_entry(int i, uint32_t base, uint32_t limit,
                           uint8_t access, uint8_t flags)
{
    gdt[i].base_low    = base & 0xFFFF;
    gdt[i].base_mid    = (base >> 16) & 0xFF;
    gdt[i].base_high   = (base >> 24) & 0xFF;
    gdt[i].limit_low   = limit & 0xFFFF;
    gdt[i].flags_limit = ((flags & 0xF) << 4) | ((limit >> 16) & 0xF);
    gdt[i].access      = access;
}

void gdt_init(void)
{
    // Null descriptor (mandatory)
    gdt_set_entry(GDT_NULL,  0, 0,          0x00, 0x0);

    // Kernel Code — base 0, limit 10MB, ring 0, executable
    gdt_set_entry(GDT_KCODE, 0, 0x009FFFFF, 0x9A, 0xC);
    //                                       P=1 DPL=00 S=1 E=1 DC=0 RW=1 A=0
    //                                       flags: G=1 DB=1 → 4KB granularity, 32-bit

    // Kernel Data — base 0, limit 10MB, ring 0, data
    gdt_set_entry(GDT_KDATA, 0, 0x009FFFFF, 0x92, 0xC);

    // Kernel Stack — base 0, limit 10MB, ring 0, data (expand-down possible)
    gdt_set_entry(GDT_KSTACK, 0, 0x009FFFFF, 0x92, 0xC);

    // User Code — base 0, limit 10MB, ring 3, executable
    gdt_set_entry(GDT_UCODE, 0, 0x009FFFFF, 0xFA, 0xC);

    // User Data — base 0, limit 10MB, ring 3, data
    gdt_set_entry(GDT_UDATA, 0, 0x009FFFFF, 0xF2, 0xC);

    // User Stack — base 0, limit 10MB, ring 3, data
    gdt_set_entry(GDT_USTACK, 0, 0x009FFFFF, 0xF2, 0xC);

    gdtp.limit = (sizeof(t_gdt_entry) * GDT_COUNT) - 1;
    gdtp.base  = GDT_ADDR;

    // Load the GDT and reload segments (see boot.asm: gdt_flush)
    __asm__ volatile (
        "lgdt %0\n\t"
        "movw $0x10, %%ax\n\t"   // SEG_KDATA
        "movw %%ax,  %%ds\n\t"
        "movw %%ax,  %%es\n\t"
        "movw %%ax,  %%fs\n\t"
        "movw %%ax,  %%gs\n\t"
        "movw $0x18, %%ax\n\t"   // SEG_KSTACK
        "movw %%ax,  %%ss\n\t"
        "ljmp $0x08, $1f\n\t"    // far jump pour recharger CS (SEG_KCODE)
        "1:\n\t"
        :: "m"(gdtp) : "eax"
    );
}

/*
** print_stack — display kernel stack frames
** Uses EBP to traverse the frame chain.
** Each frame: [EBP] = previous EBP, [EBP+4] = return address
*/
void print_stack(uint32_t max_frames)
{
    uint32_t    *ebp;
    uint32_t    frame;

    __asm__ volatile ("mov %%ebp, %0" : "=r"(ebp));

    kprintf("=== Kernel Stack Trace ===\n");
    frame = 0;
    while (ebp && frame < max_frames)
    {
        uint32_t ret_addr = *(ebp + 1);
        if (ret_addr == 0)
            break;
        kprintf("  [%u] EBP=0x%x  RET=0x%x\n", frame, (uint32_t)ebp, ret_addr);
        ebp = (uint32_t *)(*ebp);
        frame++;
    }
    if (frame == 0)
        kprintf("  (empty stack or unreliable EBP)\n");
    kprintf("==========================\n");
}