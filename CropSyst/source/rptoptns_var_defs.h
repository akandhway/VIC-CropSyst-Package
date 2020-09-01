// 070207 now using CSVC variables directly

#define DLY_year                       CSVC_weather_date_year
#define DLY_day_of_year                CSVC_weather_date_DOY
#define DLY_month                      CSVC_weather_date_month
#define DLY_day                        CSVC_weather_date_DOM
#define DLY_crop_name                  CSVC_crop_base_name
#define DLY_growth_stage               CSVC_crop_base_growth_stage
#define DLY_fruit_biomass               CSVC_crop_base_fruit_biomass
#define DLY_fruit_canopy_biomass        CSVC_crop_base_fruit_canopy_biomass
#define DLY_leaf_area_index             CSVC_crop_base_leaf_area_index
#define DLY_green_area_index            CSVC_crop_base_green_area_index
#define DLY_root_depth                  CSVC_crop_base_root_depth
#define DLY_crop_water_stress           CSVC_crop_base_water_stress_factor
#define DLY_temperature_stress          CSVC_crop_base_temperature_stress_factor
#define DLY_VPD_daytime                 CSVC_crop_base_VPD_daytime
#define DLY_intercepted_PAR             CSVC_crop_base_intercepted_PAR
#define DLY_intercepted_PAR_accum       CSVC_crop_base_intercepted_PAR_accum_to_date_time
#define DLY_canopy_ground_cover         CSVC_crop_base_fract_canopy_cover
#define DLY_residue_ground_cover        CSVC_residue_ground_cover
#define DLY_residue_water_storage       CSVC_residue_water_storage
#define DLY_surface_residue_biomass     CSVC_residue_surface_biomass
#define DLY_residue_plant_incorporated  CSVC_residue_plant_incorporated
#define DLY_residue_manure_incorporated CSVC_residue_manure_incorporated
#define DLY_soil_clod_rate              CSVC_soil_erosion_clod_rate
#define DLY_soil_loss                   CSVC_soil_erosion_loss_time_step_sum
#define DLY_ET_max                      CSVC_weather_ET_max_time_step_sum
#define DLY_ET_act                      CSVC_weather_ET_act_time_step_sum
#define DLY_transp_pot                  CSVC_crop_base_transp_pot_time_step_sum
#define DLY_transp_act                  CSVC_crop_base_transp_act_time_step_sum
#define DLY_evap_soil_pot               CSVC_soil_base_evap_pot_time_step_sum
#define DLY_evap_soil_act               CSVC_soil_base_evap_act_time_step_sum
#define DLY_evap_residue_pot            CSVC_residue_pot_evap_time_step_sum
#define DLY_evap_residue_act            CSVC_residue_act_evap_time_step_sum
#define DLY_irrigation                  CSVC_mgmt_irrig_applied_time_step_sum
#define DLY_precipitation               CSVC_weather_precipitation_time_step_sum
#define DLY_crop_water_inter            CSVC_crop_base_water_intrcpt_time_step_sum
#define DLY_residue_water_inter         CSVC_residue_water_intrcpt_time_step_sum
#define DLY_surface_water_runoff        CSVC_soil_runoff_surface_water_runoff_time_step_sum
#define DLY_water_entering_soil         CSVC_soil_base_water_entering_time_step_sum
#define DLY_soil_water_drainage         CSVC_soil_base_water_drainage_time_step_sum
#define DLY_ET_max_accum                CSVC_weather_ET_max_accum_to_date_time
#define DLY_ET_act_accum                CSVC_weather_ET_act_accum_to_date_time
#define DLY_transp_pot_accum            CSVC_crop_base_transp_pot_accum_to_date_time
#define DLY_transp_act_accum            CSVC_crop_base_transp_act_accum_to_date_time
#define DLY_evap_soil_pot_accum         CSVC_soil_base_evap_pot_accum_to_date_time
#define DLY_evap_soil_act_accum         CSVC_soil_base_evap_act_accum_to_date_time
#define DLY_evap_residue_pot_accum      CSVC_residue_pot_evap_accum_to_date_time
#define DLY_evap_residue_act_accum      CSVC_residue_act_evap_accum_to_date_time
#define DLY_irrig_accum                 CSVC_mgmt_irrig_applied_accum_to_date_time
#define DLY_precip_accum                CSVC_weather_precipitation_accum_to_date_time
#define DLY_crop_water_intrcpt_accum    CSVC_crop_base_water_intrcpt_accum_to_date_time
#define DLY_residue_water_intrcpt_accum CSVC_residue_water_intrcpt_accum_to_date_time
#define DLY_surface_water_runoff_accum  CSVC_soil_runoff_surface_water_runoff_accum_to_date_time
#define DLY_ponded_water                CSVC_soil_pond_water_depth
#define DLY_water_enter_soil_accum      CSVC_soil_base_water_entering_accum_to_date_time
#define DLY_soil_water_drain_accum      CSVC_soil_base_water_drainage_accum_to_date_time
#define DLY_snow_storage                CSVC_weather_snow_storage
#define DLY_soil_water_depletion        CSVC_soil_base_water_depletion_accum_to_date_time
#define DLY_soil_water_balance          CSVC_soil_base_water_balance_accum_to_date_time
#define DLY_nitrogen_stress_index       CSVC_crop_N_stress_index
#define DLY_avg_nitrogen_stress_index   CSVC_crop_N_stress_index_period_avg
#define DLY_nitrogen_applied            CSVC_mgmt_N_applied_time_step_sum
#define DLY_nitrogen_uptake             CSVC_crop_N_uptake_time_step_sum
#define DLY_nitrogen_leached            CSVC_soil_N_leached_time_step_sum
#define DLY_nitrogen_applied_accum      CSVC_mgmt_N_applied_accum_to_date_time
#define DLY_nitrogen_uptake_accum       CSVC_crop_N_uptake_accum_to_date_time
#define DLY_nitrogen_leached_accum      CSVC_soil_N_leached_accum_to_date_time
#define DLY_canopy_N_concentration       CSVC_crop_N_concentration
#define DLY_mineralizated_residue       CSVC_residue_mineralized_time_step_sum
#define DLY_N_immobilization            CSVC_soil_N_immobilization_time_step_sum
#define DLY_N_immobilization_accum      CSVC_soil_N_immobilization_accum_to_date_time
#define DLY_mineralizated_residue_accum CSVC_residue_mineralized_accum_to_date_time
#define DLY_mineralizated_manure        CSVC_residue_manure_mineralized_time_step_sum
#define DLY_mineralizated_manure_accum  CSVC_residue_manure_mineralized_accum_to_date_time
#define DLY_mineralizated_OM            CSVC_organic_matter_mineralized_time_step_sum
#define DLY_mineralizated_OM_accum      CSVC_organic_matter_mineralized_accum_to_date_time
#define DLY_automatic_N_in_uptake       CSVC_crop_N_automatic_uptake_time_step_sum
#define DLY_automatic_N_in_uptake_accum CSVC_crop_N_automatic_uptake_accum_to_date_time

