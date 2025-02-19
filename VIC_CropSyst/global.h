/**********************************************************************
                        Global Variables

  NOTE: This file exists because global variables that are shared among
        files via the "extern" statement must be initially declared
        (without the word "extern") ONLY once.  Currently, vicNl_def.h
        is included (via vicNl.h) in every .c file, meaning that any
        declarations in vicNl_def.h end up happening multiple times
        (once per .c file).  Thus, these "extern" variables cannot be
        declared in vicNl_def.h.  This is not a problem for #define
        statements and typedef statements, which is what vicNl_def.h
        is primarily composed of.

  $Id: global.h,v 4.9.2.18 2012/10/25 23:15:58 vicadmin Exp $

  2003-Oct-29 Added version string and removed unused options from
	      optstring.						TJB
  2009-Jun-09 Added definitions of reference landcover types, used
	      mainly for pot_evap computations but also defines the
	      characteristics of bare soil.				TJB
  2012-Jan-16 Removed LINK_DEBUG code					BN
  2012-Nov-13 a few lines added due to cropsyst coupling   KM
**********************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
#include <iostream>

#ifndef CROP_DAILY_OUTPUT_MEMFIRST
    extern std::ofstream debugout;
#endif

/*LML 141105 moved all global variable definations into global.c
*Include:
*global_param,*veg_lib,*crop_lib,*crop_rotation_lib,options,Error,param_set
* *version,*optstring,temps[],flag
*/
#include <VCS_Nl_def.h>
#ifdef VCS_V5
#include <vic_def.h>
#else
#include <vicNl_def.h>
#endif
extern char *version;
extern char *optstring;
#if QUICK_FS
extern double temps[];
#endif
extern int flag;
extern global_param_struct global_param;
extern veg_lib_struct *veg_lib;

#ifdef VIC_CROPSYST_VERSION
#include "VCS_global.h"
#endif

extern option_struct options;
extern Error_struct Error;
extern param_set_struct param_set;

/***** VIC model version *****/
extern char *version;

/* global variables */
extern size_t NR;			/* array index for atmos struct that indicates
                   the model step avarage or sum */
extern size_t NF;			/* array index loop counter limit for atmos
                   struct that indicates the SNOW_STEP values */


  /**************************************************************************
    Define some reference landcover types that always exist regardless
    of the contents of the library (mainly for potential evap calculations):
    Non-natural:
      satsoil = saturated bare soil
      h2osurf = open water surface (deep enough to have albedo of 0.08)
      short   = short reference crop (grass)
      tall    = tall reference crop (alfalfa)
    Natural:
      natveg  = current vegetation
      vegnocr = current vegetation with canopy resistance set to 0
    NOTE: these are external variables, declared in vicNl_def.h.
    NOTE2: bare soil roughness and displacement will be overwritten by the
           values found in the soil parameter file; bare soil wind_h will
	   be overwritten by the value specified in the global param file.
  **************************************************************************/

  /* One element for each non-natural PET type */
  //#ifndef REF_VEG_DEFINATION
  //#define REF_VEG_DEFINATION
  extern char   ref_veg_over[];
  extern double ref_veg_rarc[];
  extern double ref_veg_rmin[];
  extern double ref_veg_lai[];
  extern double ref_veg_albedo[];
  extern double ref_veg_rough[];
  extern double ref_veg_displ[];
  extern double ref_veg_wind_h[];
  extern double ref_veg_RGL[];
  extern double ref_veg_rad_atten[];
  extern double ref_veg_wind_atten[];
  extern double ref_veg_trunk_ratio[];
  /* One element for each PET type (non-natural or natural) */
  extern char ref_veg_ref_crop[];
  //#endif

#endif
