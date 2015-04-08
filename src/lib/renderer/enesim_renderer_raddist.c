/* ENESIM - Drawing Library
 * Copyright (C) 2007-2013 Jorge Luis Zapata
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
#include "enesim_private.h"

#include "enesim_main.h"
#include "enesim_log.h"
#include "enesim_color.h"
#include "enesim_rectangle.h"
#include "enesim_matrix.h"
#include "enesim_pool.h"
#include "enesim_buffer.h"
#include "enesim_format.h"
#include "enesim_surface.h"
#include "enesim_renderer.h"
#include "enesim_renderer_raddist.h"
#include "enesim_object_descriptor.h"
#include "enesim_object_class.h"
#include "enesim_object_instance.h"

#ifdef BUILD_OPENCL
#include "Enesim_OpenCL.h"
#endif

#ifdef BUILD_OPENGL
#include "Enesim_OpenGL.h"
#include "enesim_opengl_private.h"
#endif

#include "enesim_renderer_private.h"
#include "enesim_coord_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
#define ENESIM_RENDERER_RADDIST(o) ENESIM_OBJECT_INSTANCE_CHECK(o,		\
		Enesim_Renderer_Raddist,					\
		enesim_renderer_raddist_descriptor_get())

typedef struct _Enesim_Renderer_Raddist_State
{
	double scale;
	double radius;
	/* the x and y origin of the circle */
	int orx;
	int ory;
} Enesim_Renderer_Raddist_State;

typedef struct _Enesim_Renderer_Raddist
{
	Enesim_Renderer parent;
	/* properties */
	Enesim_Surface *src;
	Enesim_Renderer_Raddist_State current;
	Enesim_Renderer_Raddist_State past;
	/* private */
	uint32_t *ssrc;
	size_t sstride;

	Eina_F16p16 r_inv;
	Eina_Bool changed : 1;
	Eina_Bool src_changed : 1;
} Enesim_Renderer_Raddist;

typedef struct _Enesim_Renderer_Raddist_Class {
	Enesim_Renderer_Class parent;
} Enesim_Renderer_Raddist_Class;

static void _span_identity(Enesim_Renderer *r,
		int x, int y, int len, void *ddata)
{
	Enesim_Renderer_Raddist *thiz;
	uint32_t *dst = ddata;
	uint32_t *end = dst + len;
	double r_inv;
	int sw, sh;
	double ox, oy;

	thiz = ENESIM_RENDERER_RADDIST(r);
	/* setup the parameters */
	enesim_surface_size_get(thiz->src, &sw, &sh);
	/* FIXME move this to the setup */
	r_inv = 1.0f / thiz->current.radius;

	enesim_renderer_origin_get(r, &ox, &oy);
	x -= (int)ox;
	y -= (int)oy;

	x -= thiz->current.orx;
	y -= thiz->current.ory;

	while (dst < end)
	{
		Eina_F16p16 sxx, syy;
		uint32_t p0;
		double rad = hypot(x, y);

		rad = (((thiz->current.scale * (thiz->current.radius - rad)) + rad) * r_inv);
		sxx = eina_f16p16_double_from((rad * x) + thiz->current.orx);
		syy = eina_f16p16_double_from((rad * y) + thiz->current.ory);

		p0 = enesim_coord_sample_good_restrict(thiz->ssrc, thiz->sstride, sw, sh, sxx, syy);

		*dst++ = p0;
		x++;
	}
}
/*----------------------------------------------------------------------------*
 *                      The Enesim's renderer interface                       *
 *----------------------------------------------------------------------------*/
static const char * _raddist_name(Enesim_Renderer *r EINA_UNUSED)
{
	return "raddist";
}

static Eina_Bool _raddist_sw_setup(Enesim_Renderer *r,
		Enesim_Surface *s EINA_UNUSED, Enesim_Rop rop EINA_UNUSED,
		Enesim_Renderer_Sw_Fill *fill, Enesim_Log **l EINA_UNUSED)
{
	Enesim_Renderer_Raddist *thiz;
	Enesim_Matrix_Type type;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (!thiz->src) return EINA_FALSE;

	if (!enesim_surface_map(thiz->src, (void **)&thiz->ssrc, &thiz->sstride))
		return EINA_FALSE;

	type = enesim_renderer_transformation_type_get(r);
	if (type == ENESIM_MATRIX_TYPE_IDENTITY)
		*fill = _span_identity;
	else
		return EINA_FALSE;
	return EINA_TRUE;
}

static void _raddist_sw_cleanup(Enesim_Renderer *r, Enesim_Surface *s EINA_UNUSED)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (thiz->src)
	{
		enesim_surface_unmap(thiz->src, (void **)&thiz->src, EINA_FALSE);
		thiz->ssrc = NULL;
	}
	thiz->changed = EINA_FALSE;
	thiz->src_changed = EINA_FALSE;
	thiz->past = thiz->current;
}

static Eina_Bool _raddist_bounds_get(Enesim_Renderer *r,
		Enesim_Rectangle *rect, Enesim_Log **log EINA_UNUSED)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (!thiz->src)
	{
		rect->x = 0;
		rect->y = 0;
		rect->w = 0;
		rect->h = 0;
		return EINA_FALSE;
	}
	else
	{
		int sw, sh;

		enesim_surface_size_get(thiz->src, &sw, &sh);
		/* translate by the origin */
		enesim_renderer_origin_get(r, &rect->x, &rect->y);
		rect->w = sw;
		rect->h = sh;
		return EINA_TRUE;
	}
}

