/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kernel.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 15:25:41 by benpicar          #+#    #+#             */
/*   Updated: 2026/06/30 15:58:16 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "kernel.h"
#include "vga.h"
#include "gdt.h"
#include "kprintk.h"

uint16_t	*vga = (uint16_t*)0xB8000;

char scancode_map[128] = {
	0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0, 0,
	'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
	'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
	'z','x','c','v','b','n','m',',','.','/', 0, 0, 0,' '
};

void	delete_handler()
{
	t_vga *vga_cur = &g_screens[g_cur];

	if (vga_cur->cursor_x > 0)
	{
		vga_cur->cursor_x--;
		putchar(' ');  // overwrite the character with a space
		vga_cur->cursor_x--;
	}
	else if (vga_cur->cursor_y > 0)
	{
		vga_cur->cursor_y--;
		vga_cur->cursor_x = 79;
		putchar(' ');  // overwrite the character with a space
		vga_cur->cursor_y--;
		vga_cur->cursor_x = 79;
	}
	update_cursor();
}

/**
 * @brief	Handle keyboard interrupts
 */
void keyboard_handler()
{
	char	c;
	uint8_t scancode = inb(0x60);

	if (scancode & 0x80) {
		// key released ignored
	}
	else if (scancode == 0x53) // Delete key
		delete_handler();
	else if (g_cur != 0 && scancode == 0x3B) // F1
		switch_screen(0);
	else if (g_cur != 1 && scancode == 0x3C) // F2
		switch_screen(1);
	else
	{
		c = scancode_map[scancode];
		if (c)
		{
			putchar(c);
		}
	}

	outb(0x20, 0x20);  // EOI — end of interuption PIC
}

/**
 * @brief	Enable the cursor
 * 
 * @param	top	The top scan line
 * @param	bottom	The bottom scan line
 */
void enable_cursor(uint8_t top, uint8_t bottom)
{
	outb(0x3D4, 10);
	outb(0x3D5, (0x00 << 5) | top);	// bit 5 = 0 → blinking

	outb(0x3D4, 11);
	outb(0x3D5, bottom);
}

/**
 * @brief	Update the cursor position
 */
void update_cursor()
{
	uint16_t pos = g_screens[g_cur].cursor_y * 80 + g_screens[g_cur].cursor_x;
	outb(0x3D4, 14);
	outb(0x3D5, pos >> 8);
	outb(0x3D4, 15);
	outb(0x3D5, pos & 0xFF);
}

/**
 * @brief	Print a character to the screen
 * 
 * @param	c The character to print
 */
void putchar(char c)
{
	t_vga *vga_cur = &g_screens[g_cur];
	
	if (c == '\n')
	{
		vga_cur->cursor_x = 0;
		vga_cur->cursor_y++;
		if (vga_cur->cursor_y >= 25)
		{
			scroll();
		}
	}
	else
	{
		vga[vga_cur->cursor_y * 80 + vga_cur->cursor_x] = (vga_cur->color << 8) | c;
		vga_cur->lines[vga_cur->cursor_y][vga_cur->cursor_x] = (vga_cur->color << 8) | c;
		vga_cur->cursor_x++;
		if (vga_cur->cursor_x >= 80)
		{
			vga_cur->cursor_x = 0;
			vga_cur->cursor_y++;
			if (vga_cur->cursor_y >= 25)
			{
				scroll();
			}
		}
	}
	update_cursor();
}

/**
 * @brief	Main kernel function
 */
void kernel_main(void)
{
	gdt_init();
	idt_init();
	pic_init();
	idt_set_gate(33, (uint32_t)keyboard_stub, 0x08, 0x8E);

	__asm__ volatile ("sti");  // activate interrupts

	ft_bzero(g_screens, sizeof(g_screens));
	g_screens[0].color = 0x0F;
	g_screens[1].color = 0x24;
	ft_memset_short(g_screens[0].lines, 0x0F << 8 | ' '
			, sizeof(g_screens[0].lines));
	ft_memset_short(g_screens[1].lines, 0x24 << 8 | ' '
			, sizeof(g_screens[1].lines));
	enable_cursor(0, 15);
	kprintf("42\nGDT initialisee a 0x%x\n", GDT_ADDR);
    print_stack(10);   // affiche la stack kernel au boot
	while (1)
	{}
}
