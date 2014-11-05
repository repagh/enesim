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
#include "enesim_color.h"
#include "enesim_rectangle.h"
#include "enesim_log.h"
#include "enesim_matrix.h"
#include "enesim_renderer.h"
#include "enesim_renderer_importer.h"

#include "enesim_pool_private.h"
#include "enesim_buffer_private.h"
#include "enesim_surface_private.h"

#if BUILD_OPENGL
#include "enesim_log.h"
#include "enesim_rectangle.h"
#include "enesim_matrix.h"
#include "enesim_color.h"
#include "enesim_renderer.h"
#include "Enesim_OpenGL.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
#define ENESIM_LOG_DEFAULT enesim_log_surface

static inline Eina_Bool _format_to_buffer_format(Enesim_Format fmt,
		Enesim_Buffer_Format *buf_fmt)
{
	switch (fmt)
	{
		case ENESIM_FORMAT_ARGB8888:
		*buf_fmt = ENESIM_BUFFER_FORMAT_ARGB8888_PRE;
		return EINA_TRUE;

		case ENESIM_FORMAT_A8:
		*buf_fmt = ENESIM_BUFFER_FORMAT_A8;
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}
}
static inline Eina_Bool _buffer_format_to_format(Enesim_Buffer_Format buf_fmt,
		Enesim_Format *fmt)
{
	switch (buf_fmt)
	{
		case ENESIM_BUFFER_FORMAT_ARGB8888_PRE:
		*fmt = ENESIM_FORMAT_ARGB8888;
		return EINA_TRUE;

		case ENESIM_BUFFER_FORMAT_A8:
		*fmt = ENESIM_FORMAT_A8;
		return EINA_TRUE;

		default:
		return EINA_FALSE;
	}

}

