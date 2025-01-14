#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>

static char vcid[] = "$Id: initialize_soil.c,v 4.3.2.5 2011/03/07 05:14:57 vicadmin Exp $";

void initialize_soil (cell_data_struct **cell, 
                      soil_con_struct   *soil_con,
                      veg_con_struct    *veg_con,
                      int                veg_num)
/**********************************************************************
	initialize_soil		Keith Cherkauer		July 31, 1996

  This routine initializes the soil variable arrays for each new
  grid cell.

  modifications:
  11-18-02 Modified to initialize wetland soil moisture.          LCB
  2006-Nov-07 Removed LAKE_MODEL option.				TJB
  2007-Aug-10 Added features for EXCESS_ICE option.			JCA
  2009-Mar-16 Modified to use min_liq (minimum allowable liquid water
	      content) instead of resid_moist.  For unfrozen soil,
	      min_liq = resid_moist.					TJB
  2009-Jul-31 Replaced extra lake/wetland veg tile with reference to
	      veg_con[j].LAKE.						TJB
  2009-Dec-11 Removed min_liq and options.MIN_LIQ.			TJB
  2011-Mar-01 Now initializes more cell data structure terms, including
	      asat and zwt.						TJB
**********************************************************************/
{
  extern option_struct options;
  double tmp_moist[MAX_LAYERS];
  double tmp_runoff = 0;
  for (int veg = 0; veg <= veg_num; veg++) {
    for (int band = 0; band < options.SNOW_BAND; band++) {
      cell_data_struct &cur_cell = cell[veg][band];
      cur_cell.baseflow = 0;
      cur_cell.runoff = 0;
      for (int lindex=0; lindex < options.Nlayer; lindex++) {
        cur_cell.layer[lindex].evap = 0;
#if (VIC_CROPSYST_VERSION>=3)
        cur_cell.layer[lindex].moist = /*190806LML soil_con->Wcr[lindex]*/soil_con->VCS.Field_Capacity[lindex];
#else
        cur_cell.layer[lindex].moist = soil_con->init_moist[lindex];
#endif
        if (cur_cell.layer[lindex].moist > soil_con->max_moist[lindex])
          cur_cell.layer[lindex].moist = soil_con->max_moist[ lindex];
        tmp_moist[lindex] = cur_cell.layer[lindex].moist;
        #if SPATIAL_FROST
        for (int frost_area=0; frost_area < FROST_SUBAREAS; frost_area++) {
          cur_cell.layer[lindex].ice[frost_area] = 0;
        }
        #else
        cur_cell.layer[lindex].ice = 0;
#endif
      }
      //compute_runoff_and_asat(*soil_con, tmp_moist, 0, &(cur_cell.asat), &tmp_runoff);
      compute_runoff_and_asat(soil_con, tmp_moist, 0, &(cur_cell.asat), &tmp_runoff);
      wrap_compute_zwt(soil_con, &(cur_cell));
    }
  }

}
