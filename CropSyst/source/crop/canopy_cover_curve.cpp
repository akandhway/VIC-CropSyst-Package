
#include "canopy_cover_curve.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#ifndef moremathH
#  include "corn/math/moremath.h"
   // using namespace std;
#endif

///* The is to calculate relative thermal time similar to April 2019 version
#include <iostream>
namespace CropSyst
{
//______________________________________________________________________________
bool Canopy_cover_curve_common::parameterize
(float64 cover_maximum_CO2_adj_
,float64 cover_initial_)                                         initialization_
{
   cover_maximum_CO2_adj = cover_maximum_CO2_adj_;
   cover_initial = cover_initial_;
   return true;
}
//______________________________________________________________________________
float64 Canopy_cover_curve_common::calculate()                      calculation_
{
   return accrescence   ? calc_during_accrescence()
        : culminescence ? calc_during_culminescence()
        : senescence    ? calc_during_senescence()
        : 0.0;
}
//_calculate________________________________________________________2013-06-11_/
Canopy_cover_curve_common::Canopy_cover_curve_common
(const float32 &canopy_cover_season_end_)
:Canopy_cover_curve()
,accrescence                           (0)
,culminescence                         (0)
,senescence                            (0)                                       //181108
,canopy_cover_season_end               (canopy_cover_season_end_)
,cover_maximum_CO2_adj                 (0)
,cover_initial                         (0)
{}
//_Canopy_cover_curve_common:constructor____________________________2017-12-20_/
bool Canopy_cover_curve_common::know_accrescence
(const Phenology::Period_thermal *accrescence_)                    modification_
{  accrescence    =accrescence_     ;
   culminescence  = 0;
   senescence     = 0;
   return true; //accrescence    != 0;
}
bool Canopy_cover_curve_common::know_culminescence
(const Phenology::Period_thermal *culminescence_)                  modification_
{  culminescence  = culminescence_   ;
   accrescence    = 0;
   senescence     = 0;
   return true; //culminescence  != 0;
}
bool Canopy_cover_curve_common::know_senescence
(const Phenology::Period_thermal *senescence_)                     modification_
{  senescence     =senescence_      ;
   accrescence    = 0;
   culminescence  = 0;
   return true; //senescence     != 0;
}
//_know_XXXscence___________________________________________________2019-10-21_/
Canopy_cover_curve_2017::Canopy_cover_curve_2017
   (const float32 & canopy_cover_season_end_
   ,float64 shape_coef_accrescent_
   ,float64 shape_coef_senescent_
   ,const float32 &thermal_time_fractional_at_half_cover_max_                    //181015
   ,const float32 &thermal_time_fractional_at_half_cover_senescence_)            //181015
: Canopy_cover_curve_common
   (canopy_cover_season_end_)
, cover_current         (0)
, B1                    (0)
, B2                    (0)
, shape_coef_vegetative (shape_coef_accrescent_)   // (9.0) may need to be in UI
, shape_coef_senescent  (shape_coef_senescent_)    // (9.0) may need to be in UI
, CCmax2_actual         (0)
, CCmax_asymptotic      (0)
, CCend_asymptotic      (0)
, thermal_time_fractional_at_half_cover_max
                                    (thermal_time_fractional_at_half_cover_max_) //181015
, thermal_time_fractional_at_half_cover_senescence
                             (thermal_time_fractional_at_half_cover_senescence_) //181015
{}
//_Canopy_cover_curve_2017:constructor______________________________2017-12-20_/
bool Canopy_cover_curve_2017::parameterize
(float64 cover_maximum_CO2_adj_
,float64 cover_initial_)                                         initialization_
{  bool paramed = Canopy_cover_curve_common::parameterize
      (cover_maximum_CO2_adj_,cover_initial_);
   B1 = 1.0 / exp(-shape_coef_vegetative * thermal_time_fractional_at_half_cover_max);
   B2 = 1.0 / exp(-shape_coef_senescent * thermal_time_fractional_at_half_cover_senescence);
   CCmax_asymptotic = (cover_maximum_CO2_adj_ - cover_initial_)
      * (1.0 + B1 * exp(-shape_coef_vegetative * 1.0)) + cover_initial_;
   return paramed;
      // Not currently needed because shape parameters
      // are calculated ?daily?
}
//_Canopy_cover_curve_2017:parameterize_____________________________2017-12-20_/
bool Canopy_cover_curve_2017::know_accrescence
(const Phenology::Period_thermal *accrescence_)                    modification_
{  return Canopy_cover_curve_common::know_accrescence(accrescence_);
}
//_know_accrescence_________________________________________________2018-12-30_/
bool Canopy_cover_curve_2017::know_culminescence
(const Phenology::Period_thermal *culminescence_)                  modification_
{  return Canopy_cover_curve_common::know_culminescence(culminescence_);
}
//_know_culminescence_______________________________________________2018-12-30_/
float64 Canopy_cover_curve_2017::calc_during_accrescence()                 const
{
   float64 relative_thermal_time =
      accrescence->get_thermal_time_relative_elapsed();                          //200304_181107
      //200304 accrescence->calc_relative_thermal_time();                        //181107
     // This would be with respect to 0 at emergence (which it previously wasn't)
   float64 calculated_cover
       = cover_initial + (CCmax_asymptotic - cover_initial)
       / (1.0 + B1 * exp(-shape_coef_vegetative * relative_thermal_time));
   cover_current = calculated_cover;
   return calculated_cover;
}
//_calc_during_accrescence__________________________________________2017-12-20_/
float64 Canopy_cover_curve_2017::calc_during_culminescence()               const
{  return cover_current;
}
//_calc_during_culminescence____________________________2014-11-23__2017-12-20_/
bool Canopy_cover_curve_2017::know_senescence
(const Phenology::Period_thermal *senescence_)                     modification_
{
   bool starting_senescence = senescence_ != 0;                                  //200409
   Canopy_cover_curve_common::know_senescence(senescence_);
   if (starting_senescence) // senescence may be deactivated with 0
   {
   float64 CCmax1_actual_unused        // 200306 obsolete
      = cover_initial
       +  (CCmax_asymptotic - cover_initial)
        / (1.0 + B1 * exp(-shape_coef_vegetative));
   // now accounts for previous stress
   CCmax2_actual
      = (cover_current // 200306 CCmax1_actual
         * (1.0 + B2) - canopy_cover_season_end) / B2;
   CCend_asymptotic
      =   CCmax2_actual
        + (canopy_cover_season_end - CCmax2_actual)
         *(1.0 + B2 * exp(-shape_coef_senescent));
   }
   return true;                                                      //181109
}
//_start_senescence_________________________________________________2017-12-20_/
float64 Canopy_cover_curve_2017::calc_during_senescence()                  const
{
   float64 relative_thermal_time =
      senescence->get_thermal_time_relative_elapsed();                           //200304
   float64 calculated_cover
      = CCmax2_actual
       - (CCmax2_actual - CCend_asymptotic)
        /(1.0 + B2 * exp(-shape_coef_senescent * relative_thermal_time));
   if (calculated_cover < canopy_cover_season_end )
      calculated_cover = canopy_cover_season_end;
   if (calculated_cover > CCmax2_actual )
      calculated_cover = CCmax2_actual;
   return calculated_cover;
}
//_calc_during_senescence___________________________________________2017-12-20_/
}//_namespace CropSyst_________________________________________________________/

