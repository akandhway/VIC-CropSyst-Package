#  include <math.h>
#  include "common/weather/snow_pack.h"
#  include "corn/math/compare.hpp"
//______________________________________________________________________________
Snow_pack_common::Snow_pack_common
(modifiable_ Precipitation     &_liberated_water
,const Precipitation           &_precipitation
,const Air_temperature_average &_air_temperature_avg
)
: Snow_pack                            ()
, liberated_water                      (_liberated_water)
, precipitation                        (_precipitation)
, air_temperature_avg                  (_air_temperature_avg)
, liquid_water_m(0)                                                              //990111
, ice_water_m(0)                                                                 //990111
, snow_melted_today(0.0)
, new_snow_this_time_step(false)
{
#ifdef DEBUG2
DBG2        debug << "date/time,"
DBG2              << "solar elev angle,"
DBG2              << "solar rad KJ/m2,"
DBG2              << "air temp C*10,"
DBG2              << "???,"
DBG2              << "wind rough,"
DBG2              << "dew pt *10,"
DBG2              << "Cloud emis * 100,"
DBG2              << "???,"
DBG2              << "??? ,"
DBG2              << "rain cm *10000,"
DBG2              << "tot water before melt *10000,"
DBG2              << "cos.i.angle deg*1000,"
DBG2              << "solar rad KJ/m2,"
DBG2              << "albedo% *100,"
DBG2              << "q.srad KJ/m2,"
DBG2              << "q.lw,"
DBG2              << "vap.d.air *10000,"
DBG2              << "q.conv KJ/m2,"
DBG2              << "q.cond KJ/m2,"
DBG2              << "q.rain.ground KJ/m2,"
DBG2              << "melt cm*10000,"
DBG2              << "cold.content cm*10000,"
DBG2              << "refreeze.lw cm*10000,"
DBG2              << "refreeze.input cm*10000,"
DBG2              << "ice.melt cm *10000,"
DBG2              << "ice.content cm*10000,"
DBG2              << "liquid.water cm*10000,"
DBG2              << "melt.water cm*10000,"
DBG2              << endl;
#endif
}
//_Snow_pack_common:constructor________________________________________________/
float64 Snow_pack_common::update_snow_pack_thickness()            rectification_
{
/*151023  CropSyst allowed snow to be reset from observed data
   if (observed_snow_pack_thickness_today)
   {  snow_melted_today = (
         snow_pack_thickness_m //yesterday's
         > observed_snow_pack_thickness)
      ? (snow_pack_thickness_m - observed_snow_pack_thickness) / 5.0
      :  0.0;
      snow_pack_thickness_m = observed_snow_pack_thickness;
   } else
*/
      snow_pack_thickness_m = ice_water_m * 5.0;
   return snow_pack_thickness_m;
}
//_update_snow_pack_thickness_______________________________________1999-02-02_/
const CORN::Quality &Snow_pack_common::update()                   rectification_
{  new_snow_this_time_step = false; // today's snow computed next                //000315
   // liberated water has been set to precipitation by Precipitation_storage     //141205


float64 lib_before_melt = liberated_water.get_m();

   float64 new_liberated_water = liberated_water.get_m();
   if (should_convert_rain_to_snow())                                            //981029
   { // if freezing weather, accumulate precip to snow storage
     snow_melted_today = 0.0;                                                    //981208
     // some or all precipitation may turn to snow returning only a portion as rain
     new_liberated_water -= snow_fall(precipitation.get_m());
   }
   //   else //if warm weather, accumulate snow melt to precipitation            //991011
   new_liberated_water = snow_melt(new_liberated_water);/**0.1/100;*/            //160823

//if (!CORN::is_approximately<float64>(lib_before_melt,new_liberated_water))
/*
if (snow_melted_today > 0.0000001)
std::cout << "Reached snow_melted_today" << std::endl;
*/
/*
if (new_liberated_water > lib_before_melt)
std::cout << "Reached" << std::endl;
*/
   liberated_water.set_m(new_liberated_water,simulated_quality);                 //14105
   update_snow_pack_thickness();
   return liberated_water.get_quality();
}
//_update________________________________________________2014-12-05_1999-09-29_/
bool Snow_pack_common::is_melted_completely()                       affirmation_
{  return
      CORN::is_approximately<float64>(snow_pack_thickness_m,0.0,0.0000000001)
   && CORN::is_approximately<float64>(precipitation.ref_amount(),0.0,0.0000000001);
}
//_is_melted_completely_____________________________________________2016-01-17_/
Snow_pack_temperature_based::Snow_pack_temperature_based
(modifiable_ Precipitation     &_liberated_water
,const Precipitation           &_precipitation
,const Air_temperature_average &_air_temperature_avg)
:Snow_pack_common(_liberated_water,_precipitation,_air_temperature_avg)
{}
//_Snow_pack_temperature_based:constructor__________________________2014-12-05_/
float64 Snow_pack_temperature_based::snow_melt(float64 actual_rain_m)      const
{  static const float64 snow_melt_factor = 0.0025; /* m/day/C'  water equivalent Peralta's factor*/
   snow_melted_today = (air_temperature_avg.Celcius() < 1.0)                     //990125
   ? 0.0 : (snow_melt_factor * (air_temperature_avg.Celcius() - 1.0));           //990125
   float64 snow_water_equivelent_depth = get_water_equivelent_depth();           //151026
      //151026 get_curr_snow_water_equivelent_depth();
   if (snow_water_equivelent_depth > 0.0)
   {  if (snow_water_equivelent_depth <= snow_melted_today)
      {  snow_melted_today = snow_water_equivelent_depth;
         liquid_water_m = ice_water_m = 0.0;
      } else
         ice_water_m -= snow_melted_today;
   } else
      snow_melted_today = 0.0;   // I added this line because it seemed necessary
   snow_melted_today += actual_rain_m;
   return snow_melted_today;
}
//_snow_melt___________________________________________________________________/
float64 Snow_pack_common::snow_fall(float64 precip)                        const
{  ice_water_m += precip;
   if (precip > 0.0000001)                                                       //000315
      new_snow_this_time_step = true;                                            //000315
   return precip; // take all precip as snow
}
//_snow_fall___________________________________________________________________/
Snow_pack_energy_based::Snow_pack_energy_based
(modifiable_ Precipitation             &_liberated_water
,const Precipitation                   &_precipitation
,const Air_temperature_average         &_air_temperature_avg
,const Dew_point_temperature_average   &_dew_point_temperature_avg
,const Solar_radiation                 &_solar_radiation
,const Sky                             &_sky                                     //141205
,const Sun                             &_sun
,const Aerodynamics                    &_aerodynamics
,CORN::Seconds                          _time_step
,float64                                _soil_slope_percent
,float64                                _site_aspect_deg
,bool                                   _forest_vegetation)
: Snow_pack_common
   (_liberated_water
   ,_precipitation
   ,_air_temperature_avg
   )
