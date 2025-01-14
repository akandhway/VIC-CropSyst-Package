#ifndef WP_latent_heat_vaporizationH
#define WP_latent_heat_vaporizationH
#include "weather/parameter/WP_air_temperature.h"
class Air_temperature_average;
//______________________________________________________________________________
class Latent_heat_vaporization         // I think this is Actually   Latent heat of or at vaporization (not Vaporization at latent heat)
: public extends_ CS::Parameter_float64    // so it would then be derived from Physical::Latent_heat
{
 public:
   inline Latent_heat_vaporization()
      : CS::Parameter_float64(INTENSIVE,0.0,UC_MJ_per_kg,CORN::unknown_quality)
      {}
   inline float64 get_MJ_per_kg()                                          const
      { return in_prescribed_units(); }                                              //150122
   inline float64 set_MJ_per_kg
      (float64 value_MJ_kg, const CORN::Quality &_quality)       modification_ //140528
      {  assume(_quality);                                                       //150120
         return set_amount(value_MJ_kg);
      }
   inline float64 update_MJ_per_kg
      (float64 value_MJ_kg, const CORN::Quality &_quality)      rectification_
      {  assume(_quality);                                                       //150120_140120
         return update_value_with_units(value_MJ_kg,UC_MJ_per_kg);}
};
//_Latent_heat_vaporization________________________________________2014-05-07_/
class Latent_heat_vaporization_calculator
: public extends_ Latent_heat_vaporization
{
   const Air_temperature_average &air_temperature_avg;
 public:
   inline Latent_heat_vaporization_calculator
      (const Air_temperature_average &_air_temperature_avg)
      :Latent_heat_vaporization()
      ,air_temperature_avg(_air_temperature_avg)
      {}
   inline virtual const CORN::Quality &update()                   rectification_
      {  CORN::Quality_clad LHV_quality(quality_calculated_from
            (air_temperature_avg.get_quality_code()));
         float64 LHV = calc_from_avg_air_temperature();
         update_MJ_per_kg(LHV,LHV_quality);
         updating = false;                                                       //150127
         return get_quality();                                                   //150123
      }
 protected:
   inline float64 calc_from_avg_air_temperature()                         const
      { return 2.501 - air_temperature_avg.Celcius() * 0.002361;   }
};
//_Latent_heat_vaporization_calculator_____________________________2014-05-07_/
#endif
