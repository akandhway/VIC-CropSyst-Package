#include "crop/crop_param.h"
#include "crop/biomass_growth_RUE_TUE.h"
#include "crop/thermal_time_common.h"
#include "crop/CO2_response.h"
#include "crop/canopy_growth.h"

#include "common/weather/parameter/WP_solar_radiation.h"

namespace CropSyst
{
//______________________________________________________________________________
Biomass_growth_RUE_TUE::Biomass_growth_RUE_TUE
(bool                                is_fruit_tree_
,const Crop_parameters              *parameters_
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
,const Normal_crop_event_sequence   &growth_stage_
#endif
,const nat8                         &emergence_count_
,const float64                      &pot_transpiration_m_CO2_adj_                //191025
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
,const Thermal_time_common          *thermal_time_
#endif
#if ((PHENOLOGY_VERSION==2018))
,const float64                      &accum_deg_days_adjusted_for_clipping_
#endif

,const Crop_CO2_response            *CO2_response_
,const Canopy_leaf_growth           *canopy_leaf_growth_                         //191025
,const Air_temperature_average      &air_temperature_avg_
,const Solar_radiation              &solar_radiation_
,const float64                      &intercepted_radiation_
#ifdef YAML_PARAM_VERSION
// Using adjustable parameter idiom
,const CropSyst::Biomass_production_transpiration_dependent_V5 &transpiration_dependent_growth_
#else
,const CropSyst::Biomass_production_transpiration_dependent_V4 &transpiration_dependent_growth_
#endif
,CORN::statistical::Sample_cumulative<float64,float64>
      &report_temperature_stress_stats_
//200608 ,const bool                         &at_max_fruit_load_ref_                       //180308
)
: Biomass_growth()
, is_fruit_tree                                                 (is_fruit_tree_)
, parameters                                                       (parameters_)
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
, growth_stage                                                   (growth_stage_)
#endif
, emergence_count                                             (emergence_count_)
, pot_transpiration_m_CO2_adj                     (pot_transpiration_m_CO2_adj_) //191025
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
, thermal_time                                                   (thermal_time_)
#endif
#if ((PHENOLOGY_VERSION==2018))
,accum_deg_days_adjusted_for_clipping    (accum_deg_days_adjusted_for_clipping_) //181118
#endif
, CO2_response                                                   (CO2_response_)
, canopy                                                   (canopy_leaf_growth_) //191025 (canopy_)
, air_temperature_avg                                     (air_temperature_avg_)
, solar_radiation                                             (solar_radiation_)
, intercepted_radiation                                 (intercepted_radiation_)
, transpiration_dependent_growth               (transpiration_dependent_growth_)
, report_temperature_stress_stats             (report_temperature_stress_stats_)
//200608 , at_max_fruit_load_unused                                     (at_max_fruit_load_ref_) //180308
, efficiency_model      (parameters_->ref_biomass_production().efficiency_model) //191030
, RUE_efficiency_decrease_rate_kg_m2_d_MJ2
   (g_to_kg(parameters->ref_biomass_production().RUE_efficiency_decrease_rate))
{
   for (int i = 0; i <= 1; i++)                                                  //191030
      RUE_kg_MJ[i] = g_to_kg(parameters_->ref_biomass_production().RUE[i]);      //191030
   efficiency_model =
      (parameters->biomass_production.efficiency_model_raw == "TUE")
      ? TUE_efficiency
      : RUE_efficiency;
}
//_Biomass_growth_RUE_TUE:constructor_______________________________2018-01-11_/
float64 Biomass_growth_RUE_TUE::calc_attainable_top_growth()        calculation_
{
   float64 RUE_temperature_limitation_factor = is_fruit_tree
      ? 1.0 : calc_RUE_temperature_limitation();                                 //180111_130513

   // WUE = Water Use Efficiency
   #ifdef YAML_PARAM_VERSION
      // YAML version (and eventually V5) use parameter adjustment idiom         //150907
   #else
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   float64 accum_deg_days_adjusted_for_clipping
      = thermal_time->get_accum_degree_days(/* now default true*/);              //151110_100730_130822
   #endif
   bool before_WUE_change                                                        //080721
      = !parameters->has_leaf_senescence() // 100729 (Not sure if has_leaf_senescence is needed, it so probably should hide the WUE_changes checkbox in the crop editor.
         || (parameters->biomass_production.WUE_changes &&                       //110218_100729
             (accum_deg_days_adjusted_for_clipping <= parameters->phenology.WUE_change));  //100730
   #endif

   float64 solar_rad = solar_radiation.get_MJ_m2();                              //140813_131104
   float64 RUE_kg_MJ = get_RUE_global_kg_MJ();                                   //191030
      // either RUE_at_low_VPD or RUE_at_low_total_radiation and CO2 adjusted.

   float64 result_attainable_top_growth = 0;

   if (efficiency_model == TUE_efficiency)                                       //191030
   {
      float64 radiation_dependent_growth
      =  intercepted_radiation * RUE_kg_MJ;
      float64 temperature_corrected_intercepted_radiation_dependent_growth       //080428
      = radiation_dependent_growth * RUE_temperature_limitation_factor;          //080428
      float64 pot_transpiration_dependent_growth                                 //080721
      = transpiration_dependent_growth.calc_potential_biomass_production
           (pot_transpiration_m_CO2_adj);                                        //191025
      result_attainable_top_growth =                                             //011022
         std::min<float64>                                                       //011022
         (temperature_corrected_intercepted_radiation_dependent_growth           //080428
         ,pot_transpiration_dependent_growth);                                   //011022

   } else // RUE_EFFICIENCY
   {  // based on global radiation
      //float64 RUE_efficiency_decrease_rate_kg_m2_d_MJ2 = ;
            // (g m� d)/ MJ�
      float64 RUE_kg_MJ_adjusted = RUE_kg_MJ
         - (RUE_efficiency_decrease_rate_kg_m2_d_MJ2 * solar_rad);
      float64 radiation_dependent_growth
         =  intercepted_radiation * RUE_kg_MJ_adjusted;
      float64 temperature_corrected_intercepted_radiation_dependent_growth       //080428
          = radiation_dependent_growth * RUE_temperature_limitation_factor;      //080428
      result_attainable_top_growth
         =  temperature_corrected_intercepted_radiation_dependent_growth;        //011022
   }
   float64 continuous_cultivation_impact_factor_local                            //160916
      = parameters->biomass_production.continuous_cultivation_impact_factor;
   #ifndef YAML_PARAM_VERSION
   if (parameters->is_perennial())                                               //080801
   {
      int season_index = (emergence_count > 5) ? 5 : emergence_count;            //080801
      continuous_cultivation_impact_factor_local =                               //151101
         parameters->biomass_production.season_biomass_adjustment[season_index]; //110218
   }
   #endif
/*std::cout << today.as_string() << "\tR:\t" << temperature_corrected_intercepted_radiation_dependent_growth
         << "\tT:\t"    << pot_transpiration_dependent_growth
         << "\tRad:\t"  << radiation_dependent_growth
         //<< "\tPOT_Trans:\t" << m_to_mm(pot_transpiration_m[CO2_NONE])
         << "\tPOT_Trans_CO2_adj:\t" << m_to_mm(pot_transpiration_m_CO2_adj)
         //<< "\ttradj:\t" << CO2_transpiration_adjustment_factor
         << "\tTUEadj:\t" << CO2_TUE_adjustment
         << "\tRUE_T_factor:\t" << RUE_temperature_limitation_factor
         << "\tresult_attainable_top_growth_before_adjust:" << result_attainable_top_growth
         << "\tcontinuous_cultivation_impact_factor_local:" << continuous_cultivation_impact_factor_local
         << std::endl;*/
   result_attainable_top_growth *= continuous_cultivation_impact_factor_local;   //160916151101
   // Only consider temperature stress in when PAR conversion to biomass is the limiting factor

   float64 temperature_stress =                                                  //011022
   RUE_temperature_limitation_factor;                                            //011022
   report_temperature_stress_stats.append(temperature_stress);                   //010726
   return result_attainable_top_growth;                                          //011022
}
//_calc_attainable_top_growth____________________________2018-01-11_2001-10-22_/
float64 Biomass_growth_RUE_TUE::calc_RUE_temperature_limitation()          const
{  float64 TSF = 1.0; // RUE_temperature_limitation_factor
   float64 min_tolerable_temperature =
      parameters->biomass_production.temperature_limitation.min;
   float64 max_tolerable_temperature =
      parameters->biomass_production.temperature_limitation.max;
   float64 low_optimum_temperature   =
      parameters->biomass_production.temperature_limitation.low_optimum;
   float64 high_optimum_temperature  =
      parameters->biomass_production.temperature_limitation.high_optimum;
   if (   (air_temperature_avg <= min_tolerable_temperature)
       || (air_temperature_avg >= max_tolerable_temperature))
      TSF = 0.00000001 ; // full limitation (avoid div0)
   else if
      (   (air_temperature_avg >= low_optimum_temperature)
       || (air_temperature_avg <= high_optimum_temperature))
      TSF = 1.0;
   if (   (air_temperature_avg < low_optimum_temperature)
       && (air_temperature_avg > min_tolerable_temperature))
      TSF = 1.0 - (  (low_optimum_temperature - air_temperature_avg)
                   / (low_optimum_temperature - min_tolerable_temperature));
   if (   (air_temperature_avg > high_optimum_temperature)
       && (air_temperature_avg < max_tolerable_temperature))
      TSF = (max_tolerable_temperature - air_temperature_avg)
           /(max_tolerable_temperature - high_optimum_temperature);
   if (TSF > 1.0)      TSF = 1.0;       // No limitation   //011016
   if (TSF <= 0.01)    TSF = 0.00000001;// full limitation //011016
   return TSF;
}
//_calc_RUE_temperature_limitation______________________2016-02-04__2013-05-13_/
float64 Biomass_growth_RUE_TUE::get_RUE_global_kg_MJ() const
{
/*191030 it looks like we never really had RUE change,
and in anycase, if it is needed, can use parameter adjustment methods
   // WUE = Water Use Efficiency
   #if (!defined(YAML_PARAM_VERSION) || (CROPSYST_VERSION==4))
   //170217 (CROPSYST_VERSION==4)
   float64 accum_deg_days =
         #if (PHENOLOGY_VERSION==2018)
         accum_deg_days_adjusted_for_clipping;                                   //181230
         #else
         thermal_time->get__accum_degree_days(true);                              //130822_080408
         #endif
   bool before_WUE_change = (!parameters->has_leaf_senescence()
        || accum_deg_days < parameters->phenology.WUE_change);                   //150913_080514
   #endif
   #ifdef CROP_ORCHARD
   #ifndef CROP_CALIBRATION
   if (at_max_fruit_load) before_WUE_change = true;
      // for fruit switch  transpiration to WUE after reproductive growth parameter. //081103
   #endif
   #endif
   float64 RUE_global =
      parameters->ref_biomass_production().RUE_global;                           //150907
      #ifdef YAML_PARAM_VERSION
      parameters->ref_biomass_production().RUE_global;                           //150907
      #else
      parameters->param_RUE_global(before_WUE_change);                           //110904
      #endif

*/
   float64 RUE_global_kg_MJ = RUE_kg_MJ[efficiency_model];                       //191030
   #ifdef CO2_CHANGE
   float64 CO2_RUE_adjustment = CO2_response
      ? CO2_response->actual_biomass_gain_ratio_in_response_to_CO2_RUE
      : 1.0;                                                                     //110904
   RUE_global_kg_MJ *= CO2_RUE_adjustment;
   #endif
   return RUE_global_kg_MJ;
}
//_get_radiation_use_efficiency_global__________________2018-01-11__2008-04-29_/
}//_namespace CropSyst_________________________________________________________/

