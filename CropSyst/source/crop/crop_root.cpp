#include "crop/crop_root.h"
#include "soil/soil_I.h"
#include "crop/thermal_time.h"
#include "crop/crop_param.h"
#include <math.h>
#include "corn/math/compare.hpp"
#include "corn/measure/measures.h"

#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"
//#define DEBUG_ROOT
#include <fstream>
#ifdef DEBUG_ROOT
extern CORN::Date simulation_today;
std::ofstream roots_txt("roots.txt");
#endif
namespace CropSyst {
//______________________________________________________________________________
Crop_root_common::Crop_root_common()
: root_length(0)
, biomass(0)
{}
//_Crop_root_common:constructor_____________________________________2013-09-09_/
Crop_root_dormant::Crop_root_dormant
(float64 _dormant_root_length
,float64 _biomass
,soil_layer_array64(_biomass_by_layer))
{  biomass     = _biomass;                                                       //130909
   root_length = _dormant_root_length;                                           //130909
   copy_layer_array64(biomass_by_layer,_biomass_by_layer);
}
//_Crop_root_dormant:constructor_______________________________________________/
Crop_root_vital::Crop_root_vital
(const Crop_parameters_struct::Root       &parameters_
,const Phenology::Period_thermal           &root_growth_period_                  //181108
,const Soil::Layers_interface              &soil_layers_
,float32                                   initial_root_length_)
   // initial_root_length_ is used by  calc_max_N_update_per_unit_root_length()
:soil_layers                                                      (soil_layers_)
,parameters                                                        (parameters_) //061212
,root_growth_period                                        (root_growth_period_) //181108
,apparent_root_depth                                      (initial_root_length_)
,growth_biomass                                                              (0) //070130
,normalized_thermal_time_today                                               (0) //061212
,normalized_thermal_time_yesterday                                           (0) //061212
,root_length_unstressed_today                             (initial_root_length_)
,root_length_unstressed_yesterday                         (initial_root_length_)
,biomass_yesterday                                                           (0) //070410
,rooting_layer                                                               (2)
,start_rooting_depth                          (soil_layers_.get_thickness_m(1) )
,total_root_lengths                                                   (0.000001) //150601
{
   root_length                    = initial_root_length_;                        //130909
   for (short int sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)
   {  root_density            [sublayer] = 0.0;
      total_fract_root_length [sublayer] = 0.0;                                  //010322
      root_lengths            [sublayer] = 0.0;                                  //061129
   }
}
//_Crop_root_vital:constructor__________________________________________________
bool Crop_root_vital::initialize(float64 root_length_to_initialize_to) initialization_
{  for (short int sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)
   {  root_density            [sublayer] = 0.0;
      total_fract_root_length [sublayer] = 0.0;                                  //010322
      root_lengths            [sublayer] = 0.0;                                  //061129
   }
   return true;
}
//_initialize__________________________________________________________________/
float64 Crop_root_vital::update_length(float64 stress_factor)
// Currently only the water stress factor!
{  // after end of LAI development no more root growth
   if (root_growth_period.has_expired()) return root_length;

   // no root growth until emergence
   // Calculate length depth gain today
   // When we reach deg days of max root depth the normalized_thermal_time_today will be 1.
   float64 period_accum_GDDs
      = root_growth_period.get_thermal_time_accum(/*would always be unclipped false*/) //200201
      - root_growth_period.get_param_thermal_time_accum();
   float64 period_duration_GDDs = root_growth_period.get_param_duration_GDDs();
   normalized_thermal_time_today = period_accum_GDDs  / period_duration_GDDs;

   /* Not sure why root_growth_period is including the germination otherwise this should work
      = root_growth_period.calc_relative_thermal_time(); //181107
   */
   float64 max_seasonal_root_length_gain
      = (parameters.max_depth - start_rooting_depth)
        - cm_to_m(parameters.length_at_emergence_cm);
   root_length_unstressed_today = cm_to_m(parameters.length_at_emergence_cm)
      + max_seasonal_root_length_gain * normalized_thermal_time_today;
   float64 stress_adjustment =
      (stress_factor < 0.000001) ? 1.0 :                                         //131011
      pow(stress_factor,(float64)parameters.sensitivity_to_water_stress);        //121004
   float64  root_depth_gain // Today's growth
      = (root_length_unstressed_today - root_length_unstressed_yesterday)
       * stress_adjustment;
   if (root_depth_gain < 0.0)                                                    //070105
      root_depth_gain = 0;                                                       //070105
   root_length += root_depth_gain;
   root_length = std::min<float64>(root_length,parameters.max_depth - start_rooting_depth); //070321
   return root_length;
}
//_update_length____________________________________________________2006-12-12_/
bool Crop_root_vital::start_day()
{  normalized_thermal_time_yesterday = normalized_thermal_time_today; // <- this should be in end_day()
   root_length_unstressed_yesterday  = root_length_unstressed_today;
   biomass_yesterday = biomass;                                                  //070130
   return true;
}
//_start_day________________________________________________________2006-12-12_/
float64 Crop_root_vital::update_root_depth(bool terminate_pending)
{  if (!terminate_pending)
   {  // When terminate has been started (process biomatter fate) the root_length will have been reset
      apparent_root_depth = (root_length > 0.0)                                  //981208
      ? root_length + start_rooting_depth                                        //981208
      : 0.0;                                                                     //981208
      float64 limit_to_depth =soil_layers.get_depth_profile_m();
      if (limit_to_depth > 0.00001)          // Could be 0 if dummy soil         //981208
         apparent_root_depth = std::min<float64>(apparent_root_depth, limit_to_depth );   //981208
   }
   return apparent_root_depth;                                                   //981208
}
//_update_root_depth________________________________________________1998-10-23_/
float64 Crop_root_vital::update_root_densities()
{
   clear_layer_array64(root_distribution_unadjust); root_distr_unadjust_sum=0;   //180727
   float64 depth_top_soil_layer = 0;                                             //030722
   float64 density_area_sum = 0;                                                 //030722
   float64 max_root_length = (parameters.max_depth - start_rooting_depth);       //030722
   max_root_density = parameters.surface_density*root_length/max_root_length;    //030722
   nat8  number_effective_layers = soil_layers.count_uninundated();              //061208
   for (nat8  layer = rooting_layer; layer <= number_effective_layers; layer++)  //030722
   {  float64 layer_thickness = soil_layers.get_thickness_m(layer);              //061208
      float64 density_distribution_curvature
         = parameters.density_distribution_curvature;                            //061212
      float64 exp_term = exp(density_distribution_curvature *
         (root_length - depth_top_soil_layer) / parameters.surface_density);     //030722
      float64 root_distr_unadjust_layer = CORN::must_be_0_or_greater<float64>    //180727
         (  - max_root_density * (1.0 - exp_term));                              //180727
      root_distribution_unadjust[layer] = root_distr_unadjust_layer;             //180727
      root_distr_unadjust_sum += root_distr_unadjust_layer;                      //180727
      depth_top_soil_layer += layer_thickness;                                   //030722
   }
   return root_distr_unadjust_sum;                                               //180726
}
//_update_root_densities____________________________________________2006-12-08_/
float64 Crop_root_vital::update_fractions()
{
   // In CropSyst, we use density area, for CropGro, use simply density.
   nat8 layers_eff = soil_layers.count_uninundated();                            //080211_031202_031202
      // usually the number of sublayers (but may be less in water table conditions)
   clear_sublayer_array(total_fract_root_length);                                //051013
   if (CORN::is_zero<float64>(root_distr_unadjust_sum,1e-12))
      //031203  180727LML at the start of growing season the L_sum is very small
      return 1.0; // no roots yet (this may happen in CropGro).                  //031203
   float64 sum_fract_root = 0.0;
   for (nat8  layer = rooting_layer; layer <= layers_eff; layer++)               //981021
   {  total_fract_root_length[layer] =                                           //010322
         root_distribution_unadjust[layer] /  root_distr_unadjust_sum;           //180726_030721
      sum_fract_root += total_fract_root_length[layer];                          //010322
   }
   if (is_zero<float64>(sum_fract_root))                                         //981023
      sum_fract_root = 1.0;                                                      //981023
   float64 soil_depth_limit = soil_layers.get_depth_m(layers_eff)                //981009
      - (soil_layers.get_thickness_m(layers_eff)/2.0);
   if (((root_length + soil_layers.get_depth_m(rooting_layer-1)) > soil_depth_limit)
           || //180521LML_981009
      (! CORN::is_approximately<float64>(sum_fract_root,1.0,0.000000001)))
   {  for (nat8  layer = rooting_layer; layer <= layers_eff; layer++)
        total_fract_root_length[layer] *= 1.0 / sum_fract_root;                  //010322
      for (nat8  layer = layers_eff+1;  layer <= MAX_soil_layers ;layer++)       //981021
        total_fract_root_length[layer] = 0.0;                                    //010322
      sum_fract_root = 1.0;                                                      //061208
   }
   return sum_fract_root;
}
//_update_fractions____________________________________________________________/
float64 Crop_root_vital::update_lengths()
{  float64 root_length_remaining = root_length;
   nat8  number_soil_layers = soil_layers.count();
   total_root_lengths = 0;
   for (nat8  lyr = rooting_layer
       ;(lyr <= number_soil_layers) && (root_length_remaining > 0.0); lyr++)     //120514
   {  float64 soil_layer_thickness = soil_layers.get_thickness_m(lyr);
      float64 soil_layer_root_length
         = (root_length_remaining > soil_layer_thickness)
         ? soil_layer_thickness
         : root_length_remaining;
      root_density[lyr] = max_root_density * total_fract_root_length[lyr] / total_fract_root_length[rooting_layer];
      root_length_remaining -= soil_layer_root_length;
      float64 root_density_lyr_m_m3 = cm_per_cm3_to_m_per_m3(root_density[lyr]);  //060612 cm/cm3
      float64 root_lengths_lyr // This is the length of all root pieces in this layer
         = root_density_lyr_m_m3 * soil_layer_root_length;  // usually layer thickness execpt for the last
      root_lengths[lyr] = root_lengths_lyr;                                      //061129
      total_root_lengths += root_lengths_lyr;
#ifdef DEBUG_ROOT
roots_txt <<
simulation_today.get()
<<  '\t'  <<
(int) lyr << '\t' <<soil_layer_root_length << '\t' << root_density_lyr_m_m3
   << '\t' <<  root_lengths_lyr<< '\t' << total_root_lengths << '\t'
   <<total_fract_root_length[lyr]<< std::endl;
#endif
   }
   return total_root_lengths;
}
//_update_lengths___________________________________________________2006-12-08_/
bool Crop_root_vital::update
(float64 stress_factor_water                                                     //191021
,bool terminate_pending)
{  bool updated = true;
   update_length(stress_factor_water);                                           //191021
   update_root_depth(terminate_pending);                                         //130909_981023
   float64 density_area_sum = update_root_densities();                           //180726_061208
   update_fractions();                                                           //031102
   update_lengths();                                                             //061208
   biomass = calc_biomass(0);                                                    //070130
   growth_biomass = (biomass > biomass_yesterday)
      ? biomass - biomass_yesterday : 0.0;                                       //070130
   return updated;
}
//_update_______________________________________________________________2003?__/
float64 Crop_root_dormant::calc_biomass(float64 *output_root_biomass_by_layer) const
{  // if root_biomass_by_layer array is passed, record the root biomass values by layer
   float64 calced_root_biomass = 0.0;
   for (nat8  lyr = 0; (lyr < MAX_soil_layers); lyr++)
   {  if (output_root_biomass_by_layer)
         output_root_biomass_by_layer[lyr] = biomass_by_layer[lyr];
      calced_root_biomass +=  biomass_by_layer[lyr];
   }
   return calced_root_biomass;
}
//_calc_biomass_____________________________________________________2006-06-12_/
bool Crop_root_vital::kill()
{  clear_layer_array64(root_density);
   clear_layer_array64(total_fract_root_length);
   clear_layer_array64(root_lengths);
   root_length = 0;
   return true;                                                                  //190709
}
//_kill_____________________________________________________________2006-06-12_/
RENDER_INSPECTORS_DEFINITION(CropSyst::Crop_root_common)
{
   #ifdef CS_OBSERVATION
/*NYI
CSVP_crop_base_root_fraction   1-31
*/
   // uses crop emanator                                                         //160616
   // Now the root object simply uses the context emanator

   inspectors.append(new CS::Inspector_scalar(biomass,UC_kg_m2,*context
      ,"root/biomass" ,value_statistic,CSVP_crop_base_root_biomass_current));
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-09_/
RENDER_INSPECTORS_DEFINITION(CropSyst::Crop_root_vital)
{
   #ifdef CS_OBSERVATION
   // Crop_root_vital uses the crop's emanator.                                  //160616
   inspectors.append(new CS::Inspector_scalar(apparent_root_depth,UC_meter
      ,*context,"root/depth",value_statistic,CSVP_crop_base_root_depth));
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-09_/
}//_namespace CropSyst_________________________________________________________/


