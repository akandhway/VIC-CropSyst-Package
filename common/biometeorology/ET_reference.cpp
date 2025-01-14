#include "biometeorology/ET_reference.h"

#include "solar/irradiance_extraterrestrial.h"
#include "common/weather/weather_provider.h"
#include "weather/parameter/WP_atmospheric_pressure.h"
#include "weather/parameter/WP_vapor_pressure_act.h"
#include "weather/parameter/WP_wind_speed.h"
#include "weather/parameter/WP_air_temperature.h"
#include "biometeorology/aerodynamics.h"
#include "UED/library/std_codes.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"

#include "weather/parameter/WP_solar_radiation.h"
#include "weather/weather_types.h"
#include <math.h>
#include "corn/math/moremath.h"
#include "corn/measure/measures.h"
#include <assert.h>
#   include <algorithm>
//---------------------------------------------------------------------------
#define NOT_YET_IMPLEMENTED 0
//______________________________________________________________________________
Reference_evapotranspiration_model_set::Reference_evapotranspiration_model_set
(float32           elevation_                                                    //100420
,const float64    &day_length_hours_today_                                       //150627
,Weather_provider &weather_
,bool _Priestley_Taylor_constant_is_adjusted_for_VPD                             //100315
)
:ET_Reference_common
   (elevation_
   ,day_length_hours_today_                                                      //150627
   ,weather_.aerodynamics                                                        //151208_150624
   ,weather_.ET_solar_radiation                                                  //151208
   ,weather_.air_temperature_max
   ,weather_.air_temperature_min
   ,weather_.atmospheric_pressure
   ,weather_.vapor_pressure_actual
   ,weather_.solar_radiation
   ,weather_.vapor_pressure_saturation_slope                                     //140512
   ,weather_.latent_heat_vaporization                                            //140512
   ,weather_.psychrometric_constant                                              //140512
)
, air_temperature_avg(weather_.air_temperature_avg)                              //150624
,wind_speed(weather_.wind_speed)
,avg_fullday_vapor_pressure_deficit(weather_.vapor_pressure_deficit_fullday_avg)
,Priestley_Taylor_constant(DEF_PT_constant)
,Priestley_Taylor_constant_is_adjusted_for_VPD(_Priestley_Taylor_constant_is_adjusted_for_VPD) //100315
,Hargreaves_ET_slope(0.0)
,Hargreaves_ET_intercept(0.0)
,Hargreaves_parameter_set(false)
{}
//_Reference_evapotranspiration_model_set:constructor___________________________
float64 Reference_evapotranspiration_model_set::calc_general_equation
(float64 radiation_term
,float64 vapor_pressure_term
,float64 slope_of_saturated_vapor_pressure_curve
,float64 psycrometric_term   // kPa/�C
,float64 aerodynamic_resistance_term)                               calculation_
{  return (radiation_term + vapor_pressure_term)
      /  (slope_of_saturated_vapor_pressure_curve
          + psycrometric_term * aerodynamic_resistance_term);
}
//_calc_general_equation________________________________________________________
float64 Reference_evapotranspiration_model_set::calc_Penman_radiation_term() calculation_
{  return
      vapor_pressure_saturation_slope *(net_radiation - soil_heat_flux_density);
}
//_calc_Penman_radiation_term___________________________________________________
float64 Reference_evapotranspiration_model_set
::calc_Priestly_Taylor(CORN::Quality &quality)                      calculation_
{

//#error This is obsolete

   // The PT constant adjusted for VPD was removed in 1998 and restored in 2002. //020212
   float64 adj_PT_const  // Adjust PT constant for aridity                       //020212
   = ((avg_fullday_vapor_pressure_deficit_kPa > 1.0)  // only apply correction when fullday_VPD > 1.0 021212x
     && Priestley_Taylor_constant_is_adjusted_for_VPD)                           //100315
   ? (1.0 + (Priestley_Taylor_constant - 1.0) * avg_fullday_vapor_pressure_deficit_kPa) //021212
   : Priestley_Taylor_constant;                                                  //021212
   float64 potential_mm = adj_PT_const  *                                        //980311
          calc_general_equation
          (Penman_radiation_term
         ,0.0 // No vapor pressure term VPD is used to adjust Priestley_Taylor_constant
         ,vapor_pressure_saturation_slope //140512 renamed slope_of_saturated_vapor_pressure_curve
         ,psychrometric_constant  // kPa/�C
         ,1.0 // No aerodynamic resistance in PT
         );

#ifdef NYI
                   // Need to check if  daily_ratio_elevated_to_baseline_ppm_CO2_ET adjustment would apply to all models

   potential_mm *= daily_ratio_elevated_to_baseline_ppm_CO2_ET;  // when CO2change is not simulated daily_ratio_elevated_to_baseline_ppm_CO2_ET is 1.0
       // Need to check if  daily_ratio_elevated_to_baseline_ppm_CO2_ET adjustment would apply to all models
#endif
   potential_mm = std::max<float64>(0.05, potential_mm/latent_heat_vaporization);//980311
   quality.assume(avg_fullday_vapor_pressure_deficit);                           //150120
   return potential_mm;
}
//_calc_Priestly_Taylor________________________________________________________/
float64 Reference_evapotranspiration_model_set
::calc_Penman(CORN::Quality &quality)                               calculation_
{  // This is the original equation by Penman
   // Penman, H.L. 1948. Natural evaporation from open water, bare soil, and grass. Proc. Roy. Soc. London A193:120-146.
   assert(NOT_YET_IMPLEMENTED);
#ifdef NYI
   float64  windspeed = not sure the units (mm/day?)
   float64  wind_function  // mm /(day kPa)   // conventionally called Wf  W sub f
      =    a + b * ( windspeed );

      // The wind function is typically expressed as a linear function of wind speed
      // Penman (1948) derived a curved Wf function (a1 * (windspeed ^ b1)) but found a linear function was adequate.
/*
Penman noted in his 1948 paper one of the experimental problems needing a solution was the
reliable estimation of the daily mean dew point temperature. This problem has led to
current differences in using Penman�s equation and has resulted in myriad different
versions of a �modified Penman equation� with varying wind functions and methods for
estimating mean daily vapor pressure deficit (eo - ea) (Jensen et al., 1990).
*/

   // wind function is typically expressed as a linear function
   //  of wind speed in m s-1 (Uz) at the reference height (z) above the ground]

   float64 saturated_vapor_pressure_at_air_temp_mean // kPa
         // Saturated vapor pressure at mean air temperature (Conventionally call e0 e super 0)
      = xxxx  I not currently have this in the weather, but can be calculated

   float64 saturated_vapor_pressure_at_dewpt_mean // kPa
      // Mean ambient vapor pressure in kPa at the reference height above ground [ea = RH eo, where
      // RH is mean relative humidity as a fraction; conceptually,

      // It should equal the saturated vapor pressure at the daily mean dew point temperature   (Conventionally call ea   e sub a)
      = xxxx  I mot currently have this in the weather, but can be calculated

   float64  Ea  // mm/day
      = wind_function
         * (saturated_vapor_pressure_at_air_temp_mean
            - saturated_vapor_pressure_at_dewpt_mean);


   float64 vapor_pressure_term =  psychrometric_const * latent_heat_vaporization * Ea;
   float64 evaporative_latent_heat_flux // MJ/m2  (daily)     (open water evaporation?)
      = calc_general_equation
      (Penman_radiation_term
      ,vapor_pressure_term
      ,slope_of_saturated_vapor_pressure_curve
      ,psycrometric_const
      ,1.0 // The Penman equation had no aerodynamic adjustment for psycrometry
      );
   quality = xxxxx;
   return evaporative_latent_heat_flux; //BIG WARNING I am assuming originally denoted as lambda ETo is ET_ref
#else
   quality.assume_code(CORN::missing_quality);                                   //150130
   return 0.0; // NYI
#endif
}
//_calc_Penman_________________________________________________________________/
float64 Reference_evapotranspiration_model_set::calc_Penman_Monteith (CORN::Quality &quality)  const
{
   assert(NOT_YET_IMPLEMENTED);
#ifdef NYI
   float64 air_density //  kg/m^3  (conventionally expressed as rho sub a)
      = xxxxxx;
   static const float64 specific_heat_dry_air   // MJ / (kg * �C)   (conventionally expressed as C sub P)
      = 1.013e-3;
      // specific heat of dry air

   float64 saturated_vapor_pressure_mean // kPa    (conventionally expressed as (e sub s)sup 0)  )
      = I think:   (weather.get_sat_VP_at_max_temp() + weather.get_sat_VP_at_min_temp()) / 2.0;
      // mean saturated vapor pressure computed as the mean eo
      // at the daily minimum and maximum air temperature in �C

   float64 ambient_vapor_pressure_daily_mean    // kPa (conventionally expressed as ( e sub a))
      = weather.get_curr_act_vapor_pressure();  // RLN:  Need to verify this
      // mean daily ambient vapor pressure in kPa
   float64 water_vapor_aerodynamic_resistance      // units?
      = xxxxxx
      // water vapor aerodynamic resistance
   float64  vapor_pressure_term
      =  (  seconds_per_day  // 86400
            * air_density
            * specific_heat_dry_air
            * (saturated_vapor_pressure_mean
               - ambient_vapor_pressure_daily_mean))
         / water_vapor_aerodynamic_resistance;
   float64 canopy_surface_resistance    // s/m (conventionally expressed as ( r sub s))
      =  xxxx
      // canopy surface resistance depends on the biological behavior
      // of the crop canopy surface and is related to both crop specific
      // parameters (light attenuation, leaf stomatal resistances, etc.) and environmental
      // parameters (radiation, vapor pressure deficit, etc.).
   float64 aerodynamic_adjustment = 1.0 + (canopy_surface_resistance / r_av);
   float64 lambda_ET_ref_short_crops  // Reference ET for grass or short clipped crops
      = calc_general_equation
      (Penman_radiation_term
      ,vapor_pressure_term
      ,slope_of_saturated_vapor_pressure_curve
      ,psycrometric_const
      ,aerodynamic_adjustment);
   quality = xxxxx;
   return lambda_ET_ref_short_crops;
#else
   quality.assume_code(CORN::missing_quality);                                   //150130
   return 0.0; // NYI
#endif
}
//_calc_Penman_Monteith_____________________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set::calc_FAO_56
(CORN::Quality &quality
,nat8 include_terms                                                              //100509
,float64 Cn  // Default for FA0_56
,float64 Cd )                                                              const
{
#ifdef REPLACED
//These were the original calculation of saturation_vapor_pressure_deficit.
//Here I am just getting fullday VPD from the weather class.
//Which I have confirmed is the same value as avg_fullday_vapor_pressure_deficit_kPa
//Don't delete this, keep it for documentation purposes.
               float64 Tmax = weather.get_curr_max_temperature();
               float64 Tmin = weather.get_curr_min_temperature();
               float64 SatTmax = weather.curr_vapor_pressure_actual.calc_saturation_kPa(Tmax, Tmax, true); /*kPa*/
               float64 SatTmin = weather.curr_vapor_pressure_actual.calc_saturation_kPa(Tmin, Tmin, true); /*kPa*/
   float64 mean_saturation_vapor_pressure // kPa  (es)
      = (SatTmax+ SatTmin) / 2.0;
               float64 act_VP = weather.get_curr_act_vapor_pressure();
   float64 saturation_vapor_pressure_deficit // kPa
      = mean_saturation_vapor_pressure - act_VP;

#endif
   float64 saturation_vapor_pressure_deficit = avg_fullday_vapor_pressure_deficit_kPa;

// Note: this equation can be applied hourly if the 900.0 is divided by 24.
// and the Rn and G terms are expressed as MJ/(m2*hour)
   float64 vapor_pressure_term
      = psychrometric_constant
        * (Cn /* 900 for short crop */ /
            air_temperature_avg.kelvin())                                        //140519
        * wind_speed_at_standard_height_m_s
        * saturation_vapor_pressure_deficit;
   float64 aerodynamic_adjustment
      = (1.0 + Cd /*0.34 for short crop */ * wind_speed_at_standard_height_m_s);
   float64 radiation_term = inverse_latent_heat_flux_vaporization/*131105 0.408*/ * Penman_radiation_term;                       //100509

   // .408 may be 1/ lambda

   if (!(include_terms & include_radiation_term))                                //100509
      radiation_term = 0.0;                                                      //100509
   if (!(include_terms & include_aerodynamic_term))                              //100509
      vapor_pressure_term = 0.0;                                                 //100509
   float64 ETo   // mm/day hypothetical reference crop evapotranspiration rate
                 // (conventionally denoted as  ET sub 0)
      = calc_general_equation
      (radiation_term
      ,vapor_pressure_term
      ,vapor_pressure_saturation_slope //140512 slope_of_saturated_vapor_pressure_curve
      ,psychrometric_constant
      ,aerodynamic_adjustment);
   quality.assume(                                                               //040511
      (air_temperature_max       .get_quality().worst
      (air_temperature_min       .get_quality().worst
      (solar_radiation           .get_quality().worst
      (wind_speed                .get_quality().worst                            //150908
      (avg_fullday_vapor_pressure_deficit.get_quality())))))); // Yes best
   if (quality.is_valid())                                                       //040511
      quality.assume_code(quality_estimated_from_quality(quality));                   //150130_040511
   return ETo;
}
//_calc_FAO_56______________________________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set
::calc_ASCE(CORN::Quality &quality) const
{  // This is documented in
   // http://www.kimberly.uidaho.edu/water/asceewri/ASCE_Standardized_Ref_ET_Eqn_Phoenix2000.pdf
   assert(NOT_YET_IMPLEMENTED);
#ifdef NYI
/*_______*/
   ET_ref =
      = calc_general_equation
      (radiation_term
      ,vapor_pressure_term
      ,slope_of_saturated_vapor_pressure_curve
      ,psycrometric_const
      ,aerodynamic_adjustment
      );
   quality = xxxxx;
   return ET_ref;
#else
   quality.assume_code(CORN::missing_quality);
   return 0.0; // NYI
#endif
}
//_calc_ASCE________________________________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set
::calc_ASCE_EWRI_short_crop(CORN::Quality &quality)                        const
{  // This is documented in
   // http://www.cprl.ars.usda.gov/wmru/pdfs/PM%20COLO%20Bar%202004%20corrected%209apr04.pdf
   return calc_FAO_56(quality,Reference_evapotranspiration_model_set::include_total_terms,900.0,0.34);
}
//_calc_ASCE_EWRI_short_crop________________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set
::calc_ASCE_EWRI_tall_crop(CORN::Quality &quality)                         const
{  // This is documented in
   // http://www.cprl.ars.usda.gov/wmru/pdfs/PM%20COLO%20Bar%202004%20corrected%209apr04.pdf
   return calc_FAO_56(quality,Reference_evapotranspiration_model_set::include_total_terms,1600.0,0.38);
}
//_calc_ASCE_EWRI_tall_crop_________________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set
::calc_Penman_Monteith_CropSyst4 (CORN::Quality &quality)                  const
{  float64 ET_ref = 0.0;
   float64 wind_speed_m_d = wind_speed.get_m_d();                                //150727
   float64 wind_speed_at_standard_height_m_d =
         aerodynamics.
      calc_normalized_wind_speed(wind_speed_m_d);
   curr_aerodynamic_resistance_d_m =         // ra                               //160731
         aerodynamics.calc_resistance_of_plants_d_m
      (wind_speed_at_standard_height_m_d                                         //011008
      ,0.12); // meters reference crop plant height                              //011008

      // Actual vapor pressure computed by common_et
      // (it is used also for isothermal longwave net radiation).
      // Warning psychrometric_constant is not the same as FAO (data not available) using CropSyst
   slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference.update(); //160731
   float64 vapor_pressure_plus_resistance =                                      //011008
      slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference.get_kPa_per_C(); //150617

/*150617 RLN now use slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
         calc_vapor_pressure_plus_resistance
         (  vapor_pressure_saturation_slope                                      //140512 was slope_of_saturated_vapor_pressure_curve
         ,   psychrometric_constant
         ,   reference_canopy_resistance // bulk_resistance
         ,   curr_aerodynamic_resistance);
*/
   float64 atmospheric_density // kg/m3
      = calc_atmospheric_density(C_to_kelvin(air_temperature_avg));              //011009
   static const float64 spec_heat_cap_moist_air = 0.001013; // MJ/(kg C)         //011009
   {  float64 vol_heat_cap = atmospheric_density * spec_heat_cap_moist_air;      //011009
      float64 vapor_pressure_component
         = (avg_fullday_vapor_pressure_deficit_kPa * vol_heat_cap)
          /curr_aerodynamic_resistance_d_m;                                      //011009
      float64 Rad_term_num = vapor_pressure_saturation_slope
         * net_radiation / vapor_pressure_plus_resistance;                       //011009
      float64 Rad_term = Rad_term_num / latent_heat_vaporization;                //011009
      float64 Aero_term_num
         = vapor_pressure_component / vapor_pressure_plus_resistance;            //011009
      float64 Aero_term = Aero_term_num / latent_heat_vaporization;              //011009
      ET_ref = Rad_term + Aero_term;                                             //011009
   }
   quality.assume(                                                               //150130_040511
      (air_temperature_max       .get_quality().worst
      (air_temperature_min       .get_quality().worst
      (solar_radiation           .get_quality().worst
      (wind_speed                .get_quality().worst                            //150908
      (avg_fullday_vapor_pressure_deficit.get_quality())))))); // Yes best
   quality.assume_code(quality_estimated_from_quality(quality));                 //150130_040511
   return ET_ref;
}
//_calc_Penman_Monteith_CropSyst4___________________________________2009-09-28_/
static const float64 CP = 0.001013; // Specific heat of air.
//______________________________________________________________________________
float64 CalculateETRadTerm
(float64 Delta
, float64 Gamma
, float64 Lambda
, float64 Rn
, float64 rc
, float64 ra )
{
   float64 ET_Rad_Term = Delta * Rn / (Delta + Gamma * (1.0 + rc / ra));
   return ET_Rad_Term / Lambda;
}
//_CalculateETRadTerm__________________________________________________________/
float64 CalculateETAeroTerm
(float64 Delta
,float64 Gamma 
,float64 Lambda 
,float64 rc
,float64 ra
,float64 VPD 
,float64 tmean 
//, Elev As Single
, float64 P)
{
   /* RLN moved to caller
   static const float64  CP = 0.001013;
   float64 P = 101.3 * pow(((293 - 0.0065 * Elev) / 293) , 5.26);
   */
   float64 Tkv = 1.01 * (tmean + 273.0);
   float64 AirDensity = 3.486 * P / Tkv;
   float64 VolHeatCap = CP * AirDensity;
   float64 Aero_Term = (VolHeatCap*VPD / ra) / (Delta + Gamma * (1.0 + rc/ra));
   return Aero_Term / Lambda;
}
//_CalculateETAeroTerm_________________________________________________________/
float64 CalculateGamma
(float64 Lambda
,float64 P)
{  static const float64 water_to_air_molecular_weight_ratio = 0.622;
   float64 Gamma = CP * P / (water_to_air_molecular_weight_ratio * Lambda);
   return Gamma;
}
//_CalculateGamma______________________________________________________________/
#ifdef NOT_NEEDED
// Not needed gives the same value as mine 110128
float64 CalculateAeroRes(float64 Uz , float64 z)
{
   static const float64 VK = 0.41;
    float64 U2 = (z == 2)
      ? Uz
      : Uz * (4.87 / (log(67.8 * z - 5.42)));
   U2 = U2 * 86400.0; //Convert to m/day
   static const float64 d = 0.08;
   static const float64 zom = 0.01476;
   static const float64 zoh = 0.001476;
   static const float64 Zm = 2.0;
   static const float64 zh = 2.0;
   float64 Term1 = log((Zm - d) / zom);
   float64 Term2 = log((zh - d) / zoh);
   float64 test = Term1 * Term2 / (VK * VK * U2);
   return test;
}
//_CalculateAeroRes____________________________________________________________/
#endif
//Private Function Calculatef_arccos(X As Single) As Single 'New 042906:  X was defined as integer
//    Calculatef_arccos = Atn(-X / Sqr(-X * X + 1)) + 2 * Atn(1)
//End Function
//______________________________________________________________________________
float64 CalculateNetRad
(float64 Pot_Rad
,float64 Solar_Rad
,float64 Actual_VP
,float64 Tmax
,float64 Tmin )
{  //'Calculate shortwave net radiation
   static const float64 Albedo = 0.23;
   float64 Rns = (1 - Albedo) * Solar_Rad;
   // Calculate cloud factor
   float64 F_Cloud = 1.35 * (Solar_Rad / (Pot_Rad * 0.75)) - 0.35;
   // Calculate humidity factor
   float64 F_Hum = (0.34 - 0.14 * sqrt(Actual_VP));
   // Calculate Isothermal LW net radiation
   float64 LWR = Stephan_Boltzmann_MJ_K4_m2_day /*0.000000004903*/
               * (pow((Tmax + 273.15) ,4.0) + pow((Tmin + 273.15), 4.0)) / 2.0;
   float64 Rnl = LWR * F_Cloud * F_Hum;
   return Rns - Rnl;
}
//_CalculateNetRad_____________________________________________________________/
#if (CS_VERSION!=4)
float64 Reference_evapotranspiration_model_set
::calc_Penman_Monteith_CropSyst_VB(CORN::Quality &quality)                 const
{  float64 ET_ref = 0.0;
   static const float64 rc = 0.00081; //day/m

//'Define year and day
//Year = TimeState.CurrentYear
//Year_Count = TimeState.YearCount
//DOY = TimeState.DayOfYear

   float64 Tmax = air_temperature_max.Celcius();
   float64 Tmin = air_temperature_min.Celcius();
   float64 SRad = solar_radiation.get_MJ_m2();
   float64 tmean = air_temperature_avg.Celcius();                                // (Tmax + Tmin) / 2
   float64 Actual_VP = vapor_pressure_actual.get_kPa();                          // CalculateVP(Es_Tmax, Es_Tmin, RHmax, RHmin)
   float64 Vap_Pres_Def = avg_fullday_vapor_pressure_deficit.get_kPa();          // CalculateVPD(Es_Tmax, Es_Tmin, Actual_VP)
      // RLN Warning I am using either calculated from saturated vapor pressure or calculated from VPDmax
      // with preference for from VPDmax

   float64 Pot_Rad = 0.0;
   Pot_Rad = solar_radiation_extraterrestrial/*180822 ET_solar_radiation*/.get_MJ_per_m2_day();                        //150626
   net_radiation = calc_net_radiation(false);  // RLN mine, just for debug MJ/(m2 day)
   net_radiation = CalculateNetRad(Pot_Rad, SRad, Actual_VP, Tmax, Tmin);

   float64 wind_speed_m_d = wind_speed.get_m_d();                                //150908_101206
   aerodynamic_resistance = aerodynamics.calc_resistance_of_plants_d_m
      (wind_speed_m_d,0.12);                                                     //011008
      // meters reference crop plant height                                      //011008
   //Same as my implementation   aerodynamic_resistance = CalculateAeroRes(wind_speed_m_s , Screening_Height);   //Aero_Res = CalculateAeroRes(Wind, Screening_Height)
   float64 Slope_Sat_FN = vapor_pressure_saturation_slope;//_obsolete;//LML 140808 slope_of_saturated_vapor_pressure_curve; // CalculateDelta(tmean, Es_Tmean)
   //latent_heat_vaporization = calc_latent_heat_vaporization(tmean);  // CalculateLambda(tmean)
   float64 P = atmospheric_pressure.get_kPa();  // 101.3 * pow(((293 - 0.0065 * Altitude) / 293) , 5.26);
   float64 psychrometric_const = CalculateGamma(latent_heat_vaporization/*, Altitude*/,P);
   float64 Aero_Term = CalculateETAeroTerm
      (Slope_Sat_FN
      , psychrometric_const
      , latent_heat_vaporization
      , rc, aerodynamic_resistance
      , Vap_Pres_Def
      , tmean
      // , Altitude
      , P);
   float64 Rad_Term = CalculateETRadTerm
      (Slope_Sat_FN
      , psychrometric_const
      , latent_heat_vaporization
      , net_radiation
      , rc
      , aerodynamic_resistance);
   ET_ref = Aero_Term + Rad_Term;                                                //   PM_ET = Aero_Term + Rad_Term
   quality.assume_code(air_temperature_max
      .worst(air_temperature_min
      .worst(solar_radiation
      .worst(wind_speed                                                          //150908
      .worst(vapor_pressure_actual)))).get_quality_code());                      //140808LML_110113
   quality.assume_code(quality_estimated_from_quality(quality));                 //150121_040511
   //110113 We must use estimated or calculated from because
   //the user may have entered values which we don't want to override
   return ET_ref;
}
#endif
//_calc_Penman_Monteith_CropSyst_VB_________________________________2010-12-15_/
float64 Reference_evapotranspiration_model_set
::calc_Hargreaves_unadjusted(CORN::Quality &quality)                       const
{
   float64 radiation_extraterrestrial = 0.0;
   float64 sqrt_temperature_range = 0.0;
   sqrt_temperature_range
      = sqrt(air_temperature_max.Celcius() - air_temperature_min.Celcius());     //150727
   radiation_extraterrestrial = solar_radiation_extraterrestrial.get_MJ_per_m2_day(); //150626_110414_101206
   float64 ET_hargreaves_unadjusted = 0.0023 * (air_temperature_avg + 17.8)
       * inverse_latent_heat_flux_vaporization                                   //131105 011008
       * sqrt_temperature_range
       * radiation_extraterrestrial;                                             //110414_101206
   quality.assume_code(quality_estimated_from_quality(air_temperature_max.worst(air_temperature_min)));      //104020
   return ET_hargreaves_unadjusted;
}
//_calc_Hargreaves_unadjusted_______________________________________2009-09-28_/
float64 Reference_evapotranspiration_model_set
::calc_Hargreaves_adjusted(CORN::Quality &quality)                         const
{  float64 Hargreaves_adjusted = 0.0;
   if (Hargreaves_parameter_set)
   {  float64 Hargreaves_unadjusted = provide_Hargreaves_unadjusted(quality);    //100629
      Hargreaves_adjusted
      = Hargreaves_unadjusted * Hargreaves_ET_slope + Hargreaves_ET_intercept;   //100629
   } else quality.assume_code(CORN::unknown_quality);                            //150130_100629
   return  Hargreaves_adjusted;                                                  //100629
}
//_calc_Hargreaves_adjusted_________________________________________2009-09-28_/
bool Reference_evapotranspiration_model_set::start_day()
{
   for (int ETm = 0; ETm < ET_Model_COUNT; ETm ++)
   {  // Invalidate provided values
      ET_ref_raw[ETm].invalidate();                                              //110415
   }
   provided = 0;
   // Update common terms:
   // saturation_vapor_pressure_deficit; // kPa
   // reference_canopy_resistance; // day/m for reference grass
   // reference_canopy_resistance is allowed to be variable
   // because CO2 simulations will need to adjust this value
   // Current values used in more than one of the ET models.

   float64 wind_speed_m_d = wind_speed.get_m_d();                                //150727
   wind_speed_at_standard_height_m_s =
      aerodynamics.calc_normalized_wind_speed(wind_speed_m_d/seconds_per_day);
   soil_heat_flux_density     = 0.0; // 0.0 when full day  (if daylight, a value of 1.0) would be used.      // MJ/(m2 day)
   net_radiation              = calc_net_radiation(false);  // MJ/(m2 day) at crop surface
   latent_heat_vaporization.update();                                            //140512
   vapor_pressure_saturation_slope.update();                                     //140519
   psychrometric_constant.update();                                              //140512
   Penman_radiation_term    = calc_Penman_radiation_term();  // Must be calculated last!         //090929
   avg_fullday_vapor_pressure_deficit_kPa
      = avg_fullday_vapor_pressure_deficit.get_kPa();
   return                                                                        //101206
      (  air_temperature_max       .get_quality().is_better_than(CORN::not_valid_quality)
      && air_temperature_min       .get_quality().is_better_than(CORN::not_valid_quality)
      && solar_radiation           .get_quality().is_better_than(CORN::not_valid_quality)
      && wind_speed                .get_quality().is_better_than(CORN::not_valid_quality)
      && avg_fullday_vapor_pressure_deficit.get_quality().is_better_than(CORN::not_valid_quality));
}
//_start_day___________________________________________________________________/
#define provide_XXXX(model_enum_index,provide_method_name,calc_method_name)    \
float64 Reference_evapotranspiration_model_set::provide_method_name            \
(CORN::Quality &return_quality) provision_                                     \
{                                                                              \
   if (!(provided & (1 << model_enum_index)))                                  \
   {  ET_ref_raw[model_enum_index].set_mm(calc_method_name(ET_ref_raw[model_enum_index]));\
      provided |= (1 << model_enum_index);                                     \
   }                                                                           \
   return_quality.assume(ET_ref_raw[model_enum_index].get_quality());          \
   return    ET_ref_raw[model_enum_index].get_mm();                            \
}
//______________________________________________________________________________
provide_XXXX(ET_Penman                    ,provide_Penman                     ,calc_Penman)
provide_XXXX(ET_Penman_Monteith           ,provide_Penman_Monteith            ,calc_Penman_Monteith)
provide_XXXX(ET_FAO_56_Penman_Monteith    ,provide_FAO_56                     ,calc_FAO_56)
provide_XXXX(ET_ASCE_Penman_Monteith      ,provide_ASCE                       ,calc_ASCE)
provide_XXXX(ET_ASEC_EWRI_Penman_Monteith_short_crop  ,provide_ASCE_EWRI_short_crop ,calc_ASCE_EWRI_short_crop)
provide_XXXX(ET_ASEC_EWRI_Penman_Monteith_tall_crop ,provide_ASCE_EWRI_tall_crop  ,calc_ASCE_EWRI_tall_crop)
provide_XXXX(ET_Priestley_Taylor          ,provide_Priestly_Taylor            ,calc_Priestly_Taylor)
provide_XXXX(ET_Hargreaves_unadjusted     ,provide_Hargreaves_unadjusted      ,calc_Hargreaves_unadjusted)
provide_XXXX(ET_Hargreaves_adjusted       ,provide_Hargreaves_adjusted        ,calc_Hargreaves_adjusted)
provide_XXXX(ET_FAO_56_Penman_Monteith_CropSyst       ,provide_Penman_Monteith_CropSyst4  ,calc_Penman_Monteith_CropSyst4)
#if (CS_VERSION!=4)
provide_XXXX(ET_CropSyst_VB               ,provide_Penman_Monteith_CropSyst_VB,calc_Penman_Monteith_CropSyst_VB)
#endif
//______________________________________________________________________________
void Reference_evapotranspiration_model_set
::known_for_Hargreaves_model
(float64 _ET_slope
,float64 _ET_intercept)                                            modification_
{  Hargreaves_ET_slope     = _ET_slope;
   Hargreaves_ET_intercept = _ET_intercept;
   Hargreaves_parameter_set= true;
}
//_known_for_Hargreaves_model__________________________________________________/
float64 Reference_evapotranspiration_model_set
::calc_daily_Priestley_Taylor_constant()                                   const
{  CORN::Quality_clad total_quality;
   float64 total_ET = calc_FAO_56(total_quality,Reference_evapotranspiration_model_set::include_total_terms);
   float64 radiation_ET = calc_FAO_56(total_quality,Reference_evapotranspiration_model_set::include_radiation_term);
   float64 PT_constant = total_ET / radiation_ET;
   return  PT_constant;
}
//_calc_daily_Priestley_Taylor_constant________________________________________/
void Reference_evapotranspiration_model_set::dump_header(std::ofstream &strm)
{  ET_Reference_common::dump_header(strm);
   strm  << "WindSp\t" << "Tavg\t"<< "VPDfullday\t" <<  "SlopeVPSat\t" << "Pschro\t" << "LatentHeatVap\t" << "aeroresist\t"<< "ETref\t";
}
//______________________________________________________________________________
void Reference_evapotranspiration_model_set::dump_detail(std::ofstream &strm)
{  ET_Reference_common::dump_detail(strm);
   CORN::Quality_clad ET_quality;
   float64 ET_ref =
   #if (CS_VERSION==5)
   provide_Penman_Monteith_CropSyst_VB(ET_quality);
   #else
   0.0;
   #endif
        strm
        << wind_speed_at_standard_height_m_s       << "\t"
        << air_temperature_avg                     << "\t"
        <<  avg_fullday_vapor_pressure_deficit_kPa << "\t"
        << vapor_pressure_saturation_slope /*140512 slope_of_saturated_vapor_pressure_curve.get_kPa_C*/  << "\t"
        << psychrometric_constant                     << "\t"
        << latent_heat_vaporization                <<"\t"
        << aerodynamic_resistance                  << "\t"
        <<  ET_ref   << "\t";
}
//_dump_detail_____________________________________________________________________________

