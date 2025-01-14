#ifndef WP_vapor_pressure_saturation_slopeH
#define WP_vapor_pressure_saturation_slopeH
#include "common/CS_parameter_float64.h"
// May want to move this units definition to units_code_SI.h
//______________________________________________________________________________
class Vapor_pressure_saturation_slope
: public extends_ CS::Parameter_float64       // I think intensive value in default units kPa/C
{
 public:
   inline Vapor_pressure_saturation_slope()
      : CS::Parameter_float64(INTENSIVE,0.0,UC_kPa_per_C,CORN::unknown_quality)
      {}
   inline float64 get_kPa_per_C()                                          const { return in_prescribed_units(); }
   inline float64 set_kPa_per_C(float64 value_kPa_C, const CORN::Quality &_quality)
      {  assume(_quality);                                                       //150123_140528
         return update_value_with_units(value_kPa_C,UC_kPa_per_C);               //150123
      }
   inline float64 set_kPa_per_C(float64 value_kPa_C, CORN::Quality_code _quality)//150318LML
      {  assume_code(_quality);
         return update_value_with_units(value_kPa_C,UC_kPa_per_C);
      }
   inline float64 update_kPa_per_C(float64 value_kPa_C, const CORN::Quality &_quality) rectification_ //140602
      {  assume(_quality);                                                       //150121
         return update_value_with_units(value_kPa_C,UC_kPa_per_C);               //150123
      }
};
//_Vapor_pressure_saturation_slope_________________________________2014-05-07_/
#endif
