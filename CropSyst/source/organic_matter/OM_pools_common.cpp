
#include "organic_matter/OM_pools_common.h"
#include "organic_matter/OM_common.h"
#include "soil/nitrogen_I.h"
#include "soil/structure_I.h"
#include "soil/soil_I.h"
#include "soil/disturbance.h"
#include "soil/abiotic_environment.h"
#include "corn/math/compare.hpp"
#include "corn/const.h"
#include "corn/chronometry/date_32.h"
#include "options.h"
#include "CS_suite/observation/CS_inspector.h"
#include "CS_suite/observation/CS_optation.h"
#include "csvc.h"

#ifdef DETAILED_BALANCES
extern C_Balance *C_balance;                                                     //111005
#endif
static float32 deleted_residues = 0;
//#if (CROPSYST_VERSION==5)
#include "CS_suite/observation/CS_inspector.h"
//#endif

#define OM_type_index      nat8
#define OM_position_index  nat8
#define Mass_index         nat8
/*debug output
std::ofstream actual_decomp("act_decomp.dat");
*/

namespace CS
{
extern int32 simdate_debug;
}

//______________________________________________________________________________
Organic_matter_total_common::Organic_matter_total_common()
{  // Apparently autoincrement of  enums in not allowed in standard C++
   for (Mass_index mt = BIOMASS_TOTAL; mt < MASS_TOTAL_COUNT; mt++)
        mass[mt] = 0;
}
//_Organic_matter_total_common:constructor_____________________________________/
void Organic_matter_total_common::add(const Organic_biomatter_common &other)
{  mass[BIOMASS_TOTAL]        += other.kg();
   mass[N_MASS_TOTAL]         += other.get_nitrogen_mass();
   mass[C_MASS_TOTAL]         += other.get_carbon_kg();
#ifdef NYN
currently not by layer or type
   mass[N_MINERALIZED_TOTAL]  += other.N_mineralized_output;
   mass[N_IMMOBILIZED_TOTAL]  += other.N_immobilized_output;
#endif
}
//_add_________________________________________________________________________/
void Organic_matter_total_common::clear()
{  for (Mass_index  mt = BIOMASS_TOTAL; mt < MASS_TOTAL_COUNT; mt++)
      mass[mt] = 0;
}
//_clear_______________________________________________________________________/
void Organic_matter_pools_totals_common
::add(const Organic_biomatter_common &org_BM)
{  masses[org_BM.type][org_BM.position].add(org_BM);
}
//_Organic_matter_pools_totals_common__________________________________________/
Organic_matter_pools_totals_common::Organic_matter_pools_totals_common()
{  for (OM_type_index omt = FIRST_ORGANIC_MATTER_TYPE; omt< ORGANIC_MATTER_TYPE_COUNT; omt++)
      for (OM_type_index omp = FIRST_OM_POSITION; omp < OM_POSITION_COUNT; omp++)
         masses[omt][omp].clear();
}
//_Organic_matter_pools_totals_common:constructor___________________2007-07-07_/
Organic_matter_pools_common::Organic_matter_pools_common
(const Organic_matter_decomposition_parameters &OM_decomposition_parameters_     //120912
,Soil::Abiotic_environment_profile   &soil_abiotic_environment_profile_           //060726
,const Soil::Layers_interface        &soil_layers_                                //060726
,const Soil::Structure    &soil_structure_                             //070725
,Soil::Nitrogen            *soil_N_
,const Soil::Texture_interface       &soil_texture_)                              //070707
: CORN::Unidirectional_list                                                   ()
, pools_totals                                                               (0)
//190215, subsurface_contact_fraction                                      (1.0)
, OM_decomposition_parameters                     (OM_decomposition_parameters_) //120912
, soil_texture                                                   (soil_texture_) //070707
, soil_abiotic_environment_profile           (soil_abiotic_environment_profile_) //060726
, soil_layers                                                     (soil_layers_) //060726
, soil_structure                                               (soil_structure_) //070725
, soil_N                                                               (soil_N_)
, soil_disturbance                                                           (0) //060807
, subsurface_contact_fraction                                              (1.0) //190327
{  clear_totals();
   for (nat8 layer = 0; layer < MAX_soil_layers; layer++)
   {  for (OM_type_index OM_type = (Organic_matter_type)0                        //060726
          ; OM_type < ORGANIC_MATTER_TYPE_COUNT;OM_type++)
         organic_matter[OM_type][layer] = 0;                                     //060726
   }
}
//_Organic_matter_pools_common:constructor_____________________________________/
Organic_matter_pools_common::~Organic_matter_pools_common()
{
   #if (CROPSYST_VERSION==5)
   //201130LML some errors if (emanator_known) emanator_known->set_status(CS::Emanator::deleted_status); //160614
   #endif
   if (pools_totals) delete pools_totals;                                        //201129LML
}
//_Organic_matter_pools_common:destructor___________________________2016-06-16_/
#ifdef NYI
May need to move to organic_matter_pools_single_pool
Organic_biomatter_common *Organic_matter_pools_common::provide_organic_matter_pool
(Organic_matter_type type,nat8 layer)                                 provision_ //190215
{  Organic_biomatter_common *provided_pool = organic_matter[type][layer];
   if (!provided_pool)
   {  provided_pool = organic_matter[type][layer]
         = new Organic_biomatter_common
         (type
         ,layer
         ,IRRELEVENT_POSITION
         ,0
         ,subsurface_contact_fraction
         ,soil_texture
         ,soil_abiotic_environment_profile.ref_soil_abiotic_environment_layer( layer));
   }
   return provided_pool;
}
//_provide_organic_matter_pool______________________________________2006-07-26_/
#endif
Organic_biomatter_common *
Organic_matter_pools_common::get_organic_matter_pool
(Organic_matter_type type,nat8  layer)
{  Organic_biomatter_common *got_pool = organic_matter[type][layer];
   return got_pool;
}
//_get_organic_matter_pool__________________________________________2017-03-01_/
void Organic_matter_pools_common::clear_totals()               //initialization_
{  CO2_C_loss_daily[0] = 0;                                                      //060811
   CO2_C_loss_daily[1] = 0;                                                      //060811
   CO2_C_loss_daily[2] = 0;                                                      //060811
   today_OM_mineralized_kgN_m2 = 0;
   today_N_immobilized = 0;
   C_fraction_transfered_to_CO2 = 0;
   clear_layer_array64(immobilized_NO3_N);
   clear_layer_array64(immobilized_NH4_N);
   clear_layer_array64(CO2_C_loss_subsurface_residue_and_OM);
   clear_layer_array64(CO2_C_loss_subsurface_residue);                           //080902
   clear_layer_array64(CO2_C_loss_subsurface_OM);                                //080902
   clear_layer_array64(N_immobilization_demand_sum/*190513 _subsurface*/);
   clear_layer_array64(mineralized_N_residue);
   clear_layer_array64(mineralized_N_OM);                                        //060512
   clear_layer_array64(mineralized_N_OM_residue);                                //060512
   clear_layer_array64(N_deficit_for_immobilization_subsurface);                 //120922  actually this doesn't appear to be used anywhere
   for (OM_position_index p  = FIRST_OM_POSITION
       ;p < OM_POSITION_COUNT; p+=1)
   {  act_C_decomposed_from_all   [p] = 0;
      act_C_decomposed_from_residue[p] = 0;
   }
   C_incorporated_by_tillage = 0;
   clear_layer_array64(reduction_factor_subsurface);                             //110928
   clear_layer_array64(pot_C_decomp_subsurface);                                 //111004
   for (OM_position_index p = FIRST_OM_POSITION; p < OM_POSITION_COUNT; p+=1)    //111004
      pot_C_decomp_surface    [p] = 0;                                           //111004
}
//_clear_totals________________________________________________________________/
#ifdef NYN
bool Organic_matter_pools_V4_4::initialize(
      soil_horizon_array32(horizon_percent_organic_matter)
      ,const Organic_matter_historical_parameters &_organic_matter_historical_parameters)
{}
//_initialize_______________________________________________________2006-02-16_/
#endif
float64 Organic_matter_pools_common::get_mass(Mass_total mass_total,uint32 include_organic_matter_types,uint32 include_positions) const
{  float64 total_mass = 0;
   if (!pools_totals)   update_totals();
   for (OM_type_index t = FIRST_ORGANIC_MATTER_TYPE; t < ORGANIC_MATTER_TYPE_COUNT; t++)
      if ((1 << t) & include_organic_matter_types)
         for (OM_position_index p = FIRST_OM_POSITION ; p < OM_POSITION_COUNT; p++)
            if ((1 << p) & include_positions)
               total_mass +=  pools_totals->masses[t][p].mass[mass_total];
   return total_mass;
}
//_get_mass_________________________________________________________2006-03-31_/
float64 Organic_matter_pools_common::get_total_N_immobilized() const
{  return today_N_immobilized;
}
//_get_total_N_immobilized__________________________________________2006-04-03_/
#ifdef NYN
float64 Organic_matter_pools_V4_4::get_mass_subsurface_only(Mass_total mass_total,uint32 include_organic_matter_types) const
{  float64 total_mass = 0;
   if (!pools_totals)   update_totals();
   for (Organic_matter_type t = FIRST_ORGANIC_MATTER_TYPE; t < ORGANIC_MATTER_TYPE_COUNT; t++)
      if ((1 << t) & include_organic_matter_types)
               total_mass +=  pools_totals->masses[t][p].mass[mass_total];
   return total_mass;
}
//_get_mass_subsurface_only_________________________________________2006-03-31_/
#endif
bool  Organic_matter_pools_common::is_included(Organic_matter_type om_type,uint32 include_organic_matter_types_mask) const
{  bool included = false;
      if ((1 << om_type) & include_organic_matter_types_mask)
           included = true;
   return included;
}
//_is_included______________________________________________________2006-04-03_/
float64 Organic_matter_pools_common
::get_mass_from_layer_to_layer                                                   //071214
   (Mass_total mass_total
   ,nat32 include_organic_matter_types
   ,nat8 from_layer
   ,nat8 to_layer
   ,float64 prorate_fraction_bottom_layer) const
