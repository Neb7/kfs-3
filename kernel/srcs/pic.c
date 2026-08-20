/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pic.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/29 15:48:38 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/29 15:55:43 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"

/**
 * @brief	Send a byte to a port
 * 
 * @param	port The port to send to
 * @param	val The value to send
 */
inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile ("outb %0, %1" :: "a"(val), "Nd"(port));
}

/**
 * @brief	Read a byte from a port
 * 
 * @param	port The port to read from
 * @return	The value read
 */
inline uint8_t inb(uint16_t port)
{
	uint8_t val;
	__asm__ volatile ("inb %1, %0" : "=a"(val) : "Nd"(port));
	return val;
}

/**
 * @brief	Initialize the PIC
 */
void pic_init()
{
	// ICW1 — beginning
	outb(PIC1_CMD,  0x11);
	outb(PIC2_CMD,  0x11);

	// ICW2 — vector remapping
	outb(PIC1_DATA, 0x20);  // IRQ0-7  → vecteurs 32-39
	outb(PIC2_DATA, 0x28);  // IRQ8-15 → vecteurs 40-47

	// ICW3 — cascade
	outb(PIC1_DATA, 0x04);  // PIC1 : PIC2 connected to IRQ2
	outb(PIC2_DATA, 0x02);  // PIC2 : connected to IRQ2 du PIC1

	// ICW4 — mode 8086
	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	// Masks — everything masked except IRQ1 (keyboard)
	outb(PIC1_DATA, 0xFD);  // 1111 1101
	outb(PIC2_DATA, 0xFF);
}
