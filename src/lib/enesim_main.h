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

#ifndef ENESIM_MAIN_H_
#define ENESIM_MAIN_H_

/**
 * @file
 * @ender_group{Enesim_Basic}
 * @ender_group{Enesim_Main}
 */

/**
 * @defgroup Enesim_Basic Basic types
 * @brief Basic types
 * @{
 */

/**
 * Angle enumeration
 */
typedef enum _Enesim_Angle
{
	ENESIM_ANGLE_0, /**< 0 degrees angle */
	ENESIM_ANGLE_90, /**< 90 degrees angle CW */
	ENESIM_ANGLE_180, /**< 180 degrees angle CW */
	ENESIM_ANGLE_270, /**< 270 degrees angle CW */
} Enesim_Angle;

#define ENESIM_ANGLE_LAST (ENESIM_ANGLE_270 + 1)

/**
 * Raster operations at pixel level
 */
typedef enum _Enesim_Rop
{
	ENESIM_ROP_BLEND, /**< D = S + D(1 - Sa) */
	ENESIM_ROP_FILL, /**< D = S */
} Enesim_Rop;

#define ENESIM_ROP_LAST (ENESIM_ROP_FILL + 1)

/**
 * Quality values
 */
typedef enum _Enesim_Quality
{
	ENESIM_QUALITY_BEST, /**< Best quality */
	ENESIM_QUALITY_GOOD, /**< Good quality */
	ENESIM_QUALITY_FAST, /**< Lower quality, fastest */
} Enesim_Quality;

#define ENESIM_QUALITY_LAST (ENESIM_QUALITY_FAST + 1)

/**
 * Priorities
 */
typedef enum _Enesim_Priority
{
	ENESIM_PRIORITY_NONE = 0,
	ENESIM_PRIORITY_MARGINAL = 64,
	ENESIM_PRIORITY_SECONDARY = 128,
	ENESIM_PRIORITY_PRIMARY = 256,
} Enesim_Priority;

/**
 * RGBA Channels
 */
typedef enum _Enesim_Channel
{
	ENESIM_CHANNEL_RED, /**< Red channel */
	ENESIM_CHANNEL_GREEN, /**< Green channel */
	ENESIM_CHANNEL_BLUE, /**< Blue channel */
	ENESIM_CHANNEL_ALPHA, /**< Alpha channel */
	ENESIM_CHANNEL_LUMINANCE, /**< Luminance to alpha, with 0.2125, 0.7154 and 0.0721 coefficients */
} Enesim_Channel;

/**< The number of channels */
#define ENESIM_CHANNEL_LAST (ENESIM_CHANNEL_LUMINANCE + 1)


/**
 * Alpha hints
 * @ender_name{enesim.aplha_hint}
 */
typedef enum _Enesim_Alpha_Hint
{
	ENESIM_ALPHA_HINT_NORMAL, /**< Alpha can be in the whole range */
	ENESIM_ALPHA_HINT_SPARSE, /**< Alpha is sparsed only, that is or 0 or 255 */
	ENESIM_ALPHA_HINT_OPAQUE, /**< Alpha is always 255 */
} Enesim_Alpha_Hint;

/**< The number of alpha hints */
#define ENESIM_ALPHA_HINT_LAST (ENESIM_ALPHA_HINT_OPAQUE + 1)

/**
 * Repeat modes
 * @ender_name{enesim.repeat_mode}
 */
typedef enum _Enesim_Repeat_Mode
{
	ENESIM_REPEAT_MODE_RESTRICT,
	ENESIM_REPEAT_MODE_PAD,
	ENESIM_REPEAT_MODE_REFLECT,
	ENESIM_REPEAT_MODE_REPEAT,
} Enesim_Repeat_Mode;

#define ENESIM_REPEAT_MODE_LAST (ENESIM_REPEAT_MODE_REPEAT + 1)

/**
 * The backend used for drawing
 */
typedef enum _Enesim_Backend
{
	ENESIM_BACKEND_INVALID, /**< Invalid backend */
	ENESIM_BACKEND_SOFTWARE, /**< Software based backend */
	ENESIM_BACKEND_OPENCL, /**< OpenCL based backend (not working) */
	ENESIM_BACKEND_OPENGL, /**< OpenGL based backend (experimental) */
} Enesim_Backend;

#define ENESIM_BACKEND_LAST (ENESIM_BACKEND_OPENGL + 1)

/**
 * @}
 * @defgroup Enesim_Main Main
 * @brief Main functions
 * @{
 */

EAPI int enesim_init(void);
EAPI int enesim_shutdown(void);
EAPI void enesim_version_get(unsigned int *major, unsigned int *minor, unsigned int *micro);

/** @} */

#endif /*ENESIM_MAIN_H_*/
