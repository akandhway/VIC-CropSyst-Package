/******************************************************************************
 * @section DESCRIPTION
 *
 * Header file for vic_run routines
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#ifndef VIC_RUN_H
#define VIC_RUN_H

#include <vic_def.h>
double calc_latent_heat_of_sublimation(double temp);
double calc_latent_heat_of_vaporization(double temp);
double calc_outgoing_longwave(double temp, double emis);
double calc_scale_height(double tair, double elevation);
double calc_sensible_heat(double atmos_density, double t1, double t0,
                          double Ra);
#endif
