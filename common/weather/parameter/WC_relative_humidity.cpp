#include "weather/parameter/WC_relative_humidity.h"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/quality.hpp"
#include "weather/weather_types.h"
#include "weather/loc_param.h"

#include "weather/parameter/WP_precipitation.h"

#include "weather/parameter/WP_air_temperature.h"
#include "weather/parameter/WP_dew_point_temperature.h"
#include "weather/parameter/WP_vapor_pressure_deficit.h"
#include "weather/parameter/WP_humidity_specific.h"
#include "weather/parameter/WP_atmospheric_pressure.h"

/* 040114_ RLN:
According to the World Meteorological Organization WMO
RH is calculated by computing the denominoator (or maximum possible water vapor content)
over water rather than ice regardless of the temperature.
This results in a higher value of maximum possible water
vapor contant at any given temperature.
Apparently this is  to prevent the occassional possibility of RH values
in exccess of 100% when the atmosphere is in a super-cooled state.
*/
using namespace CORN;

#ifdef DETAIL_TRACE
#include <fstream>
using namespace std;
extern int trace_indent;
extern std::ofstream trace;
#endif
//______________________________________________________________________________
Relative_humidity_calculator::Relative_humidity_calculator
( Dew_point_temperature_average *dew_point_temperature_avg_
, Vapor_pressure_deficit_max    *vapor_pressure_deficit_max_                     //081016
,Air_temperature_maximum        *air_temperature_max_)                           //081016
: Relative_humidity()                                                            //140220
, dew_point_temperature_avg                         (dew_point_temperature_avg_)
, vapor_pressure_deficit_max                       (vapor_pressure_deficit_max_) //081016
, air_temperature_max                                     (air_temperature_max_)
, generation_parameters(0)                                                       //091216
{}
//_Relative_humidity_calculator:constructor____________________________________/
float64 Relative_humidity_calculator::set_value_quality                          //150121
(float64 _value
,const CORN::Quality &new_quality
,bool override_safety)                                                 mutation_ //090312
{  // Require Rel humidity to be between 0 and 100.
   // We don't even allow valid values to be out of range
   // but if it is valid sofar, change the quality to indicate that it has been interpolated.
   // we do preserve measured values at this point. (probably should mark it out of range, but this is left to validator).
   float64 result = 0;                                                           //081027
   if (new_quality.is_valid())
   {  CORN::Quality_code swapped_quality = new_quality.get_quality_code();       //110113
      if (_value > max_valid_relative_humidity)
      {  _value = max_valid_relative_humidity; swapped_quality = quality_interpolated_from_quality(new_quality); }
      else if (_value < min_valid_relative_humidity)
      {  _value =min_valid_relative_humidity;  swapped_quality = quality_interpolated_from_quality(new_quality); }
      result = update_value_qualified_code(_value,swapped_quality);              //150121_081027
   }
   return result;
}
//_set_value_quality___________________________________________________________/
float64 Relative_humidity_extreme_calculator::calc_from_humidity_specific
(CORN::Quality &RHxxx_quality)                                             const
{
   static float64 constant_B = 0.621991; // Epsilon
   if (!humidity_specific || !atmospheric_pressure)
      return 0.0;
   //float64 humid_spec = humidity_specific->kg_kg();
   float64 mixing_ratio = humidity_specific->calc_mixing_ratio_kg_kg(); // humid_spec / (1.0 + humid_spec);
   float64 water_vapor_pressure
      = (mixing_ratio * atmospheric_pressure->get_kPa())
       /(mixing_ratio + constant_B);
   float64 Txxx = ref_temperature_air_associated().Celcius();
   float64 VP_sat = Physical::calc_saturation_kPa(Txxx,Txxx,false);

   //abandoned float64 VP_sat = get_VP_sat();
   /*
   saturation_water_vapor_pressure
      = 0.6108*EXP(17.27 * temp_air / (237.3 + temp_air))
   */
   //RHxxx_quality.assume_code(CORN::calculated_quality);
   RHxxx_quality.assume_code(CORN::quality_calculated_from_quality
      (humidity_specific->worst(ref_temperature_air_associated())));
      // Actually this should be calculated from the temperature quality
      //
   float64 RH = 100.0 * water_vapor_pressure  / VP_sat;
   return RH;
}
//_Vapor_pressure_daytime_calculator::calc_from_humidity_specific___2018-08-09_/

