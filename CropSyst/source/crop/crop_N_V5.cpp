#include "crop_N_V5.h"
namespace CropSyst
{
//______________________________________________________________________________
Crop_nitrogen_V5::Crop_nitrogen_V5
(Crop_parameters::Nitrogen &parameters_
,Crop_complete             &crop_
,Soil::Soil_interface      *soil_                                                //181206
,Soil::Nitrogen            *chemicals_                                           //181206
,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance
   *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference_
   // although _slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
   // is optional, if it is not specified, there will be no correction for transpiration
,const float64 &transpiration_use_efficiency_CO2_adjusted_                       //151104
   // adjusted when CO2 is enabled otherwise unadjusted
#if ((PHENOLOGY_VERSION==2018))
,const float64 &accum_deg_days_adjusted_for_clipping_                            //181118
#endif

)
: Crop_nitrogen_common
   (parameters_
   ,crop_
   ,soil_
   ,chemicals_
   ,slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference_
   #if ((PHENOLOGY_VERSION==2018))
   ,accum_deg_days_adjusted_for_clipping_                                        //181118
   #endif
   )
, transpiration_use_efficiency_CO2_adjusted(transpiration_use_efficiency_CO2_adjusted_)
{}
//_Crop_nitrogen_V5:constructor________________________________________________/
float64 Crop_nitrogen_V5::update_limited_pot_transpiration()       modification_
{
   // N stress reduce transpiration-use efficiency in addition to biomass gain.  //150916COS
   float64 TUE_reduction_factor_due_to_N_stress
      = (stress_factor < 1.0)
      ? CORN::must_be_0_or_greater<float64>
         (-0.201 * stress_factor * stress_factor + 0.3258 * stress_factor + 0.8604)
         //151117 (-0.8783 * stress_factor * stress_factor + 1.8871 * stress_factor - 0.0088)
         //160119 (pow(((1.0 + stress_factor) / 2.0),(2.0 - parameters.TUE_stress_sensitivity)))     //151117
      : 1.0;
   /*
   It is not necessary to check the upper limit (1) of the calculated value
   because it will not be greater that if the stress_factor is valid (< 1)
   which has already been checked.
   */
   float64 transpiration_use_efficiency_reduced // N
      = transpiration_use_efficiency_CO2_adjusted
      * TUE_reduction_factor_due_to_N_stress;
   float64 N_limited_pot_transpiration_mm
      =  N_limited_growth  / transpiration_use_efficiency_reduced;
   N_limited_pot_transpiration = mm_to_m(N_limited_pot_transpiration_mm);
   return N_limited_pot_transpiration;
}
//_update_limited_pot_transpiration_________________________________2015-10-30_/
float64 Crop_nitrogen_V5::update_limited_growth
(float64 attainable_growth
,float64 root_activity_factor)                                     modification_
{
   N_limited_growth = Crop_nitrogen_common::update_limited_growth
      (attainable_growth,root_activity_factor);
   stress_factor =
      (attainable_growth > 0.000001) // avoid div zero
      ? N_limited_growth / attainable_growth
      : 1.0 ;
   stress_factor_stats.append(stress_factor);
   return N_limited_growth;
}
//_update_limited_growth____________________________________________2015-10-31_/
}//_namespace_CropSyst_________________________________________________________/

