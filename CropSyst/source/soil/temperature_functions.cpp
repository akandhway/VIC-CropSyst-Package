#include "temperature_functions.h"
//---------------------------------------------------------------------------
#include "corn/math/compare.hpp"
//______________________________________________________________________________
float64
calc_local_heat_capacity_J_m3_C   //  J/(m3 C)
(float64 bulk_density_sl
,float64 water_content_sl
,float64 ice_content_sl)
{
   return
          2400000.0 * bulk_density_sl / 2.65             //mineral
        + 4184000.0 * (water_content_sl - ice_content_sl)//water
        + 1882800.0 * ice_content_sl;                    //Ice
}
//_calc_local_heat_capacity_J_m3_C_____________________________________________/
//          Latent heat of fusion in J/kg
float64
DeVries_thermal_conductivity  // J / (sec m C) DeVries
(float64 water_content_sl
,float64 ice_content_sl
,float64 bulk_density_sl ) //080123 const
{
#define        mineral_thermal_conductivity  7.3  /* J/(sec m C) Kelvin? */
#define        water_thermal_conductivity    0.57 /* J/(sec m C) */
#define        ice_thermal_conductivity      2.2  /* J/(sec m C) */
#define        air_thermal_conductivity      0.02 /* J/(sec m C) */
   float64 mineral_fraction = bulk_density_sl / 2.65;
   float64 water_fraction   = water_content_sl;
   float64 ice_fraction     = ice_content_sl;
   float64 air_fraction     = CORN::must_be_0_or_greater<float64>
     ((1.0 - (mineral_fraction + water_fraction + ice_fraction)));
   float64 denominator = 0.2 * mineral_fraction + 2.5 * air_fraction
      + 1.0 * water_fraction + 0.5 * ice_fraction;
   float64 numerator =
        0.2 * mineral_fraction * mineral_thermal_conductivity
      + 2.5 * air_fraction     * air_thermal_conductivity
      + 1.0 * water_fraction   * water_thermal_conductivity
      + 0.5 * ice_fraction     * air_thermal_conductivity;
   return numerator / denominator;
}
//_DeVries_thermal_conductivity_____________________________________1999-02-15_/
