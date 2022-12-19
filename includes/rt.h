/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctrouve <ctrouve@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 17:07:07 by pnoutere          #+#    #+#             */
/*   Updated: 2022/12/19 13:24:32 by ctrouve          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef RT_H
# define RT_H

# include "libft.h"
# include "SDL.h"
# include "dm_2d.h"
# include "dm_bdf_render.h"
# include "dm_vectors.h"
# include <fcntl.h>
# include <math.h>
# include <stdio.h>
# include <time.h>

# define SCREEN_X 2560 / 4
# define SCREEN_Y 1440 / 4
# define T_MAX 100000000.0f
# define BIAS 0.000001
# define IMAGES 10
# define THREADS 32
# define PHOTONS 5000
# define PHOTON_RADIUS 1.0
# define N_CLOSEST_PHOTONS 1
# define CAMERA_BOUNCES 5
# define LIGHT_BOUNCES 3
# define MAX_DENSITY 5
# define MAX_LUMEN 100

# define KEY_A 1
# define KEY_W 2
# define KEY_D 4
# define KEY_S 8
# define KEY_SPACE 16
# define KEY_LSHIFT 32

# ifndef PI
#  define PI 3.141592
# endif

/*DELETE THESE LATER*/
int		mid;
t_3d	tan_temp[2];

/*Typedef enums*/

typedef enum e_obj_type
{
	LIGHT = 0,
	SPHERE = 1,
	PLANE = 2,
	CONE = 3,
	CYLINDER = 4,
	BOX = 5,
	DISC = 6
}				t_obj_type;

/*Typedef structs*/

typedef struct s_2d
{
	double	x;
	double	y;
}				t_2d;

typedef struct s_2f
{
	float	x;
	float	y;
}				t_2f;

typedef struct s_dir
{
	t_3d	forward;
	t_3d	up;
	t_3d	right;
}				t_dir;

typedef struct s_proj
{
	double	z_near;
	double	z_far;
	double	fov;
	double	asp_ratio;
	double	fov_rad;
}				t_proj;

typedef struct s_mat
{
	double	m[4][4];
}				t_mat;

typedef struct s_quadratic
{
	t_3d	w;
	t_3d	h;
	t_3d	subtr_top_bot;
	double	a;
	double	b;
	double	c;
	double	m;
}	t_quadratic;

typedef struct		s_rgba
{
	uint8_t			b;
	uint8_t			g;
	uint8_t			r;
	uint8_t			a;
}					t_rgba;

typedef union		u_color
{
	uint32_t		combined;
	t_rgba			channel;
}					t_color;

typedef struct		s_emission
{
	t_color			color;
	double			intensity;
}					t_emission;

typedef struct s_object
{
	double		axis_length;
	double		radius;
	double		roughness;
	double		density;
	int			lumen;
	int			type;
	t_color		color;
	t_3d		axis;
	t_3d		end;
	t_3d		hit_point;
	t_3d		length;
	t_3d		normal;
	t_3d		origin;
	t_3d		position;
	t_3d		rotation;
	t_2i		rgb_coords;
	t_2i		shade_coords;
}				t_object;

typedef struct s_hit
{
	t_3d		point;
	t_3d		normal;
	t_object	*object;
	t_color		color;
	int			inside;
}				t_hit;

typedef struct s_ray
{
	t_3d		origin;
	t_3d		forward;
	t_3d		hit_point;
	t_object	*object;
	double		distance;
	t_2i		coords;
}				t_ray;

typedef struct s_camera
{
	t_ray		ray;
	t_3d		up;
	t_3d		right;
	double		fov;
	double		aspect_ratio;
}				t_camera;

typedef struct s_ray_hit
{
	t_3d		point;
	uint32_t	color;
}				t_ray_hit;

typedef struct s_cam_hit
{
	t_ray_hit	hit;
	t_ray_hit	photon[N_CLOSEST_PHOTONS];
}				t_cam_hit;

typedef struct s_light_path
{
	t_3d	color;
	t_3d	origin;
	t_3d	normal;
}				t_light_path;

typedef struct s_scene
{
	t_list		*object_list;
	t_list		*light_list;
	t_list		*photon_list[THREADS];
	t_list		*last_photon_node[THREADS];
	t_camera	*camera;
	t_3d		camera_angle;
	t_rgba		ambient_color;
	t_2i		resolution_range;
	t_2i		resolution;
	t_2i		accum_resolution;
	t_3d		*accum_buffer;
	t_cam_hit	*cam_hit_buffer;
	uint32_t	*cam_hit_color;
	uint32_t	*cam_hit_intensity;
	t_light_path	light_path[LIGHT_BOUNCES];
}				t_scene;

