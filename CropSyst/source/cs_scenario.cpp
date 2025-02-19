#  include "options.h"
#  include "static_phrases.h"
#  include "cs_scenario.h"
#  include "cs_scenario_directory.h"
#  include "mgmt_param.h"
#  include "corn/data_source/datasrc.h"
#  include "common/biomatter/decomposition_const.h"
#  include "corn/measure/common_units.h"
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#  include "corn/OS/file_system_engine.h"
#  define CORN_Directory_entry_name CORN::OS::Directory_entry_name
#include "corn/data_source/vv_file.h"
#include "corn/container/text_list.h"

#include <math.h>
#ifdef LADSS_MODE
#include "LADSS.h"
extern bool LADSS_mode;
class LADSS_CropSyst_Model;
extern LADSS_CropSyst_Model *LADSS_CropSyst_model;
#endif
#define CropSyst_scenario_superclass  CS::Simulation_control
#if CROPSYST_VERSION==4
#  include "project/GIS/GIS_table.h"
// else GIS setup not uses scenario generator
#endif
#include <stdio.h>

extern bool prompt_denitrification_params;                                       //151110
   // This is temporary until I put these parameters in the scenario editor.

//______________________________________________________________________________
Label_description cropping_system_mode_label_table[] =
{{"none"                ,"No crop"}
,{"single"              ,"Single crop/season"}
,{"rotation"            ,"Rotation"}
,{"established"         ,"Established orchard/vinard"}
,{0,0}};
//_cropping_system_mode_label_table_________________________________2005-11-07_/
namespace CropSyst                                                               //130621
{

CORN::Parameter_range  residue_PR ={0.0    ,1.5 , 0.0,4.0};
// CORN::Parameter_formatX residue_PF ={{9},{4,10}};

CORN::Parameter_properties_literal  stubble_residue_PP =                         {LABEL_stubble_biomass         , "","Surface standing stubble biomass",TL_Residue,""/*"https://en.wikipedia.org/wiki/XXXX" */ ,UC_kg_m2,0};
CORN::Parameter_properties_literal  surface_residue_PP =                         {LABEL_surface_biomass         , "",TL_Surface_biomass,TL_Residue,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_kg_m2,0};

#ifdef NITROGEN
CORN::Parameter_properties_literal  N_conc_residue_PP =                          {LABEL_N_conc                 , "",TL_Nitrogen_concentration,TL_Residue,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_kg_m2,TU_kgN_kgDM};
CORN::             Parameter_range  N_conc_residue_PR ={0.005  ,0.03, 0.0,1.0};
//CORN::            Parameter_formatX  N_conc_residue_PF ={{8},{4,10}};
#endif
/*200220 obs
#if ((CS_VERSION > 0) && (CS_VERSION <= 4))
// else only use 50% decomp mode
CORN::Parameter_properties_literal decomposition_time_residue_63_PP =            {LABEL_decomposition_time     , "",TL_Decomposition_time_constant,TL_Residue,"",UT_day,"Days"};
#endif
*/
CORN::Parameter_properties_literal  halflife_PP =           {"halflife" /*LABEL_decomposition_time_50*/  , "",TL_Decomposition_time_constant,TL_Residue,""/*"https://en.wikipedia.org/wiki/XXXX" */,UT_day,"Days"};
CORN::  Parameter_range  halflife_PR ={0      ,1000, 0  ,9999};
//CORN:: Parameter_formatX  halflife_PF ={{8},{0,10}};
CORN::Parameter_properties_literal  surface_manure_PP =                          {LABEL_surface_biomass        , "",TL_Surface_biomass,TL_Surface_biomass,""/*"https://en.wikipedia.org/wiki/XXXX" */,UC_kg_m2,TU_kg_m2};
CORN:: Parameter_range  surface_manure_PR ={0.0    ,1.5 , 0.0,4.0};
//CORN::Parameter_formatX  surface_manure_PF ={{8},{4,10}};
CORN::Parameter_properties_literal  incorporated_manure_PP =                     {LABEL_incorporated_biomass   , "",TL_Incorporated_biomass,TL_Manure,""/*"https://en.wikipedia.org/wiki/XXXX" */,UC_kg_m2,TU_kg_m2};
CORN::  Parameter_range  incorporated_manure_PR ={0.0    ,0.5 , 0.0,4.0};
//CORN::Parameter_formatX  incorporated_manure_PF ={{8},{4,10}};
#ifdef NITROGEN
CORN::Parameter_properties_literal  N_conc_manure_PP =                           {LABEL_N_conc                 , "",TL_Nitrogen_concentration,TL_Manure,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_kg_m2,TU_kg_m2};
CORN:: Parameter_range  N_conc_manure_PR ={0.005  ,0.03, 0.0,1.0};
//CORN::Parameter_formatX  N_conc_manure_PF ={{8},{4,10}};

CORN:: Parameter_range  initial_water_table_N_PR ={0.0,10.0,0.0,100.0};
//CORN::Parameter_formatX  initial_water_table_N_PF ={{8},{3,10}};
CORN::Parameter_properties_literal  initial_water_table_N_NO3_PP =               {"water_table_NO3_N"          , "","Water table nitrate NO3-N concentration",HINT_UNUSED,"https://en.wikipedia.org/wiki/XXXX" ,UC_kg_m3,0};
CORN::Parameter_properties_literal  initial_water_table_N_NH4_PP =               {"water_table_NH4_N"          , "","Water table ammonium NH4-N concentration",HINT_UNUSED,"https://en.wikipedia.org/wiki/XXXX",UC_kg_m3,0};

#endif
//200220 CORN::Parameter_properties_literal  halflife__manure__PP =            {"halflife" /*LABEL_decomposition_time_50*/  , "",TL_Decomposition_time_constant,TL_Manure,""/*"https://en.wikipedia.org/wiki/XXXX" */,UT_day,"Days"};
#if defined(SALINITY)
CORN::Parameter_properties_literal  initial_water_table_salinity_PP =            {LABEL_water_table_salinity   , "",TL_Water_table_salinity,HINT_UNUSED,""/*"https://en.wikipedia.org/wiki/XXXX" */,UC_dS_m,TU_dS_m};
CORN:: Parameter_range  initial_water_table_salinity_PR ={0.0,26.0,0.0,50.0};
//CORN::Parameter_formatX  initial_water_table_salinity_PF ={{8},{3,0}};
#endif
#if (defined(SALINITY) || defined(NITROGEN))
CORN::Parameter_properties_literal  ECe_to_TDS_PP =                              {"ECe_to_TDS"                 , "","Soil profile ECe (dS/m) -> TDS (kg/m�) conversion factor","Eletrical conductivity in water (dS/m) to total dissolved solids (l/g or kg/m�). Usually about 0.64.",""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_factor,0};
CORN::Parameter_properties_literal  ECw_to_TDS_PP =                              {"ECw_to_TDS"                 , "","Water table ECw (dS/m) -> TDS (kg/m�) conversion factor","Eletrical conductivity in water (dS/m) to total dissolved solids (l/g or kg/m�). Usually about 0.64." ,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_factor,0};
CORN:: Parameter_range  ECx_to_TDS_PR ={0.0, 1.0,0.0, 2.0};
//CORN::Parameter_formatX  ECx_to_TDS_PF ={{8},{3,10}};
#endif
CORN::Parameter_properties_literal  annual_temperature_avg_PP =                  {"temperature_avg"            , "","Average annual temperature",HINT_UNUSED,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_Celcius,0};
CORN:: Parameter_range  annual_temperature_avg_PR ={0,40,0,45};
//CORN::Parameter_formatX  annual_temperature_avg_PF ={{8},{2,10}};
CORN::Parameter_properties_literal  annual_temperature_avg_half_amplitude_PP =   { "temperature_amplitude_avg" , "","Half amplitude of annual temperature range",HINT_UNUSED,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UC_Celcius,0};
CORN:: Parameter_range  annual_temperature_avg_half_amplitude_PR ={0,20,0,40};
//CORN::Parameter_formatX  annual_temperature_avg_half_amplitude_PF ={{8},{2,10}};
CORN::Parameter_properties_literal  annual_temperature_phase_PP =                {"temperature_phase"          , "","Average annual temperature phase",HINT_UNUSED,""/*"https://en.wikipedia.org/wiki/XXXX" */       ,UT_day,"Days"};
CORN:: Parameter_range  annual_temperature_phase_PR ={1,456,1,456};
//CORN::Parameter_formatX  annual_temperature_phase_PF ={{8},{0,10}};
///______________________________________________________________________________
Single_cropping_system::Single_cropping_system()
:planting_date()                                                                   //051231
,crop_filename      ("") //180510  ("<Required for single crop, orchard or vinard cropping system mode>")     //051231
,management_filename("") //180510 ("<Optional for single crop, orchard or vinard cropping system mode>")  //051231
,stop_at_harvest    (false)                                                      //181031
{}
//______________________________________________________________________________
Scenario::Scenario
(bool CPF_mode)                                                                  //160222
: CS::Simulation_control()                                                      //130621
,soil_filename          ("") //180510 ("<Required (This must be selected)>")                   //000211
#if (((CS_VERSION > 0) && (CS_VERSION <= 4)) && !defined(AGMIP))
,location_filename      ("") //180510 ("<Required (This must be selected)>")                   //990227
#endif
,weather_filename       ("") //180510 ("<Required (This must be selected)>")                   //030206
,rotation_filename      ("") //180510 ("<Usually specified (unless always fallow)>")           //010109
,fixed_management_filename    ("") //180510 ("<Optional (Currently not selected)>")            //000107
,organic_matter_initial_filename("") //180510 ("initial.organic_matter")                       //160315
,recalibration_filename ("") //180510 (L"<Optional (Currently not selected).rcl")
   // allows compatibility with previous versions where the recalibration file was the same name as the sim. 010109
,water_table_filename   ("") //180510 ("<Optional (Currently not_selected)>")
,cropping_system_mode_clad                               (no_cropping_system) //051231
, single                                                                      () //180425
/*200122
#ifdef SOILR
,grass_filename("\\cropsyst\\soilr\\grasses\\grass.crp","crp",translate(L_Grass_file) ,"*.crp")
#endif
*/
,H2O_recalibrate_date  (0) //170525 ((Year)0,(CORN::DOY)0,D_MD,D_Mmm,' ')                     //011116
#ifdef NITROGEN
,POM_recalibrate_date  (0) //170525 ((Year)0,(CORN::DOY)0,D_MD,D_Mmm,' ')                     //011116
,N_NO3_recalibrate_date(0) //170525 ((Year)0,(CORN::DOY)0,D_MD,D_Mmm,' ')                     //011116
,N_NH4_recalibrate_date(0) //170525 ((Year)0,(CORN::DOY)0,D_MD,D_Mmm,' ')                     //011116
,denitrification_parameters()                                                    //150824
#if (CROPSYST_VERSION == 4)
,V3_adjust_mineralize_32(0.8) ,v_V3_adjust_mineralize (V3_adjust_mineralize_32   ,UC_adjustment ,LABEL_adjust_mineralize      ,4,0.8,0.0,10.0,  0.0,100.0  ,TU_0_2,TL_mineralization_rate_adjustment)
,adjust_nitrify_V4_1(0.8)     ,v_adjust_nitrify_V4_1  (adjust_nitrify_V4_1       ,UC_adjustment ,LABEL_adjust_nitrify         ,4,0.8,0.0,10.0,  0.0,100.0  ,TU_0_2,TL_Nitrification_rate_adjustment)
,adjust_denitrify(0.1)        ,v_adjust_denitrify     (adjust_denitrify          ,UC_adjustment ,LABEL_adjust_denitrify       ,4,1.0,0.0,20.0,  0.0,100.0  ,TU_0_2,TL_denitrification_rate_adjustment) //081014
,max_transform_depth_32(0.5)  ,v_max_transform_depth  (max_transform_depth_32    ,UC_meter      ,LABEL_max_transform_depth    ,3,0.5,0.0, 0.5,  0.0,  5.0  ,TU_m,TL_maximum_transformation_depth)
#endif
#endif
#ifdef PHOSPHORUS
,phosphorus_in_profile(0.0), v_phosphorus_in_profile(phosphorus_in_profile,UC_kg_mass,"profile",3,0.0,0.0,300.0,0.0,9999.9,"kg/ha","Total soil profile phosphorus P mass")
#endif
#ifdef CHEM_PROFILE
,initial_chemical_profiles()
#endif
/*200122
,statsgo_muid("")      // I.e.  WA0001                                           //000116
,statsgo_seqnum(0) ,v_statsgo_seqnum   (statsgo_seqnum,LABEL_seqnum    ,0,1,99,1,9999) // Up to two digits appears to always be 1 indexed.   //000120
*/
/*170929 old LADSS  Don't delete until I prepare LADSS scenario generator
,soil_ID_str   ("")                                                              //140214
,loc_ID_str    ("")                                                              //140214
,rot_ID_str    ("")                                                              //140214
,weather_ID_str("")                                                              //140214
*/
#ifdef RESIDUES
,residue_recalibration_date(0)                                                   //160126
,reset_residue_deprecated(false)                                                 //010717
#if (((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4)))
// 070118 becoming obsolete Claudio decided to alway be initialy 0.0 (as in V.B. code)
,water_hold_const_residue_32(0.0) // 070118 (0.002)                              //070118
#endif
,stubble_residue_32(0)              ,p_stubble_residue             (stubble_residue_32,stubble_residue_PP,residue_PR,PPF_F) // residue_PF)
,surface_residue_32(0)              ,p_surface_residue             (surface_residue_32,surface_residue_PP,residue_PR,PPF_F) // residue_PF)
#ifdef NITROGEN
,N_conc_residue_32(0.0)             ,p_N_conc_residue(N_conc_residue_32 ,N_conc_residue_PP,N_conc_residue_PR,PPF_F) // N_conc_residue_PF)
#endif
/*200220
#if ((CS_VERSION > 0) && (CS_VERSION <= 4))
,decomposition_time_residue_63(60)  ,p_decomposition_time_residue__63(decomposition_time_residue__63 ,decomposition_time_residue__63_PP,halflife_PR,halflife_PF)
// else only use 50% decomp mode
#endif
*/
//200220obs,halflife__residue(41)  ,p_halflife__residue(halflife__residue ,halflife__residue_PP,halflife_PR,halflife_PF)
,previous_residue_crop_filename("<unknown or no previous crop residue>.crp")     //060224
#if (((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4)))
// 070118 becoming obsolete Claudio decided to alway be initialy 0.0 (as in V.B. code)
,water_hold_const_manure_32(0.002)                                               //010717
#endif
,surface_manure_32(0)               ,p_surface_manure(surface_manure_32 ,surface_manure_PP,surface_manure_PR,PPF_F) // surface_manure_PF)
,incorporated_manure_32(0)          ,p_incorporated_manure(incorporated_manure_32 ,incorporated_manure_PP,incorporated_manure_PR,PPF_F) // incorporated_manure_PF)
#ifdef NITROGEN
,N_conc_manure_32(0.0)              ,p_N_conc_manure(N_conc_manure_32 ,N_conc_manure_PP,N_conc_manure_PR,PPF_F) // N_conc_manure_PF)
#endif
//200220 ,halflife__manure(41)                ,p__halflife__manure(halflife__manure ,halflife__manure__PP,halflife_PR,halflife_PF)
#endif
#if defined(SALINITY)
,initial_water_table_salinity_32(0)  ,p_initial_water_table_salinity(initial_water_table_salinity_32 ,initial_water_table_salinity_PP,initial_water_table_salinity_PR,PPF_F) // initial_water_table_salinity_PF)
#endif
#if (defined(SALINITY) || defined(NITROGEN))
,ECe_to_TDS                   (0.64) ,p_ECe_to_TDS(ECe_to_TDS ,ECe_to_TDS_PP,ECx_to_TDS_PR,PPF_F) // ECx_to_TDS_PF)
,ECw_to_TDS                   (0.64) ,p_ECw_to_TDS(ECw_to_TDS ,ECw_to_TDS_PP,ECx_to_TDS_PR,PPF_F) // ECx_to_TDS_PF)
#endif
#ifdef NITROGEN
,initial_water_table_N_NO3    (0.0)  ,p_initial_water_table_N_NO3(initial_water_table_N_NO3 ,initial_water_table_N_NO3_PP,initial_water_table_N_PR,PPF_F) // initial_water_table_N_PF)
,initial_water_table_N_NH4    (0.0)  ,p_initial_water_table_N_NH4(initial_water_table_N_NH4 ,initial_water_table_N_NH4_PP,initial_water_table_N_PR,PPF_F) // initial_water_table_N_PF)
#endif
/*200122
#ifdef SOILR
,SoilR_latest_date_to_clip(365,D_MD,D_YYYY,D_lead_zero,D_M,'/')                  //980724
,SoilR_adjust_relative_growth_rate_for_clipping(1.0)  ,p_SoilR_adjust_relative_growth_rate_for_clipping(SoilR_adjust_relative_growth_rate_for_clipping ,SoilR_adjust_relative_growth_rate_for_clipping_PP,SoilR_adjust_relative_growth_rate_for_clipping_PR,SoilR_adjust_relative_growth_rate_for_clipping_PF)
,SoilR_minimum_biomass_required_for_clipping(1000)    ,p_SoilR_minimum_biomass_required_for_clipping (SoilR_minimum_biomass_required_for_clipping ,SoilR_minimum_biomass_required_for_clipping _PP,SoilR_minimum_biomass_required_for_clipping _PR,SoilR_minimum_biomass_required_for_clipping _PF)
,SoilR_biomass_forces_clipping(99999L)                ,p_SoilR_biomass_forces_clipping(SoilR_biomass_forces_clipping ,SoilR_biomass_forces_clipping_PP,SoilR_biomass_forces_clipping_PR,SoilR_biomass_forces_clipping_PF)
,SoilR_biomass_removed_clipping(0)                    ,p_SoilR_biomass_removed_clipping(SoilR_biomass_removed_clipping ,SoilR_biomass_removed_clipping_PP,SoilR_biomass_removed_clipping_PR,SoilR_biomass_removed_clipping_PF)
// SoilR Grass
,override_crop(false)
,SoilR_max_root_depth(1.5)       ,p_SoilR_max_root_depth(SoilR_max_root_depth ,SoilR_max_root_depth_PP,SoilR_max_root_depth_PR,SoilR_max_root_depth_PF)
,SoilR_max_LAI(4.0)              ,p_SoilR_max_LAI(SoilR_max_LAI ,SoilR_max_LAI_PP,SoilR_max_LAI_PR,SoilR_max_LAI_PF)
,SoilR_ET_coef(1.0)              ,p_SoilR_ET_coef(SoilR_ET_coef ,SoilR_ET_coef_PP,SoilR_ET_coef_PR,SoilR_ET_coef_PF)
,SoilR_induce_dormancy_temp(10)  ,p_SoilR_induce_dormancy_temp(SoilR_induce_dormancy_temp ,SoilR_induce_dormancy_temp_PP,SoilR_induce_dormancy_temp_PR,SoilR_induce_dormancy_temp_PF)
#endif
*/
// The following are essentially the percent of the initial organic matter carbon mass that is apportioned to the respective organic matter pools // 040610
,field_size_ha(0)                                                                //040819
,field_number(0)                                                                 //040819
,p_annual_temperature_avg(annual_temperature.avg ,annual_temperature_avg_PP,annual_temperature_avg_PR,PPF_F) // annual_temperature_avg_PF)
,p_annual_temperature_avg_half_amplitude(annual_temperature.avg_half_amplitude ,annual_temperature_avg_half_amplitude_PP,annual_temperature_avg_half_amplitude_PR,PPF_F) // annual_temperature_avg_half_amplitude_PF)
,p_annual_temperature_phase(annual_temperature.phase  ,annual_temperature_phase_PP,annual_temperature_phase_PR,PPF_N) // annual_temperature_phase_PF)
,site_aspect_N0_E90(180)  // Currently only setup by GIS but could be a user parameter.                                                                                                                                                                  //060725
,use_soil_parameter_organic_matter(false)                                        //090202
{
   // In the case of GIS and LADSS simulations the output directory
   // will be different from the template scenario filename
   annual_temperature.avg = 0;                                                   //141222
   annual_temperature.avg_half_amplitude = 0;                                    //141222
   annual_temperature.phase = 0;                                                 //141222
   char i_str[10];
   if (CPF_mode)                                                                 //160222
      CropSyst_Model_options::CPF_irrigation_disable = true;                     //150419

   for (nat8 i = 0; i <= MAX_soil_horizons; i++)  // layer 0 not used            //970614
   {  CORN::int8_to_cstr(i,i_str,10);                                            //001116
      horizon_H2O[i]                      = 0.0;                                 //051228
      #ifdef NITROGEN
      horizon_N_NO3[i]                    = 0.0;
      horizon_N_NH4[i]                    = 0.0;
      horizon_percent_organic_matter[i]   = 0.0;
      #endif
      #ifdef PHOSPHORUS
      /*NYI
      horizon_P[i] = 0.0;                                                        //110801
      v_P[i] = new Valid_float32(horizon_P[i], UC_kg_ha,i_str,3,0.0,0.0,100.0,0.0,9999.9,TU_kg_ha,"Amount of P (in the form of diphophorus pentoxide)"); //110801
      */
      #endif
      #ifdef SALINITY
      horizon_salinity_dSm[i]                 = 0.0;
      v_salinity[i]  = new Valid_float32(horizon_salinity_dSm[i] ,UC_dS_m ,i_str,3,0.0,0.0,26.0,0.0,50.0,TU_dS_m,TL_Salinity); // 990225
      #endif
      #ifdef RESIDUES
      horizon_incorporated_residue[i]             = 0.0;
      #endif
   }
   #ifdef RESIDUES
   p_stubble_residue       .set_alt_units_codes(common_mass_per_area_units);     //060526
   p_surface_residue       .set_alt_units_codes(common_mass_per_area_units);     //060526
   //170604 p_incorporated_residue  .set_alt_units_codes(common_mass_per_area_units);     //060526
   //NYI     v_N_conc_residue        .set_alt_units_codes(alt_conc_units);          //060526
   p_surface_manure        .set_alt_units_codes(common_mass_per_area_units);     //060526
   p_incorporated_manure   .set_alt_units_codes(common_mass_per_area_units);     //060526
   //NYI     v_N_conc_manure         .set_alt_units_codes(alt_conc_units);          //060526
   #endif
   for (nat8  m = 0; m <= 12; m++) mean_peak_rainfall[m] = 0;                    //091205
   soil_root_activity_coef.dry = 8.0;                                            //180416_170518 170623COS replaced 0.8 with 8
   soil_root_activity_coef.saturation = 8.0;                                     //180416_170518 170623COS replaced 0.3 with 8
}
//------------------------------------------------------------------------------
bool Scenario::expect_structure(bool for_write)
{  bool expected = CropSyst_scenario_superclass::expect_structure(for_write);    //161023_130619
   structure_defined = false;                                                    //120314
   set_current_section("scenario");                                              //090511
   set_current_section(LABEL_parameter_filenames);                               //020909
   /*170929 Don't delete until I prepare LADSS scenario generator
       // These ID's are for LADSS or other generated scenarios
      expect_string("SOIL_ID", soil_ID_str); // ID fields are for LADSS and GIS modes //020612
      expect_string("LOC_ID", loc_ID_str);                                       //020612
      expect_string("ROT_ID", rot_ID_str);                                       //020612
      expect_string("ROTATION_ID", rot_ID_str);                                  //020612
   */
      #ifndef __linux__
      // Not sure why this crashes when reading
      expect_string("LBF_ID",ID.code);                                           //160222
      #endif
      /*  Don't delete until I prepare LADSS scenario generator
      expect_string("WEATHER_ID",weather_ID_str);                                //140214
      */
      expect_file_name(LABEL_soil,soil_filename);
      #if (((CS_VERSION > 0) && (CS_VERSION <= 4)) && !defined(AGMIP))
      //191010 expect_file_name
      expect_string(LABEL_location,location_filename);                           //191010
      expect_file_name(LABEL_weather_database,weather_filename); // location is used for the UED filename, because UED stores location data. 030206_
      if (!for_write)
      #endif
      #if (CS_VERSION > 4)
      // To read V4 weather files
      if (!for_write)
         expect_file_name(LABEL_weather_database,weather_filename);                            //110101
      #endif

      expect_file_name(LABEL_weather,weather_filename); // location is used for the UED filename, because UED stores location data. 030206_
      if (!for_write) // this is to import scenario files prior to 4.1.60  (rotation has been moved to "cropping_system" section. //051231
         expect_file_name(LABEL_rotation,rotation_filename);
      expect_file_name(LABEL_management,fixed_management_filename);  // prior to V 4.1.60  //000106
      expect_file_name(LABEL_fixed_management,fixed_management_filename); // renamed this to fixed_management to avoid confusion with single management added in V 4.1.60        //060103
      expect_file_name(LABEL_recalibration,recalibration_filename);
      expect_file_name(LABEL_water_table,water_table_filename);
      expect_file_name("organic_matter",organic_matter_initial_filename);        //160315
         // Preexisting organic matter produced from previous CropSyst runs.
         // V5 will have scenario composition and parameter file discovery
         // the file or reference to it is simply placed in the
         // scenario directory.
         // REACCH composes the filename internally for the cell.

   set_current_section("cropping_system");                                       //051231
      expect_enum("cropping_mode",cropping_system_mode_clad);                    //051231
      if (!for_write || (cropping_system_mode_clad.get() == rotation_cropping_system)) //051231
         expect_file_name(LABEL_rotation,rotation_filename);
      if (!for_write || (cropping_system_mode_clad.get() == single_cropping_system))   //051231
      {
         expect_int32("sowing_date",single.planting_date.mod_date32());          //191212_170525
         expect_bool ("stop_at_harvest",single.stop_at_harvest);                 //181031
      }
      if (!for_write || (cropping_system_mode_clad.get() == single_cropping_system)
          || (cropping_system_mode_clad.get() == established_cropping_system))   //051231
      {
         expect_file_name("crop"          ,single.crop_filename);                //051231
         expect_file_name(LABEL_management,single.management_filename);          //051231
      }
   setup_structure(*this,for_write);                                             //020621
   set_current_section(LABEL_H2O);                                               //011116
      expect_enum("moisture",moisture_initial_clad);                             //170320
      expect_int32(LABEL_recalibrate_date,H2O_recalibrate_date.mod_date32());    //170525
      expect_float32_array_with_units_label
         (LABEL_H2O,TU_m_H2O_m_soil,horizon_H2O,1,MAX_soil_horizons,false);      //070404
#ifdef RESIDUES

// Note that eventually this will become a recalibration event.

   set_current_section(LABEL_initial_residue);
      if (!for_write)
      {
         // reset has been replaced with residue_recalibration_date
         // this is provided to read old versions.
         expect_bool(LABEL_reset_residue,reset_residue_deprecated,CORN::VV_bool_entry::form_true );
      }
      expect_int32(LABEL_recalibrate_date,residue_recalibration_date.mod_date32());//170525
      expect_parameter(p_stubble_residue);                                       //060526
      expect_parameter(p_surface_residue);
      expect_float32_array_with_units_code_fixed("incorporated_residue",UC_kg_m2
         ,horizon_incorporated_residue,1,MAX_soil_horizons,false);               //070404
      set_current_section("incorporated_residue");                               //060526
   #if (((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4)))
   // 070118 becoming obsolete Claudio decided to always be initialy 0.0 (as in V.B. code)
      expect_float32(LABEL_water_hold,water_hold_const_residue_32); // no longer and edited field 980206P
   #endif
      #ifdef NITROGEN
      expect_parameter(p_N_conc_residue);
      #endif
      //200220 expect_parameter(p_halflife__residue);
      expect_file_name("previous_crop",previous_residue_crop_filename);          //060224

#ifdef LADSS_MODE
      if (!for_write)                                                            //030308
      {  // The following are for LADSS                                          //030308
         expect_float32(LABEL_surface_residue_obsolete,surface_residue_32);      //030308
         expect_float32(LABEL_incorp_residue_obsolete,incorporated_residue_32_obsolete_V4_4_1);    //030308
         expect_float32(LABEL_residue_N_conc_obsolete,N_conc_residue_32);        //030308
      }
#endif
   set_current_section(LABEL_initial_manure);                                    //010717
      expect_parameter(p_surface_manure);                                        //010717
      expect_parameter(p_incorporated_manure);                                   //010717
#if (((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4)) )
// 070118 becoming obsolete Claudio decided to always be initially 0.0 (as in V.B. code)
      expect_float32(LABEL_water_hold,water_hold_const_residue_32); // no longer and edited field 010717_
#endif
      #ifdef NITROGEN
      expect_parameter(p_N_conc_manure);
      #endif
      //200220 expect_parameter(p__halflife__manure);                            //010717
#endif
#ifdef SALINITY
   set_current_section("salt");
      expect_parameter(p_initial_water_table_salinity);
      expect_parameter(p_ECe_to_TDS);                                            //080402
      expect_parameter(p_ECw_to_TDS);                                            //080402
      expect_float32_array_with_units_label(                                     //981212
#if CROPSYST_VERSION >= 5
                     "ECw" // This value is more conventually known as ECw
#else
                     "salt" //LABEL_salt
#endif
                     ,TU_dS_m,horizon_salinity_dSm,1,MAX_soil_horizons,false);
#endif
/*
#ifdef SOILR
   set_current_section(LABEL_SoilR);                                             //970614
      expect_parameter(p_SoilR_adjust_relative_growth_rate_for_clipping);
      expect_parameter(p_SoilR_minimum_biomass_required_for_clipping);
      expect_parameter(p_SoilR_biomass_forces_clipping);
      expect_parameter(p_SoilR_biomass_removed_clipping);

      expect_int32(LABEL_latest_date_to_clip,SoilR_latest_date_to_clip.ref_date32()); //980724

      expect_bool(LABEL_override_crop,override_crop,CORN::VV_bool_entry::form_true );
      expect_valid_float32(v_SoilR_max_root_depth);
      expect_valid_float32(v_SoilR_max_LAI);
      expect_valid_float32(v_SoilR_ET_coef);
      expect_valid_float32(v_SoilR_induce_dormancy_temp);                        //970521

      expect_int32(LABEL_start_dormancy_valid_date,SoilR_start_dormancy_valid_date.ref_date32()); //170525_970619
      expect_int32(LABEL_leave_dormancy_valid_date,SoilR_leave_dormancy_valid_date.ref_date32()); //170525_970619
      expect_file_name(LABEL_grass_filename,grass_filename);
#endif
*/
#ifdef CHEM_PROFILE
      horizon_percent_organic_matter[0] = 0;
#endif
#ifdef NITROGEN
   #if (CROPSYST_VERSION==4)
   set_current_section(LABEL_adjustment);
      expect_valid_float32(v_max_transform_depth);
      expect_valid_float32(v_V3_adjust_mineralize);
   #endif
   set_current_section(LABEL_NO3);
      #if (CROPSYST_VERSION==4)
      expect_float32(LABEL_adjust_denitrify,adjust_denitrify);                   //081014
      #endif
      expect_int32(LABEL_recalibrate_date,N_NO3_recalibrate_date.mod_date32());  //170525
      expect_float32_array_with_units_code_fixed
         (LABEL_NO3,UC_kg_ha,horizon_N_NO3,1,MAX_soil_horizons,false);           //070404
      expect_parameter(p_initial_water_table_N_NO3);                             //080213
    set_current_section("denitrification");                                      //140824
      expect_float32("rate_max",denitrification_parameters.rate_max);
      expect_float32("scale",denitrification_parameters.scale);
      expect_float32("alpha",denitrification_parameters.alpha);                  //150903
      //this was for testing expect_float32("respiration_std",denitrification_parameters.standardized_respiration_for_potential);
   set_current_section(LABEL_NH4);
      #if (CROPSYST_VERSION==4)
      expect_float32(LABEL_adjust_nitrify,adjust_nitrify_V4_1);
      #endif
      expect_int32(LABEL_recalibrate_date,N_NH4_recalibrate_date.mod_date32());  //170523
      expect_float32_array_with_units_code_fixed
         (LABEL_NH4,UC_kg_ha,horizon_N_NH4,1,MAX_soil_horizons,false);           //070404
      expect_parameter(p_initial_water_table_N_NH4);                             //080213
#endif
#ifdef PHOSPHORUS
   set_current_section("phosphorus");                                            //110801
      //NYI expect_date(LABEL_recalibrate_date,P_recalibrate_date,false);
      /* NYI
      expect_float32_array_with_units_code_fixed("P",UC_kg_ha,horizon_P,1,MAX_soil_horizons,false);  //110801

      */
      expect_valid_float32(v_phosphorus_in_profile);
#endif
#ifdef PESTICIDE
      expect_string_array(LABEL_chemicals,chemical_names,1,MAX_CHEM_PROFILES);   //981212
#endif
#ifdef NITROGEN
  set_current_section(LABEL_POM);

   expect_int32(LABEL_recalibrate_date,POM_recalibrate_date.mod_date32());       //170525
   expect_float32_array_with_units_code_fixed(LABEL_POM,UC_percent,horizon_percent_organic_matter,1,MAX_soil_horizons,false);
   expect_bool("get_OM_from_soil_DS",use_soil_parameter_organic_matter);
   // 120909 eventually this will be replaced with above as is done for REACCH
   set_current_section("organic_matter_initialization");                                                                                                                            //040812
      expect_float32_array_with_units_code_fixed(LABEL_microbial_biomass,UC_percent,organic_matter_initialization_parameters.microbial_biomass_percent,1,MAX_soil_horizons,false);  //070404// "Microbial biomass"           ,"portion of the total organic matter")
      expect_float32_array_with_units_code_fixed(LABEL_labile_SOM       ,UC_percent,organic_matter_initialization_parameters.labile_SOM_percent       ,1,MAX_soil_horizons,false);  //070404// "Labile stable organic matter","portion of the total organic matter")
      expect_float32_array_with_units_code_fixed(LABEL_metastable_SOM   ,UC_percent,organic_matter_initialization_parameters.metastable_SOM_percent   ,1,MAX_soil_horizons,false);  //070404// "Metastable organic matter"   ,"portion of the total organic matter")
      expect_float32_array_with_units_code_fixed(LABEL_passive_SOM      ,UC_percent,organic_matter_initialization_parameters.passive_SOM_percent      ,1,MAX_soil_horizons,false);  //070404// "Passive organic matter"      ,"portion of the total organic matter")
   set_current_section("organic_matter_history");                                                                             //060610
      expect_enum(LABEL_partitioning_mode,organic_matter_initialization_parameters.organic_matter_partitioning_mode_clad); //060610
   set_current_section("organic_matter_decomposition");                                                                       //060812

      expect_parameter(organic_matter_decomposition_parameters.p_unadj_microbial_decomposition_const);                    //060610
      expect_parameter(organic_matter_decomposition_parameters.p_labile_active_SOM_decomposition_const);                  //060610
      expect_parameter(organic_matter_decomposition_parameters.p_metastable_active_SOM_decomposition_const);              //060610
      expect_parameter(organic_matter_decomposition_parameters.p_passive_SOM_decomposition_const);                        //060610
      expect_parameter(organic_matter_decomposition_parameters.p_microbial_CN_ratio);                                     //060610
      expect_parameter(organic_matter_decomposition_parameters.p_labile_active_SOM_CN_ratio);                             //060610
      expect_parameter(organic_matter_decomposition_parameters.p_metastable_active_SOM_CN_ratio);                         //060610
      expect_parameter(organic_matter_decomposition_parameters.p_passive_SOM_CN_ratio);                                   //060610
      expect_parameter(organic_matter_decomposition_parameters.p_max_apparent_soil_C_decomposition_const);                //070710
      expect_parameter(organic_matter_decomposition_parameters.p_single_SOM_CN_ratio);                                    //110928
      expect_parameter(organic_matter_decomposition_parameters.p_C_fraction_from_labile_to_passive_SOM );                 //090515
      expect_parameter(organic_matter_decomposition_parameters.p_C_fraction_from_metastable_to_passive_SOM );             //090515

#endif
   set_current_section("topography");                                            //060725
      expect_float64("aspect",site_aspect_N0_E90);                               //060725
   /*200122 obsolete, no longer supporting direct STATSGO database access (use import utilities)
   set_current_section(LABEL_STATSGO);                                           //000118
      expect_string(LABEL_muid,statsgo_muid,7);                                  //000118
      expect_valid_int16(v_statsgo_seqnum);                                      //000120
   */
   set_current_section("field");                                                 //040819
      expect_int16("number",field_number);                                       //040819
      expect_float32("size",field_size_ha," ha");                                //040819
   set_current_section("weather");                                               //060427
      expect_parameter(p_annual_temperature_avg);                                //060502
      expect_parameter(p_annual_temperature_avg_half_amplitude);                 //060501
      expect_parameter(p_annual_temperature_phase);                              //060501

   #if (!defined(USE_CS_LAND_UNIT))
   expect_float32_array_with_units_label
       (LABEL_mean_peak_rainfall,"mm"/*TU_mm*/,mean_peak_rainfall,1,12,false);   //091206
   #endif
   set_current_section("transpiration");                                         //170518
      expect_float32
        ("dry_soil_root_activity_coef",soil_root_activity_coef.dry);             //180416_170518
      expect_float32
        ("saturated_soil_root_activity_coef",soil_root_activity_coef.saturation);//180416_170518
   structure_defined = true;                                                     //120314
   return expected;                                                              //161023
}
//_get_section_entry_changes________________________________________2002-09-09_/
bool Scenario::set_start()
{
   bool started = CropSyst_scenario_superclass::set_start();                     //161023_051028
/*
#ifdef SOILR
   for (int8 layer = 1; layer <= MAX_soil_horizons; layer++)
      H2O[layer] = H2O[1]; // SoilR soil initialization has uniform water profile
#endif
*/
#ifdef PESTICIDE_NYI
#ifdef PESTICIDE
warning this needs to be reimplemented:
   int chem_num = 0;
   for (initial_chemical_profile *chem_profile
         = (initial_chemical_profile *)initial_chemical_profiles.move_to_head()  //980730
          ;chem_profile                                                          //980730
          ;chem_profile = (initial_chemical_profile *)
            initial_chemical_profiles.move_to_next())                            //980730
   {  chem_num++;
      chem_profile->setup_structure(sim_file,chem_num
         #ifdef NITROGEN
               ,adjust_denitrify_16, adjust_nitrify_16
         #endif
         );
   }
   num_chemicals = chem_num;                                                     //970709
#endif
#endif
   return started;                                                               //161023
}
//_set_start________________________________________________________2001-01-09_/
bool Scenario::get_start()
{   // Clear the chemical names array
   bool started = CS::Simulation_control::get_start();                           //161023
#ifdef PESTICIDE
   for (uint16 i = 0; i < MAX_CHEM_PROFILES; i++)
      chemical_names[i].assign("");
#endif
   //NYI started &= CropSyst_Model_options::get_start();                         //161023
   return started;
}
//_get_start________________________________________________________2001-01-09_/
bool Scenario::get_end()
{  bool ended = true;                                                            //170202
   ID.set_code(ID.code);                                                         //160222
      // setup numeric representation of the code just for consistancy.
      // (if it isn't numeric, it will just be 0)

   // File names could be relative or shortcuts we need to set these to
   // absolute filenames.
   // Note that this may break some old version .CSN simulation runs
   // that may have used relative file names, but this is unlikely
   // because I don't think the relative filename selection was working correctly.
/*200122
#ifdef SOILR
   for (int8 layer = 1; layer <= MAX_soil_horizons; layer++)
      H2O[layer] = H2O[1]; // SoilR soil initialization has uniform water profile
#endif
*/
   /* We actually only only generate precip intensity if
       we have monthly mean 30 precip intensities and
       we are running the numerical runoff_model. 990403 */

   //  Observed leaching can only be provided in the cascade models , not the F.D..
   if (  (leaching_observation_depth_mode_clad.get() == LEACH_AT_SPECIFIED_DEPTH)
       &&(runoff_model_clad.get() == NUMERICAL_RUNOFF_MODEL))                    //070116
        leaching_observation_depth_mode_clad.set(LEACH_BOTTOM_OF_PROFILE);       //990407
   /*200220 obs
   #ifdef RESIDUES
   #if (((CS_VERSION>=1) && (CS_VERSION<=4)))
   if (((decomposition_time_residue_50==0))&&(decomposition_time_residue_63>0))  //040830
          decomposition_time_residue_50 = convert_decomposition_time_63_to_50    //040830
            (decomposition_time_residue_63);
      // We don't need to convert manure decomposition time, it is new to version 4.
   #endif
   #endif
   */
   #ifdef PESTICIDE_NYI
   #ifdef PESTICIDE
   // Now for each chemical name create and instance of the chemical name and read it
  // After the first load, we have the number chemicals so we can set up structure for the chemical initializations
   for (int chem_num = 1; chem_num <= num_chemicals; chem_num++)
   {   CORN_string chem_name(chemical_names[chem_num]);
      if (!((chem_name == LABEL_NO3) ||   // these chemicals already expected
            (chem_name == LABEL_NH4) ||
            (chem_name == LABEL_salt)))
      {
         initial_chemical_profile *new_initial_chemical_profile
            = new initial_chemical_profile(chem_name);                           //980730
         new_initial_chemical_profile->setup_structure(sim_file,chem_num         //980730
            #ifdef NITROGEN
            ,adjust_denitrify_16, adjust_nitrify_16
            #endif
            );
         initial_chemical_profiles.append(
         (CORN_list_item *)  // <- Warning this cast shouldn't be needed something might not be setup right
         new_initial_chemical_profile);                                          //980730
      }
   }
   #endif
   #endif
   /*200122
   #ifdef SOILR
   ending_date.set(years_in_rotation+1,1);  // SoilR needs to override the simulation period
   #endif
   */
   //051231 Cropping system mode was added in V 4.1.60 since the default is no_cropping_system
   //       old version scenario files will load as no_cropping_system so now we check the if the specified rotation filename exists for rotation mode.
   if (cropping_system_mode_clad.get() == no_cropping_system)                    //051231
      if (CORN::OS::file_system_engine.exists(rotation_filename))
         cropping_system_mode_clad.set(rotation_cropping_system);                //051231
   #if (((CROPSYST_VERSION >0) && (CROPSYST_VERSION < 5)) )
   if (organic_matter_and_residue_model_clad.get() == V4_4_ORGANIC_MATTER_AND_RESIDUE)
       organic_matter_and_residue_model_clad.set(multiple_SOM_multiple_residues);
   if (organic_matter_and_residue_model_clad.get() == V4_9_ORGANIC_MATTER_AND_RESIDUE)
       organic_matter_and_residue_model_clad.set(single_SOM_multiple_residues);
   #endif
   CropSyst_scenario_superclass::get_end();                                      //130619_020911
   #ifdef RESIDUES
   if (reset_residue_deprecated)                                                 //160126
      residue_recalibration_date.set_DOY(start_date.get_DOY());
   #endif
   ended &=                                                                      //180202
      #if ((CROPSYST_VERSION >= 1) && (CROPSYST_VERSION < 5))
      CropSyst_Model_options::get_end() &&
      #endif
      CS::Simulation_control::get_end();                                         //170809_161023
   return ended;
}
//_get_end__________________________________________________________2001-01-09_/
#ifdef ABANDONED
// Don't delete this, it works, but is slow if there are many directories
// I should have a scenario control option to enable this.

// This is intended for cases where scenarios or scenario fragments
// or scenario directories intentionally are moved, copied or generated in alternate locations;
// such that relative filenames would be expected to be out of alignment.
// It would then be necessary to search for the files where the filenames.
// Are unqualified.   A better solution would be to use
// macro names  (I.e.   %PROJECT%/Database/Soil/xxxx.CS_soil)
// although the filenames may be provided in fragments
// or the files may be in subdatabases.

bool Scenario::qualify_unqualified_parameter_filenames
(CS::Databases &databases
,bool verbose)
{  bool assigned = false;
   CORN::Unidirectional_list database_subdirs;
   FOR_EACH_IN(section,VV_Section,sections,each_section)
   {
      FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
      {
         const CORN::VV_directory_entry_entry *entry_as_DEN =
            dynamic_cast<const CORN::VV_directory_entry_entry *>(entry);
         if (entry_as_DEN)
         {
            CORN::OS::Directory_entry_name &param_filename = entry_as_DEN->ref_value();
            std::string name;
            CORN::wstring_to_string(param_filename.get_name(),name);
            CORN::Text_list path_items_unused;
            if (!database_subdirs.count())
               databases.render_subdirectories(database_subdirs,CORN::Inclusion("*",true),verbose);
            CORN::OS::File_name *most_appropriate_filename
               = databases.render_find_most_appropriate_in_any_context_string
               (param_filename.get_extension(),name,path_items_unused,database_subdirs,verbose);
            if (most_appropriate_filename)
            {  assigned = true;
               param_filename.set_DEN(*most_appropriate_filename);
            }
         }
      } FOR_EACH_END(each_entry)
   } FOR_EACH_END(each_section)
   return assigned;
}
#endif
//_qualify_unqualified_parameter_filenames__________________________2016-10-31_/

#ifdef LADSS_MODE
Common_operation *Scenario::get_operation
(const char        *operation_type_str
,const std::string &operation_ID
,modifiable_
CORN::OS::File_name &operation_filename_not_used_by_CropSyst                     //041215
   // This is not currently used by CropSyst
   // (it stores an optional CANMS operation file when running under CANMS
,int32          resource_set_ID     // currently used only by LADSS
,bool          &relinquish_op_to_event)
{  Common_operation  *loaded_op = 0;
   CORN::Data_source       *op_data_source = 0;
   CropSyst_Op_clad cs_op_clad(operation_type_str);
   nat32 op_ID = CORN::str_to_nat32(operation_ID.c_str(),10);                    //041215
   if (LADSS_mode && LADSS_CropSyst_model)
   {
      relinquish_op_to_event = true;
      switch (cs_op_clad.get())
      {
/* Not currently handled here?
                     case CS_OP_ROT_SOWING     :
                        loaded_op = new xxxx_operation;
                        op_data_source = &xxxxxxxx_data_source;
                     break;
*/
         case CS_OP_MGMT_TILLAGE :
            loaded_op = new CropSyst::Tillage_operation;
//                        tillage does not have parameter tables,
//                        we simply create the respective operation
//                        and load the SCS code from the RESOURCE_SET
         break;
         case CS_OP_MGMT_RESIDUE:
            loaded_op = new CropSyst::Residue_operation;
//                        tillage does not have parameter tables,
//                        we simply create the respective operation
//                        and load the SCS code from the RESOURCE_SET
         break;
         case CS_OP_MGMT_HARVEST_OR_CLIPPING:                                    //031014
            loaded_op = new CropSyst::Harvest_or_clipping_operation;
            op_data_source = new Database_data_source(LADSS_database_schema table("HARVEST"),"OP_ID",std::ios_base::in  database_session);   //031129
         break;
         case CS_OP_MGMT_IRRIGATION :
            loaded_op = new CropSyst::Irrigation_operation;
            op_data_source = new Database_data_source(LADSS_database_schema table("IRRIGATION")            ,"OP_ID",std::ios_base::in  database_session);
         break;
         case CS_OP_MGMT_INORGANIC_NITROGEN           :
            loaded_op = new CropSyst::Inorganic_nitrogen_operation;
            op_data_source = new Database_data_source(LADSS_database_schema table("INORGANIC_NITROGEN")    ,"OP_ID",std::ios_base::in  database_session);
         break;
         case CS_OP_MGMT_BIOMATTER_APPLICATION           :                       //080829
            loaded_op = new CropSyst::Biomatter_application_operation;           //080829
            op_data_source = new Database_data_source(LADSS_database_schema table("BIOMATTER_APPLICATION")    ,"OP_ID",std::ios_base::in  database_session); //080829
         break;
         case CS_OP_MGMT_ORGANIC_NITROGEN             :
            loaded_op = new CropSyst::Organic_nitrogen_operation;
            op_data_source = new Database_data_source(LADSS_database_schema table("ORGANIC_NITROGEN")      ,"OP_ID",std::ios_base::in  database_session);
         break;
/*NYI                case CS_OP_MGMT_CHEMICAL                     :
                        loaded_op = new Chemical_operation;
                        op_data_source = &xxxx_data_source;
                     break;
*/
         case CS_OP_MGMT_AUTO_IRRIGATION              :
            loaded_op = new CropSyst::Automatic_irrigation_mode;                 //060804
            op_data_source = new Database_data_source(LADSS_database_schema table("AUTO_IRRIGATION")       ,"OP_ID",std::ios_base::in  database_session);
         break;
         case CS_OP_MGMT_AUTO_N                     : // <- This is a place holder for LADSS
            loaded_op = new CropSyst::N_application_soil_observation_mode ;
            op_data_source = new Database_data_source(LADSS_database_schema table("AUTO_N")              ,"OP_ID",std::ios_base::in  database_session);
         break;
         case CS_OP_MGMT_AUTO_N_BALANCE_BASED      :
            loaded_op = new CropSyst::N_application_soil_observation_mode ;
            op_data_source = new Database_data_source(LADSS_database_schema table("AUTO_N"
               )              ,"OP_ID" ,std::ios_base::in database_session);
         break;
         #if (((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5)) )
         case CS_OP_MGMT_AUTO_N_LOCAL_CALIBRATION  :
            loaded_op = new CropSyst::N_application_soil_observation_mode ;
            op_data_source = new Database_data_source(LADSS_database_schema table("AUTO_NO3")              ,"OP_ID",std::ios_base::in  database_session);
         break;
         #endif
         case CS_OP_MGMT_AUTO_N_APPL :
            loaded_op = new CropSyst::N_application_soil_observation_mode ::Split_application;
            op_data_source = new Database_data_source(LADSS_database_schema table("AUTO_N_APPL")         ,"OP_ID",std::ios_base::in  database_session);

         break;
/*NYI
                     case CS_OP_MGMT_SOIL_N_SAMPLING :
                        loaded_op = new xxxx_operation;
                        op_data_source = &xxxxxxxx_data_source;
                     break;
*/
         case CS_OP_MGMT_AUTO_CLIP_BIOMASS :
            loaded_op = new CropSyst::Auto_clip_biomass_mode;
            op_data_source = new Database_data_source(LADSS_database_schema table("HARVEST")     ,"OP_ID",std::ios_base::in  database_session);
         break;
/*NYI
                     case CS_OP_RECALIBRATION :
                        loaded_op = new xxxx_operation;
                        op_data_source = &xxxxxxxx_data_source;
                     break;
*/
      }  // end switch
      if (loaded_op)
      {  Database_data_source resource_set_data_source(LADSS_database_schema table("RESOURCE_SET"),"RES_SET_ID" ,std::ios_base::in database_session);  //031129
         resource_set_data_source.where_long("RES_SET_ID",resource_set_ID);      //031129
         resource_set_data_source.get(*loaded_op);                               //031129
         if (op_data_source)
         {  op_data_source->where_long("OP_ID",op_ID);
            op_data_source->get(*loaded_op);
            delete op_data_source;                                               //040706
         }
         // else is it probably tillage or residue which only get SCS code from resource set
      }
   }
   // else operations are loaded with the event list.
   return loaded_op;
}
#endif
//_get_operation____________________________________________________2003-08-08_/
bool Scenario::has_mean_peak_rainfall()                                    const
{  bool result = true;     // This used to be in the location file, which is now deprecated
   for (nat8  i = 1; i <= 12; i++)
      if (CORN::is_zero<float32>(mean_peak_rainfall[i]/*200127 ,0.0,0.000001*/))
         result = false;
   return result;
}
//_has_mean_peak_rainfall___________________________________________2009-12-05_/
#if (CROPSYST_VERSION==4)
   // GIS scenario generation is now provided by scenario generator              //170202
nat8 Scenario::assign_IDs_to_parameter_filenames()                 modification_
{  // This is used with GIS and other scenario template based simulations
   // where the template scenario specifies filenames
   // that I identify parameter file directories
   // and the filename will be replaced with the
   nat8 IDs_assigned = 0;
/*
   if (soil_ID_str   .length() && (soil_ID_str    != "0"))  { soil_filename     .replace_name_str(soil_ID_str);     IDs_assigned++;}
   if (rot_ID_str    .length() && (rot_ID_str     != "0"))  { rotation_filename .replace_name_str(rot_ID_str);      IDs_assigned++;}
   if (weather_ID_str.length() && (weather_ID_str != "0"))  { weather_filename  .replace_name_str(weather_ID_str);  IDs_assigned++;}
*/
   //loc is deprecated if (loc_ID_str.length())
   return IDs_assigned;
}
#endif
//_assign_IDs_to_parameter_filenames________________________________2014-02-14_/
#if (CROPSYST_VERSION==4)
   // GIS scenario generation is now provided by scenario generator              //170202
bool Scenario::apply_simulation_override_record
(const CropSyst::Simulation_override_record &GIS_record)

// rename GIS_record to override_record

{
      CORN::OS::File_name_concrete new_soil_filename(soil_filename);
      new_soil_filename.replace_name_str(GIS_record.parameter_overrides.soil_code); //050921
      if (CORN::OS::file_system_engine.exists(new_soil_filename))
          soil_filename.set_DEN(new_soil_filename);
      #if ((CS_VERSION==4) &&!defined(AGMIP))
      CORN::OS::File_name_concrete new_location_filename(location_filename);
      new_location_filename.replace_name_str(GIS_record.parameter_overrides.weather_code);  // this may be obsolete at least provided for backward compatibility 050921
      if (CORN::OS::file_system_engine.exists(new_location_filename))
          location_filename.
          assign(new_location_filename.c_str());                                //191010
          //191010 set_DEN(new_location_filename);
      #endif
      CORN::OS::File_name_concrete new_weather_database(weather_filename);
      new_weather_database.replace_name_str(GIS_record.parameter_overrides.weather_code);
      if (CORN::OS::file_system_engine.exists(new_weather_database))
      {
         #if ((CS_VERSION==4) &&!defined(AGMIP))
         location_filename.assign(new_weather_database.c_str());  // weather database trumps location
         //191010 location_filename.set_DEN(new_weather_database);  // weather database trumps location
         #endif
         weather_filename.set_DEN(new_weather_database);                         //050619
      }
      CORN::OS::File_name_concrete new_rotation_filename(rotation_filename);     //010518
      new_rotation_filename.replace_name_str(GIS_record.parameter_overrides.rotation_code); //050921
      if (CORN::OS::file_system_engine.exists(new_rotation_filename))            //010518
          rotation_filename.set_DEN(new_rotation_filename);                      //010518
      CORN::OS::File_name_concrete new_recalibration_filename(recalibration_filename,".RCL");         //060411
      new_recalibration_filename.replace_name_str(GIS_record.parameter_overrides.recalibration_code); //060411
      if (CORN::OS::file_system_engine.exists(new_recalibration_filename))       //060411
          recalibration_filename.set_DEN(new_recalibration_filename);            //060411
   return true;
}
//_apply_simulation_override_record_________________________________2014-02-14_/
#endif
}//_namespace CropSyst______________________________________________2013-06-21_/

