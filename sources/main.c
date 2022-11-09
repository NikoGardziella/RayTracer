/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pnoutere <pnoutere@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/27 12:43:48 by dmalesev          #+#    #+#             */
/*   Updated: 2022/11/08 11:39:33 by dmalesev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	del_object(void *content, size_t content_size)
{
	t_object	*object;

	object = (t_object *)content;
	if (object != NULL)
	{
		ft_bzero(content, content_size);
		free(object);
	}
}

void	close_prog(void *param, char *exit_msg, int exit_code)
{
	static t_env	*env;

	if (env == NULL)
	{
		env = param;
		return ;
	}
	// free_images(env->img, IMAGES);
	free_font(&env->font);
	ft_lstdel(&env->scene->objects_list, &del_object);
	ft_lstdel(&env->scene->lights_list, &del_object);
	ft_putendl(exit_msg);
	exit (exit_code);
}

void	sdl_init(t_sdl *sdl)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		SDL_Quit();
	if (SCREEN_X < 200 || SCREEN_X > 2560 || SCREEN_Y < 200 || SCREEN_Y > 1440)
		close_prog(NULL, "Window size specified not in range...", -1);
	sdl->window = SDL_CreateWindow("Ray Tracer", 800, 400, SCREEN_X, SCREEN_Y, 0);
	if (sdl->window == NULL)
		close_prog(NULL, "Creating window failed...", -1);
	sdl->screen = SDL_GetWindowSurface(sdl->window);
	if (sdl->screen == NULL)
		close_prog(NULL, "Creating window surface failed...", -1);
}

int	main(int argc, char **argv)
{
	t_env	env;

	close_prog(&env, "Initializing close program function.", 42);
	ft_bzero(&env, sizeof(t_env));
	env.scene = malloc(sizeof(t_scene));
	if (env.scene == NULL)
		close_prog(NULL, "Malloc env.scene failed...", -1);
	init_main(&env);
	env.scene->camera = load_scene_camera(argv[1]);
	/*PROTECC MALLOC*/
	env.scene->objects_list = load_scene_objects(argv[1]);
	env.scene->lights_list = load_scene_lights(argv[1]);
	env.scene->camera_angle = (t_3d){0.0f, 0.0f, 0.0f};
	sdl_init(&env.sdl);
	SDL_RaiseWindow(env.sdl.window);
	env.img = create_images(IMAGES);
	if (env.img == NULL)
		close_prog(NULL, "Creating images failed...", -1);
	env.mouse_state = 0;
	env.keymap = 0;
	while (1)
	{
		if (env.sdl.event.type == SDL_QUIT || env.sdl.event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		{
			SDL_Quit();
			break ;
		}
		mouse_main(&env);
		keyboard_main(&env);
		if (keyboard_add_vectors(&env) | mouse_move(&env))
		{
			env.scene->resolution.x = env.scene->resolution_range.x;
			env.scene->resolution.y = env.scene->resolution_range.x;
			put_images_to_screen(&env);
		}
		if (env.scene->resolution.y < env.scene->resolution_range.y)
		{
			put_images_to_screen(&env);
		}
		if (env.sdl.event.type == SDL_WINDOWEVENT)
		{
			if (env.sdl.event.window.event == SDL_WINDOWEVENT_EXPOSED)
				put_images_to_screen(&env);
		}
		SDL_PollEvent(&env.sdl.event);
	}
	SDL_DestroyWindow(env.sdl.window);
	(void)argc;
	(void)argv;
	return(0);
}