static Eina_Bool _raddist_has_changed(Enesim_Renderer *r)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (thiz->src_changed) return EINA_TRUE;
	if (!thiz->changed) return EINA_FALSE;

	if (thiz->current.orx != thiz->past.orx)
		return EINA_TRUE;
	if (thiz->current.ory != thiz->past.ory)
		return EINA_TRUE;
	if (thiz->current.scale != thiz->past.scale)
		return EINA_TRUE;
	if (thiz->current.radius != thiz->past.radius)
		return EINA_TRUE;
	return EINA_FALSE;
}

static void _raddist_features_get(Enesim_Renderer *r EINA_UNUSED,
		Enesim_Renderer_Feature *features)
{
	*features = ENESIM_RENDERER_FEATURE_TRANSLATE |
			ENESIM_RENDERER_FEATURE_ARGB8888;
}

/*----------------------------------------------------------------------------*
 *                            Object definition                               *
 *----------------------------------------------------------------------------*/
ENESIM_OBJECT_INSTANCE_BOILERPLATE(ENESIM_RENDERER_DESCRIPTOR,
		Enesim_Renderer_Raddist, Enesim_Renderer_Raddist_Class,
		enesim_renderer_raddist);

static void _enesim_renderer_raddist_class_init(void *k)
{
	Enesim_Renderer_Class *klass;

	klass = ENESIM_RENDERER_CLASS(k);
	klass->base_name_get = _raddist_name;
	klass->bounds_get = _raddist_bounds_get;
	klass->features_get = _raddist_features_get;
	klass->has_changed = _raddist_has_changed;
	klass->sw_setup = _raddist_sw_setup;
	klass->sw_cleanup = _raddist_sw_cleanup;
}

static void _enesim_renderer_raddist_instance_init(void *o EINA_UNUSED)
{
}

static void _enesim_renderer_raddist_instance_deinit(void *o EINA_UNUSED)
{
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @brief Creates a new radial distortion filter renderer
 * @return The renderer
 */
EAPI Enesim_Renderer * enesim_renderer_raddist_new(void)
{
	Enesim_Renderer *r;

	r = ENESIM_OBJECT_INSTANCE_NEW(enesim_renderer_raddist);
	return r;
}

/**
 * @brief Sets the radius of the renderer
 * @ender_prop{radius}
 * @param[in] r The radial distortion renderer
 * @param[in] radius The radius to use
 */
EAPI void enesim_renderer_raddist_radius_set(Enesim_Renderer *r, double radius)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (!radius)
		radius = 1.0;
	thiz->current.radius = radius;
	thiz->changed = EINA_TRUE;
}

/**
 * @brief Gets the radius of the renderer
 * @ender_prop{radius}
 * @param[in] r The radial distortion renderer
 * @return The radius to use
 */
EAPI double enesim_renderer_raddist_radius_get(Enesim_Renderer *r)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	return thiz->current.radius;
}

/**
 * @brief Sets the factor of the renderer
 * @ender_prop{factor}
 * @param[in] r The radial distortion renderer
 * @param[in] factor The factor to use
 */
EAPI void enesim_renderer_raddist_factor_set(Enesim_Renderer *r, double factor)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (factor > 1.0)
		factor = 1.0;
	thiz->current.scale = factor;
	thiz->changed = EINA_TRUE;
}

/**
 * @brief Gets the factor of the renderer
 * @ender_prop{factor}
 * @param[in] r The radial distortion renderer
 * @return The factor to use
 */
EAPI double enesim_renderer_raddist_factor_get(Enesim_Renderer *r)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	return thiz->current.scale;
}

/**
 * @brief Sets the source surface of the renderer
 * @ender_prop{source_surface}
 * @param[in] r The radial distortion renderer
 * @param[in] src The source surface @ender_transfer{full}
 */
EAPI void enesim_renderer_raddist_source_surface_set(Enesim_Renderer *r, Enesim_Surface *src)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	if (thiz->src)
		enesim_surface_unref(thiz->src);
	thiz->src = src;
	thiz->src_changed = EINA_TRUE;
}

/**
 * @brief Gets the source surface of the renderer
 * @ender_prop{source_surface}
 * @param[in] r The radial distortion renderer
 * @return The source surface to use
 */
EAPI Enesim_Surface * enesim_renderer_raddist_source_surface_get(Enesim_Renderer *r)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	return enesim_surface_ref(thiz->src);
}

/**
 * @brief Sets the center in X of the renderer
 * @ender_prop{x}
 * @param[in] r The radial distortion renderer
 * @param[in] ox The center in X of the radial distortion
 */
EAPI void enesim_renderer_raddist_x_set(Enesim_Renderer *r, double ox)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	thiz->current.orx = ox;
	thiz->changed = EINA_TRUE;
}

/**
 * @brief Sets the center in Y of the renderer
 * @ender_prop{y}
 * @param[in] r The radial distortion renderer
 * @param[in] oy The center in Y of the radial distortion
 */
EAPI void enesim_renderer_raddist_y_set(Enesim_Renderer *r, double oy)
{
	Enesim_Renderer_Raddist *thiz;

	thiz = ENESIM_RENDERER_RADDIST(r);
	thiz->current.ory = oy;
	thiz->changed = EINA_TRUE;
}
