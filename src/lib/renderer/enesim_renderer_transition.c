/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2011 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
typedef struct _Enesim_Renderer_Transition {

	int interp;
	struct {
		int x, y;
	} offset;

	struct {
		Enesim_Matrix original;
		double ox, oy;
		double sx, sy;
		Enesim_Renderer *r;
	} r0, r1;
} Enesim_Renderer_Transition;

static inline Enesim_Renderer_Transition * _transition_get(Enesim_Renderer *r)
{
	Enesim_Renderer_Transition *thiz;

	thiz = enesim_renderer_data_get(r);
	return thiz;
}

static void _span_general(Enesim_Renderer *r, int x, int y, unsigned int len, void *ddata)
{
	Enesim_Renderer_Transition *t;
	Enesim_Renderer_Sw_Data *s0data;
	Enesim_Renderer_Sw_Data *s1data;
	Enesim_Renderer *s0, *s1;
	int interp ;
	uint32_t *dst = ddata;
	unsigned int *d = dst, *e = d + len;
	unsigned int *buf;

	t = _transition_get(r);
	s0 = t->r0.r;
	s1 = t->r1.r;
	interp = t->interp;

	s0data = enesim_renderer_backend_data_get(s0, ENESIM_BACKEND_SOFTWARE);
	s1data = enesim_renderer_backend_data_get(s1, ENESIM_BACKEND_SOFTWARE);
	if (interp == 0)
	{
		s0data->fill(s0, x, y, len, d);
		return;
	}
	if (interp == 256)
	{
		s1data->fill(s1, t->offset.x + x, t->offset.y + y, len, d);
		return;
	}
	buf = alloca(len * sizeof(unsigned int));
	s1data->fill(s1, t->offset.x + x, t->offset.y + y, len, buf);
	s0data->fill(s0, x, y, len, d);

	while (d < e)
	{
		uint32_t p0 = *d, p1 = *buf;

		*d++ = argb8888_interp_256(interp, p1, p0);
		buf++;
	}
}
/*----------------------------------------------------------------------------*
 *                      The Enesim's renderer interface                       *
 *----------------------------------------------------------------------------*/
static const char * _transition_name(Enesim_Renderer *r)
{
	return "transition";
}

static Eina_Bool _state_setup(Enesim_Renderer *r,
		const Enesim_Renderer_State *state,
		Enesim_Surface *s,
		Enesim_Renderer_Sw_Fill *fill, Enesim_Error **error)
{
	Enesim_Renderer_Transition *t;

	t = _transition_get(r);
	if (!t || !t->r0.r || !t->r1.r)
		return EINA_FALSE;

	enesim_renderer_relative_set(r, t->r0.r, &t->r0.original, &t->r0.ox, &t->r0.oy, &t->r0.sx, &t->r0.sy);
	if (!enesim_renderer_setup(t->r0.r, s, error))
		goto r0_end;
	enesim_renderer_relative_set(r, t->r1.r, &t->r1.original, &t->r1.ox, &t->r1.oy, &t->r1.sx, &t->r1.sy);
	if (!enesim_renderer_setup(t->r1.r, s, error))
		goto r1_end;

	*fill = _span_general;

	return EINA_TRUE;
r1_end:
	enesim_renderer_relative_unset(r, t->r1.r, &t->r1.original, t->r1.ox, t->r1.oy, t->r1.sx, t->r1.sy);
	enesim_renderer_cleanup(t->r0.r, s);
r0_end:
	enesim_renderer_relative_unset(r, t->r0.r, &t->r0.original, t->r0.ox, t->r0.oy, t->r0.sx, t->r0.sy);
	return EINA_FALSE;
}

static void _state_cleanup(Enesim_Renderer *r, Enesim_Surface *s)
{
	Enesim_Renderer_Transition *t;

	t = _transition_get(r);
	enesim_renderer_cleanup(t->r0.r, s);
	enesim_renderer_relative_unset(r, t->r0.r, &t->r0.original, t->r0.ox, t->r0.oy, t->r0.sx, t->r0.sy);
	enesim_renderer_cleanup(t->r1.r, s);
	enesim_renderer_relative_unset(r, t->r1.r, &t->r1.original, t->r1.ox, t->r1.oy, t->r1.sx, t->r1.sy);
}

