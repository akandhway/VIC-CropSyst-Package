
// This module only needs to be included if using variable recorders

// This file uses 8 bit characters in literal text strings

#include "csvc.h"
#include "CS_suite/observation/CS_inspector.h"
#include "CS_suite/observation/CS_examination.h"

#include "crop/crop_cropsyst.h"
#include "soil/chemicals_profile.h"
#include "soil/nitrogen_profile.h"
#include "soil/infiltration_I.h"
#include "soil/hydrology_I.h"
#include "soil.h"
#include "organic_matter/OM_pools_common.h"
#include "cs_ET.h"
#include "UED/library/std_codes.h"
#include "land_unit_sim.h"
#include "soil/soil_evaporator.h"

#define CSVC_UNKNOWN 0

#define TS_VALUE value_statistic
// Representative value for timestep.
// It is may be the cumulative value for the time step (I.e. the current day)
// and/or total for components,
// but not considered a precalculated statistic.

// template
// inspectors.append(new CS::Inspector(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVC_XXXX));

/* NYI
Annual

[ ]CSVC_mgmt_N_organic_gaseous_loss_period_sum          ,UC_kg_ha}
[ ]CSVC_mgmt_N_inorganic_gaseous_loss_period_sum        ,UC_kg_ha}

[ ]CSVC_soil_C_SOM_profile                              ,UC_kg_ha}
[ ]CSVC_soil_C_SOM_30cm                                 ,UC_kg_ha}

Seasonal

[ ]Duration
*/

//_____________________________________________________________________________/

/*
nitrogen

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_yield));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_useful));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_disposal));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_chaff));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_stubble));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_grazing_to_manure));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_grazing_to_disposal));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_uncut_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_non_yield));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_residue));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_grazing));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_roots_live));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_roots_dead));
*/
/*
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_above_ground));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_grazing));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_mass_removed_residue));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_yield));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_useful));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_disposal));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_chaff));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_stubble));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_grazing_to_manure));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_grazing_to_disposal));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_uncut_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_canopy));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_non_yield));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_residue));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_grazing));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_roots_live));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_roots_dead));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_produced_above_ground));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_above_ground));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_grazing));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_crop_N_conc_removed_residue));
*/
//______________________________________________________________________________
/*template
namespace CropSyst {
RENDER_INSPECTORS_DEFINITION(XXXXX)
{
// inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_XXXX));
// inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_XXXX));
// inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_XXXX));
   return 0; // emanator;
}

//______________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/
*/
//_Crop_nitrogen::render_inspectors_________________________________2013-07-30_/

/* NYI

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_water_drainage));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_water_depletion));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_water_balance));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_erosion_clod_rate));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_erosion_loss));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_erosion_loss));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_erosion_index));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_erosion_RUSLE_C_factor));

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_runoff_surface_water_runon));
CSVP_soil_C_SOM_profile
CSVP_soil_C_SOM_5cm
CSVP_soil_C_SOM_10cm
CSVP_soil_C_SOM_15cm
CSVP_soil_C_SOM_30cm
CSVP_soil_C_SOM_60cm
CSVP_soil_C_SOM_90cm
CSVP_soil_C_microbial_profile
CSVP_soil_C_microbial_5cm
CSVP_soil_C_microbial_10cm
CSVP_soil_C_microbial_15cm
CSVP_soil_C_microbial_30cm
CSVP_soil_C_microbial_60cm
CSVP_soil_C_microbial_90cm
CSVP_soil_C_labile_profile
CSVP_soil_C_labile_5cm
CSVP_soil_C_labile_10cm
CSVP_soil_C_labile_15cm
CSVP_soil_C_labile_30cm
CSVP_soil_C_labile_60cm
CSVP_soil_C_labile_90cm
CSVP_soil_C_metastable_profile
CSVP_soil_C_metastable_5cm
CSVP_soil_C_metastable_10cm
CSVP_soil_C_metastable_15cm
CSVP_soil_C_metastable_30cm
CSVP_soil_C_metastable_60cm
CSVP_soil_C_metastable_90cm
CSVP_soil_C_passive_profile
CSVP_soil_C_passive_5cm
CSVP_soil_C_passive_10cm
CSVP_soil_C_passive_15cm
CSVP_soil_C_passive_30cm
CSVP_soil_C_passive_60cm
CSVP_soil_C_passive_90cm
CSVP_soil_C_residues_surface
CSVP_soil_C_residues_profile
CSVP_soil_C_residues_5cm
CSVP_soil_C_residues_10cm
CSVP_soil_C_residues_15cm
CSVP_soil_C_residues_30cm
CSVP_soil_C_residues_60cm
CSVP_soil_C_residues_90cm
CSVP_soil_C_residues_surface
CSVP_soil_C_residues_profile
CSVP_soil_C_residues_5cm
CSVP_soil_C_residues_10cm
CSVP_soil_C_residues_15cm
CSVP_soil_C_residues_30cm
CSVP_soil_C_residues_60cm
CSVP_soil_C_residues_90cm
CSVP_soil_C_decomposed_residue_profile
CSVP_soil_C_decomposed_residue_5cm
CSVP_soil_C_decomposed_residue_10cm
CSVP_soil_C_decomposed_residue_15cm
CSVP_soil_C_decomposed_residue_30cm
CSVP_soil_C_decomposed_residue_60cm
CSVP_soil_C_decomposed_residue_90cm
CSVP_soil_C_decomposed_residue_flat
CSVP_soil_C_decomposed_residue_stubble
CSVP_soil_C_decomposed_residue_attached
CSVP_soil_C_incorporated_by_tillage
CSVP_soil_C_removed_by_harvest
CSVP_soil_C_decomposed_SOM_profile
CSVP_soil_C_decomposed_SOM_5cm
CSVP_soil_C_decomposed_SOM_10cm
CSVP_soil_C_decomposed_SOM_15cm
CSVP_soil_C_decomposed_SOM_30cm
CSVP_soil_C_decomposed_SOM_60cm
CSVP_soil_C_decomposed_SOM_90cm

// soil nitrogen

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_N_available));

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_N_input));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_N_output));
// Soil pond

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_water_content)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_plant_avail_water)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_water_potential)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_lateral_in_flow_depth)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_lateral_out_flow_depth)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_temperature));   1-31

   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_N_mineralization)); 1-31));


   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_P_P_amount)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_salinity)); 1-31
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_soil_base_salt)); 1-31
*/
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
/*200509 moved to soil
RENDER_INSPECTORS_DEFINITION(CropSyst::Soil_components)
{
   emanator_known
      = Soil_base::render_inspectors(inspectors,context,instance);
   if (chemicals) chemicals->render_inspectors(inspectors,emanator_known,"chemical");
   //NYI if (erosion) erosion->render_inspectors(inspectors,&emanator,"erosion");
   //NYI if (tillage_effect) tillage_effect->render_inspectors(inspectors,&emanator,"tillage_effect");
   return emanator_known; // emanator;
}
*/
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-11_/

