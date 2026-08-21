/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kmalloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vsyutkin <vsyutkin@student.42mulhouse.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/21 07:59:57 by vsyutkin          #+#    #+#             */
/*   Updated: 2026/08/21 07:59:57 by vsyutkin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	More-than-naive allocator.
	On demand, gives necessary memory space, from paged memory.
	Will follow what page is used, what in the page is allocated to optimize memory usage
(e.g. no 2-full page reserved for 4097 allocated only).
*/
