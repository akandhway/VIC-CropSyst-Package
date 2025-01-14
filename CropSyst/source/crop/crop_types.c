#include "crop/crop_types.h"
//______________________________________________________________________________
const char *harvested_part_label_table[] =  // was crop_kind_label_table[] =
{
#if (CROPSYST_VERSION==4)
 "seed"        // "SEED"
#else
 "grain"
#endif
,"root"        // "ROOT"
,"tuber"       // "TUBER"
,"bulb"                                                                          //120505
,"leaf"        // "LEAF"
,"fruit"       // "FRUIT"
,"canopy"      // Essentially all above ground biomass                           //031015
,"complete"    // The entire plant                                               //031015
,"unknown"     // "UNKNOWN"
,0};
//_harvested_part_label_table__________________________________________________/
//Note: if these labels are changed or added, dont forget to add to schedule viewer
const char *crop_model_label_table[] =
{// The following are CropSyst
 "crop"
,"fruit"
// The following are external models
,"cropgro"
,"nwheats"                                                                       //140716
,0};
//_crop_model_label_table___________________________________________2002-04-09_/
const char *life_cycle_label_table[] =
{ "indeterminate"
, "annual"
, "biennial"
, "perennial"                                                                    //080722
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// Included for compatibility
, "perennial_herbaceous"
, "perennial_woody"
#endif
,0};
//_life_cycle_label_table___________________________________________2002-07-16_/
const char *stem_type_label_table[] =
{ "herbaceous"
, "woody"
,0};
//_stem_type_label_table____________________________________________2008-07-22_/
const char * abscission_label_table[]=
{  "none"    // Leaves are retained throughout the growth of the plant I.e. annuals and monocots
,  "deciduous"         // All leaves are shed at the end of the season.

,  "semi-deciduous"    // Similar to marcescent, but leaves may completely shed and not be replaced for a short time.
                       // all leaves will be shed at the start of the season
                       // There will be no effette leaves coexisting with vital leaves.

,  "marcescent"        // Leaves may scenese but are not shed until new growth replaces (knock the leaves off?)
                       // usually as the start of the new season, leaves may persist into successive seasons.
                       // There may be effette leaves coexisting with vital leaves.
                       // May be knocked off by wind.
,  "semi-evergreen"    // Leaves are shed as new replacement growth occurs (continously, not seasonally)
,  "evergreen"         // Persistent year round but individual leaves may scenese
                       // these are generally shed immediately or may be attached to the plant for some specified period.
                       // (individual leaves are not replaced)
};
//_abscission_label_table___________________________________________2011-02-08_/
const char *photosynthetic_pathway_label_table[] =
{"C3"  //0
,"C4"  //1
,"CAM" // Crassulacean Acid Metabolism                                           //090309
,"C2"  //  photorespiration (oxidative photosynthetic carbon cycle)
,0};
//_photosynthetic_pathway_label_table__________________________________________/
const char *row_type_label_table[] =
{"hedge_rows"
,"standard_rows"
,"no_rows"
,0};
//_row_type_label_table_____________________________________________2002-07-16_/
//151109 This is being replaced with Thermal_time_temperature_response
const char *Thermal_time_resolution_label_table_deprecated[] =
{"hourly"
,"daily"
,0};
//_Thermal_time_resolution_label_table_deprecated___________________2002-11-24_/
const char *Thermal_response_label_table[] =
{"nonlinear"
#ifdef THERMAL_RESPONSE_2018
, "linear_up"
, "linear_down"
, "linear_up_cutoff"
, "linear_trapazoidal"
#else
,"linear"
#endif
,0};
//_Thermal_response_label_table_____________________________________2015-11-09_/
const char *emergence_model_label_table[] =
{"thermal_time"
,"hydrothermal_time"
,0};
//_emergence_model_label_table______________________________________2002-07-16_/
//160531 These tables are actually obsolete in V5 but currently V5 is reading V4 files
// #if ((CROPSYST_VERSION >= 1) && (CROPSYST_VERSION < 5))
/*191026 obsolete
const char *transpiration_model_label_table[] =
{"leaf_and_root"
,"original"       // deprecated
,0};
//_transpiration_model_label_table__________________________________2002-07-16_/
*/
const char *senesced_biomass_fate_label_table[] =
{"endure"
,"shed"
,"linger"
,"decompose"
,0};
//_senesced_biomass_fate_label_table________________________________2006-08-17_/
const char *potential_N_uptake_model_label_table[] =
{"original"  // this
,"root_factor"
,0};
//_potential_N_uptake_model_label_table_____________________________2006-12-01_/
//160532 #endif
const char *nitrogen_leaf_stress_model_label_table[] =
{"none"
,"minimum_to_critical_conc"
,"straddle_critical_conc"
,0};
//_nitrogen_leaf_stress_model_label_table___________________________2007-02-23_/
const char *continuation_mode_label_table[] =  //200212 was season_dormancy_mode_label_table
{"single"
,"multiple"
,"dormancy"
,0};
//_continuation_mode_label_table________________________2020-02-13__2008-03-31_/
const char * harvest_condition_mode_label_table[] =
{"management_scheduled"
,"consider_fixed_date"
,"consider_min_temperature"
,"consider_avg_temperature"
,0};
//_harvest_condition_mode_label_table_______________________________2008-12-04_/
const char *TUE_equation_label_table[] =
{"Tanner_Sinclair"
,"water_productivity"
,"TUE_curve"
,0};
//_TUE_equation_label_table_________________________________________2008-03-31_/
const char *canopy_growth_label_table[] =
{
 "leaf_area_index_V5"
,"canopy_cover_2017"
,"leaf_area_index"
,"canopy_cover"
,0};
//_canopy_growth_label_table________________________________________2008-07-25_/
const char *RUE_basis_label_table[] =
{ "global_radiation"
, "photosynthetically_active_radiation"
, "unspecified"
,0};
//_RUE_basis_label_table_________________________________2011-08-25_2008-07-31_/
const char *photoperiodism_label_table[] =
{  "none"       // was none                                               //151108
,  "short-day"
,  "long-day"
,  "intermediate-day"
,  "day-neutral"
, 0};
//_photoperiodism_label_table_______________________________________2010-05-12_/

