#include <memory>

#  include "land_unit_sim.h"
#  include "cs_accum.h"
   // CS_accum must be first
#  include "csvc.h"

#  include "cs_simulation.h"

#  include "crop/crop_param.h"
#  include "crop/phenology_I.h"
#  include "crop/crop_interfaced.h"
#  include "management/management_param_class.h"

#ifdef VIC_CROPSYST_VERSION
#else
#  include "crop/crop_N_balancer.h"
#  if (CS_VERSION >= 5)
#     include "common/biometeorology/ET_reference_FAO.h"
#  else
#     include "cs_ET.h"
#  endif
#endif
#  include "rot_param.h"
#  include "cs_scenario_directory.h"
#if (CROPSYST_VERSION==4)
#  ifdef WSHEDMOD
#     include "wshedio.h"
#  endif
#else
#  include "management/management_param_class.h"
#endif
#ifdef RESIDUES
#  include "organic_matter/OM_residues_profile_common.h"
#  include  "common/residue/residues_interface.h"
#endif

#  include  "common/residue/residue_balancer.h"

#  include "corn/data_source/vv_file.h"
#  include "common/recorder/recorder_datasrc.h"
#  include "common/simulation/log.h"
#  include "cs_mgmt.h"
#  include "soil.h"
#  include "soil/infiltration_I.h"
#  include "soil/dynamic_water_entering.h"
#  include "soil/chemicals_profile.h"
#ifdef SOIL_TEMPERATURE
#  include "soil/temperature_profile.h"
#endif
#  include "soil/soil_evaporator.h"
#ifdef RUNOFF
#  include "soil/runoff_SCS.h"
#endif
#ifdef EROSION
#  include "soil/erosion_RUSLE.h"
#endif
#include "common/weather/stormgen.h"
#  include "common/weather/database/weather_database.h"
#  include "weather/storms.h"
#  include "cs_event.h"
#  include "common/biomatter/decomposition_const.h"
#  include "soil/temperature_hourly.h"
#ifdef SOIL_FREEZING
#  include "soil/freezing_jumakis.h"
#endif
#ifdef SALINITY
#  include "soil/salinity_profile.h"
#endif
#ifdef SOIL_INFILTRATION
#  ifdef FINITE_DIFF
#     include "soil/hydrology_finite_diff.h"
#     include "soil/infiltration_finite_diff.h"
#  endif
#  ifdef CASCADE
#     include "soil/hydrology_cascade.h"
#     include "soil/infiltration_cascade_daily.h"
#     include "soil/infiltration_cascade_hourly.h"
#  endif
#endif
#  ifdef LADSS_MODE
#     include "LADSS.h"
      extern bool LADSS_mode;
#  endif
#  ifdef BUILDER_API
#     include <stdlib.h>
#  endif
#include "static_phrases.h"
#ifdef STATSGO_DATABASE
#  include "soil/soil_param_with_STATSGO.h"
#endif
#ifndef VIC_CROPSYST_VERSION
//140917 Not sure how/where SOIL_TEMPERATURE is getting defined
#ifdef SOIL_TEMPERATURE
#  include "soil/temperature_hourly.h"
#endif
#endif
#ifdef SOIL_INFILTRATION
#include "watertab.h"
#endif
#ifdef NITROGEN
#if (CS_VERSION == 4)
#include "organic_matter/simple/cs_chem_pot_mineral_N_V4.h"
#endif
#include "soil/nitrogen_profile.h"
#include "soil/chemical_mass_profile.h"
#endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "soil/abiotic_environment.h"
#include "organic_matter/single_pool/OM_pools_single.h"
#include "organic_matter/multiple_pool/OM_pools_multiple.h"
#include "common/weather/atmospheric_CO2_change_linear.h"
#include "common/weather/atmospheric_CO2_change_recalibration.h"

using namespace std;
#if ((CROPSYST_VERSION==4) )
#  include "organic_matter/simple/OM_residues_profile_simple_V4.h"
#endif

#include "common/weather/hour/weather_hours.h"
#include "CS_suite/observation/CS_observation.h"

bool harvest_event_failed_today=false; // This is here only for Mike Rivington's spreadsheet

#ifdef DETAILED_BALANCES
extern C_Balance *C_balance;
#endif

#include "crop/crop_cropsyst.h"

