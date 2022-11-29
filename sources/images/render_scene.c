/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_scene.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctrouve <ctrouve@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 14:38:21 by ctrouve           #+#    #+#             */
/*   Updated: 2022/11/29 12:29:52 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"
#include <stdlib.h>

uint32_t	state = 1234;

uint32_t	random_in_range(uint32_t lower, uint32_t upper, uint32_t *state)
{
	return ((xorshift32(state) % (upper - lower + 1)) + lower);
}

t_3d	random_vector(t_3d refl_vec, float max_theta)
{
	t_3d	vec;
	t_3d	tangent;
	t_3d	bitangent;
	float	phi;
	float	theta;
	float	sin_theta;
	t_2f	random;

	random.x = (float)random_rangef(0, 1, &state);
	random.y = (float)random_rangef(0, 1, &state);
	if (refl_vec.x > refl_vec.z)
		tangent = (t_3d){-refl_vec.y, refl_vec.x, 0.0};
	else
		tangent = (t_3d){0.0, -refl_vec.z, refl_vec.y};
	
	if (dot_product(refl_vec, (t_3d){0.0, 1.0, 0.0}) == 1.0)
		tangent = cross_product(refl_vec, (t_3d){0.0, 0.0, 1.0});
	else
		tangent = cross_product(refl_vec, (t_3d){0.0, 1.0, 0.0});
	tangent = normalize_vector(tangent);
	bitangent = cross_product(refl_vec, tangent);
	tangent = cross_product(refl_vec, bitangent);
	tan_temp[0] = tangent;
	tan_temp[1] = bitangent;
	phi = 2.0f * (float)PI * random.x;
	theta = random.y * max_theta;
	sin_theta = sinf(theta);
	vec = scale_vector(tangent, cos(phi));
	vec = add_vectors(vec, scale_vector(bitangent, sin(phi)));
	vec = scale_vector(vec, sin_theta);
	vec = add_vectors(vec, scale_vector(refl_vec, cos(theta)));
	return (vec);
	vec = scale_vector(refl_vec, -1);
	if (max_theta == 0)
		return (refl_vec);
	while (1)
	{
		if (dot_product(refl_vec, vec) > 1.0f - (max_theta))
			break ;
		theta = (float)random_rangef(0, PI * 2, &state);
		phi = (float)random_rangef(0, PI * 1.0f, &state);
		vec.z = cos(phi);
		vec.x = sqrt(1.0 - pow(vec.z, 2)) * cos(theta);
		vec.y = sqrt(1.0 - pow(vec.z, 2)) * sin(theta);
	}
	return (vec);
	t_3d	vecs[3];
	vecs[0] = scale_vector(vec, cos(PI));
	vecs[1] = scale_vector(cross_product(refl_vec, vec), sin(PI));
	vecs[2] = scale_vector(refl_vec, dot_product(refl_vec, vec));
	vecs[2] = scale_vector(vecs[2], 1.0 - cos(PI));
	vec = add_vectors(vecs[0], vecs[1]);
	vec = add_vectors(vec, vecs[2]);
	
	double	angle = angle_between_vectors((t_3d){0.0, 0.0, -1.0}, refl_vec);
	printf("Angle: %f\n", angle);
	t_mat	vtovmat;
	t_mat	vtovmat2;
	t_mat	mat;
	double	c;
	double	s;
	t_3d	v;

	v = cross_product(vec, refl_vec);
	c = dot_product(vec, refl_vec);
	s = vector_magnitude(v);
	ft_bzero(&mat, sizeof(t_mat));
	mat.m[0][0] = 1;
	mat.m[1][2] = 1;
	mat.m[2][2] = 1;
	mat.m[3][3] = 1;
	vtovmat = init_vtovmatrix(v);
	vtovmat2 = scale_matrix(&vtovmat2, (1.0 - c) / pow(s, 2));
	vtovmat2 = multiply_matrices(&vtovmat, &vtovmat);
	mat = add_matrices(&mat, &vtovmat);
	mat = add_matrices(&mat, &vtovmat2);
	t_3d	out;
	matrix_multip(&vec, &out, &mat);
	/*
	t_3d	orth[2];
	orth[0] = cross_product(vec, refl_vec);
	orth[1] = normalize_vector(cross_product(vec, orth[0]));
	theta = (float)((1.0 - (dot_product(refl_vec, vec) + 1) / 2) * PI * 2);
	vec = scale_vector(vec, cos(theta));
	vec = add_vectors(vec, scale_vector(orth[1], sin(theta)));
	*/
}

