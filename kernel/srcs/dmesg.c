/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmesg.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:54:16 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/07/07 13:54:29 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dmesg.h"
#include "kernel.h"

static char g_dmesg[DMESG_LINES][DMESG_LINE_LEN];
static int  g_head  = 0;
static int  g_count = 0;

void    dmesg_write(const char *msg)
{
    ft_strlcpy(g_dmesg[g_head], msg, DMESG_LINE_LEN);
    g_head = (g_head + 1) % DMESG_LINES;
    if (g_count < DMESG_LINES)
        g_count++;
}

void    dmesg_print(void)
{
    int start;
    int i;
    int j;

    start = (g_count < DMESG_LINES) ? 0 : g_head;
    i = 0;
    while (i < g_count)
    {
        j = 0;
        while (g_dmesg[(start + i) % DMESG_LINES][j])
            putchar(g_dmesg[(start + i) % DMESG_LINES][j++]);
        putchar('\n');
        i++;
    }
}
