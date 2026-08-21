/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   frame_allocator.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: benpicar <benpicar@student.42mulhouse.fr > +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 14:00:00 by benpicar          #+#    #+#             */
/*   Updated: 2026/08/21 14:00:00 by benpicar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FRAME_ALLOCATOR_H
# define FRAME_ALLOCATOR_H

# include "kernel.h"

# define FRAME_ALLOC_FAILED	0xFFFFFFFF

void		frame_allocator_init(uint32_t mb_info_addr);
uint32_t	frame_alloc(void);
void		frame_free(uint32_t addr);

#endif
