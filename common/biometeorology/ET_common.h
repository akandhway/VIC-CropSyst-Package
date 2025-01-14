#ifndef ET_commonH
#define ET_commonH
#include "corn/primitive.h"
#     ifndef measuresH
#        include "corn/measure/measures.h"
#     endif
#include "common/CS_parameter_float64.h"
#include "common/weather/parameter/WC_ET_reference.h"
#include "biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#include <fstream>
//______________________________________________________________________________
namespace CS {
class Solar_radiation_extraterrestrial;                                          //151022
}
class Atmospheric_pressure;
class Vapor_pressure_actual;
class Air_temperature_maximum;
class Air_temperature_minimum;
class Solar_radiation;
class Vapor_pressure_saturation_slope;
class Latent_heat_vaporization;
class Psychrometric_constant;

#define canopy_resistance_constant_d_m 0.00081
// since 980220 we used a canopy resistance of 0.000787037
// Claudio now wants to use 0.00081                                              //011108
// RLN actually it is 70/86400 = 8.1018518518518518518518518518519e-4
// see http://www.sciencedirect.com/science/article/pii/S0378377402001749
// Fixed versus variable bulk canopy resistance for reference evapotranspiration
// estimation using the Penman�Monteith equation under semiarid conditions.
//______________________________________________________________________________
class Evapotranspiration_parameter
: public extends_ CS::Parameter_float64     // Probably can be replaced by Property length
{public:
   inline Evapotranspiration_parameter()
      : CS::Parameter_float64(EXTENSIVE,0.0,UC_mm,CORN::unknown_quality)
      {}
   inline Evapotranspiration_parameter
      (float64             default_value   // Probably should be NAN
      ,CORN::Units_code    default_units   = U_units_not_specified
      ,CORN::Quality_code  default_quality= CORN::unknown_quality)
      : CS::Parameter_float64(EXTENSIVE,default_value,default_units,default_quality)
      { assume_code(default_quality); }                                          //141101_140526
   inline float64 get_mm()               const { return in_prescribed_units(); }
   inline float64 get_m()                const { return mm_to_m(get_mm()); }
   inline float64 set_mm(float64 _ET_mm)       { return set_amount(_ET_mm); }    //111013_110911
};
//_Evapotranspiration_parameter________________________________________________/
class ET_Reference_common //190812 was ET_common                                 //190812_000727
: public extends_ ET_Reference                                                   //180404
{ // This class provides methods common to all ET models

   // This (and derived classes) is an ET_reference calculator,
   // it is an alternative to WC_ET_reference_calculator

 protected:
   const CS::Solar_radiation_extraterrestrial &solar_radiation_extraterrestrial; //150625
   const float64 &day_length_hours_today;                                        //1506??
   float32 elevation; // meters  (may be 0 for no elevation adjustment)          //150604
   const Atmospheric_pressure    &atmospheric_pressure;                          //100421
   const Vapor_pressure_actual   &vapor_pressure_actual;
   const Air_temperature_maximum &air_temperature_max;
   const Air_temperature_minimum &air_temperature_min;
   const Solar_radiation         &solar_radiation;

   mutable float64 isothermal_long_wave_net_radiation;                           //070204
   mutable float64 net_radiation;                                                //070204
 protected:
   const Aerodynamics                     &aerodynamics;                         //150617
   const Vapor_pressure_saturation_slope  &vapor_pressure_saturation_slope;      //140512
   const Latent_heat_vaporization         &latent_heat_vaporization;             //140512
   const Psychrometric_constant           &psychrometric_constant;               //140512
   mutable float64 curr_aerodynamic_resistance_d_m;                              //160731_150601_011119
 public:
   Slope_saturated_vapor_pressure_plus_psychrometric_resistance                  //150617
      slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference;    //150617
   float64 Stefan_Boltzmann_law()                                         const;
   static const float64 inverse_latent_heat_flux_vaporization; // = 0.408        //131105
 public:
   float64 calc_net_radiation(bool day_light_hours)                       const;   // need to check if can be const
   virtual float64 calc_isothermal_long_wave_net_radiation()              const;
      ///< elevation can be 0 if no elevation adjustment.
 public:
   ET_Reference_common //190812 ET_common
      (float32             elevation                                             //150603
      ,const float64      &day_length_hours_today                                //150627
         // actually this could be hourly or daily (I don't have interface yet)
      ,const Aerodynamics &aerodynamics                                          //150617
      ,const CS::Solar_radiation_extraterrestrial &ET_solar_radiation_new        //150625
      ,const Air_temperature_maximum  &air_Tmax
      ,const Air_temperature_minimum  &air_Tmin
      ,const Atmospheric_pressure     &atmospheric_pressure_parameter            //100421
      ,const Vapor_pressure_actual    &vapor_pressure_actual                     //100421
      ,const Solar_radiation          &solar_radiation_parameter
      ,const Vapor_pressure_saturation_slope  &vapor_pressure_saturation_slope   //140512
      ,const Latent_heat_vaporization         &latent_heat_vaporization          //140512
      ,const Psychrometric_constant           &psychrometric_constant);          //140512
   float64 calc_atmospheric_density(float64 temperature_kelvin)           const;
 public:
   float64 get_isothermal_long_wave_net_radiation()                        const
      { return isothermal_long_wave_net_radiation;}
   float64 get_net_radiation()                    const { return net_radiation;}
   inline const Slope_saturated_vapor_pressure_plus_psychrometric_resistance     //160731
      &ref_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference() const
      { return slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference; }
   inline virtual const float64  &ref_aerodynamic_resistance_ref_plants() const  //180805_160731
      { return curr_aerodynamic_resistance_d_m; }                                //150601_140508
 private:
   float64 get_cloud_factor_for_net_rad(float64 clear_sky_solar_rad)      const;
 public:
   virtual void dump_header(std::ofstream &strm);
   virtual void dump_detail(std::ofstream &strm);
};
//_ET_common___________________________________________________________________/
#endif
// ET.h

