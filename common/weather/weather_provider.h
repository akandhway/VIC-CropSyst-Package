#ifndef weather_providerH
#define weather_providerH
//______________________________________________________________________________
class Weather_parameter;
#include "UED/library/UED_types.h"
#include "common/weather/loc_param.h"
#include "corn/const.h"
namespace CS {
   class Solar_radiation_extraterrestrial;                                       //151022
}
class Vapor_pressure_saturation_slope;
#include "weather/parameter/WP_precipitation.h"
#include "weather/parameter/WC_relative_humidity.h"
#include "weather/parameter/WC_air_temperature.h"
#include "weather/parameter/WC_dew_point_temperature.h"
#include "weather/parameter/WC_vapor_pressure.h"
#include "weather/parameter/WC_vapor_pressure_act.h"
#include "weather/parameter/WC_vapor_pressure_deficit.h"
#include "weather/parameter/WC_solar_radiation.h"
#include "weather/parameter/WP_atmospheric_pressure.h"
#include "weather/parameter/WP_wind_speed.h"
#include "weather/parameter/WC_vapor_pressure_saturation_slope.h"
#include "weather/parameter/WP_latent_heat_vaporization.h"
#include "weather/parameter/WC_psychrometric_const.h"
#include "weather/parameter/WP_ET_reference.h"
#include "weather/parameter/WC_humidity_specific.h"

#include "biometeorology/aerodynamics.h"
//______________________________________________________________________________
class Weather_provider   // rename to Weather_provider_daily
{
 public:
   const CORN::Date_const    &date; //probably should be protected
   const Geocoordinate       &geocoordinate;                                     //150629
 public:
   // I may just want to have a pointer to location
   // if optional locations is specified in the constructor
   // then use it, otherwise instanciate it and use the defaultsfo
   const Location_parameters  *location; bool location_owned;
         Vapor_pressure_deficit_max_calculator::Parameters &VPDmax_parameters;   //170803
   const Solar_radiation_parameters             &solar_radiation_generation_parameters;
   const CS::Solar_radiation_extraterrestrial   &ET_solar_radiation;             //151022_150625
 protected:
   contribute_ Season_S_W_SF        season_S_W_SF; // for dew point
   contribute_ CS::Parameter_cowl_T<float64>  *element[256];  // Currently I am using fixed size arrays, but they should be dynamic
 public:
   Precipitation                             precipitation_actual;
   // Need to add effective precipitation
   Air_temperature_maximum                   air_temperature_max;
   Air_temperature_minimum                   air_temperature_min;
   Air_temperature_average_calculator        air_temperature_avg;
   Solar_radiation_with_Donatelli_Campbell_estimation_parameter solar_radiation;

      // Rename to solar_radiation_global
      // (I think this is also down short wave solar radiation



   Dew_point_temperature_maximum_calculator dew_point_temperature_max;
   Dew_point_temperature_minimum_calculator dew_point_temperature_min;
   Dew_point_temperature_average_calculator dew_point_temperature_avg;
   Relative_humidity_maximum_calculator     relative_humidity_max;
   Relative_humidity_minimum_calculator     relative_humidity_min;
   Relative_humidity_calculator             relative_humidity_avg;               //130218
   Wind_speed                               wind_speed;
   Atmospheric_pressure                     atmospheric_pressure;
   Vapor_pressure_deficit_fullday_calculator vapor_pressure_deficit_fullday_avg;
   Vapor_pressure_deficit_daytime_calculator vapor_pressure_deficit_daytime;
   Vapor_pressure_deficit_max_calculator     vapor_pressure_deficit_max;
   Vapor_pressure_daytime_calculator         vapor_pressure_daytime; // AKA vapor_pressure_max
   Vapor_pressure_nighttime_calculator       vapor_pressure_nighttime;
   Vapor_pressure_actual_calculator          vapor_pressure_actual;

   Latent_heat_vaporization_calculator          latent_heat_vaporization;        //140512
   Vapor_pressure_saturation_slope_calculator   vapor_pressure_saturation_slope; //140512
   Psychrometric_constant_calculator            psychrometric_constant;          //140512
   float64                                   aerodynamic_resistance_ref_plants;
   Aerodynamics                              aerodynamics;
   float64 atmospheric_CO2_conc_ppm;
   ET_Reference                              ET_reference;                       //180806
   Humidity_specific_calculator              humidity_specific;                  //180808
 public:
   Weather_provider
      (const CORN::Date_const           &date
      ,const Geocoordinate              &geocoordinate                           //150629
      ,const CS::Solar_radiation_extraterrestrial                                //151022
            &ET_solar_radiation_                                                 //150625
      // actually daily or hourly (haven't defined interface yet
      ,const Location_parameters        *location = 0);
/*NYN
   bool know_given_element(UED::Variable_code variable,CORN::Units_code time_step);
   ** This tells the weather provided that it is expected that
       a weather element will be provided in the data source.
       If certain weather elements are not available in the data
       source the provider with not instanciate
       calculating/estimating/generating Weather_parameters (elements)
       that require elements that are not provided by the data source.
       This function should be called by
   **
*/
   virtual ~Weather_provider();
   virtual void invalidate(bool absolutely = true);
   virtual bool update()                                           computation_;
protected:
public:
   const Vapor_pressure_saturation_slope &ref_vapor_pressure_saturation_slope() const;  //140507
};
//_Weather_provider____________________________________________________________/
#endif

