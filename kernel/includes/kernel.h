/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:55:16 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/07/07 13:55:17 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KERNEL_H
# define KERNEL_H

# include "libft/libft.h"

typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
typedef signed char			int8_t;
typedef short				int16_t;
typedef int					int32_t;
typedef long long			int64_t;

extern char scancode_map[128];
extern uint16_t *vga;

struct idt_entry {
	uint16_t base_low;   // bits 0-15 of the handler address
	uint16_t selector;   // code segment (usually 0x08) 0x10
	uint8_t  zero;	   // always 0
	uint8_t  flags;	  // type + privilege
	uint16_t base_high;  // bits 16-31 of the handler address
} __attribute__((packed));

struct idt_ptr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

extern struct idt_entry idt[256];
extern struct idt_ptr   idtp;

#define PIC1_CMD  0x20 // master
#define PIC1_DATA 0x21 // master
#define PIC2_CMD  0xA0 // slave
#define PIC2_DATA 0xA1 // slave

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
void pic_init();
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void idt_init();
void enable_cursor(uint8_t top, uint8_t bottom);
void update_cursor();
extern void keyboard_stub(void);
void putchar(char c);
void kernel_main(void);

#endif
