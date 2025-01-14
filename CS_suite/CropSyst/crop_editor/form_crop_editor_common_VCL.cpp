//Method implementations common to both the CropSyst V4 and V5 soil editor form
#if (__BCPLUSPLUS__>0x600)
#define _UNICODE
#endif
#include <corn/data_source/vv_file.h>
#include "CropSyst/source/cs_filenames.h"
#include "CropSyst/source/crop/crop_types.h"
#include "CS_suite/application/CS_suite_directory.h"

#  include "crop_editor_frm.h"
#  include "CropSyst/source/crop/crop_param_V5.h"
#  define residue_decomposition_ residue_decomposition.
   using namespace CropSyst;

#include "corn/math/compare.hpp"

#include "corn/application/program_directory_OS_FS.h"
#include "corn/OS/file_system_engine.h"
#include "corn/application/documentor.h"

//______________________________________________________________________________
void __fastcall TCrop_editor_form::bind_to_V4_and_V5(CropSyst_Crop_parameters *_crop,Tparameter_file_form *_parameter_form,const char *_crop_model)
{
   CORN::documentor.know_documentation_directory(&program_directory->Documentation()); //131118
   crop_model = _crop_model;
   crop = _crop;
   crop->parameter_editor_mode = true;
   parameter_file_form = _parameter_form;
      if (crop_model)
      {
         // we don't instanciate crop_filename so that Ok handler will
         // make sure to prompt for a filename.
         crop->crop_model_clad.set_label(crop_model);
      }
      else
      {  // it must be a file name
      }
   button_calibrate->Visible = true;

   // Classification
   radiogroup_landuse                     ->bind_to(&(crop->land_use_clad) V4_HELP_URL(HELP_P_crop_landuse));
   radiogroup_photosynthetic              ->bind_to(&(crop->photosynthetic_pathway_clad) V4_HELP_URL(HELP_P_crop_photosynthetic_pathway));
   radiogroup_life_cycle                  ->bind_to(&(crop->life_cycle_clad) V4_HELP_URL(HELP_P_crop_life_cycle));
   radiogroup_harvested_biomass           ->bind_to(&(crop->harvested_part_clad) V4_HELP_URL(HELP_P_crop_harvest_class));


// This checkbox has been moved to the parameter form
//   checkbox_advanced                      ->bind_to(&(crop->advanced_mode),0/*NO HELP YET*/);
   parameter_file_form->advanced_mode_checkbox();

   edit_TUE_at_1kPa_VPD_vegetative     ->bind_to(&(crop->biomass_production.TUE_at_1kPa_VPD[1])     ,2     V4_HELP_URL(HELP_P_crop_TUE_at_1kPa_VPD) );

   checkbox_WUE_changes          ->bind_to(&(crop->biomass_production.WUE_changes)/*NO_HELP_YET_*/);
   edit_WUE_change_deg_day       ->bind_to(&(crop->phenology.p_WUE_change_deg_day)/*NO_HELP_YET_*/);
   edit_TUE_scaling_coef_vegetative ->bind_to(&(crop->biomass_production.TUE_scaling_coef[1]),2 V4_HELP_URL(HELP_P_crop_TUE_scaling_coef                ));
                                                                                                                          // 080428 Warning, I need new help codes for these new parameters
   edit_TUE_at_1kPa_VPD                ->bind_to(&(crop->biomass_production.p_TUE_at_1kPa_VPD)            V4_HELP_URL(HELP_P_crop_TUE_at_1kPa_VPD                ));
   /*191026 AquaCrop and Tanner Sinclair now obsolete
   edit_water_productivity             ->bind_to(&(crop->biomass_production.p_water_productivity)         V4_HELP_URL(HELP_P_crop_water_productivity             ));
   edit_reference_biomass_transp_coef_0->bind_to(&(crop->biomass_production.p_biomass_transpiration_coef) V4_HELP_URL(HELP_P_crop_k_Tanner_Sinclair              ));
   */

   bind_to_vernalization();
   // Photoperiod
//   has_photoperiod = crop->photoperiod;

   bind_to_photoperiod();

   // Harvest Index
   // The same translocation factor is used for grain and fruit
   bind_to_harvest_index();

   // Fruit
   bool has_fruit = true;
   if (has_fruit) bind_to_fruit();

   bool has_residue_decomposition = true;
   radiogroup_RULSE2_residue_type            ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ RUSLE2_type_clad)                   : 0,0);
   edit_fast_cycling_top_fraction            ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ fast_cycling_top_fraction)           : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);
   edit_slow_cycling_top_fraction            ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ slow_cycling_top_fraction)           : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);
   edit_lignified_top_fraction               ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ lignified_top_fraction)              : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);
   edit_fast_cycling_root_fraction           ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ fast_cycling_root_fraction)          : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);
   edit_slow_cycling_root_fraction           ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ slow_cycling_root_fraction)          : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);
   edit_lignified_root_fraction              ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ lignified_root_fraction)             : 0,2 V4_HELP_URL(HELP_P_crop_residue_cycling_biomass_fraction) /*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_cycling_biomass_fraction).c_str()*/);

   top_biomass_checksum ->bind_to(&top_checksum,2 /*,0 NO_HELP_YET_*/);
   root_biomass_checksum->bind_to(&root_checksum,2 /*,0 NO_HELP_YET_*/);


   // rename these to halflife
   edit_fast_cycling_halflife   ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ fast_cycling_halflife)  : 0,2 V4_HELP_URL(HELP_P_crop_residue_decomp_time)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_decomp_time).c_str()*/);
   edit_slow_cycling_halflife   ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ slow_cycling_halflife)  : 0,2 V4_HELP_URL(HELP_P_crop_residue_decomp_time)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_decomp_time).c_str()*/);
   edit_lignified_halflife      ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ lignified_halflife)     : 0,2 V4_HELP_URL(HELP_P_crop_residue_decomp_time)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_decomp_time).c_str()*/);
   //deprecated in V4 edit_decomposition_time_50                ->bind_to(has_residue_decomposition ? &(crop->residue_decomposition_ decomposition_time_50)               : 0 V4_HELP_URL(HELP_P_crop_residue_decomp_time)/*,parameter_file_form->compose_manual_subdirectory_filename(HELP_P_crop_residue_decomp_time).c_str()*/);


   // Transpiration
   edit_ET_crop_coef                      ->bind_to(&(crop->transpiration.p_ET_coef)              V4_HELP_URL(HELP_P_crop_transpiration_ET_crop_coef));
   edit_leaf_water_pot_stomatal_closure   ->bind_to(&(crop->transpiration.p_stomatal_closure_LWP) V4_HELP_URL(HELP_P_crop_transpiration_crit_leaf_water_pot));
   edit_wilt_leaf_water_pot               ->bind_to(&(crop->transpiration.p_wilt_LWP)             V4_HELP_URL(HELP_P_crop_transpiration_wilt_leaf_water_pot));
   edit_max_water_uptake_mm               ->bind_to(&(crop->transpiration.p_max_water_uptake)     V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));

   // Leaf
   edit_max_LAI                           ->bind_to(&(crop->canopy_growth_LAI_based.p_max_LAI)              V4_HELP_URL(HELP_P_crop_leaf_max_LAI));
   edit_initial_GAI                       ->bind_to(&(crop->canopy_growth_LAI_based.p_initial_GAI)          V4_HELP_URL(HELP_P_crop_leaf_initial_GAI));
   edit_regrowth_GAI                      ->bind_to(&(crop->canopy_growth_LAI_based.p_regrowth_GAI)         V4_HELP_URL(HELP_P_crop_leaf_regrowth_GAI));
   edit_specific_leaf_area                ->bind_to(&(crop->canopy_growth_LAI_based.p_specific_leaf_area)   V4_HELP_URL(HELP_P_crop_leaf_spec_leaf_area));
   edit_stem_leaf_partition               ->bind_to(&(crop->canopy_growth_LAI_based.p_stem_leaf_partition)  V4_HELP_URL(HELP_P_crop_leaf_stem_part));
   edit_fract_LAI_mature                  ->bind_to(&(crop->canopy_growth_LAI_based.p_fract_LAI_mature)     V4_HELP_URL(HELP_P_crop_leaf_fract_max_LAI_mature));
   // canopy cover
   edit_canopy_cover_initial              ->bind_to(&(crop->canopy_growth_cover_based.p_cover_initial)            V4_HELP_URL(HELP_P_canopy_cover_initial));
   edit_canopy_cover_maximum              ->bind_to(&(crop->canopy_growth_cover_based.p_cover_maximum)            V4_HELP_URL(HELP_P_canopy_cover_maximum));
