#include "enesim_example_renderer.h"

/**
 * @example enesim_renderer_path05.c
 * Example usage of a path renderer
 * @image html enesim_renderer_path05.png
 */
Enesim_Renderer * enesim_example_renderer_renderer_get(Enesim_Example_Renderer_Options *options EINA_UNUSED)
{
	Enesim_Renderer *r;
	Enesim_Path *p;

	p = enesim_path_new();
	enesim_path_move_to(p, 20, 20);
	enesim_path_line_to(p, 220, 20);
	enesim_path_line_to(p, 220, 40);
	enesim_path_line_to(p, 20, 40);
	enesim_path_line_to(p, 20, 20);

	r = enesim_renderer_path_new();
	enesim_renderer_path_inner_path_set(r, p);
	enesim_renderer_shape_fill_color_set(r, 0xffff0000);
	enesim_renderer_shape_draw_mode_set(r, ENESIM_RENDERER_SHAPE_DRAW_MODE_FILL);

	return r;
}
