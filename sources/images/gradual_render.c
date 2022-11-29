/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   gradual_render.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmalesev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 13:34:58 by dmalesev          #+#    #+#             */
/*   Updated: 2022/11/23 14:50:11 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

double	time_since_success(double ammount, int id,int mode)
{
	static struct timespec	time[8];
	struct timespec			curr_time;
	long int				elapsed;

	clock_gettime(CLOCK_MONOTONIC, &curr_time);
	elapsed = (curr_time.tv_sec - time[id].tv_sec) * 1000000
		+ (curr_time.tv_nsec - time[id].tv_nsec) / 1000;
	if (mode == 1 || (mode == 2 && elapsed > (long int)(ammount * 1000000.0)))
	{
		time[id] = curr_time;
	}
	return ((double)elapsed / 1000000.0);
}

void	gradual_render(t_img *img, void *param)
{
	t_env	*env;
	t_2i	coords;
	t_2i	color;
	
	color = (t_2i){0x00000,0xFFFFFF};
	env = param;
	if (env->scene->resolution.x == env->scene->resolution_range.x && env->scene->resolution.y == env->scene->resolution_range.y)
	{
		if (env->scene->accum_resolution.x == env->scene->resolution_range.x && env->scene->accum_resolution.y == env->scene->resolution_range.x)
			env->plot_time = (double)time_since_success(0.0f, 0,2);
	}
	if (env->scene->accum_resolution.x == env->scene->resolution_range.x && env->scene->accum_resolution.y == env->scene->resolution_range.y)
		env->plot_time = (double)time_since_success(0.0f, 0,2);
	coords = (t_2i){0, img->dim.size.y - (int)env->font->bound_box[1] - (int)env->font->bound_box[1]};
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, "Plot time: " ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords, (t_2d){env->plot_time, 3.0f} ,color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " Frame: " ,color);
	coords = display_int(&(t_pxl){env->font, put_pixel, img}, coords, env->frame_index ,color);
	coords = (t_2i){0, img->dim.size.y - (int)env->font->bound_box[1]};
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, "cam:" ,color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " x:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords,(t_2d){env->scene->camera->ray.origin.x, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " y:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords,(t_2d){env->scene->camera->ray.origin.y, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " z:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords,(t_2d){env->scene->camera->ray.origin.z, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " dir:" ,color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " x:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords,(t_2d){env->scene->camera->ray.forward.x, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " y:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords,(t_2d){env->scene->camera->ray.forward.y, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, " z:" ,color);
	coords = display_double(&(t_pxl){env->font, put_pixel, img}, coords, (t_2d){env->scene->camera->ray.forward.z, 1.0},color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, "  KEY: " ,color);
	coords = display_int(&(t_pxl){env->font, put_pixel, img}, coords, (int)env->keymap, color);
	coords = display_str(&(t_pxl){env->font, put_pixel, img}, coords, "  MOUSE: " ,color);
	coords = display_int(&(t_pxl){env->font, put_pixel, img}, coords, (int)env->mouse.state, color);
	
	coords = (t_2i){img->dim.size.x / 2, img->dim.size.y / 2};
	draw_circle(&(t_pxl_func){&put_pixel, img}, coords, 3, 0x5D8ACF);
	draw_circle(&(t_pxl_func){&put_pixel, img}, coords, 4, 0x052156);
	coords = (t_2i){img->dim.size.x - 1, img->dim.size.y - 1};
	draw_rect(&(t_pxl_func){&put_pixel, img}, (t_2i){0, 0}, coords, 0xFFFF00);
}
