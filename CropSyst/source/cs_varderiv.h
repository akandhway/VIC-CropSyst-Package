#ifndef cs_varderivH
#define cs_varderivH

// CropSyst variable derivations.

//#define         0000xxx CSVC_Weather_mask
// CSVCG = CropSyst Variable Class Group (The upper nyble of the variable class)
//                The CSVCG is not used for composing variable codes, but is used for user interface grouping
// CSVC  = CropSyst Variable Class  // I.e. preipc can be echoed to output file

#define CSVCG_weather            0x0      /*atmospheric*/
#define     CSC_weather          0x00
#define        CSV_weather_precipitation              0x01
#define        CSV_weather_snow_storage               0x02
#define        CSV_weather_air_temperature            0x03
#define        CSV_weather_solar_radiation            0x04

#define     CSC_weather_VPD      0x01
#define     CSC_weather_ET       0x02
#  define      CSV_weather_ET_max                     0x01
#  define      CSV_weather_ET_act                     0x02
// Reference short crop and tall crop
#  define      CSV_weather_ET_ref_short         0x03
//NYN          CSV_weather_ET_ref_tall          0x04

#define     CSC_weather_date    0x03
#  define      CSV_weather_date_year_day              0x00
// (this is date num format (I.e. 2000001 is Jan first)
#  define      CSV_weather_date_year                  0x01
#  define      CSV_weather_date_DOY                   0x02
#  define      CSV_weather_date_month                 0x03
#  define      CSV_weather_date_DOM                   0x04

//#define         001xxxx  CSVC_Soil_mask
//#define    CSVC_reserved      0x10
#define CSVCG_soil               0x1
#define     CSC_soil_base        0x11
// define      CSV_soil_00                   0x00
#  define      CSV_soil_01      0x01
#  define      CSV_soil_base_water_entering  0x02
#  define      CSV_soil_base_water_drainage  0x03
#  define      CSV_soil_base_evap_pot        0x04
#  define      CSV_soil_base_evap_act        0x05
// define      CSV_soil_06                   0x06
// define      CSV_soil_05                   0x07
// define      CSV_soil_08                   0x08
#  define      CSV_soil_base_water_depletion_balance 0x09
#  define      CSV_soil_base_water_balance   0x0A
#  define      CSV_soil_salinity             0x0B
#  define      CSV_soil_salt                 0x0C
#  define      CSV_soil_base_water_depletion_seasonal 0x0D
// define      CSV_soil_0E                   0x0E
// define      CSV_soil_0F                   0x0F

// The following are by sublayer indexed from 1 to 32
// Index 0x_0 is for the total soil profile.
// define      CSV_soil_10                   0x10
#  define      CSV_soil_base_water_content    0x11
#  define      CSV_soil_base_temperature          0x12
#  define      CSV_soil_water_potential      0x13
#  define      CSV_soil_lateral_in_flow_depth   0x14
#  define      CSV_soil_lateral_out_flow_depth  0x15
#  define      CSV_soil_base_plant_avail_water    0x16
#  define      CSV_soil_water_depth               0x17
// define      CSV_soil_18                   0x18
// define      CSV_soil_19                   0x19
// The following are soil hydraulic properties
//# define       CSV_soil_1A                   0x1A
//# define       CSV_soil_1B                   0x1B
//# define       CSV_soil_1C                   0x1C
#  define      CSV_soil_field_capacity             0x1D
# define       CSV_soil_permanent_wilt_point       0x1E
# define       CSV_soil_saturation_water_content   0x1F


#define CSC_soil_carbon_mass       0x12
// This is total carbon from all organic matter types

#  define      CSV_soil_carbon_surface       0x00
#  define      CSV_soil_carbon_profile       0x01
#  define      CSV_soil_carbon_5cm           0x02
#  define      CSV_soil_carbon_10cm          0x06
#  define      CSV_soil_carbon_15cm          0x07
#  define      CSV_soil_carbon_30cm          0x03
#  define      CSV_soil_carbon_60cm          0x04
#  define      CSV_soil_carbon_90cm          0x05

// The following are labile + metastable + passive ??+microbial??
                                          // 0x10  May use for surface SOM (currently there is no such thing)
#  define      CSV_SOM_C_mass_profile        0x11
#  define      CSV_SOM_C_mass_5cm            0x12
#  define      CSV_SOM_C_mass_10cm           0x16
#  define      CSV_SOM_C_mass_15cm           0x17
#  define      CSV_SOM_C_mass_30cm           0x13
#  define      CSV_SOM_C_mass_60cm           0x14
#  define      CSV_SOM_C_mass_90cm           0x15
                                          // 0x14-0x1F available for future use (I.e. sums to other depths)
