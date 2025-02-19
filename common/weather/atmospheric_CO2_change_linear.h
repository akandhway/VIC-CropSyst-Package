#ifndef atmospheric_CO2_change_linearH
#define atmospheric_CO2_change_linearH
#include "common/weather/atmospheric_CO2_change.h"
#include "corn/primitive.h"
//______________________________________________________________________________
class Atmospheric_CO2_change_linear
: public Atmospheric_CO2_change_element
{
private:
   float32 annual_CO2_conc_change ;  // ppm
   CS::CO2_atmospheric_concentration  curr_CO2_conc;                             //150507
public:
   Atmospheric_CO2_change_linear
      (float32 _initial_CO2_conc
      ,float32 _annual_CO2_conc_change
      ,const CORN::date32 &simdate_raw_);                                          //171207
   virtual bool end_year()                                        modification_;
   inline virtual float64 get_current_CO2_conc()                           const
      { return curr_CO2_conc.ppm(); }
   inline virtual const CS::CO2_atmospheric_concentration
      &ref_CO2_current_conc()                                              const //150507
      { return curr_CO2_conc; }
};
//_Atmospheric_CO2_change_linear_______________________________________________/
#endif