static void _boundings(Enesim_Renderer *r, Enesim_Rectangle *rect)
{
	Enesim_Renderer_Transition *trans;
	Enesim_Rectangle r0_rect;
	Enesim_Rectangle r1_rect;

	trans = _transition_get(r);
	rect->x = 0;
	rect->y = 0;
	rect->w = 0;
	rect->h = 0;

	if (!trans->r0.r) return;
	if (!trans->r1.r) return;

	enesim_renderer_boundings(trans->r0.r, &r0_rect);
	enesim_renderer_boundings(trans->r1.r, &r1_rect);

	rect->x = r0_rect.x < r1_rect.x ? r0_rect.x : r1_rect.x;
	rect->y = r0_rect.y < r1_rect.y ? r0_rect.y : r1_rect.y;
	rect->w = r0_rect.w > r1_rect.w ? r0_rect.w : r1_rect.w;
	rect->h = r0_rect.h > r1_rect.h ? r0_rect.h : r1_rect.h;
}

static void _transition_flags(Enesim_Renderer *r, Enesim_Renderer_Flag *flags)
{
	Enesim_Renderer_Transition *thiz;

	thiz = _transition_get(r);
	if (!thiz)
	{
		*flags = 0;
		return;
	}

	*flags = ENESIM_RENDERER_FLAG_AFFINE |
			ENESIM_RENDERER_FLAG_PROJECTIVE |
			ENESIM_RENDERER_FLAG_ARGB8888;
}

static void _free(Enesim_Renderer *r)
{
	Enesim_Renderer_Transition *thiz;

	thiz = _transition_get(r);
	free(thiz);
}

static Enesim_Renderer_Descriptor _descriptor = {
	/* .version = 			*/ ENESIM_RENDERER_API,
	/* .name = 			*/ _transition_name,
	/* .free = 			*/ _free,
	/* .boundings = 		*/ _boundings,
	/* .destination_transform = 	*/ NULL,
	/* .flags = 			*/ _transition_flags,
	/* .is_inside = 		*/ NULL,
	/* .damage = 			*/ NULL,
	/* .has_changed = 		*/ NULL,
	/* .sw_setup = 			*/ _state_setup,
	/* .sw_cleanup = 		*/ _state_cleanup
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Creates a transition renderer
 * @return The new renderer
 */
EAPI Enesim_Renderer * enesim_renderer_transition_new(void)
{
	Enesim_Renderer *r;
	Enesim_Renderer_Transition *thiz;

	thiz = calloc(1, sizeof(Enesim_Renderer_Transition));
	if (!thiz) return NULL;
	r = enesim_renderer_new(&_descriptor, thiz);

	return r;
}
/**
 * Sets the transition level
 * @param[in] r The transition renderer
 * @param[in] level The transition level. A value of 0 will render
 * the source renderer, a value of 1 will render the target renderer
 * and any other value will inteprolate between both renderers
 */
EAPI void enesim_renderer_transition_level_set(Enesim_Renderer *r, double level)
{
	Enesim_Renderer_Transition *t;

	t = _transition_get(r);
	if (level < 0.0000001)
		level = 0;
	if (level > 0.999999)
		level = 1;
	if (t->interp == level)
		return;
	t->interp = 1 + (255 * level);
}
/**
 * Sets the source renderer
 * @param[in] r The transition renderer
 * @param[in] r0 The source renderer
 */
EAPI void enesim_renderer_transition_source_set(Enesim_Renderer *r, Enesim_Renderer *r0)
{
	Enesim_Renderer_Transition *thiz;

	thiz = _transition_get(r);
	if (r0 == r)
		return;
	if (thiz->r0.r == r0)
		return;
	if (thiz->r0.r)
		enesim_renderer_unref(thiz->r0.r);
	thiz->r0.r = r0;
	if (thiz->r0.r)
		thiz->r0.r = enesim_renderer_ref(thiz->r0.r);
}
/**
 * Sets the target renderer
 * @param[in] r The transition renderer
 * @param[in] r1 The target renderer
 */
EAPI void enesim_renderer_transition_target_set(Enesim_Renderer *r, Enesim_Renderer *r1)
{
	Enesim_Renderer_Transition *thiz;

	thiz = _transition_get(r);
	if (r1 == r)
		return;
	if (thiz->r1.r == r1)
		return;
	if (thiz->r1.r)
		enesim_renderer_unref(thiz->r1.r);
	thiz->r1.r = r1;
	if (thiz->r1.r)
		thiz->r1.r = enesim_renderer_ref(thiz->r1.r);
}
/**
 * To be documented
 * FIXME: To be fixed
 * FIXME why do we need this?
 */
EAPI void enesim_renderer_transition_offset_set(Enesim_Renderer *r, int x, int y)
{
	Enesim_Renderer_Transition *t;

	t = _transition_get(r);
	if ((t->offset.x == x) && (t->offset.y == y))
		return;
	t->offset.x = x;
	t->offset.y = y;
}