// The following are for the different pools
                                          // 0x20  May use for surface microbial (currently there is no such thing)
#  define      CSV_microbial_C_mass_profile  0x21
#  define      CSV_microbial_C_mass_5cm      0x22
#  define      CSV_microbial_C_mass_10cm     0x26
#  define      CSV_microbial_C_mass_15cm     0x27
#  define      CSV_microbial_C_mass_30cm     0x23
#  define      CSV_microbial_C_mass_60cm     0x24
#  define      CSV_microbial_C_mass_90cm     0x25
                                          // 0x24-0x2F available for future use (I.e. sums to other depths)
                                          // 0x30  May use for surface labile (currently there is no such thing)
#  define      CSV_labile_C_mass_profile     0x31
#  define      CSV_labile_C_mass_5cm         0x32
#  define      CSV_labile_C_mass_10cm        0x36
#  define      CSV_labile_C_mass_15cm        0x37
#  define      CSV_labile_C_mass_30cm        0x33
#  define      CSV_labile_C_mass_60cm        0x34
#  define      CSV_labile_C_mass_90cm        0x35
                                          // 0x34-0x3F available for future use (I.e. sums to other depths)
                                          // 0x40  May use for surface metastable (currently there is no such thing)
#  define      CSV_metastable_C_mass_profile 0x41
#  define      CSV_metastable_C_mass_5cm     0x42
#  define      CSV_metastable_C_mass_10cm    0x46
#  define      CSV_metastable_C_mass_15cm    0x47
#  define      CSV_metastable_C_mass_30cm    0x43
#  define      CSV_metastable_C_mass_60cm    0x44
#  define      CSV_metastable_C_mass_90cm    0x45
                                          // 0x44-0x4F available for future use (I.e. sums to other depths)
                                          // 0x50  May use for surface passive (currently there is no such thing)
#  define      CSV_passive_C_mass_profile    0x51
#  define      CSV_passive_C_mass_5cm        0x52
#  define      CSV_passive_C_mass_10cm       0x56
#  define      CSV_passive_C_mass_15cm       0x57
#  define      CSV_passive_C_mass_30cm       0x53
#  define      CSV_passive_C_mass_60cm       0x54
#  define      CSV_passive_C_mass_90cm       0x55
                                          // 0x56-0x5F available for future use (I.e. sums to other depths)
#  define      CSV_residue_C_mass_surface    0x60
#  define      CSV_residue_C_mass_profile    0x61
#  define      CSV_residue_C_mass_5cm        0x62
#  define      CSV_residue_C_mass_10cm       0x66
#  define      CSV_residue_C_mass_15cm       0x67
#  define      CSV_residue_C_mass_30cm       0x63
#  define      CSV_residue_C_mass_60cm       0x64
#  define      CSV_residue_C_mass_90cm       0x65
                                          // 0x66-0x6F reserved for possible total Residue C mass to various depths
                                          // 0x70-0x7F reserved for possible stubble residue C mass to various depths
                                          // 0x80-0x8F reserved for possible manure C mass to various depths
                                          // 0x90-0x9F available for future use
                                          // Note that the enumeration A0-A3 is the same order as Organic_matter_position
#  define    CSV_C_decomposed_residue_subsurface   0xA0
#  define    CSV_C_decomposed_residue_flat         0xA1
#  define    CSV_C_decomposed_residue_stubble      0xA2
#  define    CSV_C_decomposed_residue_attached     0xA3
                                                   // 0xBx fate due to management
#  define    CSV_C_incorporated_by_tillage         0xB0
#  define    CSV_C_removed_by_harvest              0xB1


//#  define      CSV_C_decomposed_residue_profile    0xC1
#  define      CSV_C_decomposed_residue_5cm        0xC2
#  define      CSV_C_decomposed_residue_10cm       0xC6
#  define      CSV_C_decomposed_residue_15cm       0xC7
#  define      CSV_C_decomposed_residue_30cm       0xC3
#  define      CSV_C_decomposed_residue_60cm       0xC4
#  define      CSV_C_decomposed_residue_90cm       0xC5

                                          // 0xC6-0xCF available for future use
                                                //
#  define      CSV_C_decomposed_SOM_profile    0xD1
#  define      CSV_C_decomposed_SOM_5cm        0xD2
#  define      CSV_C_decomposed_SOM_10cm       0xD6
#  define      CSV_C_decomposed_SOM_15cm       0xD7
#  define      CSV_C_decomposed_SOM_30cm       0xD3
#  define      CSV_C_decomposed_SOM_60cm       0xD4
#  define      CSV_C_decomposed_SOM_90cm       0xD5

                                          // 0xD6-0xFF available for future use


