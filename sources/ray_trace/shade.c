/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shade.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctrouve <ctrouve@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/02 15:08:05 by ctrouve           #+#    #+#             */
/*   Updated: 2022/12/06 22:48:52 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static double	get_light_level(double t, double lumen, t_3d normal, t_3d dir)
{
	double	light_intensity;
	double	difuse;
	double	light_level;

	light_intensity = t / (lumen * lumen);
	difuse = dot_product(normal, dir);
	light_level = ((difuse + 0.0f) / 1.0f) - light_intensity;
	if (light_level < 0.0)
		light_level = 0.0;
	return (light_level);
}

static t_color	calc_light(t_color final, t_color light, t_color object, double level)
{
	t_3i	color_calc;

	color_calc.x = (int)final.channel.r;
	color_calc.y = (int)final.channel.g;
	color_calc.z = (int)final.channel.b;
	color_calc.x += (int)((double)object.channel.r * level * (double)(light.channel.r / 255.0));
	color_calc.y += (int)((double)object.channel.g * level * (double)(light.channel.g / 255.0));
	color_calc.z += (int)((double)object.channel.b * level * (double)(light.channel.b / 255.0));
	final.channel.r = (uint8_t)ft_min(color_calc.x, 255);
	final.channel.g = (uint8_t)ft_min(color_calc.y, 255);
	final.channel.b = (uint8_t)ft_min(color_calc.z, 255);
	return (final);
}

static t_3d	offset_shadow_ray(t_ray shadow, double radius)
{
	shadow.forward = normalize_vector(shadow.forward);
	shadow.forward = random_vector(shadow.forward, 1.0f);
	shadow.forward = scale_vector(shadow.forward, -radius);
	return (shadow.forward);
}

static int	check_for_refraction(t_ray *shadow, t_list *object_list)
{
	t_hit	hit;
	int		ret;
	
	ft_bzero(&hit, sizeof(t_hit));
	ret = 0;
	while (intersects(shadow, object_list, &hit, 0) && hit.object != NULL && hit.object->density < MAX_DENSITY)
	{
		//shadow->forward = subtract_vectors(hit.object->origin, shadow->origin);
		//shadow->forward = offset_shadow_ray(*shadow, hit.object->radius);
		shadow->forward = get_refraction_ray(hit.normal, shadow->forward, (t_2d){1, hit.object->density});
		shadow->origin = add_vectors(hit.point, scale_vector(hit.normal, BIAS * -1));
		ret = 1;
	}
	return (ret);
}

static void	cast_light_ray(t_object *light, t_list *object_list)
{
	t_ray		light_ray;
	t_hit		hit;

	light_ray.origin = light->origin;
	light_ray.forward = random_vector((t_3d){0.0, 1.0, 0.0}, 2.0f);
	light->color.combined = 0x000000;
	if (intersects(&light_ray, object_list, &hit, 0) == 1)
	{
		light->color.channel.r = (uint8_t)(light->color.channel.r * (double)(hit.object->color.channel.r / 255.0));
		light->color.channel.g = (uint8_t)(light->color.channel.g * (double)(hit.object->color.channel.g / 255.0));
		light->color.channel.b = (uint8_t)(light->color.channel.b * (double)(hit.object->color.channel.b / 255.0));
		light->origin = add_vectors(hit.point, scale_vector(hit.normal, BIAS * 1));
	}
}

uint32_t	light_up(t_list *object_list, t_color obj_color, t_ray shadow, t_3d normal)
{
	t_list		*object_list_start;
	t_object	*object;
	t_color		color;
	float		level;
	double		t;
	int			light_bounces;
	t_object	temp_light;

	color.combined = 0x000000;
	object_list_start = object_list;
	while (object_list != NULL)
	{
		object = (t_object *)object_list->content;
		temp_light = *object;
		if (object->type == LIGHT)
		{
			light_bounces = LIGHT_BOUNCES;
			while (light_bounces > 0)
			{
				shadow.forward = subtract_vectors(temp_light.origin, shadow.origin);
				//shadow.forward = add_vectors(shadow.forward, offset_shadow_ray(shadow, object->radius));
				(void)offset_shadow_ray;
				t = vector_magnitude(shadow.forward);
				shadow.forward = normalize_vector(shadow.forward);
				if (check_for_refraction(&shadow, object_list_start) || t < intersect_loop(&shadow, object_list_start, NULL, 0).x)
				{
					level = (float)get_light_level(t, temp_light.lumen, normal, shadow.forward);
					color = calc_light(color, object->color, obj_color, (double)(level));
				}
				cast_light_ray(&temp_light, object_list_start);
				if (temp_light.color.combined == 0x000000)
					break ;
				light_bounces -= 1;
			}
		}
		object_list = object_list->next;
	}
	return (color.combined);
}
