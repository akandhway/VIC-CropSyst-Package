#include "weather/parameter/WC_vapor_pressure_act.h"

#include "weather/weather_types.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"
#include "weather/parameter/WP_air_temperature.h"
#include "weather/parameter/WP_relative_humidity.h"
#include "weather/parameter/WP_dew_point_temperature.h"
using namespace CORN;
//______________________________________________________________________________
Vapor_pressure_actual_calculator::Vapor_pressure_actual_calculator
   (Vapor_pressure_deficit_max      *vapor_pressure_deficit_max_
   ,Vapor_pressure_deficit          *vapor_pressure_deficit_                     //080617
   ,Dew_point_temperature_maximum   *dew_point_temperature_max_   // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_calculator
   ,Dew_point_temperature_minimum   *dew_point_temperature_min_   // May be 0 if unknown
   ,Relative_humidity               *relative_humidity_max_       // May be 0 if unknown
   ,Relative_humidity               *relative_humidity_min_       // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_calculator
   ,Air_temperature_maximum         *air_temperature_max_         // May be 0 if unknown
   ,Air_temperature_minimum         *air_temperature_min_         // May be 0 if unknown
   ,Dew_point_temperature_average   *dew_point_temperature_avg_)                 //100302
: Vapor_pressure_actual()                                                        //140220
, Vapor_pressure_parameter
   (dew_point_temperature_max_   // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
   ,relative_humidity_min_       // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
   ,air_temperature_max_         // May be 0 if unknown
   ,air_temperature_min_)        // May be 0 if unknown
,vapor_pressure_deficit_max                        (vapor_pressure_deficit_max_) //050506
,vapor_pressure_deficit                                (vapor_pressure_deficit_) //080617
,dew_point_temperature_min                          (dew_point_temperature_min_) //050506
,relative_humidity_max                                  (relative_humidity_max_) //050506
,dew_point_temperature_avg                          (dew_point_temperature_avg_) //100230
{}
//_Vapor_pressure_actual_calculator:constructor________________________________/
Vapor_pressure_actual_calculator::Vapor_pressure_actual_calculator
(Vapor_pressure_deficit_max      &vapor_pressure_deficit_max_
,Relative_humidity               &relative_humidity_max_
,Relative_humidity               &relative_humidity_min_
,Air_temperature_maximum         &air_temperature_max_
,Air_temperature_minimum         &air_temperature_min_)
: Vapor_pressure_actual()                                                        //140220
, Vapor_pressure_parameter
   (0   // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
   ,&relative_humidity_min_       // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_calculator
   ,&air_temperature_max_     // May be 0 if unknown
   ,&air_temperature_min_)     // May be 0 if unknown
,vapor_pressure_deficit_max                       (&vapor_pressure_deficit_max_) //050506
,dew_point_temperature_min                                                   (0) //050506
,relative_humidity_max                                 (&relative_humidity_max_) //050506
,dew_point_temperature_avg                                                   (0) //100320
{}
//_Vapor_pressure_actual_calculator:constructor________________________________/
Vapor_pressure_actual_calculator::Vapor_pressure_actual_calculator
(Vapor_pressure_deficit_max      &vapor_pressure_deficit_max_
,Air_temperature_maximum         &air_temperature_max_
,Air_temperature_minimum         &air_temperature_min_
,Vapor_pressure_deficit          *vapor_pressure_deficit_) // May be 0 if unknown
: Vapor_pressure_actual                                                       () //140220
, Vapor_pressure_parameter
   (0   // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_calculator
   ,0       // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_calculator
   ,&air_temperature_max_     // May be 0 if unknown
   ,&air_temperature_min_)     // May be 0 if unknown