typedef struct s_dim
{
	t_2i	size;
	t_2i	start;
}				t_dim;

typedef struct s_img
{
	SDL_Surface		*surface;
	int				bits_per_pixel;
	int				line_length;
	t_dim			dim;
	void			*addr;
	void			(*draw_func)(struct s_img *img, void *param);
}				t_img;

typedef struct s_sdl
{
	SDL_Event		event;
	SDL_Window		*window;
	SDL_Surface		*screen;
}				t_sdl;

typedef struct s_bmptxtr
{
	SDL_Surface	*sidebar;
	SDL_Surface	*slider;
	SDL_Surface	*slide;
}				t_bmptxtr;

typedef struct s_mouse
{
	t_2i	pos;
	t_2i	move;
	uint8_t	state;
}				t_mouse;

typedef struct s_fog
{
	double	color_value;
	double	offset;
	double	offset_value;
	double	trans;
	double	geom_term;
	t_ray	particle_ray;
	t_3d	light_vec;
	double	dist;
	double	pdf;
	double	light_vector_length;
}				t_fog;

typedef struct s_env
{
	int				selected;
	int				sel_element;
	t_sdl			sdl;
	int				height;
	int				width;
	t_img			*img;
	t_scene			*scene;
	t_font			*font;
	t_uint			keymap;
	int				sidebar;
	int				render_mode;
	t_ray			sel_ray;
	t_bmptxtr		bmptxtr;
	t_mouse			mouse;
	double			plot_time;
	int				frame_index;
	uint32_t		state;
	double			photon_cluster_radius;
	t_3d			camera_default;
	double			slider_value;
	char			*file_path;
}				t_env;

t_env	*temp_env;

typedef struct s_multithread
{
	t_env		*env;
	t_img		*img;
	t_2i		*resolution;
	int			nb;
	int			start;
	int			end;
	int 		render_mode;
}				t_multithread;

/*Parser Functions*/

t_list		*load_scene_objects(char *path);
int			add_object(t_list **objects, t_object *object);
int			read_object_info(char *line, t_object *object);
int			transformations(char *line, t_object *object);
int			read_object(t_object *object, char *line);

/*Init functions*/

void		init_main(t_env *env);
t_img		*create_images(size_t count);
t_mat		init_rmatrix_x(double angle_x);
t_mat		init_rmatrix_z(double angle_z);
t_mat		init_rmatrix_y(double angle_y);
t_mat		init_vtovmatrix(t_3d orth_vector);
t_mat		init_pmatrix(t_proj *proj);

/*Keyboard functions*/

void		keyboard_events(t_env *env);
int			keyboard_hold(t_env *env);

/*Mouse functions*/

void		mouse_events(void *param);
int			mouse_main(void *param);
void		left_button_up(void *param);
void		left_button_down(void *param);
void		right_button_up(void *param);
void		right_button_down(void *param);
int			mouse_scroll(void *param);

/*Close and free functions*/

void		close_prog(void *param, char *exit_msg, int exit_code);
t_img		*free_images(t_img *img, size_t count);

/*Image functions*/

t_2i		display_double(t_pxl *pxl, t_2i coords, t_2d flt_prec, t_2i color);
t_2i		display_str(t_pxl *pxl, t_2i coords, char *str, t_2i color);
t_2i		display_int(t_pxl *pxl, t_2i coords, int nbr, t_2i color);
void		main_image(t_img *img, void *param);
void		sidebar_button(t_img *img, void *param);
void		sidebar(t_img *img, void *param);
void		ray_debugger(t_img *img, void *param);
void		render_scene(t_env *env, t_img *img, t_scene *scene, int render_mode);
void		put_images_to_screen(t_env *env);
void		gradual_render(t_img *img, void *param);
void		render_screen(t_env *env);
void		slider(t_img *img, void *param);
void		draw_shade_picker(t_img *img, void *param);
void		draw_rgb_slider(t_img *img, void *param);
t_uint		shade_picker(t_img *img, t_2i *coords, uint32_t color);
t_uint		rgb_slider(t_img *img, t_2i *coords);
double		get_smallest_photon_cluster(t_cam_hit *hit_buffer);