#define CSC_soil_nitrogen     0x13
// define      CSV_soil_N_00                 0x00
#  define      CSV_soil_N_leached            0x01
#  define      CSV_soil_N_available          0x02

#  define      CSV_soil_NH3_volatilization   0x03
#  define      CSV_soil_volatilization_total 0x04
// define      CSV_soil_N_05                 0x05
// define      CSV_soil_N_06                 0x06
// define      CSV_soil_N_07                 0x07
// define      CSV_soil_N_08                 0x08
// define      CSV_soil_N_09                 0x09
// define      CSV_soil_N_0A                 0x0A
#  define      CSV_soil_N_N2O_loss_denitrification 0x0B
#  define      CSV_soil_N_N2O_loss_nitrification   0x0C
// The following are soil N balance added for Luca VATE
#  define      CSV_soil_N_input               0x0D
#  define      CSV_soil_N_output              0x0E
//  define      CSV_soil_N_0C                0x0F // This one may be for balance



// The following are by sublayer indexed from 1 to 32
// Index 0x_0 is for the total soil profile.
#  define      CSV_soil_N_amount             0x10
#  define      CSV_soil_N_NO3_amount         0x11
#  define      CSV_soil_N_NH4_amount         0x12
//061116  Soil mineralization and is becoming obsolete.
// This is now a property of the organic matter
#  define      CSV_soil_N_mineralization     0x13
#  define      CSV_soil_N_nitrification      0x14
#  define      CSV_soil_N_denitrification    0x15
#  define      CSV_soil_N_immobilization     0x16

// The following are sums to the specified depth
#  define      CSV_soil_N_immobilization_profile      0x61
#  define      CSV_soil_N_immobilization_5cm          0x62
#  define      CSV_soil_N_immobilization_10cm         0x66
#  define      CSV_soil_N_immobilization_15cm         0x67
#  define      CSV_soil_N_immobilization_30cm         0x63
#  define      CSV_soil_N_immobilization_60cm         0x64
#  define      CSV_soil_N_immobilization_90cm         0x65


#define CSC_soil_freezing  0x14

#define CSC_soil_erosion   0x15
#  define      CSV_soil_erosion_clod_rate             0x01
#  define      CSV_soil_erosion_loss                  0x02
#  define      CSV_soil_erosion_index                 0x03
// reserved for future use
#  define      CSV_soil_RUSLE_C_factor                0x10
#  define      CSV_soil_RUSLE_erosivity               0x11

#define CSC_soil_runoff   0x16
#  define      CSV_soil_runoff_surface_water_runoff   0x01
#  define      CSV_soil_runoff_surface_water_runon    0x02
//             This runon is an input introduced for watershed model

#define CSC_soil_complete 0x17

#define     CSC_soil_pond     0x18
#define        CSV_soil_pond_water_depth              0x00

#define CSC_soil_phosphorus   0x19
   // The following are by sublayer indexed from 1 to 32
   // Index 0x_0 is for the total soil profile.
   // Currently only soil profile is used 110801
#  define      CSV_soil_P_amount                0x10

#define CSVCG_crop            0x2
#define     CSC_crop_base     0x21
//             base_crop_output_variables
#  define      CSV_crop_base_name                     0x00 /* This is the crop description */
#  define      CSV_crop_base_growing_degree_days      0x01 /* accumulated growing degree days adjusted for clipping and physiologic response (degree day for the day is not output)*/
#  define      CSV_crop_base_dry_biomass_deprecate    0x02 /* current biomass obtained directly from sum_daily_canopy_biomass was CSV_crop_base_dry_biomass */
#  define      CSV_crop_base_root_biomass_deprecate   0x03
#  define      CSV_crop_base_leaf_area_index          0x04
#  define      CSV_crop_base_green_area_index         0x05
#  define      CSV_crop_base_root_depth               0x06
#  define      CSV_crop_base_sum_daily_canopy_biomass_deprecate  0x07 /* this is the total biomass produced */
//#  define      CSV_crop_base_water_stress             0x08     Moved to  0x41
#  define      CSV_crop_season_duration               0x08
#  define      CSV_crop_base_leaf_water_pot           0x09
#  define      CSV_crop_base_fract_canopy_cover       0x0A
#  define      CSV_crop_base_transp_pot               0x0B
#  define      CSV_crop_base_transp_act               0x0C
#  define      CSV_crop_base_water_inter              0x0D
#  define      CSV_crop_phenology_growth_stage        0x0E
#  define      CSV_crop_seasonal_thermal_time         0x0F  /* total accumulated degree days */



