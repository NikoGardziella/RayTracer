/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnoutere <pnoutere@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/02 10:43:48 by dmalesev          #+#    #+#             */
/*   Updated: 2022/12/19 13:03:52 by pnoutere         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

t_camera	init_camera(t_2i size, t_3d origin, t_3d forward, double fov)
{
	t_camera	camera;

	camera.fov = fov;
	camera.aspect_ratio = (float)size.x / (float)size.y;
	camera.up = (t_3d){0.0f, 1.0f, 0.0f};
	camera.ray.forward = forward;
	camera.ray.origin = origin;
	if (dot_product(camera.up, camera.ray.forward) == 1)
		camera.up = normalize_vector((t_3d){0.0f, 0.0f, 1.0f});
	camera.right = cross_product(camera.ray.forward, camera.up);
	camera.up = normalize_vector(cross_product(camera.right,
				camera.ray.forward));
	camera.right = cross_product(camera.ray.forward, camera.up);
	return (camera);
}
