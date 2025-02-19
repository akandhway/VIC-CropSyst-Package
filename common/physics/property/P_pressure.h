#ifndef P_pressureH
#define P_pressureH
#include "physics/property.h"
#include "corn/measure/measures.h"
namespace Physical
{
//_________________________________________________________________________________________
class Pressure_interface
: public extends_interface_ Property_interface
{
 public:

   inline float64 get_kPa()                                                const
      { return in_prescribed_units()/*amount*/; }                                //101209
   inline float64 set_kPa(float64 _value)                          modification_
      {  return update_value_with_units(_value,UC_kPa);}
   inline float64 update_kPa(float64 _value)                      rectification_ //150123
      {  return update_value_with_units(_value,UC_kPa);}

};
//_Pressure_interface_______________________________________________2015-01-21_/
}//_namespace_Physical_________________________________________________________/

#endif