#if (CROPSYST_VERSION >= 5)
#define CSC_crop_phenology    0x25
#else
// V4 did not have separate section for phenology
#define CSC_crop_phenology    0x21
#endif

// Crop phenology dates formatted YMD are reserved 0x10 - 0x1F
#define        CSV_crop_phenology_planting_date_YMD        0x10
#define        CSV_crop_phenology_emergence_date_YMD       0x11
#define        CSV_crop_phenology_flowering_date_YMD       0x12
#define        CSV_crop_phenology_grain_filling_date_YMD   0x13
#define        CSV_crop_phenology_peak_LAI_date_YMD        0x14
#define        CSV_crop_phenology_maturity_date_YMD        0x15
#define        CSV_crop_phenology_harvest_date_YMD         0x16
#define        CSV_crop_phenology_restart_date_YMD         0x17
#define        CSV_crop_phenology_bud_break_date_YMD                      0x18
#define        CSV_crop_phenology_initial_fruit_growth_date_YMD           0x19
#define        CSV_crop_phenology_rapid_fruit_growth_date_YMD             0x1A
#define        CSV_crop_phenology_inactive_begin_date_YMD                 0x1B
#define        CSV_crop_phenology_inactive_end_date_YMD                   0x1C
#define        CSV_crop_phenology_chill_requirement_satisfied_date_YMD    0x1D
#define        CSV_crop_phenology_tuber_init_date_YMD                     0x1E

// 0x1E-0x1F reserved for additional dates
// Crop phenology dates formatted YD are reserved 0x20 - 0x2F
#define        CSV_crop_phenology_planting_date_YD         0x20
#define        CSV_crop_phenology_emergence_date_YD        0x21
#define        CSV_crop_phenology_flowering_date_YD        0x22
#define        CSV_crop_phenology_grain_filling_date_YD    0x23
#define        CSV_crop_phenology_peak_LAI_date_YD         0x24
#define        CSV_crop_phenology_maturity_date_YD         0x25
#define        CSV_crop_phenology_harvest_date_YD          0x26
#define        CSV_crop_phenology_restart_date_YD          0x27
#define        CSV_crop_phenology_bud_break_date_YD                    0x28
#define        CSV_crop_phenology_initial_fruit_growth_date_YD         0x29
#define        CSV_crop_phenology_rapid_fruit_growth_date_YD           0x2A
#define        CSV_crop_phenology_inactive_begin_date_YD               0x2B
#define        CSV_crop_phenology_inactive_end_date_YD                 0x2C
#define        CSV_crop_phenology_chill_requirement_satisfied_date_YD  0x2D
#define        CSV_crop_phenology_tuber_init_date_YD                   0x2E



/*
// Crop phenology dates formatted YMD are reserved 0x10 - 0x1F
#define        CSV_crop_base_planting_date_YMD        0x10
#define        CSV_crop_base_emergence_date_YMD       0x11
#define        CSV_crop_base_flowering_date_YMD       0x12
#define        CSV_crop_base_grain_filling_date_YMD   0x13
#define        CSV_crop_base_peak_LAI_date_YMD        0x14
#define        CSV_crop_base_maturity_date_YMD        0x15
#define        CSV_crop_base_harvest_date_YMD         0x16
#define        CSV_crop_base_restart_date_YMD         0x17
#define        CSV_crop_base_bud_break_date_YMD                      0x18
#define        CSV_crop_base_initial_fruit_growth_date_YMD           0x19
#define        CSV_crop_base_rapid_fruit_growth_date_YMD             0x1A
#define        CSV_crop_base_inactive_begin_date_YMD                 0x1B
#define        CSV_crop_base_inactive_end_date_YMD                   0x1C
#define        CSV_crop_base_chill_requirement_satisfied_date_YMD    0x1D
#define        CSV_crop_base_tuber_init_date_YMD                     0x1E

// 0x1E-0x1F reserved for additional dates
// Crop phenology dates formatted YD are reserved 0x20 - 0x2F
#define        CSV_crop_base_planting_date_YD         0x20
#define        CSV_crop_base_emergence_date_YD        0x21
#define        CSV_crop_base_flowering_date_YD        0x22
#define        CSV_crop_base_grain_filling_date_YD    0x23
#define        CSV_crop_base_peak_LAI_date_YD         0x24
#define        CSV_crop_base_maturity_date_YD         0x25
#define        CSV_crop_base_harvest_date_YD          0x26
#define        CSV_crop_base_restart_date_YD          0x27
#define        CSV_crop_base_bud_break_date_YD                    0x28
#define        CSV_crop_base_initial_fruit_growth_date_YD         0x29
#define        CSV_crop_base_rapid_fruit_growth_date_YD           0x2A
#define        CSV_crop_base_inactive_begin_date_YD               0x2B
#define        CSV_crop_base_inactive_end_date_YD                 0x2C
#define        CSV_crop_base_chill_requirement_satisfied_date_YD  0x2D
#define        CSV_crop_base_tuber_init_date_YD                   0x2E
*/