//==============================================================================
Relative_humidity_maximum_calculator::Relative_humidity_maximum_calculator
(float64 value_, const CORN::Quality  & quality_)
: Relative_humidity_extreme_calculator                        (value_, quality_)
, air_temperature_min                                                        (0)
, unlimited_RHmax_window                                                     (0) //100119
,precipitation                                                               (0) //101215
, when_precip_set_100                                                     (true)
{  // Use this constructor when relative humidity is known.
}
//_Relative_humidity_maximum_calculator:constructor____________________________/
Relative_humidity_maximum_calculator::Relative_humidity_maximum_calculator
(Air_temperature_maximum        *_air_temperature_max                            //081016
,Air_temperature_minimum        *_air_temperature_min
,Dew_point_temperature_minimum  *_dew_point_temperature_min
,Vapor_pressure_deficit_max     *_vapor_pressure_deficit_max                     //081016
,Dew_point_temperature_average  *_dew_point_temperature_avg
,Vapor_pressure_nighttime       *_vapor_pressure_nighttime                       //081030
,Precipitation                  *_precipitation                                  //101215
)
: Relative_humidity_extreme_calculator
   (_dew_point_temperature_avg,_vapor_pressure_deficit_max,_air_temperature_max)
, air_temperature_min(_air_temperature_min)
, dew_point_temperature_min(_dew_point_temperature_min)
, vapor_pressure_nighttime(_vapor_pressure_nighttime)                            //081030
, unlimited_RHmax_window(0)                                                      //100119
, when_precip_set_100                                                    (false)
, precipitation(_precipitation)                                                  //100215
, precipitation_today(false)                                                     //100119
{}
//_Relative_humidity_maximum_calculator:constructor____________________________/
Relative_humidity_maximum_calculator::Relative_humidity_maximum_calculator
(Air_temperature_maximum          *_air_temperature_max                          //081016
,Air_temperature_minimum          *_air_temperature_min
,Dew_point_temperature_minimum   *_dew_point_temperature_min
,Vapor_pressure_deficit_max      *_vapor_pressure_deficit_max                    //081016
,Precipitation                   *_precipitation)                                //101215
: Relative_humidity_extreme_calculator(0,_vapor_pressure_deficit_max,_air_temperature_max)
, air_temperature_min(_air_temperature_min)
, dew_point_temperature_min(0)
, vapor_pressure_nighttime(0)                                                    //081030
, unlimited_RHmax_window(0)                                                      //100119
, precipitation(_precipitation)                                                  //100215
, when_precip_set_100                                                    (false)
, precipitation_today(false)                                                     //100119
{}
//_Relative_humidity_maximum_calculator:constructor____________________________/
Relative_humidity_maximum_calculator::Relative_humidity_maximum_calculator
(Air_temperature_minimum         *_air_temperature_min
,Dew_point_temperature_average   *_dew_point_temperature_avg
,Precipitation                   *_precipitation)                                //101215
: Relative_humidity_extreme_calculator(_dew_point_temperature_avg,NULL,NULL)
, air_temperature_min(_air_temperature_min)
, dew_point_temperature_min(0)
, vapor_pressure_nighttime(0)                                                    //081030
, unlimited_RHmax_window(0)
, when_precip_set_100                                                    (false)
, precipitation(_precipitation)                                                  //100215
{}
//_Relative_humidity_maximum_calculator:constructor_____________________________
Relative_humidity_maximum_calculator::Relative_humidity_maximum_calculator
(Air_temperature_minimum       *air_temperature_min_ // required
,const Humidity_specific       *humidity_specific_
,const Atmospheric_pressure    *atmospheric_pressure_)
: Relative_humidity_extreme_calculator
   (humidity_specific_
   ,atmospheric_pressure_)