t_color	raycast(t_ray *ray, t_scene *scene, int bounces)
{
	t_hit	hit;
	t_color	color;
	t_color	color_refl;
	t_color	color_refr;
	t_ray	shadow_ray;
	t_ray	bounce_ray;

	color.combined = 0x000000; // replace with ambient color defined in param file
	ft_bzero(&hit, sizeof(t_hit));
	if (intersects(ray, scene->object_list, &hit))
	{
		if (hit.object->type == LIGHT || bounces == -1)
			return (hit.color);
//		color.combined = render_with_normals(normal);
		shadow_ray.origin = scale_vector(hit.normal, BIAS);
		shadow_ray.origin = add_vectors(hit.point, shadow_ray.origin);
		if((hit.object->roughness <= 1.0 || hit.object->density < MAX_DENSITY) && bounces > 0)
		{
			if (hit.object->roughness <= 1.0f)
			{
				bounce_ray.forward = reflect_vector(ray->forward, hit.normal);
				bounce_ray.forward = random_vector(bounce_ray.forward, (float)hit.object->roughness);
				bounce_ray.origin = add_vectors(hit.point, scale_vector(hit.normal, BIAS * 1));
				color.combined = light_up(scene->object_list, hit.object->color, shadow_ray, hit.normal);
				color_refl = raycast(&bounce_ray, scene, bounces - 1);
				//color.combined = color_refl.combined;
					color_refl.channel.r = (uint8_t)(color_refl.channel.r * (double)(hit.object->color.channel.r / 255.0));
					color_refl.channel.g = (uint8_t)(color_refl.channel.g * (double)(hit.object->color.channel.g / 255.0));
					color_refl.channel.b = (uint8_t)(color_refl.channel.b * (double)(hit.object->color.channel.b / 255.0));
				color.combined = transition_colors(color_refl.combined, color.combined, (float)hit.object->roughness);
			}
			if (hit.object->density < MAX_DENSITY)
			{
				if (hit.inside == 1)
					bounce_ray.forward = get_refraction_ray(hit.normal, ray->forward, (t_2d){hit.object->density, 1});
				else
					bounce_ray.forward = get_refraction_ray(hit.normal, ray->forward, (t_2d){1, hit.object->density});
				bounce_ray.forward = random_vector(bounce_ray.forward, (float)hit.object->roughness);
				bounce_ray.origin = add_vectors(hit.point, scale_vector(hit.normal, BIAS * -1));
				if (hit.inside == 1)
					bounces -= 1;
				color_refr = raycast(&bounce_ray, scene, bounces);
				color.combined = transition_colors(color_refr.combined, color_refl.combined, 0.0);
			}
		}
	}
	return (color);
}

void	resolution_adjust(t_2i coords, uint32_t color, t_img *img, int res_range)
{
	t_2i	res_coords;

	res_coords.y = 0;
	while (res_coords.y < res_range)
	{
		res_coords.x = 0;
		while (res_coords.x < res_range)
		{
			put_pixel((t_2i){coords.x + res_coords.x, coords.y + res_coords.y}, color, img);
			res_coords.x += 1;
		}
		res_coords.y += 1;
	}
}

