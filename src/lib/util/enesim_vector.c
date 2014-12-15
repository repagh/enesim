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

#include <float.h>

#include "enesim_vector_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
static inline void _polygon_update_bounds(Enesim_Polygon *ep, Enesim_Point *p)
{
	if (p->x > ep->xmax) ep->xmax = p->x;
	if (p->y > ep->ymax) ep->ymax = p->y;
	if (p->x < ep->xmin) ep->xmin = p->x;
	if (p->y < ep->ymin) ep->ymin = p->y;
}

static Eina_Bool _points_equal(Enesim_Point *p0, Enesim_Point *p1, double threshold)
{
	Eina_Bool ret = EINA_FALSE;
	double x01;
	double y01;

	x01 = fabs(p0->x - p1->x);
	y01 = fabs(p0->y - p1->y);
	if (x01 < threshold && y01 < threshold)
		ret = EINA_TRUE;
	return ret;
}

static void _polygon_point_append(Enesim_Polygon *thiz, Enesim_Point *p)
{
	thiz->points = eina_list_append(thiz->points, p);
	_polygon_update_bounds(thiz, p);
}

static void _polygon_point_prepend(Enesim_Polygon *thiz, Enesim_Point *p)
{
	thiz->points = eina_list_prepend(thiz->points, p);
	_polygon_update_bounds(thiz, p);
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*----------------------------------------------------------------------------*
 *                              F16p16 Vector                                 *
 *----------------------------------------------------------------------------*/
Eina_Bool enesim_f16p16_vector_setup(Enesim_F16p16_Vector *thiz,
		Enesim_Point *p0, Enesim_Point *p1, double tolerance)
{
	double x0, y0, x1, y1;
	double x01, y01;
	double len;

	x0 = p0->x;
	y0 = p0->y;
	x1 = p1->x;
	y1 = p1->y;

	x01 = x1 - x0;
	y01 = y1 - y0;
	len = hypot(x01, y01);
	if (len < tolerance)
		return EINA_FALSE;

	thiz->a = eina_f16p16_double_from(-y01 / len);
	thiz->b = eina_f16p16_double_from(x01 / len);
	thiz->c = eina_f16p16_double_from(((y1 * x0) - (x1 * y0)) / len);
	thiz->xx0 = eina_f16p16_double_from(x0);
	thiz->yy0 = eina_f16p16_double_from(y0);
	thiz->xx1 = eina_f16p16_double_from(x1);
	thiz->yy1 = eina_f16p16_double_from(y1);

	if ((thiz->yy0 == thiz->yy1) || (thiz->xx0 == thiz->xx1))
		thiz->sgn = 0;
	else
	{
		thiz->sgn = 1;
		if (thiz->yy1 > thiz->yy0)
		{
			if (thiz->xx1 < thiz->xx0)
				thiz->sgn = -1;
		}
		else
		{
			 if (thiz->xx1 > thiz->xx0)
				thiz->sgn = -1;
		}
	}

	if (thiz->xx0 > thiz->xx1)
	{
		Eina_F16p16 xx0 = thiz->xx0;
		thiz->xx0 = thiz->xx1;
		thiz->xx1 = xx0;
	}

	if (thiz->yy0 > thiz->yy1)
	{
		Eina_F16p16 yy0 = thiz->yy0;
		thiz->yy0 = thiz->yy1;
		thiz->yy1 = yy0;
	}
	return EINA_TRUE;
}
/*----------------------------------------------------------------------------*
 *                                 Points                                     *
 *----------------------------------------------------------------------------*/

Enesim_Point * enesim_point_new(void)
{
	Enesim_Point *thiz;

	thiz = calloc(1, sizeof(Enesim_Point));
	return thiz;
}

Enesim_Point * enesim_point_new_from_coords(double x, double y, double z)
{
	Enesim_Point *thiz;

	thiz = enesim_point_new();
	enesim_point_coords_set(thiz, x, y, z);

	return thiz;
}

/*----------------------------------------------------------------------------*
 *                                 Polygon                                    *
 *----------------------------------------------------------------------------*/
Enesim_Polygon * enesim_polygon_new(void)
{
	Enesim_Polygon *p;

	p = calloc(1, sizeof(Enesim_Polygon));
	p->xmax = p->ymax = -DBL_MAX;
	p->xmin = p->ymin = DBL_MAX;
	p->threshold = DBL_EPSILON;
	return p;
}

void enesim_polygon_threshold_set(Enesim_Polygon *p, double threshold)
{
	p->threshold = threshold;
}

void enesim_polygon_point_append_from_coords(Enesim_Polygon *thiz, double x, double y)
{
	Enesim_Point *p;
	Eina_List *l;

	l = eina_list_last(thiz->points);
	if (l)
	{
		Enesim_Point tmp;

		p = eina_list_data_get(l);
		tmp.x = x;
		tmp.y = y;
		tmp.z = 0;
		if (_points_equal(&tmp, p, thiz->threshold))
			return;
	}
	p = enesim_point_new_from_coords(x, y, 0);
	_polygon_point_append(thiz, p);
}

void enesim_polygon_point_prepend_from_coords(Enesim_Polygon *thiz, double x, double y)
{
	Enesim_Point *p;

	if (thiz->points)
	{
		Enesim_Point tmp;

		p = eina_list_data_get(thiz->points);
		tmp.x = x;
		tmp.y = y;
		tmp.z = 0;
		if (_points_equal(&tmp, p, thiz->threshold))
			return;
	}
	p = enesim_point_new_from_coords(x, y, 0);
	_polygon_point_prepend(thiz, p);
}

int enesim_polygon_point_count(Enesim_Polygon *thiz)
{
	return eina_list_count(thiz->points);
}

void enesim_polygon_clear(Enesim_Polygon *thiz)
{
	Enesim_Point *pt;
	EINA_LIST_FREE(thiz->points, pt)
	{
		free(pt);
	}
}

void enesim_polygon_delete(Enesim_Polygon *thiz)
{
	enesim_polygon_clear(thiz);
	free(thiz);
}

void enesim_polygon_dump(Enesim_Polygon *thiz)
{
	Enesim_Point *pt;
	Eina_List *l2;

	printf("New %s polygon\n", thiz->closed ? "closed": "opened");
	EINA_LIST_FOREACH(thiz->points, l2, pt)
	{
		printf("%g %g\n", pt->x, pt->y);
	}
}

void enesim_polygon_merge(Enesim_Polygon *thiz, Enesim_Polygon *to_merge)
{
	Enesim_Point *last, *first;
	Eina_List *l;

	if (!thiz->points) return;
	if (!to_merge->points) return;

	/* check that the last point at thiz is not equal to the first point to merge */
	l = eina_list_last(thiz->points);
	last = eina_list_data_get(l);
	first = eina_list_data_get(to_merge->points);
	if (_points_equal(first, last, thiz->threshold))
	{
		to_merge->points = eina_list_remove(to_merge->points, first);
		free(first);
	}
	thiz->points = eina_list_merge(thiz->points, to_merge->points);
	/* update the bounds */
	if (to_merge->xmax > thiz->xmax) thiz->xmax = to_merge->xmax;
	if (to_merge->ymax > thiz->ymax) thiz->ymax = to_merge->ymax;
	if (to_merge->xmin < thiz->xmin) thiz->xmin = to_merge->xmin;
	if (to_merge->ymin < thiz->ymin) thiz->ymin = to_merge->ymin;

	/* finally remove the to_merge polygon */
	free(to_merge);
}

void enesim_polygon_close(Enesim_Polygon *thiz, Eina_Bool close)
{
	thiz->closed = close;
}

Eina_Bool enesim_polygon_bounds(const Enesim_Polygon *thiz, double *xmin, double *ymin, double *xmax, double *ymax)
{
	if (!thiz->points) return EINA_FALSE;
	*xmin = thiz->xmin;
	*ymin = thiz->ymin;
	*ymax = thiz->ymax;
	*xmax = thiz->xmax;
	return EINA_TRUE;
}

/*----------------------------------------------------------------------------*
 *                                  Figure                                    *
 *----------------------------------------------------------------------------*/
Enesim_Figure * enesim_figure_new(void)
{
	Enesim_Figure *thiz;

	thiz = calloc(1, sizeof(Enesim_Figure));
	return thiz;
}

void enesim_figure_delete(Enesim_Figure *thiz)
{
	enesim_figure_clear(thiz);
	free(thiz);
}

Eina_Bool enesim_figure_bounds(const Enesim_Figure *thiz, double *xmin, double *ymin, double *xmax, double *ymax)
{
	Enesim_Polygon *p;
	Eina_Bool valid = EINA_FALSE;
	Eina_List *l;
	double fxmax;
	double fxmin;
	double fymax;
	double fymin;

	if (!thiz->polygons) return EINA_FALSE;

	fxmax = fymax = -DBL_MAX;
	fxmin = fymin = DBL_MAX;
	EINA_LIST_FOREACH(thiz->polygons, l, p)
	{
		double pxmin;
		double pxmax;
		double pymin;
		double pymax;

		if (!enesim_polygon_bounds(p, &pxmin, &pymin, &pxmax, &pymax))
			continue;

		if (pxmax > fxmax) fxmax = pxmax;
		if (pymax > fymax) fymax = pymax;
		if (pxmin < fxmin) fxmin = pxmin;
		if (pymin < fymin) fymin = pymin;
		valid = EINA_TRUE;
	}
	if (!valid) return EINA_FALSE;

	*xmax = fxmax;
	*xmin = fxmin;
	*ymax = fymax;
	*ymin = fymin;
	return EINA_TRUE;
}

void enesim_figure_polygon_append(Enesim_Figure *thiz, Enesim_Polygon *p)
{
	thiz->polygons = eina_list_append(thiz->polygons, p);
}

void enesim_figure_polygon_remove(Enesim_Figure *thiz, Enesim_Polygon *p)
{
	thiz->polygons = eina_list_remove(thiz->polygons, p);
}

int enesim_figure_polygon_count(Enesim_Figure *thiz)
{
	return eina_list_count(thiz->polygons);
}

void enesim_figure_clear(Enesim_Figure *thiz)
{
	Enesim_Polygon *p;
	EINA_LIST_FREE(thiz->polygons, p)
	{
		enesim_polygon_delete(p);
	}
}

void enesim_figure_dump(Enesim_Figure *f)
{
	Enesim_Polygon *p;
	Eina_List *l1;

	EINA_LIST_FOREACH(f->polygons, l1, p)
	{
		enesim_polygon_dump(p);
	}
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