static void _surface_sw_free_func(void *data, void *user_data)
{
	Enesim_Surface *thiz = user_data;
	Enesim_Buffer_Sw_Data *sw_data = data;

	if (thiz->free_func)
	{
		/* given that is an union it does not matter, i suppose ... */
		thiz->free_func(sw_data->argb8888_pre.plane0, thiz->free_func_data);
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void * enesim_surface_backend_data_get(Enesim_Surface *s)
{
	return enesim_buffer_backend_data_get(s->buffer);
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * @brief Create a new surface from a buffer
 * @param[in] buffer The buffer to create a surface from
 * @return The newly created surface
 *
 * This function creates a surface only if the buffer format
 * is of type @ref ENESIM_BUFFER_FORMAT_ARGB8888_PRE or
 * @ref ENESIM_FORMAT_ARGB8888
 */
EAPI Enesim_Surface * enesim_surface_new_buffer_from(Enesim_Buffer *buffer)
{
	Enesim_Surface *s;
	Enesim_Buffer_Format buf_fmt;
	Enesim_Format fmt;

	if (!buffer) return NULL;

	buf_fmt = enesim_buffer_format_get(buffer);
	if (!_buffer_format_to_format(buf_fmt, &fmt))
	{
		Enesim_Renderer *importer;
		int w, h;

		enesim_buffer_size_get(buffer, &w, &h);
		s = enesim_surface_new_pool_from(ENESIM_FORMAT_ARGB8888,
				w, h, enesim_pool_ref(buffer->pool));
		importer = enesim_renderer_importer_new();
		enesim_renderer_importer_buffer_set(importer, buffer);
		enesim_renderer_draw(importer, s, ENESIM_ROP_FILL, NULL, 0, 0, NULL);
		enesim_renderer_unref(importer);
	}
	else
	{
		s = calloc(1, sizeof(Enesim_Surface));
		EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);
		s->format = fmt;
		s->buffer = enesim_buffer_ref(buffer);
		s = enesim_surface_ref(s);
	}
	return s;
}

/**
 * @brief Create a new surface using a pool an user provided data
 * @param[in] fmt The format of the surface
 * @param[in] w The width of the surface
 * @param[in] h The height of the surface
 * @param[in] p The pool to use
 * @param[in] copy In case the data needs to be copied to create the surface
 * or used directly
 * @param[in] stride The stride of the surface
 * @param[in] data The data of the surface pixels
 * @param[in] free_func The function to be called whenever the data of the surface
 * needs to be freed
 * @param[in] free_func_data The private data for the @a free_func callback
 * @return The newly created surface
 */
EAPI Enesim_Surface * enesim_surface_new_pool_and_data_from(Enesim_Format fmt,
		uint32_t w, uint32_t h, Enesim_Pool *p, Eina_Bool copy,
		void *data, size_t stride, Enesim_Buffer_Free free_func,
		void *free_func_data)
{
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Buffer_Format buf_fmt;
	Enesim_Buffer_Sw_Data sw_data;

	if ((!w) || (!h)) return NULL;
	if (!_format_to_buffer_format(fmt, &buf_fmt))
		return NULL;

	switch (fmt)
	{
		case ENESIM_FORMAT_ARGB8888:
		sw_data.argb8888_pre.plane0 = data;
		sw_data.argb8888_pre.plane0_stride = stride ? stride : w * 4;
		break;

		case ENESIM_FORMAT_A8:
		sw_data.a8.plane0 = data;
		sw_data.a8.plane0_stride = stride ? stride : w;
		break;

		default:
		WRN("Unsupported format %d", s->format);
		return EINA_FALSE;
	}

	s = calloc(1, sizeof(Enesim_Surface));
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);
	s->format = fmt;
	s->free_func = free_func;
	s->free_func_data = free_func_data;
	s = enesim_surface_ref(s);

	b = enesim_buffer_new_pool_and_data_from(buf_fmt, w, h, p, copy, &sw_data, _surface_sw_free_func, s);
	if (!b)
	{
		free(s);
		return NULL;
	}
	s->buffer = b;

	return s;
}


/**
 * @brief Create a new surface using an user provided data
 * @param[in] fmt The format of the surface
 * @param[in] w The width of the surface
 * @param[in] h The height of the surface
 * @param[in] copy In case the data needs to be copied to create the surface
 * or used directly
 * @param[in] stride The stride of the surface
 * @param[in] data The data of the surface pixels
 * @param[in] free_func The function to be called whenever the data of the surface
 * needs to be freed
 * @param[in] free_func_data The private data for the @a free_func callback
 * @return The newly created surface
 */
EAPI Enesim_Surface * enesim_surface_new_data_from(Enesim_Format fmt,
		uint32_t w, uint32_t h, Eina_Bool copy, void *data,
		size_t stride, Enesim_Buffer_Free free_func,
		void *free_func_data)
{
	return enesim_surface_new_pool_and_data_from(fmt, w, h, NULL, copy,
			data, stride, free_func, free_func_data);
}

/**
 * @brief Create a new surface using a pool
 * @param[in] f The format of the surface
 * @param[in] w The width of the surface
 * @param[in] h The height of the surface
 * @param[in] p The pool to use
 * @return The newly created surface
 */
EAPI Enesim_Surface * enesim_surface_new_pool_from(Enesim_Format f,
		uint32_t w, uint32_t h, Enesim_Pool *p)
{
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Buffer_Format buf_fmt;

	if ((!w) || (!h)) return NULL;
	if (!_format_to_buffer_format(f, &buf_fmt))
		return NULL;

	b = enesim_buffer_new_pool_from(buf_fmt, w, h, p);
	if (!b) return NULL;

	s = calloc(1, sizeof(Enesim_Surface));
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);
	s->format = f;
	s->buffer = b;
	s = enesim_surface_ref(s);

	return s;
}

/**
 * @brief Create a new surface using the default memory pool
 * @param[in] f The format of the surface
 * @param[in] w The width of the surface
 * @param[in] h The height of the surface
 * @return The newly created surface
 * @see enesim_pool_default_get()
 * @see enesim_pool_default_set()
 */
EAPI Enesim_Surface * enesim_surface_new(Enesim_Format f, uint32_t w, uint32_t h)
{
	Enesim_Surface *s;

	s = enesim_surface_new_pool_from(f, w, h, NULL);

	return s;
}