void	render_scene(t_env *env, t_img *img, t_scene *scene, int render_mode)
{
	t_2i		coords;
	t_ray		ray;
	t_color		color;
	t_camera	*camera;
	t_2i		*resolution;
	int			i;

	i = 0;
	if (scene->resolution.x == scene->resolution_range.x && scene->resolution.y == scene->resolution_range.y)
	{
		while (i < 100)
		{
			put_pixel((t_2i){(int)xorshift32(&env->state), (int)xorshift32(&env->state)}, 0xFFFFFF, img);
			i++;
		}
		if (scene->accum_resolution.x == scene->resolution_range.x && scene->accum_resolution.y == scene->resolution_range.y)
		{
			scene->accum_resolution.x = scene->resolution_range.x;
			scene->accum_resolution.y = scene->resolution_range.x;
		}
		resolution = &scene->accum_resolution;
	}
	else
	{
		ft_bzero(scene->accum_buffer, SCREEN_X * SCREEN_Y * sizeof(t_3d));
		env->frame_index = 0;
		resolution = &scene->resolution;
		scene->accum_resolution.x = scene->resolution_range.x;
		scene->accum_resolution.y = scene->resolution_range.x;
	}
	camera = scene->camera;
	*camera = init_camera(img->dim.size, camera->ray.origin, camera->ray.forward, camera->fov);
	coords.y = 0;
	while (coords.y < img->dim.size.y - 1)
	{
		if (coords.y % scene->resolution_range.y == resolution->y)
		{
			coords.x = 0;
			while (coords.x < img->dim.size.x - 1)
			{
				if (coords.x % scene->resolution_range.y == resolution->x)
				{
					if (coords.x == img->dim.size.x / 2 && coords.y == img->dim.size.y / 2)
						mid = 1;
					else
						mid = 0;
					ray = get_ray(coords, img, camera);
					ray.object = NULL;
					if (render_mode == -1)
						color = raycast(&ray, scene, -1);
					else
						color = raycast(&ray, scene, BOUNCE_COUNT);
					if (env->sel_ray.object != NULL && env->sel_ray.object == ray.object)
					{
						color.combined = transition_colors(color.combined, ~color.combined & 0x00FFFFFF, 0.25f);
		//				color.combined = transition_colors(color.combined, 0xCD5400, 0.45f);
					}
					if (resolution == &scene->accum_resolution && env->frame_index > 0)
					{
						scene->accum_buffer[coords.y * SCREEN_X + coords.x] = (t_3d){
							(float)(color.channel.r + scene->accum_buffer[coords.y * SCREEN_X + coords.x].x),
							(float)(color.channel.g + scene->accum_buffer[coords.y * SCREEN_X + coords.x].y),
							(float)(color.channel.b + scene->accum_buffer[coords.y * SCREEN_X + coords.x].z)};
						color.channel.r = (uint8_t)(scene->accum_buffer[coords.y * SCREEN_X + coords.x].x / env->frame_index);
						color.channel.g = (uint8_t)(scene->accum_buffer[coords.y * SCREEN_X + coords.x].y / env->frame_index);
						color.channel.b = (uint8_t)(scene->accum_buffer[coords.y * SCREEN_X + coords.x].z / env->frame_index);
					}
					put_pixel(coords, color.combined, img);
					if (scene->resolution.x == scene->resolution.y)
						resolution_adjust(coords, color.combined, img, scene->resolution_range.y - scene->resolution.y);
				}
				coords.x += 1;
			}
		}
		coords.y += 1;
	}
	if (resolution->x == scene->resolution_range.y - 1 && resolution->y == scene->resolution_range.y - 1)
		env->frame_index += 1;
	if (resolution->x < scene->resolution_range.y && resolution->y < scene->resolution_range.y)
		resolution->x += 1;
	if (resolution->x >= scene->resolution_range.y && resolution->y < scene->resolution_range.y)
	{
		resolution->x = scene->resolution_range.x;
		resolution->y += 1;
	}
}
