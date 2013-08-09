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
#ifndef ENESIM_PERLIN_PRIVATE_H_
#define ENESIM_PERLIN_PRIVATE_H_

Eina_F16p16 enesim_perlin_get(Eina_F16p16 xx, Eina_F16p16 yy,
	unsigned int octaves, Eina_F16p16 *xfreq, Eina_F16p16 *yfreq,
	Eina_F16p16 *ampl);
void enesim_perlin_coeff_set(unsigned int octaves, double persistence,
	double xfreq, double yfreq, double amplitude, Eina_F16p16 *xfreqcoeff,
	Eina_F16p16 *yfreqcoeff, Eina_F16p16 *amplcoeff);

#endif /*ENESIM_PERLIN_H_*/
