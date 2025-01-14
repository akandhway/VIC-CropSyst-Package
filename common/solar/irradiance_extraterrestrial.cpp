#include "irradiance_extraterrestrial.h"
#include "corn/measure/measures.h"

#define solar_constant_MJ_m2_day    118.08
#define solar_constant_J_m2_sec     1360.0
//RLN warning I am not sure about units/value of solar_constant_J_m2_sec

#include <assert.h>

namespace CS
{
//______________________________________________________________________________
Solar_radiation_extraterrestrial_daily::Solar_radiation_extraterrestrial_daily
(const Geocoordinate &geocoordinate_
,const CORN::Date_const &today_)                                                 //170524
: today         (today_)
, geocoordinate (geocoordinate_)
{}
//_Solar_radiation_extraterrestrial_daily::constructor______________2015-06-25_/
bool Solar_radiation_extraterrestrial_daily::start()              modification_
{
   for (CORN::DOY doy = 1; doy < 366; doy++){
       irradiance_MJ_m2_day[doy] = calculate(doy);
       //debug std::cout<<"DOY:"<<doy<<" "<<irradiance_MJ_m2_day[doy]<<std::endl;
   }
   return true;
}
//_Solar_radiation_extraterrestrial_daily::start____________________2015-06-25_/
float64 Solar_radiation_extraterrestrial_daily
::calculate(CORN::DOY doy)                                          calculation_
{
   // RLN day_angle is my term, I don't know exactly what it is.
   float64 doy_f64 = doy;                                                        //070604
   float64 day_angle = 2.0 * CORN_pi / 365.0 * doy_f64;
   float64 declination = 0.409 * sin(day_angle - 1.39);
   float64 inverse_relative_dist_sun_earth = 1.0 + 0.0334 * cos(day_angle);
   // The distance from the Earth to the Sun varies
   // by a factor of twice the eccentricity.
   // That is  2 � 0.0167 = 0.0334 (or 3.34%).
   // So 0.0334 should be  more accurate than 0.033, 0.0334 is what Claudio has in VB code. 041027
   float64 latitude_rad = geocoordinate.get_latitude_radians();
   float64 acos_term = -tan(latitude_rad) * tan(declination);                    //080318
   acos_term = CORN::must_be_between<float64>(acos_term,-1.0,1.0);               //080318
   float64 sunset_hour_angle_rad = acos(acos_term);
   // This equation was blowing up 080318
  // acos(-tan(latitude_rad) * tan(declination));                                //080318
   float64 ExTerr_rad // MJ/(m2 day)
   = solar_constant_MJ_m2_day
   * inverse_relative_dist_sun_earth
   * ((    sunset_hour_angle_rad * sin(latitude_rad) * sin(declination))
     +(sin(sunset_hour_angle_rad)* cos(latitude_rad) * cos(declination)))
   / CORN_pi;
   // RLN: in polar regions (above the artic circle) the above calculation gives 0.0. 080318
   //  This can cause div zero errors in later equations) so I added this hack (this limit could possible be a little higher)
   if (ExTerr_rad < 0.0001) ExTerr_rad = 0.0001;                                 //080318
   return ExTerr_rad;
}
//_Solar_radiation_extraterrestrial_daily::calculate________________2001-10-08_/
float64 Solar_radiation_extraterrestrial_daily
::get_MJ_per_m2_day(CORN::DOY doy)                                         const
{
   if (!doy) doy = today.get_DOY();
   float64 MJ_per_m2_day = irradiance_MJ_m2_day[doy];
   return MJ_per_m2_day;

   // Warning need to check this

}
//_Solar_radiation_extraterrestrial_daily::get_MJ_per_m2_day___________________/
float64 Solar_radiation_extraterrestrial_daily
::get_J_per_m2_day(CORN::DOY doy)                                          const
{
   if (!doy) doy = today.get_DOY();
   float64 J_per_m2_day =  MJ_m2_to_J_m2(irradiance_MJ_m2_day[doy]);
   return J_per_m2_day;

   // Warning need to check this
}
//_Solar_radiation_extraterrestrial_daily::get_J_per_m2_day_________2015-06-25_/
float64 Solar_radiation_extraterrestrial_daily
::get_insolation_fraction(CORN::DOY doy, CORN::Hour hour)                  const
{
   assert(false);
   // This should never be called
   // If you get here then use Solar_radiation_extraterrestrial_hourly
   return 99999;
}
//_Solar_radiation_extraterrestrial_daily::get_insolation_fraction_____________/
float64 Solar_radiation_extraterrestrial_daily
::get_insolation_fraction_today(CORN::Hour hour)                           const
{
   assert(false);
   // This should never be called
   // If you get here then use Solar_radiation_extraterrestrial_hourly
   return 99999;
}
//_Solar_radiation_extraterrestrial_daily::get_insolation_fraction_today_______/
//______________________________________________________________________________
Solar_radiation_extraterrestrial_hourly::Solar_radiation_extraterrestrial_hourly
(const Geocoordinate &geocoordinate_
,const Sun_days      &sun_days_
,const Sun_hours     &sun_hours_
,const CORN::Date_const &today_)                                                 //170524
: Solar_radiation_extraterrestrial_daily(geocoordinate_,today_)
,sun_days   (sun_days_)
,sun_hours  (sun_hours_)
{
   for (CORN::DOY doy = 0; doy < 367; doy++)
   {
      radiation_J_m2_day  [doy] = 0;
      /*
      radiation_J_m2_s  [doy] = 0;
      insolation_fraction[doy] = 0;
      */
      for (CORN::Hour hour = 0; hour < 24; hour++)
      {
         radiation_J_m2_s  [doy][hour] = 0;
         insolation_fraction[doy][hour] = 0;
      }
   }
}
//_Solar_radiation_extraterrestrial_hourly:construtor_______________2015-06-25_/
bool Solar_radiation_extraterrestrial_hourly::start()             modification_
{  bool started = true;
   started &= Solar_radiation_extraterrestrial_daily::start();                  //150901
   for (CORN::DOY doy = 1; doy <=366; doy++)
   { started &= start_DOY(doy);}
   return started;
}
//_Solar_radiation_extraterrestrial_hourly:start_______________________________/
bool Solar_radiation_extraterrestrial_hourly
::start_DOY(CORN::DOY doy)                                         modification_
{
   {
      float64 sum_J_m2_sec = 0.0;
      for (CORN::Hour hour= 0; hour < 24; hour++)
         sum_J_m2_sec +=
            radiation_J_m2_s[doy][hour] = calculate(doy, hour);
      // must be separate loops!
      for (CORN::Hour hour= 0; hour < 24; hour++)
      {  float64 insolation_fraction_hour
           = radiation_J_m2_s[doy][hour] / sum_J_m2_sec;
         insolation_fraction[doy][hour] = insolation_fraction_hour;
      }
      radiation_J_m2_day[doy] = sum_J_m2_sec/24.0;
   }
   return true;
}
//_Solar_radiation_extraterrestrial_hourly:start_DOY________________2015-06-25_/
float64 Solar_radiation_extraterrestrial_hourly
::calculate(CORN::DOY doy, CORN::Hour hour)                         calculation_
{  // Note Claudio original V.B. code calculated J/m2 sec
   // In CropSyst the current solar constant is setup for the current time step and requested units
   float64 earth_sun_distance_adj    = sun_days.get_earth_sun_distance_adj(doy);
   float64 cosine_solar_zenith_angle = sun_hours.get_cosine_solar_zenith_angle(doy,hour);
   float64 extraterrestrial_radiation
         = solar_constant_J_m2_sec * CORN_sqr(earth_sun_distance_adj)
           * cosine_solar_zenith_angle;

   // Cosine_Solar_Zenith_Angle goes negative when the sun is below the horizon
   // solar_constant_J_m2_sec = 1360
   if (extraterrestrial_radiation < 0.0) extraterrestrial_radiation = 0; // Sun is below the horizon
   return extraterrestrial_radiation;
}
//_Solar_radiation_extraterrestrial_hourly::calculate__________________________/
float64 Solar_radiation_extraterrestrial_hourly
::get_MJ_per_m2_day   (CORN::DOY doy) const
{  if (!doy) doy = today.get_DOY();
   return  J_m2_to_MJ_m2(radiation_J_m2_day[doy]/* /24.0*/) * seconds_per_day;
}
//_Solar_radiation_extraterrestrial_hourly::get_MJ_per_m2_day__________________/
float64 Solar_radiation_extraterrestrial_hourly
::get_J_per_m2_day(CORN::DOY doy)                                          const
{  if (!doy) doy = today.get_DOY();
   return  radiation_J_m2_day[doy];
}
//_Solar_radiation_extraterrestrial_hourly:____________________________________/
float64 Solar_radiation_extraterrestrial_hourly
::get_MJ_per_m2_hour(CORN::DOY doy, CORN::Hour hour)                       const
{  if (!doy) doy = today.get_DOY();
   // WARNING Units conversion not varified yet  151222
   return  J_m2_to_MJ_m2(radiation_J_m2_s[hour][doy]) * seconds_per_hour;
}
//_Solar_radiation_extraterrestrial_hourly::get_MJ_per_m2_hour_________________/
float64 Solar_radiation_extraterrestrial_hourly
::get_J_per_m2_hour(CORN::DOY doy, CORN::Hour hour )                       const
{  if (!doy) doy = today.get_DOY();
   // WARNING Units conversion not varified yet  151222
   return  radiation_J_m2_s[doy][hour] * seconds_per_hour;
}
//_Solar_radiation_extraterrestrial_hourly::get_J_per_m2_hour______2015-06-25_/
float64 Solar_radiation_extraterrestrial_hourly
::get_Watt_per_m2_hour(CORN::DOY doy, CORN::Hour hour) const
{  float64 J_per_m2_hour = get_J_per_m2_hour(doy,hour);
   float64 Watt_per_m2 = J_per_m2_hour / 3600.0;
   return Watt_per_m2;
}
//_Solar_radiation_extraterrestrial_hourly::get_Watt_per_m2_hour___2015-12-14_/
float64 Solar_radiation_extraterrestrial_hourly
::get_insolation_fraction(CORN::DOY doy, CORN::Hour hour)                  const
{  return insolation_fraction[doy][hour];
}
//_Solar_radiation_extraterrestrial_hourly::get_insolation_fraction_2015-06-27/
float64 Solar_radiation_extraterrestrial_hourly
::get_insolation_fraction_today( CORN::Hour hour)                          const
{  return get_insolation_fraction(today.get_DOY(),hour);
}
//_get_insolation_fraction_today_2015-06-27____________________________________/
}//_namespace CS_______________________________________________________________/

