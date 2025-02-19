#include "common/weather/atmospheric_CO2_change_linear.h"
//______________________________________________________________________________
Atmospheric_CO2_change_linear::Atmospheric_CO2_change_linear
(float32 initial_CO2_conc_
,float32 annual_CO2_conc_change_
,const CORN::date32 &simdate_raw_)                                               //171207

: Atmospheric_CO2_change_element(simdate_raw_)                                   //171207
, curr_CO2_conc()                                                                //150507
, annual_CO2_conc_change(annual_CO2_conc_change_)
{  curr_CO2_conc.set_ppm(initial_CO2_conc_,CORN::measured_quality);
   // presuming measured quality 150507
}
//_Atmospheric_CO2_change_linear:constructor___________________________________/
bool Atmospheric_CO2_change_linear::end_year()                     modification_
{
   // Warning NYI
   curr_CO2_conc.set_ppm                                                         //150507
      (curr_CO2_conc.ppm() + annual_CO2_conc_change
      ,CORN::estimated_quality);
   return true;
}
//_end_year____________________________________________________________________/
#pragma package(smart_init)
