#include "Enesim.h"

int main(int argc, char **argv)
{
	Enesim_Renderer *r;
	Enesim_Surface *s;
	Enesim_Log *error = NULL;

	enesim_init();
	r = enesim_renderer_rectangle_new();
	enesim_renderer_shape_fill_color_set(r, 0xffffffff);
	/* we dont set any property to force the error */

	s = enesim_surface_new(ENESIM_FORMAT_ARGB8888, 320, 240);

	if (!enesim_renderer_draw(r, s, ENESIM_ROP_FILL, NULL, 0, 0, &error))
	{
		enesim_log_dump(error);
		enesim_log_unref(error);
	}

	enesim_shutdown();

	return 0;
}

