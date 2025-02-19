#include "corn/parameters/parameter_interpolation.h"
namespace CORN
{
//______________________________________________________________________________
Parameter_interpolation_dated::Parameter_interpolation_dated
(Interpolation_curve_type     _curve_type
,const         CORN::Date    &_today
,modifiable_   Number_keyed  &_parameter
)
: CORN::Item                                                                  ()
, today                                                                 (_today)
, parameter                                                         (_parameter)
, interpolator           (_curve_type,_parameter.get_value_as_float32(),&_today)
{}
//______________________________________________________________________________
bool Parameter_interpolation_dated::update()
{  parameter.set_value_as_float32
      ((float32)interpolator.get_ordinate_by_date(today));
   return true;
}
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