,vapor_pressure_deficit_max                       (&vapor_pressure_deficit_max_)
,dew_point_temperature_min                                                   (0)
,relative_humidity_max                                                       (0)
,dew_point_temperature_avg                                                   (0)
,vapor_pressure_deficit                                (vapor_pressure_deficit_)
   // fullday May be 0 if unknown (used calculating from VPD)
{}
//_Vapor_pressure_actual_calculator_________________________________2010-12-08_/
float64 Vapor_pressure_actual_calculator::calc_from_dew_point
(float64 min_dew_point_temp
,float64 max_dew_point_temp
,float64 min_air_temp                                                            //050831
,float64 max_air_temp                                                            //050831
,bool over_water_only) const                                                     //040114
{  float64 act_VP_at_Tdewmin = calc_saturation_kPa(min_dew_point_temp,min_air_temp,over_water_only);
   float64 act_VP_at_Tdewmax = calc_saturation_kPa(max_dew_point_temp,max_air_temp,over_water_only);
   return (act_VP_at_Tdewmin + act_VP_at_Tdewmax) / 2.0;                         //011008
}
//_calc_from_dew_point______________________________________________2002-07-25_/
float64 Vapor_pressure_actual_calculator::calc_from_relative_humidity
(float64 min_rel_humid
,float64 max_rel_humid
,float64 min_temp
,float64 max_temp
,bool over_water_only)                                              calculation_
{  float64 sat_VP_at_max_temp = calc_saturation_kPa(max_temp,max_temp,over_water_only);   //011008
   float64 sat_VP_at_min_temp = calc_saturation_kPa(min_temp,min_temp,over_water_only);   //011008
      float64 act_vapor_pressure =
         ((sat_VP_at_min_temp * max_rel_humid/100.0)
         +(sat_VP_at_max_temp * min_rel_humid/100.0))/2.0;
      return act_vapor_pressure;
}
//_calc_from_relative_humidity______________________________________2002-07-25_/
bool Vapor_pressure_actual_calculator::is_valid_dew_point_temperature() affirmation_
{  if (!dew_point_temperature_max)  return false;
   if (!dew_point_temperature_min)  return false;
   return   dew_point_temperature_max->is_valid()
         && dew_point_temperature_min->is_valid();
}
//_is_valid_dew_point_temperature___________________________________2008-10-29_/
bool Vapor_pressure_actual_calculator::is_relative_humidity_better_than_derived() affirmation_
{  if (!relative_humidity_max)  return false;
   if (!relative_humidity_min)  return false;
   bool RHmax_qual_better_than_derived = relative_humidity_max->is_valid();
   bool RHmin_qual_better_than_derived = relative_humidity_min->is_valid();
   return RHmax_qual_better_than_derived
       && RHmin_qual_better_than_derived;
}
//_is_better_than_derived_relative_humidity_________________________2010-04-12_/
bool Vapor_pressure_actual_calculator::is_valid_temperature()       affirmation_
{  if (!air_temperature_max)  return false;
   if (!air_temperature_min)  return false;
   return air_temperature_max->is_valid()
       && air_temperature_min->is_valid();
}
//_is_valid_temperature_____________________________________________2008-10-29_/
const CORN::Quality &Vapor_pressure_actual_calculator::update()   rectification_
{  float64 quiet_NaN = 0.0; // This should be NaN but I need to trace down daily update error  std::numeric_limits<float64>::quiet_NaN();                //150123
   if (!updating)                                                                //081027
   {  updating = true;                                                           //081027
   float64 act_vapor_pressure = 0.0; // kPa   (ea)
   CORN::Quality_clad new_quality(CORN::missing_quality);                        //150123_050616
   float64 DPmax = dew_point_temperature_max ? dew_point_temperature_max->Celcius() : quiet_NaN;//150123_081106 //150121
   float64 DPmin = dew_point_temperature_min ? dew_point_temperature_min->Celcius() : quiet_NaN;//150123_081106 //150121
   float64 RHmax = relative_humidity_max     ? relative_humidity_max->percent()     : quiet_NaN;//150123_081106
   float64 RHmin = relative_humidity_min     ? relative_humidity_min->percent()     : quiet_NaN;//150123_081106
   float64 Tmax  = air_temperature_max       ? air_temperature_max->Celcius()       : quiet_NaN;//150123_081106
   float64 Tmin  = air_temperature_min       ? air_temperature_min->Celcius()       : quiet_NaN;//150123_081106
   float64 DPavg = dew_point_temperature_avg ? dew_point_temperature_avg->Celcius() : quiet_NaN;//150123_100322
   if (is_valid_dew_point_temperature()                                          //011008
       && dew_point_temperature_max->is_better_than(*relative_humidity_max)      //150123_011008
       && dew_point_temperature_min->is_better_than(*relative_humidity_min))     //150123_011008
   {  // If the dew point temperature qualities are better than relative         //011008
      // humidity calculate actual vapo _pressure from dew point;                //011008
      // Not sure why these are called act_VP_at_Tdewmin instead of sat_VP_at_Tdewmin //020312
                  // May be a typo, but I think it may be just that it is an estimate.
//040114             RLN check with Claudio: I don't know if we will want to consider over ice here, we originally only had OVER_WATER_ONLY!!!!
      act_vapor_pressure = calc_from_dew_point                                   //020725
         (DPmin,DPmax,Tmin,Tmax,OVER_WATER_ONLY);
      new_quality.assume(dew_point_temperature_max->worst(*dew_point_temperature_min));                          //150123_020723
   }
   else                                                                          //011008
   {  // Calculate from relative humidity
      if (is_relative_humidity_better_than_derived())                            //100312
      {  act_vapor_pressure // kPa   (ea)
           = calc_from_relative_humidity(RHmin,RHmax,Tmin,Tmax,OVER_WATER_ONLY); //020725
         new_quality.assume(relative_humidity_min->worst(*relative_humidity_max));//150123_020723
      } else if (is_valid_temperature())
      {  float64 VPD = vapor_pressure_deficit ? vapor_pressure_deficit->get_kPa():  quiet_NaN;  //150123_020724_101208
         if (vapor_pressure_deficit->is_valid())
         {
            act_vapor_pressure = calc_from_vapor_pressure_deficit                //080617
               (VPD,Tmax,Tmin ,OVER_WATER_ONLY);                                 //080617
            new_quality.assume(*vapor_pressure_deficit);                         //150123_081024
         }                                                                       //080617
         else if (dew_point_temperature_avg->is_valid())  // This was added for Troy Peters Washington Irrigation Guide //100322
         {  act_vapor_pressure = calc_from_dew_point_avg(DPavg,OVER_WATER_ONLY); //100322
            new_quality.assume_code(quality_calculated_from_quality(*dew_point_temperature_avg)); //150123
   }  }  }
   update_kPa(act_vapor_pressure);                                               //150123
   assume(new_quality);                                                          //150123
   updating = false;                                                             //081027
   }
   return get_quality();
}
//_update___________________________________________________________2008-09-08_/
float64 Vapor_pressure_actual_calculator
   ::calc_from_vapor_pressure_deficit
   (float64 VPD
   ,float64 max_temp
   ,float64 min_temp
   ,bool over_water_only)                                           calculation_ //040114
{  float64 sat_VP_at_max_temp = calc_saturation_kPa(max_temp,max_temp,over_water_only);
   float64 sat_VP_at_min_temp = calc_saturation_kPa(min_temp,min_temp,over_water_only);
   float64 vapor_atmospher_can_hold = (sat_VP_at_max_temp + sat_VP_at_min_temp) / 2.0; //080908
   float64 act_VP = vapor_atmospher_can_hold - VPD;
if (act_VP < 0.001)
act_VP = 0.001;
   return act_VP;
}
//_calc_from_vapor_pressure_deficit_________________________________2008-06-17_/
float64 Vapor_pressure_actual_calculator
   ::calc_from_dew_point_avg(float64 DPavg,bool over_water_only)    calculation_
{  float64 act_VP = calc_saturation_kPa(DPavg,DPavg,over_water_only);
   return act_VP;
}
//_calc_from_dew_point_avg__________________________________________2010-03-22_/

