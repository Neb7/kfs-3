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

static t_cmd    g_cmds[] = {
    {"halt",    	cmd_shutdown},
    {"reboot",      cmd_reboot},
    {"print-stack", cmd_print_stack},
    {"dmesg",       cmd_dmesg},
    {NULL,          NULL}
};

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

void    shell_exec(int y)
{
    shell_parse(g_screens[g_cur].lines[y]);
}