#define DLY_organic_N_applied           CSVC_mgmt_N_organic_applied_time_step_sum
#define DLY_organic_N_applied_accum     CSVC_mgmt_N_organic_applied_accum_to_date_time
#define DLY_N_Fixation                  CSVC_crop_N_fixation
#define DLY_N_Fixation_accum            CSVC_crop_N_fixation_time_step_sum

#define  YLY_year                       CSVC_weather_date_year
#define  YLY_residue_ground_cover       CSVC_residue_ground_cover
#define  YLY_soil_loss                  CSVC_soil_erosion_loss_time_step_sum
#define  YLY_ET_max                     CSVC_weather_ET_max_time_step_sum
//190812 #define  YLY_ET_pot                     CSVC_weather_ET_pot_time_step_sum
#define  YLY_ET_act                     CSVC_weather_ET_act_time_step_sum
#define  YLY_transp_pot                 CSVC_crop_base_transp_pot_time_step_sum
#define  YLY_transp_act                 CSVC_crop_base_transp_act_time_step_sum
#define  YLY_evap_soil_pot              CSVC_soil_base_evap_pot_time_step_sum
#define  YLY_evap_soil_act              CSVC_soil_base_evap_act_time_step_sum
#define  YLY_evap_residue_pot           CSVC_residue_pot_evap_time_step_sum
#define  YLY_evap_residue_act           CSVC_residue_act_evap_time_step_sum
#define  YLY_irrigation                 CSVC_mgmt_irrig_applied_time_step_sum
#define  YLY_precipitation              CSVC_weather_precipitation_time_step_sum
#define  YLY_crop_water_intrcpt         CSVC_crop_base_water_intrcpt_time_step_sum
#define  YLY_residue_water_intrcpt      CSVC_residue_water_intrcpt_time_step_sum
#define  xYLY_surface_water_runoff      CSVC_soil_runoff_surface_water_runoff_time_step_sum
#define  YLY_ponded_water               CSVC_soil_pond_water_depth
#define  YLY_water_enter_soil           CSVC_soil_base_water_entering_time_step_sum
#define  YLY_soil_water_drainage        CSVC_soil_base_water_drainage_time_step_sum
#define  YLY_soil_water_balance         CSVC_soil_base_water_balance_time_step_sum
//040930 040930 was #define  YLY_soil_water_balance         CSVC_soil_base_water_balance_accum_to_date_time
#define  YLY_nitrogen_applied_total     CSVC_mgmt_N_applied_time_step_sum
#define  YLY_nitrogen_leached_total     CSVC_soil_N_leached_time_step_sum
#define  YLY_nitrogen_uptake_total      CSVC_crop_N_uptake_time_step_sum
#define  YLY_added_auto_N_in_uptake     CSVC_crop_N_automatic_uptake_time_step_sum
#define  YLY_mineralized_from_residue   CSVC_residue_mineralized_time_step_sum
#define  YLY_mineralized_from_org_fert  CSVC_residue_manure_mineralized_time_step_sum
#define  YLY_mineralized_OM             CSVC_organic_matter_mineralized_time_step_sum
#define  YLY_N_immobilization           CSVC_soil_N_immobilization_time_step_sum
#define  YLY_total_mineralization       CSVC_misc_N_total_mineralization_time_step_sum
#define  YLY_organic_N_applied          CSVC_mgmt_N_organic_applied_time_step_sum
#define  YLY_organic_N_gaseous_loss     CSVC_mgmt_N_organic_gaseous_loss_time_step_sum
#define  YLY_inorganic_N_gaseous_loss   CSVC_mgmt_N_inorganic_gaseous_loss_time_step_sum
#define  xYLY_N_fixation                 CSVC_crop_N_fixation_time_step_sum

