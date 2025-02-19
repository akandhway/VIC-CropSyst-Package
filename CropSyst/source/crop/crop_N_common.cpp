#include "options.h"
#include "crop_N_common.h"
#include "crop/crop_cropsyst.h"
#include "crop/canopy_growth.h"
#include "common/biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#include "crop/crop_N_balancer.h"
#include "soil/soil_I.h"
#include "soil/hydrology_I.h"
#include "soil/structure_I.h"
#include "soil/nitrogen_I.h"
#include "corn/measure/measures.h"                                                    //160926LML

#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"
#define TS_VALUE value_statistic

#ifdef DEBUG_CROP_N_FATE
#ifdef _Windows
extern ofstream crop_N_fate_txt_file;
bool debug_N_pot_uptake_headers_written = false;
ofstream debug_N_pot_uptake("N_pot_uptake.txt");
#endif
#endif
#define xDEBUG_N_CONC
#ifdef DEBUG_N_CONC
#include <iomanip>
ofstream debug_N_conc ("N_conc.txt");
#endif
/*200813
obsolete
#ifdef QND
#include <q_n_d.h>
#endif
*/
#include "crop/CO2_response.h"

namespace CropSyst
{
#ifdef DEBUG_CROP_N_FATE
   static bool N_fate_headers_dumped =false;
#endif
//______________________________________________________________________________
//                                                                       // C3       C4      CAM (crassulacean acid metabolism)
static const float64 N_crit_conc_at_emergence_species              [3] = { 0/*???*/,0/*???*/,0/*???*/};
static const float64 scaling_factor_min_N_conc_species_N_crit_based[3] = { 0.69    ,0.69    ,0.69  };
// to scale critical to minimum
//______________________________________________________________________________
Crop_nitrogen_common::Crop_nitrogen_common
(CropSyst::Crop_parameters::Nitrogen &parameters_
,Crop_complete             &crop_                                                //020505
,Soil::Soil_interface      *soil_                                                //050721
,Soil::Nitrogen            *chemicals_                                           //050721
,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance              //150601
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference_  //150601
#if ((PHENOLOGY_VERSION==2018))
,const float64 &accum_deg_days_adjusted_for_clipping_                            //181118
#endif
      )
: crop             (crop_)                                                       //020505
, crop_parameters  (*crop_.parameters)                                           //151029
, parameters       (parameters_)                                                 //070209
, photosynthetic_pathway(crop_parameters.photosynthetic_pathway_clad.get())      //070202
   // Must be initialized before N_xxx_conc_at_maturity!
, soil_properties  (soil_)                                                       //050721
, soil_chemicals   (chemicals_)                                                  //050721
, slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
   (slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference_)     //150601
, N_limited_growth                  (0.0)                                        //151105
, N_limited_pot_transpiration       (0.0)                                        //151105
, stress_factor                     (1.0)                                        //151031
, canopy_resistance_under_N_stress  (0.00081)                                    //150601
, daily                             ()
, seasonal                          ()
, seedling_N                        (0.0)
, reported_uptake_accum             (0.0)                                        //160628
, reported_canopy_mass_current      (0.0)                                        //161110
, reported_root_conc                (0.0)                                        //161110
, canopy_concentration_vital        (0.0)                                        //161110
, root_concentration_vital          (0.0)                                        //161110
, canopy_concentration_effete       (0.0)                                        //161110
, root_concentration_effete         (0.0)                                        //161110
, root_N_mass_vital                 (0.0)                                        //161110
, canopy_N_mass_vital               (0.0)                                        //161110
, root_N_mass_effete                (0.0)                                        //161110
, canopy_N_mass_effete              (0.0)                                        //161110
, store_persistent                  (0.0)                                        //161110
, max_N_uptake_per_unit_root_length (0.0)                                        //161110
, N_fate_today                      (0)                                          //161110
, stress_factor_stats               (STAT_arithmetic_mean_bit)                   //011117
, automatic_nitrogen_to_plant       (false) // now set based on management event //050722
, water_availability_coef           (1.0) // Setup in initialize                 //070209
, NO3_N_availability_adj            (1.0) // Setup in initialize                 //070209
, NH4_N_availability_adj            (1.0) // Setup in initialize                 //070209
, N_max_conc_at_change              (0.0)                                        //161110
, N_crit_conc_at_change             (0.0)                                        //161110
, N_min_conc_at_change              (0.0)                                        //161110
, N_max_conc_at_maturity (parameters_.dilution_curve_max_conc_mature)            //150913_120505_070129
, N_crit_conc_at_maturity(parameters_.calc_N_mature_crit(photosynthetic_pathway))//120505_070129
, N_min_conc_at_maturity (parameters_.calc_N_mature_min(photosynthetic_pathway)) //120505_070129
, N_max_concentration               (0.0)                                        //161110
, N_crit_concentration              (0.0)                                        //161110
, N_min_concentration               (0.0)                                        //161110
, N_max_conc_at_emergence           (0.0)                                        //161110
, concentration_slope               (0.0)                                        //161110
, N_crit_conc_at_emergence          (0.0)                                        //161110
, N_min_conc_at_emergence           (0.0)                                        //161110
, before_change                     (true)                                       //130405
, start_change_today                (false)                                      //070126_121219(was start_flowering_today)
, after_start_of_maturity           (false)                                      //161110
, mass_balance                      (0)                                          //070219
#if ((PHENOLOGY_VERSION==2018))
,accum_deg_days_adjusted_for_clipping(accum_deg_days_adjusted_for_clipping_)     //181118
#endif
{}
//_Crop_nitrogen_common:constructor____________________________________________/
Crop_nitrogen_common::~Crop_nitrogen_common()
{  delete N_fate_today; N_fate_today = 0;
   end_day(); // This is needed because the crop may be deleted before the end of the day. //070221
}
//_~Crop_nitrogen_common_______________________________________________________/
float64 Crop_nitrogen_common::get_N_max_conc_at_emergence()                const
{  float64 param_N_max_conc_emerge  = crop.parameters->nitrogen.emergence_max_conc;
   return param_N_max_conc_emerge;
}
//_get_N_max_conc_at_emergence______________________________________2006-12-12_/
float64 Crop_nitrogen_common::get_N_crit_conc_at_emergence()               const
{  return crop.parameters->nitrogen.emergence_crit_conc;
}
//_get_N_crit_conc_at_emergence_____________________________________2012-12-19_/
float64 Crop_nitrogen_common::get_residue_concentration()                  const
{  float64 straw_N_conc // Warning may need selection for vital and/or effect      070326
   = parameters.stubble_max_conc
      * canopy_concentration_vital / N_max_conc_at_maturity;                     //070129
   straw_N_conc = std::min<float64>(straw_N_conc,parameters.stubble_max_conc);   //160405
   return straw_N_conc;
}
//_get_residue_concentratioN________________________________________2006-12-15_/
float64 Crop_nitrogen_common::get_expected_root_concentration()            const
{  float64 root_N_conc = parameters.root_conc;  // 070326 Warning may need selection for vital and/or effet
   return root_N_conc;
}
//_get_expected_root_concentration__________________________________2006-12-15_/
bool Crop_nitrogen_common::initialize()                          initialization_ //150601
{  daily.clear();
   seasonal.clear();
   seedling_N                 = 0.0;
   canopy_N_mass_vital        = 0.0;
   canopy_N_mass_effete       = 0.0;
   canopy_concentration_vital = 0.0;
   canopy_concentration_effete= 0.0;
   root_concentration_vital   = 0.0;
   root_concentration_effete  = 0.0;
   N_fate_today               = 0;
   root_N_mass_vital          = 0.0;
   root_N_mass_effete         = 0.0;
   store_persistent           = 0.0;                                             //160409
   /*200529
   stress_factor_stats.reset();
   stress_factor_stats.append(1.0);                                              //200529
   */
   stress_factor_stats.clear (1.0);                                              //200529
   N_max_conc_at_change       = 0.0;
   N_crit_conc_at_change      = 0.0;
   N_min_conc_at_change       = 0.0;
   N_max_concentration        = 0.0;
   N_crit_concentration       = 0.0;
   N_min_concentration        = 0.0;
   N_max_conc_at_emergence = get_N_max_conc_at_emergence();                      //120427
   parameters.calc_biomass_to_start_dilution_min_N_conc_T_ha
         (photosynthetic_pathway);
   {  N_crit_conc_at_emergence = get_N_crit_conc_at_emergence();                 //121219
      float64 scaling_factor_min_N_conc
        = scaling_factor_min_N_conc_species_N_crit_based[photosynthetic_pathway];//121219
      N_min_conc_at_emergence = N_crit_conc_at_emergence * scaling_factor_min_N_conc;
   }
   concentration_slope =
       - crop.parameters->nitrogen.provide_concentration_slope(photosynthetic_pathway);
   parameters.PAW_where_N_uptake_rate_decreases = CORN::must_be_greater_or_equal_to<float64>
      (parameters.PAW_where_N_uptake_rate_decreases,0.00001);
   water_availability_coef = 5.259 * pow((float64)(parameters.PAW_where_N_uptake_rate_decreases),-1.0246);
   // The constants for water_availability_coef were derived from a fitted power trend.
   // see the spreadsheet C:\CS_Suite_4\CropSyst\crop_editor\manual\N_uptake.xls
   float64 N_conc_where_N_uptake_decreases_minus_residual_N
      = parameters.soil_N_conc_where_N_uptake_decreases
       -parameters.residual_N_not_available_for_uptake;
   NO3_N_availability_adj = (N_conc_where_N_uptake_decreases_minus_residual_N > 0.00001)   // avoid power of negatives.
   ? 4.9259 * pow(N_conc_where_N_uptake_decreases_minus_residual_N,-0.9821)
   : 0.0;
   NH4_N_availability_adj =  4.9259
      * pow((float64)(parameters.soil_N_conc_where_N_uptake_decreases),-0.9821);
   // The constants for N_availability_adj were derived from a fitted power trend.
   // see the spreadsheet C:\CS_Suite_4\CropSyst\crop_editor\manual\N_uptake.xls
/*
#ifdef DEBUG_N_POT_UPTAKE
if (!debug_N_pot_uptake_headers_written)
{

cout << "Temporary output enabled:_N_pot_uptake.txt" << endl;
   debug_N_pot_uptake_headers_written = true;
   debug_N_pot_uptake << "today\tlayer\tmax_n_uptake_unit_length\troot_lengths\t"
                      << "root_activity\t"
                      <<  "residual_N(kg/m2)\t"
                      <<  "residual_N(ppm)\t"
                      <<  "soil_NO3_N(kg/m2)\tsoil_NO3_N(ppm)\t"
                      << "NO3_avail_fact\t" << "NO3_N_pot_up(kg/m2)\t" << "soil_NO3_avail(kg/m2)\t" << "limited_NO3_N_pot_up(kg/m2)\t"
                      <<  "soil_NH4_N(kg/m2)\tsoil_NH4_N(ppm)\t"
                      << "NH4_avail_fact\t" << "NH4_N_pot_up(kg/m2)\t" << "limited_NH4_N_pot_up(kg/m2)\t"
                      << "water_avail_fact\t"
                      << "NO3_max_n_uptake_unit_length\t"
                      << "NH4_max_n_uptake_unit_length\t"
                      << endl;
}
#endif
*/
   return true;                                                                  //150601
}
//_initialize_______________________________________________________2006-12-14_/
void Crop_nitrogen_common::restart
(float64 initial_biomass,float64 restart_GAI,bool use_clumping_factor, bool after_dormancy)
{  crop.canopy_leaf_growth->calc_cover_fract
      (restart_GAI,false,use_clumping_factor,false);                             //080801
   crop.get_canopy_biomass_kg_m2_including(include_vital|include_effete);  // check this
   float64 seedling_canopy_N_mass = 0.0;                                         //070321
   if (!after_dormancy)                                                          //070321
   {  seedling_canopy_N_mass = initial_biomass  * get_N_max_conc_at_emergence(); //070321
      canopy_N_mass_vital = seedling_canopy_N_mass;                              //070321
   }
   canopy_concentration_vital = get_N_max_conc_at_emergence();                   //061212
   stress_factor_stats.clear(1.0);                                               //200529
/*200529
   stress_factor_stats.reset();                                                  //011117
   stress_factor_stats.ref_current() = 1.0;                                      //161202
//   stress_factor_stats.append(1.0);                                              //200529_161202
*/
   if (after_dormancy)
   {  float64 instantaneous_root_restoration_after_dormancy
         = crop.roots_vital->get_biomass() * root_concentration_vital;           //070312
      root_N_mass_vital += instantaneous_root_restoration_after_dormancy;        //070312
   }
   else seedling_N = seedling_canopy_N_mass + root_N_mass_vital;                 //960802
   float64 start_or_restart_N_change=seedling_canopy_N_mass + root_N_mass_vital;
   if (mass_balance)
       mass_balance->initial_seed_or_restart += start_or_restart_N_change;       //070220
}
//_restart_____________________________________________________________________/
bool Crop_nitrogen_common::start_season()
{  seasonal.clear();
   before_change = true;                                                         //120408
   after_start_of_maturity = false;
   stress_factor_stats.clear(1.0);                                               //200529
/*200529
   stress_factor_stats.reset();  // was in start_day                             //190707
   stress_factor_stats.ref_current() = 1.0;                                      //161202
//   stress_factor_stats.append(1.0);                                              //200529_161202
*/
   return true;
}
//_start_season_____________________________________________________2006-12-14_/
bool Crop_nitrogen_common::start_day()
{  daily.clear();                                                                //061213
   delete N_fate_today; N_fate_today = 0;                                        //040625
   const Phenology &phenology = crop.ref_phenology();                            //181111
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   Normal_crop_event_sequence growth_stage=phenology.get_growth_stage_sequence();//130904
   if ((growth_stage>=FGS_BUD_BREAK) && (growth_stage<NGS_QUIESCENCE))           //070202
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   if (phenology.is_in_growing_season())                                         //181107
   #endif
      find_concentration_range();                                                //070202
   daily.gaseous_loss = 0.0;                                                     //011219
      // Make sure this is cleared because it won't be recalculated after maturity
   /*moved to start_season
   if (phenology.is_in_quiescence())
      stress_factor_stats.reset();  // Warning may need to move to start_day
   */
   max_N_uptake_per_unit_root_length =                                           //080722
         crop.roots_vital                                                        //100120
         ? calc_max_N_uptake_per_unit_root_length                                //080722
               (crop.roots_vital->get_total_root_lengths())
         : 0.0;                                                                  //100120
   return true;
}
//_start_day________________________________________________________2004-02-05_/
bool Crop_nitrogen_common::end_day()
{  if (mass_balance)                                                             //070220
   {  mass_balance->current_canopy_vital  = canopy_N_mass_vital;                 //070220
      mass_balance->current_canopy_effete = canopy_N_mass_effete;                //070326
      mass_balance->current_root_vital    = root_N_mass_vital;                   //070220
      mass_balance->current_root_effete   = root_N_mass_effete;                  //070220
   }
   seasonal.accumulate(daily);                                                   //061213
   reported_uptake_accum =                                                       //160628_120516
      (  get_mass_in_plant()
       - get_mass_of_seedling()
       + seasonal.gaseous_loss
       + get_mass_removed_seasonal());
   reported_canopy_mass_current = canopy_N_mass_vital+canopy_N_mass_effete;      //160628
   reported_root_conc = get_root_concentration(include_vital | include_effete);  //160629
//if (stress_factor_stats.get_current() < 1.0)
//std::clog << "N stress:" << stress_factor_stats.get_current() << std::endl;//161207
   return true;
}
//_end_day__________________________________________________________2004-02-05_/
void Crop_nitrogen_common::start_dormancy()
{  canopy_N_mass_effete += canopy_N_mass_vital;
   canopy_N_mass_vital = 0;
   root_N_mass_effete += root_N_mass_vital;
   root_N_mass_vital = 0;
   // Warning assuming the effete mass from previous seasons at dormancy is essentially 0
   // Otherwise it would be necessary to some how merge the concentrations.
   // I think each canopy portion should have the concentration of the portion.
   canopy_concentration_effete = canopy_concentration_vital;
   root_concentration_effete = root_concentration_vital;
   // After dormancy it is assumed the canopy concentration effete does not change.
}
//_start_dormancy___________________________________________________2007-03-27_/
float64 Crop_nitrogen_common::calc_leaf_stress_factor()             calculation_
//200529 (Nitrogen_leaf_stress_mode calc_mode)                               calculation_
{  // This was a test and deemed unnecessary, it could be deleted
   float64 low_N_conc = N_min_concentration;
   float64 high_N_conc= N_crit_concentration;
/* //200528
   switch (calc_mode)
   {  case no_leaf_stress : return 1.0; //break;
      case minimum_to_critical_conc :
      {
         low_N_conc =  N_min_concentration;
         high_N_conc = N_crit_concentration;
      } break;
      case straddle_critical_conc :
      {  low_N_conc = (N_crit_concentration + N_min_concentration) / 2.0;
         high_N_conc =(N_max_concentration + N_crit_concentration) / 2.0;
      } break;
   }
*/
   float64 denominator = (high_N_conc - low_N_conc);
   float64 numerator = (canopy_concentration_vital - low_N_conc);
   float64 leaf_stress_factor =
          (CORN::is_zero<float64>(denominator) || (numerator < 0.00001))
          ? 0.001 //200528 1.0
          : pow((numerator / denominator)
               ,parameters.leaf_expansion_sensitivity);                          //200528
   leaf_stress_factor=CORN::must_be_between<float64>(leaf_stress_factor,0.0,1.0);//070326
   return leaf_stress_factor;                                                    //041206
}
//_calc_leaf_stress_factor__________________________________________2007-02-24_/
float64 Crop_nitrogen_common::calc_max_N_uptake_per_unit_root_length
(float64 total_root_lengths)                                               const //080723
//  in VB version this is named MaximumNitrogenUptakePerUnitRootLength
{  // This is called one time in the setup of the crop nitrogen class.
   float64 max_uptake_daily_rate_kg_m2 =  per_ha_to_per_m2(crop.parameters->nitrogen.max_uptake_daily_rate_kg_ha);
   float64 _max_N_uptake_per_unit_root_length  // kg / m root / day   (return value)
      = max_uptake_daily_rate_kg_m2 / total_root_lengths;
   return _max_N_uptake_per_unit_root_length;   // kg/ha
}
//_calc_max_N_uptake_per_unit_root_length___________________________2006-11-29_/
float64 Crop_nitrogen_common::find_pot_uptake
(soil_sublayer_array_64(N_NO3_pot_uptake)  //returned (kg N) / (m^2 soil) elemental
,soil_sublayer_array_64(N_NH4_pot_uptake)  //returned (kg N) / (m^2 soil) elemental
,float64 root_activity)                                                    const
{  // This was the original potential N uptake                                   //061129
   // It used N availability, residual N parameter an water availability
   Soil::Soil_interface                  &soil      = *soil_properties;                //050721
   Soil::Nitrogen/*181206 Soil_nitrogen_interface*/ &chemicals = *soil_chemicals;                         //050721
   //170217unused static const float64 cmRoot_cm3Soil_to_mRoot_m2Soil  = 10000.0;               //070220
   float64 pot_N_uptake = 0.0;   // kg N / m^2 soil
   short int soil_rooting_sublayer = 2;
   float64 adjustment_layer_sum = 0.0;
   for (nat8 layer = soil_rooting_sublayer; layer <= soil.ref_layers()->count(); layer ++) //layer_number-1} DO
   {  float64 plant_avail_water        = soil.ref_hydrology()->get_plant_avail_water(layer);       //160412
      float64 water_availability_factor= 1.0 - exp(- water_availability_coef * plant_avail_water); //070209
      float64 layer_thickness          = soil.ref_layers()->get_thickness_m(layer); //m
      float64 layer_bulk_density       = soil.ref_structure()->get_bulk_density_g_cm3(layer);      //070725x
      float64 residual_N_conc_ppm      = parameters.residual_N_not_available_for_uptake;           //070215
      float64 root_lengths             = crop.roots_vital->get_lengths(layer); // m root/ m^2 soil    in the layer   //070131
      float64 residue_N_mass_kg_m2     = ppm_to_kg_m2(residual_N_conc_ppm,layer_thickness, layer_bulk_density);      //070216
      float64 soil_NO3_N_mass_layer = chemicals.get_NO3_N_mass_kg_m2(layer);                       //060714
      float64 NO3_N_conc_ppm           = kg_m2_to_ppm(soil_NO3_N_mass_layer,layer_thickness, layer_bulk_density); // ppm ( mg N)/ (kg soil)
      //std::cout<<"Layer:"<<(int)layer<<"\tplant_avil_w:"<<plant_avail_water<<std::endl;
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake
   << today_for_debug_output << '\t'
   << (int)layer << '\t'<<  max_N_uptake_per_unit_root_length << '\t'<< root_lengths  << '\t' << root_activity << '\t'
   << residue_N_mass_kg_m2 << '\t'<< residual_N_conc_ppm << '\t'
   << soil_NO3_N_mass_layer<< '\t'<< NO3_N_conc_ppm<< '\t';
#endif
      float64 soil_NH4_N_mass_layer = chemicals.get_NH4_N_mass_kg_m2(layer);     //070216
      float64 soil_N_mass_layer =(soil_NO3_N_mass_layer + soil_NH4_N_mass_layer);//070220
/*
if ((soil_N_mass_layer < 0.000001) && (stress_factor_stats.get_current() < 1.0))
std::clog <<"sNmass " << ((int)layer) << " :" << soil_N_mass_layer << std::endl;
*/

      if (soil_N_mass_layer > 0.0)                                               //070220
      {  float64 NO3_max_N_uptake_per_unit_root_length                           //070216
            = max_N_uptake_per_unit_root_length
               * (soil_NO3_N_mass_layer/ soil_N_mass_layer);
         if (NO3_N_conc_ppm > residual_N_conc_ppm)                               //070215
         {
         float64 NO3_availability_factor =                                       //070209
            CORN::must_be_0_or_greater<float64>
            (1.0 - exp(-(NO3_N_conc_ppm  - residual_N_conc_ppm)
                       * NO3_N_availability_adj));                               //070209
         N_NO3_pot_uptake[layer] = root_lengths * NO3_availability_factor  * water_availability_factor   //070207
               * NO3_max_N_uptake_per_unit_root_length                           //070214
               * root_activity;
         float64 soil_NO3_N_available = (soil_NO3_N_mass_layer - residue_N_mass_kg_m2);
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake   << NO3_availability_factor<< '\t' << N_NO3_pot_uptake[layer]<< '\t' << soil_NO3_N_available<< '\t';
#endif
         if (N_NO3_pot_uptake[layer] > soil_NO3_N_available)                     //070216
             N_NO3_pot_uptake[layer] = soil_NO3_N_available;                     //070216
         // else there is enough available to meet potential uptake              //070216
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake   << N_NO3_pot_uptake[layer]<< '\t' ;
#endif
         } else // No pot NO3 uptake                                             //070215
         {  N_NO3_pot_uptake[layer] = 0.0;
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake  << "N/A" << '\t'  <<N_NO3_pot_uptake[layer]<< '\t' << "N/A" << '\t' << "N/A" << '\t';
#endif
         }
         // Now deduct from NH4
         float64 NH4_N_conc_ppm = kg_m2_to_ppm(soil_NH4_N_mass_layer,layer_thickness , layer_bulk_density); // ppm ( mg N)/ (kg soil)

#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake   <<  soil_NH4_N_mass_layer << '\t' << NH4_N_conc_ppm << '\t';
#endif

         float64 NH4_max_N_uptake_per_unit_root_length
            = max_N_uptake_per_unit_root_length
             * (soil_NH4_N_mass_layer/ soil_N_mass_layer);                       //070216
         if (NH4_N_conc_ppm > 0.0)                                               //070215
         {  float64 NH4_availability_factor =                                    //070209
               CORN::must_be_0_or_greater<float64>                               //151109
                  (1.0 - exp(-(NH4_N_conc_ppm) * NH4_N_availability_adj));       //070215_070209
            N_NH4_pot_uptake[layer] = root_lengths                               //070207
               * NH4_availability_factor * water_availability_factor
               * NH4_max_N_uptake_per_unit_root_length                           //070214
               * root_activity;
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake   << NH4_availability_factor<< '\t' << N_NH4_pot_uptake[layer]<< '\t';
#endif
            if (N_NH4_pot_uptake[layer] > soil_NH4_N_mass_layer)                 //981211
               N_NH4_pot_uptake[layer] = soil_NH4_N_mass_layer;                  //981211
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake   << N_NH4_pot_uptake[layer]<< '\t' ;
#endif
         } else // No pot NO3 uptake                                             //070215
         {  N_NH4_pot_uptake[layer] = 0.0;
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake  << "N/A"<< '\t'  << N_NH4_pot_uptake[layer]<< '\t'<< N_NH4_pot_uptake[layer]<< '\t';
#endif
         }
         pot_N_uptake += N_NO3_pot_uptake[layer] + N_NH4_pot_uptake[layer];
#ifdef DEBUG_N_POT_UPTAKE
debug_N_pot_uptake    << water_availability_factor << '\t';
debug_N_pot_uptake  << NO3_max_N_uptake_per_unit_root_length<< '\t'  << NH4_max_N_uptake_per_unit_root_length<< '\t' ;
debug_N_pot_uptake     << endl;
#endif
         float64 adjustment_layer =  water_availability_factor  * soil_N_mass_layer;
         adjustment_layer_sum += adjustment_layer;
      }                                                                          //070220
   }
   /*//cout << "Root lengths:" << sum_root_lengths << endl;
   cout << "max_n_uptake_unit:" << max_N_uptake_per_unit_root_length << endl;
   cout << "pot_N_uptake:" << pot_N_uptake << endl;*/
   return pot_N_uptake;
}
//_find_pot_uptake_____________________________________________________________/
void Crop_nitrogen_common::find_concentration_range()
{  // As implemented in V.B. version as of Dec.12,2006
   // This should be done every day early in the processes.
   // This is because there can still be gaseous loss even after growth.         //070202
   float64 accum_deg_days_clipping_adj =
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
      crop.thermal_time->get_accum_degree_days(/*200211 now default true*/);   //130820 confirmed adjusted
      #endif
      #if ((PHENOLOGY_VERSION==2018))
         accum_deg_days_adjusted_for_clipping;                                   //181118
      #endif
   if (
      start_change_today ||
      before_change)                                                             //070126_121221
   {  // was based on flowering now based on Senescence  but will continue to use flowering until crop parameter file is upgraded
      start_change_today = false;
      if (parameters.end_dilution_curve_deg_day  // 0 indicates no end           //130408
          && (crop.get_thermal_time_cumulative()
              >= parameters.end_dilution_curve_deg_day))
      {  start_change_today = true;
         before_change= false;
      }
      compute_dilution();                                                        //121221
      if (start_change_today)                                                    //070126_121221(was start_flowering_today)
      {
         N_max_conc_at_change = N_max_concentration;
         N_crit_conc_at_change = N_crit_concentration;
         N_min_conc_at_change = N_min_concentration;
         // Now correct the target N concentratons at maturity with respect to
         // the adjusted slope.
         // These corrections are between 0 and 1.                               //070129
         N_max_conc_at_maturity               = parameters.dilution_curve_max_conc_mature;
         N_crit_conc_at_maturity              = parameters.calc_N_mature_crit(photosynthetic_pathway);
         N_min_conc_at_maturity               = parameters.calc_N_mature_min(photosynthetic_pathway);
         start_change_today = false;                                             //121221_070126
      } // after_start_of_change
   } else
   if (crop.parameters->phenology.maturity_significant)                          //080319
   {  // 'at change'
      float64 maturity_degree_days = crop.parameters->phenology.initiation.maturity;
      float64 begin_change_deg_day =
         #if (CROPSYST_VERSION == 4)
         (parameters.dilution_curve_based_on_max_conc)                           //121221
         ? crop.parameters->phenology.initiation.flowering:
         #endif
         crop.parameters->nitrogen.end_dilution_curve_deg_day;                   //130408

      // This limitation may be changed so that when the value at maturity
      // is reached then these reference concentations donot change
      N_max_concentration =
         (N_max_conc_at_change > N_max_conc_at_maturity)                         //110915
         ?  N_max_conc_at_change-(N_max_conc_at_change-N_max_conc_at_maturity)
            * (1.0 - (maturity_degree_days - accum_deg_days_clipping_adj)
                    /(maturity_degree_days - begin_change_deg_day))
         : N_max_conc_at_change  ;                                               //110915
      N_crit_concentration =
         (N_crit_conc_at_change > N_crit_conc_at_maturity)                       //110915
         ? N_crit_conc_at_change-(N_crit_conc_at_change-N_crit_conc_at_maturity)
            * (1.0 - (maturity_degree_days - accum_deg_days_clipping_adj)
                   / (maturity_degree_days - begin_change_deg_day))
         : N_crit_conc_at_change;                                                //110915
      N_min_concentration  =
         (N_min_conc_at_change > N_min_conc_at_maturity)
         ? N_min_conc_at_change - (N_min_conc_at_change-N_min_conc_at_maturity)
         * (1.0 - (maturity_degree_days - accum_deg_days_clipping_adj)
                 /(maturity_degree_days - begin_change_deg_day))
         : N_min_conc_at_change;
   } else                                                                        //121221
   {  //continue with dilution curve after scenesce
      // this is the same as above, make a function.
      // float64 pot_cumulative_top_biomass_ton_ha =
      compute_dilution();                                                        //121221
      //note that the  returned pot_cumulative_top_biomass_ton_ha not used in this case.
   }
}
//_find_concentration_range_________________________________________2006-12-12_/
void /*180822 float64*/ Crop_nitrogen_common::compute_dilution()    computation_
{  float64 pot_cumulative_top_biomass_ton_ha = kg_m2_to_tonne_ha(crop.get_canopy_biomass_kg_m2_including(include_vital));
   if (pot_cumulative_top_biomass_ton_ha)                                        //180822
   {  // the if is a hack, the canopy_curve model is giving AGB biomass 0 on the first day
      // it probably should have an initial biomass as will the LAI_canopy model
      // should discuss with Claudio                                             //180822
   N_max_concentration  = std::min<float64>(N_max_conc_at_emergence , N_max_conc_at_emergence  * pow(pot_cumulative_top_biomass_ton_ha,concentration_slope));
   N_crit_concentration = std::min<float64>(N_crit_conc_at_emergence, N_crit_conc_at_emergence * pow(pot_cumulative_top_biomass_ton_ha,concentration_slope));
   N_min_concentration  = std::min<float64>(N_min_conc_at_emergence , N_min_conc_at_emergence  * pow(pot_cumulative_top_biomass_ton_ha,concentration_slope));
   }
//180822    return pot_cumulative_top_biomass_ton_ha;
}
//_compute_dilution_________________________________________________2012-12-21_/
float64 Crop_nitrogen_common::find_plant_demand
(float64 daily_crop_top_growth_pot  // today's production   water stress and radiation stress affected
,float64 daily_crop_root_growth_pot // currently this is actually yesterday's actual root growth,
,float64 FND                        // this is acceptable because generally root growth is satisfied.
,float64 &canopy_N_demand           // Only very rare conditions is potential root growth not met because of difficiency.
,float64 &root_N_demand)
{  //  Note that the nitrogen model is concern only with new vital growth        //070326
   float64 top_N_deficiency_demand;
   float64 canopy_biomass = crop.get_canopy_biomass_kg_m2_including(include_vital);//070321
    // Nitrogen required for potential growth }
   float64 root_N_max_conc = parameters.root_conc;                               //070131
   float64 N_pot_root_growth = root_N_max_conc * daily_crop_root_growth_pot;     //011016
      //  N demand for root growth
   float64 root_biomass = crop.roots_current ? crop.roots_current->get_biomass() : 0.0; //190102
   float64 root_biomass_before_growth // <- biomass before new growth
      =  (root_biomass - daily_crop_root_growth_pot);
   root_concentration_vital =  !CORN::is_zero(root_biomass_before_growth) ? (root_N_mass_vital / root_biomass_before_growth) : 0.0;
   float64 top_N_pot_growth = N_max_concentration * daily_crop_top_growth_pot;
      // N needed for potential top growth
    float64 N_grain = 0.0;
      // Nitrogen in grain should already by included in the total above ground biomas
    float64 top_N_new_growth =  top_N_pot_growth + N_grain;
    // roots always have the same root_concentration (dont accumulate excess)    //040623
    top_N_deficiency_demand = CORN::must_be_0_or_greater<float64>(               //151109
       (N_max_concentration - canopy_concentration_vital) *                      //070326
       (canopy_biomass - crop.unclipped_biomass));                               //010328
    canopy_N_demand = top_N_deficiency_demand + top_N_new_growth;
    float64 root_N_deficiency_demand = CORN::must_be_0_or_greater<float64>(
       (root_N_max_conc - root_concentration_vital) *
       (root_biomass_before_growth ));                                           //010328
    root_N_demand = root_N_deficiency_demand + N_pot_root_growth;
    // Note that Fract N Demand is 0.0 if non-legume
    float64 plant_N_demand = canopy_N_demand + root_N_demand;                    //070131
    if (FND > 0.0) // (crop.parameters->nitrogen->legume_fraction)               //070131
    {                                                                            //070131
      float64 daily_fixation = (FND > 0.0)                                       //070131
      ? std::min<float64>(plant_N_demand* FND * crop.parameters->nitrogen.legume_fraction, 0.0006)    //070131
         // legume_fraction of crop mix is a special parameter for the CFF (temporary)
         // for pasture crops that are a mixture of legume and gremilia.
      : 0.0;                                                                     //070131
      daily.fixation = daily_fixation;                                           //070220
      float64 fixation_to_partition = daily_fixation;                            //070131
         // Must pass a copy of the value to allocate_available_to_plant_part_as_demanded!
      // Roots are given priority                                                //070131
           allocate_available_to_plant_part_as_demanded                          //070131
            (fixation_to_partition,root_N_mass_vital,root_N_demand);
      if (!allocate_available_to_plant_part_as_demanded                          //070131
            (fixation_to_partition,canopy_N_mass_vital,canopy_N_demand))
         std::cerr << std::endl                                                  //070131
            << "Not all fixed N was allocated to plant tissues. "
            "May be a programming error in find_plant_demand()" << std::endl;
      // At this point fixation_to_partition must be 0!
      plant_N_demand = canopy_N_demand + root_N_demand;                          //070131
      if (mass_balance) mass_balance->fixation += daily_fixation;                //070220
    }
    // N_stored is subtracted from uptake and not from canopy_N_demand }
   if (store_persistent > plant_N_demand)                                        //160409
   {  store_persistent -= plant_N_demand;
      plant_N_demand = 0;
   } else
   {  plant_N_demand -=  store_persistent;
      store_persistent = 0;
   }
   return plant_N_demand;                                                        //070131
}
//_find_plant_demand___________________________________________________________/
float64 Crop_nitrogen_common::find_act_uptake
(float64 pot_N_uptake
,float64 act_plant_N_demand
,float64 canopy_N_demand                                                         //070131
,float64 root_N_demand                                                           //070131
,soil_sublayer_array_64(N_NO3_pot_uptake)
,soil_sublayer_array_64(N_NH4_pot_uptake)
,float64 &N_uptake)
{  float64 act_N_uptake = 0.0;                                                   //061205
   if (   CORN::is_zero<float64>(pot_N_uptake/*,0.0*/,0.000000001)          //160411
       || CORN::is_zero<float64>(act_plant_N_demand,/*0.0,*/0.000000001)    //160411_150814
      ) return 0.0;
   short int sublayer ;
   float64 profile_N_NO3_uptake  = 0.0; //kg N /m2 soil
   float64 profile_N_NH4_uptake  = 0.0; //kg N /m2 soil
   N_uptake = act_plant_N_demand;     //rename to expected_plant_N_demandp
   //{Remove it from the soil }
   Soil::Soil_interface &soil      = *soil_properties;                           //181206_050721
   Soil::Nitrogen       &chemicals = *soil_chemicals;                            //181206_050721
   float64 act_pot_uptake_ratio =  act_plant_N_demand / pot_N_uptake;
   for (sublayer = 2; sublayer <= soil.ref_layers()->count(); sublayer++)
         // layer 2 is currently always to rooting sublayer
       if (pot_N_uptake > 0.0)
       {
            float64 N_NO3_uptake_sublayer  //kg N/m2 soil
            = N_NO3_pot_uptake[sublayer] *act_pot_uptake_ratio ;
            profile_N_NO3_uptake += chemicals.uptake_N_mass_from_NO3(sublayer,N_NO3_uptake_sublayer);
            float64 N_NH4_uptake_sublayer //kg N /m2 soil
            =  N_NH4_pot_uptake[sublayer] * act_pot_uptake_ratio;
            profile_N_NH4_uptake += chemicals.uptake_N_mass_from_NH4(sublayer,N_NH4_uptake_sublayer);
      }
   chemicals.commit_diffs();                                                     //120730
   if (mass_balance) { mass_balance->NO3_N_uptake+=profile_N_NO3_uptake; mass_balance->NH4_N_uptake+=profile_N_NH4_uptake; } //070220
   act_N_uptake = profile_N_NH4_uptake + profile_N_NO3_uptake;                   //061205
     // Satisfy root demand first

//act_N_uptake_cum_debug += act_N_uptake;

   // At this point act_N_uptake_to_partition must be 0!
   return act_N_uptake;
}
//_find_act_uptake_____________________________________________________________/
float64 Crop_nitrogen_common::find_actual_growth  // rename to calc_actual_growth
(float64 attainable_growth)        //attainable_growth_transp_or_RUE
{
   float64 act_growth = 0.0;                                                     //011016
   if (N_crit_concentration != N_min_concentration)   // avoid div by 0
   {
      float64 N_reduction_factor
      = 1.0  - (N_crit_concentration - canopy_concentration_vital)               //070326
        / (N_crit_concentration - N_min_concentration);
      N_reduction_factor = CORN::must_be_between<float64>(N_reduction_factor ,0.0,1.0);   //011117
      float64 nitrogen_limitation                                                //011016
      = (canopy_concentration_vital >= N_crit_concentration)                     //070326
      ? 1.0                                                                      //011016
      : N_reduction_factor;                                                      //011117
      act_growth = attainable_growth * nitrogen_limitation;                      //011016
   }
#ifndef _Windows
   else
       std::cerr << "Division by zero in find_actual_growth. This should never occur" << std::endl;
#endif

   return act_growth;   // at this point in V5 this would be nitrogen limited growth
}
//_calc_actual_growth__________________________________________________________/
float64 Crop_nitrogen_common::nitrogen_depend_growth
(float64 root_activity
,float64 daily_crop_root_mass
,float64 daily_crop_root_growth_pot
,float64 attainable_growth_transp_or_RUE            //<- mustn't be 0!           //011117
,float64 FND)
{
   float64 canopy_N_mass_vital_yesterday = canopy_N_mass_vital;                  //160411
   float64 stored_today = 0;                                                     //160411
   float64 N_uptake ;        // (kg N) / (m^2 soil)
   soil_layer_array64(N_NO3_pot_uptake); clear_layer_array64(N_NO3_pot_uptake); // (kg N) / (m^2 soil) elemental
   soil_layer_array64(N_NH4_pot_uptake); clear_layer_array64(N_NH4_pot_uptake); // (kg N) / (m^2 soil) elemental
   // N demand units: (kg N) / (m^2 soil)                                        //070131
   float64 canopy_N_demand  = 0;
   float64 root_N_demand = 0;                                                    //070131
   float64 plant_N_demand = find_plant_demand
      (attainable_growth_transp_or_RUE                                           //151031_011117
      ,daily_crop_root_growth_pot
      ,FND
      ,canopy_N_demand   // returned
      ,root_N_demand);   // returned                                             //070131
   float64 pot_uptake =
      find_pot_uptake(N_NO3_pot_uptake,N_NH4_pot_uptake,root_activity);          //061129
   float64 canopy_biomass = crop.get_canopy_biomass_kg_m2_including(include_vital);//020719
      // yesterday's biomass
   float64 new_biomass = (canopy_biomass + attainable_growth_transp_or_RUE);
   float64 expected_N_uptake = std::min<float64>(plant_N_demand,pot_uptake);     //160409
   float64 unable_to_meet_N_root_demand = (expected_N_uptake - root_N_demand); //need better name
   if (unable_to_meet_N_root_demand < 0 )
       unable_to_meet_N_root_demand = 0;
   float64 canopy_N_conc_expected =                                              //160411
      (canopy_N_mass_vital + unable_to_meet_N_root_demand) / new_biomass;
   if (canopy_N_conc_expected < 0)
   std::cout << "Negative canopy concentration. This may occur if crop parameter 'N demand adjustment is negative'." << std::endl;

   // if cannot sustain potential new growth  reduce conc. to below critical line

   root_N_mass_vital += root_N_demand;                                           //160411
   float64 plant_N_required = 0.0;
   if (canopy_N_conc_expected < N_crit_concentration)                            //070326
   {
      if (automatic_nitrogen_to_plant)
      {  // N needed to make canopy N concentration = critical concentration after new growth is met kg/m2
          plant_N_required
            = new_biomass * (N_crit_concentration - canopy_N_conc_expected);
         daily.fertilization_requirements = plant_N_required;
         canopy_N_mass_vital = new_biomass * N_crit_concentration;
         canopy_concentration_vital = N_crit_concentration;
      } else
      {  canopy_concentration_vital = canopy_N_conc_expected;
         canopy_N_mass_vital = new_biomass * canopy_concentration_vital;
      }
   } else
   if (canopy_N_conc_expected > N_max_concentration + 0.000000001)
      {  float64 expected_over_max_conc =(canopy_N_conc_expected - N_max_concentration);
         stored_today = new_biomass * expected_over_max_conc;
         store_persistent += stored_today;
         canopy_N_mass_vital = new_biomass * N_max_concentration;
         canopy_concentration_vital = N_max_concentration;
      } else
      {  canopy_concentration_vital = canopy_N_conc_expected;
         canopy_N_mass_vital = new_biomass * canopy_concentration_vital;
         // canopy_N_mass_vital has already been updated in find_act_uptake
         // and no adjustment is done here.
      }
   float64 act_plant_N_demand = canopy_N_mass_vital - canopy_N_mass_vital_yesterday + root_N_demand -  plant_N_required ;
   daily.uptake                                                                  //070209
       = find_act_uptake
      (pot_uptake                                                                //061205
      ,act_plant_N_demand
      ,canopy_N_demand                                                           //070131
      ,root_N_demand                                                             //070131
      ,N_NO3_pot_uptake
      ,N_NH4_pot_uptake
      ,N_uptake);
#ifdef DEBUG_N_CONC
debug_N_conc << "GDD=" << crop.get_thermal_time_cumulative(/*200211obs false*/) << " BM=" << crop.get_canopy_biomass_kg_m2_including(include_vital) << " "  << "Nconcentration: curr=" << canopy_concentration_vital << " min="  << N_min_concentration << " max="<< N_max_concentration << " crit=" <<N_crit_concentration << endl;
#endif
   N_limited_growth = find_actual_growth                                         //151105
      (attainable_growth_transp_or_RUE);                                         //151105
   return N_limited_growth;
}
//_nitrogen_depend_growth______________________________________________________/
float64 Crop_nitrogen_common::process_any_gaseous_losses()         modification_
{  float64 gaseous_loss_act = 0.0;
   float64 canopy_biomass=crop.get_canopy_biomass_kg_m2_including(include_vital);//020719
   float64 max_conc_for_gaseous_loss = N_max_concentration;
   if (canopy_concentration_vital > max_conc_for_gaseous_loss)
   {  float64 gaseous_loss_pot                                                   //130628
         =canopy_biomass*(canopy_concentration_vital-max_conc_for_gaseous_loss);
      gaseous_loss_act = gaseous_loss_pot * 0.1;                                 //130628
      gaseous_loss_act = CORN::must_be_0_or_greater<float64>(gaseous_loss_act);  //130628_070412
      if (gaseous_loss_act > canopy_N_mass_vital)                                //130628_070412
         gaseous_loss_act = 0.0;                                                 //130628_070412
      canopy_N_mass_vital -= gaseous_loss_act;                                   //130628_011218
      canopy_concentration_vital = canopy_N_mass_vital/ canopy_biomass;
      if (mass_balance) mass_balance->gaseous_loss += gaseous_loss_act;          //130628_070220
   } // else default to 0.0      070328  else  daily.gaseous_loss = 0.0;         //011216
   daily.gaseous_loss = gaseous_loss_act;                                        //130628_070220
   return gaseous_loss_act;                                                      //130628
}
//_process_any_gaseous_losses_______________________________________2007-02-02_/
float64 Crop_nitrogen_common::Fract_N_demand()
{
   Soil::Soil_interface &soil      = *soil_properties;                           //181206_050721
   Soil::Nitrogen       &chemicals = *soil_chemicals;                            //181206_050721
   float64 depth                ;    // m
   float64 soil_temp_2 = soil.get_temperature_C(2);
   if (crop.parameters->nitrogen.fixating)                                       //990227
   {
      float64 Nfix_crop_dev_effect = 1.0;    // 0-1
      float64 accum_degree_days =
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         crop.thermal_time->get_accum_degree_days(/*200211default true*/);       //040830_130820
      #endif
      #if ((PHENOLOGY_VERSION==2018))
         accum_deg_days_adjusted_for_clipping;                                   //181118
      #endif
// 030521  WARNING:  need to confirm with Claudio not sure if to use conditions if not perennial or if has_leaf_senescence
      if (crop.parameters->is_perennial())                                       //980723
         Nfix_crop_dev_effect = 1.0;                                             //980723
      else                                                                       //980723
      {
         const Phenology &phenology = crop.ref_phenology();                      //181111
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
     switch (phenology.get_growth_stage_sequence())                              //130904
      { case NGS_ACTIVE_GROWTH:
       Nfix_crop_dev_effect = accum_degree_days / crop.parameters->phenology.initiation.flowering;
      break;
      case NGS_ANTHESIS:
         Nfix_crop_dev_effect = 1.0;
      break;
      case NGS_FILLING: case FGS_VERAISON : case FGS_RAPID_FRUIT_DEVELOPMENT :
      // WARNING: Claudio needs to confirm to do this for fruit crops            //130902_030521
      //case FGS_INFRUCTESCENCE :
      {  float64 param_deg_day_begin_grain_filling = crop.parameters->phenology.initiation.filling;   //020809
         float64 param_begin_maturity_deg_day      = crop.parameters->phenology.initiation.maturity;  //020629
         float64 deg_day_end_fix = (param_begin_maturity_deg_day + param_deg_day_begin_grain_filling)/2.0;
         Nfix_crop_dev_effect = (accum_degree_days <= deg_day_end_fix)
         ? (1.0 - (accum_degree_days - param_deg_day_begin_grain_filling) /
                  (deg_day_end_fix   - param_deg_day_begin_grain_filling))
         : 0.0;
       }break;
       default: break;                                                           //170216
      }
      #endif
      #if ((PHENOLOGY_VERSION==2018))
      if (phenology.is_in_accrescence())
         Nfix_crop_dev_effect = accum_degree_days / crop.parameters->phenology.initiation.flowering;
      else if (phenology.is_in_anthesis())
         Nfix_crop_dev_effect = 1.0;
      else if (phenology.is_in_yield_formation()) // includes filling through but not including maturity
         // including veraison and rapid_fruit_growth
      // WARNING: Claudio needs to confirm to do this for fruit crops   //130902_030521
      //case FGS_INFRUCTESCENCE :
      {
         const Phenology::Period_thermal *filling_period
            = phenology.get_yield_formation_period(); //181111
         if (filling_period)
         {  // culmination is filling end was maturity
         float64 deg_day_end_fix /*half_duration*/ = filling_period->get_param_duration_GDDs()/2.0; //181111
         float64 thermal_time_accum = filling_period->get_thermal_time_accum();  //181111
         Nfix_crop_dev_effect = (accum_degree_days <= deg_day_end_fix)           //181111
         ? (1.0 - (accum_degree_days) / (deg_day_end_fix  ))                     //181111
         : 0.0;
       }
       // else not applicable
      }
      #endif
   }
    int count_PAW_50cm = 0;
    float64 sum_PAW_50cm = 0.0;  // 0-1
    short int sublayer = 1;
    depth = soil.ref_layers()->get_thickness_m(sublayer);
    while (depth < 0.5 /*m*/ && (sublayer <= soil.ref_layers()->count())) // was 0.3   //040217
    { if (sublayer != 1) // Exclude the evaporative layer                        //980723
      {  count_PAW_50cm += 1;                                                    //980723
         sum_PAW_50cm += soil.ref_hydrology()->get_plant_avail_water(sublayer);  //160412
      }
      depth += soil.ref_layers()->get_thickness_m(sublayer);
      sublayer += 1;                                                             //980723
    }
    float64 avg_PAW_50cm = count_PAW_50cm > 0 ? sum_PAW_50cm / count_PAW_50cm : 0.0;
    float64 Nfix_water_effect      // 0-1    PAW threshold for N fixation 0.3
    = std::min<float64>(1.0,std::max<float64>(0.0,((avg_PAW_50cm - 0.3) / 0.3)));//980727
    float64 N_index  = 0.0;             // kg N/m2
    for (sublayer = 1; sublayer <=  soil.ref_layers()->count(); sublayer ++)
    { float64 root_length_layer_fract = crop.get_total_fract_root_length(sublayer); //061208
      if (root_length_layer_fract  > 0.0)
          N_index +=                                                             //010322
                (root_length_layer_fract
                 /soil.ref_layers()->get_thickness_m(sublayer)) *
                ( chemicals.get_NO3_N_mass_kg_m2(sublayer)
                 +chemicals.get_NH4_N_mass_kg_m2(sublayer));
    }
    float64 Nfix_nitro_effect        // 0-1
    = (N_index > 0.03)       // 300 kg N/ha
    ? 0.0
    : (N_index < 0.01)  // 100 kg N/ha
      ? 1.0
      : (1.0 - ((N_index -  0.01)/0.02));
    float64 Nfix_temp_effect         // 0-1
    = (soil_temp_2 > 36)
    ? 0.7
    : (soil_temp_2 > 30)
      ? 1.0
      : (soil_temp_2 < 0)
        ?  0.0
        :  (soil_temp_2 / 30.0);  // 'C
    return Nfix_crop_dev_effect *
            std::min<float64>(std::min<float64>
               (Nfix_water_effect , Nfix_nitro_effect), Nfix_temp_effect);
 }
 else return  0.0;
}
//_Fract_N_demand________________________________________________________1998?_/
float64 Crop_nitrogen_common::update_limited_growth
(float64 attainable_growth // _TUE_or_RUE
,float64 root_activity_factor)                                     modification_
{
   N_limited_growth = attainable_growth;                                         //011016
   float64 accum_degree_days =
      #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
         crop.thermal_time->get_accum_degree_days(/*200211default true*/);       //040830_130820
      #endif
      #if ((PHENOLOGY_VERSION==2018))
         accum_deg_days_adjusted_for_clipping;                                   //181118
      #endif
   if ((attainable_growth > 0.0) &&                                              //011016
       (crop.parameters->is_perennial() ||
        (!crop.parameters->phenology.maturity_significant) ||                    //101115
         (crop.parameters->phenology.maturity_significant &&                     //080319
          (accum_degree_days < crop.parameters->phenology.initiation.maturity))))
   {     float64 FND = Fract_N_demand();                                         //020505
         float64 pot_root_biomass
            =  crop.roots_current ?                                              //190102
               crop.roots_current->calc_biomass(0)                               //011022
               : 0.0;                                                            //190102

         N_limited_growth = nitrogen_depend_growth                               //011016
              ( root_activity_factor
                ,pot_root_biomass                                                //010716
                ,crop.roots_vital->get_growth_biomass()                          //070130
               // Warning this will be the growth from yesterday
               // because we haven't yet updated biomass growth
                ,attainable_growth                                               //011216
                ,FND
                //already is member ,automatic_nitrogen
                );
   } else
      if (!crop.parameters->phenology.maturity_significant)                      //130430
      process_any_gaseous_losses();                                              //070202
      // Now N gaseous losses may continue throughout maturity (and dormancy)    //070202
      // Note that N gaseous loss during active                                  //070202
      // growth is handled by actual biomass growth
   return N_limited_growth;                                                     //151030
}
//_update_limited_growth____________________________________________2015-10-30_/
Crop_mass_fate *Crop_nitrogen_common::render_N_fate
(Biomass_fated_cause cause
,float64 canopy_before,float64 root_before)                           rendition_
{
   if (crop.roots_current)
   {
   //170216unused float64 root_biomass =
      crop.roots_current->get_biomass();                                         //070328
   }
   if (CORN::is_zero<float64>(canopy_before,0.00001))
      canopy_before = canopy_N_mass_vital + canopy_N_mass_effete;                //131005
   if (CORN::is_zero<float64>(root_before,0.00001))
      root_before = root_N_mass_vital;
   Crop_mass_fate *rendered_N_fate = new Crop_mass_fate
      (cause,canopy_before,root_before);                                         //070625
   return rendered_N_fate;
}
//_render_N_fate____________________________________________________2013-10-05_/
Crop_mass_fate *Crop_nitrogen_common::apportion_fate
(const Crop_mass_fate &biomass_fate_now
,Crop_mass_fate *N_fate_now     // should be a reference
,const Crop_biomass &canopy_biomass_removed                                      //120514
,bool plant_part_N_concentrations_specified
,nat8 roots_vital_or_effete)
{
   float64  root_concentration_vital_or_effete  =  (roots_vital_or_effete == include_vital) //070328
      ? root_concentration_vital                                                 //070328
      : root_concentration_effete;                                               //070328
   if (plant_part_N_concentrations_specified)                                    //070130
   {  // Stubble N concentration is fixed                                        //070130
      float64 act_residue_conc = get_residue_concentration();                    //070130
      float64 unharvested_above_ground_N_mass = 0;                               //070130
      unharvested_above_ground_N_mass += N_fate_now->useful                 =biomass_fate_now.useful               *act_residue_conc;     //070130
      // The grazing here is for the pasture model.
      unharvested_above_ground_N_mass += N_fate_now->grazing_to_manure      =biomass_fate_now.grazing_to_manure    *act_residue_conc;     //070130
      unharvested_above_ground_N_mass += N_fate_now->grazing_to_disposal    =biomass_fate_now.grazing_to_disposal  *act_residue_conc;     //070130
      unharvested_above_ground_N_mass += N_fate_now->disposal               =biomass_fate_now.disposal             *act_residue_conc;     //070130
      unharvested_above_ground_N_mass += N_fate_now->chaff_or_leaf_litter   =biomass_fate_now.chaff_or_leaf_litter *act_residue_conc;     //070130
      unharvested_above_ground_N_mass += N_fate_now->stubble_dead           =biomass_fate_now.stubble_dead         *act_residue_conc;     //070130
      unharvested_above_ground_N_mass += N_fate_now->uncut_canopy           =biomass_fate_now.uncut_canopy         *act_residue_conc;     //070130
      N_fate_now->yield = biomass_fate_now.yield * canopy_concentration_vital;
   }
   float64 N_roots_live = 0;                                                     //070130
   if (soil_properties)                                                          //070130
   for (nat8 layer = 0; layer <= soil_properties->ref_layers()->count();layer++) //070130
   {  // dead roots                                                              //070130
      float64 root_N_removed_from_layer = biomass_fate_now.roots_dead[layer]* root_concentration_vital_or_effete; //070130
      root_N_removed_from_layer = std::min<float64>(root_N_removed_from_layer,root_N_mass_vital); //070312
      // This shouldn't really happen but root concentration is not always in sync
      N_fate_now->roots_dead[layer] = root_N_removed_from_layer;                 //070130
      root_N_mass_vital -= root_N_removed_from_layer;                            //070130
      if (mass_balance) mass_balance->removed_root += root_N_removed_from_layer; //070220
      // live roots
      float64 root_N_remaining_in_layer = biomass_fate_now.roots_live[layer]* root_concentration_vital_or_effete; //070130
      N_fate_now->roots_live[layer] = root_N_remaining_in_layer;                 //070130
      N_roots_live += root_N_remaining_in_layer;                                 //070130
   }
   if (!N_fate_today) N_fate_today =new Crop_mass_fate(*N_fate_now);
   else N_fate_today->add(*N_fate_now);                                          //120514
   // Currently only removing top mass
/*200813
obsolete
#ifdef QND
QND_plant_N_grazed_kg_m2 += N_fate_now->grazing_to_manure;
QND_plant_N_ungrazed_kg_m2 += N_fate_now->grazing_to_disposal + N_fate_now->removed_for_disposal;
#endif
*/

   // We need now set set the biomass to the actual biomass
   // the unstressed growth resumes from this point

   N_fate_now->canopy_after =canopy_N_mass_vital + canopy_N_mass_effete;         //070130
   N_fate_now->root_after = root_N_mass_vital;                                   //070130
// Currently this is only an ancillary  output
#ifdef DEBUG_CROP_N_FATE
#ifdef _Windows
   if (!N_fate_headers_dumped)
   {  N_fate_now->dump_headers(crop_N_fate_txt_file,true,soil_properties->ref_layers()->count(),"N");
      crop_N_fate_txt_file << "\tN_gaseous_loss\t";
      biomass_fate_now.dump_headers(crop_N_fate_txt_file,true,soil_properties->ref_layers()->count(),"biomass");
      crop_N_fate_txt_file << endl;                                              //070201
      N_fate_headers_dumped = true;                                              //070130
   }                                                                             //070130
   N_fate_now->dump(crop_N_fate_txt_file,true);                                  //070130
   crop_N_fate_txt_file << "\t" << seasonal.gaseous_loss << '\t';                //070206
   biomass_fate_now.dump(crop_N_fate_txt_file,true);                             //070130
   crop_N_fate_txt_file << endl;                                                 //070201
#endif
#endif
   return N_fate_now;                                                            //070625
}
//_apportion_fate___________________________________________________2005-07-22_/
float64 Crop_nitrogen_common::calc_uptake_excess_or_deficit()              const
{  // This can be called at any time, but usually used at maturity
   float64 total_N_uptake = seasonal.uptake;
   float64 above_ground_biomass_curr= crop.get_canopy_biomass_kg_m2_including(include_vital);
   float64 root_biomass_curr        = crop.roots_current ? crop.roots_current->calc_biomass(0): 0;  //190102_070312
   float64 max_above_ground_N_mass  = above_ground_biomass_curr * N_max_conc_at_maturity;
   float64 max_root_N_mass          = root_biomass_curr         * parameters.root_conc;
   float64 uptake_excess_or_deficit
      = total_N_uptake -  max_above_ground_N_mass -  max_root_N_mass;
   return uptake_excess_or_deficit;
}
//_calc_uptake_excess_or_deficit____________________________________2006-12-13_/
bool Crop_nitrogen_common::allocate_available_to_plant_part_as_demanded
(float64 &to_be_allocated
,float64 &plant_part_N_mass
,float64 &plant_part_N_demand) const   //160408  rename demand to requirement
{
   float64 N_to_plant_part = 0.0;
   if (to_be_allocated > plant_part_N_demand)
   {   N_to_plant_part = plant_part_N_demand;
       to_be_allocated -= plant_part_N_demand;
       plant_part_N_demand = 0.0;
   } else
   {  N_to_plant_part = to_be_allocated;
      plant_part_N_demand -= to_be_allocated;
      to_be_allocated -= to_be_allocated;
   }
   plant_part_N_mass += N_to_plant_part;
   bool  to_be_allocated_is_zero                                                 //081219
      =     is_zero(plant_part_N_demand,/*0.0,*/0.00000001)                 //081219
         || CORN::is_zero<float64>(to_be_allocated,/*0,*/0.0000000001);     //081219
   return to_be_allocated_is_zero;
}
//_allocate_available_to_plant_part_as_demanded_____________________2007-01-31_/
float64 Crop_nitrogen_common::get_canopy_concentration(nat8 vital_effete) const
{
   float64 canopy_conc = canopy_concentration_vital;
   if ((vital_effete == include_effete) && (canopy_N_mass_effete > 0.0))
      canopy_conc = canopy_concentration_effete;
   if ((vital_effete == (include_vital | include_effete)))
   {  // In the case of vital and effete (should used only for output)
      // recalculate the concentration from total biomass and total nitrogen
      float64 above_ground_biomass_curr=crop.get_canopy_biomass_kg_m2_including
         (include_vital|include_effete);
      if (above_ground_biomass_curr > 0.0)                                       //070326
      {  float64 above_ground_N_curr=canopy_N_mass_vital + canopy_N_mass_effete;
         canopy_conc = above_ground_N_curr/above_ground_biomass_curr;
      }
   }
/*
if (canopy_conc > canopy_concentration_vital)
std::clog << "Reached" << std::endl;

canopy_conc = canopy_concentration_vital;
*/
   // else  just the vital
   // NYI In the case of vital and effete (should used only for output)
   // recalculate the concentration from total biomass and total nitrogen
   // currently dead does not apply
   return canopy_conc;
}
//_get_canopy_concentration____________________________________________________/
float64 Crop_nitrogen_common::get_root_concentration(nat8 vital_effete)    const
{
   float64 root_conc = root_concentration_vital;
   if ((vital_effete == include_effete) && (root_N_mass_effete > 0.0))
      root_conc = root_concentration_effete;
   if ((vital_effete == (include_vital | include_effete)))
   {  // In the case of vital and effete (should used only for output)
      // recalculate the concentration from total biomass and total nitrogen
      float64 root_biomass_curr  = crop.roots_current                           //190102
         ?crop.roots_current->calc_biomass(0) : 0;
      if (root_biomass_curr > 0.0)
      {  float64 root_N_curr  = root_N_mass_vital + root_N_mass_effete;
         root_conc = root_N_curr/root_biomass_curr;
      }
   }
   // else  just the vital
   // NYI In the case of vital and effete (should used only for output) recalculate the concentration from total biomass and total nitrogen
   // currently dead does not apply
   return root_conc;
}
//_get_root_concentration___________________________________________2007-03-21_/
float64 Crop_nitrogen_common::remove_canopy_nitrogen_for
(float64 respective_biomass,nat8 include_vital_or_effete)
{  float64 act_N_removed = 0.0;
   if (include_vital_or_effete & include_vital)                                  //070627
   {  float64 vital_N_mass_to_remove = respective_biomass * canopy_concentration_vital;
      vital_N_mass_to_remove = std::min<float64>(vital_N_mass_to_remove,canopy_N_mass_vital);
      canopy_N_mass_vital -= vital_N_mass_to_remove;
      act_N_removed += vital_N_mass_to_remove;
   }
   if (include_vital_or_effete & include_effete)                                 //070627
   {  float64 effete_N_mass_to_remove =  respective_biomass * canopy_concentration_effete;
      effete_N_mass_to_remove = std::min<float64>(effete_N_mass_to_remove,canopy_N_mass_effete);
      canopy_N_mass_effete -= effete_N_mass_to_remove;
      act_N_removed += effete_N_mass_to_remove;
   }
   daily.removed                    += act_N_removed;                            //070627
   if (mass_balance) mass_balance->removed_canopy += act_N_removed;
   return act_N_removed;
}
//_remove_canopy_nitrogen_for_______________________________________2007-04-12_/
Crop_nitrogen_common::Amounts::Amounts()
{  clear(); }
//_Amounts:constructor______________________________________________2006-12-13_/
void Crop_nitrogen_common::Amounts::clear()
{  uptake                     = 0.0;                                             //070209
   fixation                   = 0.0;
   gaseous_loss               = 0.0;
   removed                    = 0.0;
   fertilization_requirements = 0.0;
}
//_clear_______________________________________________________________________/
void Crop_nitrogen_common::Amounts::accumulate(const Amounts &augend)
{  uptake                     += augend.uptake;                                  //070209
   fixation                   += augend.fixation;
   gaseous_loss               += augend.gaseous_loss;
   removed                    += augend.removed;
   fertilization_requirements += augend.fertilization_requirements;
}
//__________________________________________________________________accumulate_/
RENDER_INSPECTORS_DEFINITION(Crop_nitrogen_common)
{
   #ifdef CS_OBSERVATION
   //  uses crop emanator context because the nitrogen is static member          //160616
   inspectors.append(new CS::Inspector_scalar(stress_factor_stats.ref_current()   ,UC_factor  ,*context,"nitrogen/stress/factor"           ,TS_VALUE  ,CSVP_crop_N_stress_factor));
   inspectors.append(new CS::Inspector_scalar(stress_factor_stats.index           ,UC_index   ,*context,"nitrogen/stress/index"            ,TS_VALUE  ,CSVP_crop_N_stress_index));
   inspectors.append(new CS::Inspector_scalar(reported_canopy_mass_current        ,UC_kg_m2   ,*context,"nitrogen/canopy/mass"             ,sum_statistic    ,CSVC_crop_N_mass_canopy_current));             //CSVP_crop_N_mass_canopy_current
            // N mass is cumulative //171030
   inspectors.append(new CS::Inspector_scalar(daily.gaseous_loss                  ,UC_kg_m2   ,*context,"nitrogen/canopy/gaseous_loss"     ,TS_VALUE,CSVP_crop_N_canopy_gaseous_loss/*171222 CSVC_crop_N_canopy_gaseous_loss*/));             //CSVP_crop_N_canopy_gaseous_loss
   inspectors.append(new CS::Inspector_scalar(daily.fertilization_requirements    ,UC_kg_m2   ,*context,"nitrogen/requirements/additional" ,TS_VALUE,CSVP_crop_N_automatic_uptake/*171222 CSVC_crop_N_automatic_uptake_time_step_sum*/ ));  //CSVP_crop_N_automatic_uptake
   inspectors.append(new CS::Inspector_scalar(daily.fixation                      ,UC_kg_m2   ,*context,"nitrogen/fixation"                ,TS_VALUE,CSVP_crop_N_fixation));
   //NYI (seasonal) inspectors.append(new CS::Inspector_scalar(member_variable                     ,UC_kg_kg   ,*context,"nitrogen/flower_mature_conc_ratio",value_statistic  ,CSVP_crop_N_flower_mature_conc_ratio));
   inspectors.append(new CS::Inspector_scalar(daily.uptake                        ,UC_kg_m2   ,*context,"nitrogen/uptake"                  ,TS_VALUE,CSVP_crop_N_uptake)); //,CSVC_crop_N_uptake_time_step_sum));
   //NYI (seasonal?) inspectors.append(new CS::Inspector_scalar(report_excess_or_deficit            ,UC_kg_m2   ,*context,"nitrogen/uptake_excess_or_deficit",sum_statistic    ,CSVP_crop_N_uptake_excess_or_deficit,CSVC_UNKNOWN)); // sum? diff?
   inspectors.append(new CS::Inspector_scalar(N_min_concentration                 ,UC_kg_kg   ,*context,"nitrogen/min/conc"                ,TS_VALUE  ,CSVP_crop_N_conc_min));
   inspectors.append(new CS::Inspector_scalar(N_max_concentration                 ,UC_kg_kg   ,*context,"nitrogen/max/conc"                ,TS_VALUE  ,CSVP_crop_N_conc_max));
   inspectors.append(new CS::Inspector_scalar(N_crit_concentration                ,UC_kg_kg   ,*context,"nitrogen/crit/conc"               ,TS_VALUE  ,CSVP_crop_N_conc_critical));
   inspectors.append(new CS::Inspector_scalar(reported_root_conc                  ,UC_kg_kg   ,*context,"nitrogen/root/conc"               ,TS_VALUE  ,CSVP_crop_N_conc_roots_live));
   //NYI inspectors.append(new CS::Inspector_scalar(reported_root_dead_conc                  ,UC_kg_kg   ,*context,"nitrogen/root/dead/conc"               ,TS_VALUE  ,CSVP_crop_N_conc_roots_dead/*171222 CSVC_crop_N_conc_roots_live*/ )); //CSVP_crop_N_conc_roots_live
   #endif
   return 0; // emanator;
}
//_RENDER_INSPECTORS_DEFINITION________________________________________________/
}//_namespace_CropSyst_________________________________________________________/

