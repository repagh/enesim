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
#include "enesim_pool.h"
#include "enesim_buffer.h"

#include "enesim_converter_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
static void _2d_rgb565_none_argb8888_pre(Enesim_Buffer_Sw_Data *data, uint32_t dw, uint32_t dh,
		Enesim_Buffer_Sw_Data *sdata, uint32_t sw EINA_UNUSED, uint32_t sh EINA_UNUSED)
{
	uint16_t *dst = data->rgb565.plane0;
	uint32_t *src = sdata->argb8888_pre.plane0;
	size_t dstride = data->rgb565.plane0_stride;
	size_t sstride = data->argb8888_pre.plane0_stride / 4;

	while (dh--)
	{
		uint16_t *ddst = dst;
		uint32_t *ssrc = src;
		uint32_t ddw = dw;
		while (ddw--)
		{
			*dst = ((*src & 0xf80000) >> 8) | ((*src & 0xfc00) >> 5)
					| ((*src & 0xf8) >> 3);
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
void enesim_converter_rgb565_init(void)
{
	/* TODO check if the cpu is the host */
	enesim_converter_surface_register(
			ENESIM_CONVERTER_2D(_2d_rgb565_none_argb8888_pre),
			ENESIM_BUFFER_FORMAT_RGB565,
			ENESIM_ANGLE_NONE,
			ENESIM_BUFFER_FORMAT_ARGB8888_PRE);
}
/** @endcond */