//   #ifdef SENESCENCE_REDUCTION_200124
//   edit_canopy_cover_total_season_end     ->Visible = false;
//   #else
   edit_canopy_cover_total_season_end     ->bind_to(&(crop->canopy_growth_cover_based.p_cover_total_season_end)       V4_HELP_URL(HELP_P_canopy_canopy_cover_mature)); //171219
//   #endif

   edit_canopy_cover_green_season_end     ->bind_to(&(crop->canopy_growth_cover_based.p_cover_green_season_end)       V4_HELP_URL(0)); //171219_171009
   //edit_accrescence_duration             ->bind_to(&(crop->canopy_growth_LAI_based.p_xxxx)            V4_HELP_URL(0));


   //190627 edit_thermal_time_season_end           ->bind_to(&(crop->canopy_growth_cover_based.p_thermal_time_season_end)       V4_HELP_URL(0)); //171219_171009
   edit_shape_accrescent                  ->bind_to(&(crop->canopy_growth_cover_based.p_shape_coef_accrescent)       V4_HELP_URL(0)); //171221
   edit_shape_senescent                   ->bind_to(&(crop->canopy_growth_cover_based.p_shape_coef_senescent)       V4_HELP_URL(0)); //171221
   edit_light_extinction_coef->bind_to(&(crop->morphology .p_light_extinction_coef) V4_HELP_URL(HELP_P_crop_transpiration_extinct_coef)); //191121

   // senescence
   edit_leaf_duration_sensitivity_stress  ->bind_to(&(crop->canopy_growth_LAI_based.p_leaf_duration_sensitivity_stress) V4_HELP_URL(HELP_P_crop_senescence_leaf_duration_sens));
   edit_deg_day_leaf_duration             ->bind_to(&(crop->canopy_growth_LAI_based.p_leaf_duration_deg_day)            V4_HELP_URL(HELP_P_crop_senescence_leaf_duration));
   radiogroup_abscission                  ->bind_to(&(crop->abscission_clad)      V4_HELP_URL(HELP_P_crop_senescence_senesced_biomass_fate));

   // Root
   edit_max_root_depth                    ->bind_to(&(crop->root.p_max_depth)                                  V4_HELP_URL(HELP_P_crop_root_max_root_depth));
   #if (CROPSYST_VERSION == 4)
   edit_root_length_per_unit_mass         ->bind_to(&(crop->root.p_length_per_unit_mass)                       V4_HELP_URL(HELP_P_crop_root_length_per_unit_mass));
   #endif

   edit_root_elongation_duration          ->bind_to(&(crop->phenology.p_root_elongation_deg_day)); //191121

   edit_surface_root_density              ->bind_to(&(crop->root.p_surface_density)                            V4_HELP_URL(HELP_P_crop_root_max_surface_density));
   edit_root_density_distribution_curvature->bind_to(&(crop->root.p_density_distribution_curvature)            V4_HELP_URL(HELP_P_crop_root_density_distribution_curvature));
   //191121 edit_max_root_depth_dd_obsolete                 ->bind_to(&(crop->phenology.p_max_root_depth_deg_day)                V4_HELP_URL(HELP_P_crop_root_max_root_depth_dd));
   edit_root_shoot_emergence_ratio        ->bind_to(&(crop->root.p_root_shoot_emergence_ratio)     /*NO_HELP_YET_*/); //130626
   edit_root_shoot_full_ratio             ->bind_to(&(crop->root.p_root_shoot_full_ratio)          /*NO_HELP_YET_*/); //130626
   edit_root_sensitivity_to_water_stress  ->bind_to(&(crop->root.p_sensitivity_to_water_stress)   /*NO_HELP_YET_*/);

   // Emergence
   radiogroup_emergence_model             ->bind_to(&(crop->emergence.emergence_model_cowl)        /*NO_HELP_YET_*/);  // NO_HELP_YET   emergence_model_clad
   edit_seedling_leaf_area                ->bind_to(&(crop->emergence.p_seedling_leaf_area)        /*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_germination_base_temperature      ->bind_to(&(crop->emergence.p_germination_base_temperature)/*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_mean_base_soil_water_pot          ->bind_to(&(crop->emergence.p_mean_base_soil_water_pot)  /*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_stdev_base_soil_water_pot         ->bind_to(&(crop->emergence.p_stdev_base_soil_water_pot) /*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_a                                 ->bind_to(&(crop->emergence.p_a)                         /*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_b                                 ->bind_to(&(crop->emergence.p_b)                         /*NO_HELP_YET_*/);  // NO_HELP_YET
   // Growth

   edit_seasonal_adjustment_2    ->bind_to(&(crop->biomass_production.p_season_biomass_adjustment_2)/*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_seasonal_adjustment_3    ->bind_to(&(crop->biomass_production.p_season_biomass_adjustment_3)/*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_seasonal_adjustment_4    ->bind_to(&(crop->biomass_production.p_season_biomass_adjustment_4)/*NO_HELP_YET_*/);  // NO_HELP_YET
   edit_seasonal_adjustment_5    ->bind_to(&(crop->biomass_production.p_season_biomass_adjustment_5)/*NO_HELP_YET_*/);  // NO_HELP_YET