, sky                      (_sky)                                                //141205
, sun                      (_sun)                                                //141205
, aerodynamics             (_aerodynamics)                                       //141205
, time_step                (_time_step)                                          //141205
, site_aspect_deg          (_site_aspect_deg)
, soil_slope_percent       (_soil_slope_percent)
, dew_point_temperature_avg(_dew_point_temperature_avg)
, solar_radiation          (_solar_radiation)
, forest_vegetation        (_forest_vegetation)
, snow_temperature_last_timestep_C(0)
{}
//_Snow_pack_energy_based:constructor_______________________________2014-10-05_/
/*
#  Input date in (Day of year), year, measured solar radiation (KJ/m^2),
#  elevation angle (degrees), tavg (C x 10),
#  precip. (cm x 10000),
#  wind roughness wind_roughness (s/m), tdew (C x 10), and cloudiness (% x 100)
*/
float64 Snow_pack_energy_based::adjust_temperature_by_elevation
(float64 temperature_at_reference_elevation
,float64 target_elevation_meter
)
{  static const float64 lapse_rate_environmental_C_km = 6.49;                      //140723_061009
// http://en.wikipedia.org/wiki/Lapse_rate
//             The lapse rate is the drop in air temperature with elevation degC/km
//             3.5 defF/1000ft

//             The lapse rate method uses the relationship between temperature
//             and elevation for a region to estimate temperatures at unsampled sites.
//             Typically, temperatures decreases as elevation increases.
//             This relationship between temperature and elevation is know n as the lapse rate.
//             The lapse rate method uses the temperature value of the nearest weather station
//             and the difference in elevation to estimate temperature at the unsampled site.
//             To estimate temperature at an unsampled site,
//             the difference in elevation is multiplied by the lapse rate
//             and the subsequent number is added to or subtracted from the weather station temperature
//             to yield the site temperature.
//             The lapse rate method makes the assumption that the lapse rate
//             is constant for the study region.
/*
 Otherwise the lapse rate is map
that equals zero at the same elevation the air temperature was measured,
positive values for cells having elevations less than the elevation where
the air temp was measured, and negative numbers for higher elevation cells.
*/
//

   assert(false); // not yet implemented

   return temperature_at_reference_elevation;
}
//_adjust_temperature_by_elevation:constructor_________________________________/
#ifdef NYI
float64 Snow_pack_energy_based::get_dew_point_temperature
(const CORN_date_time &date_time
   //NYI  , CORN_seconds    time_step
)
{  // if time_step is one day use the average dew point temperature
   float64 dew_point_temperature = (get_max_dew_point_temperature() + get_min_dew_point_temperature()) / 2.0;
   return  adjust_temperature_by_elevation(dew_point_temperature);
   //NYI if time_step is less than 1 day, get the closest temperature for the specified time
}
#endif
//_get_dew_point_temperature___________________________________________________/
float64 Snow_pack_energy_based::snow_melt(float64 actual_rain_m)           const
{  // If no snow pack, don't bother melting, there is only rain (if any)
   float64 liquid_water_precipitated_today = 0;
   if (get_water_equivelent_depth() > 0.0)                                       //151026
   {
      liquid_water_precipitated_today =
         enhanced_snow_melt(actual_rain_m)
         + actual_rain_m;                                                        //170104
/*
if (liquid_water_precipitated_today)
std::clog << "liberated:" << liquid_water_precipitated_today << std::endl;
*/

   }
   else liquid_water_precipitated_today = actual_rain_m;
   return liquid_water_precipitated_today;
}
//_snow_melt___________________________________________________________________/
float64 Snow_pack_energy_based::enhanced_snow_melt // This should work for any time step
(float64 rain_m ) const  // actual rain (deducting snow)
{
   // Snowmelt Calculations based on U.S. Army Corps Model and the WHYM model Walter (1995)
   // Assumes precip falls as snow if tair > 1 C
   // Currently if we are melting then all it is warm enough for all precipitation to go to rain
   float64 total_water_before_melting = get_water_equivelent_depth();            //151026
   float64 slope_dec = soil_slope_percent / 100.0;  // Convert percent to decimal percent
   float64 aspect_rad = deg_to_rad(site_aspect_deg);
   // thermal quality is the fraction of the snowpack which is ice
   float64 thermal_quality_last_timestep =  // % 0-100
   CORN::is_approximately<float64>(total_water_before_melting, 0,0.000001)
   ? 97.0
   : ice_water_m / total_water_before_melting * 100;
   // Calculate the cosine of the incident angle using slope% x 10
   // and $elev angle in degrees and aspect in degrees from north
   // where 0 degrees is a south facing slope units of cos.i.angle are degrees
   float64 sun_elevation_angle_rad = sun.get_elevation_angle_rad();              //141205
   float64 cos_i_angle_rad =
   CORN::must_be_greater_or_equal_to<float64>
   ((sin(atan(slope_dec))*cos(sun_elevation_angle_rad)*cos(-1 * aspect_rad)
    + cos(atan(slope_dec)) * sin(sun_elevation_angle_rad))
   ,0.0);
   // Now get solar radiation through vegetation (KJ/m^2)
   // the minimum transmissivity due to forest cover is 0.25
   // units of cos.i.angle are degrees x 1000 and elevation angle is in degrees srad
   // Realistically though it should be adjusted some way by the leaf area index.
   float64 cover_transmissivity = forest_vegetation ? 0.25  : 1.0;
   float64 solar_rad_through_vegetation_kJ_m2 = cover_transmissivity
      * solar_radiation.get_kJ_m2()                                              //141205
      //141205 * MJ_m2_to_kJ_m2(solar_radiation_MJ_m2)
      * cos_i_angle_rad/sin(sun_elevation_angle_rad);
   // net heat flux added to the snowpack by shortwave rad.
   // albedo is in units of 0-1
   // # q.srad (KJ/m^2)
   float64 shortwave_net_rad_kJ_m2 = 0.0;
// 041108 The following line should be obsolete
// verify that the calculation below give identical value then delete this line
   float64 sky_albedo = sky.get_albedo();                                        //141205
   shortwave_net_rad_kJ_m2 = 1000 *  // <- MJ to KJ
      sky.calc_shortwave_net_radiation(sky_albedo,kJ_m2_to_MJ_m2(solar_rad_through_vegetation_kJ_m2));
   shortwave_net_rad_kJ_m2 =                                                     //041108
      sky.calc_shortwave_net_radiation(sky_albedo,solar_rad_through_vegetation_kJ_m2); //041108
   float64  net_heat_flux_from_shortwave_rad_KJ_m2 = shortwave_net_rad_kJ_m2;
      // Assume the emissivity of snow is 0.97
   static const float64 snow_emissivity = 0.97;                                  //061009
   float64 cloud_emissivity = sky.get_cloud_emissivity();
   float64 above_ground_emissivity = forest_vegetation
    ? 0.90
    : std::max<float64>(0.76,cloud_emissivity);
   static const float64 snow_melt_temperature = 0;                               //061009
   float64 air_temp_kelvin = air_temperature_avg.kelvin();                       //141205
   float64  cover_emissivity = above_ground_emissivity
   * pow(air_temp_kelvin,4)
     - snow_emissivity * pow(C_to_kelvin(snow_melt_temperature),4);
      //  net heat energy added to the snowpack by longwave rad.
      // Stefan Boltzmann constant = 5.67 x 10^-8 W/m^2/K^4
      // (NOTE: THIS CONSTANT MUST BE CONVERTED TO J/m^2/K^4/hr or /day ACCORDING TO THE DESIRED TIMESTEP)
   static const float64 Stefan_Boltzmann_constant_per_day = 5.67e-8;             //061009
   float64 new_heat_energy_from_longwave_rad_KJ_m2 =
      Stefan_Boltzmann_constant_per_day * time_step / 1000 * cover_emissivity;
   static const float64 thermodynamic_vapor_constant_KJ_kg_K = 0.4615; //KJ/(kg K)  //061009
   float64 esat_kPa =
      exp((16.78 * (dew_point_temperature_avg.Celcius()) - 116.9)
      / (dew_point_temperature_avg.Celcius() + 237.3));
   //             act. air vapor density (kg/m^3) calculated using tdew
   float64 act_air_vapor_density_kg_m3
      = esat_kPa /(dew_point_temperature_avg.kelvin())
             / thermodynamic_vapor_constant_KJ_kg_K;
   //             assume vapor density (kg/m^3) of the snow is constant at 0.0048 kg/m^3
   //             snow vapor density found by calculating the sat. vap. density at 0 C.
   static const float64 snow_vapor_density_kg_m3 = 0.0048;                       //061009
   static const float64 latent_heat_vaporization_KJ_kg = 2500.0;  // KJ/kg       //061009
   // heat_conv_vap_exch is the heat from convective vapor exchange (condensation/evaporation) (KJ/m^2)
   float64 wind_roughness_ts = aerodynamics.get_resistance_of_snow(time_step);  //141205
   float64 heat_conv_vap_exch_KJ_m2 = latent_heat_vaporization_KJ_kg
   * (act_air_vapor_density_kg_m3 - snow_vapor_density_kg_m3) / wind_roughness_ts;
   // Density of air 1.29 kg/m^3, heat capacity of air 1 kJ/kg/C
   static const float64 air_density_kg_m3 = 1.29;                                //061009
   // Assume snow temperature is 0 C when snow is melting.
   static const float64 snow_temperature_when_melting =0.0;                      //061009
   static const float64 heat_capacity_air_KJ_kgC = 1.0;                          //061009
   // conv_grad_air_snow is the heat from convective temp. gradients between air and snow (KJ/m^2)
   const float64 air_temp_avg = air_temperature_avg.Celcius();                   //141205
   float64 conv_grad_air_snow_KJ_m2 = heat_capacity_air_KJ_kgC * air_density_kg_m3
      * (air_temp_avg - snow_temperature_when_melting) / wind_roughness_ts;
   //             ground melt constant at 173 KJ/m^2/day or 7.2 KJ/m^2/hr
   static const float64 ground_melt_constant_KJ_m2_day = 173.0;                  //061009
   static const float64 ground_melt_constant_KJ_m2_hour =7.2;                    //061009
   static const float64 ground_melt_constant_KJ_m2_half_hr =3.6;                 //061009
   float64  ground_melt_constant_KJ_m2_ts = 173.0 * time_step / seconds_per_day;
   static const float64 heat_capacity_water_KJ_kgC = 4.2;      // heat capacity of water is 4.2 KJ/(kg C)   //061009
   static const float64 water_density_kg_m3        = 1000.0;   // density of water is 1000 kg/m^3           //061009
   // rain_ground_cond_heat is the combined heat convected from rain and conducted from ground (KJ/m^2)
   float64 rain_ground_conv_heat_KJ_m2 = ground_melt_constant_KJ_m2_ts
     + heat_capacity_water_KJ_kgC * rain_m * water_density_kg_m3 *(air_temp_avg-get_rain_to_snow_temperature())  ;
   static const float64 latent_heat_of_fusion_KJ_kg = 335.0;   // latent heat of fusion is 335 KJ/kg  //061009
   static const float64 ice_density_kg_m3 = 916.0;    // density of ice 916 kg/m^3     //061009
   float64 total_heat = ( net_heat_flux_from_shortwave_rad_KJ_m2
            + new_heat_energy_from_longwave_rad_KJ_m2
            + heat_conv_vap_exch_KJ_m2
            + conv_grad_air_snow_KJ_m2
            + rain_ground_conv_heat_KJ_m2);
   // Melt calculation before cold content and whc removed (cm*100)
   float64 pot_melt_m = (total_water_before_melting > 0.0)
   ? std::max<float64>(0
         ,total_heat
          / (ice_density_kg_m3 * latent_heat_of_fusion_KJ_kg *
            (thermal_quality_last_timestep / 100)) )
      : 0;
   // Assume the heat capacity of snow is 2.1 KJ/Kg/C
/*
if (pot_melt_m > total_water_before_melting)
std::cout << "pot_melt_m (" << pot_melt_m<< ") > total_water_before_melting (" << total_water_before_melting << ")" << std::endl;
*/
   pot_melt_m = CORN::must_be_between<float64>(pot_melt_m,0.0,total_water_before_melting); //170104

   static const float64 snow_heat_capacity_KJ_kg_C = 2.1;                        //061009
   // cold_content is the depth (snow water equivalent) of water needed to raise the temperature of the snowpack to 0 C
   float64 cold_content_m = CORN::must_be_greater_or_equal_to<float64>
   ((-snow_temperature_last_timestep_C * total_water_before_melting
    * (snow_heat_capacity_KJ_kg_C / latent_heat_of_fusion_KJ_kg))
   ,0.0);
   // depth of liquid water in the snowpack refrozen to satisfy the cold.content
   float64 snowpack_refrozen_liquid_m = std::min<float64>(liquid_water_m,cold_content_m);
   float64 snowpack_input_m = // is the amount of melt water or rain frozen in the snowpack to satisfy the cold.content
      std::min<float64>(pot_melt_m + rain_m
           ,std::min<float64>(
              CORN::must_be_greater_or_equal_to<float64>
               (cold_content_m + snowpack_refrozen_liquid_m,0.0)
              ,liquid_water_m + rain_m));
   // snow_melted_today is the amount of ice in the snowpack melted after the cold content is satisfied.
   float64 snow_melted_today_m = CORN::must_be_0_or_greater<float64>(pot_melt_m - snowpack_input_m); //170104
/*
if (snow_melted_today_m > 0.0)
std::clog << "reached snow_melted_today_m" << snow_melted_today_m << std::endl;
*/
   snow_melted_today = snow_melted_today_m;                                      //170104
   // Adjust ice_content to the amount of ice remaining in the snowpack
   ice_water_m =
      CORN::must_be_greater_or_equal_to<float64>
      (ice_water_m // <- Includes any snowfall already taken today
         + std::min<float64>(liquid_water_m + rain_m
                   ,snowpack_refrozen_liquid_m + snowpack_input_m)
         - snow_melted_today_m
      ,0.0);
   // Assume water holding capacity is 3% of snow_water_equivalent_depth  this could be a parameter rangin from 1 to 5%
   static const float64 max_snow_water_holding_capacity = 0.03;                  //061009
   // Update liquid_water_m (the amount of liquid water remaining in the snowpack)
   liquid_water_m =
      is_approximately(ice_water_m,0.0,0.0000001)
      ? 0
      : CORN::must_be_between<float64>(liquid_water_m + rain_m + snow_melted_today_m
               - snowpack_refrozen_liquid_m - snowpack_input_m
             ,0.0
             ,max_snow_water_holding_capacity * total_water_before_melting);
   float64 total_water_after_melting = get_water_equivelent_depth(); //151026 get_curr_snow_water_equivelent_depth();
   // drained_melt_water_m is all the water draining from the snowpack
   // 141205 was (i.e. rain+snowmelt)
   // Note:  when there is no snowpack melt.water=rainfall
   // snow temperature calculated for the next time step
   snow_temperature_last_timestep_C = CORN::must_be_less_or_equal_to<float64>
      (dew_point_temperature_avg.Celcius(),0.0);

   /*170104 this is always 0 because we are not changing the total liquid water context
   float64 drained_melt_water_m =
      CORN::must_be_greater_or_equal_to<float64>(
         //141205 rain_m -  141205 now this is only melted water
              (total_water_after_melting - total_water_before_melting),0.0);
   return  drained_melt_water_m;
   */
   return snow_melted_today_m;                                                   //170104
}
//_enhanced_snow_melt__________________________________________________________/
float64 Snow_pack_common::get_rain_to_snow_temperature()                   const //141205
{  return -1.5;                                                                  //070314
#ifdef OBSOLETE
//070314 This temperature was intended to be location specific, but I never
// added it to the location file. Now I am getting rid of the location file so this would need to be accomodated otherwise
// if I decide to add it.
   Location_parameters *location = weather.weather_data->get_location_parameters(); //050525
   return  location? location->get_rain_to_snow_temperature() : -1.5;
#endif
}
//_get_rain_to_snow_temperature________________________________________________/

