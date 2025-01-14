#ifndef UED_CODES_H
#define UED_CODES_H
/* As of 2008/3/11*/
// If modifying this file, don't forget to update:
// dev\UED\spatial\interpolation\command\UED_spatial_interpolate_main.cpp
#define UED_STD_VC_date 0x1               /* Date*/
#define UED_STD_VC_precip 0x5820001       /* precipitation (liquid equivelent)*/
#define UED_STD_VC_Tmax 0x2870001         /* max. air temperature*/
#define UED_STD_VC_Tmin 0x3870001         /* min. air temperature*/
#define UED_STD_VC_Tavg 0x1870001         /* avg. air temperature*/
#define UED_STD_VC_Tobs 0x870001          /* observed air temperature*/
#define UED_STD_VC_Tdewmax 0x2870002      /* max. dew pt. temperature*/
#define UED_STD_VC_Tdewmin 0x3870002      /* min. dew pt. temperature*/
#define UED_STD_VC_Tdewavg 0x1870002      /* avg. dew pt. temperature*/
#define UED_STD_VC_RHmax 0x2830001        /* max. relative humidity*/
#define UED_STD_VC_RHmin 0x3830001        /* min. relative_humidity*/
#define UED_STD_VC_RHavg 0x1830001        /* avg. relative_humidity*/
#define UED_STD_VC_SH    0x1830011        // specific humidity (daily average?)

#define UED_STD_VC_Srad_total 0x5860201         /* solar radiation*/
#define UED_STD_VC_Srad_ERRONEOUS 0x1860201         /* solar radiation*/

#define UED_STD_VC_SradAvgHrly 0x1860209  /* solar radiation hourly (special case)*/
#define UED_STD_VC_sunhours 0x586020A     /* solar radiation*/
#define UED_STD_VC_isoLWnetrad 0x1860202  /* isothermal longwave net radiation*/
#define UED_STD_VC_rad_net 0x1860203      /* net radiation*/
#define UED_STD_VC_ws 0x1a20001           /* wind speed*/
#define UED_STD_VC_WindDir 0x1a20002      /* wind direction*/
#define UED_STD_VC_ETpot 0x5830003        /* pot.evapotranspiration*/
#define UED_STD_VC_ETact 0x5830002        /* act.evapotranspiration*/
#define UED_STD_VC_ETpan 0x5830402        /* pan evaporation*/
#define UED_STD_VC_PMETref 0x5830004      /* ref.evapotranspiration Penman-Monteith*/
#define UED_STD_VC_PTETref 0x5830005      /* ref.evapotranspiration Priestly-Taylor*/


#define UED_STD_VC_ref_ET_ASCE_EWRI_short_crop  0x5830007
#define UED_STD_VC_ref_ET_ASCE_EWRI_tall_crop   0x5830008


#define UED_STD_VC_HarETref 0x5830006     /* ref.evapotranspiration Hargreaves*/
#define UED_STD_VC_VPDdaytime 0x1830601   /* daytime vapor pressure deficit*/
#define UED_STD_VC_VPDfullday 0x1830601   /* fullday vapor pressure deficit*/
#define UED_STD_VC_VPD 0x2830601          /* vapor pressure deficit*/

#define UED_STD_VC_VP 0x0000000  //NYI    /* vapor pressure */

//WARNING Need to derive this variable code


#define UED_STD_VC_cloud_cover 0x1830010  /* cloud cover*/
#define UED_STD_VC_snow_fall 0x5820002    /* snow fall*/
#define UED_STD_VC_snow_depth 0x820003    /* snow depth*/

#endif

