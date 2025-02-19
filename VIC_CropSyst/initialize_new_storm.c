#include <stdio.h>
#include <stdlib.h>
#include <vicNl.h>
#include <math.h>

static char vcid[] = "$Id: initialize_new_storm.c,v 5.1.2.1 2007/04/18 20:18:59 vicadmin Exp $";

int  initialize_new_storm(cell_data_struct ***cell,
              veg_var_struct   ***veg_var,
              int                 veg,
              int                 Nveg,
              int                 rec,
              double              old_mu,
              double              new_mu) {
/**********************************************************************
  initialize_new_storm.c    Keith Cherkauer        June 24, 1998

  This subroutine averages all soil moisture components before the
  start of a new storm, so that the model loses memory of the previous
  storm's location (which was never really known).

  Modifications:
  07-13-98 modified to redistribute Wdew within all defined
           elevation bands                                         KAC
  6-8-2000 modified to work with spatially distributed frost       KAC
  2007-Apr-04 Modified to return to main subroutine on cell error GCT/KAC
**********************************************************************/
  extern option_struct options;
  unsigned char error;
#if SPATIAL_FROST
  int           frost_area;
#endif
  /** Redistribute Soil Moisture **/
  for (int layer = 0; layer < options.Nlayer; layer++) {
    for (int band = 0; band < options.SNOW_BAND; band++) {
      double temp_wet = cell[WET][veg][band].layer[layer].moist;
      double temp_dry = cell[DRY][veg][band].layer[layer].moist;
      error = average_moisture_for_storm(&temp_wet, &temp_dry, old_mu, new_mu);
      if(error) {
        fprintf(stderr,"moist does not balance before new storm: %f -> %f record %i\n",
          cell[WET][veg][band].layer[layer].moist * new_mu
          + cell[DRY][veg][band].layer[layer].moist * (1. - new_mu),
          temp_wet + temp_dry, rec);
        return( ERROR );
      }
      cell[WET][veg][band].layer[layer].moist = temp_wet;
      cell[DRY][veg][band].layer[layer].moist = temp_dry;
      
      #if SPATIAL_FROST
      for ( frost_area = 0; frost_area < FROST_SUBAREAS; frost_area++ ) {
        temp_wet = cell[WET][veg][band].layer[layer].ice[frost_area];
        temp_dry = cell[DRY][veg][band].layer[layer].ice[frost_area];
      #else
        temp_wet = cell[WET][veg][band].layer[layer].ice;
        temp_dry = cell[DRY][veg][band].layer[layer].ice;
      #endif
      error = average_moisture_for_storm(&temp_wet, &temp_dry, old_mu,
                       new_mu);
      if(error) {
        #if SPATIAL_FROST
        fprintf(stderr,"ice does not balance before new storm: %f -> %f record %i\n",
          cell[WET][veg][band].layer[layer].ice[frost_area] * new_mu
          + cell[DRY][veg][band].layer[layer].ice[frost_area]
          * (1. - new_mu), temp_wet + temp_dry, rec);
        #else
        fprintf(stderr,"ice does not balance before new storm: %f -> %f record %i\n",
          cell[WET][veg][band].layer[layer].ice * new_mu
          + cell[DRY][veg][band].layer[layer].ice * (1. - new_mu),
          temp_wet + temp_dry, rec);
        #endif
        return( ERROR );
      }
      #if SPATIAL_FROST
      cell[WET][veg][band].layer[layer].ice[frost_area] = temp_wet;
      cell[DRY][veg][band].layer[layer].ice[frost_area] = temp_dry;
      }
#else
    cell[WET][veg][band].layer[layer].ice = temp_wet;
    cell[DRY][veg][band].layer[layer].ice = temp_dry;
#endif 
    }
  }
  /****************************************
    Redistribute Stored Water in Vegetation
  ****************************************/
  if (veg < Nveg) {
    for (int band = 0; band < options.SNOW_BAND; band++) {
      double temp_wet = veg_var[WET][veg][band].Wdew;
      double temp_dry = veg_var[DRY][veg][band].Wdew;
      error = average_moisture_for_storm(&temp_wet, &temp_dry, old_mu, new_mu);
      if(error) {
    fprintf(stderr,"Wdew does not balance before new storm: %f -> %f record %i\n",
        veg_var[WET][veg][band].Wdew * new_mu
        + veg_var[DRY][veg][band].Wdew * (1. - new_mu),
        temp_wet + temp_dry, rec);
    return( ERROR );
      }
      veg_var[WET][veg][band].Wdew = temp_wet;
      veg_var[DRY][veg][band].Wdew = temp_dry;
    }
  }
  return (0);
}
//______________________________________________________________________________
unsigned char average_moisture_for_storm(double *wet_value,
                     double *dry_value,
                     double  old_mu,
                     double  new_mu) {
/**********************************************************************
  This subroutine averages total soil moisture between the wet and dry
  fractions of the soil column
**********************************************************************/

  unsigned char error;
  double temp_wet;
  double temp_dry;
  double diff;

  temp_wet = *wet_value * old_mu;
  temp_dry = *dry_value * (1. - old_mu);
  *wet_value = temp_wet + temp_dry;
  *dry_value = temp_wet + temp_dry;
  diff = (temp_wet+temp_dry) - (*wet_value*new_mu + *dry_value*(1.-new_mu));     //LML 150429 always = 0
  if(fabs(diff) > 1.e-10) error = TRUE;
  else error = FALSE;

  return (error);
}
