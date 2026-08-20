/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dmesg.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/07 13:55:24 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/07/07 13:55:25 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DMESG_H
# define DMESG_H

# define DMESG_LINES    64
# define DMESG_LINE_LEN 128

void    dmesg_write(const char *msg);
void    dmesg_print(void);

#endif
