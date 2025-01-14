#include "corn/measure/measures.h"
#include "corn/math/compare.hpp"
#include <math.h>
#include <float.h>
#include "corn/math/moremath.h"
#include "common/weather/sky.h"
#include "common/weather/parameter/WP_air_temperature.h"
#include "common/weather/parameter/WP_precipitation.h"
#include "common/weather/parameter/WP_solar_radiation.h"

// Albedo ranges from 0-1
// In some places this is used as a percent
//______________________________________________________________________________
Sky::Sky
(const CS::Solar_radiation_extraterrestrial  &ET_solar_radiation_new_            //150626
,const Air_temperature_average   &air_temperature_avg_                           //151023
,const Solar_radiation           &solar_radiation_                               //151023
,const Precipitation             &precipitation_                                 //151023
)
// radiation units are UC_MJ_m2 (MJ/m2) or UC_J_m2 (J/m2)
: ET_solar_radiation_new                               (ET_solar_radiation_new_) //150626
, air_temperature_avg                                     (air_temperature_avg_) //151023
, solar_radiation                                             (solar_radiation_) //151023
, precipitation                                                 (precipitation_) //151023
,albedo_last_time_step                                                     (0.3) //000315
,albedo                                                                    (0.3)
,cloud_emissivity                                                          (0.0)
,curr_cloud_cover_fraction                                                 (0.0) //000216
,cloud_cover_quality                                   (CORN::not_valid_quality) //000326
{}
//_Sky:constructor__________________________________________________2004-11-09_/
float64 Sky::calc_albedo(bool any_new_snow, Seconds for_time_step)         const //131104
{
//  Albedo approximated from Army Corps relationship.
//  for the daily model the coefficents are: 0.95, 0.965, 0.98, 0.99
//  for the hourly model the coefficents are:0.9978 , 0.9985, 0.9991, 0.99955
//  assume albedo for new fallen snow is 85% and albedo of old snow
//  is never less than 30% albedo
   bool daily_time_step = (for_time_step == seconds_per_day);
   float64 albedo_coef_a = (daily_time_step) ? 0.95   :0.9978;
   float64 albedo_coef_b = (daily_time_step) ? 0.965  :0.9985;
   float64 albedo_coef_c = (daily_time_step) ? 0.98   :0.9991;
   float64 albedo_coef_d = (daily_time_step) ? 0.99   :0.99955;
   // Albedo in cropsyst ranges 0-1  not 0-100
   float64 local_albedo = 0.0;
   if (any_new_snow)
      local_albedo = 0.8500;       // <- this will never be the case
   else
      local_albedo =
                   albedo_last_time_step > 0.6400
                     ? albedo_coef_a * albedo_last_time_step
                     : albedo_last_time_step > 0.4200
                        ? albedo_coef_b*albedo_last_time_step
                        : albedo_last_time_step > 0.3500
                           ? albedo_coef_c * albedo_last_time_step
                           : (albedo_last_time_step * albedo_coef_d) > 0.3000
                              ? albedo_coef_d*albedo_last_time_step
                              : 0.3000;
   albedo_last_time_step = local_albedo; // remember for next time step
   return local_albedo;
}
//_calc_albedo_________________________________________________________________/
float64 Sky::calc_clear_sky_emissivity(float64 avg_air_temperature)        const //131104
{  // This works for any time step
   float64 clear_sky_emiss =  (0.72 + 0.005 * avg_air_temperature);
   return clear_sky_emiss; //  Unsworth and Montieth (1975)?
}
//_calc_clear_sky_emissivity________________________________________2004-10-25_/
float64 Sky::calc_clear_sky_emissivity_2
(float64 avg_air_temperature_kelvin,float64 vapor_pressure)                const //131104
{  // This is currently used only in V4 hourly soil temperature calculation, may want to merge with the original above
   float64  clear_sky_emiss = (vapor_pressure == -999.0) // Not sure what condition this is
   ? 0.0000092 * CORN_sqr(avg_air_temperature_kelvin)
   : 1.72 * pow((vapor_pressure / avg_air_temperature_kelvin),(1.0 / 7.0));
   return clear_sky_emiss;
}
//_calc_clear_sky_emissivity_2______________________________________2004-10-25_/
float64 Sky::calc_cloud_emissivity
(float64 avg_air_temperature   // This works for any time step
,float64 cloud_fract
,float64 clear_sky_emissivity)                                             const //131104_041023
{  // Emissivity of cloudy skies calculated from the Unsworth and Montieth (1975)
 // relationship e=(0.72+0.005*Tavg)*(1-0.84*Cloud fraction)+0.84*Cloud fraction
 //# assumes temperature of snow is 0 C when melting
   float64 cloud_fract_term = 0.84 * cloud_fract;
   float64 cloud_fract_fact =  (1.0 - cloud_fract_term);
   float64 cloud_emiss = (cloud_fract_fact *clear_sky_emissivity+ cloud_fract_term);
   if (CORN::is_approximately<float64>(clear_sky_emissivity,0.0,0.000001) )
      clear_sky_emissivity = calc_clear_sky_emissivity(avg_air_temperature);     //041025

   //131104  Something doesn't look right here
   //I think calc_clear_sky_emissivity should go before  cloud_emiss
   // division by zero thing maybe?

   return cloud_emiss;
}
//_calc_cloud_emissivity_______________________________________________________/
float64 Sky:: calc_cloud_cover_fraction
(float64 pot_solar_rad        // will work with any solar rad units I.e. MJ/m2
,float64 solar_radiation     // will work with any solar rad units I.e. MJ/m2
,float64 min_atmospheric_transmission_coef)                                const //131104_070213
{  //   float64 RSO = std::max<float64>(pot_solar_rad*0.75, solar_radiation);
   //  Note Claudios VB code uses only max of 0.75 waiting for a reply to finalize 041025
/*RLN
if (std::isnan(solar_radiation)) return 0.0;
*/
   float64 fract_rad_act_to_pot =  solar_radiation / pot_solar_rad;              //070604
   float64 atmospheric_transmission_coef = CORN::must_be_between<float64>
      (fract_rad_act_to_pot,min_atmospheric_transmission_coef,0.75);             //060229
   float64 pot_cloud_fract = 2.33 - 3.33 * atmospheric_transmission_coef;
   float64 calc_cloud_fract = CORN::must_be_between<float64>(pot_cloud_fract,0.0,1.0);
   return  calc_cloud_fract;
}
//_calc_cloud_cover_fraction________________________________________2000-03-08_/
float64 Sky::calc_shortwave_net_radiation
(float64 _albedo   // This could be any albedo,
,float64 _sol_rad) const // This will work for any units of solar rad
{  float64 swnr = ( 1.0 - _albedo ) * _sol_rad;
   return swnr;
}
//_calc_shortwave_net_radiation_____________________________________1999-03-29_/
bool Sky::update_clouds()
{
// Probably  Not needed, already updated
/*150628 now using CS::Solar_extraterrestrial_radiation
   Extraterrestrial_solar_radiation_day::update();                              //011009
*/
   float64 air_temp_avg = air_temperature_avg.Celcius();                         //151023
   bool any_new_snow                                                             //151023
      = air_temp_avg <= 0.0
      &&precipitation.get_m() > 0.0;
   albedo           = calc_albedo(any_new_snow,seconds_per_day);
   if (!cloud_cover_quality.is_valid())
   {
      const CS::Solar_radiation_extraterrestrial_daily
         *as_solar_radiation_extraterrestrial_daily                              //151027
         = dynamic_cast<const CS::Solar_radiation_extraterrestrial_daily *>
            (&ET_solar_radiation_new);                                           //151027
      float64 extraterrestrial_radiation_MJ_per_m2_day                           //151027
         = as_solar_radiation_extraterrestrial_daily                             //151027
         ? as_solar_radiation_extraterrestrial_daily->get_MJ_per_m2_day()        //151027
         : ET_solar_radiation_new.get_MJ_per_m2_day();                           //151027
      curr_cloud_cover_fraction   =calc_cloud_cover_fraction
        (extraterrestrial_radiation_MJ_per_m2_day,solar_radiation.get_MJ_m2());  //151024
      cloud_cover_quality.assume_code(CORN::estimated_quality);                  //150210
         // probably should be estimated (or calculated) from solar_radiation quality.
   }
   cloud_emissivity = calc_cloud_emissivity(air_temp_avg,curr_cloud_cover_fraction);//151023
   return true;                                                                  //151117
}
//_update_clouds____________________________________________________1999-10-22_/
void Sky::invalidate(bool absolutely)
{
   cloud_cover_quality.invalidate(absolutely);                                   //091215
}
//_invalidate_______________________________________________________2009-12-16_/
