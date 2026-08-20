/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   idt.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 15:32:10 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/29 15:47:56 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

struct idt_entry idt[256];
struct idt_ptr   idtp;

/**
 * @brief	Set a gate in the IDT
 * 
 * @param	num		The gate number
 * @param	base	The base address of the interrupt handler
 * @param	selector	The code segment selector
 * @param	flags	The flags for the gate
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector  = selector;
    idt[num].zero      = 0;
    idt[num].flags     = flags;
}

/**
 * @brief	Initialize the IDT.
 */
void idt_init()
{
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;

    for (int i = 0; i < 256; i++)
        idt_set_gate(i, 0, 0, 0);

    __asm__ volatile ("lidt %0" :: "m"(idtp));
}
