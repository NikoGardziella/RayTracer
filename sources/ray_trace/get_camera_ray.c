/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_camera_ray.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnoutere <pnoutere@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 14:04:53 by ctrouve           #+#    #+#             */
/*   Updated: 2022/12/19 19:06:05 by pnoutere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

t_ray	get_ray(t_2i coords, t_img *img, t_camera *camera)
{
	t_ray	ray;
	t_dir	dir;
	t_2d	norm_screen;
	double	h_w[2];

	norm_screen.x = (double)(2 * coords.x) / (double)img->dim.size.x - 1.0;
	norm_screen.y = (double)(-2 * coords.y) / (double)img->dim.size.y + 1.0;
	h_w[0] = (double)tan(camera->fov * PI / 360);
	h_w[1] = h_w[0] * camera->aspect_ratio;
	dir.forward = camera->ray.forward;
	dir.right = scale_vector(camera->right, h_w[1] * norm_screen.x);
	dir.up = scale_vector(camera->up, h_w[0] * norm_screen.y);
	ray.forward = add_vectors(dir.forward, dir.right);
	ray.forward = add_vectors(ray.forward, dir.up);
	ray.forward = normalize_vector(ray.forward);
	ray.origin = camera->ray.origin;
	return (ray);
}
