/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:54:53 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/07/07 13:57:25 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shell.h"
#include "vga.h"
#include "gdt.h"
#include "dmesg.h"
#include "paging.h"
#include "kmalloc.h"
#include "kprintk.h"

typedef struct s_cmd
{
    const char  *name;
    void        (*fn)(void);
}   t_cmd;

static void cmd_shutdown(void)
{
    __asm__ volatile ("cli; hlt");
}

static void cmd_reboot(void)
{
    outb(0x64, 0xFE);
}

static void cmd_print_stack(void)
{
    print_stack(10);
}

static void cmd_dmesg(void)
{
    dmesg_print();
}

// Deliberately dereferences an address outside the identity-mapped 4MB
// (see PAGE_TEST_UNMAPPED_ADDR) to trigger a real #PF and exercise
// page_fault_handler instead of a triple fault.
static void cmd_page_fault(void)
{
    volatile uint32_t  *unmapped = (uint32_t *)PAGE_TEST_UNMAPPED_ADDR;

    *unmapped;
}

static void cmd_meminfo(void)
{
    frame_allocator_dump();
}

static void cmd_kheap(void)
{
    kheap_dump();
}

static void cmd_vheap(void)
{
    vheap_dump();
}

// Allocates two kernel-heap blocks, prints their pointer/size, frees the
// first one, then dumps the block list — proves kmalloc/kfree/ksize work
// live and shows a freed block sitting in the list, ready to be reused.
static void cmd_kalloc_test(void)
{
    void    *a;
    void    *b;

    a = kmalloc(64);
    b = kmalloc(128);
    kprintk(KERN_INFO "kalloc-test: a=%p (size=%u) b=%p (size=%u)\n",
        a, ksize(a), b, ksize(b));
    kfree(a);
    kprintk(KERN_INFO "kalloc-test: freed a\n");
    kheap_dump();
}

// Same demonstration as cmd_kalloc_test, for the user heap (vmalloc).
static void cmd_valloc_test(void)
{
    void    *a;
    void    *b;

    a = vmalloc(64);
    b = vmalloc(128);
    kprintk(KERN_INFO "valloc-test: a=%p (size=%u) b=%p (size=%u)\n",
        a, vsize(a), b, vsize(b));
    vfree(a);
    kprintk(KERN_INFO "valloc-test: freed a\n");
    vheap_dump();
}

static void cmd_help(void);

static t_cmd    g_cmds[] = {
    {"halt",    	cmd_shutdown},
    {"reboot",      cmd_reboot},
    {"print-stack", cmd_print_stack},
    {"dmesg",       cmd_dmesg},
    {"page-fault",  cmd_page_fault},
    {"meminfo",     cmd_meminfo},
    {"kheap",       cmd_kheap},
    {"vheap",       cmd_vheap},
    {"kalloc-test", cmd_kalloc_test},
    {"valloc-test", cmd_valloc_test},
    {"help",        cmd_help},
    {NULL,          NULL}
};

// Lists every command by walking g_cmds itself (the same table shell_parse
// dispatches from), so it never drifts out of sync when a command is
// added/removed: nothing here to update by hand.
static void cmd_help(void)
{
    int i;

    kprintk(KERN_INFO "Available commands:\n");
    i = 0;
    while (g_cmds[i].name)
    {
        kprintk(KERN_INFO "  %s\n", g_cmds[i].name);
        i++;
    }
}

static int  cmd_match(uint16_t *line, const char *name)
{
    int i;
    int len;

    len = ft_strlen(name);
    i = 0;
    while (i < len)
    {
        if ((char)(line[i] & 0xFF) != name[i])
            return (0);
        i++;
    }
    return ((line[i] & 0xFF) == ' ');
}

static void shell_parse(uint16_t *line)
{
    int i;

    // future: tokenize line into cmd + args
    i = 0;
    while (g_cmds[i].name)
    {
        if (cmd_match(line, g_cmds[i].name))
        {
            g_cmds[i].fn();
            return;
        }
        i++;
    }
}

void    shell_exec(uint16_t *line)
{
    shell_parse(line);
}