// 0x28-0x2F reserved for additional dates
// Crop harvest/Biomatter fate (one time or intermittent)
//070206 #define        CSV_crop_base_yield                    0x30  now xxxx_fate
#define        CSV_crop_base_peak_LAI                 0x31
//070206 #define			CSV_crop_base_used_biomass			      0x32  now xxxx_fate
//070206 #define			CSV_crop_base_grazed_biomass			   0x33  now xxxx_fate
//070206 #define			CSV_crop_base_residue_biomass			   0x34  now xxxx_fate
//070206 #define			CSV_crop_base_disposed_biomass			0x35  now xxxx_fate
//070206 #define			CSV_crop_base_residue_stubble_biomass  0x36  now xxxx_fate
#define			CSV_crop_base_green_biomass            0x37
/* was CSV_crop_base_live_biomass */
// 0x38-0x3F  reserved for future biomasses
#define        CSV_crop_base_VPD                      0x40
#define        CSV_crop_base_intercepted_PAR          0x41

#  define      CSV_crop_base_water_stress_factor      0x42
#  define      CSV_crop_base_water_stress_index       0x43
#  define      CSV_crop_base_temperature_stress_factor 0x44
#  define      CSV_crop_base_temperature_stress_index 0x45
#  define      CSV_crop_base_tuber_init_temperature_stress_index 0x46
#  define      CSV_crop_base_tuber_filling_temperature_stress_index 0x47

#  define      CSV_crop_base_flower_temperature_stress_index 0x48


//                                                    0x48 to 0x4F reserved for future stress ans stress index (index are odd)

// Fruit model
#define        CSV_crop_fruit_biomass                 0x50
#define        CSV_crop_fruit_canopy_biomass          0x51
#define        CSV_crop_fruit_reserves_biomass        0x52
//#define
// Orchard
#define        CSV_orchard_solid_canopy_interception  0x5A
#define        CSV_orchard_shaded_area_GAI            0x5B
#define        CSV_canopy_porosity                    0x5C

#define        CSV_root_fraction                      0x60
// 0x61-0x7F The root fraction is by sublayer indexed from 1 to 32  (0x60 is not current used itself, but could be the total (which should be 1.0)



//   nitrogen_crop_output_variables
#define    CSC_crop_nitrogen_misc 0x23
// masses
#  define      CSV_crop_N_00                 0x00
#  define      CSV_crop_N_daily_fert_requirements   0x01      /* series at time step*/
#  define      CSV_crop_N_stress_factor      0x02      /* series at time step*/

//07006 never used #  define      CSV_crop_N_top_plant          0x03     /* series (at time step) although accumulated, it is the value at the time step (it can up and down)*/
//070206 not sure how different from CSV_crop_N_above_ground
// eventually replace with      CSC_crop_N_mass CSV_total_above_ground

#  define      CSV_crop_N_plant_concentration_obsolete      0x04     /* series at time step */   // Eventually replace with CSV_crop_N_canopy_concentration
#  define      CSV_crop_N_fixation           0x05     /* series total for time step */
#  define      CSV_crop_N_uptake             0x06

//110915 #  define      CSV_crop_N_above_ground_renamed      0x07   /* renamed to CSV_crop_N_canopy_assimilation */
//110915 #  define      CSV_crop_N_canopy_assimilation       0x07
#  define      CSV_crop_N_mass_canopy_production   0x07

// This is the N in the current biomass plus any that was removed by clipping or other loss  ( was CSV_crop_N_above_ground)


#  define      CSV_crop_N_automatic_uptake   0x08


#  define      CSV_crop_N_stress_index       0x09      /* series at time step*/
#  define      CSV_crop_N_root_V3               0x0A    // This only appears in V3,   V4 now uses CSV_crop_root_live
#  define      CSV_crop_N_canopy_gaseous_loss      0x0B   // This only appears in V3 not sure why not in V4
#  define      CSV_crop_N_balance_V3               0x0C    // This only appears in V3 not sure why not in V4
#  define      CSV_crop_N_flower_mature_conc_ratio 0x0D
#  define      CSV_crop_N_uptake_excess_or_deficit 0x0E
#  define      CSV_crop_N_mass_canopy_current      0x0F
                                             //    0x10-0x14   available


