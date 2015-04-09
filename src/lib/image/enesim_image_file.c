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
#include "enesim_format.h"
#include "enesim_surface.h"
#include "enesim_stream.h"
#include "enesim_image.h"
#include "enesim_image_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/** @cond internal */
#define ENESIM_LOG_DEFAULT enesim_log_image

typedef struct _Enesim_Image_File_Data
{
	Enesim_Image_Callback cb;
	void *user_data;
	Enesim_Stream *data;
} Enesim_Image_File_Data;

static void _enesim_image_file_cb(Enesim_Buffer *b, void *user_data,
		Eina_Bool success, Eina_Error error)
{
	Enesim_Image_File_Data *fdata = user_data;

	fdata->cb(b, fdata->user_data, success, error);
	enesim_stream_unref(fdata->data);
	free(fdata);
}

static const char * _enesim_image_file_get_extension(const char *file)
{
	char *tmp;

	tmp = strrchr(file, '.');
	if (!tmp) return NULL;
	return tmp + 1;
}

static Eina_Bool _file_save_data_get(const char *file, Enesim_Stream **data, const char **mime)
{
	Enesim_Stream *d;
	const char *m;
	const char *ext;

	ext = _enesim_image_file_get_extension(file);
	if (!ext)
	{
		WRN("No extension found for file '%s'", file);
		return EINA_FALSE;
	}

	d = enesim_stream_file_new(file, "wb");
	if (!d)
	{
		WRN("Can not create a stream for file '%s'", file);
		return EINA_FALSE;
	}

	m = enesim_image_mime_extension_from(ext);
	if (!m)
	{
		WRN("Can not find mime for file '%s'", file);
		enesim_stream_unref(d);
		return EINA_FALSE;
	}
	*mime = m;
	*data = d;

	return EINA_TRUE;
}

static Eina_Bool _file_load_data_get(const char *file, Enesim_Stream **data, const char **mime)
{
	Enesim_Stream *d;
	const char *m;

	d = enesim_stream_file_new(file, "rb");
	if (!d) return EINA_FALSE;

	m = enesim_image_mime_data_from(d);
	if (!m)
	{
		enesim_stream_unref(d);
		return EINA_FALSE;
	}
	enesim_stream_reset(d);
	*mime = m;
	*data = d;

	return EINA_TRUE;
}
/** @endcond */
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
/**
 * Gets the information about an image file
 *
 * @param file The image file to load
 * @param w The image width
 * @param h The image height
 * @param sfmt The image original format
 * @param options Any option the provider might require
 * @param[out] err The error in case the file info load fails
 */
EAPI Eina_Bool enesim_image_file_info_get(const char *file, int *w, int *h,
		Enesim_Buffer_Format *sfmt, const char *options,
		Eina_Error *err)
{
	Enesim_Stream *data;
	Eina_Bool ret;
	const char *mime;

	if (!_file_load_data_get(file, &data, &mime))
		return EINA_FALSE;
	ret = enesim_image_info_get(data, mime, w, h, sfmt, options, err);
	enesim_stream_unref(data);
	return ret;
}
/**
 * Load an image synchronously
 *
 * @param file The image file to load
 * @param b The buffer to write the image pixels to. It must not be NULL.
 * @param mpool The mempool that will create the surface in case the surface
 * reference is NULL
 * @param options Any option the emage provider might require
 * @param[out] err The error in case the file load fails
 * @return EINA_TRUE in case the image was loaded correctly. EINA_FALSE if not
 */
EAPI Eina_Bool enesim_image_file_load(const char *file, Enesim_Buffer **b,
		Enesim_Pool *mpool, const char *options, Eina_Error *err)
{
	Enesim_Stream *data;
	Eina_Bool ret;
	const char *mime;

	if (!_file_load_data_get(file, &data, &mime))
		return EINA_FALSE;
	ret = enesim_image_load(data, mime, b, mpool, options, err);
	enesim_stream_unref(data);
	return ret;
}
/**
 * Load an image file asynchronously
 *
 * @param file The image file to load
 * @param b The buffer to write the image pixels to. It must not be NULL.
 * @param mpool The mempool that will create the surface in case the surface
 * reference is NULL
 * @param cb The function that will get called once the load is done
 * @param user_data User provided data
 * @param options Any option the emage provider might require
 */
EAPI void enesim_image_file_load_async(const char *file, Enesim_Buffer *b,
		Enesim_Pool *mpool, Enesim_Image_Callback cb,
		void *user_data, const char *options)
{
	Enesim_Stream *data;
	Enesim_Image_File_Data *fdata;
	const char *mime;

	if (!_file_load_data_get(file, &data, &mime))
	{
		cb(NULL, user_data, EINA_FALSE, ENESIM_IMAGE_ERROR_PROVIDER);
		return;
	}

	fdata = malloc(sizeof(Enesim_Image_File_Data));
	fdata->cb = cb;
	fdata->user_data = user_data;
	fdata->data = data;

	enesim_image_load_async(data, mime, b, mpool, _enesim_image_file_cb, fdata, options);
}
/**
 * Save an image file synchronously
 *
 * @param file The image file to save
 * @param b The surface to read the image pixels from. It must not be NULL.
 * @param options Any option the emage provider might require
 * @param[out] err The error in case the file save fails
 * @return EINA_TRUE in case the image was saved correctly. EINA_FALSE if not
 */
EAPI Eina_Bool enesim_image_file_save(const char *file, Enesim_Buffer *b, const char *options, Eina_Error *err)
{
	Enesim_Stream *data;
	Eina_Bool ret;
	const char *mime;

	if (!_file_save_data_get(file, &data, &mime))
	{
		if (err) *err = ENESIM_IMAGE_ERROR_PROVIDER;
		return EINA_FALSE;
	}
	ret = enesim_image_save(data, mime, b, options, err);
	enesim_stream_unref(data);
	return ret;
}
/**
 * Save an image file asynchronously
 *
 * @param file The image file to save
 * @param b The surface to read the image pixels from. It must not be NULL.
 * @param cb The function that will get called once the save is done
 * @param user_data User provided data
 * @param options Any option the emage provider might require
 *
 */
EAPI void enesim_image_file_save_async(const char *file, Enesim_Buffer *b, Enesim_Image_Callback cb,
		void *user_data, const char *options)
{
	Enesim_Stream *data;
	Enesim_Image_File_Data *fdata;
	const char *mime;

	if (!_file_save_data_get(file, &data, &mime))
	{
		cb(NULL, user_data, EINA_FALSE, ENESIM_IMAGE_ERROR_PROVIDER);
		return;
	}

	fdata = malloc(sizeof(Enesim_Image_File_Data));
	fdata->cb = cb;
	fdata->user_data = user_data;
	fdata->data = data;

	enesim_image_save_async(data, mime, b, _enesim_image_file_cb, fdata, options);
}