//130918 #define  YLY_soil_carbon_profile        CSVC_soil_C_all_profile
#define  YLY_soil_carbon_30cm           CSVC_soil_C_all_30cm
#define  YLY_biomass_grazed             CSVC_mgmt_biomass_grazed_time_step_sum

#define  GS_crop_name                  CSVC_crop_base_name
#define  GS_planting_date_YMD          CSVC_crop_base_planting_date_YMD
#define  GS_restart_date_YMD          CSVC_crop_base_restart_date_YMD
#define  GS_emergence_date_YMD         CSVC_crop_base_emergence_date_YMD
#define  GS_flowering_date_YMD         CSVC_crop_base_flowering_date_YMD
#define  GS_grain_filling_date_YMD     CSVC_crop_base_grain_filling_date_YMD
#define  GS_peak_LAI_date_YMD          CSVC_crop_base_peak_LAI_date_YMD
#define  GS_maturity_date_YMD          CSVC_crop_base_maturity_date_YMD
#define  GS_harvest_date_YMD           CSVC_crop_base_harvest_date_YMD
#define  GS_planting_date_YD           CSVC_crop_base_planting_date_YD
#define  GS_restart_date_YD           CSVC_crop_base_restart_date_YD
#define  GS_emergence_date_YD          CSVC_crop_base_emergence_date_YD
#define  GS_flowering_date_YD          CSVC_crop_base_flowering_date_YD
#define  GS_grain_filling_date_YD      CSVC_crop_base_grain_filling_date_YD
#define  GS_peak_LAI_date_YD           CSVC_crop_base_peak_LAI_date_YD
#define  GS_maturity_date_YD           CSVC_crop_base_maturity_date_YD
#define  GS_harvest_date_YD            CSVC_crop_base_harvest_date_YD
#define  GS_peak_LAI                   CSVC_crop_base_peak_LAI_time_step_max
#define  GS_yield                      CSVC_crop_biomass_yield_period_sum
#define  GS_used_biomass               CSVC_crop_biomass_useful_period_sum
#define  GS_grazed_biomass             CSVC_crop_biomass_removed_grazing_period_sum
#define  GS_residue_biomass            CSVC_crop_biomass_removed_residue_period_sum
#define  GS_disposed_biomass           CSVC_crop_biomass_disposal_period_sum
#define  GS_daytime_VPD_avg            CSVC_crop_base_VPD_daytime_period_avg
#define  GS_intercepted_PAR_accum      CSVC_crop_base_intercepted_PAR_period_sum
#define  GS_root_depth                 CSVC_crop_base_root_depth
#define  GS_surface_residue_biomass    CSVC_residue_surface_biomass
#define  GS_incorporated_plant_residue CSVC_residue_plant_incorporated
#define  GS_incorporated_manure        CSVC_residue_manure_incorporated
#define  GS_erosion_index              CSVC_soil_erosion_index_period_sum