// qualifiable  , CSV_avg_nitrogen_stress_index        // series accumulated for period

// not curr output    , CSV_N_stored                         // series (at time step) although accumulated, it is the value at the time step (it can up and down)
// not curr output    , CSV_seedling_N                       // scalar at time step
// not curr output   , CSV_total_removed_plant_N            // series total for time step
// not curr output directly    , CSV_plant_N                          // series (at time step) although accumulated, it is the value at the time step (it can up and down)
// not curr output directly   , CSV_gaseous_N_loss                   // series total for time step

/* 070206 NYI
// available #  define CSV_crop_N_xxxxxx           0x15    // was root concentration
#  define      CSV_crop_N_yield_concentration      0x20
// available #  define      CSV_crop_N_XXXXX_concentration      0x17
#  define      CSV_crop_N_beneficial_use_concentration 0x17
#  define      CSV_crop_N_disposal_concentration   0x18
#  define      CSV_crop_N_chaff_concentration      0x19
#  define      CSV_crop_N_stubble_concentration    0x1A
#  define      CSV_crop_N_live_canopy_concentration 0x1B
#  define      CSV_crop_N_grazing_to_manure_concentration 0x1C
#  define      CSV_crop_N_grazing_to_disposal_concentration 0x1D
// available #  define CSV_crop_N_xxxxxx_concentration           0x1E
// available #  define CSV_crop_N_xxxxxx_concentration           0x1F
*/
//#  define      CSV_crop_N_yield                    0x46
/* 070206 NYI
#  define      CSV_crop_N_beneficial_use           0x47
#  define      CSV_crop_N_disposal_concentration   0x48
#  define      CSV_crop_N_chaff_concentration      0x49
#  define      CSV_crop_N_stubble_concentration    0x4A
#  define      CSV_crop_N_live_canopy              0x4B
#  define      CSV_crop_N_grazing_to_manure        0x4C
#  define      CSV_crop_N_grazing_to_disposal      0x4D
// available #  define CSV_crop_N_xxxxxx           0x4E
// available #  define CSV_crop_N_xxxxxx           0x4F

// Concentrations


#  define      CSV_crop_N_root                     0x20
// 0x21-0x3F reserved for root N mass by layer
// 0x40-0x4F   is available but reserve if possible if we need more than 31 layers

#  define      CSV_crop_N_root_concentration       0x50      series at time step   // Eventually replace with CSV_crop_N_canopy_concentration
// 0x51 to 0x6F reserved for root concentration by layer

// 0x70-0x7F  is available but reserve if possible if we need more than 31 layers

*/

#define    CSC_crop_phosphorus_misc 0x24
#  define      CSV_crop_P_uptake             0x06


// 0x80-0xFF   available


// For each of these CSC codes, the CSV are the same.
// Carbon output variables
#define    CSC_crop_biomass            0x25
#define    CSC_crop_nitrogen_mass      0x26
#define    CSC_crop_nitrogen_conc      0x27
#define    CSC_crop_carbon_mass        0x28
//#define    CSC_crop_phosphorus_mass    0x29


// They are composed from the above ground fate bitmask codes
// 0x00-0x07 are the specific parts of the plant distributed
//    they are generated by make_csvc

// 0x08-0x1F are reserved for future use

// 0x20-0x4F are combinations but currently only the following are significant
#define     CSV_plant                           0x20  // was  CSC_crop_nitrogen CSV_crop_N_plant_concentration
#define     CSV_canopy                          0x21
#define     CSV_removed_canopy                  0x22
#define     CSV_removed_non_yield               0x23
#define     CSV_removed_residue                 0x24
#define     CSV_removed_grazing                 0x25

//BITMASK_REMOVED_ABOVE_GROUND   (BITMASK_YIELD | BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
//BITMASK_REMOVED_NON_YIELD      (                BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
//BITMASK_REMOVED_RESIDUE        (                                                  BITMASK_CHAFF | INDEX_STUBBLE)
//BITMASK_REMOVED_GRAZING        (                                                                                  BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL)
//BITMASK_TOTAL_ABOVE_GROUND     (BITMASK_YIELD | BITMASK_USED | BITMASK_DISPOSAL | BITMASK_CHAFF | INDEX_STUBBLE | BITMASK_GRAZED_MANURE | BITMASK_GRAZED_DISPOSAL | BITMASK_LIVE_CANOPY)


// 0x08-0x1F are combinations but currently only only the following are significant

