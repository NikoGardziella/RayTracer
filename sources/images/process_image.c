/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_image.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmalesev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/01 11:21:22 by dmalesev          #+#    #+#             */
/*   Updated: 2022/11/04 09:30:36 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	put_image_to_screen_surface(t_img *img, SDL_Surface *screen)
{
	uint8_t	*dest;
	int		offset;
	int		y;

	y = 0;
	offset = img->dim.start.y * screen->pitch;
	offset += img->dim.start.x * screen->format->BytesPerPixel;
	while (y < img->dim.size.y)
	{
		dest = ((uint8_t *)screen->pixels + offset + (screen->pitch * y));
		dest = (uint8_t *)ft_memcpy(dest, img->surface->pixels + img->surface->pitch * y, (size_t)img->surface->pitch);
		y += 1;
	}
}

void	blit_surface(SDL_Surface *src, t_dim *srcrect, SDL_Surface *dest, t_dim *destrect)
{
	uint32_t	offset;
	uint32_t	*srcaddr;
	uint32_t	*destaddr;
	t_2d		step;
	t_2d		coords;

	if (src == NULL || dest == NULL)
		return ;
	if (srcrect == NULL)
		srcrect = &(t_dim){(t_2i){src->w, src->h}, (t_2i){0, 0}, (t_2i){0, 0}};
	if (destrect == NULL)
		destrect = &(t_dim){(t_2i){dest->w, dest->h}, (t_2i){0, 0}, (t_2i){0, 0}};
	srcrect->size.x = ft_max(srcrect->size.x, 1);
	srcrect->size.y = ft_max(srcrect->size.y, 1);
	srcrect->size.x = ft_min(srcrect->size.x, src->w);
	srcrect->size.y = ft_min(srcrect->size.y, src->h);
	srcrect->start.x = ft_max(srcrect->start.x, 0);
	srcrect->start.y = ft_max(srcrect->start.y, 0);
	srcrect->start.x = ft_min(srcrect->start.x, src->w);
	srcrect->start.y = ft_min(srcrect->start.y, src->h);
	destrect->size.x = ft_max(destrect->size.x, 1);
	destrect->size.y = ft_max(destrect->size.y, 1);
	step.x = (double)srcrect->size.x / (double)destrect->size.x;
	step.y = (double)srcrect->size.y / (double)destrect->size.y;
	coords.y = 0;
	while ((double)((coords.y + srcrect->start.y) * step.y) < (double)srcrect->size.y)
	{
		coords.x = 0;
		while ((double)((coords.x + srcrect->start.x) * step.x) < (double)srcrect->size.x)
		{
			if (((int)coords.x + (int)destrect->start.x >= (int)(dest->w)))
				break ;
			offset = (uint32_t)((int)((coords.y + srcrect->start.y) * step.y) * src->pitch);
			offset += (uint32_t)((int)((coords.x + srcrect->start.x) * step.x) * src->format->BytesPerPixel);
			//offset += (uint32_t)(srcrect->start.x * src->format->BytesPerPixel + srcrect->start.y * src->pitch);
			srcaddr = (src->pixels + offset);

			offset = (uint32_t)((coords.y + destrect->start.y) * dest->pitch);
			offset += (uint32_t)((coords.x + destrect->start.x) * dest->format->BytesPerPixel);
			//offset += (uint32_t)(destrect->start.x * dest->format->BytesPerPixel + destrect->start.y * dest->pitch);
			destaddr = (dest->pixels + offset);

			if (destaddr - (uint32_t *)dest->pixels >= 0x0 && destaddr - (uint32_t *)dest->pixels < dest->w * dest->h)
			{
				if (srcaddr - (uint32_t *)src->pixels >= 0x0 && srcaddr - (uint32_t *)src->pixels < src->w * src->h)
				{
					if (((int)coords.x + (int)destrect->start.x >= 0))
						*destaddr = *srcaddr;
				}
			}
			coords.x += 1;
		}
		coords.y += 1;
	}
}

void	process_image(t_sdl *sdl, t_img *img, int mode, void *param)
{
	SDL_LockSurface(sdl->screen);
	if ((mode & 1) != 1)
		ft_bzero(img->surface->pixels, (size_t)(img->surface->h * img->surface->pitch));
	if (img->draw_func != NULL)
		img->draw_func(img, param);
	if ((mode & 2) == 2)
	{
		//blit_surface_fast(img->surface, NULL, sdl->screen, &img->dim);
		put_image_to_screen_surface(img, sdl->screen);
	}
	if ((mode & 4) == 4)
	{
		SDL_UpdateWindowSurface(sdl->window);
	}
	SDL_UnlockSurface(sdl->screen);
}
