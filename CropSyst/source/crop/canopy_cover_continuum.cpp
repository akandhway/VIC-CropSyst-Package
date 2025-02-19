#include "crop/canopy_cover_continuum.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/math/moremath.h"
#include <iostream>
namespace CropSyst
{
//______________________________________________________________________________
Canopy_cover_abstract::Canopy_cover_abstract
(const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter_
,const Crop_parameters_struct::Morphology  &canopy_growth_common_parameter_
,const Phenology              &phenology_                                        //110824
,bool                          is_fruit_tree_)                                   //110422
:canopy_parameter                                            (canopy_parameter_)
,canopy_growth_common_parameter                (canopy_growth_common_parameter_)
,accrescence                                                                 (0) //190816_181109
,culminescence                                                               (0) //181109
,senescence                                                                  (0) //181109
,maturity_lapse                                                              (0)
,is_fruit_tree                                                  (is_fruit_tree_) //220422
,interception_global_green                                                 (0.0) //110906
,interception_global_green_yesterday                                       (0.0) //110906
, interception_PAR_green                       (canopy_parameter_.cover_initial)
{}
//_Canopy_cover_abstract:constructor___________________________________________/
bool Canopy_cover_abstract::know_accrescence
(const Phenology::Period_thermal *accrescence_)
{  accrescence   = accrescence_;
   culminescence = 0;
   senescence    = 0;
   return true; //200409 accrescence;
}
//_know_accrescence_________________________________________________2018-11-18_/
bool Canopy_cover_abstract::know_culminescence
(const Phenology::Period_thermal *culminescence_)
{
   accrescence   = 0;
   culminescence = culminescence_;
   senescence    = 0;
   return true; //200409 culminescence;
}
//_know_culminescence_______________________________________________2018-11-18_/
bool Canopy_cover_abstract::know_senescence
(const Phenology::Period_thermal *senescence_)
{
   accrescence    = 0;
   culminescence  = 0;
   senescence     = senescence_;
   return true; //200409 senescence;
}
//know_senescence___________________________________________________2018-11-18_/
/*200409 unused
bool Canopy_cover_abstract::know_maturity(const CS::Period *maturity_)
{  maturityX = maturity_;
   return true; //200409 maturity != 0;
}
//_know_maturity____________________________________________________2018-11-18_/
*/
bool Canopy_cover_abstract::end_day()          modification_
{  interception_global_green_yesterday = interception_global_green;              //110906
   if (maturity_lapse) maturity_lapse++;                                         //200409
   return true;
}
//_end_day__________________________________________________________2011-06-01_/
bool Canopy_cover_abstract::start_season()
{  interception_global_green = 0;
   interception_global_green_yesterday= 0;
   interception_PAR_green = canopy_parameter.cover_initial;
   return true;
}
//_start_season_____________________________________________________2011-08-30_/
float64 Canopy_cover_abstract::set_interception_insolation_global_green
(float64 interception_global_green_)               modification_
{  interception_global_green           = interception_global_green_;
   interception_global_green_yesterday = interception_global_green_;
   return interception_global_green;
}
//_set_interception_insolation_global_green_canopy____________________2013-05-20_/
float64 Canopy_cover_reference::set_interception_insolation_global_green_at_senescence
(float64 interception_insolation_global_green)                    modification_
{
    canopy_cover_curve->set_cover_at_senescence(interception_insolation_global_green);
   return set_interception_insolation_global_green(interception_insolation_global_green);
}
//_set_interception_insolation_global_green_________________________2020-03-06_/
float64 Canopy_cover_abstract::adjust_interception_insolation_global_green       //190405
(float64 adjustment)                                               modification_
{  interception_global_green             *= adjustment;
   interception_global_green_yesterday   *= adjustment;
   // Warning need to check if I want to adjust these also
   interception_PAR_green                           *= adjustment;
   return interception_global_green;
}
//_adjust_interception_insolation_global_green_canopy_________________2019-04-05_/
Canopy_cover_reference::Canopy_cover_reference
(const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter_
,const Crop_parameters_struct::Morphology  &canopy_growth_common_parameter_
,const Phenology                 &phenology_                                     //110824
,bool                             is_fruit_tree_)                                //120510_110422
: Canopy_cover_abstract
   (canopy_parameter_,canopy_growth_common_parameter_,phenology_,is_fruit_tree_)
, canopy_cover_curve                                                         (0) //171220
, canopy_senescence_rate                                                   (0.0) //100720
, unstressed_green_fract                                                   (0.0) //110513
//200304unused , expansion_potential_yesterday                                            (0.0) //110901
{  canopy_cover_curve = new Canopy_cover_curve_2017
      (canopy_parameter_.cover_green_season_end  // Warning claudio reintroduced canopy_cover_green_mature
      ,(float64)canopy_parameter_.shape_coef_accrescent
      ,(float64)canopy_parameter_.shape_coef_senescent
      ,canopy_parameter_.thermal_time_fractional_at_half_cover_max               //181015
      ,canopy_parameter_.thermal_time_fractional_at_half_cover_senescence);      //181015
}
//_Canopy_cover_reference::constructor_________________________________________/
bool Canopy_cover_reference::initialize()                        initialization_ //131206
{  return canopy_cover_curve->parameterize
      (canopy_parameter.cover_maximum_no_CO2_adj
      ,canopy_parameter.cover_initial);
}
//_initialize_______________________________________________________2011-06-28_/
bool Canopy_cover_reference::start_day()                           modification_
{  expansion_potential_today = 0;
   return Canopy_cover_abstract::start_day();
}
//_start_day________________________________________________________2011-09-01_/
bool Canopy_cover_reference::end_day()                             modification_
{
   //200304unused expansion_potential_yesterday = expansion_potential_today;
   return Canopy_cover_abstract::end_day();
}
//_end_day__________________________________________________________2019-08-21_/
float64 Canopy_cover_reference::calculate()                                const
{  return canopy_cover_curve->calculate();
}
//_Canopy_cover_reference::calculate___________________________________________/
bool Canopy_cover_reference::update_cover
(float64 leaf_water_potential_yesterday_NA_this_version)           modification_
{
/*200409
   if (maturityX && (is_fruit_tree))                                              //181107
   {  nat16 days_since_start_of_maturity = maturityX->get__day_count();            //181107
      if (days_since_start_of_maturity == 1)
            // Reduce canopy 80% at the time of tree fruit maturity
            unstressed_interception_insolation_global_green *= 0.2;
   }
*/
   if (maturity_lapse && (is_fruit_tree))                                              //181107
   {  if (maturity_lapse == 1)
            // Reduce canopy 80% at the time of tree fruit maturity
            unstressed_interception_insolation_global_green *= 0.2;
         /*else no change in interception*/
   }
   else
   {  unstressed_green_fract = calculate();                                      //181107_130821_110902
      unstressed_interception_insolation_global_green = unstressed_green_fract;
      // Allow for continuation of canopy senescence between maturity and harvest or dormancy
      if (senescence)
         canopy_senescence_rate = unstressed_interception_insolation_global_green
            - unstressed_interception_insolation_global_green_yesterday;
   }
   interception_PAR_green = 1.0 - exp(1.4 * log(1.0 - unstressed_green_fract));
         // RLN log appears to be natural log
   if (accrescence)
   {
         // set_CC_at_max_canopy is called every day just so that
         // we have a values at start of quiesence
         // because otherwise update_cover is called before quiesence is identified
      expansion_potential_today
         = unstressed_interception_insolation_global_green
         - unstressed_interception_insolation_global_green_yesterday;
      //200126 cover_latest_vegetative = cover_unstressed_green;
   }
   else expansion_potential_today = 0;                                           //200306
/*200306 no longer needed
   if (senescence)                                                               //200125
   {
      #ifdef ABANDONED
      reduction_potential_today                                                  //200124
         = abs( unstressed_interception_insolation_global_green                  //200124
         - unstressed_interception_insolation_global_green_yesterday);           //200124
      #endif
      //200126 interception_insolation_global_green =  cover_unstressed_green;   //141125
      //200126 unstressed_interception_insolation_global_green_yesterday = cover_unstressed_green;   //141125
      expansion_potential_today = 0;                                             //141125
   }
*/

   return true;                                                                  //110906
}
//_Canopy_cover_reference::update_cover________________________________________/
Canopy_cover_actual::Canopy_cover_actual
(const Crop_parameters_struct::Canopy_growth_cover_based &canopy_parameter_
,const Crop_parameters_struct::Morphology  &canopy_growth_common_parameter_
,const Phenology              &phenology_                                        //110824
,bool                          is_fruit_tree_                                    //110422
,const float64                &water_stress_index_yesterday_                     //110902
, Canopy_cover_reference      &canopy_cover_reference_)
: Canopy_cover_abstract
  (canopy_parameter_,canopy_growth_common_parameter_,phenology_,is_fruit_tree_)
,global_solar_rad_intercepted_total                                          (0) //110910
, canopy_cover_reference                               (canopy_cover_reference_)
, water_stress_index_yesterday                   (water_stress_index_yesterday_)
//200125 , first                                                                  (false)
   // RLN I think this is false but the logic seems odd to me.
, cover_attained_max                                                       (0.0)
, cover_to_lose_total                                                      (0.0)
, N_leaf_stress_factor                                                     (1.0) //200528
{}
//_Canopy_cover_actual::constructor____________________________________________/
bool Canopy_cover_reference::start_season()
{  unstressed_green_fract = 0.0;
   //200304unused expansion_potential_yesterday = 0.0;                           //110901
   expansion_potential_today = 0.0;                                              //110901
   //200306abandoned reduction_potential_today = 0.0;
   return Canopy_cover_abstract::start_season();
}
//_Canopy_cover_reference::start_season_____________________________2011-08-30_/
bool Canopy_cover_actual::start_season()                           modification_
{
   cover_attained_max = 0.0;
   cover_to_lose_total= 0.0;
   interception_global_green = 0;
      // was    FractionSolarRadInterceptedGreenCanopy
   interception_global_green_yesterday=0;
   global_solar_rad_intercepted_total = 0;                                       //110910
//200125   first = false;
   return Canopy_cover_abstract::start_season();
}
//_Canopy_cover_actual::start_season________________________________2011-08-30_/
bool Canopy_cover_actual::know_senescence
(const Phenology::Period_thermal *senescence_)
{  // This was moved from

   float64 actual_canopy_cover_green = interception_global_green_yesterday;

/* actual_canopy_cover_green is now computed directly from shape curve
   float64 actual_canopy_cover_green
      = interception_insolation_global_green_yesterday
      - canopy_cover_reference.reduction_potential_today;
      + canopy_cover_reference.expansion_potential_today;
   actual_canopy_cover_green = CORN::must_be_between<float64>
      (actual_canopy_cover_green,0.001,0.9999);
*/


   float64 actual_total_canopy_cover_at_season_end
      = canopy_parameter.cover_total_season_end;

/*2000125 warning dont delete,
Claudio says to remove this, but I think may still need CO2 adjustment

            = actual_canopy_cover_green
            * canopy_parameter.cover_total_season_end
            / canopy_parameter.cover_maximum_CO2_adj;
            // scales up the value at maturity due to CO2
*/
   cover_attained_max = actual_canopy_cover_green;
   cover_to_lose_total
            = cover_attained_max - actual_total_canopy_cover_at_season_end;
   /// global_solar_rad_intercepted_total = actual_canopy_cover_green;
   return Canopy_cover_abstract::know_senescence(senescence_);
}
//_know_senescence__________________________________________________2020-01-25_/
bool Canopy_cover_actual::know_N_leaf_stress_factor(float64 N_leaf_stress_factor_)
{  N_leaf_stress_factor = N_leaf_stress_factor_;
}
//_know_N_leaf_stress_factor________________________________________2020-05-28_/
bool Canopy_cover_actual::end_day()                                modification_
{  interception_global_green_yesterday
        = interception_global_green;
   return Canopy_cover_abstract::end_day();
}
//_end_day__________________________________________________________2019-08-21_/
bool Canopy_cover_actual::update_cover
(float64 leaf_water_potential_yesterday)                           modification_
{  // Determine green canopy cover  // in VB is ActualCanopyCover
   float64 reduce_canopy_expansion_LWP
      = canopy_growth_common_parameter.reduce_canopy_expansion_LWP;
   float64 actual_canopy_expansion_today = 0;
   /*200306
   float64 actual_canopy_reduction_today                                         //200124
      = canopy_cover_reference.reduction_potential_today;                        //200124
   */
   float64 actual_canopy_cover_green = 0.0;                                      //200306
   //200409 if (is_fruit_tree && maturityX && (maturityX->get__day_count() == 1))            //181109
   #ifdef OBSOLETE
   200701 Claudio decided to remove this reduction

   if (is_fruit_tree && maturity_lapse == 1)                                     //200409_181109
   {
      /*200421  simply reduces by 80% as was done below.
      actual_canopy_expansion_today =
            // 'Reduce canopy 80% at the time of tree fruit maturity
             - interception_global_green_yesterday * 0.8
                //101030 WARNING confirm this with Claudio,  the negative value looks odd and
                // doesn't match above, why not actual_canopy_expansion_today = solar_rad_intercepted_green_canopy_yesterday * 0.2;
         ;

      //RLN this wasnot being calculated
         actual_canopy_cover_green                                               //200421
            = interception_global_green_yesterday                                //200421
            + actual_canopy_expansion_today;                                     //200421
      */
            // Reduce canopy 80% at the time of tree fruit maturity
           interception_global_green *= 0.2;
           actual_canopy_cover_green = interception_global_green;                //200421

   } // fruit mature
   else // all other crops and not fruit mature
   #endif
   {
      float64 today_LWP = leaf_water_potential_yesterday;
      // RLN today_LWP looks odd the name implies today but the value is from yesterday.

      if (culminescence)                                                         //200306
         actual_canopy_cover_green=get_interception_global_green();              //200306
      if (senescence)                                                            //200306
         actual_canopy_cover_green
            = canopy_cover_reference.get_interception_global_green();            //200306

/*200306  No special case for maturity
      else
      if (culminescence || maturity)                                             //190816_181109
         //200306 previously include culminescence senescence
          //181109 WARNING the original 2013 condition would include culminescence period,
          // but I would think this period would not have canopy espansion check with Claudio
      {
         #ifdef obsolete_200125
         //expansion_potential_today = 0;                                        //200124
         // There is no more expansion after senescence begins                   //141125
         //already cleared above actual_canopy_expansion_today = 0.0;            //141125
         float64 cover_unstressed_green =                                        //171221
            std::min<float64>(cover_latest_vegetative                            //171221
            , canopy_cover_reference
            .get_interception_global_green());  //200228 was calc                //141125
         interception_insolation_global_green          = cover_unstressed_green; //141125
         interception_insolation_global_green_yesterday= cover_unstressed_green; //141125
         global_solar_rad_intercepted_total           = cover_unstressed_green;  //141125
         #endif
         #endif
      }
*/
      if (accrescence)
      {
         float64 stop_canopy_expansion_LWP
            = canopy_growth_common_parameter.stop_canopy_expansion_LWP;
            // was MinimumLeafWaterPotentialForCanopyExpansion;
         float64 water_stress =
            (canopy_cover_reference.expansion_potential_today > 0)
            ?  (today_LWP >= reduce_canopy_expansion_LWP )
                ? 1.0 // no stress
                : (today_LWP > stop_canopy_expansion_LWP)
                  ? (today_LWP - stop_canopy_expansion_LWP)
                     / (reduce_canopy_expansion_LWP - stop_canopy_expansion_LWP)
                  : 0.0    // max stress
            : (1.0 +  (water_stress_index_yesterday));
               // what is this else case

         float64 min_N_water_stress = std::min<float64>(water_stress,N_leaf_stress_factor);  //200528

         actual_canopy_expansion_today
            = min_N_water_stress * canopy_cover_reference.expansion_potential_today; //200528
         actual_canopy_cover_green                                               //200306
            = interception_global_green_yesterday                                //200306
            + actual_canopy_expansion_today;                                     //200306
         /*200306
         actual_canopy_expansion_today =
            (canopy_cover_reference.expansion_potential_today > 0)
            ?  (today_LWP >= reduce_canopy_expansion_LWP )
                ? canopy_cover_reference.expansion_potential_today   // no stress
                : (today_LWP > stop_canopy_expansion_LWP)
                  ? canopy_cover_reference.expansion_potential_today
                     * (today_LWP - stop_canopy_expansion_LWP)
                     / (reduce_canopy_expansion_LWP - stop_canopy_expansion_LWP)
                  : 0.0    // max stress
            : canopy_cover_reference.expansion_potential_today       // what is this case
               * (1.0 +  (water_stress_index_yesterday));
         */
      } // if thermal time
   } //if mature
   /*200306
   float64 actual_canopy_cover_green
      = interception_global_green_yesterday
      - actual_canopy_reduction_today                                            //200124
      + actual_canopy_expansion_today;
   */
   actual_canopy_cover_green = CORN::must_be_between<float64>
      (actual_canopy_cover_green,0.0,0.9999);                                    //200213
      //200213 ,0.001,0.9999

   interception_global_green = actual_canopy_cover_green;
   // PAR canopy interception assume that PAR extinction coefficient
   // is 1.4 times greater than solar extinction coefficient.
   interception_PAR_green     // output only
      = 1.0 - exp(1.4 * log(1.0 - actual_canopy_cover_green));
   // Determine total canopy cover

   if (senescence)                                                               //181107
   {  // Adjusting for effect of CO2 on total canopy cover a maturity
      /*200125 restored but moved to know_senescence
      if (!first)
      {
         //200125 restored but moved to know_senescence
         float64 actual_total_canopy_cover_at_season_end
            = actual_canopy_cover_green
            * canopy_parameter.cover_total_season_end
            / canopy_parameter.cover_maximum_CO2_adj;                            //131206
            // scales up the value at maturity due to CO2
         cover_attained_max = actual_canopy_cover_green;
         cover_to_lose_total
            = cover_attained_max - actual_total_canopy_cover_at_season_end;
         first = true;
         global_solar_rad_intercepted_total = actual_canopy_cover_green;
      } else
      */
      {
         float32 X
            = (cover_to_lose_total *
                  senescence->get_thermal_time_relative_elapsed());              //181107
                  //200403 senescence->calc_relative_thermal_time());            //181107

            // I think relative_thermal_time is calculated more than once
            // a day.  So probably only need to calculate in start day
            // and use get idiom here.  200304

         global_solar_rad_intercepted_total = cover_attained_max - X;
      }
   } else // before senescence
   {
      global_solar_rad_intercepted_total = actual_canopy_cover_green;
      /*200124
      if (accrescence)                                                           //191021
         cover_latest_vegetative = actual_canopy_cover_green;                    //191021
      */
      cover_attained_max = std::max<float64>                                     //200124
            (cover_attained_max,actual_canopy_cover_green);                      //200124
   }
   global_solar_rad_intercepted_total =                                          //140225
    CORN::must_be_between<float64>(global_solar_rad_intercepted_total,0.0,1.0);  //200409
    /*200409
      CORN::must_be_0_or_greater<float64>
      (global_solar_rad_intercepted_total);
    */
   return true;
}
//_Canopy_cover_actual::update_cover___________________________________________/
bool Canopy_cover_abstract::start_dormancy()                       modification_
{  // In VB Version this is done in Crop Development class when entering dormancy
   interception_global_green = 0;
   interception_global_green_yesterday = 0;                                      //110512
   interception_PAR_green = 0;                                                   //110512

   accrescence    = 0;                                                           //181109
   culminescence  = 0;                                                           //181109
   senescence     = 0;                                                           //181109
   maturity_lapse = 0;                                                           //200409_181109
   return true;
}
//_Canopy_cover_abstract::start_dormancy____________________________2011-04-05_/
bool Canopy_cover_reference::start_dormancy()                      modification_
{
   //200304 expansion_potential_yesterday = 0.0 ;                                //110512
   return Canopy_cover_abstract::start_dormancy();
}
//_Canopy_cover_reference::start_dormancy___________________________2011-09-01_/
bool Canopy_cover_actual::start_dormancy()                         modification_
{  interception_global_green = 0.0;
//200125 undone  first = true;
   cover_attained_max = 0.0 ;                                                    //110512
   cover_to_lose_total = 0.0 ;                                                   //110512
   return Canopy_cover_abstract::start_dormancy();
}
//_Canopy_cover_actual::start_dormancy______________________________2001-04-05_/
bool Canopy_cover_abstract::respond_to_clipping()                  modification_
{  interception_global_green = canopy_parameter.cover_initial;                   //110906
   return true;
}
//_respond_to_clipping______________________________________________2001-04-21_/
bool Canopy_cover_actual::respond_to_clipping()                    modification_
{  interception_global_green  = canopy_parameter.cover_initial;
   interception_global_green_yesterday=canopy_parameter.cover_initial;

   //130508 NYI  this will need to go back to portions that

   // we are sort of restarting
   cover_attained_max = 0.0;
//200125   first = false;
   return Canopy_cover_abstract::respond_to_clipping();
}
//_respond_to_clipping______________________________________________2001-04-21_/
float64 Canopy_cover_actual::calc_leaf_area_index()                        const
{  return calc_XXXX_area_index(get_interception_global_total());
}
//_calc_leaf_area_index_____________________________________________2011-05-26_/
float64 Canopy_cover_actual::calc_green_area_index()                       const
{  return  calc_XXXX_area_index(interception_global_green);
}
//_calc_leaf_area_index_____________________________________________2011-05-26_/
float64 Canopy_cover_actual::calc_peak_leaf_area_index()                   const
{  return calc_XXXX_area_index(cover_attained_max);
}
//_calc_leaf_area_index_____________________________________________2011-05-26_/
float64 Canopy_cover_actual::calc_XXXX_area_index(float64 fract_intercept) const
{  float64 xAI
      = - CORN_ln(1.0 - fract_intercept)
          / (float64)canopy_growth_common_parameter.light_extinction_coef;
   return xAI;
}
//_calc_XXXX_area_index_____________________________________________2011-08-11_/
float64 Canopy_cover_actual::adjust_interception_insolation_global_total
(float64 adjustment)                                              modification_
{
   Canopy_cover_abstract::adjust_interception_insolation_global_total(adjustment);
   global_solar_rad_intercepted_total *= adjustment;
   // global_solar_rad_intercepted_total_canopy_yesterday   *= adjustment;

   // we are sort of restarting
   cover_attained_max = 0.0;                                                     //190612
   //200125 first = false;                                                       //190612

   // Warning need to check if I want to adjust these also
   // PAR_solar_rad_intercepted_total_canopy                *= adjustment;
   return global_solar_rad_intercepted_total;
}
//_adjust_global_solar_rad_intercepted_total_canopy_________________2019-06-12_/
void Canopy_cover_abstract::dump_headers(std::ostream &strm,const char *prefix)  const
{  strm
      << "\t" << prefix << "_" << "interception_insolation_global_green_canopy"
      << "\t" << prefix << "_" << "interception_PAR_green_canopy";
}
//_dump_headers_____________________________________________________2011-09-06_/
void Canopy_cover_reference::dump_headers(std::ostream &strm)              const
{  Canopy_cover_abstract::dump_headers(strm,"ref");
   strm << "\texpansion_potential_today";
}
//_dump_headers_____________________________________________________2011-09-06_/
void Canopy_cover_actual::dump_headers(std::ostream &strm)                 const
{  Canopy_cover_abstract::dump_headers(strm,"act");
   strm << "\tglobal_solar_rad_intercepted_total_canopy"
        << "\tcover_attained_max"
//200125 undone         << "\tcover_to_lose_total"
        ;
}
//_dump_headers_____________________________________________________2011-09-06_/
void Canopy_cover_abstract::dump(std::ostream &strm)                       const
{  strm
   << "\t" <<interception_global_green
   << "\t" <<interception_PAR_green;
}
//_dump_____________________________________________________________2011-09-06_/
void Canopy_cover_reference::dump(std::ostream &strm)                      const
{  Canopy_cover_abstract::dump(strm);
   strm << "\t" <<expansion_potential_today;
}
//_dump_____________________________________________________________2011-09-06_/
void Canopy_cover_actual::dump(std::ostream &strm)                         const
{  Canopy_cover_abstract::dump(strm);
   strm << "\t" <<global_solar_rad_intercepted_total
      << "\t" <<cover_attained_max
//200125 undone       << "\t" <<cover_to_lose_total
      ;
}
//_dump_____________________________________________________________2011-09-06_/
}//_namespace CropSyst_________________________________________________________/