#  define      CSV_crop_roots_live                0x50
// 0x50 is for the entire profile
// 0x51-0x6F are for roots by layer currently CS only uses 31 layer max
// 0x70-0x7F reserved are for roots by layer incase we need more than 31 layers

#  define      CSV_crop_roots_dead                0x80
// 0x80 is for the entire profile
// 0x81-0x9f are for roots by layer currently CS only uses 31 layer max

// 0xA0-0xFF are miscellaneous common
//NYN #  define      CSV_mean                          0xA0
// These are are currently used only for Crop Nitrogen concentration
#  define      CSV_minimum                       0xA1
#  define      CSV_maximum                       0xA2
#  define      CSV_critical                      0xA3

// #define    CSC_crop_fate_carbon     0x27   may be needed at some time

/*
// The temperature extremes a special output.
// Eventually this will be removed from CropSyst and implemented
// as a standalone utility.
// The low 7 bits of the CSV (CSPROP) of the composed variable code
// is the temperature (integer I.e. 34 Celcius
// bit 6 is the sign.
// bit 7 indicates high temperature
// bits 8 to 11 is the initial growth stage (value of 0 indicates the whole season
// bits 12 to 15 would be final stage (not yet needed)

 151211  8 7 6 5    0
+----+----+-+-+------+
|0000| ngs|h|s| temp |
+----+----|-+-+------+
*/
#define CSC_crop_extreme_temperature  0x28

//                                                 0x00-0x0F are reserved for specific

// For the following fate variables are common to biomass, nitrogen, carbon, etc.
// The following are atomic above ground and/or yield which may be below ground (I.e. tuber or peanut)
/* Obsolete now composing these from Above ground bitmasks
#define        CSV_crop_base_yield_fate            0x00
#define			CSV_crop_base_used_fate             0x01
#define			CSV_crop_base_disposed_fate         0x02
#define			CSV_crop_base_chaff_fate	         0x03
#define			CSV_crop_base_stubble_fate          0x04
#define			CSV_crop_base_grazed_manure_fate	   0x05
#define			CSV_crop_base_grazed_disposal_fate  0x06
#define			CSV_crop_base_live_fate             0x07
*/

#  define     CSV_balance                        0xF0
// balance applies only to masses not to concentration


#define    CSC_crop_complete     0x2A



//#define         011xxxx  CSVC_Residues_mask
//==============================================================================
#define CSVCG_residue        0x3
//------------------------------------------------------------------------------
#define    CSC_residue       0x30
   // This is the residue totals (I.e. all the residues considered as a single unit)
// define    CSV_residue_xxx                 0x00
#  define    CSV_residue_ground_cover        0x01
#  define    CSV_residue_water_storage       0x02
#  define    CSV_residue_surface_biomass     0x03
#  define    CSV_residue_XXXXXXXXXXXX        0x04 // was CSV_residue_incorporated
#  define    CSV_residue_pot_evap            0x05
#  define    CSV_residue_act_evap            0x06
#  define    CSV_residue_water_inter         0x07

//Mineralized may need to be by layer
#  define    CSV_residue_mineralizated       0x08
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0A
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0B
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0C
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0D
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0E
//#  define    CSV_residue_XXXXXXXXXXXXXX    0x0F

#  define    CSV_residue_incorporated        0x10

// The following variable codes are by residue horizon (usually soil sublayer) indexed from 1 to 32
// Index 0x_0 is for the total residue profile.


//------------------------------------------------------------------------------
#define    CSC_residue_plant          0x31
// define    CSV_residue_plant_XXX                 0x00
// define    CSV_residue_plant_XXX                 0x01
// define    CSV_residue_plant_XXX                 0x02
#  define    CSV_residue_plant_surface_BM          0x03
#  define    CSV_residue_plant_surface_flat_BM     0x04
#  define    CSV_residue_plant_surface_stubble_BM  0x05
// define    CSV_residue_plant_XXX                 0x06
// define    CSV_residue_plant_XXX                 0x07
// define    CSV_residue_plant_XXX                 0x08
// define    CSV_residue_plant_XXX                 0x09
// define    CSV_residue_plant_XXX                 0x0A
// define    CSV_residue_plant_XXX                 0x0B
// define    CSV_residue_plant_XXX                 0x0C
// define    CSV_residue_plant_XXX                 0x0D
// define    CSV_residue_plant_XXX                 0x0E
// define    CSV_residue_plant_XXX                 0x0F

#  define    CSV_residue_plant_incorporated        0x10
                                    //             0x11 - 0xFF reserved for sublayers and future sublayer variables

