/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gdt.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/30 14:55:25 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/30 15:39:50 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GDT_H
# define GDT_H

# include "kernel.h"

# define GDT_ADDR   0x00000800
# define GDT_COUNT  6

// Segment indices
# define GDT_NULL        0
# define GDT_KCODE       1
# define GDT_KDATA       2
# define GDT_KSTACK      3
# define GDT_UCODE       4
# define GDT_UDATA       5
# define GDT_USTACK      6  // optionnel, voir note

// Selectors (index * 8 | RPL)
# define SEG_KCODE  (GDT_KCODE  * 8)       // 0x08
# define SEG_KDATA  (GDT_KDATA  * 8)       // 0x10
# define SEG_KSTACK (GDT_KSTACK * 8)       // 0x18
# define SEG_UCODE  (GDT_UCODE  * 8 | 3)  // 0x23
# define SEG_UDATA  (GDT_UDATA  * 8 | 3)  // 0x2B
# define SEG_USTACK (GDT_USTACK * 8 | 3)  // 0x33

typedef struct __attribute__((packed)) s_gdt_entry
{
    uint16_t    limit_low;      // bits 0-15 de la limite
    uint16_t    base_low;       // bits 0-15 de la base
    uint8_t     base_mid;       // bits 16-23 de la base
    uint8_t     access;         // octet d'accès
    uint8_t     flags_limit;    // flags (4 bits) + limite bits 16-19
    uint8_t     base_high;      // bits 24-31 de la base
}   t_gdt_entry;

typedef struct __attribute__((packed)) s_gdt_ptr
{
    uint16_t    limit;
    uint32_t    base;
}   t_gdt_ptr;

void    gdt_init(void);
void    print_stack(uint32_t max_frames);

extern uint32_t stack_bottom_addr;
extern uint32_t stack_top_addr;

#endif