/*Ray tracing functions*/

t_emission		raycast(t_ray *ray, t_scene *scene, int bounces);
uint32_t	shade(t_scene *scene, t_hit *hit);
t_3d		calculate_normal(t_object *object, t_3d hit_point, t_2d t);
t_ray		get_ray(t_2i coords, t_img *img, t_camera *camera);
uint32_t	light_up(t_list *scene, t_color obj_color, t_ray to_light, t_3d normal);
t_3d		get_refraction_ray(t_3d normal, t_3d ray_dir, t_2d index);
double		ray_march(t_2i coords, t_ray ray, t_object *light, t_scene *scene);
/*MOVE TO VECTOR LIBRARY LATER*/
t_3d	random_vector(t_3d refl_vec, float max_theta);
/*Photon mapping functions*/
void	photon_mapping(t_env *env, t_img *img, t_multithread *tab);
void	shoot_photons(t_scene *scene, size_t count, int thread_id);
void	*compare_ray_hits(void *arg);

/* Color operations functions*/

t_rgba		ft_add_rgba(t_rgba c1, t_rgba c2);
t_rgba		ft_make_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
t_rgba		ft_mul_rgba_rgba(t_rgba a, t_rgba b);
t_rgba		ft_mul_rgba(t_rgba c, double t);
t_rgba		ft_lerp_rgba(t_rgba c1, t_rgba c2, double t);
uint		ft_get_color(t_rgba c);

/*Parser functions*/

t_camera	init_camera(t_2i size, t_3d origin, t_3d forward, double fov);
t_camera	*load_scene_camera(char *path);
t_list		*load_scene_objects(char *path);
t_list		*load_scene_lights(char *path);
t_3d		make_vector(double x, double y, double z);
int			read_camera_info(char *line, t_camera *camera);
int			read_object_info(char *line, t_object *object);
int			transformations(char *line, t_object *object);
int			add_object(t_list **objects, t_object *object);

/*Drawing functions*/

void		put_pixel(t_2i coords, t_uint color, void *param);
t_color		get_pixel(t_2i coords, void *param);
void		fill_image(t_img *img, t_uint color);
void		process_image(t_sdl *sdl, t_img *img, int mode, void *param);
void		blit_surface(SDL_Surface *src, t_dim *srcrect, SDL_Surface *dest, t_dim *destrect);

/*Intersect functions*/

int			quadratic_equation(t_quadratic *q, t_2d *t);
int			intersect_plane(t_object *plane, t_ray ray, t_2d *t);
int			intersect_cone(t_object *cone, t_ray ray, t_2d *t);
int			intersect_sphere(t_object *sphere, t_ray ray, t_2d *t);
int			intersect_cylinder(t_object *cylinder, t_ray ray, t_2d *t);
int			intersect_disc(t_object *disc, t_ray ray, t_2d *t);
int			intersect_box(t_object *box, t_ray ray, t_2d *t);
t_2d		intersect_loop(t_ray *ray, t_list *objects, t_hit *hit, int mode);
int			intersects(t_ray *ray, t_list *object_list, t_hit *hit, int mode);

/*Matrix transformation functions*/

t_3d		rotate_point(t_3d point, t_3d rot);
t_3d		get_points(t_img *img, t_3d *xyz, t_3d *rot, t_proj *proj);
t_proj		init_proj(double fov, t_2i *dim, t_2d *z_depth);
void		matrix_multip(t_3d *in, t_3d *out, t_mat *matrix);
t_mat		multiply_matrices(t_mat *m1, t_mat *m2);
t_mat		add_matrices(t_mat *m1, t_mat *m2);
t_mat		scale_matrix(t_mat *m1, double factor);

/*Other functions*/

double		time_since_success(double ammount, int id,int mode);
int			coords_in_area(t_dim dim, t_2i coords);

/*Bidirectional path tracing functions*/

void		trace_light_path(t_scene *scene);
t_3d		trace_eye_path(t_env *env, t_ray *ray, t_scene *scene, int camera_bounces);

/*Saving scene file functions*/

void		save_scene(t_scene *scene, char *path);
char		*get_object_type(t_object *object);
char		*get_object_vector(const t_3d *vector, int axis);
char		*get_object_float(float value);
char		*get_object_hex(unsigned int value);
void		write_objects_to_file(t_list *object_list, int fd);
void		write_camera_to_file(t_camera *camera, int fd);

#endif