, dew_point_temperature_min(0)
, vapor_pressure_nighttime(0)                                                    //081030
, unlimited_RHmax_window(0)
, when_precip_set_100                                                    (false)
, precipitation(0)
{
   air_temperature_min = air_temperature_min_;
}
//_Relative_humidity_maximum_calculator:constructor_________________2018-08-10_/
const CORN::Quality & Relative_humidity_maximum_calculator::update()     rectification_
{  if (is_empty())  // this is a litle hack until I fix setting invalid quality in the UED weather editor 091204
      assume_code(missing_quality);                                              //150121_091204
   if (is_valid())                                                               //081028
   {}
   else
   {  if (updating)                                                              //081027
      {  // We are already updating, so don't come here yet again
      } else
      {  updating = true;                                                        //081027
         CORN::Quality_clad RHmax_quality;
         float64 RHmax = calc_from_DewPt_min(RHmax_quality);
         if (RHmax_quality.is_valid())                                           //150203
            Relative_humidity_calculator::set_value_quality(RHmax,RHmax_quality);//150121
         else
         {  RHmax = calc_from_humidity_specific(RHmax_quality);                  //180813
            Relative_humidity_calculator::set_value_quality(RHmax,RHmax_quality);//180811
            // Quality should be setup in calc_from_humidity_specific
         }
         if (RHmax_quality.is_optimal())                                         //081104
         {   // we have the best quality no need to check other methods.
         } else
         {  RHmax = est_from_vapor_pressure_nighttime(RHmax_quality);            //081104
            Relative_humidity_calculator::set_value_quality(RHmax,RHmax_quality);//150121

            //091212  I have reinstated this for lack of anything else, but Claudio says this is generally not so good.
            if (dew_point_temperature_avg)                                       //081016
            {  //CORN::Quality RHmax_quality;
               RHmax = est_from_avg_dew_point_temperature(RHmax_quality);
               // This is special for the agrimet data sets
               // we presume the calculated relative humidity is real
               // otherwise the recalculation throws things off.
               Relative_humidity_calculator::set_value_quality(RHmax,RHmax_quality); // force //150121
            }

         }
         float64 precipitation_mm =  precipitation ? precipitation->get_mm() : 0.0;
         bool precipitation_today = precipitation_mm > 0.0;                      //101212
         {  unlimited_RHmax_window += (int)precipitation_mm/10;
            // give a day of unlimited RH for each mm of precipitation
            // this is assuming that the more precipitation, the more
            // humid the whole time period could be
            precipitation_today = precipitation_mm > 0.0 ;
            if (precipitation_today) // Give at least 1 extra day if there is only negligable precip
            {  unlimited_RHmax_window += 1;
            } else
               if (unlimited_RHmax_window)
                   unlimited_RHmax_window -= 1;
         }
         if (!unlimited_RHmax_window && generation_parameters)                   //150203_091217
            constrain_to_range  // Limit to reasonable values                    //150123
               (generation_parameters->typical[1][*current_season_known][0]      // typical low
               ,generation_parameters->typical[1][*current_season_known][1]);    // typical high
         constrain_to_range(15.0,100.0); // absolute limit
         if (when_precip_set_100 && precipitation_today) force_amount(100.0);    //200228_150125_100119
         updating = false;                                                       //081027
      }
   }
   return get_quality();
}
//_Relative_humidity_maximum_calculator::update_____________________2008-09-08_/
float64 Relative_humidity_maximum_calculator::est_from_vapor_pressure_nighttime(CORN::Quality &RHmax_quality) const
{  RHmax_quality.assume_code(unknown_quality);                                   //150123
   float64 est_RHmax = percent();                                                //091216
   if (vapor_pressure_nighttime && air_temperature_min)                          //081030
   {  // Estimate from vapor pressure minimum and air temperature minimum  WARNING Durning generation this method should have better quality than the first (when calculating from temperature only)

      float64 VP_nighttime = vapor_pressure_nighttime ->get_value_update_if_necessary();
      float64 Tmin         = air_temperature_min      ->get_value_update_if_necessary();
      Quality_clad est_quality(quality_estimated_from_quality(vapor_pressure_nighttime->worst(*air_temperature_min)));
      if (est_quality.is_better_than(get_quality()))
      {  RHmax_quality.assume(est_quality);
         float64 VPsat_nighttime = vapor_pressure_nighttime->calc_saturation_kPa(Tmin,Tmin,false);
         est_RHmax = (VP_nighttime / VPsat_nighttime) * 100.0;
      } else
      {
      }
   } else
   {  RHmax_quality.assume_code(missing_quality);
   }
   return est_RHmax;
}
//_est_from_VP_nighttime____________________________________________2009-12-12_/
float64 Relative_humidity_maximum_calculator::calc_from_DewPt_min(CORN::Quality &RHmax_quality) const
{  // Determine what the calc'ed max rel humidity would be

//TRACE          trace << std::string(" ",trace_indent) << "RHmax calculating from VPsat_at_DewPtmax" << endl; trace_indent += 1;
   RHmax_quality.assume_code(unknown_quality);                                   //081104
   float64 RHmax = 0.0;                                                          //081104
   if (air_temperature_min && dew_point_temperature_min)                         //081104
   {  float64 DewPtmin=dew_point_temperature_min->get_value_update_if_necessary();//150126_081104
      float64 min_temp=air_temperature_min      ->get_value_update_if_necessary();//150126_980306
      const CORN::Quality &contrib_quality = dew_point_temperature_min->worst
         (*air_temperature_min);                                                 //150126 081104
      if (contrib_quality.is_valid())                                            //991014
      {
         RHmax_quality.assume(contrib_quality);                                  //150123_001107
      // Dew point and relative humidity are always the same quality
      // (in the real world measured relative humidity is actually typically calculateded directly from dewpoint temperature)

         // RLN modified for relative humidity in subzero temperature (ref: RH&WMO)
         Vapor_pressure_base vapor_pressure;                                     //050602
         float64 VPsat_at_DewPtmin =  vapor_pressure.calc_saturation_kPa (DewPtmin, min_temp,OVER_WATER_ONLY); //040114
         float64 VPsat_at_Tmin =  vapor_pressure.calc_saturation_kPa(min_temp,min_temp,OVER_WATER_ONLY);       //040114
         RHmax =  VPsat_at_DewPtmin / VPsat_at_Tmin * 100.0;                     //980305

//TRACE                trace << std::string(" ",trace_indent);
//TRACE                trace << "RHmax = VPsat_at_DewPtmin / VPsat_at_Tmin * 100 [limit range 15 to 100]";
//TRACE                trace << '\t' << RHmax << " = " << VPsat_at_DewPtmin << " / " << VPsat_at_Tmin << " * 100.0";
//TRACE                trace << '\t' << "RHmax<" << (int)(RHmax_quality.get_int32()) << "> same quality as VPsat_at_DewPtmin qual <" << (int)(contrib_quality.get_int32()) << ">" << endl;
//TRACE                trace << endl;

      } else
      {  RHmax_quality.assume_code(not_valid_quality);
//TRACE                trace << std::string(" ",trace_indent) << "RHmax not valid because min_temp and/or DewPtmin are not valid" << endl;
      }
   } else                                                                        //081104
   {  RHmax_quality.assume_code(missing_quality);
//TRACE             trace << std::string(" ",trace_indent) << "RHmax missing because Tmax and DewPtmin are not available" << endl;
   }
//TRACE trace_indent -= 1;
   return RHmax;                                                                 //970723
}
//_Relative_humidity_maximum_calculator::calc_from_DewPt_min________2018-08-08_/
float64 Relative_humidity_minimum_calculator::calc_from_VPsat_at_max_dew_point
(CORN::Quality &RHmin_quality)                                             const
{ // Determine what the calc'ed min rel humidity would be
//TRACE          trace << std::string(" ",trace_indent) << "RHmin calculating from VPsat_at_DewPtmax" << endl; trace_indent += 1;
   RHmin_quality.assume_code(unknown_quality);                                   //081028
   float64 RHmin = 0.0;
   if (air_temperature_max   && vapor_pressure_deficit_max)                      //081028
   {
      float64 tmax = air_temperature_max->get_value_update_if_necessary();       //150126
      float64 VPsat_DewPtmax =                                                   //040114
        vapor_pressure_deficit_max->saturation_at_max_dew_point(OVER_WATER_ONLY);//050602
      const CORN::Quality &contrib_quality = vapor_pressure_deficit_max->get_quality().worst(air_temperature_max->get_quality());   //081104
      if (contrib_quality.is_valid())                                            //991014
      {
         RHmin_quality.assume_code(quality_calculated_from(quality_calculated_from_quality(contrib_quality/*.get_code()*/) )); //150121_001107
         // Don't bother calculating if the input data is unusable, but we also don't want to override real data
         // Dew point and relative humidity are always the same quality (the calculation is one to one)
         // (in the real world measured relative humidity is actually typically calculated directly from dewpoint temperature)

            /* Have one more calculation in quality because this requires and additional calculation */
         Vapor_pressure_base vapor_pressure;                                     //050602
         float64 VPsat_at_Tmax = vapor_pressure.calc_saturation_kPa(tmax,tmax,OVER_WATER_ONLY); // Yes max_temperature!
         if (VPsat_at_Tmax == 0.0)                                               //020530
             VPsat_at_Tmax = 0.00001; // Avoid div 0 error
         RHmin = VPsat_DewPtmax / VPsat_at_Tmax * 100.0;                         //970723

//TRACE                trace << std::string(" ",trace_indent);
//TRACE                trace << "RHmin = VPsat_DewPtmax / VPsat_at_Tmax * 100 [limit range 10 to 100]";
//TRACE                trace << '\t' << RHmin << " = " << VPsat_DewPtmax << " / " << VPsat_at_Tmax << " * 100.0";
//TRACE                trace << '\t' << "RHmin<" << (int)RHmin_quality.get_int32() << "> calced from VPsat_DewPtmax qual <" << (int)Tmax_quality.get_int32() << "> and Tmax qual <" << Tmax_quality.get_int32() << ">" << endl;
//TRACE                trace << endl;

      } else                                                                     //081104
      {  RHmin_quality.assume_code(not_valid_quality);                                   //081104
//TRACE                trace << std::string(" ",trace_indent) << "RHmin not valid because Tmax and VPDmax are not valid" << endl;
      }
   } else                                                                        //081030
   {  RHmin_quality.assume_code(missing_quality);                                        //081104
//TRACE             trace << std::string(" ",trace_indent) << "RHmin missing because Tmax and VPDmax are not available" << endl;
   }
//TRACE trace_indent -= 1;
   return RHmin;                                                                 //970723
}
//_calc_from_VPsat_at_max_dew_point____________________________________________/
float64 Relative_humidity_minimum_calculator::est_from_VPD_max
(CORN::Quality &ested_RHmin_quality)                                       const
{                                           // rename this to calc_from_VPD_max
//TRACE          trace << std::string(" ",trace_indent) << "RHmin estimating from VPDmax" << endl; trace_indent += 1;
   ested_RHmin_quality.assume_code(unknown_quality);
   float64 RHmin = 0.0;                                                          //081104
   if (vapor_pressure_deficit_max && air_temperature_max)                        //081028
   {
      float64 max_temp = air_temperature_max->get_value_update_if_necessary();
      float64 VPD_max       = vapor_pressure_deficit_max->get_value_update_if_necessary();
      const CORN::Quality &contrib_quality = air_temperature_max->worst(*vapor_pressure_deficit_max);    //081104
      if (contrib_quality.is_valid())
      {  ested_RHmin_quality.assume_code(quality_estimated_from_quality(contrib_quality/*.get_code()*/));
         Vapor_pressure_base vapor_pressure;
         float64 VPsat_at_Tmax = vapor_pressure.calc_saturation_kPa  (max_temp,max_temp, OVER_WATER_ONLY);
         float64 VP_act        = VPsat_at_Tmax - VPD_max;   // VP at noon
         RHmin = VP_act / VPsat_at_Tmax * 100.0;

//if (dump_it)
//{
// dump << "\tVPnoon=\t" << VP_act;
// dump << "\tRHmin=\t" << RHmin;
//}

//TRACE                trace << std::string(" ",trace_indent);
//TRACE                trace << "RHmin = VPact / VPsat_at_Tmax * 100 [limit range 10 to 100]";
//TRACE                trace << '\t' << RHmin << " = " << VP_act << " / " << VPsat_at_Tmax << " * 100.0";;
//TRACE                trace << '\t' << "RHmin<" << (int)ested_RHmin_quality.get_int32() << "> est from VPact qual <" << (int)contrib_quality.get_int32() << ">" << endl;
//TRACE                trace << endl;

//TRACE                trace << std::string(" ",trace_indent);
//TRACE                trace << "VPact(noon) = VPsat_at_Tmax - VPDmax";
//TRACE                trace << '\t' << VP_act << " = " << VPsat_at_Tmax << " - " << VPD_max;
//TRACE                trace << '\t' << "VPact qual is worst of Tmax <" << (int)Tmax_quality.get_int32() << "> , VPDmax <" << (int)VPDmax_qual.get_int32() << ">" << endl;
//TRACE                trace << endl;
      } else
      {  ested_RHmin_quality.assume_code(not_valid_quality);
//TRACE                trace << std::string(" ",trace_indent) << "RHmin not valid" << endl;
      }
   } else
   {  ested_RHmin_quality.assume_code(missing_quality);
//TRACE             trace << std::string(" ",trace_indent) << "RHmin missing" << endl;
   }
//TRACE trace_indent -= 1;
   return RHmin;
}
//_est_from_VPD_max_________________________________________________2008-10-16_/
Relative_humidity_minimum_calculator::Relative_humidity_minimum_calculator
(Air_temperature_maximum         *air_temperature_max_     // May be 0 if unknown
,Dew_point_temperature_maximum   *dew_point_temperature_max_                     //081028
,Vapor_pressure_deficit_max      *vapor_pressure_deficit_max_
,Dew_point_temperature_average   *dew_point_temperature_avg_)  // May be 0 if unknown
: Relative_humidity_extreme_calculator                                           //180810
   (dew_point_temperature_avg_
   ,vapor_pressure_deficit_max_
   ,air_temperature_max_)
   , dew_point_temperature_max(dew_point_temperature_max_)                       //081028
{}
//_Relative_humidity_minimum_calculator:constructor____________________________/
Relative_humidity_minimum_calculator::Relative_humidity_minimum_calculator
(float64 value_, const CORN::Quality &quality_)
: Relative_humidity_extreme_calculator(value_,quality_)                          //180810
{}
//_Relative_humidity_minimum_calculator:constructor____________________________/
Relative_humidity_minimum_calculator::Relative_humidity_minimum_calculator
(Air_temperature_maximum       *air_temperature_max_  // required
,const Humidity_specific       *humidity_specific_
,const Atmospheric_pressure    *atmospheric_pressure_)
: Relative_humidity_extreme_calculator(humidity_specific_,atmospheric_pressure_)
//, air_temperature_max(air_temperature_max_)
, dew_point_temperature_max(0)
{
   air_temperature_max =air_temperature_max_;
}
//_Relative_humidity_minimum_calculator constructor__________________2018-08-11_
const CORN::Quality & Relative_humidity_minimum_calculator::update()   rectification_
{  if (is_empty())
      // is_empty is a litle hack until I fix setting invalid quality in the UED weather editor //091204
      assume_code(missing_quality);                                      //091204
   if (is_valid())
   {
   }
   else {                                                                        //081027
      if (updating)                                                              //081027
      {  assume_code(CORN::unknown_quality);                                     //150123_091212
      } else
      {  updating = true;                                                        //081027
         CORN::Quality_clad RHmin_quality;                                            //081027

         float64 RHmin = calc_from_VPsat_at_max_dew_point(RHmin_quality);
            // this may be est from
         Relative_humidity_calculator::set_value_quality(RHmin,RHmin_quality);   //081028
         if (RHmin_quality.is_optimal())                                         //081104
         {   // we have the best quality no need to check other methods.         //081104
         } else                                                                  //081104
         {  // we may be able to get better data
               if (vapor_pressure_deficit_max)                                   //081016
               {  RHmin = est_from_VPD_max(RHmin_quality);                       //081016
                  Relative_humidity_calculator::set_value_quality(RHmin,RHmin_quality);
               }
               else
               {  RHmin =calc_from_humidity_specific(RHmin_quality);             //180811
                  Relative_humidity_calculator::set_value_quality(RHmin,RHmin_quality);//180811
               }
               //#ifdef OBSOLETE
               if (dew_point_temperature_avg)                                    //081016
               { // We first try to calc from avg dew pt temperature // If still not valid then try to estimate from avg dew point temperature
                  RHmin = est_from_avg_dew_point_temperature(RHmin_quality);
                  // This is special for the agrimet data sets
                  // we presume the calculated relative humidity is real
                  // otherwise the recalculation throws things off.
                  Relative_humidity_calculator::set_value_quality(RHmin,RHmin_quality); // forcing
               }
               //#endif
         } // if calculated quality
         if  (generation_parameters && current_season_known)
            constrain_to_range
               (generation_parameters->typical[0][*current_season_known][0]  // typical low
               ,generation_parameters->typical[0][*current_season_known][1]); // typical high //091217
         else
            constrain_to_range(10.0,100.0);
         updating = false;   // Now that we have relative humidity value, it is available to be used for other weather element calculations/estimations  //081027
      }
   }
   return get_quality();
}
//_Relative_humidity_minimum_calculator::update_____________________2008-09-08_/
void Relative_humidity_calculator::know_generation_parameters
(const Relative_humidity_extremes &_generation_parameters
,const Season_S_W_SF &_current_season_known)                       contribution_
{  generation_parameters = &_generation_parameters;
   current_season_known  = &_current_season_known;
}
//_know_generation_parameters_______________________________________2009-12-16_/
float64  Relative_humidity_minimum_calculator
::est_from_avg_dew_point_temperature(CORN::Quality &RHmin_quality)         const
{  // Determine what the calc'ed max rel humidity would be
  RHmin_quality.assume_code(unknown_quality);                                    //081104
  float64 RHmin = 0.0;                                                           //980305
  if (dew_point_temperature_avg && air_temperature_max)                          //081104
  {                                                                              //081104
      float64 max_temp        = air_temperature_max      ->get_value_update_if_necessary();//980306
      float64 avg_dewpt_temp  = dew_point_temperature_avg->get_value_update_if_necessary();//980306
      Quality_clad adj_DPTavg_quality(                                           //081104
         (dew_point_temperature_avg->is_measured())                              //081106
         ? other_real_quality
            // we don't want avg_dewpt temperature (although it may be real)
            // to be treated as measured quality because we are using it
            // here only as an approximation
         : dew_point_temperature_avg->get_quality_code());                       //150121
      const CORN::Quality &contrib_quality = adj_DPTavg_quality.worst(*air_temperature_max); //081104
      if (contrib_quality.is_valid())                                            //081104
      {  // The quality needs to be calculated from because we need to be able to  //081104
         // have relative humidity min the same quality (or calculated from) avg dewpt
         RHmin_quality.assume_code(quality_estimated_from_quality(contrib_quality/*.get_code()*/));  //150121_081104
         // RLN modified for relative humidity in subzero temperature (ref: RH&WMO)                   //040114
         Vapor_pressure_base vapor_pressure;                                     //050602
         float64 act_VP_avg_dewpt_temp =  vapor_pressure.calc_saturation_kPa     //040114
            (avg_dewpt_temp,max_temp,OVER_WATER_ONLY);
         float64 VPsat_max_temp =  vapor_pressure.calc_saturation_kPa            //040114
            (max_temp,max_temp,OVER_WATER_ONLY);
         RHmin =  act_VP_avg_dewpt_temp / VPsat_max_temp * 100.0;                //980305
      } else  RHmin_quality.assume_code(not_valid_quality);                      //081104
   } else  RHmin_quality.assume_code(missing_quality);                           //081104
  return RHmin;
}
//_Relative_humidity_minimum_calculator:est_from_avg_dew_point_temperature_____/
float64 Relative_humidity_maximum_calculator::est_from_avg_dew_point_temperature(CORN::Quality &RHmax_quality) const
{  // Determine what the calc'ed max rel humidity would be
   RHmax_quality.assume_code(unknown_quality);
   float64 RHmax =  0.0;                                                         //980305
   if (air_temperature_min && dew_point_temperature_avg)                         //081028
   {  float64 min_temp = air_temperature_min->get_value_update_if_necessary();   //150126_980306
      float64 avg_dewpt_temp = dew_point_temperature_avg->get_value_update_if_necessary();//150126_980306
      Quality_clad adj_DPTavg_quality(                                           //081104
         (dew_point_temperature_avg->is_measured())                              //081106
         ? other_real_quality
            // we don't want avg_dewpt temperature (although it may be real)
            // to be treated as measured quality because we are using
            // it here only as an approximation
         : dew_point_temperature_avg->get_quality_code());                        //150126
      const CORN::Quality &contrib_quality
         = adj_DPTavg_quality.worst(*air_temperature_min);                       //081028
      if (contrib_quality.is_valid())                                            //991014
      {  RHmax_quality.assume_code
            (quality_estimated_from_quality(contrib_quality));                   //001107
         // RLN modified for relative humidity in subzero temperature (ref: RH&WMO) //040114
         Vapor_pressure_base vapor_pressure;                                     //050602
         float64 act_VP_avg_dewpt_temp = vapor_pressure.calc_saturation_kPa      //040114
            (avg_dewpt_temp,min_temp,OVER_WATER_ONLY);
            // (actual vapor pressure based on average dew point)
         float64 VPsat_at_Tmin         = vapor_pressure.calc_saturation_kPa      //040114
            (min_temp,min_temp,OVER_WATER_ONLY);
         RHmax         = act_VP_avg_dewpt_temp / VPsat_at_Tmin * 100.0;          //980305
      } else  RHmax_quality.assume_code(not_valid_quality);
   } else RHmax_quality.assume_code(missing_quality);
   return  RHmax;                                                                //970723
}
//_est_from_avg_dew_point_temperature_______________________________2005-06-05_/