//061218 rename this to get_mass_from_position
{  // Here we have to sum each pool because I dont have tally by layer
   float64 total_mass = 0;

   FOR_EACH(org_BM,Organic_biomatter_common,org_BM_iter)
   {  bool is_in_layer_selection = ((org_BM->layer >= from_layer) && (org_BM->layer<=to_layer))   //070315
              || ((from_layer == 0) && (org_BM->position > OM_subsurface_position));   //090226
      bool is_selected_OM_type = is_included(org_BM->type,include_organic_matter_types);
      float64 mass_in_layer = 0.0;                                               //071214
      if (is_in_layer_selection && is_selected_OM_type)
         switch(mass_total)
         {  case BIOMASS_TOTAL      : mass_in_layer= org_BM->kg();               break;
            case N_MASS_TOTAL       : mass_in_layer= org_BM->get_nitrogen_mass();break;
            case C_MASS_TOTAL       : mass_in_layer= org_BM->get_carbon_kg();    break;
            case N_MINERALIZATION_TOTAL: mass_in_layer= org_BM->get_N_mineralization_mass(); break;   //061116
            //  case N_IMMOBILIZATION_DEMAND_TOTAL: mass_in_layer= org_BM->get_N_immobilization_demand_mass(); break;
            //                        case xxxxx: mass_in_layer= pool->xxx; break;
            default: break;                                                      //170217
         }
      float64 mass_from_layer = mass_in_layer;                                   //071214_
      // If we are looking at the bottom layer (to_layer) then the caller may only want to include
      // a fraction of the mass because the observation depth may be shorted that the depth of the layer
      // (currently dont have a need to prorate the top layer (from_layer).
      if (org_BM->layer == to_layer) mass_from_layer *= prorate_fraction_bottom_layer; //071214
      total_mass += mass_from_layer;                                             //071214
   } FOR_EACH_END(org_BM_iter)
   return total_mass;
}
//_get_mass_from_layer_to_layer_____________________________________2006-03-31_/
float64 Organic_matter_pools_common::get_C_decomposed_from_layer_to_layer // This is used only for output carbon annual report
(nat32 include_organic_matter_types
,nat8 from_layer
,nat8 to_layer
,float64 prorate_fraction_bottom_layer)                                    const
{  // Here we have to sum each pool because I dont have tally by layer
   float64 total_C_decomp_mass = 0;
   FOR_EACH(org_BM,Organic_biomatter_common,org_BM_iter)
   {  bool is_in_layer_selection = ((org_BM->layer >= from_layer) && (org_BM->layer<=to_layer))
              || ((from_layer == 0) && (org_BM->position > OM_subsurface_position));
      bool is_selected_OM_type = is_included(org_BM->type,include_organic_matter_types);
      float64 C_decomp_mass_from_layer = 0.0;
      if (is_in_layer_selection && is_selected_OM_type)
         {  C_decomp_mass_from_layer= org_BM->get_C_decomposed_for_output_only();
         }
      // If we are looking at the bottom layer (to_layer) then the caller may only want to include
      // a fraction of the mass because the observation depth may be shorted that the depth of the layer
      // (currently dont have a need to prorate the top layer (from_layer).
      if (org_BM->layer == to_layer) C_decomp_mass_from_layer *= prorate_fraction_bottom_layer;
      total_C_decomp_mass += C_decomp_mass_from_layer;
   } FOR_EACH_END(org_BM_iter)
   return total_C_decomp_mass;
}
//_get_C_decomposed_from_layer_to_layer_____________________________2009-05-20_/
float64 Organic_matter_pools_common::get_nitrogen_mass_including
(nat32 include_organic_matter_types,uint32 include_positions)             const
{  return  get_mass(N_MASS_TOTAL,include_organic_matter_types,include_positions);
}
//_get_nitrogen_mass___________________________________________________________/
uint32 Organic_matter_pools_common::update_totals()                   provision_
{  nat32 OM_pool_count = 0;
   pools_totals = new Organic_matter_pools_totals_common;                        //070707
   FOR_EACH(org_BM,Organic_biomatter_common,org_BM_iter)
   {  pools_totals->add(*org_BM);
      OM_pool_count++;
   } FOR_EACH_END(org_BM_iter)
   return OM_pool_count;
}
//_update_totals_______________________________________________________________/
bool Organic_matter_pools_common::start_day()
{  invalidate_totals();
   bool success = true;                                                          //090702
   clear_totals();
   success &= residue_detrition();
   FOR_EACH(org_BM,Organic_biomatter_common,org_BM_iter)   // continue here use common
   {
     org_BM->clear_decomposition();
     #if (CROPSYST_VERSION >= 5)
      // Actually may want to do this is all versions (Not currently doing this for V4 because it does give a little bit of discrepency)
      if (org_BM->is_straw_or_manure_residue() && (org_BM->kg() < 0.0000001))
      {  // delete negligible residues
          deleted_residues += org_BM->kg();
          org_BM_iter->delete_current();
      }
     #endif
   } FOR_EACH_END(org_BM_iter)
   return success;
}
//_start_day________________________________________________________2006-03-30_/
bool Organic_matter_pools_common::tally_immobilization()
{
   today_N_immobilized = 0.0;                                                    //060717
   if (soil_N)
   {  for (nat8 layer = 1; layer <= MAX_soil_layers /*actually only the current soil layers are needed */
       ;layer++)
      {  update_soil_N(layer);
         today_N_immobilized += immobilized_NO3_N[layer] + immobilized_NH4_N[layer];
      }
      soil_N->commit_diffs();                                                    //120808
   }
   return true;
}
//_tally_immobilization________________________________________________________/
bool Organic_matter_pools_common::process()
{  bool processed = false;
   potential_C_decomposition(); // every where
   processed |= determine_reduction_factor_layers();
                actual_C_decomposition();
   processed |= tally_immobilization();
   return processed ;
}
//_process_______________________________________________2011-09-28_2007-07-07_/
bool Organic_matter_pools_common::residue_detrition()             modification_
{  FOR_EACH(org_BM,Organic_biomatter_common,org_BM_iter)
   {  if (org_BM->is_straw_or_manure_residue() && !org_BM->is_subsurface()) // stubble or flat
      {  Organic_biomatter_common *residue_detritis = org_BM->detrition();
         if (residue_detritis)
         {  Organic_biomatter_common *similar_detritis = find_similar_organic_biomatter(*residue_detritis);
            if (similar_detritis)
            {  similar_detritis->add_mass(residue_detritis->kg());
               delete residue_detritis; residue_detritis = 0;
            }
            else
            {
//               assert(residue_detritis->layer == org_BM->layer); //190218
//*190218 Disabled because I want soil_abiotic_env_layer to be const reference, but it needs to be updated

               residue_detritis->know_soil_abiotic_environment(
                  soil_abiotic_environment_profile
                  .ref_soil_abiotic_environment_layer(residue_detritis->layer));//090827

/*
               residue_detritis->soil_abiotic_env =
                  soil_abiotic_environment_profile
                  .ref_soil_abiotic_environment_layer(residue_detritis->layer);//090827
*/
//*/
               append(residue_detritis);
            }
         }
      }
   } FOR_EACH_END(org_BM_iter)
   return true;
}
//_residue_detrition________________________________________________2009-07-02_/
void Organic_matter_pools_common::invalidate_totals()                  mutation_
{  if (pools_totals) delete pools_totals;
   pools_totals = 0;
}
//_invalidate_totals___________________________________________________________/
bool Organic_matter_pools_common::set_modified(bool _modified)         mutation_
{  CORN::Unidirectional_list::set_modified(_modified);
   invalidate_totals();
   return true;
}
//_set_modified________________________________________________________________/
void Organic_matter_pools_common::update_soil_N
(nat8 residue_horizon)
{     // Surface residue N input and output goes to sublayer 1
   nat8   soil_layer = residue_horizon ? residue_horizon : (nat8)1;
   float64 NO3_molecular_immobilized      = soil_N->convert_NO3_N_to_molecular_mass(immobilized_NO3_N[soil_layer]);
   float64 NH4_molecular_mass_immobilized = soil_N->convert_NH4_N_to_molecular_mass(immobilized_NH4_N[soil_layer]);
   float64 total_N_mineralization   = (mineralized_N_residue[soil_layer] + mineralized_N_OM[soil_layer]); //060512
   float64 total_NH4_molecular_mineralization_ = soil_N->convert_NH4_N_to_molecular_mass(total_N_mineralization);;   //060717
   soil_N->subtract_NO3_molecular_mass(soil_layer,NO3_molecular_immobilized);
   soil_N->subtract_NH4_molecular_mass(soil_layer,NH4_molecular_mass_immobilized);
   soil_N->add_NH4_molecular_mass(soil_layer,total_NH4_molecular_mineralization_);     //060717
   mineralized_N_OM_residue[soil_layer] = total_N_mineralization;                //060512
   today_OM_mineralized_kgN_m2 += total_N_mineralization;
   float64 NO3_amount_change_M = NO3_molecular_immobilized;
   #ifdef CROPSYST_PROPER
   soil_N->NO3_dec_residue(NO3_amount_change_M);
   soil_N->NH4_dec_residue(-total_NH4_molecular_mineralization_);  // This will increment by total_NH4_molecular_mineralization_ 061219
   soil_N->NH4_dec_residue(NH4_molecular_mass_immobilized);                      //061219
   #endif
}
//_update_soil_N_______________________________________________________________/
float64 Organic_matter_pools_common::get_soil_organic_matter_mineralization_profile_N() const
{  float64 profile_sum = 0.0;
   for (int l = 0; l < MAX_soil_layers; l++)
      profile_sum += get_soil_organic_matter_mineralization_N(l);                //060717
   return profile_sum;
}
//_get_soil_organic_matter_mineralization_profile_N_________________2006-04-03_/
float64 Organic_matter_pools_common::get_soil_organic_matter_immobilization_profile_N() const
{  float64 profile_sum = 0.0;
   for (int l = 0; l < MAX_soil_layers; l++)
      profile_sum += get_immobilization_N(l);
   return profile_sum;
}
//_get_soil_organic_matter_immobilization_profile_N_________________2011-09-18_/
float64 Organic_matter_pools_common
::get_soil_organic_matter_mineralization_N(nat8 layer)                     const
{  return mineralized_N_OM_residue[layer];  // we report back both residue and SOM  060717
}
//_get_soil_organic_matter_immobilization_profile_N_________________2006-04-03_/
Organic_biomatter_common *Organic_matter_pools_common
::find_similar_organic_biomatter(const Organic_biomatter_common &to_be_found)
{  FIND_FOR_EACH(found_match,match,Organic_biomatter_common ,true,each_OM)
   {  if (match->is_comparable(to_be_found) )
         found_match = match;
   } FOR_EACH_END(each_OM)
   return found_match;
}
//_find_similar_organic_biomatter___________________________________2006-06-05_/
bool Organic_matter_pools_common::respond_to_field_operation                     //VB OrganicCarbonMixingDueToTillage(....)
(float64 tillage_op_depth_m                                                      //VB  T_Depth = ManagementState.TillageDepth
,float64 tillage_op_mixing_fraction                                              //VB  T_Mix = ManagementState.TillageMixingFraction
,const Soil::Texture_interface &soil_texture                                      // texture may have changed   //060807
,const Soil::Disturbance       *_soil_disturbance)                                //060807
{
   soil_disturbance = _soil_disturbance;                                         //060807
// 060727 RLN Warning this redistribution routine doesn't look quite right to me
//        This will preserve the mass balance, but
//        I dont think the mixing preserves the relative carbon and nitrogen  balance.
//        The old residue redistribution does things right.
//        I think this should take biomass out of the contributing layer by some fraction
//        then add the taken out part in to the the mixed into layer.
//        The way the old residue redistribution does.

//Tillage event check not applicable in C++ because the event is scheduled.  VB Tillage_Application = ManagementState.TillageEvent
//Tillage event check not applicable in C++ because the event is scheduled. If Tillage_Application Then

   // Load C/N ratios
/* RLN: Not needed in C++ version because adding the pool takes care of C and N masses and rations
   float64 MB_CN_Ratio = SimConstants.MicrobialBiomassCNRatio
	LabileSOM_CN_Ratio = SimConstants.LabileActiveSOMCNRatio
	MetastableSOM_CN_Ratio = SimConstants.MetastableActiveSOMCNRatio
	PassiveSOM_CN_Ratio = SimConstants.PassiveSOMCNRatio
*/
   if (CORN::is_zero<float64>(tillage_op_depth_m))                               //070106
      return 0; // Note that get_layer_at(0.0) returns the total soil profile depth //070106
   nat8 last_tilled_layer = soil_layers.get_layer_at_depth(tillage_op_depth_m);    //070828 VB Last_Tilled_Layer = ManagementState.LastLayerTilled
   soil_layer_array64(soil_mass_kg_m2);                                          // VB Dim Layer_Soil_Mass(6) As Single
   float64 SOM_biomass_fract[MAX_soil_layers_alloc][ORGANIC_MATTER_TYPE_COUNT];
   float64 SOM_mixed[ORGANIC_MATTER_TYPE_COUNT];
   // Clearing arrays
   for (OM_type_index OM_type = FIRST_ORGANIC_MATTER_TYPE; OM_type < ORGANIC_MATTER_TYPE_COUNT; OM_type++)
   {  SOM_mixed[OM_type] = 0.0;
      for (int lyr = 0; lyr < MAX_soil_layers_alloc; lyr++)
         SOM_biomass_fract[lyr][OM_type] = 0;
   }
   // In C++ version I use a matrix and also masses are stored in biomass     //060727
   // Retrieve mass-base contents.  All in unit of kg per kg of soil
   for (nat8 layer = 1; layer <= last_tilled_layer; layer++)
   {  float64 bulk_density_layer_kg_m3    = soil_structure.get_bulk_density_kg_m3(layer);    //070725
      float64 soil_mass_layer_kg_m2       = soil_mass_kg_m2[layer] = bulk_density_layer_kg_m3 * soil_layers.get_thickness_m(layer);
      for (nat8 OM_index = SOM_INDEX_FIRST
          ; OM_index < SOM_index_last() ; OM_index++)
      {  Organic_biomatter_common *SOM_pool
            = get_organic_matter_pool                                            //170301
               ((Organic_matter_type)OM_index,layer);
         float64 SOM_BM_fract = SOM_pool // NYN && !is_zero<float>(soil_mass_layer_kg_m2)
            ? SOM_pool->kg() / soil_mass_layer_kg_m2 : 0.0;
         SOM_biomass_fract[layer][OM_index] = SOM_BM_fract;                      //170301
      }
   } // for tilled layers

//cout << "sum_soil_mass_before:"<< sum_soil_mass_before << endl;
   float64 total_soil_mass_mixed = 0.0;                                      // VB Dim Total_Soil_Mass_Mixed As Single
   soil_layer_array64(soil_mass_mixed_after_tillage);      clear_layer_array64(soil_mass_mixed_after_tillage);       // VB Dim Layer_Soil_Mass_Mixed_After_Tillage(6) As Single
   soil_layer_array64(soil_mass_remaining_after_tillage);  clear_layer_array64(soil_mass_remaining_after_tillage);   // VB Dim Layer_Soil_Mass_Remaining_After_Tillage(6) As Single
   float64 bottom_depth_layer_above = 0;

   for (nat8 layer = 1; layer <= last_tilled_layer; layer++)
   {  float64 soil_mass_layer_kg_m2= soil_mass_kg_m2[layer];
      float64 bottom_depth_layer = soil_layers.get_depth_m(layer);
      if (bottom_depth_layer <= tillage_op_depth_m)     // tillage apply for the full depth of the layer
      {  soil_mass_remaining_after_tillage[layer]  = (1.0 - tillage_op_mixing_fraction) * soil_mass_layer_kg_m2;
         soil_mass_mixed_after_tillage[layer]      = soil_mass_layer_kg_m2 * tillage_op_mixing_fraction;
      } else if ((bottom_depth_layer > tillage_op_depth_m) && (bottom_depth_layer_above < tillage_op_depth_m))
      {  float64 thickness_layer = soil_layers.get_thickness_m(layer);
         soil_mass_mixed_after_tillage[layer]      = tillage_op_mixing_fraction * soil_mass_layer_kg_m2 * (tillage_op_depth_m - bottom_depth_layer_above) / thickness_layer; // ' weights the mixing by the fraction of the layer affected by tillage
         soil_mass_remaining_after_tillage[layer]  = soil_mass_layer_kg_m2 - soil_mass_mixed_after_tillage[layer];
      } else  //  this else is redundant, as if there is no tillage there is no mixing, keep it here for clarity
      {  soil_mass_remaining_after_tillage[layer]  = soil_mass_layer_kg_m2;
         soil_mass_mixed_after_tillage[layer]      = 0.0;
      }
      float64 soil_mass_mixed_after_tillage_layer = soil_mass_mixed_after_tillage[layer];
      float64  soil_mass_mixed_to_depth = (total_soil_mass_mixed + soil_mass_mixed_after_tillage_layer); //061110
      if (soil_mass_mixed_to_depth > 0.0) // Could be 0.0 if the mixing effect in negligible (I.e. Lifting)  (061110 this is not in the V.B. version)
         for (nat8 OM_index = SOM_INDEX_FIRST                                    //170301
            ; OM_index < SOM_index_last(); OM_index++)
            SOM_mixed[OM_index] = (total_soil_mass_mixed * SOM_mixed[OM_index]
                 + soil_mass_mixed_after_tillage_layer * SOM_biomass_fract[layer][OM_index])
               / soil_mass_mixed_to_depth;
      total_soil_mass_mixed += soil_mass_mixed_after_tillage_layer;
      bottom_depth_layer_above = bottom_depth_layer;
   }
   for (nat8 layer = 1; layer <= last_tilled_layer; layer++)
   {
      float64 soil_mass_layer_kg_m2 = soil_mass_kg_m2[layer];
      float64 soil_mass_remaining_after_tillage_layer = soil_mass_remaining_after_tillage[layer];
      float64 soil_mass_mixed_after_tillage_layer     = soil_mass_mixed_after_tillage[layer];
      for (nat8 OM_index = SOM_INDEX_FIRST                                       //170301
          ;     OM_index < SOM_index_last(); OM_index++)
      {  float64 new_SOM_biomass_fraction
            = (SOM_biomass_fract[layer][OM_index] * soil_mass_remaining_after_tillage_layer
             + SOM_mixed[OM_index] * soil_mass_mixed_after_tillage_layer) / soil_mass_layer_kg_m2;
         Organic_biomatter_common *SOM_pool  =
            get_organic_matter_pool((Organic_matter_type)OM_index,layer);        //070707
            // (layer pools should now always already exist provide_organic_matter_pool //170301
         SOM_pool->reset_mass(new_SOM_biomass_fraction * soil_mass_layer_kg_m2);
      }
   }
   //Tillage event: check not applicable in C++ because the event is scheduled.
   return true;
}
//_respond_to_field_operation_______________________________________2006-07-27_/
float64 Organic_matter_pools_common::get_CO2_C_loss_reported_daily(nat8 include_OM) const
{  float64 CO2_loss_included = 0;
   switch (include_OM)
   {  case INCLUDE_RESIDUE_OM       : CO2_loss_included = CO2_C_loss_daily[2] ; break;
      case INCLUDE_SOM_AND_MICROBIAL: CO2_loss_included = CO2_C_loss_daily[1] ; break;
      default                       : CO2_loss_included = CO2_C_loss_daily[0]; break; /* INCLUDE_ALL_ORGANIC_MATTER */
   }
   return CO2_loss_included;
}
//_get_CO2_C_loss_reported_daily____________________________________2006-04-26_/
bool Organic_matter_pools_common::determine_reduction_factor_layers() modification_
{  // reduction factor for surface (residues) is always 1  (so it appears in VB code) 060329

   for (nat8 layer = 1; layer <= MAX_soil_layers /*actually only the current soil layers are needed */
       ;layer++)
   {  // Reduction factor is 1.0 when not simulating N
      reduction_factor_subsurface[layer] = (soil_N)
      ?  determine_reduction_factor
         (layer
         ,soil_N->get_NH4_N_mass_kg_m2(layer)
         ,soil_N->get_NO3_N_mass_kg_m2(layer)
         ,N_immobilization_demand_sum/*190513 _subsurface*/[layer]
         ,N_deficit_for_immobilization_subsurface[layer])
      : 1.0;
   }

#ifdef IMMOBILIZE_SURFACE_WITH_3LAYERS

   // Process the subsurface first.
    determine_reduction_factor_surface
         (//member reduction_factor_subsurface // applied to layers 1 to 3
         //member ,soil_N
         N_immobilization_demand_sum[0]
         //member,N_deficit_for_immobilization_subsurface[layer]
         ); //190513

#endif
   return true;
}
//_determine_reduction_factor_layers________________________________2011-09-28_/
float64 Organic_matter_pools_common::transfer_carbon_and_nitrogen
(float64 &CO2_loss_SOM,float64 &CO2_loss_residue)
{  float64 total_C_transfer_to_CO2 = 0.0;
   #ifdef DETAILED_BALANCES
   C_balance->residue.current = 0.0;                                             //111005
   C_balance->OM.current = 0.0;                                                  //111005
   #endif
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)                    //111004
   {  float64 pool_C_transfer_to_CO2 = org_BM->transfer_carbon_and_nitrogen();
      nat8 layer = org_BM->is_above_ground()                                     //080819
         ? (nat8)1   // Above ground biomass exchanges matter with the soil surface
         : org_BM->layer ;
      CO2_C_loss_subsurface_residue_and_OM[layer] += pool_C_transfer_to_CO2;     //060606
      total_C_transfer_to_CO2 += pool_C_transfer_to_CO2;
      if (org_BM->is_straw_or_manure_residue())
      {  CO2_loss_residue += pool_C_transfer_to_CO2;

/*
if (CO2_loss_residue > 0)
std::clog << "residue CO2 loss to atm: " << CO2_loss_residue << std::endl;
*/


         CO2_C_loss_subsurface_residue[layer ] += pool_C_transfer_to_CO2;        //081030
         #ifdef DETAILED_BALANCES
         C_balance->residue.CO2_emission += pool_C_transfer_to_CO2;
         #endif
      }
      else
      {  CO2_loss_SOM += pool_C_transfer_to_CO2;  // for stable organic matter
         CO2_C_loss_subsurface_OM[layer ] += pool_C_transfer_to_CO2;             //081030
         #ifdef DETAILED_BALANCES
         C_balance->OM.CO2_emission += pool_C_transfer_to_CO2;
         #endif
      }
   } FOR_EACH_END(org_BM_iter)
    // These are in separate loops because there is interaction between the pools,
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)                    //111004
   {  org_BM->post_carbon_transfer();
      #ifdef DETAILED_BALANCES
      if (org_BM->is_straw_or_manure_residue())                                  //111005
         C_balance->residue.current += org_BM->get_carbon_kg();
      else
         C_balance->OM.current += org_BM->get_carbon_kg();
      #endif
   } FOR_EACH_END(org_BM_iter)
   return total_C_transfer_to_CO2;
}
//_transfer_carbon_and_nitrogen_____________________________________2011-10-04_/
// (Was in Organic_matter_pools_multiple)
bool Organic_matter_pools_common::transfer_carbon_and_nitrogen_between_pools()
{  // transfer carbon and nitrogen between pools
   float64 CO2_loss_SOM       = 0;                                               //060811
   float64 CO2_loss_residue   = 0;                                               //060811
   float64 CO2_loss_all
      = transfer_carbon_and_nitrogen (CO2_loss_SOM,CO2_loss_residue);            //060606
   CO2_C_loss_daily[0] += CO2_loss_all;                                          //060606
   CO2_C_loss_daily[1] += CO2_loss_SOM;                                          //060606
   CO2_C_loss_daily[2] += CO2_loss_residue;                                      //060606
   return true;
}
//_transfer_carbon_and_nitrogen_between_pools_______________________2011-09-28_/
float64 Organic_matter_pools_common::potential_C_decomposition()
{  float64 total_pot_C_decomp = 0.0;
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)
   {
      if (  (org_BM->layer >=1)                                                  //090706
         && (org_BM->is_straw_or_manure_residue()))                              //090826
      {  // This is a little hack to reset the contact fraction for detritis that was moved into the soil.
         org_BM->know_contact_fraction(subsurface_contact_fraction);             //190327
      }
      if (org_BM->kg() > 0.0)  // No need to do anything if there is no mass (There could be no mass for SOM pools
      {  float64 pot_C_decomp = 0;
         bool above_ground = org_BM->is_above_ground();
         nat8 soil_layer = above_ground                                          //190513
            ? (nat8) 1   // Above ground biomass exchanges matter with the soil surface
            : org_BM->layer ;
         float64  decomposition_adjustment_factor =  (soil_disturbance)
           ? soil_disturbance->get_decomposition_adjustment_factor(soil_layer) : 1.0; //070801
         pot_C_decomp = org_BM->potential_C_decomposition(
               (org_BM->is_straw_or_manure_residue()                             //060512
                ? mineralized_N_residue[soil_layer]                              //060512
                : mineralized_N_OM[soil_layer])                                  //060512
               ,N_immobilization_demand_sum/*190513_subsurface*/
                  [
                  #ifdef IMMOBILIZE_SURFACE_WITH_3LAYERS
                  // implementing
                  above_ground ? 0 : soil_layer
                  #endif
                  soil_layer
                  ]  //190513_060512
               ,decomposition_adjustment_factor);                                //060807
            if (org_BM->is_above_ground())
                  pot_C_decomp_surface[org_BM->position] += pot_C_decomp;
            else  pot_C_decomp_subsurface[soil_layer]    += pot_C_decomp;
         total_pot_C_decomp += pot_C_decomp;
      }
   } FOR_EACH_END(org_BM_iter)
   return total_pot_C_decomp ;
}
//_potential_C_decomposition___________________________________________________/
float64 Organic_matter_pools_common::actual_C_decomposition()
{  float64 total_act_C_decomp = 0.0;
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)
   {  nat8 layer = org_BM->is_above_ground()
         ? 1 // Above ground biomass exchanges matter with the soil surface
         : org_BM->layer ;
      {  float64 act_C_decomp = org_BM->actual_C_decomposition
            (N_immobilization_demand_sum[layer]
            ,reduction_factor_subsurface[layer]);
/*debug
if ((layer==1)) // && (CS::simdate_debug==1710175))
{
actual_decomp
<< CS::simdate_debug <<  '\t'
<< (int)org_BM->carbon_cycling << '\t'
<< act_C_decomp
<< "\tRF=" << reduction_factor_subsurface[layer]
<< "\tND:" << N_immobilization_demand_sum[layer] << '\t'
<< std::endl;
}
*/
         act_C_decomposed_from_all[org_BM->position] += act_C_decomp;            //060817
         if (!org_BM->is_above_ground())
            act_C_decomp_subsurface[layer]    += act_C_decomp;
         if (org_BM->is_straw_or_manure_residue())                               //060817
         {  // In think this is just output
            act_C_decomposed_from_residue[org_BM->position] += act_C_decomp;
         }
/*
else
{
if (layer == 1)
{
std::clog << "som:1=" << org_BM->kg() << std::endl;
}
}
*/
         total_act_C_decomp += act_C_decomp;
      } // else nothing demanded for decomposition
   } FOR_EACH_END(org_BM_iter)
   transfer_carbon_and_nitrogen_between_pools();                                 //110928
   return total_act_C_decomp ;
}
//_actual_C_decomposition______________________________________________________/
float64 Organic_matter_pools_common::determine_reduction_factor

