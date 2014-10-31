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
#include "libargb.h"

#include "enesim_main.h"
#include "enesim_pool.h"
#include "enesim_buffer.h"

#include "enesim_color_private.h"
#include "enesim_converter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
static void _2d_argb8888_none_argb8888_pre(Enesim_Buffer_Sw_Data *data, uint32_t dw, uint32_t dh,
		Enesim_Buffer_Sw_Data *sdata, uint32_t sw EINA_UNUSED, uint32_t sh EINA_UNUSED)
{
	uint8_t *src = (uint8_t *)sdata->argb8888_pre.plane0;
	uint8_t *dst = (uint8_t *)data->argb8888.plane0;
	size_t dstride = data->argb8888.plane0_stride;
	size_t sstride = sdata->argb8888.plane0_stride;

	while (dh--)
	{
		uint32_t *ddst = (uint32_t *)dst;
		uint32_t *ssrc = (uint32_t *)src;
		uint32_t ddw = dw;
		while (ddw--)
		{
			uint8_t pa;

			pa = (*ssrc >> 24);
			if ((pa > 0) && (pa < 255))
			{
				*ddst = (pa << 24)|
					(((enesim_color_red_get(*ssrc) * 255) / pa) << 16) |
					(((enesim_color_green_get(*ssrc) * 255) / pa) << 8) |
					((enesim_color_blue_get(*ssrc) * 255) / pa);
			}
			else
			{
				*ddst = *ssrc;
			}
			ssrc++;
			ddst++;
		}
		dst += dstride;
		src += sstride;
	}
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void enesim_converter_argb8888_init(void)
{
	enesim_converter_surface_register(
			ENESIM_CONVERTER_2D(_2d_argb8888_none_argb8888_pre),
			ENESIM_BUFFER_FORMAT_ARGB8888,
			ENESIM_ANGLE_0,
			ENESIM_BUFFER_FORMAT_ARGB8888_PRE);
}
/** @endcond */