#if BUILD_OPENGL
/**
 * @brief Create a new OpenGL based surface from a texture
 * @param[in] fmt The format of the surface
 * @param[in] w The width of the surface
 * @param[in] h The height of the surface
 * @param[in] texture The texture id to use
 * @return The newly created surface
 */
EAPI Enesim_Surface * enesim_surface_new_opengl_data_from(Enesim_Format fmt,
		uint32_t w, uint32_t h,
		GLuint texture)
{
	Enesim_Surface *s;
	Enesim_Buffer *b;
	Enesim_Buffer_Format buf_fmt;
	GLuint textures[1];

	if ((!w) || (!h)) return NULL;
	if (!_format_to_buffer_format(fmt, &buf_fmt))
		return NULL;

	textures[0] = texture;
	b = enesim_buffer_new_opengl_data_from(buf_fmt, w, h, textures, 1);
	if (!b) return NULL;

	s = calloc(1, sizeof(Enesim_Surface));
	EINA_MAGIC_SET(s, ENESIM_MAGIC_SURFACE);
	s->format = fmt;
	s->buffer = b;
	s = enesim_surface_ref(s);

	return s;
}

/**
 * @brief Gets the OpenGL data associated with a surface
 * @param[in] thiz The surface to get the data from
 * @return The OpenGL data associated with a surface
 */
EAPI const Enesim_Buffer_OpenGL_Data * enesim_surface_opengl_data_get(Enesim_Surface *thiz)
{
	if (!thiz) return NULL;
	return enesim_buffer_backend_data_get(thiz->buffer);
}

#endif

/**
 * @brief Gets the size of a surface
 * @param[in] s The surface to get the size from
 * @param[out] w The width of the surface
 * @param[out] h The height of the surface
 */
EAPI void enesim_surface_size_get(const Enesim_Surface *s, int *w, int *h)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	if (w) *w = s->buffer->w;
	if (h) *h = s->buffer->h;
}

/**
 * @brief Gets the format of a surface
 * @param[in] s The surface to get the format from
 * @return The format of the surface
 */
EAPI Enesim_Format enesim_surface_format_get(const Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	return s->format;
}

/**
 * @brief Gets the backend of a surface
 * @param[in] s The surface to get the backend from
 * @return The backend of the surface
 */
EAPI Enesim_Backend enesim_surface_backend_get(const Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	return enesim_buffer_backend_get(s->buffer);
}

/**
 * @brief Gets the pool of a surface
 * @param[in] s The surface to get the pool from
 * @return The pool of the surface
 */
EAPI Enesim_Pool * enesim_surface_pool_get(Enesim_Surface *s)
{
	return enesim_buffer_pool_get(s->buffer);
}

/**
 * @brief Increase the reference counter of a surface
 * @param[in] s The surface
 * @return The input parameter @a s for programming convenience
 */
EAPI Enesim_Surface * enesim_surface_ref(Enesim_Surface *s)
{
	if (!s) return NULL;
	ENESIM_MAGIC_CHECK_SURFACE(s);
	s->ref++;
	return s;
}

/**
 * @brief Decrease the reference counter of a surface
 * @param[in] s The surface
 */
EAPI void enesim_surface_unref(Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	s->ref--;
	if (!s->ref)
	{
		DBG("Unreffing surface %p with buffer %p", s, s->buffer);
		enesim_buffer_unref(s->buffer);
		free(s);
	}
}

/**
 * Gets the data associated with a software based surface
 * @param[in] s The surface to get the data from
 * @param[out] data The pointer to store the surface data
 * @param[out] stride The stride of the surface
 * @return EINA_TRUE if sucessfull, EINA_FALSE otherwise
 */
