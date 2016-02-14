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
#include "enesim_format.h"
#include "enesim_pool.h"
#include "enesim_buffer.h"
#include "enesim_surface.h"

#include "enesim_buffer_private.h"
#include "enesim_converter_private.h"

/*
 * TODO
 * Add the following parameters to the conversion:
 * angle The rotation angle
 * clip A clipping area on the source buffer
 * x The destination x coordinate to put the buffer
 * y The destination y coordinate to put the buffer
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
typedef Enesim_Converter_2D Enesim_Converter_2D_Lut[ENESIM_BUFFER_FORMAT_LAST][ENESIM_ANGLE_LAST][ENESIM_BUFFER_FORMAT_LAST];

Enesim_Converter_2D_Lut _converters_2d;
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_init(void)
{
	enesim_converter_argb8888_init();
	enesim_converter_xrgb8888_init();
	enesim_converter_rgb888_init();
	enesim_converter_bgr888_init();
	enesim_converter_rgb565_init();
}
void enesim_converter_shutdown(void)
{
}

void enesim_converter_surface_register(Enesim_Converter_2D cnv,
		Enesim_Buffer_Format dfmt, Enesim_Angle angle,
		Enesim_Buffer_Format sfmt)
{
	//printf("registering converter dfmt = %d sfmt = %d\n", dfmt, sfmt);
	_converters_2d[dfmt][angle][sfmt] = cnv;
}

Enesim_Converter_2D enesim_converter_surface_get(Enesim_Buffer_Format dfmt,
		Enesim_Angle angle, Enesim_Buffer_Format sfmt)
{
	return _converters_2d[dfmt][angle][sfmt];
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