// Warning I think these need to be _period_accum not time_step_sum
#define  GS_soil_loss                  CSVC_soil_erosion_loss_period_sum
#define  GS_ET_max                     CSVC_weather_ET_max_period_sum
//190812#define  GS_ET_pot                     CSVC_weather_ET_pot_period_sum
#define  GS_ET_act                     CSVC_weather_ET_act_period_sum
#define  GS_ET_ref                     CSVC_weather_ET_ref_period_sum
#define  GS_transp_pot                 CSVC_crop_base_transp_pot_period_sum
#define  GS_transp_act                 CSVC_crop_base_transp_act_period_sum
#define  GS_evap_soil_pot              CSVC_soil_base_evap_pot_period_sum
#define  GS_evap_soil_act              CSVC_soil_base_evap_act_period_sum
#define  GS_evap_residue_pot           CSVC_residue_pot_evap_period_sum
#define  GS_evap_residue_act           CSVC_residue_act_evap_period_sum
#define  GS_irrig                      CSVC_mgmt_irrig_applied_period_sum
#define  GS_precip                     CSVC_weather_precipitation_period_sum
#define  GS_crop_water_intrcpt         CSVC_crop_base_water_intrcpt_period_sum
#define  GS_residue_water_intrcpt      CSVC_residue_water_intrcpt_period_sum
#define  GS_surface_water_runoff      CSVC_soil_runoff_surface_water_runoff_period_sum
#define  GS_ponded_water               CSVC_soil_pond_water_depth
#define  GS_water_enter_soil           CSVC_soil_base_water_entering_period_sum
#define  GS_soil_water_drainage        CSVC_soil_base_water_drainage_period_sum
#define  GS_avg_water_stress_index     CSVC_crop_base_water_stress_index_period_avg
#define  GS_avg_temperature_stress_index CSVC_crop_base_temperature_stress_index_period_avg
#define  GS_nitrogen_leached_total     CSVC_soil_N_leached_period_sum
#define  GS_nitrogen_applied_total     CSVC_mgmt_N_applied_period_sum