EAPI Eina_Bool enesim_surface_sw_data_get(Enesim_Surface *s, void **data, size_t *stride)
{
	Enesim_Buffer_Sw_Data sw_data;

	if (!data) return EINA_FALSE;
	ENESIM_MAGIC_CHECK_SURFACE(s);
	if (!enesim_buffer_sw_data_get(s->buffer, &sw_data))
	{
		WRN("Impossible to get the buffer data");
		return EINA_FALSE;
	}

	switch (s->format)
	{
		case ENESIM_FORMAT_ARGB8888:
		*data = sw_data.argb8888_pre.plane0;
		if (stride) *stride = sw_data.argb8888_pre.plane0_stride;
		break;

		case ENESIM_FORMAT_A8:
		*data = sw_data.a8.plane0;
		if (stride) *stride = sw_data.a8.plane0_stride;
		break;

		default:
		WRN("Unsupported format %d", s->format);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

/**
 * Maps the surface into user space memory
 * @param[in] s The surface to map
 * @param[out] data The pointer to store the surface data
 * @param[out] stride The stride of the surface
 * @return EINA_TRUE if sucessfull, EINA_FALSE otherwise
 */
EAPI Eina_Bool enesim_surface_map(const Enesim_Surface *s, void **data, size_t *stride)
{
	Enesim_Buffer_Sw_Data sw_data;

	if (!data) return EINA_FALSE;
	if (!enesim_buffer_map(s->buffer, &sw_data))
	{
		WRN("Impossible to map the buffer data");
		return EINA_FALSE;
	}

	switch (s->format)
	{
		case ENESIM_FORMAT_ARGB8888:
		*data = sw_data.argb8888_pre.plane0;
		if (stride) *stride = sw_data.argb8888_pre.plane0_stride;
		break;

		case ENESIM_FORMAT_A8:
		*data = sw_data.a8.plane0;
		if (stride) *stride = sw_data.a8.plane0_stride;
		break;

		default:
		WRN("Unsupported format %d", s->format);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

/**
 * @brief Unmaps the surface
 * Call this function when the mapped data of a surface is no longer
 * needed.
 * @param[in] s The surface to unmap
 * @param[in] data The pointer where the surface data is mapped
 * @param[in] written EINA_TRUE in case the mapped data has been written, EINA_FALSE otherwise
 * @return EINA_TRUE if sucessfull, EINA_FALSE otherwise
 */
EAPI Eina_Bool enesim_surface_unmap(const Enesim_Surface *s, void *data, Eina_Bool written)
{
	Enesim_Buffer_Sw_Data sw_data;

	if (!data) return EINA_FALSE;

	switch (s->format)
	{
		case ENESIM_FORMAT_ARGB8888:
		sw_data.argb8888_pre.plane0 = data;
		break;

		case ENESIM_FORMAT_A8:
		sw_data.a8.plane0 = data;
		break;

		default:
		WRN("Unsupported format %d", s->format);
		return EINA_FALSE;
	}
	if (!enesim_buffer_unmap(s->buffer, &sw_data, written))
	{
		WRN("Impossible to unmap the buffer data");
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

/**
 * @brief Gets the buffer associated with a surface
 * @param[in] s The surface to get the buffer from
 * @return The buffer associated with the surface. Call @ref enesim_buffer_unref()
 * after usage.
 */
EAPI Enesim_Buffer * enesim_surface_buffer_get(Enesim_Surface *s)
{
	return enesim_buffer_ref(s->buffer);
}

/**
 * Store a private data pointer into the surface
 */
EAPI void enesim_surface_private_set(Enesim_Surface *s, void *data)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	s->user = data;
}

/**
 * Retrieve the private data pointer from the surface
 */
EAPI void * enesim_surface_private_get(Enesim_Surface *s)
{
	ENESIM_MAGIC_CHECK_SURFACE(s);
	return s->user;
}

/**
 * @brief Locks a surface
 * @param[in] s The surface to lock
 * @param[in] write Lock for writing
 */
EAPI void enesim_surface_lock(Enesim_Surface *s, Eina_Bool write)
{
	enesim_buffer_lock(s->buffer, write);
}

/**
 * @brief Unlocks a surface
 * @param[in] s The surface to unlock
 */
EAPI void enesim_surface_unlock(Enesim_Surface *s)
{
	enesim_buffer_unlock(s->buffer);
}
