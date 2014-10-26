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
#ifndef ENESIM_COMPOSITOR_PRIVATE_H_
#define ENESIM_COMPOSITOR_PRIVATE_H_

#define ENESIM_COMPOSITOR_SPAN(f) ((Enesim_Compositor_Span)(f))
#define ENESIM_COMPOSITOR_POINT(f) ((Enesim_Compositor_Point)(f))

/**
 * Function to draw a point
 * @param d Destination surface data
 * @param s In case of using the surface as pixel source
 * @param color To draw with a color or as a multiplier color in case of using s
 * @param m In case of using a mask
 */
typedef void (*Enesim_Compositor_Point)(uint32_t *d, uint32_t s,
		Enesim_Color color, uint32_t m);
EAPI Enesim_Compositor_Point enesim_compositor_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt);
/**
 * Function to draw a span
 * @param d Destination surface data
 * @param len The length of the span
 * @param s In case of using the surface as pixel source
 * @param color To draw with a color or as a multiplier color in case of using s
 * @param m In case of using a mask
 */
typedef void (*Enesim_Compositor_Span)(uint32_t *d, uint32_t len, uint32_t *s,
		Enesim_Color color, uint32_t *m);

Enesim_Compositor_Span enesim_compositor_span_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt, Enesim_Channel mchan);

/* TODO remove this */
Enesim_Compositor_Point enesim_compositor_point_get(Enesim_Rop rop,
		Enesim_Format *dfmt, Enesim_Format sfmt, Enesim_Color color,
		Enesim_Format mfmt);

void enesim_compositor_init(void);
void enesim_compositor_shutdown(void);

void enesim_compositor_argb8888_init(void);
void enesim_compositor_argb8888_shutdown(void);

void enesim_compositor_pt_color_register(Enesim_Compositor_Point sp,
		Enesim_Rop rop, Enesim_Format dfmt);
void enesim_compositor_pt_pixel_register(Enesim_Compositor_Point sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);
void enesim_compositor_pt_mask_color_register(Enesim_Compositor_Point sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt);
void enesim_compositor_pt_pixel_mask_register(Enesim_Compositor_Point sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt);
void enesim_compositor_pt_pixel_color_register(Enesim_Compositor_Point sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);

void enesim_compositor_span_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt);
void enesim_compositor_span_pixel_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);
void enesim_compositor_span_mask_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format mfmt,
		Enesim_Channel mchan);
void enesim_compositor_span_pixel_mask_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt,
		Enesim_Format mfmt, Enesim_Channel mchan);
void enesim_compositor_span_pixel_color_register(Enesim_Compositor_Span sp,
		Enesim_Rop rop, Enesim_Format dfmt, Enesim_Format sfmt);

#endif /* ENESIM_COMPOSITOR_H_*/
