#ifndef ET_reference_FAOH
#define ET_reference_FAOH

//100330 This code is very specific to CropSyst
// A more general form of the Penman-Monteith FAO implementation is in ET_reference.cpp

#ifndef ET_reference_interfaceH
#include "biometeorology/ET_reference_interface.h"
#endif
#ifndef ET_commonH
#  include "biometeorology/ET_common.h"
#endif

#ifndef UED_CODES_H
#  include "UED/library/codes.h"
#endif
#ifndef measuresH
#  include "corn/measure/measures.h"
#endif
#ifndef ET_modelH
#  include "biometeorology/ET_model.h"
#endif
#ifndef WC_vapor_pressure_saturation_slopeH
#  include "weather/parameter/WC_vapor_pressure_saturation_slope.h"
#endif
#ifndef WC_psychrometric_constH
#  include "weather/parameter/WC_psychrometric_const.h"
#endif
#ifndef WP_latent_heat_vaporizationH
#  include "weather/parameter/WP_latent_heat_vaporization.h"
#endif
#include "biometeorology/aerodynamics.h"
#include "biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#include "common/geodesy/geocoordinate.h"

class Weather_provider;
class Weather;
//______________________________________________________________________________
class ET_Reference_FAO         // 100330 need to rename to ET_Reference_FAO  was  Reference_ET_FAO
: public ET_Reference_common
{protected:
   const Weather_provider &weather_provider;                                     //150804
//NYN const  Aerodynamics &aerodynamics;
 public:
#ifdef LOC_ONLY_FOR_GEN
 protected: // parameters for Hargreaves sp?
   float64 ET_slope;       // default should be 1                                //070314
   float64 ET_intercept;   // default should be 0                                //070314
#endif
 protected:
   ET_Model       enabled_model;
   ET_Reference   ET_ref[ET_Model_COUNT];                                        //180404
    float64 saturation_vapor_pressure_deficit; // kPa
 protected: // Current values used in more than one of the ET models.
    mutable float64 curr_avg_air_temperature;         // degrees C
    mutable float64 curr_soil_heat_flux_density;      // MJ/(m2 day)
    mutable float64 curr_net_radiation;               // MJ/(m2 day) at crop surface

 public:
   ET_Reference_FAO
      (const Weather_provider &_weather_provider                                 //150804
      ,const Geocoordinate &geocoordinate_                                       //151118
      ,const float64                               &_day_length_hours_today      //150627
#ifdef LOC_ONLY_FOR_GEN
      ,float64 _ET_slope                                                         //070314
      ,float64 _ET_intercept                                                     //070314
#endif
      ,ET_Model preferred_ET_model
      );   // Not currently used
   virtual const CORN::Quality &update()                         rectification_; //180822
   inline virtual float64 get_preferred_m (ET_Model preferred_ET_model)   const  //140306
      {  return ET_ref[preferred_ET_model].get_m();                              //180404
      }
   inline virtual float64 get_preferred_mm(ET_Model preferred_ET_model)   const  //140306
      {  return ET_ref[preferred_ET_model].get_mm();                             //180404
      }
   inline virtual CORN::Quality_code get_preferred_quality_code                  //180404
      (ET_Model  preferred_ET_model)                                       const
      { return ET_ref[preferred_ET_model].get_quality_code(); }
   inline virtual float64 get_saturation_vapor_pressure_deficit()
      { return saturation_vapor_pressure_deficit; }
   inline virtual const Vapor_pressure_saturation_slope &ref_vapor_pressure_saturation_slope()  { return vapor_pressure_saturation_slope; }  //140507
   inline virtual const Psychrometric_constant          &ref_psychrometric_constant()           { return psychrometric_constant; }           //140508
};
//_ET_Reference_FAO____________________________________________________________/
#endif

