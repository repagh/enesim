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
#ifndef ENESIM_RENDERER_TEXT_SPAN_H_
#define ENESIM_RENDERER_TEXT_SPAN_H_

/**
 * @file
 * @ender_group{Enesim_Renderer_Text_Span}
 */

/**
 * @defgroup Enesim_Renderer_Text_Span Text Span
 * @brief Text span renderer @ender_inherits{Enesim_Renderer_Shape}
 *
 * @ender_name{enesim.renderer.text_span}
 * @ingroup Enesim_Renderer
 * @{
 */
EAPI Enesim_Renderer * enesim_renderer_text_span_new(void);
EAPI void enesim_renderer_text_span_text_set(Enesim_Renderer *r, const char *str);
EAPI const char * enesim_renderer_text_span_text_get(Enesim_Renderer *r);
EAPI void enesim_renderer_text_span_direction_set(Enesim_Renderer *r, Enesim_Text_Direction direction);
EAPI Enesim_Text_Direction enesim_renderer_text_span_direction_get(Enesim_Renderer *r);
EAPI Enesim_Text_Buffer * enesim_renderer_text_span_buffer_get(Enesim_Renderer *r);
EAPI Enesim_Text_Buffer * enesim_renderer_text_span_real_buffer_get(Enesim_Renderer *r);
EAPI void enesim_renderer_text_span_real_buffer_set(Enesim_Renderer *r, Enesim_Text_Buffer *b);

EAPI void enesim_renderer_text_span_font_set(Enesim_Renderer *r, Enesim_Text_Font *font);
EAPI Enesim_Text_Font * enesim_renderer_text_span_font_get(Enesim_Renderer *r);

EAPI Eina_Bool enesim_renderer_text_span_glyph_coord_at(Enesim_Renderer *r,
		int x, int y, int *index, int *start, int *end);
EAPI Eina_Bool enesim_renderer_text_span_glyph_index_at(Enesim_Renderer *r,
		int index, int *start, int *end);

/**
 * @}
 */

#endif