//   edit_max_RUE_total                  ->bind_to(&(crop->biomass_production.p_RUE_max)    V4_HELP_URL(HELP_P_crop_RUE_max));


   edit_RUE_at_low_VPD              ->bind_to(&(crop->biomass_production.p_RUE_at_low_VPD)               /*NO_HELP_YET_*/);
   edit_RUE_at_low_total_solar_rad  ->bind_to(&(crop->biomass_production.p_RUE_at_low_total_solar_rad)   /*NO_HELP_YET_*/);
   edit_RUE_rate_decrease           ->bind_to(&(crop->biomass_production.p_RUE_efficiency_decrease_rate) /*NO_HELP_YET_*/);
   //191026 edit_RUE_global_basis   ->bind_to(&(crop->biomass_production.p_RUE_global)V4_HELP_URL(HELP_P_crop_RUE_max));

   //V4 only edit_max_RUE_PAR                    ->bind_to(&(crop->p_light_to_biomass_deprecated)   V4_HELP_URL(HELP_P_crop_RUE_max));

   edit_TUE_scaling_coef         ->bind_to(&(crop->biomass_production.p_TUE_scaling_coef)V4_HELP_URL(HELP_P_crop_TUE_scaling_coef                ));

   edit_LWP_that_reduces_canopy_expansion ->bind_to(&(crop->morphology.p_reduce_canopy_expansion_LWP)          V4_HELP_URL(HELP_P_crop_LWP_reduces_canopy_expansion));
   edit_LWP_that_stops_canopy_expansion   ->bind_to(&(crop->morphology.p_stop_canopy_expansion_LWP)            V4_HELP_URL(HELP_P_crop_LWP_stops_canopy_expansion));
   #if (CROPSYST_VERSION == 4)
   edit_opt_temp_for_growth               ->bind_to(&(crop->biomass_production.p_early_growth_limit_temp)      V4_HELP_URL(HELP_P_crop_growth_opt_temp));
   #endif

   edit_temp_limit_min  ->bind_to(&(crop->biomass_production.p_temp_limit_min )); //191026
   edit_temp_limit_low  ->bind_to(&(crop->biomass_production.p_temp_limit_low )); //191026
   edit_temp_limit_high ->bind_to(&(crop->biomass_production.p_temp_limit_high)); //191026
   edit_temp_limit_max  ->bind_to(&(crop->biomass_production.p_temp_limit_max )); //191026

   // Phenology
   checkbox_clipping_resets                  ->bind_to(&(crop->phenology.clipping_resets)                       V4_HELP_URL(HELP_P_crop_pheno_clipping_resets));
   edit_deg_day_emerge                       ->bind_to(&(crop->phenology.p_emergence_deg_day)                   V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   edit_deg_day_tuber_initiation             ->bind_to(&(crop->phenology.p_tuber_initiation_deg_day)            V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   //190627 edit_deg_day_end_vegetative_growth        ->bind_to(&(crop->phenology.p_max_canopy_deg_day  )           V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   edit_deg_day_begin_flower                 ->bind_to(&(crop->phenology.p_begin_flowering_deg_day)             V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   edit_deg_day_begin_filling_grain_or_fruit ->bind_to(&(crop->phenology.p_begin_filling_deg_day)               V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   edit_deg_day_begin_rapid_fruit_growth     ->bind_to(&(crop->phenology.p_begin_rapid_fruit_growth_deg_day)    V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   edit_deg_day_begin_maturity               ->bind_to(&(crop->phenology.p_begin_maturity_deg_day)              V4_HELP_URL(HELP_P_crop_pheno_thermal_time_req));
   //190627 edit_begin_senescence_deg_day             ->bind_to(&(crop->phenology.p_begin_senescence_deg_day)            V4_HELP_URL(HELP_P_canopy_cover_begin_senescence_deg_day));
   //190627 edit_full_senescence_deg_day              ->bind_to(&(crop->phenology.p_full_senescence_deg_day)             V4_HELP_URL(HELP_P_canopy_cover_begin_senescence_deg_day)); //131024

   // Canopy relative thermal times
   edit_accrescence_duration  ->bind_to(&(crop->phenology.p_canopy_accrescence_duration )/*NO_HELP_YET_*/); //190627
   edit_culminescence_duration->bind_to(&(crop->phenology.p_duration_canopy_culminescence)); //191120
   edit_senescence_duration   ->bind_to(&(crop->phenology.p_duration_canopy_senescence)  /*NO_HELP_YET_*/); //190721
//200125 obsolete    edit_senescence_initiation_obsolete  ->bind_to(&(crop->phenology.p_initiation_canopy_senescence_obsolete)/*NO_HELP_YET_*/); //190721

   checkbox_maturity_significant             ->bind_to(&(crop->phenology.maturity_significant)                  /*NO_HELP_YET_*/);
   edit_phenologic_sensitivity_water_stress  ->bind_to(&(crop->phenology.p_phenologic_sensitivity_water_stress) V4_HELP_URL(HELP_P_crop_pheno_water_stress_sensitivity));
   // Thermal time accumulation mode
   edit_base_temp             ->bind_to(&(crop->thermal_time.p_base_temperature)  V4_HELP_URL(HELP_P_crop_thermal_time_accum_base_temp));
   edit_cutoff_temp           ->bind_to(&(crop->thermal_time.p_cutoff_temperature)V4_HELP_URL(HELP_P_crop_thermal_time_accum_cutoff_temp));
   edit_opt_temp              ->bind_to(&(crop->thermal_time.p_opt_temperature)   V4_HELP_URL(HELP_P_crop_thermal_time_accum_base_temp));
   edit_max_temp              ->bind_to(&(crop->thermal_time.p_max_temperature)   V4_HELP_URL(HELP_P_crop_thermal_time_accum_max_temp));
/*140206 temp disabled crashing
   edit_linger->bind_to(&(crop->p_linger_days) ,0);
*/

   bind_to_nitrogen();
   bind_to_salinity();
   bind_to_CO2_response();
   bind_to_dormancy();
   bind_to_hardiness();
   bind_to_orchard();
   // CropGro
   /* Not currently in V5
   if (crop->cropgro)
   cropgro_input_filebar->bind_to(&(crop->cropgro->model_input_file));
   */
}
//_bind_to_V4_and_V5___________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_photoperiod()
{  radiogroup_photoperiod_consideration->bind_to(&(crop->photoperiod.photoperiodism_cowl)V4_HELP_URL(HELP_P_crop_photoperiod_consideration));
   edit_stop_daylength                 ->bind_to(&(crop->photoperiod.p_stop)             V4_HELP_URL(HELP_P_crop_photoperiod));
   edit_constrained_daylength          ->bind_to(&(crop->photoperiod.p_unconstrain)      V4_HELP_URL(HELP_P_crop_photoperiod));
}
//_bind_to_photoperiod_________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_fruit()
{
   edit_fruit_fract_total_solids          ->bind_to(&(crop->fruit.p_fract_total_solids)               V4_HELP_URL(HELP_P_crop_fract_total_solids));
   edit_fruit_load_max                    ->bind_to(&(crop->fruit.p_max_fruit_load_kg_ha)             V4_HELP_URL(HELP_P_crop_max_fruit_load));
   #if (CS_VERSION==4)
   development_fract_biomass_edit_obsolete->bind_to(&(crop->fruit.p_initial_growth_fraction_to_fruit) V4_HELP_URL(HELP_P_crop_development_fraction_to_fruit));
   veraison_fract_biomass_edit_obsolete   ->bind_to(&(crop->fruit.p_rapid_growth_fraction_to_fruit)   V4_HELP_URL(HELP_P_crop_veraison_fraction_to_fruit));
   #endif
   checkbox_grape                         ->bind_to(&(crop->fruit.grape)                              V4_HELP_URL(HELP_P_crop_model));

   radiogroup_fruit_harvest_condition     ->bind_to(&(crop->fruit.harvest_condition_mode_cowl)        V4_HELP_URL(HELP_P_crop_model));
   edit_harvest_temperature               ->bind_to(&(crop->fruit.p_harvest_temperature)              V4_HELP_URL(HELP_P_crop_model));
   edit_harvest_temperature_days          ->bind_to(&(crop->fruit.p_harvest_temperature_days)         V4_HELP_URL(HELP_P_crop_model));
   edit_harvest_DOY_earliest              ->bind_to(&(crop->fruit.p_harvest_DOY_earliest)             V4_HELP_URL(HELP_P_crop_model));
   edit_harvest_DOY_latest                ->bind_to(&(crop->fruit.p_harvest_DOY_latest)               V4_HELP_URL(HELP_P_crop_model));
   #if (CROPSYST_VERSION==4)
   edit_max_water_upake_before_fruit         ->bind_to(&(crop->fruit.p_max_water_uptake_before_fruiting)       V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));
   edit_max_water_uptake_during_intial_fruit ->bind_to(&(crop->fruit.p_max_water_uptake_during_initial_fruit)  V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));
   edit_max_water_uptake_during_rapid_fruit  ->bind_to(&(crop->fruit.p_max_water_uptake_during_radid_fruit)    V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));
   edit_max_water_uptake_during_veraison     ->bind_to(&(crop->fruit.p_max_water_uptake_during_veraison)       V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));
   edit_max_water_uptake_after_maturity      ->bind_to(&(crop->fruit.p_max_water_uptake_after_maturity)        V4_HELP_URL(HELP_P_crop_transpiration_max_water_uptake));
   #endif
}
//_bind_to_fruit_______________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_nitrogen()
{  button_nitrogen_fixation                  ->bind_to(&(crop->nitrogen.fixating)                                V4_HELP_URL(HELP_P_crop_N_fixation));
   edit_residual_N_not_available_for_uptake  ->bind_to(&(crop->nitrogen.p_residual_N_not_available_for_uptake)   V4_HELP_URL(HELP_P_crop_N_uptake_adj));  // Warning needs help label
   edit_soil_N_conc_where_N_uptake_decreases ->bind_to(&(crop->nitrogen.p_soil_N_conc_where_N_uptake_decreases)  V4_HELP_URL(HELP_P_crop_N_uptake_adj));  // Warning needs help label
   edit_PAW_where_N_uptake_rate_decreases    ->bind_to(&(crop->nitrogen.p_PAW_where_N_uptake_rate_decreases)     V4_HELP_URL(HELP_P_crop_N_uptake_adj));  // Warning needs help label
   edit_N_max_stubble                        ->bind_to(&(crop->nitrogen.p_stubble_max_conc)                      V4_HELP_URL(HELP_P_crop_N_N_cont_stubble));
   edit_root_N_conc                          ->bind_to(&(crop->nitrogen.p_root_conc)                             V4_HELP_URL(HELP_P_crop_N_N_cont_stubble));  // Warning need to add root conc text
   edit_N_max_daily_uptake_rate              ->bind_to(&(crop->nitrogen.p_max_uptake_daily_rate)                 /*NO_HELP_YET_*/);
   //200529 radiogroup_nitrogen_leaf_stress_mode      ->bind_to(&(crop->nitrogen.nitrogen_leaf_stress_mode_cowl) );
// now only V4   edit_BM_start_dilution_max_conc           ->bind_to(&(crop->nitrogen.p_biomass_to_start_dilution_max_N_conc)                 ,0/*NO_HELP_YET_*/);   //130320
   edit_N_mature_max_conc                    ->bind_to(&(crop->nitrogen.p_N_mature_max_conc)                 /*NO_HELP_YET_*/);  //120505
   edit_conc_slope                           ->bind_to(&(crop->nitrogen.p_dilution_curve_slope) /*NO_HELP_YET_*/);  //120507
   edit_end_dilution_curve                   ->bind_to(&(crop->nitrogen.p_end_dilution_curve) /*NO_HELP_YET*/); //130408

   edit_BM_start_dilution_crit_conc          ->bind_to(&(crop->nitrogen.p_biomass_to_start_dilution_crit_N_conc)  /* no help yet */ ); //130319
   edit_N_emerge_crit_conc                   ->bind_to(&(crop->nitrogen.p_emergence_crit_conc)  /* no help yet */ ); //130319
   edit_N_emerge_max_conc                    ->bind_to(&(crop->nitrogen.p_emergence_max_conc)  /* no help yet */ ); //130319
   edit_nitrogen_leaf_stress_mode            ->bind_to(&(crop->nitrogen.leaf_expansion_sensitivity),6);  //200529
}
//_bind_to_nitrogen____________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_salinity()
{  edit_osmotic_pot_50     ->bind_to( &(crop->salinity.p_osmotic_pot_50)    V4_HELP_URL(HELP_P_crop_salinity_osmotic_pot));
   edit_salt_tolerance_P   ->bind_to( &(crop->salinity.p_salt_tolerance_P)  V4_HELP_URL(HELP_P_crop_salinity_tolerance_exp));
}
//_bind_to_salinity____________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_CO2_response()
{
   edit_TUE_reference_conc->bind_to( &(crop->CO2_response.p_TUE_reference_conc)  V4_HELP_URL(HELP_P_crop_CO2_base_ref_conc)); //131206
   edit_RUE_reference_conc->bind_to( &(crop->CO2_response.p_RUE_reference_conc)  V4_HELP_URL(HELP_P_crop_CO2_base_ref_conc)); //131206
   edit_CO2_alpha                   ->bind_to( &(crop->CO2_response.p_alpha)     /*NO_HELP_YET_*/); //131206
   edit_CO2_theta                   ->bind_to( &(crop->CO2_response.p_theta)     /*NO_HELP_YET_*/); //131206
   edit_CO2_growth_ratio_asymptotic        ->bind_to( &(crop->CO2_response.p_growth_ratio_asymptotic)/*NO_HELP_YET_*/); //131206
}
//_bind_to_CO2_response________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_vernalization()
{  edit_vern_low_temp      ->bind_to(&(crop->vernalization.p_low_temp)    V4_HELP_URL(HELP_P_crop_vernal_vern_Tlow));
   edit_vern_high_temp     ->bind_to(&(crop->vernalization.p_high_temp)   V4_HELP_URL(HELP_P_crop_vernal_vern_Thigh));
   edit_vern_start         ->bind_to(&(crop->vernalization.p_start)       V4_HELP_URL(HELP_P_crop_vernal_req_to_start));
   edit_vern_end           ->bind_to(&(crop->vernalization.p_end)         V4_HELP_URL(HELP_P_crop_vernal_req_to_complete));
   edit_vern_min_factor    ->bind_to(&(crop->vernalization.p_min_factor)  V4_HELP_URL(HELP_P_crop_vernal_min_factor));
   checkbox_vernalization  ->bind_to(&(crop->vernalization.enabled)       V4_HELP_URL(HELP_P_crop_vernal_enable_checkbox));
}
//_bind_to_vernalization_______________________________________________________/
void __fastcall TCrop_editor_form::bind_to_dormancy()
{  edit_chill_requirement                  ->bind_to(&(crop->dormancy.p_chill_hours)                          V4_HELP_URL(HELP_P_crop_dormancy));
   edit_deg_day_bud_break_chill_req_sat    ->bind_to(&(crop->dormancy.p_deg_day_bud_break_chill_req_sat)      V4_HELP_URL(HELP_P_crop_dormancy));
   edit_deg_day_bud_break_chill_req_not_sat->bind_to(&(crop->dormancy.p_deg_day_bud_break_chill_req_not_sat)  V4_HELP_URL(HELP_P_crop_dormancy));
   edit_dormancy_senesce                   ->bind_to(&(crop->dormancy.p_senesce_percent)                      V4_HELP_URL(HELP_P_crop_dormancy_senesce));
}
//_bind_to_dormancy____________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_hardiness()
{
   checkbox_sensitive_to_cold             ->bind_to(&(crop->tolerance.enabled)                  V4_HELP_URL(HELP_P_crop_hardiness));

   //170524 edit_salvageable_deg_day               ->bind_to(&(crop->tolerance.p_salvageable_deg_day) );
   edit_damage_cold_temperature           ->bind_to(&(crop->tolerance.p_leaf_damage_cold_temp)  V4_HELP_URL(HELP_P_crop_hardiness_damage_cold_temp));
   edit_lethal_cold_temperature           ->bind_to(&(crop->tolerance.p_leaf_lethal_cold_temp)  V4_HELP_URL(HELP_P_crop_hardiness_lethal_cold_temp));

   edit_hardiness_fruit_damage            ->bind_to(&(crop->tolerance.temperature_cold_fruit_damage),2 V4_HELP_URL(HELP_P_crop_hardiness_damage_cold_temp));
   edit_hardiness_fruit_lethal            ->bind_to(&(crop->tolerance.temperature_cold_fruit_lethal),2 V4_HELP_URL(HELP_P_crop_hardiness_lethal_cold_temp));
}
//_bind_to_hardiness___________________________________________________________/
void __fastcall TCrop_editor_form::bind_to_orchard()
{  if (!crop) return;
   edit_branch_insertion_height           ->bind_to( &(crop->canopy_architecture.p_branch_insertion_height) V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   // Layout
   edit_row_azimuth                       ->bind_to( &(crop->layout.p_row_azimuth)  V4_HELP_URL(HELP_P_crop_layout_orientation));
   edit_row_spacing                       ->bind_to( &(crop->layout.p_row_spacing)  V4_HELP_URL(HELP_P_crop_layout_orientation));
   edit_tree_spacing                      ->bind_to( &(crop->layout.p_col_spacing)  V4_HELP_URL(HELP_P_crop_layout_orientation));
   //Hedge row
   #if CS_VERSION==4
   edit_initial_row_width                 ->bind_to(&(crop->canopy_architecture.initial_width)  ,2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   edit_final_row_width                   ->bind_to(&(crop->canopy_architecture.final_width)    ,2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   edit_initial_tree_breadth              ->bind_to(&(crop->canopy_architecture.initial_breadth),2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   edit_final_tree_breadth                ->bind_to(&(crop->canopy_architecture.final_breadth)  ,2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   edit_initial_row_height                ->bind_to(&(crop->canopy_architecture.initial_height) ,2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   edit_final_row_height                  ->bind_to(&(crop->canopy_architecture.final_height)   ,2 V4_HELP_URL(HELP_P_crop_layout_canopy_characteristics));
   standard_row_image->Visible   = false; // May use this for future work crop_standard_row ;
   #else
   edit_initial_row_width                 ->bind_to(&(crop->canopy_architecture.initial_width)  ,2);
   edit_final_row_width                   ->bind_to(&(crop->canopy_architecture.final_width)    ,2);
   edit_initial_tree_breadth              ->bind_to(&(crop->canopy_architecture.initial_breadth),2);
   edit_final_tree_breadth                ->bind_to(&(crop->canopy_architecture.final_breadth),2  );
   edit_initial_row_height                ->bind_to(&(crop->canopy_architecture.initial_height) ,2);
   edit_final_row_height                  ->bind_to(&(crop->canopy_architecture.final_height) ,2  );
   #endif
}
//_bind_to_orchard_____________________________________________________________/
void __fastcall TCrop_editor_form::Update_V4_and_V5()
{
   if (!crop) return;
   TForm::Update();
   if (parameter_file_form)
      parameter_file_form->Update();

   pagecontrol_crop_model->TabIndex = crop->crop_model_clad.get();               //190627
   //190627 radiogroup_crop_model                     ->Update();
   radiogroup_landuse                        ->Update();
   radiogroup_photosynthetic                 ->Update();
   radiogroup_life_cycle                     ->Update();
   radiogroup_harvested_biomass              ->Update();
   radiogroup_stem_type                      ->Update();

   edit_light_extinction_coef                ->Update();                         //191121
   //191121 edit_kc                                   ->Update();
      //191121 kc is now light_extinction_coef
   edit_ET_crop_coef                         ->Update();
   edit_max_water_uptake_mm                  ->Update();

   edit_max_water_upake_before_fruit         ->Update();
   edit_max_water_uptake_during_intial_fruit ->Update();
   edit_max_water_uptake_during_rapid_fruit  ->Update();
   edit_max_water_uptake_during_veraison     ->Update();
   edit_max_water_uptake_after_maturity      ->Update();

   edit_leaf_water_pot_stomatal_closure      ->Update();
   edit_wilt_leaf_water_pot                  ->Update();
   edit_max_LAI                              ->Update();
   edit_initial_GAI                          ->Update();
   edit_regrowth_GAI                         ->Update();
   edit_specific_leaf_area                   ->Update();
   edit_stem_leaf_partition                  ->Update();
   edit_fract_LAI_mature                     ->Update();

   // canopy
   edit_accrescence_duration                 ->Update();                         //190627
   edit_senescence_initiation_obsolete       ->Update();                         //191121_190627
   edit_senescence_duration                  ->Update();                         //190627

   // Canopy cover based  canopy growth
   // canopy cover
   edit_canopy_cover_initial                 ->Update();
   edit_canopy_cover_maximum                 ->Update();
//   #ifndef SENESCENCE_REDUCTION_200124
   edit_canopy_cover_total_season_end        ->Update();                         //171219
//   #endif
   radiogroup_abscission                     ->Update();                         //141120

   // Root
   edit_max_root_depth                       ->Update();
   edit_root_length_per_unit_mass            ->Update();
   edit_surface_root_density                 ->Update();
   edit_root_density_distribution_curvature  ->Update();
   edit_max_root_depth_dd_obsolete           ->Update();                         //191121

   edit_root_shoot_emergence_ratio           ->Update();                         //130627
   edit_root_shoot_full_ratio                ->Update();                         //130627
   edit_root_sensitivity_to_water_stress     ->Update();                         //130628

   // Water use efficiency
   checkbox_WUE_changes                      ->Update();
   edit_WUE_change_deg_day                   ->Update();
   edit_TUE_scaling_coef                     ->Update();
   edit_TUE_scaling_coef_vegetative          ->Update();
   edit_TUE_at_1kPa_VPD                      ->Update();
   edit_TUE_at_1kPa_VPD_vegetative           ->Update();
   /*191026 AquaCrop and Tanner Sinclair now obsolete
   edit_water_productivity                   ->Update();
   edit_water_productivity_vegetative        ->Update();
   edit_reference_biomass_transp_coef_0      ->Update();
   edit_reference_biomass_transp_coef_vegetative_0->Update();
   */
   edit_seasonal_adjustment_2    ->Update();
   edit_seasonal_adjustment_3    ->Update();
   edit_seasonal_adjustment_4    ->Update();
   edit_seasonal_adjustment_5    ->Update();

   // Shouldnt need to update Efficiency model radiogroup

   edit_RUE_at_low_VPD              ->Update();                                  //110825
   edit_RUE_at_low_total_solar_rad  ->Update();                                  //110825
   edit_RUE_rate_decrease           ->Update();                                  //110825
   //191026 edit_RUE_global_basis                              ->Update();                //110825

   //191026  Only in V4   edit_RUE_PAR_basis                                ->Update();              //110825

   edit_LWP_that_reduces_canopy_expansion    ->Update();
   edit_LWP_that_stops_canopy_expansion      ->Update();

   edit_temp_limit_min                       ->Update();                         //130531
   edit_temp_limit_low                       ->Update();
   edit_temp_limit_high                      ->Update();
   edit_temp_limit_max                       ->Update();

   edit_at_pt_root_limit                     ->Update();
   checkbox_clipping_resets                  ->Update();
   edit_deg_day_emerge                       ->Update();
   edit_deg_day_tuber_initiation             ->Update();
   //190627 edit_deg_day_end_vegetative_growth        ->Update();
   edit_deg_day_begin_flower                 ->Update();
   edit_deg_day_begin_filling_grain_or_fruit ->Update();
   edit_deg_day_begin_rapid_fruit_growth     ->Update();
   edit_deg_day_begin_maturity               ->Update();
   checkbox_maturity_significant             ->Update();

   radiogroup_thermal_response->Update();                                        //170402
   edit_base_temp             ->Update();
   edit_cutoff_temp           ->Update();
   edit_opt_temp              ->Update();
   edit_max_temp              ->Update();
   edit_phenologic_sensitivity_water_stress->Update();

   checkbox_vernalization  ->Update();
   {
      edit_vern_low_temp      ->Update();
      edit_vern_high_temp     ->Update();
      edit_vern_start         ->Update();
      edit_vern_end           ->Update();
      edit_vern_min_factor    ->Update();
   }
   radiogroup_photoperiod_consideration       ->Update();
   edit_stop_daylength                       ->Update();
   edit_constrained_daylength                ->Update();

   edit_HI_unstressed                     ->Update();
   checkbox_HI_general_adjustments        ->Update(); //150210

   edit_HI_growth_sensitivity             ->Update();
   checkbox_HI_extreme_adjustments        ->Update(); //150210_110907
   edit_HI_tuber_init_sensitivity         ->Update();
   edit_HI_tuber_growth_sensitivity       ->Update();

   edit_grain_translocation_fraction        ->Update(); // was edit_grain_translocation_factor

/*currently only in V4
   edit_cold_stress_no_stress_threshold_temperature   ->Update();                //130416
   edit_cold_stress_max_stress_threshold_temperature  ->Update();                //130416
   edit_cold_stress_sensitivity                       ->Update();                //130416
   edit_cold_stress_exposure_damaging_hours           ->Update();                //130416

   edit_heat_stress_no_stress_threshold_temperature   ->Update();                //130416
   edit_heat_stress_max_stress_threshold_temperature  ->Update();                //130416
   edit_heat_stress_sensitivity                       ->Update();                //130416
   edit_heat_stress_exposure_damaging_hours           ->Update();                //130416
*/

   {
      edit_fruit_fract_total_solids          ->Update();
      edit_fruit_load_max                    ->Update();
      checkbox_grape                         ->Update();
      edit_initial_fruit_mass                ->Update();
      edit_initial_reserves_release_rate     ->Update();
      edit_clusters_per_vine                 ->Update();
      edit_berries_per_cluster               ->Update();
      edit_fruits_per_tree                ->Update();

   radiogroup_fruit_harvest_condition     ->Update();
   edit_harvest_temperature               ->Update();
   edit_harvest_temperature_days          ->Update();
   edit_harvest_DOY_earliest              ->Update();
   edit_harvest_DOY_latest                ->Update();
   }
   {
      radiogroup_RULSE2_residue_type            ->Update();
      edit_carbon_fraction                      ->Update();
      edit_stubble_area_covered_to_mass_ratio   ->Update();
      edit_surface_area_covered_to_mass_ratio   ->Update();
      edit_fast_cycling_top_fraction            ->Update();
      edit_slow_cycling_top_fraction            ->Update();
      edit_lignified_top_fraction               ->Update();
      edit_fast_cycling_root_fraction           ->Update();
      edit_slow_cycling_root_fraction           ->Update();
      edit_lignified_root_fraction              ->Update();
      edit_fast_cycling_C_N_ratio               ->Update();
      edit_slow_cycling_C_N_ratio               ->Update();
      edit_lignified_C_N_ratio                  ->Update();
      edit_fast_cycling_halflife                ->Update();
      edit_slow_cycling_halflife                ->Update();
      edit_lignified_halflife                   ->Update();
      edit_detrition_time                       ->Update();
   }
   {
      button_nitrogen_fixation                  ->Update();
      edit_residual_N_not_available_for_uptake  ->Update();
      edit_soil_N_conc_where_N_uptake_decreases ->Update();
      edit_PAW_where_N_uptake_rate_decreases    ->Update();

      edit_BM_start_dilution_crit_conc       ->Update();                         //130319
      edit_N_emerge_crit_conc                ->Update();                         //130319
      edit_N_emerge_max_conc                 ->Update();                         //130319

      edit_N_max_stubble                     ->Update();
      edit_root_N_conc                       ->Update();

      edit_N_max_daily_uptake_rate           ->Update();
      //200529 radiogroup_nitrogen_leaf_stress_mode   ->Update();
      edit_N_mature_max_conc                 ->Update();                         //120505
      crop->nitrogen.provide_concentration_slope(crop->photosynthetic_pathway_clad.get()); //120505
      edit_conc_slope                        ->Update();                         //120505
      edit_end_dilution_curve                ->Update();                         //130408
      edit_nitrogen_leaf_stress_mode         ->Update();                         //200529
   }
   edit_osmotic_pot_50                       ->Update();
   edit_salt_tolerance_P                     ->Update();
   edit_CO2_growth_ratio_elevated_to_baseline->Update();
   edit_CO2_elevated_reference_conc          ->Update();
   edit_CO2_baseline_reference_conc          ->Update();

   edit_TUE_reference_conc                   ->Update();                         //131206
   edit_RUE_reference_conc                   ->Update();                         //131206
   edit_CO2_alpha                            ->Update();                         //131206
   edit_CO2_theta                            ->Update();                         //131206
   edit_CO2_growth_ratio_asymptotic          ->Update();                         //131206

   edit_inducement_temperature               ->Update();
   edit_quiescence_end_DOY                   ->Update();                         //161004
   edit_quiescence_start_DOY                 ->Update();                         //161004
   edit_chill_requirement                    ->Update();
   edit_deg_day_bud_break_chill_req_sat      ->Update();
   edit_deg_day_bud_break_chill_req_not_sat  ->Update();
   edit_dormancy_senesce->Update();
   edit_consider_inactive_days            ->Update();
   checkbox_sensitive_to_cold             ->Update();
   edit_damage_cold_temperature           ->Update();
   edit_lethal_cold_temperature           ->Update();
   edit_hardiness_fruit_damage            ->Update();
   edit_hardiness_fruit_lethal            ->Update();

   edit_salvageable_deg_day               ->Update();
   edit_branch_insertion_height           ->Update();
   edit_branch_insertion_height           ->Update();
   edit_row_azimuth                       ->Update();
   edit_row_spacing                       ->Update();
   edit_tree_spacing                      ->Update();
   edit_initial_row_width                 ->Update();
   edit_final_row_width                   ->Update();
   edit_initial_tree_breadth              ->Update();
   edit_final_tree_breadth                ->Update();
   edit_initial_row_height                ->Update();
   edit_final_row_height                  ->Update();
   edit_leaf_duration_sensitivity_stress  ->Update();
   edit_deg_day_leaf_duration             ->Update();

   radiogroup_emergence_model             ->Update();
   edit_seedling_leaf_area                ->Update();
   edit_germination_base_temperature      ->Update();
   edit_mean_base_soil_water_pot          ->Update();
   edit_stdev_base_soil_water_pot         ->Update();
   edit_a                                 ->Update();
   edit_b                                 ->Update();
   checkbox_HI_general_adjustments        ->Update();                            //150210
   checkbox_HI_extreme_adjustments        ->Update();                            //150210
   edit_linger->Update();
   pagecontrol_inactivity
      ->ActivePageIndex = crop->continuation_mode_clad.get_int32();              //200213
   // Shouldnt need to update Efficiency model radiogroup
   //191025radiogroup_WUE_submodel->Update();
   update_root_biomass_checksum(0);
   update_top_biomass_checksum(0);
   show_hide_controls();
}
//_Update_V4_and_V5____________________________________________________________/
void TCrop_editor_form::show_hide_controls_V4_and_V5()
{
   if (!crop) return;
   bool CROPSYST_OR_CROPGRO_MODEL = ((crop->crop_model_clad.get() == CROPSYST_CROP_MODEL) || (crop->crop_model_clad.get() == CROPGRO_MODEL));
   bool CROPSYST_FRUIT = crop->is_fruit_tree();                                  //190625
   checkbox_grape->Visible             = crop->harvested_part_clad.get()==fruit_part;
   edit_deg_day_tuber_initiation->Visible = crop->harvested_part_clad.get() == tuber_part;
   edit_deg_day_begin_filling_grain_or_fruit->Visible
      =  crop->harvested_part_clad.get()==grain_part
      || crop->harvested_part_clad.get()==fruit_part;
   groupbox_max_water_uptake->Visible = CROPSYST_FRUIT;
   edit_max_water_uptake_mm->Visible = !CROPSYST_FRUIT;
   radiogroup_life_cycle            ->Visible = CROPSYST_OR_CROPGRO_MODEL;

   edit_deg_day_begin_filling_grain_or_fruit->Visible = !crop->harvested_part_clad.get() != leaf_part;
   // Always shown
   // growth_sheet
   // leaf_root_sheet
   // transpiration_sheet
   // harvest_sheet
   // dormancy_sheet
   // hardiness_sheet
   // CO2_sheet
   // dormancy_sheet
   // phenology_sheet
   Harvested_part harvested_part = crop->harvested_part_clad.get();
   edit_HI_unstressed->Visible = crop->applies_harvest_index();
   bool is_seed_crop = crop->harvested_part_clad.get() == grain_part;
   tabsheet_harvest_grain->TabVisible = true;   // It is necessary to enable all the tabsheets
   tabsheet_harvest_tuber->TabVisible = true;   // Otherwise all could be disabled
   tabsheet_harvest_fruit->TabVisible = true;   // as we switch and that will cause an error.
   tabsheet_harvest_other->TabVisible = true;
   tabsheet_harvest_grain->TabVisible = is_seed_crop;
   tabsheet_harvest_tuber->TabVisible = crop->harvested_part_clad.get() == tuber_part; // Maybe also root crop
   tabsheet_harvest_fruit->TabVisible
      = (crop->harvested_part_clad.get() == fruit_part)
      ||crop->is_fruit_tree();                                                   //190625
      //190625 (crop->crop_model_clad.get() == CROPSYST_ORCHARD_MODEL);
   tabsheet_harvest_other->TabVisible = !tabsheet_harvest_grain->TabVisible && !tabsheet_harvest_tuber->TabVisible  && !    tabsheet_harvest_fruit->TabVisible; // 110907
   // The following yields are needed
   // ROOT_CROP,LEAF_CROP
   // TEA_CROP

   radiogroup_life_cycle->Update();
   radiogroup_harvested_biomass           ->Visible = CROPSYST_OR_CROPGRO_MODEL;

   edit_temp_limit_min  ->Visible = CROPSYST_OR_CROPGRO_MODEL;                   //130531
   edit_temp_limit_max  ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   edit_temp_limit_low  ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   edit_temp_limit_high ->Visible = CROPSYST_OR_CROPGRO_MODEL;

   edit_fract_LAI_mature                  ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   edit_deg_day_emerge                    ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   //190827 edit_deg_day_end_vegetative_growth     ->Visible = true;
   //191121 edit_max_root_depth_dd_obsolete     ->Visible = !crop->is_fruit_tree();                                                    //190625
      //190625 crop->crop_model_clad.get() != CROPSYST_ORCHARD_MODEL ;
   edit_deg_day_begin_rapid_fruit_growth  ->Visible = (crop->harvested_part_clad.get() == fruit_part);
   show_hide_dormancy();
   groupbox_inactive_period->Visible = crop->continuation_mode_clad.get() >= multiple_season;
   radiogroup_landuse            ->Visible = CROPSYST_OR_CROPGRO_MODEL;
   if (crop->harvested_part_clad.get()==fruit_part)
   {
      crop->land_use_clad.set(FALLOW); // Claudio decided to use fallow for orchard model.
      groupbox_fruit_grape->Visible =  crop->fruit.grape;
      groupbox_fruit_other->Visible = !crop->fruit.grape;
   }
   checkbox_clipping_resets->Visible = !CROPSYST_FRUIT;
   groupbox_hardiness_fruit->Visible = CROPSYST_FRUIT;
   groupbox_hydrothermal_time->Visible = crop->emergence.emergence_model_cowl.get() == EMERGENCE_HYDROTHERMAL_TIME;
   edit_N_max_daily_uptake_rate->Visible = true; ;

   // For the following phenology edits, the input fields are shifted up
   // when stages are not applicable (visible)

    if (CROPSYST_FRUIT) // maturity is always significant for fruit crops.
      panel_maturity_significant             ->Visible = false;
   edit_deg_day_begin_maturity->Visible = crop->phenology.maturity_significant || CROPSYST_FRUIT;

   bool hardness_enabled = crop->tolerance.enabled;
   groupbox_cold_temperature_sensitivity->Enabled = hardness_enabled;
   edit_damage_cold_temperature->Enabled = hardness_enabled;
   edit_lethal_cold_temperature->Enabled = hardness_enabled;
   edit_salvageable_deg_day->Enabled = hardness_enabled;                         //140509

   edit_hardiness_fruit_damage->Enabled = hardness_enabled;
   edit_hardiness_fruit_lethal->Enabled = hardness_enabled;

   edit_salvageable_deg_day->Enabled = hardness_enabled;
   edit_salvageable_deg_day->Enabled = hardness_enabled;
   bool vernalization_enabled = crop->vernalization.enabled;
   edit_vern_low_temp->Enabled   = vernalization_enabled;
   edit_vern_high_temp->Enabled  = vernalization_enabled;
   edit_vern_start->Enabled      = vernalization_enabled;
   edit_vern_end->Enabled        = vernalization_enabled;
   edit_vern_min_factor->Enabled = vernalization_enabled;

   bool photoperiod_enabled = crop->photoperiod.photoperiodism > no_photoperiod; //110606
   groupbox_photoperiod_daylength->Visible = photoperiod_enabled;
   edit_stop_daylength        ->Enabled = photoperiod_enabled;
   edit_constrained_daylength ->Enabled = photoperiod_enabled;

   switch (crop->photoperiod.photoperiodism)
   {  case short_day_photoperiod :
         caption_at_which_stopped->Caption      = "day length above which";
         caption_at_which_constrained->Caption  = "day length below which";
      break;
      case long_day_photoperiod :
         caption_at_which_stopped->Caption      = "day length below which";
         caption_at_which_constrained->Caption  = "day length above which";
      break;
   }
   checkbox_senesces->Visible = (crop->life_cycle_clad.get() != annual_life_cycle);
   // Annuals always senesce
   edit_linger->Visible = (crop->abscission_clad.get() == semi_deciduous);

   bool has_leaf_senescence = crop->has_leaf_senescence();
   bool canopy_growth_leaf_area_index_based =
    (crop->morphology.canopy_growth_cowl.get()==leaf_area_index_based_duration);
   groupbox_LAI_duration->Visible =  canopy_growth_leaf_area_index_based;
   //200403 panel_canopy_model->Visible = !crop->is_fruit_tree();
   panel_canopy_model->Visible = !crop->morphology.canopy_architecture;          //200403
   bool WUE_changes_visible = crop->biomass_production.WUE_changes;
   edit_WUE_change_deg_day->Visible = WUE_changes_visible;
   /*191026 AquaCrop and Tanner Sinclair now obsolete
   edit_water_productivity_vegetative              ->Visible = WUE_changes_visible;
   groupbox_tanner_sinclair->Visible   = crop->biomass_production.TUE_equation_cowl.get() == TUE_Tanner_Sinclair;
   groupbox_aquacrop->Visible          = crop->biomass_production.TUE_equation_cowl.get() == TUE_water_productivity;
   edit_reference_biomass_transp_coef_vegetative_0 ->Visible = WUE_changes_visible;
   */
   edit_TUE_at_1kPa_VPD_vegetative                 ->Visible = WUE_changes_visible;
   edit_TUE_scaling_coef_vegetative                ->Visible = WUE_changes_visible;
   //191026 groupbox_TUE_curve->Visible         = crop->biomass_production.TUE_equation_cowl.get() == TUE_curve;
   groupbox_adjustments_enabled->Visible
   = crop->yield.apply_adjustment_to_unstressed_harvest_index                    //110907
   || CROPSYST_FRUIT                                                             //110907
   || crop->harvested_part_clad.get()==fruit_part;                               //120510

   /*always visible if HI adjustments enabled
   page_control_harvest ->Visible
   = crop->yield.apply_temperature_extreme_response                              //150210
   || CROPSYST_FRUIT                                                             //150210
   || crop->harvested_part_clad.get()==fruit_part;                               //150210
   */
   edit_grain_translocation_fraction->Visible = crop->harvested_part_clad.get() == grain_part;
}
//_show_hide_controls_V4_and_V5________________________________________________/
void TCrop_editor_form::handle_update_root_biomass_checksum()
{  bool valid =  CORN::is_approximately<float32>(root_checksum,1.0,0.000001);
   root_biomass_checksum->Color = valid ? clLime: clRed;
   root_biomass_checksum->Update();
}
//_handle_update_root_biomass_checksum_________________________________________/
void TCrop_editor_form::handle_update_top_biomass_checksum()
{  bool valid =  CORN::is_approximately<float32>(top_checksum,1.0,0.000001);
   top_biomass_checksum->Color = valid ? clLime: clRed;
   top_biomass_checksum->Update();
}
//_handle_update_top_biomass_checksum__________________________________________/
void __fastcall  TCrop_editor_form::bind_to_harvest_index()
{  if (!crop) return;
   edit_HI_unstressed->bind_to( &(crop->yield.p_index_unstressed) V4_HELP_URL(HELP_P_crop_harvest_index));
   edit_HI_unstressed->Visible = crop->applies_harvest_index();

   tabsheet_harvest_grain->TabVisible = true;  // We need to have all the tabs visible
   tabsheet_harvest_tuber->TabVisible = true;   // before we hide them incase we hide the only one that is visible (VCL compains)
   tabsheet_harvest_fruit->TabVisible = true;

   tabsheet_harvest_grain->TabVisible = crop->harvested_part_clad.get()==grain_part;
   tabsheet_harvest_tuber->TabVisible = crop->harvested_part_clad.get()==tuber_part; // Maybe also root crop
   bool is_fruit = (crop->harvested_part_clad.get() == fruit_part)
      || crop->is_fruit_tree();                                                  //190625
   tabsheet_harvest_fruit->TabVisible = is_fruit;                                //190625
   tabsheet_harvest_other->TabVisible = !tabsheet_harvest_grain->TabVisible && !tabsheet_harvest_tuber->TabVisible  && !    tabsheet_harvest_fruit->TabVisible; // 110907

   edit_HI_growth_sensitivity             ->bind_to(&(crop->yield.p_growth_sensitivity)  V4_HELP_URL(HELP_P_crop_harvest_sens_water_stress));
   edit_HI_growth_sensitivity             ->Visible =
      (((crop->harvested_part_clad.get()==leaf_part))
      &&(crop->crop_model_clad.get() == CROPSYST_CROP_MODEL ));

   // The following will be hidden using sheets

   edit_HI_tuber_init_sensitivity            ->bind_to( &(crop->yield.p_tuber_init_sensitivity)  V4_HELP_URL(HELP_P_crop_harvest_sens_water_stress));
   edit_HI_tuber_growth_sensitivity          ->bind_to( &(crop->yield.p_tuber_growth_sensitivity)  V4_HELP_URL(HELP_P_crop_harvest_sens_water_stress));

   checkbox_HI_general_adjustments->bind_to(&(crop->yield.apply_adjustment_to_unstressed_harvest_index)); //150210
   checkbox_HI_extreme_adjustments->bind_to(&(crop->yield.apply_temperature_extreme_response)); //200424_150210

   edit_grain_translocation_fraction->bind_to(&(crop->yield.p_translocation_fraction_max)/*NO_HELP_YET_*/ );            //120316




/* Currently only in V4, in V5 there will be more generalized scheduling of parameter adjustments
   edit_heat_stress_no_stress_threshold_temperature   ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity.heat.no_stress_threshold_temperature),1,0);
   edit_heat_stress_max_stress_threshold_temperature  ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity.heat.max_stress_threshold_temperature),1,0 );
   edit_heat_stress_sensitivity                       ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity.heat.sensitivity ),2,0 );
   edit_heat_stress_exposure_damaging_hours           ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity.heat.exposure_damaging_hours ),1,0 );
   edit_heat_stress_exposure_initial_hour             ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity.heat.exposure_initial_hour ),1,0 );

   edit_cold_stress_no_stress_threshold_temperature   ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity_period.cold.no_stress_threshold_temperature),1,0 );
   edit_cold_stress_max_stress_threshold_temperature  ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity_period.cold.max_stress_threshold_temperature),1,0 );
   edit_cold_stress_sensitivity                       ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity_period.cold.sensitivity),2,0 );
   edit_cold_stress_exposure_damaging_hours           ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity_period.cold.exposure_damaging_hours),1,0 );
   edit_colc_stress_exposure_initial_hour             ->bind_to(&(crop->harvest.editor_grain_stress_sensitivity_period.cold.exposure_initial_hour),1,0 );
*/
}
//_bind_to_harvest_index____________________________________________2011-06-04_/
void __fastcall TCrop_editor_form::show_hide_dormancy()
{  if (!crop) return;
   //190625 bool CROPSYST_FRUIT (crop->crop_model_clad.get() == CROPSYST_ORCHARD_MODEL);
   groupbox_chill_requirements->Visible = crop->is_fruit_tree(); //190625 CROPSYST_FRUIT;
}
//_show_hide_dormancy_______________________________________________2011-06-04_/
void TCrop_editor_form::handle_crop_class_combobox_onchange()
{
   CORN::OS::File_name_concrete default_filename
   (crop_class_combobox->Items->Strings[crop_class_combobox->ItemIndex].c_str());//190419
   if (Application->MessageBox
      (_TEXT("Loading defaults will change all current crop parameters. Continue?")
      ,_TEXT("Load default crop parameters"),MB_YESNO) == IDYES)
   {  CORN::VV_File default_file(default_filename.c_str());
      default_file.get(*crop);
      bind_to(crop,parameter_file_form,crop_model);
      Update();
      parameter_file_form->edit_description->Update();
   }
   CORN::OS::file_system_engine.set_current_working_directory(crop_directory);
}
//_handle_crop_class_combobox_onchange_________________________________________/
void TCrop_editor_form::handle_calibrate_button_on_click()
{
   // Need to save the file so the crop calibrator can load
   parameter_file_form-> WindowState = wsMinimized;
   parameter_file_form->run_file_with_program
      (CS::Suite_directory->CropSyst().crop_calibrator_executable().c_str()
      ,0    // const char *secondary_file = 0
      ,0    // const char *options = 0
      ,true //  bool  wait_for_termination = true   // True if this editor is held until the other program exits
      ,SW_SHOWNORMAL);  // or SW_MINIMIZE  SW_MAXIMIZE
   parameter_file_form-> WindowState = wsNormal;
   parameter_file_form->get_parameters();
   Update();
}
//_handle_calibrate_button_on_click____________________________________________/
void TCrop_editor_form::handle_radiogroup_photosynthetic_click()
{  int photopath = radiogroup_photosynthetic->ItemIndex;
   crop->nitrogen.p_N_mature_max_conc.initial_value
      //161004 Warning, I dont remember if initial_value replaced  default_value
      //161004 default_value
      = CropSyst::N_mature_max_conc_default[photopath];
   crop->nitrogen.dilution_curve_slope = 0.0;
   crop->nitrogen.p_dilution_curve_slope.initial_value
      = crop->nitrogen.provide_concentration_slope(photopath);
   crop->nitrogen.p_biomass_to_start_dilution_crit_N_conc.initial_value
      = CropSyst::biomass_to_start_dilution_crit_N_conc_default[photopath];      //130320
   edit_N_emerge_max_conc  ->Update();                                           //130320
   edit_N_emerge_crit_conc ->Update();                                           //130320
   edit_BM_start_dilution_crit_conc  ->Update();                                 //130320
   edit_N_mature_max_conc  ->Update();
   edit_conc_slope         ->Update();
}
//_handle_radiogroup_photosynthetic_click___________________________2012-05-05_/
void TCrop_editor_form::handle_preset_end_dilution()
{  switch (radiogroup_preset_end_dilution->ItemIndex)
   {  case 0 : crop->nitrogen.end_dilution_curve_deg_day = 0; break; // (Never ends)
      case 1 : crop->nitrogen.end_dilution_curve_deg_day = crop->phenology.initiation.flowering    ;break; // Begin flowering
      case 2 : crop->nitrogen.end_dilution_curve_deg_day = crop->phenology.initiation.filling      ;break; // Yield formation
      case 3 : crop->nitrogen.end_dilution_curve_deg_day = crop->phenology.culmination_obsolete.accrescence ;break; // End of vegetative growth
      case 4 : crop->nitrogen.end_dilution_curve_deg_day =
         crop->phenology.initiation.accrescence + crop->phenology.initiation.senescence; //190719
         break; // Begin senescence
      case 5 : crop->nitrogen.end_dilution_curve_deg_day = crop->phenology.initiation.maturity     ;break; // Physiological maturity
   }
   edit_end_dilution_curve->Update();
}
//_handle_preset_end_dilution_______________________________________2013-04-08_/
void __fastcall TCrop_editor_form::radiogroup_abscissionClick(TObject *Sender)
{  edit_linger->Visible = (crop->abscission_clad.get() == semi_deciduous );
}
//-radiogroup_abscissionClick---------------------------------------------------

/* Todo
continue here.

Move the senecense degree days to Phenology (from canopy growth copy cover)
(this is now applicable to all crops).

Emergence and max root depth degree days should be optional
(With checkbox like maturity).

Nitrogen demand adjustment (delete)

Biomass to start dilution of max N conc
replace with  new parameter
Biomass to start dilution of critical N concentration

Concentration curve slope
replace with (rename to)
Dilution curve slope

N_max_conc_at_emergence_species  will be an input



parameters.calc_biomass_to_start_dilution_crit_N_conc_T_ha
will now just be a parameter

scaling_factor_critical_N_conc_species obsolete
*/