#define    CSC_residue_manure        0x32
// define    CSV_residue_manure_XXX           0x00
// now 0x08 to be consistent with residue #  define    CSV_residue_manure_mineralizated 0x01
// define    CSV_residue_manure_XXX           0x02
#define    CSV_residue_manure_surface_BM      0x03
// define    CSV_residue_manure_XXX           0x04
// define    CSV_residue_manure_XXX           0x05
// define    CSV_residue_manure_XXX           0x06
// define    CSV_residue_manure_XXX           0x07
# define    CSV_residue_manure_mineralizated  0x08
///define    CSV_residue_manure_XXX           0x09
///define    CSV_residue_manure_XXX           0x0A
///define    CSV_residue_manure_XXX           0x0B
// define    CSV_residue_manure_XXX           0x0C
// define    CSV_residue_manure_XXX           0x0D
// define    CSV_residue_manure_XXX           0x0E
// define    CSV_residue_manure_XXX           0x0F

#  define    CSV_residue_manure_incorporated 0x10
                                    //       0x10 - 0xFF reserved for sublayers and future sublayer variables

//------------------------------------------------------------------------------
#define  CSC_organic_matter           0x35

#  define      CSV_organic_matter_percent          0x01
#  define      CSV_organic_matter_biomass          0x02
//                                                 0x03 0x0C availabe
#  define      CSV_gaseous_CO2_C_loss_residue      0x0D
#  define      CSV_gaseous_CO2_C_loss_SOM          0x0E
#  define      CSV_gaseous_CO2_C_loss_total        0x0F
#  define      CSV_N_mineralization                0x10
                                    //             0x11 - 0x3F could be reserved for sublayers and future sublayer variables

#  define      CSV_N_mineralization_profile        0x41
#  define      CSV_N_mineralization_5cm            0x42
#  define      CSV_N_mineralization_10cm           0x46
#  define      CSV_N_mineralization_15cm           0x47
#  define      CSV_N_mineralization_30cm           0x43
#  define      CSV_N_mineralization_60cm           0x44
#  define      CSV_N_mineralization_90cm           0x45

                                    //        xxx - 0xFF available
//#define         100xxxx  CSVC_Watertable_mask
//------------------------------------------------------------------------------
#define CSVCG_water_table           0x4
#define    CSC_water_table         0x41
#define      CSV_water_table_depth         0x01

//------------------------------------------------------------------------------
#define CSVCG_management               0x5
#define     CSC_management            0x50
#define        CSV_mgmt_biomass_grazed             0x01
#define        CSV_mgmt_biomass_unsatisfied        0x02

//------------------------------------------------------------------------------
#define     CSC_management_irrigation 0x51
#define        CSV_mgmt_irrigation                 0x01

//------------------------------------------------------------------------------
#define     CSC_management_N          0x52
#define        CSV_mgmt_N_applied                  0x01
#define        CSV_mgmt_organic_N_applied          0x02
#define        CSV_mgmt_N_organic_gaseous_loss     0x03
#define        CSV_mgmt_N_inorganic_gaseous_loss   0x04
#define     CSC_management_NH4        0x53
#define     CSC_management_NO3        0x54

#define     CSC_management_P          0x56
#define        CSV_mgmt_P_applied                  0x01


//------------------------------------------------------------------------------
#define CSVCG_chemical              0x6
// Each chemical (chemical profile) will be assigned as CSCV (up to 256 chemicals)

// For variables defined outside of the class use
//------------------------------------------------------------------------------
#define CSVCG_other                 0x7
#define     CSC_miscellaneous_N    0x71
#define        CSV_total_mineralization            0x01

#define     CSC_other_output        0x7F

// CSVCG Codes 0x8 are reserved for derived models (I.e. DairySyst)
//______________________________________________________________________________
//The following composes a CropSyst non-standard UED scalar property code
#define CS_PROP(CSCV,CSV)              (((nat32 )CSCV << 16) | (nat32 )CSV)
//The following composes a CropSyst non-standard UED layered property code
#define CS_LAYER_PROP(CSCV,CSV,layer)  (((nat32 )CSCV << 16) | ((((nat32 )CSV) << 8) | (nat32 )layer))
#define CS_UED_compose_var_code(qualifier,CSprop)   (((nat32 )1 << 31) | ((nat32 )qualifier << 24) | (nat32 )CSprop)
#define CS_UED_compose_layer_var_code(qualifier,CSprop)   (((nat32 )1 << 31) | ((nat32 )1 << 30) |((nat32 )qualifier << 24) | (nat32 )CSprop)
//______________________________________________________________________________
#endif
//varderiv.h