namespace CropSyst {
//______________________________________________________________________________
Land_unit_simulation_VX::Land_unit_simulation_VX
(const CS::Identification     &LBF_ID_                                           //151026
,nat32                         cycle_shift_                                      //151026
,CropSyst::Scenario           &scenario_control_and_model_options_               //130827
,const CORN::date32           &today_
,const Geocoordinate          &geocoordinate_                                    //151116
,const CS::Land_unit_meteorological &meteorology_                                //151022
#if (CS_VERSION==4)
,Common_simulation_log       &event_log_                                         //030308
#endif
,const CropSyst::Scenario_directory *scenario_directory_)
: CS::Simulation_unit(scenario_control_and_model_options_,today_)
, scenario_control_and_model_options   (scenario_control_and_model_options_)     //130826
, scenario_event_list                  ()
#ifdef DETAILED_BALANCES
,C_balance_file                        (0)                                       //111005
#endif
,scenario_directory                    (scenario_directory_)                     //060627
,ID                                    (LBF_ID_)                                 //151026
,cycle_shift                           (cycle_shift_)                            //151026
   // always needed because used in output accum                                 //141105
,meteorology                           (meteorology_)                            //151022
,soil_parameters(0)                                                              //160916
,soil_parameters_known                 (false)                                   //160916
,initialize_residues_needed            (true)                                    //161116
,soil                                  (0)
, sowing_precipitation_window_meter    ()
,daily_liquid_precipitation            (0)                                       //050314
,infiltration_submodel                 (0)
,runoff                                (0)                                       //130308
,using_numerical_runoff_model          (false)                                   //200316
//#ifndef VIC_CROPSYST_VERSION
#if (CROPSYST_PROPER==4)
,ET_ref                                (0)                                       //030830
,slope_saturated_vapor_pressure_plus_psychrometric_resistance(0)                 //180805
#endif
,soil_hydrology_ref                    (0)                                       //070825
,soil_freezing_optional                (0)                                       //070824
#ifdef VIC_CROPSYST_VERSION
   // VIC would handle any ponding (but I dont think it has ponding)
#else
,pond(today_,evaporation_potential_remaining)                                    //160408_991102
#endif
,organic_matter_residues_profile_kg_m2 (0)                                       //050726
,soil_abiotic_environment_profile      (0)                                       //060510
,auto_irrig_start_date                 ((Year)0,CORN::DOY(0))                    //000406
,auto_irrig_end_date                   ((Year)0,CORN::DOY(0))                    //000406
//200404,crop_at_start_season                  (0)                               //070227
,crop_active_or_intercrop              (0)
//200404,crop_at_end_seasonX                    (0)                              //190701
,crop_sowing_postponed                 (0)                                       //070514
,predominant_crop                      (0)                                       //160904
,crop_fract_canopy_cover               (0)
,has_any_snow                          (false)
,evaporation_potential_remaining       ()                                        //170609
#if (CS_VERSION==4)
,event_log_ref(event_log_)                                                       //021007
   // always needed because used in output accum                                 //141105
#endif

#ifdef MANAGEMENT
,curr_management                       (0)
,curr_N_application_soil_observation_mode(0)                                     //060804
,curr_automatic_clip_mode              (0)                                       //020913
,curr_automatic_irrigation_mode        (0)                                       //020913
,fixed_irrigation_scheduled_today      (false)
#endif

,harvest_today_flag                    (false)
,mulch_cover_fraction                  (0)                                       //100610
,automatic_irr_amount                  (0)                                       //050314
,total_N_application_kg_m2             (0)                                       //161129
,auto_N_previous_NO3_kg_m2             (0)                                       //170530
,auto_N_previous_NH4_kg_m2             (0)                                       //170530
,earliest_irrigation_start_hour        (0)                                       //081212
,irrig_appl_target                     (0)                                       //050314
,total_irrigation_today                (0)                                       //050428
,management_irrigation_duration_hours  (24)                                      //070107
               // Default to 24 (this was the default for previous version)
               // this will then be set for the more recent irrigation event.
,ANNL_accumulators_active              (0)                                       //050115
,GP_accumulators_active                (0)                                       //050115
#if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
// (make sure not to include in REACCH
,CONTINUOUS_accumulators_active        (0)                                       //050115
#endif
#if (CROPSYST_PROPER==4)
,FALLOW_accumulators_active            (0)                                       //050115
#endif
#ifndef VIC_CROPSYST_VERSION
,crop_nitrogen_mass_balance_ref        (0)                                       //070220
#endif
,unsatisfied_harvest_BM_today_kg_m2    (0.0)
,grazing_BM_demand_today_kg_m2         (0)                                       //050107

,crop_mass_fate_to_be_distributed      (0)                                       //070627
#if (CROPSYST_PROPER==5)
,output_act_crop_evapotransp_m         (0)                                       //151218
,output_crop_evapotransp_max_m         (0)                                       //151218
#endif
,emanator_known                        (0)
,scenario_alteration                   (simdate_raw)                             //181015
{
  // If we don't delete the database, it will add records
   grazing_hold_manure_application.org_N_amount_kg_ha = 0.0;                     //041220
   grazing_hold_manure_application.NH3_amount_kg_ha   = 0.0;                     //041220
}
//_Land_unit_simulation_VX::constructor________________________________________/
Land_unit_simulation::Land_unit_simulation
(const CS::Identification &LBF_ID_                                               //151026
,nat32                         cycle_shift_                                      //151026
,CropSyst::Scenario           &scenario_control_and_model_options_               //130827
,const CORN::date32           &today_
,const Geocoordinate          &geocoordinate_                                    //151116
,const CS::Land_unit_meteorological &meteorology_                                //151022
#if (CS_VERSION==4)
,Common_simulation_log        &event_log_                                        //030308
#endif
,const CropSyst::Scenario_directory *scenario_directory_)                        //050619
:Land_unit_simulation_VX
   (
    LBF_ID_
   ,cycle_shift_                                                                 //151126_151026
   ,scenario_control_and_model_options_                                          //130827
   ,today_
   ,geocoordinate_
   ,meteorology_
   #if (CS_VERSION==4)
   ,event_log_
   #endif
   ,scenario_directory_)
#if (CS_VERSION==4)
#ifdef WSHEDMOD
,cell_in_db(0)                                                                   //990409
,cell_out_db(0)                                                                  //990409
#endif
#  ifdef LADSS_MODE
,LADSS_daily_database_table_enabled(false)
,LADSS_daily_database_table  (0)   // These are set below
,LADSS_harvest_database_table(0)
,LADSS_annual_database_table (0)
#endif
{
#  ifdef LADSS_MODE
   if (LADSS_mode)
   {  LADSS_daily_database_table    = new LADSS_output_data_source(table("daily_output")  ,"D_OUT_ID" );   // Not currently used in LADSS
      LADSS_harvest_database_table  = new LADSS_output_data_source(table("season_output") ,"S_OUT_ID" );
      LADSS_annual_database_table   = new LADSS_output_data_source(table("annual_output") ,"A_OUT_ID" );
      AUTOMATICALLY_SEQUENCE_PRIMARY_KEY(LADSS_daily_database_table);
      AUTOMATICALLY_SEQUENCE_PRIMARY_KEY(LADSS_harvest_database_table);
      AUTOMATICALLY_SEQUENCE_PRIMARY_KEY(LADSS_annual_database_table);
      sint32 alloc_ID = LBF_ID; // FOR LADSS, LBF_ID is the alloc_ID
      if (LADSS_daily_database_table_enabled)
         LADSS_daily_database_table->delete_allocation(alloc_ID);
      LADSS_harvest_database_table->delete_allocation(alloc_ID);
      LADSS_annual_database_table->delete_allocation(alloc_ID);
   }
#endif
}
#else
{}
#endif
//_Land_unit_simulation:constructor_________________________________2011-10-28_/
Land_unit_simulation_VX::~Land_unit_simulation_VX()
{
   #ifdef MANAGEMENT
   curr_automatic_irrigation_mode = 0; // don't delete, now owned                //111130
   #endif
/*not sure why delting runoff crashed
   #ifdef RUNOFF
   delete runoff;                                                                //130308
   #endif
*/
   #if (CROPSYST_PROPER==4)
   delete ET_ref;                                                                //010214
   #endif
   delete organic_matter_residues_profile_kg_m2;                                 //050726
   delete soil_abiotic_environment_profile;                                      //060510
   delete ANNL_accumulators_active;                                              //020401
   delete GP_accumulators_active;                                                //020401
   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
   delete CONTINUOUS_accumulators_active;                                        //020401
   #if (CROPSYST_PROPER==4)
   delete FALLOW_accumulators_active;                                            //020401
   #endif
   #endif

//given to simulation   if (crop_nitrogen_mass_balance_ref)    delete crop_nitrogen_mass_balance_ref;    crop_nitrogen_mass_balance_ref = 0;  //111116
//   if (crop_mass_fate_to_be_distributed)  delete crop_mass_fate_to_be_distributed;  crop_mass_fate_to_be_distributed = 0;//111116
#ifdef DETAILED_BALANCES
   delete C_balance_file;                                                        //111005
#endif
   if (emanator_known) emanator_known->set_status(CS::Emanator::deleted_status); //160614
}
//Land_unit_simulation_VX:destructor___________________________________________/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
Land_unit_simulation::~Land_unit_simulation()
{
   #ifdef WSHEDMOD
   if (cell_in_db)                        delete cell_in_db;                     //990429
   if (cell_out_db)                       delete cell_out_db;                    //990429
   #endif
}
#endif
//_Land_unit_simulation:destructor__________________________________2011-11-08_/
bool Land_unit_simulation_VX::load_soil()
{  // VIC and MicroBasin maintains their own soil, nothing to load
   return true;
}
//_load_soil________________________________________________________2016-05-23_/
#if (defined(CROPSYST_PROPER) || defined(OFOOT_VERSION))
#ifdef STATSGO_DATABASE
extern STATSGO::Database *STATSGO_database;
#endif
bool Land_unit_simulation::load_soil()
{
   #if (defined(MICROBASIN_VERSION))
      // MicroBasin version provides its own soil
      return true;
   #else
   bool loaded_OK = false;
   #ifdef LADSS_MODE
   if (LADSS_mode)
   {  LADSS_soil_data_source soil_datasrc(soil_ID);
      //LADSS always has a soil
      soil_parameters = new Smart_soil_parameters;                               //061005
      loaded_OK = soil_datasrc.get(*soil_parameters_relinquished_to_soil);       //010110
      soil = new CropSyst::Soil(soil_parameters_relinquished_to_soil);           //060803
      //In last, initial soil profile is stored in the SOIL_LAYER table
      //(although I disagree with this).
      //We need to copy the value to the simulation arrays 030303
      for (nat8 h = 1; h <= soil->parameters->number_horizons; h++)              //030303
      {                                                                          //030303
         horizon_H2O[h]   = horizon_LADSS_H2O[h];                                //030303
         horizon_N_NO3[h] = horizon_LADSS_N_NO3[h];                              //030303
         horizon_N_NH4[h] = horizon_LADSS_N_NH4[h];                              //030303
         horizon_percent_organic_matter[h]= horizon_LADSS_percent_organic_matter[h];    //030303
         // CropSyst now has an option to load organic matter from soil parameter file.
         // This is not implemented in LADSS.                                    //090202
      }
  } else
  #endif
  {   loaded_OK = provide_soil_parameters() != 0;                                //170208_160916
      if (soil_parameters)                                                       //160916
             soil = new CropSyst::Soil_components                                //141207
               (soil_parameters
               ,!soil_parameters_known                                           //160916
               ,simdate_raw                                                      //170524
               ,scenario_control_and_model_options.annual_temperature            //150424
               ,meteorology);                                                    //151022
   }
   return loaded_OK;
   #endif
}
#endif
//_load_soil________________________________________________________2011-10-30_/
bool Land_unit_simulation_VX::initialize_soil()
{
   bool soil_initialized = true;
   soil_initialized &= load_soil();
   if (! soil) return false; // The soil was not specified (which is valid, for no limitations simulation   //060803

 #ifdef EROSION
   if (scenario_control_and_model_options.run_erosion                            //060712
       && meteorology.ref_storms()                                               //160506
       && soil)                                                                  //130827
      // must be done before soil->Initialize()
      soil->take_erosion_submodel(new RUSLE::Soil_erosion
         (dynamic_cast<Soil_base &>(*soil)                                       //111108_060712
         ,*meteorology.ref_storms()                                              //160506
         ,meteorology.ref_precipitation_actual ()                                //160510
         ,simdate_raw));                                                         //170524_160403
 #endif
 #ifdef VIC_CROPSYST_VERSION
   // VIC handles infiltration
 #else
   /* now member for faster lookup
   Infiltration_model infiltration_model                                         //060206
      =scenario_control_and_model_options.infiltration_model_clad.get();         //130827
   */
   infiltration_submodel = 0;   // will be relinquished to soil                  //060206
   Soil::Hydrology_abstract *soil_hydrology = 0; // will be relinquished to soil //060512
   const Soil::Layers_interface &layers = *(soil->ref_layers());                 //060509
   const Soil::Hydraulic_properties &hydraulic_properties
      = (const Soil::Hydraulic_properties &)(*soil->ref_hydraulic_properties()); //160412_060512
   unmodifiable_ Soil::Structure    &soil_structure = *soil->ref_structure();    //160412_140425_071001
   #ifdef RUNOFF
   switch (scenario_control_and_model_options.runoff_model_clad.get())
   {
      case CURVE_NUMBER_RUNOFF_MODEL:runoff=new Soil_runoff_SCS(*soil);break;    //200316
      case NUMERICAL_RUNOFF_MODEL   :runoff=new Soil_runoff_common(*soil);break;
      //default no runoff
   }
   #endif
   #ifdef SOIL_INFILTRATION
   switch (infiltration_model)                                                   //060505
   {
      #if (defined(FINITE_DIFF) && !defined(MICROBASIN_VERSION))
      case FINITE_DIFFERENCE_INFILTRATION :                                      //060505
      {  Soil::Hydrology_finite_difference *soil_hydrology_FD
         = new Soil::Hydrology_finite_difference(layers,hydraulic_properties,soil_structure);     //060512
         soil_hydrology  = soil_hydrology_FD;                                    //060512
         Soil::Evaporation_without_vapor_path *soil_evaporator
            = new Soil::Evaporation_without_vapor_path                            //160606
            (evaporation_potential_remaining,
             *(soil->ref_layers())
            ,(soil->ref_hydraulic_properties())
            ,*soil_hydrology_FD);
         soil->submit_evaporator(soil_evaporator);                               //160606
         infiltration_submodel = new Soil::Infiltration_finite_difference
            (*soil
            ,*(soil->mod_layers())                                               //160412_140509
            ,*(soil->ref_texture())                                              //160412_130509
            ,*(soil->ref_hydraulic_properties())                                 //160412_140423
            ,*soil_evaporator
            ,runoff
            ,pond
            ,*soil_hydrology_FD
            ,meteorology.ref_relative_humidity_min()                             //151216
            ,using_numerical_runoff_model);                                      //131001_130308_060512
         if (scenario_control_and_model_options.run_nitrogen)                    //130827_111207
             scenario_control_and_model_options.hourly_not_in_UI |= HOURLY_nitrogen_transformations; //130827_111207
      } break;                                                                   //060505
      #endif
      #ifdef CASCADE
      case CASCADE_DAILY_INFILTRATION :                                          //060505
      {  Soil::Hydrology_cascade *soil_hydrology_cascade = new                   //060512
            Soil::Hydrology_cascade(layers,hydraulic_properties,soil_structure);
         soil_hydrology = soil_hydrology_cascade;                                //060512
         Soil::Evaporator *soil_evaporator = new Soil::Evaporator                //160606
            (evaporation_potential_remaining
            ,*(soil->ref_layers())
            ,*(soil->ref_hydraulic_properties())
            ,*soil_hydrology_cascade);
         soil->submit_evaporator(soil_evaporator);
         infiltration_submodel = new Soil::Infiltration_cascade_daily
            (*(soil->ref_layers())                                               //160412_140424
            ,*(soil->ref_hydraulic_properties())                                 //160412_140423
            ,*soil_hydrology_cascade
            ,runoff,pond);
      } break;                                                                   //060505
      case CASCADE_HOURLY_INFILTRATION :                                         //060505
      {  Soil::Hydrology_cascade *soil_hydrology_cascade = new Soil::Hydrology_cascade
            (layers,hydraulic_properties,soil_structure);                        //060512
         soil_hydrology = soil_hydrology_cascade;                                //060512
         Soil::Evaporator *soil_evaporator = new Soil::Evaporator                //160606
            (evaporation_potential_remaining
            ,*(soil->ref_layers())
            ,*(soil->ref_hydraulic_properties())
            ,*soil_hydrology_cascade);
         soil->submit_evaporator(soil_evaporator);
         infiltration_submodel = new Soil::Infiltration_cascade_hourly
            (*(soil->ref_layers())                                               //1604121_140424
            ,*(soil->ref_texture())                                              //160412_140424
            ,*(soil->ref_hydraulic_properties())                                 //160412_140423
            ,*(soil_hydrology_cascade)
            ,runoff
            ,using_numerical_runoff_model                                        //200316
            ,pond);                                                              //131001_130308_060512
         if (scenario_control_and_model_options.run_nitrogen)                    //130827_111207
            scenario_control_and_model_options.hourly_not_in_UI
               |= HOURLY_nitrogen_transformations;                               //130827_111207
      } break;                                                                   //060505
      #endif
      case NO_INFILTRATION:                                                      //060512
      {  Soil::Hydrology_cascade *soil_hydrology_cascade = new Soil::Hydrology_cascade
            (layers,hydraulic_properties,soil_structure);                        //060512
         soil_hydrology = soil_hydrology_cascade;                                //060512

         // Warning need to activate evaporation here

         std::cerr << "Evaporation has not yet been enabled for no infiltration option contact rnelson@wsu.edu" << std::endl;

         assert(false);                                                          //160606
         infiltration_submodel = new Soil::Infiltration_none();
      } break;                                                                   //060512
      case NO_LIMITATION :                                                       //070724
      {  // Default to no soil hydrology, this should disable infiltration
      } break;                                                                   //060512
      default :  // default to daily cascade                                     //060512
      {  // Default to no soil hydrology, this should disable infiltration
      } break;                                                                   //060512
   }                                                                             //060505
   soil_hydrology_ref = (Soil::Hydrology_interface *) soil_hydrology;            //070825
   soil_initialized =                                                            //150417_131217
      soil->set_infiltration_hydrology
         (infiltration_model                                                     //190707
         ,infiltration_submodel                                                  //060206
         ,soil_hydrology);                                                       //060512
   #endif
      // infiltration
   #ifdef EROSION
   if (soil->erosion)                                                            //091130
      soil->erosion->know_weather
         (scenario_control_and_model_options.has_mean_peak_rainfall()            //130827
            ? scenario_control_and_model_options.mean_peak_rainfall              //130827
            : 0
         // YET IMPLEMENTED: (would only apply to V4
         // If using old location file, get the monthly means from
         // the location file;
      );
   #endif
      // erosion
 #endif
      // VIC_CROPSYST_VERSION (infiltration)

   #ifdef VIC_CROPSYST_VERSION
      // VIC handles soil initialization
   #else
   soil_initialized &=                                                           //150417RLN
      soil->reinitialize
      (scenario_control_and_model_options.moisture_initial_clad.get()            //170320
      ,scenario_control_and_model_options.horizon_H2O
      ,true,scenario_control_and_model_options.subdivide_horizons)               //130827_060206
      #ifdef SOIL_INFILTRATION
      && infiltration_submodel->initialize()
      #endif
      ;
   #endif

   // Must not be initialized before soil //071012
   //Here we set the desired soil submodel options
   if (!soil_initialized) return false;                                          //111115
   #ifdef SOIL_INFILTRATION
   if (scenario_control_and_model_options.leaching_observation_depth_mode_clad.get() == LEACH_AT_SPECIFIED_DEPTH) //130827
      infiltration_submodel->set_override_leaching_depth((float64)scenario_control_and_model_options.leaching_observation_depth); //130827_070118
   #endif

   #ifdef WATERTABLE
   bool water_table_file_exists =                                                //020620
      CORN::OS::file_system_engine.exists(scenario_control_and_model_options.water_table_filename); //130827_130807
   if (water_table_file_exists)                                                  //020620
      soil->infiltration_submodel->set_water_table                               //020620
      (scenario_control_and_model_options.water_table_filename.c_str()           //130827_020620
      ,simdate_raw                                                               //170525_151128
      ,scenario_control_and_model_options.start_date                             //110104
      ,scenario_control_and_model_options.stop_date  );                          //110104
   #endif
   #ifdef SALINITY
   soil_initialized &= initialize_salinity                                       //080213
      (soil                                                                      //080213
      #ifdef WATERTABLE
      ,water_table_file_exists                                                   //080213
      ,scenario_control_and_model_options.water_table_filename.c_str()           //130827_080213
      #else
      ,false                                                                     //090818
      ,""                                                                        //090818
      #endif
      #if (defined(SALINITY) || defined(NITROGEN))
      ,scenario_control_and_model_options.ECw_to_TDS                             //130827_080402 LML 140908 added macro
      #endif
      ,infiltration_model);                                                      //080213
   #endif
   #ifdef NITROGEN
   if (scenario_control_and_model_options.run_nitrogen)                          //130827_020620
   {
      #ifdef WATERTABLE
      //Added 080213
      Water_table_curve *water_table_NO3_N_curve =   // salt concentration curve
         (water_table_file_exists)
         ? new Water_table_curve
            (scenario_control_and_model_options.water_table_filename.c_str()     //130827
            ,"NO3-N"
            ,simdate_raw                                                         //170525
            ,scenario_control_and_model_options.start_date
            ,scenario_control_and_model_options.stop_date
            ,(float64)scenario_control_and_model_options.param_initial_water_table_N_NO3() //130827
            ,N_to_NO3_conv) // element N to molecular NO3
         :0;
      if (water_table_NO3_N_curve && !water_table_NO3_N_curve->is_valid())
      {  // It is possible the water table file didn't have any 'salt' entries
         // and the initial_water_table_salinity is unspecified (0.0)
         // In this case we simply delete the water table.
         delete water_table_NO3_N_curve;
         water_table_NO3_N_curve = 0;
      }
      Water_table_curve *water_table_NH4_N_curve =   // salt concentration curve
         (water_table_file_exists)
         ? new Water_table_curve
            (scenario_control_and_model_options.water_table_filename.c_str()     //130827
            ,"NH4-N"
            ,simdate_raw                                                         //170525
            ,scenario_control_and_model_options.start_date
            ,scenario_control_and_model_options.stop_date
            ,(float64)scenario_control_and_model_options.param_initial_water_table_N_NH4() //130827
            ,N_to_NH4_conv) // element N to molecular NH4
         :0;
      if (water_table_NH4_N_curve && !water_table_NH4_N_curve->is_valid())
      {  // It is possible the water table file didn't have any 'salt' entries
         // and the initial_water_table_salinity is unspecified (0.0)
         // In this case we simply delete the water table.
         delete water_table_NH4_N_curve;
         water_table_NH4_N_curve = 0;
      }
   #endif
      soil->enable_nitrogen                                                      //020620
         (scenario_control_and_model_options.hourly_not_in_UI & HOURLY_nitrogen_transformations //130827_070502
         ,infiltration_model                                                     //190707
         ,provide_soil_abiotic_environment_profile()
         #if (CROPSYST_VERSION==4)
         ,scenario_control_and_model_options.param_max_transform_depth()
         ,scenario_control_and_model_options.param_adjust_denitrify()            //090127
         ,scenario_control_and_model_options.param_adjust_nitrify_V4_1()
         #endif
         ,scenario_control_and_model_options.denitrification_parameters          //150824
         ,scenario_control_and_model_options.horizon_N_NO3
         ,scenario_control_and_model_options.horizon_N_NH4
         #ifdef WATERTABLE
         ,water_table_NO3_N_curve                                                //080213
         ,water_table_NH4_N_curve                                                //080213
         #else
         ,0,0                                                                    //090818
         #endif
         );
   }                                                                             //080213
  #ifdef PHOSPHORUS
   soil->know_phosphorus_in_profile(phosphorus_in_profile);                      //110801
   //NYI soil->enable_phosphorus(horizon_P);
  #endif
 #endif
 #ifdef CHEM_PROFILE
   if (soil->chemicals)
   {
      soil->chemicals->know_NH4_Langmuir(soil->NH4_Langmuir);                    //190404
      soil_initialized &= soil->chemicals->initialize(); // complete initialization of soil chemical
      #ifdef SOIL_INFILTRATION
      infiltration_submodel->know_chemicals(soil->chemicals);
      #endif
   }
 #endif
 #ifndef VIC_CROPSYST_VERSION
  #ifdef SOIL_FREEZING
   if (scenario_control_and_model_options.run_freezing)                          //130827_020620
   {  soil_freezing_optional = (Soil::Freezing_profile *)                        //181206_040714
            new Soil_freezing_profile_jumakis
            (*(soil->ref_layers())                                               //160412_051201
            ,*(soil->ref_structure())                                            //160412_070725
            ,*(soil->ref_hydraulic_properties())                                 //160412_051202
            ,*(soil->mod_hydrology())                                            //160412_051201
            ,*(soil->provide_temperature_profile())
            ,meteorology.ref_air_temperature_avg());                             //151022

      soil_hydrology->know_freezing_profile(soil_freezing_optional);             //060505
   }                                                                             //070824
  #endif
  #ifdef PESTICIDE
   // NYI
      soil->chemicals->enable_pesticies
      (initial_chemical_profiles
      #ifdef WATERTABLE
      ,water_table_filename.c_str()
      ,start_date
      ,stop_date
      #endif
      );
  #endif
 #endif
   return soil_initialized;                                                      //041111
}
//_initialize_soil__________________________________________________2003-05-27_/
#ifdef SALINITY
bool Land_unit_simulation_VX::initialize_salinity
(CropSyst::Soil_components *soil
,bool               water_table_file_exists
,const char        *water_table_filename
#ifdef NITROGEN
,float64            ECw_to_TDS_water_table                                       //140908LML added macro
#endif
,Infiltration_model infiltration_model_check_used)
{  bool inited = true;                                                           //140105
   if (scenario_control_and_model_options.run_salinity)                          //020620
   {  soil_layer_array64(sublayer_salt_concentration_dSm);                       //051120
      soil->distribute_property(scenario_control_and_model_options
         .horizon_salinity_dSm,sublayer_salt_concentration_dSm);                 //130827_051120
      #ifdef WATERTABLE
      Water_table_curve *salt_water_table_salt_curve =                           //080213
         (water_table_file_exists)                                               //071214
         ? new Water_table_curve                                                 //071214
            (water_table_filename                                                //071214
            ,"salt"                                                              //071214
            ,simdate_raw                                                         //170525
            ,scenario_control_and_model_options.start_date                       //150426_130827_110104
            ,scenario_control_and_model_options.stop_date                        //150426_130827_110104
            ,(float64)scenario_control_and_model_options.param_initial_water_table_salinity() //130827_071214
            ,ECw_to_TDS_water_table ) // Usually about 0.64                      //080402
         :0;                                                                     //071214
      if (salt_water_table_salt_curve&&!salt_water_table_salt_curve->is_valid()) //080213
      {  // It is possible the water table file didn't have any 'salt' entries   //080213
         // and the initial_water_table_salinity is unspecified (0.0)            //080213
         // In this case we simply delete the water table.                       //080213
         delete salt_water_table_salt_curve;                                     //080213
         salt_water_table_salt_curve = 0;                                        //080213
      }                                                                          //080213
      #endif
      soil->set_salinity                                                         //051120
      (  new Soil::Soil_salt
         (simdate_raw                                                            //170525
         ,*(soil->ref_layers())                                                  //160412_150925
         ,*(soil->ref_hydrology())                                               //160412_150925
         ,*(soil->ref_hydraulic_properties())                                    //160412_150925
         ,*(soil->ref_structure())                                               //160412_150925
         ,infiltration_model                                                     //190707
         , sublayer_salt_concentration_dSm
         #ifdef WATERTABLE
         , salt_water_table_salt_curve
         #endif
         , scenario_control_and_model_options.ECe_to_TDS ), true );              //130827_080403
            // truue lets the soil delete
   }
   return inited;                                                                //080213
}
#endif
//_initialize_salinity______________________________________________2008-02-13_/
Soil::Abiotic_environment_profile *Land_unit_simulation_VX::provide_soil_abiotic_environment_profile()
{
   if (!soil_abiotic_environment_profile)
        soil_abiotic_environment_profile
         = new Soil::Abiotic_environment_profile                                 //080904
            (soil->ref_layers()                                                  //181130
            ,soil->ref_temperature_profile_hourly()                              //181130
            ,soil->ref_hydrology());                                             //181130
   return soil_abiotic_environment_profile;
}
//_provide_soil_abiotic_environment_profile_________________________2006-08-10_/
bool Land_unit_simulation_VX::instanciate_organic_matter_and_residues()
{  delete organic_matter_residues_profile_kg_m2;
 #ifdef DETAILED_BALANCES
   delete C_balance;
   C_balance = new C_Balance;
 #endif
   // This was the setup for the original organic matter model.
   soil_sublayer_array_64(sublayer_percent_organic_matter);                      //051120
   if (scenario_control_and_model_options.use_soil_parameter_organic_matter)     //130827_090202
      #if (CROPSYST_VERSION==4)
      copy_horizon_array32                                                       //111120_090202
         (scenario_control_and_model_options.horizon_percent_organic_matter
         ,soil->parameters_ownable->horizon_organic_matter);
      #else
         copy_horizon_array32_sized                                              //160916
            (scenario_control_and_model_options.horizon_percent_organic_matter
            ,soil->parameters_ownable->properties.organic_matter.get_array()
            ,soil->parameters_ownable->properties.organic_matter.get_count());
      #endif
   soil->distribute_property(scenario_control_and_model_options
      .horizon_percent_organic_matter,sublayer_percent_organic_matter);          //130827_051120
   #ifdef RESIDUES
   switch (scenario_control_and_model_options
            .organic_matter_and_residue_model_clad.get())                        //130827_050726
   {                                                                             //050726
      case NO_ORGANIC_MATTER_AND_RESIDUE  :
         organic_matter_residues_profile_kg_m2 = 0; break;                       //060628
      case multiple_SOM_multiple_residues:
      {  Organic_matter_pools_multiple *organic_matter_pools_profile_relinquished
            = new Organic_matter_pools_multiple                                  //111004
            (scenario_control_and_model_options.organic_matter_decomposition_parameters
            ,*provide_soil_abiotic_environment_profile()
            ,*soil->ref_layers()                                                 //160412
            ,*soil->ref_structure()                                              //160412
            , soil->mod_nitrogen()                                               //160412
            ,*soil->ref_texture());                                              //160412

         organic_matter_residues_profile_kg_m2
            = new Organic_matter_residues_profile_common
            (soil
            ,*provide_soil_abiotic_environment_profile()
            ,evaporation_potential_remaining                                     //160412
            ,meteorology.ref_air_temperature_avg()                               //151130_150427
            ,meteorology.ref_wind_speed()                                        //151130_150427
            ,scenario_control_and_model_options.horizon_percent_organic_matter   //130827
            ,organic_matter_pools_profile_relinquished
            ,scenario_control_and_model_options.organic_matter_initialization_parameters //130827
            ,scenario_control_and_model_options.organic_matter_decomposition_parameters //130827_111004
            ,simdate_raw);                                                       //170525
      } break;
      case single_SOM_multiple_residues:
      {  Organic_matter_pools_single *organic_matter_pools_profile_relinquished
            = new Organic_matter_pools_single                                    //111004
            (scenario_control_and_model_options.organic_matter_decomposition_parameters //130827_120912
            ,*provide_soil_abiotic_environment_profile()
            ,*soil->ref_layers()                                                 //160412
            ,*soil->ref_structure()                                              //160412
            , soil->mod_nitrogen()                                               //160412
            ,*soil->ref_texture());                                              //160412_070707
         organic_matter_residues_profile_kg_m2 = new Organic_matter_residues_profile_common
            (soil
            ,*provide_soil_abiotic_environment_profile()
            ,evaporation_potential_remaining                                     //160412
            ,meteorology.ref_air_temperature_avg()                               //151130
            ,meteorology.ref_wind_speed()                                        //151130
            ,scenario_control_and_model_options.horizon_percent_organic_matter   //130827
            ,organic_matter_pools_profile_relinquished
            ,scenario_control_and_model_options.organic_matter_initialization_parameters  //130827
            ,scenario_control_and_model_options.organic_matter_decomposition_parameters //130827_111004
            ,simdate_raw);                                                       //140507
      } break;                                                                   //060327
   }                                                                             //050726
   if (organic_matter_residues_profile_kg_m2)                                    //150429
   {  if (scenario_directory)
      {  {  std::ifstream equilibrated_OM                                        //150429
               (scenario_control_and_model_options.organic_matter_initial_filename.c_str() //1603115
               ,std::ios::binary);
            if (equilibrated_OM.good())                                          //161118
            {  // If the organic file openned then we will be loading initial
               // residues from the file instead of creating from scratch.
               // initial_residues_loaded_from_file = true;
               if (organic_matter_residues_profile_kg_m2->read(equilibrated_OM)) //150429
                  initialize_residues_needed = false; //because loaded from file //161116
            }
         }                                                                       //150429
      }
      if (soil->chemicals && organic_matter_residues_profile_kg_m2)              //060310
      {  Soil::NO3_Profile *NO3_prof =  soil->chemicals->NO3;                    //060504
         if (NO3_prof)                                                           //060731
            NO3_prof->know_CO2_C_loss_subsurface
               (organic_matter_residues_profile_kg_m2->get_CO2_C_loss_profile_residue_and_OM());
      }
   }
   #endif
   return true;
}
//_instanciate_organic_matter_and_residues__________________________2011-09-15_/
bool Land_unit_simulation_VX::initialize()                       initialization_ //151227
{  bool inited = true;                                                           //131217
   infiltration_model                                                            //190707
      =scenario_control_and_model_options.infiltration_model_clad.get();         //190707
   using_numerical_runoff_model                                                  //200316
      = scenario_control_and_model_options.runoff_model_clad.get() == NUMERICAL_RUNOFF_MODEL;
  #if (!(defined(USE_CS_SIMULATION_ROTATOR) || (CS_VERSION >= 5)))
   #ifdef DETAILED_BALANCES
   if (scenario_control_and_model_options.run_nitrogen)                          //130827_070220
      // We also only need to create the crop_nitrogen_mass_balance if outputs are desired
      crop_nitrogen_mass_balance_ref = new CropSyst::Crop_nitrogen_mass_balancer;//070220
   take_balancer(crop_nitrogen_mass_balance_ref);                                //070227
   take_balancer(residue_mass_balance[OM_subsurface_position]        = new Residue_balancer_subsurface);    //090408
   take_balancer(residue_mass_balance[OM_flat_surface_position]      = new Residue_balancer_surface_flat);  //090408
   take_balancer(residue_mass_balance[OM_stubble_surface_position]   = new Residue_balancer_surface_stubble_or_attached(OM_stubble_surface_position));     //090408
   take_balancer(residue_mass_balance[OM_attached_to_plant_position] = new Residue_balancer_surface_stubble_or_attached(OM_attached_to_plant_position));   //090408

   #endif

   if (scenario_directory)                                                       //111105
       scenario_directory->Output().create();                                    //130827
  #endif
   #ifdef BUILDER_API
   randomize();
   #else
   #if defined(__MSDOS__) || defined(_Windows)
   randomize;                                                                    //960828
   #endif
   #endif
   nitrogen_daily_output.clear();                                                //131001
   daily_liquid_precipitation = 0.0;
   inited &= initialize_soil();                                                  //131217_111115_030527
if (!inited) std::clog << "CropSyst::Land_unit_simulation::initialize_soil failed" << std::endl;

   if (inited)                                                                   //131217
   {
      // We cannot setup residue until soil is initialized                       //981120
      inited &= instanciate_organic_matter_and_residues();                       //160811RLN
      setup_cropping_system();                                                   //060103
      #ifdef NITROGEN
      wettings.clear();                                                          //131106
      #endif
      /*200122
      #ifdef SOILR
      crop = new Crop_cropsyst_specific
         (soil,residue_pools
         ,0 // Grass does not have grows                                         //010110
         ,false // grass is not fallow                                           //010110
         ,false,false);
      crop->track_nitrogen_mass_balance(crop_nitrogen_mass_balance);             //070220
      {  VV_File grass_file(grass_filename);                                     //010110
         grass_file.get(crop)                                                    //010110
      }                                                                          //010110
      if (organic_matter_residues_profile_kg_m2)                                 //060911
         crop->know_residues(organic_matter_residues_profile_kg_m2->ref_residues()) //060531
      // Use simulation overrides
      if (override_crop)                                                         //970614
      { crop->max_root_depth       = SoilR_max_root_depth;                       //970608
        crop->max_LAI = SoilR_max_LAI;                                           //970608
        crop->ET_coef = SoilR_ET_coef;                                           //970608
        crop->induce_dormancy_temp = SoilR_induce_dormancy_temp;           // 'C //970608
        crop->start_dormancy_valid_date = SoilR_start_dormancy_valid_date;       //970608
        crop->leave_dormancy_valid_date = SoilR_leave_dormancy_valid_date;       //970608
      }                                                                          //970608
      #endif
      */
      // default management no planting date yet so no auto irrig.
      //We must load_reports_options BEFORE opening output files
      //We must initialize soil before openning output files
      //soil must be available and initialized by this point
      //The report options must be available and initialized by this point
   }
   inited &= setup_initial_soil_profile_recalibrations();                        //160303
   inited &= CS::Simulation_unit::initialize();                                  //151001

   #if (CROPSYST_VERSION >= 5)
   /*180109  NYI
   find layout file in path
   if layout file exists
   {
      instanciate layout
      load layout parameter
   }
   */
   #endif

// if (!inited) std::clog << "CS::Simulation_unit::initialize failed" << std::endl;

   return inited;                                                                //010205_070227
}
//_initialize_______________________________________________________2001-02-02_/
#if (CROPSYST_VERSION==4)
bool Land_unit_simulation::initialize()                          initialization_
{  bool inited =  Land_unit_simulation_VX::initialize();
   #ifdef WSHEDMOD
   // Setup for watershed model                                                  //020308
   if (watershed_simulation())  //(LBF_ID > 0)                                   //030530
   {  cell_in_db  = new Enumerated_database
         (scenario_directory->output_watershed_in_ued.c_str() );                 //990525
      cell_in_db->initialize();                                                  //990525
      cell_out_db = new Enumerated_database
         (scenario_directory->output_watershed_out_ued.c_str() );                //990525
      cell_out_db->initialize();                                                 //990525
   }
   #endif
   // ET_ref may need to be setup in start (after weather and sun have been setup)
   scenario_control_and_model_options.Priestley_Taylor_parameters
      .know_VPD_fullday(&meteorology.ref_vapor_pressure_deficit_fullday_avg());  //170808
   if (scenario_control_and_model_options
         .evapotranspiration_model_clad.get() == ET_recorded)                    //180805
        ET_ref = new ET_Reference;                                               //180805
   else ET_ref = new CropSyst_ET                                                 //180805
      (meteorology.ref_weather_provider()                                        //151023
      ,meteorology.get_geocoordinate()                                           //151118
      ,2.0  // Screening height
      ,meteorology.ref_sun().ref_day_length_hours_today()                        //150211_150627
      ,scenario_control_and_model_options.evapotranspiration_model_clad.get()    //130827_051231
      ,scenario_control_and_model_options.Priestley_Taylor_parameters);          //170808
   return inited;
}
//_initialize_______________________________________________________2011-11-08_/
#else
bool Land_unit_simulation::initialize()                          initialization_
{  bool inited =  Land_unit_simulation_VX::initialize();
   if (!inited) std::clog << "Land_unit_simulation::initialize() Land_unit_simulation_VX::initialize() failed " << std::endl;
   #if (CROPSYST_PROPER==4)
   ET_ref = new ET_Reference_FAO
         (meteorology.ref_weather_provider()
         ,meteorology.get_geocoordinate()                                        //151207
         ,meteorology.ref_sun().ref_day_length_hours_today()                     //151207
         ,ET_FAO_56_Penman_Monteith_CropSyst);
   #endif
   return inited;
}
#endif
//_initialize_______________________________________________________2015-10-22_/
bool Land_unit_simulation_VX::setup_cropping_system()
{  bool setup_good = false;
   date32 single_or_established_sowing_date = 0;
   const CORN::Date_cowl_32 &start_date = scenario_control_and_model_options.start_date; //200308
   const CORN::Date_cowl_32 &stop_date = scenario_control_and_model_options.stop_date; //200308
   const Single_cropping_system &single = scenario_control_and_model_options.single;   //200308
   switch (scenario_control_and_model_options.cropping_system_mode_clad.get())   //130827
   {  case no_cropping_system : break; // Nothing to do in this mode
      case single_cropping_system:
      {  if (single.planting_date.get_year() < 1000) // Appears to be a relative year
         {  CORN::Date_time_clad_64 now; // For unique event ID
            for (CORN::Year sim_year = start_date.get_year()
                ; sim_year <= scenario_control_and_model_options.stop_date.get_year()
                ; sim_year++)
            {
               char generated_sowing_event_ID[100];
               strcpy(generated_sowing_event_ID,CORN::float64_to_cstr(now.get_datetime64(),15));
               Sowing_operation *repeated_single_sowing_op = new Sowing_operation
                  (single.crop_filename.c_str(),single.management_filename.c_str());
               CORN::Date_clad_32 sow_date(sim_year,single.planting_date.get_DOY()); //180201
               schedule_planting_event(sow_date.get_date32(),*repeated_single_sowing_op,generated_sowing_event_ID );
               Sowing_event *repeated_single_sowing_event= new Sowing_event;
               repeated_single_sowing_event->begin_sync.set_as_actual(sow_date); //180201_170825
               repeated_single_sowing_event->set_operation(repeated_single_sowing_op,true);
               Synchronization act_date_sync(ACTUAL_DATE_MODE);
               repeated_single_sowing_event->synchronize
                  (act_date_sync,CORN::Date_clad_32 ());                         //170525
               scenario_event_list.append(repeated_single_sowing_event);
               setup_good = true;
            }
         } else                                                                  //160118
            single_or_established_sowing_date
            =single.planting_date.get_date32();                                  //130827
      } break;
      case established_cropping_system:
      {  //  simply create a sowing event being the start of simulation and add it to event queue
        single_or_established_sowing_date = start_date.get_date32();
      } break;
      case rotation_cropping_system : default :
         setup_good = load_rotation_planting_events();                           //020309
      break;
   } // switch
   if (single_or_established_sowing_date)
   {  CORN::Date_time_clad_64 now; // for unique ID
      char single_sowing_event_ID[100];
      strcpy(single_sowing_event_ID,CORN::float64_to_cstr(now.get_datetime64(),15));
      Sowing_operation *single_sowing_op = new Sowing_operation
         (single.crop_filename.c_str()
         ,single.management_filename.c_str());                                   //130827
      schedule_planting_event(single_or_established_sowing_date,*single_sowing_op,single_sowing_event_ID );
      Sowing_event * single_sowing_event= new Sowing_event;

//std::clog << "SOWID:" <<single_sowing_event_ID << std::endl;

      single_sowing_event->ID = single_sowing_event_ID;                          //170703
      single_sowing_event->begin_sync.set_as_actual
         (CORN::Date_clad_32(single_or_established_sowing_date));
      single_sowing_event->set_operation(single_sowing_op,true);
      Synchronization act_date_sync(ACTUAL_DATE_MODE);
      single_sowing_event->date.set_date32(single_or_established_sowing_date);   //170703

      single_sowing_event->synchronize(act_date_sync,CORN::Date_clad_32());
         // this should cause actual date syncs to be set in the events
      scenario_event_list.append(single_sowing_event);                           //131020_130827
      setup_good = true;
   }
   return setup_good;
}
//_setup_cropping_system____________________________________________2006-01-03_/
#ifdef RESIDUES
bool Land_unit_simulation_VX::initialize_residues()
{
   // now residues can be reset (reinitialized)
   // delete all residues
   if (organic_matter_residues_profile_kg_m2)                                    //060628
   {  organic_matter_residues_profile_kg_m2->clear_residues();                   //170219_160126
      soil_sublayer_array_64(subsurface_residue);                                //970421

      float64 initial_subsurface_residue = soil->distribute_amount               //131218_070607
         (scenario_control_and_model_options.horizon_incorporated_residue // horizon amounts  //130827_070606
         ,subsurface_residue);         // to sublayer amounts                    //070606

      Residues_interface *local_residue_pools_kg_m2                              //060115
         = organic_matter_residues_profile_kg_m2->mod_residues();
      CropSyst::Crop_parameters previous_crop_residue_parameter;                 //060225
         // actually only need the residue decomposition parameters
      if (CORN::OS::file_system_engine.exists
          (scenario_control_and_model_options.previous_residue_crop_filename))   //130827_060115
      {  VV_File prev_residue_crop(scenario_control_and_model_options.previous_residue_crop_filename.c_str());  //130827_060115
         prev_residue_crop.get(previous_crop_residue_parameter);                 //060115
      // NYI  eventually specify the name of the crop that produced              //060115
      // the initial residue and load these parameters from that.                //060115
      }                                                                          //060115
      /*200220obs
      #if (CS_VERSION==4)
      else // just use the default crop parameters
      {  // eventually the else will be obsolete,                                //060225
         // this is to read CSN files prior to V4.3
         previous_crop_residue_parameter.residue_decomposition                   //060219
               .halflife
            = scenario_control_and_model_options.halflife__residue;              //110613
      }                                                                          //060219
      #endif
      */

//131218  NYI  only add residue pools if init mass > 0.0000001
      float64 initial_stubble_residue = scenario_control_and_model_options.param_stubble_residue();//130827
      if (initial_stubble_residue > 0.0000001)
         local_residue_pools_kg_m2->add_surface_plant_pool                       //060116
         (OM_stubble_surface_position                                            //040517
            // true currently assuming initial residue is laid down flat
         ,OM_ORIGINAL_CYCLING //In V4.3 the cycling is split by add_straw_pool   //060219
         ,initial_stubble_residue                                                //130827
         #ifdef NITROGEN
         ,scenario_control_and_model_options.param_N_conc_residue()              //130827
         #endif
         ,previous_crop_residue_parameter.residue_decomposition);                //060225_110613
      float64 initial_flat_residue    = (float64)scenario_control_and_model_options.surface_residue_32;//130827
      if (initial_flat_residue > 0.0000001)
         local_residue_pools_kg_m2->add_surface_plant_pool                       //060116
         (OM_flat_surface_position                                               //040517
            // false currently assuming initial residue is laid down flat
         ,OM_ORIGINAL_CYCLING // In V4.3 the cycling is split by add_straw_pool  //060219
         ,initial_flat_residue                                                   //130827
         #ifdef NITROGEN
         ,scenario_control_and_model_options.param_N_conc_residue()              //130827
         #endif
         ,previous_crop_residue_parameter.residue_decomposition );               //130827_060225_110613
      if (initial_subsurface_residue > 0.0000001)                                //131217
         local_residue_pools_kg_m2->add_subsurface_plant_pool                    //060116
         (subsurface_residue                                                     //970421
          #ifdef NITROGEN
         ,scenario_control_and_model_options.param_N_conc_residue()              //130827
         #endif
         ,previous_crop_residue_parameter.residue_decomposition );               //060225_110613
   }

#ifdef NYI

200220 actually just take biomatter file.

I need to add to the parameter editor the
manure type, source etc..

   #define DECOMPOSITION_SCALING 0.7
   //040830  Not sure why scaling by  0.7
   /*200220 obs
   {float32 real_decomposition_time_63 = std::max<float32>((float32)decomposition_time_manure_63 * DECOMPOSITION_SCALING,1.0);         //990301
    decomposition_time_manure_63 =  real_decomposition_time_63; }                //981212
    */
   {float32 real_halflife = std::max<float32>((float32)halflife__manure * DECOMPOSITION_SCALING,1.0);         //990301      // 040826 don't know what this is
    halflife__manure =  real_halflife; } // check used, something may be off here  //981212
   float64 manure_decomposition_constant =   convert to decomposition constant
   if (local_residue_pools_kg_m2)                                                //060628
   {  soil_sublayer_array_64(subsurface_manure);                                 //010723

         // now the user can enter the whole soil horizon profile

      subsurface_manure[2] = param_incorporated_manure();                        //010723
      local_residue_pools_kg_m2->add_manure_pool_ligninous                       //010723
        (param_surface_manure()
        ,subsurface_manure
        ,param_water_hold_const_manure()
         #ifdef NITROGEN
         ,param_N_conc_manure(),param_N_conc_manure()
         #endif
        ,manure_decomposition_constant                                           //060202
        ,5.0);                                                                   //010723
   }
#endif
   return true;
}
#endif
//_restart_OM_______________________________________________________2011-09-19_/
bool Land_unit_simulation_VX::distribute_evaporation_potential_daily_to_intervals
(float64 evaporation_pot_daily)                modification_
{
   const CS::Weather_hours *weather_hourly = meteorology.provide_hourly();
   if (weather_hourly && scenario_control_and_model_options.hourly_not_in_UI & HOURLY_evaporation)
   {
      for (CORN::Hour hour = 0; hour < 24; hour++)
      {
         float64 fract_solar_irradiance_hour = weather_hourly->get_insolation_fraction(hour);
         float64 evap_potential_hour = evaporation_pot_daily * fract_solar_irradiance_hour;
         evaporation_potential_remaining.set(hour, evap_potential_hour);
      }
   } else
   {  evaporation_potential_remaining.resize(1);
      evaporation_potential_remaining.set(0, evaporation_pot_daily);
   }
   return true;
}
//_distribute_evaporation_potential_daily_to_intervals______________2016-07-19_/
bool Land_unit_simulation_VX::start()                              modification_
{
   bool started = true;
   #ifdef RESIDUES
   if (initialize_residues_needed &&                                             //161116
       (scenario_control_and_model_options.residue_recalibration_date.get_date32() == 0)) //160217
      // The initial residues recalibration date is 0 we initialize the residues
      // Otherwise initialization does not occur until the recalibration date.
      initialize_residues();                                                     //110919
   #endif
   #ifdef EROSION
   // Setup Erosion.  Must follow weather start because
   // we must have monthly values to run erosion

//050916  NYI   WARNING  in the case we are selecting UED for weather
//                we should check if there is 1/2 precip, if not then
//                also disable run_erosion
   if (!scenario_control_and_model_options.has_mean_peak_rainfall())             //130827
      scenario_control_and_model_options.run_erosion = false;
   #endif
   #ifdef MANAGEMENT
   load_events_for_simulation(scenario_control_and_model_options.fixed_management_filename);  //130827
      //  Special simulation events
   if (scenario_directory)
   {  // OFoot version isn't instanciating scenario directory (Need to check if this is actually a problem)
   CORN::OS::File_name *option_CPF_irrg_mgt_filename = scenario_directory->
      provide_CPF_water_irrigation_management_filename_optional();               //140329
   if (option_CPF_irrg_mgt_filename)                                             //150416
      load_events_for_simulation(*option_CPF_irrg_mgt_filename);                 //150416
   }
   #endif
   /*
   #ifdef SOILR
   crop->plant_crop(start_date);
   #endif
   */
//             Initialize soil temperatures (must be done after starting soil)
// 041111 Note, should beable to move this initialization to soil start because
// we now pass weather to soil start
   #ifndef VIC_CROPSYST_VERSION
   #ifdef SOIL_TEMPERATURE
   float64 air_temp_avg_first_day =                                              //141214
       meteorology.ref_air_temperature_avg().Celcius();                          //151023
   if (soil->temperature_profile)                                                //020620
       soil->temperature_profile->long_term_initialization                       //990120
       (scenario_control_and_model_options.annual_temperature.avg                //130827_060502
       ,air_temp_avg_first_day);                                                 //141214
   #endif
   #endif
   //    Note: Don't need to initialize soil_temperature profile hourly here because this is done in constructor at startup //041111
   float64 water_depth_profile = soil
      ? soil->ref_hydrology()->provide_water_depth_profile(true)                 //180106_160412_140918
      : 0.0;                                                                     //180106
   ANNL_accumulators_active       = new Annual_accumulators(*this,water_depth_profile,0.0);   //131004_990312
   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
   CONTINUOUS_accumulators_active = new Period_accumulators(*this,water_depth_profile,0.0);   //131004_990312
   #endif
   #if (CROPSYST_PROPER==4)
   FALLOW_accumulators_active     = new Period_accumulators(*this,water_depth_profile,0.0);   //131004_990312
   #endif
   #ifdef CONTINUOUS_CROP
   GP_accumulators_active         = new Period_accumulators(*this,water_depth_profile,0.0);   //131004_990313
   #else
   GP_accumulators_active         = 0;                                           //990313
   #endif
   #ifdef DETAILED_BALANCES
   if (C_balance)                                                                //111105
       C_balance->initializing = false;                                          //111005
   #endif
   // guarenteed have event scheduler at this point
   event_scheduler->take_events(scenario_event_list);                            //151203
   started &= CS::Simulation_unit::start();                                      //151203
   return started;                                                               //010207
}
//_start_______________________________________________________________________/
bool Land_unit_simulation_VX::stop()
{  // act_stop_date is not used here, but is used in derived classes
   bool stopped = true;                                                          //160614
   parameterize_organic_matter("equilibrated.organic_matter");                   //170227_150826
   stopped &= CS::Simulation_unit::stop();                                       //151203
   return stopped;
}
//_stop_____________________________________________________________2003-09-07_/
bool Land_unit_simulation_VX::start_growing_season
(modifiable_ Crop_model_interface &crop_at_start_season)                         //190616
{
   crop_active_or_intercrop = &crop_at_start_season;    // Don't delete active crop if it is a perennial, it will be the same value.
   delete GP_accumulators_active;                                                //070227
   #if (CROPSYST_PROPER==4)
   delete FALLOW_accumulators_active; FALLOW_accumulators_active = 0;            //070227
   #endif
   GP_accumulators_active = new Period_accumulators(*this,
      soil->ref_hydrology()->provide_water_depth_profile(true),0.0);             //160412_140918_070227
   return true;                                                                  //070227
}
//_start_growing_season_____________________________________________2007-02-27_/
bool Land_unit_simulation_VX::start_day()
{  bool started = true;
/*
if (simdate.get_date32() ==2013265)
{ std::clog << "reached" << endl; }
*/
   #ifdef RESIDUES
   if (scenario_control_and_model_options                                        //160127
      .residue_recalibration_date.get_DOY() == simdate.get_DOY())                //160127
      started &= initialize_residues();                                          //160127
   #endif
   float64 crop_evapotranspiration_max_m = 999999.0;                             //151113_070228
      //200323 RLN I am not sure where VIC got or used this before.

   //ET should be started before everything else
   //190721LML Claudio decide CropSyst use its own reference ET
   //#ifndef VIC_CROPSYST_VERSION
   float64 ETref_m = meteorology.ref_ET_reference().get_m();
      // At this print ETref would be value from UED file
      // if so recorded.
   #if (CROPSYST_PROPER == 4)
   if (scenario_control_and_model_options.evapotranspiration_model_clad.get() != ET_recorded) //180908
   {
      ET_ref->update();                                                          //150804
      ETref_m =ET_ref->get_m();                                                  //180416
   }
   #endif
   // ET_ref update would update resistance, but when we get ET from UED file
   // that is not calculated.  I should either have aerodynamics be a separate
   // simulation element, or simply use update idiom here.
   //200829LML added macro
   #if (CROPSYST_PROPER == 4)
   resistance_of_plants_d_m = meteorology.ref_aerodynamics()
      .calc_resistance_of_plants_d_m(meteorology.ref_wind_speed().get_m_d());    //180805
   #endif
   crop_evapotranspiration_max_m                                                 //151113_070228
   =  ( crop_active_or_intercrop)
      ? crop_active_or_intercrop->update_evapotranspiration_max(ETref_m)
         // Note that the crop here is in yesterday state
         //190812 update_pot_evapotranspiration(ETref_m)                         //121115_100315
      : ETref_m;                                                                 //011129
   output_crop_evapotransp_max_m = crop_evapotranspiration_max_m;
   //#endif
      //VIC_CROPSYST_VERSION
   #ifdef DETAILED_BALANCES
   if (C_balance) C_balance->advance();
   #endif
   // today output by simulation engine
      event_scheduler->take_events(scenario_event_list);                         //151003

   bool no_crop_cover = true;
   if (crop_active_or_intercrop)                                                 //200404
   {
      #ifdef CROPSYST_PROPER
      if (crop_active_or_intercrop->is_at_season_start())
      {
         started &= start_growing_season(*crop_active_or_intercrop);             //200404
      }
      #endif
      if (crop_active_or_intercrop->is_at_season_end())
      {  // Perhaps this may be more appropriate in end_day
         #ifdef CROPSYST_PROPER
         if (crop_nitrogen_mass_balance_ref)                                     //070227
            crop_nitrogen_mass_balance_ref->deactivate_seasonal();               //070227
         #endif
      }
      no_crop_cover = crop_active_or_intercrop->
         ref_phenology().is_in_germination();

      if (crop_active_or_intercrop->is_terminate_pending())                      //180101
      {  delete crop_active_or_intercrop;
         crop_active_or_intercrop = 0;                                           //180101
         crop_fract_canopy_cover = 0;
         // Warning may net to forget inspection
      } else
      {  crop_fract_canopy_cover =                                               //200126
            crop_active_or_intercrop->get_canopy_interception_global_total();    //200228_130716_070228

         crop_active_or_intercrop->know_snow_pack_thickness                      //161110
          (meteorology.get_snow_pack_thickness());                               //161110
         crop_active_or_intercrop->start_day();                                  //040206
      }
   } else
      crop_fract_canopy_cover = 0;

   // Need to start simulation unit after
   // - taking scenario events so they can get scheduled today.
   // - starting the crop because we may have reached phenology of thermal time

   started &= CS::Simulation_unit::start_day();

/*200404 moved
   #ifndef VIC_CROPSYST_VERSION
   if (
      //200404 crop_at_end_seasonX
      crop_active_or_intercrop && crop_active_or_intercrop->is_at_season_end()   //200404
      && crop_nitrogen_mass_balance_ref)                      //070227
      crop_nitrogen_mass_balance_ref->deactivate_seasonal();                     //070227
   #endif
*/
/*200404 moved
   if (crop_at_start_season)
   {
      started &= start_growing_season(*crop_at_start_season);                    //130716
      crop_at_start_season = 0;
   }
*/
/*200404 moved
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   if (crop_active_or_intercrop)
   {
       crop_active_or_intercrop->mod_phenology()
         .clear_days_since_start_of_harvest();                                   //131106
   }
   #endif
*/
   // PHENOLOGY_2018  days since start of harvest doesn't appear to be used anywhere
   total_irrigation_today = 0.0;                                                 //040528
   #ifdef AUTOMATIC_IRRIGATION
   fixed_irrigation_scheduled_today = false;
   automatic_irr_amount = 0.0;
   #endif
   //190707 repeated at end of function harvest_today_flag = false;              //160118
   #ifdef NITROGEN
   nitrogen_daily_output.clear();                                                //131001
   #endif
   grazing_BM_demand_today_kg_m2 = 0;                                            //060323
   unsatisfied_harvest_BM_today_kg_m2 = 0.0;                                     //081212
   earliest_irrigation_start_hour = 0;
   irrig_appl_target = 0.0;                                                       //080118
   #ifdef RESIDUES
   if (organic_matter_residues_profile_kg_m2)                                    //060817
       started &= organic_matter_residues_profile_kg_m2->start_day();            //151113_060803
   #endif
   /*200404 moved above
   bool no_crop_cover =
      is_fallow() ||                                                             //031119
      #ifdef INTERCROPPING
         predominant_crop->
      #else
         crop_active_or_intercrop->
      #endif
      //190826  implement this function has_cover();                             //190826
      ref_phenology().is_in_germination();                                       //181108_130903
   */
   #ifndef VIC_CROPSYST_VERSION
   started &= soil->mod_evaporator()->know_conditions                            //160606
         (no_crop_cover
         ,meteorology.season.is_summer_time()
         ,mulch_cover_fraction);
   #endif
   #ifdef AUTOMATIC_IRRIGATION
   #  ifdef SALINITY
   Soil::Salt_interface *soil_salinity = soil->mod_salinity();                   //160412_051120
   if (soil_salinity)                                                            //061026
      started &= soil_salinity->start_day();                                     //151113_061026
   #  endif
   #endif
   #ifdef CHEM_PROFILE
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   if (soil_chemicals) soil_chemicals->start_day();                              //140918
   #endif
   has_any_snow = false;
   #if (!defined(VIC_CROPSYST_VERSION)) // now common to MicroBasin && !defined(MICROBASIN_VERSION))
   float64 snow_water_equivelent_depth= meteorology.get_snow_water_equivelent_depth(); //151028
   has_any_snow = snow_water_equivelent_depth > 0.00001;                         //080508_151113
   #endif
   /*200404 moved above
   if (crop_active_or_intercrop)                                                 //070228
   {
      crop_fract_canopy_cover =                                                  //200126
         crop_active_or_intercrop->get_canopy_interception_global_total();       //200228_130716_070228
      if (crop_active_or_intercrop->is_terminate_pending())                      //180101
      {  delete crop_active_or_intercrop;
         crop_active_or_intercrop = 0;                                           //180101
         // Warning may net to forget inspection
      } else
      {  crop_active_or_intercrop->know_snow_pack_thickness                      //161110
          (meteorology.get_snow_pack_thickness());                               //161110
         crop_active_or_intercrop->start_day();                                  //040206
      }
   } else
      crop_fract_canopy_cover = 0;                                               //200126
   */

   //200404 crop_at_end_seasonX = 0;                                                       //070227
   /*200126 moved above
   crop_fract_canopy_cover  // (total) This is used in a number of places        //070228
      = (crop_active_or_intercrop
         ? crop_active_or_intercrop->get_fract__canopy_cover_total()              //130716_070228
         : 0.0);
   */
   float64 fract_residue_cover_unused = 0;
   float64 daily_pot_evaporation                                                 //160412
      = has_any_snow
      ? 0.0 // This should disable evaporation when there is snow
      : (1.0-crop_fract_canopy_cover) * crop_evapotranspiration_max_m;
   started &= distribute_evaporation_potential_daily_to_intervals                //160719
      (daily_pot_evaporation);
   #ifdef RESIDUES
   Residues_interface *local_residue_pools_kg_m2
         = organic_matter_residues_profile_kg_m2
            ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;         //060115
   if (local_residue_pools_kg_m2)
   {
      local_residue_pools_kg_m2->know_fract_canopy_cover(crop_fract_canopy_cover); //160523
      local_residue_pools_kg_m2->start_day();                                    //160523
   }
   #endif
   #if (!defined(VIC_CROPSYST_VERSION))
      pond.start_day();                                                          //160603
      started &= soil->start_day();                                              //151113
      // Eventually pond will be dynamic element added to list
      // of sub simulation elements, but I need to make sure
      // the ordering of (potential evaporation) deduction.
   #else
   has_any_snow = meteorology.get_snow_pack() != 0;                              //151113
   #endif
   harvest_today_flag = false;                                                   //160118
   return started;
}
//_start_day________________________________________________________2002-03-09_/
bool Land_unit_simulation_VX::process_day()                        modification_ //150611_130624
{  bool processed = true;
   processed = CS::Simulation_unit::process_day();                               //150611_130830
   float64 plant_height = 0;                                                     //200412
   bool crop_has_emerged = false;                                                //200412
   std::string growth_stage_description;                                         //200412
   if (crop_active_or_intercrop)                                                 //200412
   {
/*200503 This didn't work here with perennial crops (already advanced day)
      #ifdef CROPSYST_PROPER
      if (crop_active_or_intercrop->is_at_season_start())
      {  // crop may come into existance (planted) in process day (event handling)
         processed &= start_growing_season(*crop_active_or_intercrop);             //200404
      }
      #endif
*/
      predominant_crop =                                                            //190705 (was in start_day)
         #ifdef INTERCROPPING
         get_predominant_crop();
         #else
         crop_active_or_intercrop;
         #endif
      plant_height =                                                             //200412
         #ifdef INTERCROPPING
         predominant_crop ? predominant_crop                                     //200412
         //200412 ,get_predominant_crop() ? get_predominant_crop()
         #else
         crop_active_or_intercrop ? crop_active_or_intercrop
         #endif
            ->get_plant_height_m() : 0.0;

      // now keep a reference to weather so we don't have to call functions
      #ifdef CROP_ORCHARD
      if (   predominant_crop                                                       //040206
          && predominant_crop->is_fruit())                                          //190707
      {  if (predominant_crop->is_harvestable())                                    //190614
         {  // I am not sure this is the best place to put this                     //081204
            CropSyst::Harvest_or_clipping_operation default_fruit_harvest;          //081204
            default_fruit_harvest.terminate_crop = false;                           //081204
            perform_harvest_or_clipping(default_fruit_harvest);                     //081204
         }
      }
      #endif
      crop_has_emerged =                                                         //200412
            #ifdef INTERCROPPING
           predominant_crop ? (Land_use)predominant_crop->                       //130716
               //140316 warning not sure what to do in case of intercrop
            #else
           crop_active_or_intercrop ? crop_active_or_intercrop->
           #endif
           ref_phenology().has_emerged() : false;                                //181108

      #if ((CS_VERSION==4))
      //NYI if (arguments.progress);
      {
         std::clog
         << simdate_raw
         << " cycle=" << (int)cycle_shift << ":";                      //160603
         CORN::Text_list period_descriptions;                                    //200420
         period_descriptions.set_write_delimiter('&');
         /*200420
         std::string growth_stage_description;                                   //190613
         #ifdef INTERCROPPING
         if (predominant_crop)                                                   //130716_070228
              std::clog << predominant_crop->                                    //130716
         #else
         if (crop_active_or_intercrop)                                           //070228
              std::clog << crop_active_or_intercrop->
         #endif
         describe_growth_stage(growth_stage_description);
         else std::clog << "fallow";                                             //070228
         */

         nat8 periods =
            #ifdef INTERCROPPING
            (predominant_crop)                                                   //130716_070228
            ? predominant_crop->                                                 //200420_130716
            #else
            (crop_active_or_intercrop)                                           //070228
            ? crop_active_or_intercrop->                                         //200420
            #endif
            describe_periods(period_descriptions)                                //200420
            : 0;                                                                 //200420
         if (periods)                                                            //200420
            period_descriptions.write(std::clog);                                //200420
         else std::clog << "fallow";                                             //070228
         std::clog << "   \r";
      }
      #endif
   } // end crop
   Residues_interface *local_residue_pools_kg_m2
         = organic_matter_residues_profile_kg_m2
         ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;            //060115
   soil->current_plant_available_water
         (crop_active_or_intercrop
          ? crop_active_or_intercrop->get_total_fract_root_length_m(): 0);       //070228
   #ifdef CHEM_PROFILE
   Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();
   #endif
   #ifdef MANAGEMENT
   total_irrigation_today += irrig_appl_target;

//100505 Note it looks like autoirrigation is performed before any precipitation
// In version 5 we might want to check if there is any precipitation first
// (presumably irrigation would not be performed if precipitation was even expected that day.)
   process_any_automatic_irrigation();                                           //071017
   #endif
   daily_liquid_precipitation =
      meteorology.ref_precipitation_liberated().get_m();                         //170321_151124
   bool any_surface_water
      =  daily_liquid_precipitation > 0.0 || total_irrigation_today > 0.0;       //070107
   Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm = 0;
   #ifdef SOIL_INFILTRATION
   CORN::Time_second_clad storm_start_time;                                      //990505
   Minutes water_entering_soil_timestep_minutes = 60;                            //160511_070116
   const meteorology::Storms *optional_storms = meteorology.ref_storms();        //151215
   if (optional_storms && (daily_liquid_precipitation > 0.000001)                //151215_140226_990402
       &&(  (infiltration_model == FINITE_DIFFERENCE_INFILTRATION)               //190707
          ||(infiltration_model == CASCADE_HOURLY_INFILTRATION)))                //190707_070118
   {
      nat16 intervals = 24;                                                      //160511_140624_140217
      float64 daily_eff_precipitation_mm = m_to_mm(daily_liquid_precipitation);  //140217
      non_runoff_water_entering_in_intervals_mm = new Dynamic_water_entering_soil
         ((nat8)intervals);                                                      //160511
      optional_storms->get_eff_precipitation_mm_by_interval                      //151215
         (*non_runoff_water_entering_in_intervals_mm
         ,daily_eff_precipitation_mm
         ,storm_start_time
         ,intervals);
   }
   #endif
   #if (defined(MATCH_VB) || defined(SOIL_TEMPERATURE))
   float64 crop_LAI = crop_active_or_intercrop
      ? crop_active_or_intercrop->get_LAI(false) : 0.0;                          //070228
      //130716 WARNING  I think this would be a composite for intercrop
   #endif
   #ifndef VIC_CROPSYST_VERSION
   #ifdef SOIL_TEMPERATURE
   // We need soil temp before freezing,but after snow storage                   //980921
     if (soil->temperature_profile)
         soil->temperature_profile->find_soil_temp                               //041111 this should actually be an Update
         (crop_LAI
         ,any_surface_water
         ,local_residue_pools_kg_m2
         ,crop_fract_canopy_cover //global total
         ,plant_height);                                                         //200412

   // The following lines are probably applicable to MicroBasin implementation of
   // soil temperature, but currently this is setup by MicroBasin
   if (soil->temperature_profile_hourly)                                         //041111
   {
      soil->temperature_profile_hourly->know_has_snow_cover(meteorology.get_snow_pack() != 0);   //160721
      float64 stubble_AI = local_residue_pools_kg_m2 ? (local_residue_pools_kg_m2)->get_total_area_index(INCLUDE_POSITION_STUBBLE_SURFACE) : 0.0;
      float64 residue_AI = local_residue_pools_kg_m2 ? (local_residue_pools_kg_m2)->get_total_area_index(INCLUDE_POSITION_FLAT_SURFACE)    : 0.0;
      soil->temperature_profile_hourly->update(crop_LAI,stubble_AI,residue_AI);
   }
   #endif
   #endif
   #ifdef SOIL_FREEZING
   if (soil_freezing_optional)                                                   //070824
   {  // Eventually move this to a function freezing and thawing inside hydrology//070825
      soil_freezing_optional->                                                   //070824
      freezing_and_thawing(meteorology.get_snow_pack());                         //151022
      soil_hydrology_ref->update_liquid_and_ice_water_content();                 //070825
   }
   #endif
   float64 non_crop_intercept_precip = daily_liquid_precipitation;               //140430
   float64 non_red_intercept_precip = non_crop_intercept_precip;                 //190131
   #if (!defined(VIC_CROPSYST_VERSION) && !defined(MICROBASIN_VERSION))          //161108LML canopy interception occure in start_day() and residue interception occurs in hourly processes
   if (crop_active_or_intercrop)                                                 //070228
   {  non_crop_intercept_precip = crop_active_or_intercrop->intercept_precipitation(daily_liquid_precipitation); //140320
      //NYI trickle = non_crop_intercept_precip;
      float64 canopy_intercepted_precipitation_m_today
         = crop_active_or_intercrop->get_intercepted_precipitation_m();
      if (non_runoff_water_entering_in_intervals_mm)                             //140226
          non_runoff_water_entering_in_intervals_mm->deduct                      //990402
            (m_to_mm(canopy_intercepted_precipitation_m_today));
   }
   #ifdef RESIDUES
   // redid interception to accomodate intercropping
   float64 residue_interception=local_residue_pools_kg_m2
      ? local_residue_pools_kg_m2->water_interception(non_crop_intercept_precip)
      : 0.0;                                                                     //060115
   non_red_intercept_precip -= residue_interception;                             //190131_010202
   if (non_runoff_water_entering_in_intervals_mm)                                //140226
       non_runoff_water_entering_in_intervals_mm->deduct(m_to_mm(residue_interception));//990402
   #endif
   #endif
   float64 daily_est_runoff = 0.0;                                               //151006_981120
   #ifdef RUNOFF
   Land_treatment management_land_treatment =                                    //020719
   #ifdef MANAGEMENT
   curr_management ?  curr_management->param_land_treatment() :
   #endif
         DEFAULT_land_treatment;
   if (runoff)                                                                   //020620
   {  //200316 runoff->estimated_runoff = 0.0; // WARNING this needs to be moved //990422
      runoff->clear(); // WARNING this needs to be moved                         //200316
      runoff->calculate_crop_storage                                             //020719
         (management_land_treatment, crop_fract_canopy_cover );                  //020719
                                     // global_total
      float64 fract_residue_cover = local_residue_pools_kg_m2                    //060628
         ?  local_residue_pools_kg_m2->get_total_fract_ground_cover
            (INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE)
         : 0.0;
      runoff->calculate_residue_storage(fract_residue_cover);                    //990528
   }                                                                             //020620
   /*200412 moved above
   bool crop_has_emerged =
            #ifdef INTERCROPPING
           predominant_crop ? (Land_use)predominant_crop->                       //130716
               //140316 warning not sure what to do in case of intercrop
            #else
           crop_active_or_intercrop ? crop_active_or_intercrop->
           #endif
           ref_phenology().has_emerged() : false;                                //181108
   */
   daily_est_runoff = 0.0;                                                       //151006
   Soil_runoff_SCS *runoff_as_SCS = dynamic_cast<Soil_runoff_SCS *>(runoff);     //200316
   if (runoff_as_SCS)                                                            //200316
      /*200316
      daily_est_runoff =
      ( runoff && (scenario_control_and_model_options.runoff_model_clad.get()
                   == CURVE_NUMBER_RUNOFF_MODEL))                                //991110
      ?
         runoff->
      */
         daily_est_runoff = runoff_as_SCS->runoff                                                          //971204
           (
            (Hydrologic_condition)soil->parameters_ownable->hydrologic_condition_clad.get() //971011
           , management_land_treatment                                           //020719
           , crop_has_emerged                                                    //181113
           ,crop_fract_canopy_cover                                              //070228
           ,non_red_intercept_precip);
       //200316 :0.0;                                                                     //971204
   #endif
   #ifdef SOIL_INFILTRATION
   float64 non_runoff_precip = non_red_intercept_precip - daily_est_runoff;      //010202
   if (non_runoff_water_entering_in_intervals_mm)                                //140226
       non_runoff_water_entering_in_intervals_mm->deduct(m_to_mm(daily_est_runoff));  //990402
   float64 total_water_entering_soil =                                           //070107
            //NYI        +  local_residue_pools_kg_m2 ? local_residue_pools_kg_m2->get_water_from_change_in_biomass() : 0.0 //090306
      total_irrigation_today + non_runoff_precip ;                               //070107
   #ifdef NITROGEN
   wettings.append(total_water_entering_soil);                                   //131106_990716
   #endif
   #endif

   // We adjust random roughness after any tillage events
   // and before infiltration because presumablely
   // the farmer will not perform tillage after rainfall.
   #ifdef RUNOFF
   if ( runoff)                                                                  //020620
   {  float64 fract_residue_cover = local_residue_pools_kg_m2                    //060628
      ? local_residue_pools_kg_m2->get_total_fract_ground_cover
        (INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE) : 0.0;
       runoff->adjust_random_roughness                                           //990422
      (daily_liquid_precipitation
      ,crop_fract_canopy_cover                                                   //070228
      ,fract_residue_cover);
   }
   #endif
   {         // if automatic irrigation then override any irrigation salinity that may have been applied with fixed event   //070810
         float64 irrigation_salinity =                                           //020327
         #ifdef SALINITY
            #ifdef MANAGEMENT
            (curr_automatic_irrigation_mode)
            ? curr_automatic_irrigation_mode->get_salinity() :
            #endif
         #endif
            0.0;
         #ifdef SOIL_INFILTRATION
  // If we have a runon database
      Dynamic_float_array runon_in_intervals(0,1,0);                             //990409
#ifdef TEMP_DISABLED
2011-11-08   This watershed needs to be moved into a Land_unit_simulation::process
Need to be careful that it is done in the right sequence
      #ifdef WSHEDMOD
      soil_sublayer_array_64(lateral_flow_in);                                   //990525
      soil->clear_sublayer_array(lateral_flow_in);                               //990525
      if (cell_in_db)                                                            //990409
      {  cell_in_db->get_runon_in_intervals(today,runon_in_intervals);           //990525
         cell_in_db->get_lateral_flow_in_by_layer(today,soil->layers->count(),lateral_flow_in);       //990525
         if ( runoff && (scenario_control_and_model_options.runoff_model_clad.get() == CURVE_NUMBER_RUNOFF_MODEL))     //990728
              runoff->add_runoff_from_runon_curve_number(runon_in_intervals.sum()); //990728
         #ifdef LATERAL
         soil->subsurface_flow_in(lateral_flow_in);                              //990525
         #endif
      }
      #endif
#endif
         Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm // will be 0 if no irrigation  //070107
            = create_irrigation_in_intervals_mm                                  //070107
               (water_entering_soil_timestep_minutes);                           //070107

//for (int h = 0 ; h < 24; h++)
//cout << h << '\t' << non_runoff_water_entering_in_intervals_mm.get_no_resize(h) << endl;
         Dynamic_float_array runoff_in_intervals(0,1,0);                         //990409
         if (!(soil->infiltrate                                                  //990211
               (infiltration_model                                               //190707
               ,non_runoff_precip                                                //070107
               #ifdef MANAGEMENT
               ,total_irrigation_today
               ,irrigation_salinity
               #endif
               ,non_runoff_water_entering_in_intervals_mm                        //070119
               ,irrigation_water_entering_in_intervals_mm                        //070119
               ,runon_in_intervals                                               //990409
               ,runoff_in_intervals  // output                                   //990409
               ))) // additional runoff is local to soil classes in C++ version
         {  cerr << "Soil water infiltration failed:\nThis usually occurs when the finite difference method is unable to converge with water table conditions." << endl;   //080122
         }
      delete irrigation_water_entering_in_intervals_mm;                          //070119
      #endif
   }
   #ifdef MATCH_VB
   if (soil->temperature_profile_hourly)                                         //041111
   {  float64 stubble_AI = local_residue_pools_kg_m2 ? (local_residue_pools_kg_m2)->get_total_area_index(INCLUDE_POSITION_STUBBLE_SURFACE) : 0.0;
      float64 residue_AI = local_residue_pools_kg_m2 ? (local_residue_pools_kg_m2)->get_total_area_index(INCLUDE_POSITION_FLAT_SURFACE): 0.0;
      soil->temperature_profile_hourly->update(crop_LAI,stubble_AI,residue_AI);
   }
   #endif
   if (organic_matter_residues_profile_kg_m2)                                    //060628
       organic_matter_residues_profile_kg_m2->process_day();                     //150611_060214
   processed &= evaporate();                                                     //160408
   if (crop_active_or_intercrop)
   {
      if (!crop_active_or_intercrop->is_terminate_pending())                     //180404
         processed &= crop_active_or_intercrop->process_day();                   //180404_150611_111121
      #ifdef MANAGEMENT
      process_any_automatic_clipping(false);                                     //990913
      #endif
   } // else just fallow
   #ifdef CHEM_PROFILE
   if (soil_chemicals )
   {  Seconds preferred_transformation_timestep = seconds_per_day;
      if (scenario_control_and_model_options.organic_matter_and_residue_model_clad.get() == multiple_SOM_multiple_residues)
      {  preferred_transformation_timestep = seconds_per_hour;
         // Transformation is run at least hourly irregardless of the hydrology_time_step_resolution_today.
         // if (infiltration_model== FINITE_DIFFERENCE_MODEL)
         //    preferred_transformation_timestep = std::min<xxxx>(seconds_per_hour,soil->get_hydrology_timestep_resolution_today());
      }
      preferred_transformation_timestep = seconds_per_hour; // not sure why single pool was only daily timestep
         // Transformation is run at least seconds_per_hour
      soil_chemicals->chemical_transformations(preferred_transformation_timestep);
   }
   #endif
   #ifdef EROSION
   if (soil->erosion) // obs active_model_options->run_erosion
   {   soil->erosion->
         know_cover
         (crop_fract_canopy_cover
         ,local_residue_pools_kg_m2 ? local_residue_pools_kg_m2->get_surface_plant_biomass(INCLUDE_POSITION_FLAT_SURFACE | INCLUDE_POSITION_STUBBLE_SURFACE) : 0.0   //170107_161223_011011
         ,local_residue_pools_kg_m2 ? local_residue_pools_kg_m2->get_subsurface_plant_biomass() : 0.0   //161223_011011
         );
         // Now considering only plant residue (previouly included also manure
         // but some manures were greatly impacting the mulch factor).
         // Roger thinks probably should still include a manure, but it shouldn't
         // be so influential.
      processed &= soil->erosion->process_day();                                 //160506
   }
   #endif
   {
      /*200412 moved above
      #if ((CS_VERSION==4))
      //NYI if (arguments.progress);
      {
         std::clog << " cycle=" << (int)cycle_shift << ":";                      //160603
         std::string growth_stage_description;                                   //190613
         #ifdef INTERCROPPING
         if (predominant_crop)                                                   //130716_070228
              std::clog << predominant_crop->                                    //130716
         #else
         if (crop_active_or_intercrop)                                           //070228
              std::clog << crop_active_or_intercrop->
         #endif
         describe_growth_stage(growth_stage_description);
         else std::clog << "fallow";                                             //070228
         std::clog << "\r";
      }
      #endif
      */
      #ifdef RESIDUES
      if (local_residue_pools_kg_m2)                                             //060628
          local_residue_pools_kg_m2->balance(scenario_control_and_model_options.run_nitrogen); //980718
      // Save today's values
      // At the end of the year tell the database to flush all recorders;        //000322
      #endif
   }
   // We now do all annual things on Dec31                                       //990318
   #ifdef LATERAL
   if (is_GIS_cell() && subsurface_flow)                                         //990513
   {
         soil_sublayer_array_64(lateral_flow_depth_m);                           //990513
         soil->clear_sublayer_array(lateral_flow_depth_m);                       //990526
         if (soil->subsurface_flow_out(cell_width,lateral_flow_depth_m))         //990513
            cell_out_db->set_lateral_flow_out_by_layer                           //990513
            (today
            ,soil->layers->count()
            ,lateral_flow_depth_m);
   }
   #endif
   if (soil)                                                                     //070801
   {
      processed &= soil->process_day();                                          //190709_150611_051205
      #ifdef RUNOFF
      if (runoff && using_numerical_runoff_model)                                //130308
      {
         float64 surface_storage = soil->parameters_ownable->compute_surface_storage
                  ? runoff->maximum_depression_storage()
                  : mm_to_m(soil->parameters_ownable->get_surface_storage_mm());
         pond.surface_water_storage_capacity = surface_storage;                  //130308
            // user_or_computed_surface_storage
      } else
      #endif
         {
         #if (!defined(MICROBASIN_VERSION) && !defined(VIC_CROPSYST_VERSION))
         pond.surface_water_storage_capacity
            = mm_to_m(soil->parameters_ownable->get_surface_storage_mm());       //160921
         #endif
      }
      // must let the OM/residues know the current soil disturbance immediately  //070801
      if (organic_matter_residues_profile_kg_m2)                                 //080118
          organic_matter_residues_profile_kg_m2->know_soil_disturbance(soil->get_disturbance());//070801
      #if defined(CROPSYST_PROPER)
      // This is a hack, we need to store the new layer OM mineralization in the old
      // mineralization array for output (in daily.UED)
      if (soil &&  soil->chemicals && organic_matter_residues_profile_kg_m2
          #if ((CROPSYST_VERSION >0) && (CROPSYST_VERSION < 5))
          && (scenario_control_and_model_options.organic_matter_and_residue_model_clad.get()
              != V4_1_ORGANIC_MATTER_AND_RESIDUE)
          #endif
          )
      {
          #ifdef NITROGEN
          for (nat8 layer = 1; layer <= soil->ref_layers()->count(); layer++)    //160412_140908
             soil->chemicals->mineralization_N_E_output[layer]
                = organic_matter_residues_profile_kg_m2->
                  get_soil_organic_matter_mineralization_N(layer);
          #endif
      }
      #endif
   }
//if (debug_surface_residue_biomass != 0)
//{
//cout << "BD:" <<debug_surface_residue_biomass << '\t' << debug_subsurface_residue_biomass<< endl;
//debug_surface_residue_biomass    = local_residue_pools_kg_m2->get_surface_all_biomass();
//debug_subsurface_residue_biomass = local_residue_pools_kg_m2->get_subsurface_all_biomass();
//cout << "ED:" << debug_surface_residue_biomass << '\t' << debug_subsurface_residue_biomass<< endl;
//}
//QND_GP_immobilization +=  organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2-> get_soil_organic_matter_immobilization_profile_N() : 0; //110918
   #ifdef SOIL_INFILTRATION
   delete non_runoff_water_entering_in_intervals_mm;                             //140226
   #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   #ifdef PEST_VERSION
   perform_triggered_synchronizations_today();                                   //160203
   #endif
   #endif
   return processed;                                                             //010207
}
//_process_day____________________________________________________________2002_/
bool Land_unit_simulation_VX::end_day()
{  bool ended = true;                                                            //151016
   #ifndef VIC_CROPSYST_VERSION
   ended &= pond.end_day();                                                      //160719
   #endif
   ended &= nitrogen_daily_output.end_day();                                     //131001
   if (organic_matter_residues_profile_kg_m2)
   {
   profile_carbon_output.sum_profile   = (organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,ENTIRE_PROFILE_DEPTH_as_9999,false));
   //NYN profile_carbon_output.sum_5cm = (organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,0.05,false));
   //NYN profile_carbon_output.sum_10cm= (organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,0.10,false));
   //NYN profile_carbon_output.sum_15cm= (organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,0.15,false));
   profile_carbon_output.sum_30cm      = (organic_matter_residues_profile_kg_m2->sum_carbon_to_depth_kg_m2(INCLUDE_RESIDUE_OM|INCLUDE_SOM_AND_MICROBIAL,0.30,false));
   }
   if (crop_active_or_intercrop)                                                 //070227
   {
      bool end_season = crop_active_or_intercrop->is_at_season_end();            //200404
         // need to get the end_season status before end day because
         // end_day increment quiescence.
      crop_active_or_intercrop->end_day();                                       //070227
         // need to end_day before end_growing_season
         // to step outputs for end_growing_season
      if (end_season)                                                            //200404
         end_growing_season(*crop_active_or_intercrop);                          //200404
      /*200404
      #ifdef MANAGEMENT
      bool terminate_crop = crop_active_or_intercrop                             //181111
         && crop_active_or_intercrop->is_terminate_pending();                    //181111
      if (terminate_crop)                                                        //181111
      {  crop_at_end_seasonX = crop_active_or_intercrop;                          //181111
         if (curr_automatic_irrigation_mode) curr_automatic_irrigation_mode = 0; //181111_140529
         if (curr_automatic_clip_mode)       curr_automatic_clip_mode = 0;       //181111_140529
      }
      #endif
      if (crop_active_or_intercrop->ref_phenology().is_in_quiescence())          //181111
         // actually should be first day                                         //181111
         crop_at_end_seasonX = crop_active_or_intercrop;                         //181111
      */
   }                                                                             //070227
   if (soil) ended &= soil->end_day();                                           //151016_130911
   #ifdef CHEM_PROFILE
   Soil::Chemicals_profile       *soil_chemicals = get_soil_chemicals();
   if (soil_chemicals)           soil_chemicals->chemical_balances();  // balances are needed before output
   #endif
   update_accumulators();                                                        //151028
   #ifndef __linux
   //130807  Not yet implemented for Linux need to add C_balanc_dat to Scenario::Output class (this is rarely used)
   #ifdef DETAILED_BALANCES
   #if (CROPSYST_VERSION==4)
   if (scenario_directory)                                                       //111105
   {  if (!C_balance_file)
      {  C_balance_file = new std::ofstream(scenario_directory->Output().C_balance_dat().c_str()); //111005
         (*C_balance_file) << "date";
         (*C_balance_file) << C_balance->write_column_headers((*C_balance_file)) << std::endl;
      }
      (*C_balance_file) << simdate.as_string() << (*C_balance) << std::endl;
   }
   #endif
   #endif
   #endif
   /*2004040 moved above
   if (crop_at_end_seasonX)                                                       //091119_070227
      ended &= end_growing_season(*crop_at_end_seasonX);                          //151016
   */
   #ifdef RESIDUES
   distribute_any_crop_biomass_fate();                                           //070625
   Residues_interface *local_residue_pools_kg_m2                                 //160627
         = organic_matter_residues_profile_kg_m2                                 //160627
            ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;         //160627
   if (local_residue_pools_kg_m2)                                                //160627
      ended &= local_residue_pools_kg_m2->end_day();                             //160627
   #endif
   ended &= CS::Simulation_unit::end_day();                                      //160628
   return ended;                                                                 //151016
}
//_end_day__________________________________________________________2005-01-15_/
bool Land_unit_simulation_VX::end_growing_season
(Crop_model_interface &crop_ending_its_season)
{
   if ( (grazing_hold_manure_application.org_N_amount_kg_ha > 0.0)
      ||(grazing_hold_manure_application.NH3_amount_kg_ha))                      //041220
   {  // For the grazing model, we hold all the manure applications until the end of the season
      // This is to prevent thousands of manure pools from being generated.
      #if ((CS_VERSION > 0) && (CS_VERSION <5))
      CropSyst::Organic_nitrogen_operation manure_op;                            //040126
      manure_op.organic_matter_source_cowl.set(beef_fresh_manure);               //110831
      manure_op.org_N_volatilization_calculated=true;                            //040126
      //200220obs manure_op.decomposition_time__calculated=true;                              //040126
      //200220obs manure_op.decomposition_time_)63=60.0;                                      //040126
      manure_op.halflife=40.0;                                                   //040126
      manure_op.solid_liquid_form_clad.set(solid_form);                          //040126
      #else
      // In V5, use biomatter_application_operation
      Biomatter_application_operation manure_op;
      manure_op.provide_decomposition_parameters(beef_fresh_manure);
      // decomposition time is set up in the decomposition parameters.
      // 100216 I need to trace here to see out the solid form is now handled.
      #endif
      manure_op.NRCS_operation_description = "Graze, rotational";                //060725
         // This was originally intended for Oumarou's rotational grazing optimization model
      manure_op.org_N_kg_ha=grazing_hold_manure_application.org_N_amount_kg_ha;  // need to pass in kg/ha because these are the units in the parameter file          //040126
      manure_op.NH3_N_kg_ha=grazing_hold_manure_application.NH3_amount_kg_ha;    // need to pass in kg/ha because these are the units in the parameter file          //040126
      manure_op.org_N_appl_method_cowl.set(SURFACE_BROADCAST_INCORPORATION);
      manure_op.long_term_org_N_volatilization_loss_percent=0.0;     // NH3_volatilization will be calculated                                                        //040126
      #ifdef MANAGEMENT
      #if ((CS_VERSION > 0) && (CS_VERSION <5))
      perform_organic_nitrogen(manure_op);                                       //040126
      #else
      #ifdef NITROGEN
      perform_biomatter_application(manure_op);
      #endif
      #endif
      #endif
      grazing_hold_manure_application.org_N_amount_kg_ha = 0.0;                  //041220
      grazing_hold_manure_application.NH3_amount_kg_ha   = 0.0;                  //041220
   }

   if (crop_active_or_intercrop) // if the crop is still active then is simply the end of the season
   { // is simply the end of the season
      /* 180404
         At this point (in the case of harvest)
         the crop has already called its end_season().
         I think in all other cases where end_season in triggered
         end_season() will already have been called

      crop_active_or_intercrop->end_season();                                    //160629

      */

         //180404 I don't think it is necessary to check is_terminate_pending
         // because we probably this isnt called unless is_terminate_pending
         // has been checked

      #ifdef INTERCROPPING
      if (   get_predominant_crop()->is_terminate_pending()                      //130716
          &&!get_secondary_crop())                                               //130716
      #else
      if (crop_active_or_intercrop->is_terminate_pending())  // There has to be a crop_active at this point  //070227
      #endif
         start_fallow();
      #ifdef CHEM_PROFILE
      Soil::Chemicals_profile       *soil_chemicals = get_soil_chemicals();
      if (soil_chemicals )
      soil_chemicals->reinitialize_at_end_season();     // resets accumulators
      #endif
   }
   #ifndef VIC_CROPSYST_VERSION
   if (crop_nitrogen_mass_balance_ref) crop_nitrogen_mass_balance_ref->end_season();
   #endif
   #ifdef MANAGEMENT
   if (curr_automatic_irrigation_mode) curr_automatic_irrigation_mode = 0;       //181111_140529
   if (curr_automatic_clip_mode)       curr_automatic_clip_mode = 0;             //181111_140529
   #endif
   #ifdef INTERCROPPING
   // Done last because now we switch the active crop to the new predomonate crop
   for (int intercrop_index = 0; intercrop_index < 2; intercrop_index++)
   {  Crop_interfaced *crop_active_or_intercrop = get_intercrop(intercrop_index);
      // In INTERCROPPING mode, ths crop_active_or_intercrop is local to the loop
   #endif
   return true;
}
//_end_growing_season_______________________________________________2007-02-27_/
#if (CROPSYST_VERSION == 4)
bool Land_unit_simulation::end_growing_season
(Crop_model_interface &crop_active)  // when this becomes permanent rename to crop_ending_its_season
{  bool result = true;
   #ifdef LADSS_MODE
   if (LADSS_harvest_database_table)                                             //021206
   {  LADSS_season_output_data_record LADSS_harvest_record                       //040217
         (LBF_ID,*this,
         #ifdef INTERCROPPING

         #endif
         crop_active_or_intercrop);
      LADSS_harvest_database_table->set(LADSS_harvest_record);
   } else
   #endif
      result = Land_unit_simulation_VX::end_growing_season(crop_active);         //130716
   return result;
}
//_end_growing_season_______________________________________________2011-11-08_/
#endif
void  Land_unit_simulation_VX::start_fallow()
{
   #ifdef MANAGEMENT
   curr_automatic_clip_mode = 0; // Make sure that clipping is disabled          //040514
   #endif
   #if (CROPSYST_PROPER==4)
   FALLOW_accumulators_active = new Period_accumulators                          //051012
      (*this
      ,soil->ref_hydrology()->provide_water_depth_profile(true)                  //160412_140923
      ,0.0);
   #endif
   soil->reinitialize_for_crop(-1500.0); // in this case fallow conditions       //070228
   if (runoff) runoff->know_crop_land_use(FALLOW);                               //190617
}
//_start_fallow_____________________________________________________2002-11-14_/
#ifdef OBSOLETE
200404 this was convoluted, now replaced in all versions
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
nat16 Land_unit_simulation_VX::perform_crop_synchronizations
(Crop_model_interface &crop)                                       modification_
{
   const Phenology &crop_phenology = crop.ref_phenology();                       //181113

   if (crop_phenology.started_today(NGS_PLANTING))
      crop_at_start_season = &crop;
         // Note: sowing does set the active crop,
         // but we set crop_at_start_season to activate the start of season
   if (((crop_phenology.started_today(NGS_ACTIVE_GROWTH)                         //130915
                     &&  !GP_accumulators_active)
       || crop_phenology.started_today(NGS_RESTART)
       )
       && (crop_phenology.get_days_since_start_of(NGS_PLANTING) >=0)             //1300915
       )
      crop_at_start_season = &crop;
   if (crop_phenology.started_today(NGS_QUIESCENCE)
       || crop.is_terminate_pending())                                           //180101
     { crop_at_end_seasonX = (&crop);  }

   return 0;
}
//_perform_crop_synchronizations____________________________________2013-09-03_/
#endif
#endif
#ifdef RESIDUES
bool Land_unit_simulation_VX::distribute_any_crop_biomass_fate()
{
   if (crop_mass_fate_to_be_distributed )
   {   Crop_mass_fate *biomass_to_be_distributed = crop_mass_fate_to_be_distributed->biomass;
       float64 removed_for_grazing= biomass_to_be_distributed->grazing_to_manure;//040605
       // Note the crop_biomass_fate->grazing_to_manure
      // has no meaning it is not reapplied to the field, it is simply lost.
      // At this point it is presumed to be outside the simulation balance.
      // I should simply add it to the loss for biomatter balance.
      biomass_to_be_distributed->disposal
         += biomass_to_be_distributed->grazing_to_disposal;
      biomass_to_be_distributed->grazing_to_disposal = 0.0;                      //070625
      if (removed_for_grazing > 0.0)                                             //040126
      {  // At this point we should have total_removed_plant_N
         // which is already deducted from the balance.
         // Actually we should keep a separate variable for the
         // total grazed plant N because it is possible that there
         // is a recular clipping on the same day as grazing,
         // that however, is very unlikely management.
         float32 grazing_manure_N_production
            = biomass_to_be_distributed->grazing_to_manure;                      //040126
         // Now generate an organic fertilization event to process today
         if (!CORN::is_zero<float32>(grazing_manure_N_production,0.0))           //200127_040202
         {
            #ifdef MANAGEMENT
            //LML 140908 added the macro
            float32 org_N_amount_kg_ha= over_m2_to_ha *(grazing_manure_N_production * curr_management->latest_grazing_percent_N_as_OM / 100.0);            //040126
            float32 NH3_amount_kg_ha  = over_m2_to_ha *(grazing_manure_N_production * curr_management->latest_grazing_percent_N_as_NH3/ 100.0);            //040126
            grazing_hold_manure_application.org_N_amount_kg_ha+=org_N_amount_kg_ha; //041220
            grazing_hold_manure_application.NH3_amount_kg_ha  +=NH3_amount_kg_ha;//041220
            #endif
         }                                                                       //040202
      }                                                                          //040126
      #if defined(RESIDUES)
      Residues_interface *local_residue_pools_kg_m2
         = organic_matter_residues_profile_kg_m2
         ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;            //060628
      Crop_model_interface *fated_crop = get_crop();                             //200404_190617_070625
         //200404 get_crop_active_or_at_end_of_season();  //190617_070625
      const Residue_decomposition_parameters *residue_decomp_params
         = fated_crop? &fated_crop->get_residue_decomposition_parameters() : 0;  //160708060219_110613
      if (residue_decomp_params && local_residue_pools_kg_m2)                    //060219
      {  const Crop_nitrogen_interface *crop_N_kg_m2=fated_crop->get_nitrogen(); //060219
         float64 crop_surface_residue_N_conc
            = crop_N_kg_m2 ? crop_N_kg_m2->get_residue_concentration() : 0;      //061215
         if (biomass_to_be_distributed->chaff_or_leaf_litter > 0.0000001)
         {  local_residue_pools_kg_m2->add_surface_plant_pool(OM_flat_surface_position  //060219
               ,OM_ORIGINAL_CYCLING // In V4.3 the cycling is split by add_straw_pool
               ,biomass_to_be_distributed->chaff_or_leaf_litter
               #ifdef NITROGEN
               ,crop_surface_residue_N_conc                                      //060219LML_140908 added macro
               #endif
               ,*residue_decomp_params);                                         //070625
            biomass_to_be_distributed->chaff_or_leaf_litter = 0;
         } // if chaff_or_leaf_litter
         if (biomass_to_be_distributed->stubble_dead > 0.0000001)
         {  local_residue_pools_kg_m2->add_surface_plant_pool(OM_stubble_surface_position /* false laying flat, not standing */        //060219
               ,OM_ORIGINAL_CYCLING // In V4.3 the cycling is split by add_straw_pool
               ,biomass_to_be_distributed->stubble_dead
               #ifdef NITROGEN
               ,crop_surface_residue_N_conc                                      //060219 LML 140908 added macro
               #endif
               ,*residue_decomp_params );                                        //070625
            biomass_to_be_distributed->stubble_dead = 0;
         } //if stubble_dead
         if (biomass_to_be_distributed->get_dead_roots_residue() > 0.0000001)    //061215
         {  float64 root_N_conc = crop_N_kg_m2 ? crop_N_kg_m2->get_root_concentration(include_vital) : 0.0;
            local_residue_pools_kg_m2->add_subsurface_plant_pool
               (biomass_to_be_distributed->roots_dead
               #ifdef NITROGEN
               ,root_N_conc                                                      //060219 LML 140908 added macro
               #endif
               ,*residue_decomp_params);                                         //070625
            clear_sublayer_array(biomass_to_be_distributed->roots_dead);
         } // if dead_roots_residue
      }
      #endif
      delete crop_mass_fate_to_be_distributed;                                   //070627
      crop_mass_fate_to_be_distributed = 0;
   }
   return true;
}
//_distribute_any_crop_biomass_fate_________________________________2004-06-24_/
#endif
bool Land_unit_simulation_VX::end_year()                           modification_
{  bool ended = true;
   #ifndef __linux__
   std::cout << '\r' << simdate.get_year();                                      //130620_120524
   #endif
   #ifdef EROSION
   if (soil->erosion)                                                            //020529
   {
      soil->erosion->end_year();                                                 //160506
      if (scenario_control_and_model_options.erode_soil)
      {  // The option to actually remove eroded soil depth
         soil->erode();
         Residues_interface *local_residue_pools_kg_m2                           //060115
            = organic_matter_residues_profile_kg_m2
            ? organic_matter_residues_profile_kg_m2->mod_residues() : 0;
         if (local_residue_pools_kg_m2)                                          //060628
            local_residue_pools_kg_m2->identify_residue_horizons();              //060122
      }
   }
   #endif
   ended &= CS::Simulation_unit::end_year();                                     //151203
   return ended;                                                                 //111031
}
//_end_year_________________________________________________________2001-02-02_/
bool Land_unit_simulation_VX::start_year()                         modification_
{
   bool started = CS::Simulation_unit::start_year();                             //151203
   float64 water_depth_profile =
      soil ?                                                                     //180106
      soil->ref_hydrology()->provide_water_depth_profile(true)                   //160412_140918
      : 0.0;                                                                     //180106
   delete ANNL_accumulators_active;                                              //070227
   ANNL_accumulators_active = new Annual_accumulators(*this,water_depth_profile,0.0); //050114
   ANNL_accumulators_active->init_total_water_content = water_depth_profile;     //050114
   #ifdef EROSION
   if (soil && soil->erosion) started &= soil->erosion->start_year();            //180106_151203_131010
   #endif
   if (organic_matter_residues_profile_kg_m2)
      started &= organic_matter_residues_profile_kg_m2->start_year();            //151203_070510
   return started;                                                               //041014
}
//_start_year_______________________________________________________2001-02-02_/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
bool Land_unit_simulation::end_year()                              modification_
{  bool ended = true;                                                            //151203
   #ifdef LADSS_MODE
      if (LADSS_annual_database_table)
      {  LADSS_annual_output_data_record local_annual_record(LBF_ID,year,*this);
         LADSS_annual_database_table->set(local_annual_record);
      }
   #endif
   ended &= Land_unit_simulation_VX::end_year();                                 //151203
   return ended;
}
#endif
//_end_year_________________________________________________________2011-11-08_/
bool Land_unit_simulation::start_year()                            modification_
{  return Land_unit_simulation_VX::start_year();
}
//_start_year_______________________________________________________2011-10-28_/
void Land_unit_simulation_VX::update_accumulators()
{  const Residues_interface *local_residue_pools_kg_m2                           //060115
      = organic_matter_residues_profile_kg_m2
      ? organic_matter_residues_profile_kg_m2->ref_residues() : 0;
   float64 soil_evap_act   = (soil && soil->mod_evaporator())
      ? soil->mod_evaporator()->get_evaporation_actual_day() : 0;
   float64 pond_evap_act   = local_residue_pools_kg_m2
      ? local_residue_pools_kg_m2->get_act_evaporation() : 0.0;
   float64 crop_transp_act = crop_active_or_intercrop
      ?crop_active_or_intercrop->ref_reported().daily.transpiration_act_m : 0.0; //190705
   output_act_crop_evapotransp_m   // rename to output_crop_ET_act_m
            = soil_evap_act
            + pond_evap_act
            + crop_transp_act;                                                   //130716_000323
   ANNL_accumulators_active->accumulate();                                       //190709
   if (GP_accumulators_active)                                                   //990414
       GP_accumulators_active->accumulate();                                     //190709
   #if defined(CROPSYST_PROPER)
   CONTINUOUS_accumulators_active->accumulate();                                 //190709
   #endif
   #if (CROPSYST_PROPER==4)
   if (FALLOW_accumulators_active)                                               //990312
       FALLOW_accumulators_active->accumulate();                                 //190709
   #endif
   if (soil)                                                                     //180106
   {
   float64 curr_soil_water_content_profile =
      soil->ref_hydrology()->provide_water_depth_profile(true);                  //160412_130930
   #ifdef SOIL_INFILTRATION
   float64 infiltration_submodel_daily_error = soil->infiltration_submodel->get_daily_error(); //071024
   float64 infiltration_deferred = soil->infiltration_submodel->get_deferred();  //071024
   if (GP_accumulators_active)                                                   //990407
      GP_accumulators_active->compute_water_balance
      ( curr_soil_water_content_profile                                          //990316
      , infiltration_submodel_daily_error                                        //030710
      , infiltration_deferred                                                    //071024
      , false,false);                                                            //000626
   #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
   if (CONTINUOUS_accumulators_active)                                           //990407
      CONTINUOUS_accumulators_active->compute_water_balance
      ( curr_soil_water_content_profile                                          //990316
      , soil->infiltration_submodel->get_daily_error()                           //030710
      , infiltration_deferred                                                    //071024
      , true                                                                     //000626
      , (infiltration_model != FINITE_DIFFERENCE_INFILTRATION));                 //190707_051115
         // Actually I only want to disable if this is a batch run.
         // but quick and dirty I just need to disable for F.D.
    #endif
   #if (CROPSYST_PROPER==4)
   if (FALLOW_accumulators_active)                                               //990407
      FALLOW_accumulators_active->compute_water_balance
      ( curr_soil_water_content_profile                                          //990602
      , infiltration_submodel_daily_error                                        //030710
      , infiltration_deferred                                                    //071024
      , false,false);                                                            //000626
   #endif
   if (ANNL_accumulators_active)                                                 //990407
      ANNL_accumulators_active->compute_water_balance
      ( curr_soil_water_content_profile                                          //990316
      , infiltration_submodel_daily_error                                        //030710
      , infiltration_deferred                                                    //071024
      , false,false);                                                            //000626
   #endif
   #ifdef NITROGEN

   // 181031 I am not sure why this is here, this is already updated in
   // chemical_transport_and_exchange, however perhap
   // chemical_transport_and_exchange is not called (in some submodel selection)

   if (soil->chemicals && soil->chemicals->NO3 && soil->chemicals->NH4)
   {
      soil->chemicals->total_N_E[LAYERS_SUM_INDEX]      = 0.0;                   //131001
      soil->chemicals->NO3_N_E_output[LAYERS_SUM_INDEX] = 0.0;                   //131001
      soil->chemicals->NH4_N_E_output[LAYERS_SUM_INDEX] = 0.0;                   //131001
       for (nat8 layer = 1; layer <= soil->ref_layers()->count(); layer++)       //160412
       {
         soil->chemicals->total_N_E[layer]
              = soil->chemicals->NO3->molecular_to_elemental * (soil->chemicals->NO3->mass_M->get_layer(layer))
              + soil->chemicals->NH4->molecular_to_elemental * (soil->chemicals->NH4->mass_M->get_layer(layer));
         soil->chemicals->total_N_E[LAYERS_SUM_INDEX]      += soil->chemicals->total_N_E[layer];      //131001
         soil->chemicals->NO3_N_E_output[LAYERS_SUM_INDEX] += soil->chemicals->NO3_N_E_output[layer]; //131001
         soil->chemicals->NH4_N_E_output[LAYERS_SUM_INDEX] += soil->chemicals->NH4_N_E_output[layer]; //131001
       } // for
   } // if chemicals
   #endif
   }
}
//_update_accumulators______________________________________________2001-02-08_/
void Land_unit_simulation_VX::load_profile_recal_events(const char *filename )
{  std::ifstream recal_file(filename);
   while (recal_file.good() && !recal_file.eof())
   {  Year  year;
      DOY   doy;
      recal_file >>  year >> doy;
      if (year && doy) // Make sure the line is reading OK
      {CORN::Date_clad_32 recal_date(year,doy);
       CropSyst::Recalibration_operation *recalib_op
         = new CropSyst::Recalibration_operation(recal_file);
       scenario_event_list.append(new Common_event(recal_date,recalib_op,true)); //131020_130827_021105
      } //if
   } //while
}
//_load_profile_recal_events________________________________________2002-03-09_/
void Land_unit_simulation_VX::load_events_for_simulation(const CORN::OS::File_name  &mgt_filename)
{  Management special_management;
   if (CORN::OS::file_system_engine.exists(mgt_filename))                        //150416_031203
   {  VV_File special_management_file(mgt_filename.c_str());                     //031203
      special_management_file.get(special_management);                           //010109
      if (!special_management.crop_management)                                   //170201
      {  // The special management is intended to not associated with crop
      Common_event_list specific_events;                                         //030614
      special_management.transfer_all(specific_events,true
         ,ANY_SYNC_MODE,mgt_filename.c_str());                                   //031010
         // these are not associated with a crop
      Synchronization act_date_sync(ACTUAL_DATE_MODE);                           //030614
      specific_events.synchronize(ACTUAL_DATE_MODE,CORN::Date_clad_32());        //031010
         // this should cause actual date syncs to be set in the events
      Synchronization relative_to_sim_begin_sync(SIMULATION_BEGIN_DATE);         //060906
      Synchronization relative_to_sim_end_sync(SIMULATION_END_DATE);             //060906
         // this should cause syncs reletive to the simulatin starting period to be set in the events
      specific_events.synchronize
         (relative_to_sim_begin_sync,scenario_control_and_model_options.start_date);//060906
      specific_events.synchronize
         (relative_to_sim_end_sync,scenario_control_and_model_options.stop_date);//060906
      scenario_event_list.transfer_all_from(specific_events);                    //131020_130827_030614
      }
   }
}
//_load_events_for_simulation_______________________________________1999-06-16_/
CORN::date32 Land_unit_simulation_VX::get_today_date32()                   const
{ return simdate.get_date32(); }
//_get_today_date32_________________________________________________3003-07-14_/
/*200404 obsolete now using is_at_season_end
Crop_model_interface *Land_unit_simulation_VX::get_crop_at_end_of_season() const
// dont' return const crop because we may adjust the biomatter fate              //131008
{ return (crop_at_end_seasonX);}
//_get_crop_at_end_of_seasonX_______________________________________2007-02-27_/
*/
/*200404 obsolete now using is_at_season_end
Crop_model_interface *Land_unit_simulation_VX::get_crop_active_or_at_end_of_season() const
{  return crop_at_end_seasonX ? crop_at_end_seasonX : crop_active_or_intercrop;}
*/
//_get_crop_active_or_at_end_of_season______________________________2007-02-29_/
const CropSyst::Soil_components *Land_unit_simulation_VX::ref_soil()       const
{  return (const CropSyst::Soil_components *)soil;
}  // NYI may be able to return an interface now, need to check
//_ref_soil_________________________________________________________2006-08-03_/
CropSyst::Soil_components *Land_unit_simulation_VX::mod_soil()             const
{  return (CropSyst::Soil_components *)soil;
}  // NYI may be able to return an interface now, need to check
//_mod_soil_________________________________________________________2006-08-03_/
Soil::Chemicals_profile  *Land_unit_simulation_VX::get_soil_chemicals()    const
{
   #if (!((CROPSYST_VERSION >=5 ) || defined(USE_CS_LAND_UNIT)))
   CropSyst::Soil_components *soil = mod_soil();                                 //181225_161008
   #endif
   Soil::Chemicals_profile  *got_chemicals = soil ? soil->chemicals : 0;
   return got_chemicals;
}
//_get_soil_chemicals_______________________________________________2006-08-03_/
Dynamic_water_entering_soil *Land_unit_simulation_VX::create_irrigation_in_intervals_mm
(Minutes water_entering_soil_timestep_minutes)                             const
{  Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm = 0;   //070107
   if (total_irrigation_today > 0.0)                                             //000413
   {  // (Split it into intervals consistent with precip intervals added it to water entering in time step control
      irrigation_water_entering_in_intervals_mm
               = new Dynamic_water_entering_soil((nat8)24);                      //160511
      nat8 irrigation_start_interval = 0;
      nat16 irrigation_duration_intervals = 1;
      switch (water_entering_soil_timestep_minutes)                              //070107
      {  case 60 :
            irrigation_duration_intervals = management_irrigation_duration_hours;//070107
            irrigation_start_interval = earliest_irrigation_start_hour;          //081212
            if ((irrigation_start_interval + irrigation_duration_intervals) >= 24)
               irrigation_start_interval = 0; // Just start at the beginning of the day (midnight)
         break;
         case 30 :
           irrigation_duration_intervals=management_irrigation_duration_hours*2; //070107
           irrigation_start_interval = earliest_irrigation_start_hour * 2;       //081212
            if ((irrigation_start_interval+irrigation_duration_intervals) >= 48)
               irrigation_start_interval = 0; // Just start at the beginning of the day (midnight)
            default : irrigation_duration_intervals = 1; break;                  //070107
      } // switch
      // We need to increment because there could still be precipitation.          000413
      float32 interval_irrig_mm
         = m_to_mm(total_irrigation_today) / irrigation_duration_intervals;      //000413
      for (int i = 0; i < irrigation_duration_intervals; i++)                    //000413
         irrigation_water_entering_in_intervals_mm->set
            (i+irrigation_start_interval,interval_irrig_mm);                     //000413
   } // if irrgiation today
   return irrigation_water_entering_in_intervals_mm;                             //070107
}
//_create_irrigation_in_intervals_mm________________________________2007-01-19_/
#ifdef AUTOMATIC_IRRIGATION
bool Land_unit_simulation_VX::process_any_automatic_irrigation()   modification_
{  bool irrigation_added = false;
   if (curr_management)
   {  if (curr_automatic_irrigation_mode
          && !fixed_irrigation_scheduled_today)                                  //090722
      {  bool apply_irrigation = false;
         if (curr_automatic_irrigation_mode->consideration_mode == consider_leaf_water_potential) //120524
         {
            Crop_model_interface *crop_active =                                  //190617
               #ifdef INTERCROPPING
               get_predominant_crop();                                           //130717
               // currently the LWP mode is only applicable to single crop but
               // this will work for the predominant crop in intercropping
               #else
               crop_active_or_intercrop;                                         //130719
               #endif
            if (crop_active)                                                                             //120524
            {  // actually there should always active crop if we have management
               float32 crop_LWP =  crop_active->get_leaf_water_pot();                                    //120524
               if (crop_LWP <= /* more negative */ curr_automatic_irrigation_mode->leaf_water_potential) //091208
                  apply_irrigation = true;                                                               //091208
                // Note, probably could do soil recharge but currently only setup for a fixed amount.    //091208
            }
         }                                                                                               //091208
         else // consider_soil_water_depletion                                                           //091208
         {  // Note eventually just call process_irrigation but we need to do the following
            // 1. Set the amount in the operation before calling process_irrigation
            // 2. in process irrigation, we need to soil->set_daily_rrig_salinity
         float64 depletion_observe_depth = curr_automatic_irrigation_mode->get_depletion_observe_depth(); //060719
         switch (curr_automatic_irrigation_mode->get_depletion_observation_depth_mode())                  //070606
         {  case  constant_fixed_depth_mode :                                    //070606
            {   depletion_observe_depth = curr_automatic_irrigation_mode->get_depletion_observe_depth();  //070606
            } break;                                                             //070606
            case  fraction_of_root_depth_mode :                                  //070606
            {   if (crop_active_or_intercrop)                                    //060719
                  depletion_observe_depth = crop_active_or_intercrop->get_recorded_root_depth_m();  //060719
            } break;                                                             //070606
            case  soil_profile_depth_mode :                                      //070606
            {  const Soil::Layers_interface *layers = soil->ref_layers();        //160412_070606
               depletion_observe_depth = layers->get_depth_profile_m() ;         //070606
            } break;                                                             //070606
            default:
            {  depletion_observe_depth = curr_automatic_irrigation_mode
                  ->get_depletion_observe_depth();                               //070606
            } break;
            // This case and default is already set above
         }
         if (soil->check_automatic_irrigation
                  (curr_automatic_irrigation_mode->get_max_allowable_depletion()
                  ,depletion_observe_depth ))
            apply_irrigation = true;                                             //091208
         }
         management_irrigation_duration_hours
            = curr_automatic_irrigation_mode->get_duration_hours();              //070109
         earliest_irrigation_start_hour
            = curr_automatic_irrigation_mode->get_start_hour();                  //090722
         if (apply_irrigation)                                                   //091208
         {                                                                       //051120
            switch (curr_automatic_irrigation_mode->application_mode)            //091209
            {  case irrigate_soil_condition :                                    //091209
               {  automatic_irr_amount =                                         //091209
                     soil->determine_recharge_requirements
                     (curr_automatic_irrigation_mode->get_net_irrigation_mult()
                     ,crop_active_or_intercrop
                        ? crop_active_or_intercrop->get_root_length_m()
                        : 0.0
                     ,curr_automatic_irrigation_mode->get_refill_point());
                  float64 auto_irrig_max_application =  curr_automatic_irrigation_mode->get_max_application_m();
                     if (auto_irrig_max_application > 0.0)
                        automatic_irr_amount = CORN::must_be_less_or_equal_to
                        <float64>(automatic_irr_amount,auto_irrig_max_application);
                     if (automatic_irr_amount < curr_automatic_irrigation_mode->get_min_application_m())          //060719
                        automatic_irr_amount = 0; // The irration system cannot not deliver this minimal amount   //060719
               } break;
               case irrigate_fixed_amount :                                      //091209
               {  automatic_irr_amount = mm_to_m(curr_automatic_irrigation_mode->get_amount_mm());
               } break;
            } // switch application mode
            #ifdef SALINITY
            Soil::Salt_interface *soil_salinity = soil->mod_salinity();          //160412
            if (soil_salinity) soil_salinity->set_daily_irrig_salinity           //051120
                  (curr_automatic_irrigation_mode->get_salinity()
                  ,curr_automatic_irrigation_mode->get_ECw_to_total_dissolved_solids() //080402
                     // Typically 0.64 (used to be a constant)
                  ,automatic_irr_amount
                  ,curr_automatic_irrigation_mode->start_hour                    //081212
                  ,curr_automatic_irrigation_mode->duration_hours                //081212
                  );  // 081212 Note that we may want to all the start and duration in the automatic event
            #endif
            irrigation_added = true;                                             //071017
         } // if apply irrigation
         if (irrig_appl_target != 0.0) automatic_irr_amount = 0;
         /*debug
         if (automatic_irr_amount > 0.0)
            log_automatic_irr_event(simdate,automatic_irr_amount);
         */
         total_irrigation_today += automatic_irr_amount;
      }
   }
   return irrigation_added; // note that false is not an error
}
#endif
//_process_any_automatic_irrigation_________________________________2007-10-17_/
bool Land_unit_simulation_VX::parameterize_organic_matter
(const char *initial_or_equilibrated_organic_matter_file_name_unqual_cstr) const
{
   if (organic_matter_residues_profile_kg_m2)                                    //150921
   {

   //150916 Roger need fix it!!!!
/*
This is broken both for OFoot and Microbasin.
scenario directory is not setup.
(provide it?)
*/

#ifndef MICROBASIN_VERSION
   CORN::OS::File_name_concrete OM_filename
      (get_output_directory_name()
      ,initial_or_equilibrated_organic_matter_file_name_unqual_cstr);
   std::ofstream OM_stream(OM_filename.c_str());
   organic_matter_residues_profile_kg_m2->write(OM_stream);
#endif
   }
   return true;
}
//_parameterize_organic_matter______________________________________2012-09-09_/
#ifdef INTERCROPPING
Crop_interfaced *Land_unit_simulation_VX::get_predominant_crop()
{  return crop_active_or_intercrop?crop_active_or_intercrop->get_predominant_crop():0;
}
//_get_predominant_crop_____________________________________________2013-07-16_/
Crop_interfaced *Land_unit_simulation_VX::get_secondary_crop()
{  return crop_active_or_intercrop?crop_active_or_intercrop->get_secondary_crop():0;
}
//_get_secondary_crop_______________________________________________2013-07-17_/
Crop_interfaced *Land_unit_simulation_VX::get_intercrop(nat8 crop_index)
{  return
      crop_active_or_intercrop
      ?  (crop_index == 0)
         ? crop_active_or_intercrop->get_predominant_crop()
         :  crop_active_or_intercrop->get_secondary_crop()
      : 0;
}
//_get_intercrop____________________________________________________2013-07-17_/
#endif
#ifdef OBSOLETE
200404 we already handle ending operation period in end season
#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
nat32 Land_unit_simulation_VX::perform_triggered_synchronizations_today()
{  nat16 syncs = 0;
   #ifdef INTERCROPPING
   for (int intercrop_index = 0; intercrop_index < 2; intercrop_index++)
   {  Crop_interfaced *crop_active_or_intercrop = get_intercrop(intercrop_index);
      // In INTERCROPPING mode, ths crop_active_or_intercrop is local to the loop
   #endif
      if (crop_active_or_intercrop)
      {
         // Crop will maintain its own scheduler
         bool terminate_crop = crop_active_or_intercrop
            && crop_active_or_intercrop->is_terminate_pending();
         if (terminate_crop)
         {
            #ifdef DISABLED
            // Warning disabled, not sure where these are in this context
            if ( curr_automatic_clip_mode)
               Common_simulation::end_operation_period(*( curr_automatic_clip_mode), today);
            if ( curr_automatic_irrigation_mode)
               Common_simulation::end_operation_period(*( curr_automatic_irrigation_mode), today);
            #endif
            #ifdef MANAGEMENT
            if (curr_automatic_irrigation_mode)curr_automatic_irrigation_mode=0; //140529
            if (curr_automatic_clip_mode)      curr_automatic_clip_mode = 0;     //140529
            #endif
            // Crop now maintains its own event schedling
         }
         //200404 perform_crop_synchronizations(*crop_active_or_intercrop);               //130903
      }
   #ifdef INTERCROPPING
   }
   #endif
   return syncs;
}
//_perform_triggered_synchronizations_today_________________________2013-10-04_/
#endif
#endif
// for PHENOLOGY_2018 termiation is handled in end_day()
const Slope_saturated_vapor_pressure_plus_psychrometric_resistance
*Land_unit_simulation_VX::
 get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference() const
{
   #ifdef VIC_CROPSYST_VERSION
   return NULL; // VIC::Land_unit_simulation overrides this method
   #else
   #if (CROPSYST_PROPER==4)
   if (!slope_saturated_vapor_pressure_plus_psychrometric_resistance)            //180805
   {  const CropSyst_ET *ET_ref_as_CropSyst_ET =                                 //180805
         dynamic_cast<const CropSyst_ET *>(ET_ref);                              //180805
      if (ET_ref_as_CropSyst_ET)                                                 //180805
         slope_saturated_vapor_pressure_plus_psychrometric_resistance            //180805
         = const_cast<Slope_saturated_vapor_pressure_plus_psychrometric_resistance *>  //180805
          (&(ET_ref_as_CropSyst_ET->                                             //180805
               slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference)); //180805
      else                                                                       //180805
      {  const Weather_provider &weather = meteorology.ref_weather_provider();   //190705
         slope_saturated_vapor_pressure_plus_psychrometric_resistance = new      //180805
            Slope_saturated_vapor_pressure_plus_psychrometric_resistance         //180805
               (weather.vapor_pressure_saturation_slope                          //190705_180805
               ,weather.psychrometric_constant                                   //190705_180805
               ,meteorology.ref_aerodynamics()                                   //180805
               ,resistance_of_plants_d_m);                                       //180805
      }
   }
   return slope_saturated_vapor_pressure_plus_psychrometric_resistance;         //180805
   #else
   return &meteorology.ET_ref->ref_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference();
   #endif
   #endif
}
//_get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference 2015-06-02_/
bool Land_unit_simulation_VX::get_active_crop_name(modifiable_ std::wstring &active_crop_name)  const
{  bool in_active_crop = false;
   active_crop_name.clear();
   if (crop_active_or_intercrop)
   {  in_active_crop = crop_active_or_intercrop->get_name(active_crop_name);

      // sets current_crop_name to the name of the currently growing crop.
      // (its unqualified file name without extension).
      // Returns false if the land unit is in fallow (no crop).
      // This was added for VIC but is of general use
      // Note that the crop file name could be a symbolic link (in VIC)
      // this will be dereferenced.
   }
   return in_active_crop;
}
//_get_active_crop_name_____________________________________________2015-07-20_/
#ifdef VIC_CROPSYST_VERSION
bool Land_unit_simulation_VX::is_current_crop_fruit_tree()          affirmation_ //150720
{  return crop_active_or_intercrop
      ? crop_active_or_intercrop->is_fruit() //190707 is_orchard()
      : false;
}
//_is_current_crop_fruit_tree_______________________________________2015-07-20_/
#endif
bool Land_unit_simulation_VX::is_key_string(const std::string &key) affirmation_ //180820
{  return ID.code == key; }
bool Land_unit_simulation_VX::is_key_nat32(nat32 key)               affirmation_ //180820
{  return ID.number == key;
}
//_is_key_string________________________________________________________________
nat32 Land_unit_simulation_VX::get_key_nat32()                             const
{  return ID.number; }                                                           //151026
//_get_key_nat32_________________________________________________2015-09-17RLN_/
const CORN::OS::Directory_name &Land_unit_simulation_VX
::get_output_directory_name()                                              const
{return *(scenario_directory->output_dir);}
//_get_output_directory_name________________________________________2015-11-28_/
bool Land_unit_simulation_VX::setup_initial_soil_profile_recalibrations()
{  // 160303 (this used to be in CS_mod.cpp
   // now allow recalibration to initial soil profile values

   // Note that these recalibration events could actually be common
   // rotated scenario (in the case of rotation cycling).

   for (Year year =  scenario_control_and_model_options.start_date.get_year()
       ;     year <= scenario_control_and_model_options.stop_date .get_year()
       ;     year++)
   {
      if (scenario_control_and_model_options.H2O_recalibrate_date.get_date32())  //130828
      {  CORN::Date_clad_32 act_recal_date(year,scenario_control_and_model_options.H2O_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::WC_RECAL,scenario_control_and_model_options.horizon_H2O);   //020310
         scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      #ifdef NITROGEN
      if (scenario_control_and_model_options.N_NO3_recalibrate_date.get_date32())       //130828
      {  CORN::Date_clad_32 act_recal_date(year,scenario_control_and_model_options.N_NO3_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::NO3_RECAL,scenario_control_and_model_options.horizon_N_NO3);   //020310
         scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      if (scenario_control_and_model_options.N_NH4_recalibrate_date.get_date32())
      {  CORN::Date_clad_32 act_recal_date(year,scenario_control_and_model_options.N_NH4_recalibrate_date.get_DOY()); //130828
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation(CropSyst::Recalibration_operation::NH4_RECAL,scenario_control_and_model_options.horizon_N_NH4);   //020310
         scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      #endif
      #ifdef PHOSPHORUS
      /*110801 NYI
      if (land_unit_with_output.P_recalibrate_date.get())
      {  CORN::Date act_recal_date(year,land_unit_with_output.P_recalibrate_date.get_DOY());
         Recalibration_operation *recalib_op = new Recalibration_operation(Recalibration_operation::P_RECAL,land_unit_with_output.horizon_P);   //020310
         land_unit_with_output.event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      */
      #endif
      if (scenario_control_and_model_options.POM_recalibrate_date.get_date32())
      {  CORN::Date_clad_32 act_recal_date(year,scenario_control_and_model_options.POM_recalibrate_date.get_DOY());
         CropSyst::Recalibration_operation *recalib_op = new CropSyst::Recalibration_operation
            (CropSyst::Recalibration_operation::POM_RECAL
            ,scenario_control_and_model_options.horizon_percent_organic_matter); //020310
         scenario_event_list.append(new Common_event(act_recal_date,recalib_op,true));
      }
      // Eventually we can add salinity
   }
   return true;
}
//_setup_initial_soil_profile_recalibrations________________________2016-03-03_/
bool Land_unit_simulation_VX::evaporate_residues()                 modification_
{
   #ifdef RESIDUES
   if (organic_matter_residues_profile_kg_m2)
   {
      Residues_interface *local_residue_pools_kg_m2                              //060115
         = organic_matter_residues_profile_kg_m2->mod_residues();
      if (local_residue_pools_kg_m2)                                             //060628
          local_residue_pools_kg_m2->evaporate_day();
   }
   #endif
   return true;
}
//_evaporate_residues_______________________________________________2016-04-08_/
bool Land_unit_simulation_VX::evaporate_pond()                     modification_
{
   #ifdef VIC_CROPSYST_VERSION
   // VIC handles evaporation
   #else
   if (pond.is_daily_enabled())
       pond.evaporate_day();                                                     //160606
   #endif
   return true;
}
//_evaporate_pond___________________________________________________2016-04-08_/
bool Land_unit_simulation_VX::evaporate_soil()                     modification_
{  bool started = true;
   #ifdef VIC_CROPSYST_VERSION
   // VIC handles evaporation
   #else
   if (soil)                                                                     //180106
   {
      if (soil->ref_evaporator()->is_daily_enabled())                            //160606
          soil->mod_evaporator()->evaporate_day();                               //160606
   }
   #endif
   return started;
}
//_evaporate_soil___________________________________________________2016-04-08_/
bool Land_unit_simulation_VX::evaporate()                          modification_
{  if (has_any_snow) return true;
      // snow cover effectively prevents evaporation
      // sublimation is not currently handled here.
   return evaporate_residues()
       && evaporate_pond()
       && evaporate_soil();
}
//_evaporate________________________________________________________2016-04-08_/
float64 Land_unit_simulation_VX::evaporate_pond_hour(Hour hour)    modification_
{
   float64 evaporated = 0;
   #ifdef VIC_CROPSYST_VERSION
      // VIC handles evaporation                                                 //160629LML
   #else
   evaporated = pond.evaporate_hour(hour);
   #endif
   return evaporated;
}
//_evaporate_pond_hour______________________________________________2016-05-05_/
#ifdef RESIDUES
float64 Land_unit_simulation_VX::evaporate_residues_hour(Hour hour) modification_//160408
{  float64 evaporated = 0;

   if (organic_matter_residues_profile_kg_m2)
   {
      Residues_interface *local_residue_pools_kg_m2                              //060115
         = organic_matter_residues_profile_kg_m2->mod_residues();
      if (local_residue_pools_kg_m2)                                             //060628
         evaporated = local_residue_pools_kg_m2->evaporate_hour(hour);
   }
   return evaporated;
}
#endif
//_evaporate_residues_hour__________________________________________2016-05-05_/
float64 Land_unit_simulation_VX::evaporate_soil_hour(Hour hour)    modification_ //160408
{
   float64 evaporated = 0;
   evaporated = soil->mod_evaporator()->evaporate_hour(hour);                    //160606

   // Note that evaporation is handled by the infiltration models.
   // The hourly evaporation is implemented by the the Evaporator class
   // which simply distributes the daily evaporation into hourly intervals
   // based on fraction of solar irradiance for the Cascade models.
   // For the Finite difference model, implements its own hourly model
   // (Which appears to be actually no different from the Evaporator class implemention.
   //  eventually the F.D. evaporation simply inherit the Evaporator's hourly implementation.
   //  but I need to check this carefully.)
   return evaporated;
}
//_evaporate_soil_hour______________________________________________2016-05-05_/
bool Land_unit_simulation_VX::know_soil_parameters
(Smart_soil_parameters *parameters_known)                             cognition_
{  soil_parameters = parameters_known;
   soil_parameters_known = true;
   return true;
}
//_know_soil_parameters_____________________________________________2016-09-16_/
const Smart_soil_parameters *Land_unit_simulation_VX
::provide_soil_parameters()                                           provision_
{
   if (!soil_parameters)
   {
      soil_parameters = new Smart_soil_parameters;
      VV_File soil_datasrc(scenario_control_and_model_options.soil_filename.c_str());
      bool loaded_OK = soil_datasrc.get(*soil_parameters);
      soil_parameters_known = false; // because we have instanciated
      if (loaded_OK)
      {
         soil_parameters->limit_profile_depth                                    //170609
            (scenario_control_and_model_options.soil_profile_depth_limit);       //170609
      } else
      // Perhaps using file discovery and composition                            //160126
      {  delete soil_parameters;  // failed to load so clean up                  //090605
         cerr << TL_Unable_to_open_file << ':' << TL_Soil_file  << " ("
           << scenario_control_and_model_options.soil_filename.c_str() << ')'
           << endl;                                                              //130827
      }
   }
   return soil_parameters;
}
//_provide_soil_parameters__________________________________________2016-09-16_/
#define TS_VALUE value_statistic
RENDER_INSPECTORS_DEFINITION(Land_unit_simulation_VX)
{
   #ifdef CS_OBSERVATION
/*At this time I am not using instance description because
 * in MicroBasin it generates a inspector label for every cell.
 *
 * I should add an optional disambituation label to inspector
 * (Which would also be used by Crop
 *
   if (!instance_description)
      instance_description = get_key();
*/
   char cycle_cstr[5];
   CORN::nat8_to_cstr(cycle_shift,cycle_cstr,10,1,0);
   std::string new_instance(cycle_cstr);
   CS::Emanator &emanator  = CS::get_emanators()                                 //171115_160612
      .know((nat32)this,new_instance,get_ontology(),false,context,simdate);      //170317
   emanator_known = &emanator;                                                   //160614
   if (runoff) // Eventually move this to runoff
      inspectors.append(new CS::Inspector_scalar(runoff->ref_estimated_runoff()                    ,UC_m       ,emanator,"runoff/surface_water"    ,TS_VALUE,CSVP_soil_runoff_surface_water_runoff));
      // WARNING  multiple entries are showing up in UED file with different values
   inspectors.append(new CS::Inspector_scalar(nitrogen_daily_output.volatilization_loss_NH3   ,UC_kg_m2   ,emanator,"volatilization/N_NH3"    ,TS_VALUE,CSVP_soil_N_NH3_volatilization/*171222 CSVC_soil_N_NH3_volatilization_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(nitrogen_daily_output.volatilization_total      ,UC_kg_m2   ,emanator,"volatilization/N_total"  ,TS_VALUE,CSVP_soil_N_volatilization_total/*171222 CSVC_soil_N_volatilization_total_time_step_sum*/));
   inspectors.append(new CS::Inspector_scalar(nitrogen_daily_output.applied_total             ,UC_kg_m2   ,emanator,"application/N_total"     ,TS_VALUE,CSVP_mgmt_N_applied /*17122 CSVC_mgmt_N_applied_time_step_sum*/));

//NYI CSVP_soil_C_all_surface
   inspectors.append(new CS::Inspector_scalar(profile_carbon_output.sum_profile  ,UC_kg_m2   ,emanator,"OM:C/profile" ,sum_statistic,CSVP_soil_C_all_profile/*171222 CSVC_soil_C_all_profile*/));
//NYI inspectors.append(new CS::Inspector_scalar(XXXXX                           ,UC_kg_m2   ,emanator,//PP_C_5cm      ,UC_kg_ha,inspectors.&(STAT_sum_bit|STAT_value_bit),TS_VALUE));  // WARNING verify statistics to use
//NYI inspectors.append(new CS::Inspector_scalar(XXXXX                           ,UC_kg_m2   ,emanator,//PP_C_10cm     ,UC_kg_ha,inspectors.&(STAT_sum_bit|STAT_value_bit),TS_VALUE));  // WARNING verify statistics to use
//NYI inspectors.append(new CS::Inspector_scalar(XXXXX                           ,UC_kg_m2   ,emanator,/PP_C_15cm     ,UC_kg_ha,inspectors.&(STAT_sum_bit|STAT_value_bit),TS_VALUE));  // WARNING verify statistics to use
   inspectors.append(new CS::Inspector_scalar(profile_carbon_output.sum_30cm     ,UC_kg_m2   ,emanator,/*check ontology*/"OM:C/30cm"    ,sum_statistic,CSVP_soil_C_all_30cm/*171222 CSVC_soil_C_all_30cm*/));
//NYI CSVP_soil_C_all_60cm
//NYI CSVP_soil_C_all_90cm

//NYI CSVP_mgmt_N_organic_applied
//NYI CSVP_mgmt_N_inorganic_applied

#ifdef REDOTHIS
180416
   // Reference ET
   #if ((CROPSYST_PROPER == 4) && !defined(VIC_CROPSYST_VERSION))
   inspectors.append(new CS::Inspector_scalar(

       ET_ref->ref_applicable_mm()
      ,UC_mm   ,emanator,/*check ontology*/"biometeorology:ET/potential/reference"    ,TS_VALUE,CSVP_weather_ET_ref ));
   #else
   //160731 NYI
   #endif

#endif

   // These are ET adjusted for actual crop cover, moving to Crop
   inspectors.append(new CS::Inspector_scalar(output_act_crop_evapotransp_m,UC_m,emanator,/*check ontology*/"crop:ET/act",TS_VALUE,CSVP_weather_ET_act));
   inspectors.append(new CS::Inspector_scalar(output_crop_evapotransp_max_m,UC_m,emanator,/*check ontology*/"crop:ET/max",TS_VALUE,CSVP_weather_ET_max));

   // Management

   inspectors.append(new CS::Inspector_scalar(total_irrigation_today   ,UC_m    ,emanator,/*check ontology*/"management:irrigation/applied"    ,TS_VALUE,CSVP_mgmt_irrig_applied));
   /*
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_mgmt_biomass_grazed));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_mgmt_biomass_unsatisfied));
   */

   // Nitrogen management
   /* NYI
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_mgmt_P_applied));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_mgmt_N_organic_gaseous_loss));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_mgmt_N_inorganic_gaseous_loss));
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_misc_N_total_mineralization));
   */

   // Water table
   /*NYI
   inspectors.append(new CS::Inspector_scalar(member_variable   ,UC_xxxxx   ,emanator,"XXX/XXX"    ,XXX_statistic,CSVP_water_table_depth));
   */

   // eventually move to pond
   //NYI inspectors.append(new CS::Inspector_scalar(pond.xxx   ,UC_m   ,emanator,/*check ontology*/"pond"    ,TS_VALUE,CSVP_soil_pond_water_depth));

   if (soil) soil->render_inspectors(inspectors,emanator_known,soil->get_ontology());//160616
   if (organic_matter_residues_profile_kg_m2)
      organic_matter_residues_profile_kg_m2->render_inspectors
         (inspectors,emanator_known,"OM_and_residues");                          //160626
   return emanator_known; //emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION________________________________________________/
}//_namespace CropSyst_________________________________________________________/