// rename this to determine_reduction_factor subsurface

(nat8 residue_horizon
,float64 soil_layer_NH4_N_mass
,float64 soil_layer_NO3_N_mass
,float64 N_immobilization_demand_sum
,float64 &N_deficit_for_immobilization)   // returned
{
   float64 decomposition_reduction_factor = 1.0;
   nat8 layer = residue_horizon?residue_horizon : (nat8)1;
   if (N_immobilization_demand_sum > 0.0)
   {
      N_deficit_for_immobilization = N_immobilization_demand_sum;
      //Check if enough soil NH4 is available for immobilization
      immobilized_NH4_N[layer] = 0.0;
      if (N_deficit_for_immobilization < soil_layer_NH4_N_mass)
      {  immobilized_NH4_N[layer] = N_deficit_for_immobilization;
         N_deficit_for_immobilization = 0.0;
      } else
      {  immobilized_NH4_N[layer] = soil_layer_NH4_N_mass;
         N_deficit_for_immobilization -= immobilized_NH4_N[layer];
      }
      // If nitrogen immobilization demand remains positive
      // check if enough soil NO3 is available for immobilization
      immobilized_NO3_N[layer]  = 0.0;
      if (N_deficit_for_immobilization > 0.0)
      {  if (N_deficit_for_immobilization < soil_layer_NO3_N_mass)
         {  immobilized_NO3_N[layer] = N_deficit_for_immobilization;
            N_deficit_for_immobilization = 0.0;
         } else
         {  immobilized_NO3_N[layer] = soil_layer_NO3_N_mass;
            N_deficit_for_immobilization -= immobilized_NO3_N[layer];
         }
      }
      // If not enough soil mineral nitrogen is available to support
      // immobilization demand, then reduce the amount of decomposition.
      float64 diff = N_immobilization_demand_sum - N_deficit_for_immobilization;
      decomposition_reduction_factor
         = diff / N_immobilization_demand_sum;
   }
   return  decomposition_reduction_factor;
}
//_determine_reduction_factor____________________________________________2006?_/
#ifdef IMMOBILIZE_SURFACE_WITH_3LAYERS
// 190513 Claudio wants the surface immobilization to be satisfied by the first 3 layers
// This seems not senible to me so this is probably a hack.
float64 Organic_matter_pools_common::determine_reduction_factor_surface
(
/*
nat8 residue_horizon
,xxx soil_NH4_mass_profile
,xxx soil_NO3_mass_profile
,
*/
float64 N_immobilization_demand_sum_surface
//,float64 &N_deficit_for_immobilization
)   // returned
{
   float64 decomposition_reduction_factor = 1.0;
   float64 N_immobilization_demand_sum_surface_unsatisfied = N_immobilization_demand_sum_surface;
   bool satisfied = false;
   for (nat8 layer = 1
       ; (layer <= 3) && N_immobilization_demand_sum_surface_unsatisfied > 0.00001
       ; layer++)
   {
      float64 &N_deficit_for_immobilization/*layer*/ = N_deficit_for_immobilization_subsurface[layer];

      // At this point we have determined any immobilization for residues actually in the layer.

      soil_layer_NH4_N_mass = soil_N->get_NH4_N_mass_kg_m2(layer);
      // We haven't deducted subsurface residue immobolization yet
      // so determine the amount potentially available.
      float64 soil_layer_NH4_N_mass_avail = soil_layer_NH4_N_mass - immobilized_NH4_N[layer];
      // would be 0.0 if actual subsurface residue immobilized everything.

      // Perhaps there would be some NH4 N left in the layer for surface residues
      if (N_immobilization_demand_sum_surface_unsatisfied < soil_layer_NH4_N_mass_avail)
      {
      {  immobilized_NH4_N[layer] += N_immobilization_demand_sum_surface_unsatisfied;
         N_deficit_for_immobilization = 0.0;
         N_immobilization_demand_sum_surface_unsatisfied = 0.0;
      } else
      {  // use up all the remaining NH4
         immobilized_NH4_N[layer] += soil_layer_NH4_N_mass_avail;
         N_deficit_for_immobilization -= soil_layer_NH4_N_mass_avail;
         N_immobilization_demand_sum_surface_unsatisfied -= soil_layer_NH4_N_mass_avail;
      }

      // If N_immobilization_demand_sum_surface_unsatisfied demand remains positive
      // check if enough soil NO3 is available for immobilization
      soil_layer_NO3_N_mass = soil_N->get_NO3_N_mass_kg_m2(layer);
      // We haven't deducted subsurface residue immobolization yet
      // so determine the amount potentially available.
      float64 soil_layer_NO3_N_mass_avail = soil_layer_NO3_N_mass - immobilized_NO3_N[layer];
      // would be 0.0 if actual subsurface residue immobilized everything.

      // Perhaps there would be some NH4 N left in the layer for surface residues


      if (N_immobilization_demand_sum_surface_unsatisfied > 0.0)
      {  if (N_immobilization_demand_sum_surface_unsatisfied < soil_layer_NO3_N_mass_avail)
         {  immobilized_NO3_N[layer] += N_immobilization_demand_sum_surface_unsatisfied;
            N_deficit_for_immobilization = 0.0;
            N_immobilization_demand_sum_surface_unsatisfied = 0.0;
         } else
         {  immobilized_NO3_N[layer] += soil_layer_NO3_N_mass_avail;
            N_deficit_for_immobilization -= soil_layer_NO3_N_mass_avail;
            N_immobilization_demand_sum_surface_unsatisfied -= soil_layer_NO3_N_mass_avail;
         }
      }

I'm stuck here again because we already have calculated
decomposition_reduction_factor
and would need to recaculated it, but I don't have N_immobilization_demand_sum


      // If not enough soil mineral nitrogen is available to support
      // immobilization demand, then reduce the amount of decomposition.
      float64 diff = N_immobilization_demand_sum - N_deficit_for_immobilization;
      decomposition_reduction_factor
         = diff / N_immobilization_demand_sum;

   }
   return  decomposition_reduction_factor;
}
//_determine_reduction_factor_surface_______________________________2019-05-13_/
#endif
bool Organic_matter_pools_common::take_pools
(Unidirectional_list &pools)                                       modification_
{
      // This is used by respond_to_field_operation() (both for OM and residue pools)
      // When residues are redistributes, new pools may be created.
      // This adds the new pools.
   transfer_all_from(pools);
   return true;
}
//_take_pools___________________________________________2011-01-04__2007-08-08_/
bool Organic_matter_pools_common::clear_residues()                 modification_
{  nat16 residue_pools_cleared = 0;
   FOR_EACH(org_BM,Organic_biomatter_pool_common,org_BM_iter)
   {  if (org_BM->is_straw_or_manure_residue())
      {  org_BM_iter->delete_current();
         residue_pools_cleared++;
      }
   } FOR_EACH_END(org_BM_iter)
   return true;
}
//_clear_residues___________________________________________________2016-01-26_/
bool Organic_matter_pools_common::read(std::istream &stream)
{  bool read_success = false;
   int start_record_pos = stream.tellg();
   std::string header;
   std::getline(stream,header);
   start_record_pos = stream.tellg();
   while (stream.good() && !stream.eof())
   {  start_record_pos = stream.tellg(); //      remember file position
      nat16 type_buff;
      stream >> type_buff;                 //      read type
      if (type_buff <= ORGANIC_MATTER_TYPE_COUNT) // Some times we have garbage at end of file //121009
      {
         Organic_biomatter_pool_common *OM_pool
            =
            /* Might need to instanciate residue pool separately because
               single pool OM has its own nitrogen org store
            (type_buff <= residue)                                               //170302
            ? render_residue_pool_for_read() :                                    //170302
            */
             render_OM_pool_for_read();
         stream.seekg(start_record_pos,std::ios::beg);
         OM_pool->read(stream);
         if (CORN::is_zero<float64>(OM_pool->kg()/*200127 ,0.0,0.000001*/))      //170228
              delete OM_pool;                                                    //170228
              // It is possible there is a blank record and the end of the file
              // which shows up with zero mass
         else take_OM_pool(OM_pool);
      } // is a valid OM type
      read_success = true;
   } // while not end of stream
   return read_success;
}
//_read_____________________________________________________________2016-11-16_/
bool Organic_matter_pools_common::take_OM_pool
(Organic_biomatter_pool_common *OM_pool)                          appropriation_
{  bool took = true;
   if (is_applicable(OM_pool->type))                                             //170301
   {  Organic_biomatter_common *existing_OM = find_similar_organic_biomatter(*OM_pool); //121009
      if (existing_OM)
      {  existing_OM->update_value_with_units(OM_pool->kg(),UC_kg_mass);         //150227
         existing_OM->set_carbon_nitrogen_ratio(OM_pool->get_carbon_nitrogen_ratio());
         delete OM_pool; OM_pool = 0;
         took = true; // We took it an deleted it because similar already existed
      } else
      {
         append(OM_pool);
         nat8 layer = OM_pool->layer;
         /* Dont delete: This message needs to be an explination

         if (organic_matter[OM_pool->type][layer])
         {
            std::clog << "Duplicate OM pool loaded:" << (nat16)(OM_pool->type) << " layer:" << layer << std::endl;
            // this might happen with residues, residues should be consolidated
            // when writing historical OM pool files.
            //170228 organic_matter[OM_pool->type][layer] = OM_pool;
            //170228 Not sure why I was deleting in this case delete OM_pool;
         }
         */
         organic_matter[OM_pool->type][layer] = OM_pool;
         specialized_reference(OM_pool);
         // I am pretty sure abiotic conditions are applicable to both surface and subsurface. 170228
         // (check that there is indeed abiotics for layer 0 (surface)           170228

         OM_pool->know_soil_abiotic_environment                                  //190219
            (soil_abiotic_environment_profile
            .ref_soil_abiotic_environment_layer(layer));
         /*190219
         OM_pool->soil_abiotic_env_layer = soil_abiotic_environment_profile
            .ref_soil_abiotic_environment_layer(layer);
         */
         took = true;
      }
   }
   return took;
}
//_take_OM_pool_________________________________________2017-02-28__2016-11-16_/
CORN::Date_clad_32 dummy_date;
RENDER_INSPECTORS_DEFINITION(Organic_matter_pools_common)
{
   #ifdef CS_OBSERVATION
   KNOW_EMANATOR(dummy_date,CS_NOT_EPHEMERAL);                                   //171114_
         // Big warning, currently Organic_matter_pools_common is not derived from
         // simulation element,  once it is, should pass today raw 171115

      // probably need date

   emanator_known = &emanator;
   inspectors.append(new CS::Inspector_scalar(today_OM_mineralized_kgN_m2,UC_kg_m2,emanator,/*check ontology*/"N_mineralization/profile"   ,value_statistic,CSVP_organic_matter_N_mineralization_profile/*171221 CSVC_organic_matter_N_mineralization_profile_time_step_sum*/ ));
   // Currently there is only mineralization from SOM

///*  These have not previously been output
   inspectors.append(new CS::Inspector_scalar(today_N_immobilized        ,UC_kg_m2,emanator,"N_immobilization"   ,value_statistic,CSVP_soil_N_immobilization_profile/*171222 CSVC_soil_N_immobilization_time_step_sum*/ ));
   /*conceptual
   inspectors.append(new CS::Inspector_scalar(today_NO3_N_immobilized    ,UC_kg_m2,emanator,"N_immobilization_NO3"   ,value_statistic,CSVP_soil_N_immobilization_NO3));
   inspectors.append(new CS::Inspector_scalar(today_NH4_N_immobilized    ,UC_kg_m2,emanator,"N_immobilization_NH4"   ,value_statistic,CSVP_soil_N_immobilization_NH4));
   */
   //inspectors.append(new CS::Inspector_scalar(today_N_immobilized        ,UC_kg_m2,emanator,"N_immobilization/residue/manure/profile"   ,value_statistic,CSVP_soil_N_immobilization));
//*/

   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_all      [OM_subsurface_position]        ,UC_kg_m2,emanator,/*check ontology*/"decomposition/OM&residue/subsurface/C"  ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_all      [OM_flat_surface_position]      ,UC_kg_m2,emanator,/*check ontology*/"decomposition/OM&residue/flat/C"        ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_all      [OM_stubble_surface_position]   ,UC_kg_m2,emanator,/*check ontology*/"decomposition/OM&residue/stubble/C"     ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_all      [OM_attached_to_plant_position] ,UC_kg_m2,emanator,/*check ontology*/"decomposition/OM&residue/stubble/C"     ,value_statistic));

   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_residue  [OM_subsurface_position]        ,UC_kg_m2,emanator,/*check ontology*/"decomposition/residue/subsurface/C"     ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_residue  [OM_flat_surface_position]      ,UC_kg_m2,emanator,/*check ontology*/"decomposition/residue/flat/C"           ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_residue  [OM_stubble_surface_position]   ,UC_kg_m2,emanator,/*check ontology*/"decomposition/residue/stubble/C"        ,value_statistic));
   inspectors.append(new CS::Inspector_scalar(act_C_decomposed_from_residue  [OM_attached_to_plant_position] ,UC_kg_m2,emanator,/*check ontology*/"decomposition/residue/attached/C"       ,value_statistic));

   inspectors.append(new CS::Inspector_scalar(CO2_C_loss_daily[0],UC_kg_m2,emanator,/*check ontology*/"CO2_C_loss/OM"     ,value_statistic,CSVP_organic_matter_CO2_C_loss_residue/*17122 CSVC_organic_matter_CO2_C_loss_residue_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(CO2_C_loss_daily[1],UC_kg_m2,emanator,/*check ontology*/"CO2_C_loss/SOM"    ,value_statistic,CSVP_organic_matter_CO2_C_loss_SOM    /*17122 CSVC_organic_matter_CO2_C_loss_SOM_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(CO2_C_loss_daily[2],UC_kg_m2,emanator,/*check ontology*/"CO2_C_loss/residue",value_statistic,CSVP_organic_matter_CO2_C_loss_OM     /*17122 CSVC_organic_matter_CO2_C_loss_residue_time_step_sum*/));

   inspectors.append(new CS::Inspector_scalar(C_incorporated_by_tillage,UC_kg_m2,emanator,/*check ontology*/"incorporation/C",value_statistic));

   nat8 layers = soil_layers.count();

   if (CS::optation_global.is_desired("immobilization/NO3_N"))
      inspectors.append(new CS::Inspector_vector
      (immobilized_NO3_N,layers
      ,UC_kg_m2,*context,"immobilization/NO3_N",value_statistic));
   if (CS::optation_global.is_desired("immobilization/NH4_N"))
      inspectors.append(new CS::Inspector_vector
      (immobilized_NH4_N,layers
      ,UC_kg_m2,*context,"immobilization/NH4_N",value_statistic));
   if (CS::optation_global.is_desired("immobilization/NO3/N"))
      inspectors.append(new CS::Inspector_vector
      (immobilized_NO3_N,layers
      ,UC_kg_m2,*context,"immobilization/NO3/N",value_statistic));
   if (
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
        (CS::optation_global.is_desired(CSVC_soil_N_mineralization_time_step_sum)) ||
      #endif
        (CS::optation_global.is_desired("mineralization&total&subsurface")))
      inspectors.append(new CS::Inspector_vector
      (immobilized_NH4_N,layers,UC_kg_m2,*context
      ,"mineralization&total&subsurface",value_statistic
      ,CSVC_soil_N_mineralization_time_step_sum));
   if (CS::optation_global.is_desired("CO2_release/SOM&residue/subsurface/C"))
      inspectors.append(new CS::Inspector_vector
      (CO2_C_loss_subsurface_residue_and_OM,layers
      ,UC_kg_m2    ,*context,"CO2_release/SOM&residue/subsurface/C",value_statistic));
   if (CS::optation_global.is_desired("CO2_release/OM/subsurface/C"))
      inspectors.append(new CS::Inspector_vector
      (CO2_C_loss_subsurface_OM,layers
      ,UC_kg_m2  ,*context,"CO2_release/OM/subsurface/C",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("immobilization/demand/N")))
      inspectors.append(new CS::Inspector_vector
      (N_immobilization_demand_sum/*_subsurface*/,layers
      ,UC_kg_m2,*context,"immobilization/demand/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("mineralization/residue/N")))
      inspectors.append(new CS::Inspector_vector
      (mineralized_N_residue,layers
      ,UC_kg_m2,*context,"mineralization/residue/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("mineralization/SOM/N")))
      inspectors.append(new CS::Inspector_vector
      (mineralized_N_OM,layers
      ,UC_kg_m2,*context,"mineralization/SOM/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("mineralization/SOM&residue/N")))
      inspectors.append(new CS::Inspector_vector
      (mineralized_N_OM_residue,layers
      ,UC_kg_m2,*context,"mineralization/SOM&residue/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("mineralization/residue/N")))
      inspectors.append(new CS::Inspector_vector
      (mineralized_N_residue,layers
      ,UC_kg_m2,*context,"mineralization/residue/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("decomposition/actual/C")))
      inspectors.append(new CS::Inspector_vector
      (act_C_decomp_subsurface,layers
      ,UC_kg_m2,*context,"decomposition/actual/C",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("immobilization/deficit/N")))
      inspectors.append(new CS::Inspector_vector
      (N_deficit_for_immobilization_subsurface,layers
      ,UC_kg_m2,*context,"immobilization/deficit/N",value_statistic));
   if (
      //#if ((CS_VERSION > 0) &&(CS_VERSION < 6))
      //  (CS::optation_global.is_desired(CSVC_xxxx)) ||
      //#endif
        (CS::optation_global.is_desired("decomposition/reduction")))
      inspectors.append(new CS::Inspector_vector
      (reduction_factor_subsurface,layers
      ,UC_factor,*context,"decomposition/reduction",value_statistic));

   return emanator_known;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2017-11-15_/
//#endif

