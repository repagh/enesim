/* LIBARGB - ARGB helper functions
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
#ifndef LIBARGB_ARGB8888_UNPRE_H
#define LIBARGB_ARGB8888_UNPRE_H

#include "libargb_macros.h"
#include "libargb_types.h"

/*============================================================================*
 *                                   Core                                     *
 *============================================================================*/
#if 0
static inline void argb8888_unpre_data_copy(Enesim_Surface_Data *s, Enesim_Surface_Data *d)
{
	d->data.argb8888_unpre.plane0 = s->data.argb8888_unpre.plane0;
}
static inline void argb8888_unpre_data_increment(Enesim_Surface_Data *d, unsigned int len)
{
	d->data.argb8888_unpre.plane0 += len;
}
static inline void argb8888_unpre_data_offset(Enesim_Surface_Data *s, Enesim_Surface_Data *d, unsigned int offset)
{
	d->data.argb8888_unpre.plane0 = s->data.argb8888_unpre.plane0 + offset;
}
static inline unsigned char argb8888_unpre_data_alpha_get(Enesim_Surface_Data *d)
{
	return (*d->data.argb8888_unpre.plane0 >> 24) & 0xff;
}
#endif


#define BLEND_ARGB_256(a, aa, c0, c1) \
 ( ((((0xff0000 - (((c1) >> 8) & 0xff0000)) * (a)) \
   + ((c1) & 0xff000000)) & 0xff000000) + \
   (((((((c0) >> 8) & 0xff) - (((c1) >> 8) & 0xff)) * (aa)) \
   + ((c1) & 0xff00)) & 0xff00) + \
   (((((((c0) & 0xff00ff) - ((c1) & 0xff00ff)) * (aa)) >> 8) \
   + ((c1) & 0xff00ff)) & 0xff00ff) )

static inline uint8_t argb8888_unpre_alpha_get(uint32_t plane0)
{
	return (plane0 >> 24);
}

static inline uint8_t argb8888_unpre_red_get(uint32_t plane0)
{
	return ((plane0 >> 16) & 0xff);
}

static inline uint8_t argb8888_unpre_green_get(uint32_t plane0)
{
	return ((plane0 >> 8) & 0xff);
}

static inline uint8_t argb8888_unpre_blue_get(uint32_t plane0)
{
	return (plane0 & 0xff);
}

static inline void argb8888_unpre_from_components(uint32_t *plane0, uint8_t a, uint8_t r,
		uint8_t g, uint8_t b)
{
	*plane0 = (a << 24) | (r << 16) | (g << 8) | b;
}

static inline void argb8888_unpre_to_components(uint32_t plane0, uint8_t *a, uint8_t *r,
		uint8_t *g, uint8_t *b)
{
	if (a) *a = argb8888_unpre_alpha_get(plane0);
	if (r) *r = argb8888_unpre_red_get(plane0);
	if (g) *g = argb8888_unpre_green_get(plane0);
	if (b) *b = argb8888_unpre_blue_get(plane0);
}

static inline void argb8888_unpre_argb_from(uint32_t *plane0, uint32_t argb)
{
	uint8_t a = argb8888_alpha_get(argb);

	if ((a > 0) && (a < 255))
	{
		uint8_t r, g, b;

		r = argb8888_red_get(argb);
		g = argb8888_green_get(argb);
		b = argb8888_blue_get(argb);

		argb8888_from_components(plane0, a, (r * 255) / a,  (g * 255) / a, (b * 255) / a);
	}
	else
		*plane0 = argb;
}
static inline void argb8888_unpre_argb_to(uint32_t plane0, uint32_t *argb)
{
	uint16_t a = argb8888_unpre_alpha_get(plane0) + 1;

	if (a != 256)
	{
		*argb = (plane0 & 0xff000000) + (((((plane0) >> 8) & 0xff) * a) & 0xff00) +
		(((((plane0) & 0x00ff00ff) * a) >> 8) & 0x00ff00ff);
	}
	else
		*argb = plane0;
}

#endif
