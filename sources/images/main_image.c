/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_image.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmalesev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 11:33:20 by dmalesev          #+#    #+#             */
/*   Updated: 2022/10/31 12:52:47 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	main_image(t_img *img, void *param)
{
	t_env	*env;
	t_2i	coords;

	env = param;
	coords = (t_2i){img->dim.size.x - 1, img->dim.size.y - 1};
	draw_rectf(&(t_pxl_func){&put_pixel, img}, (t_2i){0, 0}, coords, 0xFFFFFF);
}
