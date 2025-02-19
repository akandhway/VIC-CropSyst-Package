#include "options.h"
#include "cs_accum.h"
// CS_accum must be first
#ifndef USE_CS_SIMULATION_ROTATOR
//WARNING prevent_multiple_harvests_in_same_day is a global variable.
#endif
extern bool prevent_multiple_harvests_in_same_day;

#ifdef LADSS_MODE
#  include "LADSS.h"
#endif
#include "land_unit_sim.h"
#include "soil.h"
#include "mgmt_types.h"
#include "crop/crop_param.h"
#  ifdef CROPGRO
#     include "crop/crop_interfaced_cropgro.h"
#  endif

#include "crop/crop_interfaced.h"
#ifdef CO2_CHANGE
#  include "crop/CO2_response.h"
#endif
#  include "common/weather/weather_provider.h"
#include "common/weather/hour/weather_hours.h"

#include "cs_ET.h"
#include "csvc.h"
#include "soil/chemicals_profile.h"
#include "soil/nitrogen_profile.h"
#include "inorganic_NH4.h"

#include "cs_till.h"
#include "organicn.h"
#include "soil/runoff.h"
#include "rot_param.h"
#include "cs_event.h"
#include "soil/tillage_effect.h"
#ifndef compareHPP
#     include "corn/math/compare.hpp"
#endif

#include "common/residue/residues_interface.h"
#include "common/biomatter/decomposition_const.h"
#include "common/simulation/log.h"
#include "common/weather/sky.h"                                                  //170221LML
#include "static_phrases.h"
#include "common/residue/residue_const.h"
#include "common/biomatter/biomass_decomposition_param.h"
#include "organic_matter/OM_residues_profile_abstract.h"
#include "soil/salinity_profile.h"
#include "corn/data_source/vv_file.h"
#include "cs_operation.h"
#include <math.h>
#include "cs_mgmt.h"
#include "weather/seasons.h"
#include "crop/crop_N_balancer.h"
#include "crop/crop_fruit.h"
#include "crop/crop_orchard_common.h"
#include "crop/crop_orchard_Kemanian_Stockle.h"
#include "crop/crop_orchard_Oyarzun_Stockle.h"
#include "crop/yield_temperature_extreme_response.h"
#ifdef INTERCROPPING
#include "crop/intercrop.h"
#endif
#include "soil/erosion_RUSLE.h"
#include "corn/chronometry/date_time_64.h"

using namespace std;
using namespace CropSyst;                                                        //120419
#if (!defined(USE_CS_SIMULATION_ROTATOR) && (CS_VERSION < 5))
#  define _LOG_DEPRECATED ,log
#else
#  define _LOG_DEPRECATED
#endif
#ifdef OTHER_CROP_MODELS
#include "/home/rnelson/dev/APSIM/crop/Nwheat/Nwheats_CropSyst.h"
#endif

#ifdef PEST_VERSION
#include "pest/pest.h"
#endif

#include "CS_suite/observation/CS_examination.h"
/*200811 obsolete
#include <q_n_d.h>
*/
extern bool harvest_event_failed_today;  // this is a check for Mike Rivingtons spreadsheet
//______________________________________________________________________________
namespace CropSyst {
//______________________________________________________________________________
Soil_tillage_effect *Land_unit_simulation_VX::provide_soil_tillage_effect()
{  Soil_tillage_effect *soil_tillage_effect = 0;                                 //060929
   CropSyst::Soil_components *soil = mod_soil();                                 //060803
   if (soil)                                                                     //991122
   {  if (!soil->tillage_effect)                                                 //060929
           soil->tillage_effect= new Soil_tillage_effect                         //070725
       (soil,soil->layers,soil->hydraulic_properties,soil->erosion);             //061118
      soil_tillage_effect=soil->tillage_effect;                                  //060929
   }
   return soil_tillage_effect;                                                   //060929
}
//_provide_soil_tillage_effect______________________________________2006-09-29_/
Event_status_indicator Land_unit_simulation_VX::perform_operation
(Common_operation &op_)                                            modification_ //130703
{
   Operation &CS_op = (Operation &)op_;                                          //120416
   CropSyst_Op op_type = (CropSyst_Op)CS_op.get_type();

//#if (CS_VERSION==4)
// Currently this is in a version but eventually V5 will have explaination

//V5 will use explainations || (CROPSYST_VERSION >= 5)

//Actually I think the log we passed to this function is the same
//event_log_ref but I need to check that is what we were referencing.

//170306 unused/obsolete Common_simulation_log  &log = *CS_event_log;

//CS_event_log_cycle = cycle_shift;

//   log
//   << "[event_" << management_event_count << "]" << std::endl
//   << "date=" << today.get_date32() << std::endl
//   << "type=" << op_type.get_label() << std::endl
//   op_labeled
//   << "description=" << CS_op.description << std::endl
//   << "ID=" << CS_op.name << std::endl
//   << "res_set_ID=" << //was for LADS
//   << "op_ID=" << CS_op.op_ID << std::endl // at this time this is used only for LADSS       //031124
//   <<
//   CropSyst_Op_labeled  op_labeled;                                            //030824

/*
contingency=none
operation_type=INORGANIC_NITROGEN
operation_name=
operation_filename=C:\Users\BGurmessa\Documents

op_ID=0
*/

//#endif

   Event_status_indicator event_status = ES_FAIL;                                //030519
   std::string model("CropSyst");                                                //030520
   bool op_clods_soil = false;                                                   //040513
/*Mingliang must of had this for debug 170208
   //std::cout << "\noperation_lable:"       << CS_op.op_labeled.get_label()
   //          << std::endl;
*/
   switch (op_type)
   {
      case CS_OP_ROT_SOWING :
      {  Sowing_operation & planting_op = (Sowing_operation &)CS_op;             //030519
         event_status = perform_planting(planting_op);
         model.assign("CropSyst:crop");                                          //030519
      } break;
      #ifdef MANAGEMENT
      case CS_OP_MGMT_TILLAGE :
      {
         #if (CS_VERSION == 4)
         const Residues_interface *residue_pools_kg_m2 = organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->ref_residues() : 0; //10626_060613
         float32 subsurface_biomass_before_kg_ha = residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)): 0.0;   //060613
         #endif
         event_status = perform_tillage((Tillage_operation &)CS_op);
         #if (CS_VERSION == 4)
         float32 subsurface_biomass_after_kg_ha  = residue_pools_kg_m2 ? per_m2_to_per_ha(residue_pools_kg_m2->get_biomass_output(INCLUDE_POSITION_SUBSURFACE,INCLUDE_STRAW_RESIDUE_OM|INCLUDE_MANURE_RESIDUE_OM)) : 0.0;  //060613
         float32 change_subsurface_residue = subsurface_biomass_after_kg_ha - subsurface_biomass_before_kg_ha; // Usually should be a positive gain.                                                                       //060613
         event_log_ref << "incorporated_residue=" <<  CORN::float32_to_cstr(change_subsurface_residue,  1) << " kg/ha" << endl;                                                                                                        //060613
         #else
         // V5 will used explainations
         #endif
         op_clods_soil = true;                                                                                                                                                                                             //040513
      } break;
      #if (MANAGEMENT == 4)
      case CS_OP_MGMT_RESIDUE :
      {
         event_status = perform_residue((Residue_operation &)CS_op);
         op_clods_soil = true;                                                   //040513
      } break;
      #endif
      case CS_OP_MGMT_HARVEST_OR_CLIPPING :
      {
         #if (CS_VERSION == 4)
         event_log_ref
         << "type=" << TL_T_Mow_crop_prune  // Currently using description for the type
         << "description=" << TL_T_Mow_crop_prune << endl;
         #else
         // V5 will use explainations
         #endif
         event_status = perform_harvest_or_clipping(
            (dynamic_cast<Harvest_or_clipping_operation &>(CS_op)));
         op_clods_soil = true;                                                   //040513
      } break;
      case CS_OP_MGMT_IRRIGATION :
      {
         event_status = perform_irrigation((Irrigation_operation &)CS_op);
         op_clods_soil = true;                                                   //040513
      } break;
      case CS_OP_MGMT_INORGANIC_NITROGEN:
      {  event_status = perform_inorganic_nitrogen((Inorganic_nitrogen_operation &)CS_op);
         op_clods_soil = true;                                                   //040513
      } break;
      #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
      case CS_OP_MGMT_ORGANIC_NITROGEN:
      {  event_status = perform_organic_nitrogen((CropSyst::Organic_nitrogen_operation &)CS_op);
         op_clods_soil = true;                                                   //040513
      } break;
      #endif
      case CS_OP_MGMT_BIOMATTER_APPLICATION:                                     //080829
      {  event_status = perform_biomatter_application((Biomatter_application_operation&)CS_op); //080829
         op_clods_soil = true;                                                   //080829
      } break;                                                                   //080829
      case CS_OP_MGMT_AUTO_IRRIGATION :
      {
         //170420 I intended to reimplement autoirrigation, but haven't done that yet

                                                                                 //150701LML added VIC macro
         event_status =
         scenario_control_and_model_options.                                     //130826
         CPF_irrigation_disable                                                  //021205
         ? ES_FAIL    // false                                                   //021205
         : perform_start_automatic_irrigation_mode((Automatic_irrigation_mode &)CS_op);
         #if (CS_VERSION==4)
         if (event_status >= ES_ERROR) // (!performed)                           //030517
            event_log_ref << "reason=This run is probably a crop production function." << endl;  //021205
         #else
         // V5 will use explainations
         #endif
      } break;
      case CS_OP_MGMT_AUTO_N_APPL :                                              //120624
      {  // CS_OP_MGMT_AUTO_NO3_APPL may be obsolete                             //120624
         // This is now essentially a split application event
         if (scenario_control_and_model_options.run_nitrogen)                    //020723
            event_status =
               perform_automatic_N_application((CropSyst::N_application_soil_observation_mode_class::Split_application &)CS_op); //120710
      } break;                                                                   //120624
      case CS_OP_MGMT_AUTO_CLIP_BIOMASS :
         event_status = perform_start_automatic_clip_biomass_mode
            (dynamic_cast<Auto_clip_biomass_mode &>(CS_op));
       break;
     #endif
      case CS_OP_RECALIBRATION :
         event_status = perform_recalibration((Recalibration_operation &)CS_op);
      break;
      case CS_OP_PROFILE_PRINT :                                                 //051201
      {  profile_print_event::Print_operation &print_op
            = (profile_print_event::Print_operation &)CS_op;                     //051201
         print_op.process();                                                     //051201
      } break;
      #ifdef MANAGEMENT
      case CS_OP_MGMT_SOIL_N_SAMPLING :
      { perform_start_automatic_N_application_mode                               //120624
            ((N_application_soil_observation_mode_class &)CS_op);                //120624
      } break;
      #endif
      default :
      {
      } break;
   } //switch
   if (  (event_status == ES_COMPLETE) && op_clods_soil)                         //040702
   {  //Currently all operations are assumed to disturb the soil and possible clod the soil
      Soil_tillage_effect *soil_tillage_effect = provide_soil_tillage_effect();  //060929
      if (soil_tillage_effect)                                                   //060929
          soil_tillage_effect->clod_the_soil(ref_today().get_DOY(),0.9);         //060929
   }
   return event_status ;
}
//_perform_operation________________________________________________2002-03-08_/
Event_status_indicator Land_unit_simulation_VX::terminate_operation
(Common_operation &op_)                                            modification_
{  // Currently we only terminate operation periods
   return end_operation_period(op_,simdate);
}
//_terminate_operation______________________________________________2017-03-10_/
bool Land_unit_simulation_VX::end_operation_period
(Common_operation &op
,const Date_const &date_period_ends)
{
   bool ended = true;
   #ifdef MANAGEMENT
   CropSyst::Operation &CS_op = (CropSyst::Operation &)op;                       //120416
   CropSyst_Op op_type = (CropSyst_Op )CS_op.get_type();
   CropSyst::Crop_model_interface *crop_active =                                 //190616_130719_131009
   #ifdef INTERCROPPING
//NYI   dynamic_cast<Crop_interfaced *>(event->associated_object);               //130719
//NYI assert active_crop;  //130719 this line is temporary just to make sure the referencing is correct
//NYI   if (!active_crop) crop_a       ctive =

//We need to get the associated crop object from the event and not the predominant_crop

      get_predominant_crop();                                                    //130719
   // this if statement is just a sanity check
   #else
   crop_active_or_intercrop;                                                     //130719
   #endif
   switch (op_type)
   {  case CS_OP_MGMT_AUTO_CLIP_BIOMASS :
         process_any_automatic_clipping(true);
         curr_automatic_clip_mode = 0;
            // We dont delete current_automatic_clip_mode (It is owned by the event queue).
         ended = true;
       break;
      case CS_OP_MGMT_HARVEST_OR_CLIPPING:                                       //040721
      {
         if (crop_active) crop_active->set_no_more_clippings_this_season();      //040721
         ended = true;                                                           //040721
      } break;                                                                   //040721
      case CS_OP_MGMT_AUTO_NITRO_OPTIMAL :                                       //021001
            if (crop_active) crop_active->set_automatic_nitrogen_mode(false);    //040525
       // No break  because we want to clear curr_automatic_NO3_application_mode //040525
      case CS_OP_MGMT_AUTO_N_BALANCE_BASED :                                     //021001
      #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
      case CS_OP_MGMT_AUTO_N_LOCAL_CALIBRATION :                                 //021001
       #endif
         curr_N_application_soil_observation_mode = 0;                           //021001
         ended = true;                                                           //021001
       break;                                                                    //021001
       case CS_OP_MGMT_AUTO_IRRIGATION :                                         //021204
         curr_automatic_irrigation_mode = 0;                                     //021204
         ended=true;                                                             //180702
         //printf("curr_automatic_irrigation_mode is ENDED!\n");
         if (infiltration_submodel)
            infiltration_submodel->know_directed_irrigation(1.0); //200226
         #endif
       break;                                                                    //021204
       default: break;                                                           //170216
   }
   return ended;
}
//_end_operation_period_____________________________________________2002-05-04_/
Event_status_indicator Land_unit_simulation_VX::perform_planting
(Sowing_operation &sowing_op)
{  Event_status_indicator event_status = ES_COMPLETE;                            //131018_040702
   Land_use crop_land_use = FALLOW; // set when the crop is identified           //190618
   #ifndef INTERCROPPING
   if (crop_active_or_intercrop && crop_active_or_intercrop->ref_phenology().is_in_growth_period() ) //130903
   {  return ES_ERROR; // We are already in a growing season.                    //031014
   }                                                                             //031014
   #endif
   CropSyst::Crop_model_interface *new_crop = 0;                                 //031120_131009
   #ifdef MANAGEMENT
   bool management_specified = sowing_op.management;                             //031203
   curr_management =                                                             //020719
      (management_specified)                                                     //030407
      ? sowing_op.management                                                     //030407
      : new Management;                                                          //030407
   if (soil && soil->erosion)                                                    //160506
      soil->erosion->know_soil_conservation_factor                               //160506
         (curr_management->get_soil_conservation_factor());
   #endif
   bool fall_sowing =
      //190705 CS::Seasons(ref_today(),meteorology.get_geocoordinate().is_in_southern_hemisphere()).
      // get_season_S_S_F_W() == FALL_SEASON;                                    //140812
      meteorology.ref_seasons().get_season_S_S_F_W() == FALL_SEASON;             //190705_140812
   if (crop_sowing_postponed)                                                    //070515
   {  new_crop = crop_sowing_postponed;                                          //070515
      crop_sowing_postponed = 0;                                                 //070515
   } else                                                                        //070515
   {  // Note crop_params will be relinquished to the crop                       //070515
      CropSyst::Crop_parameters *crop_params=sowing_op.relinquish_crop_params(); //030711
      bool crop_loaded = crop_params != 0;                                       //150919_030611
      if (crop_loaded )                                                          //020409
      {
         crop_land_use = crop_params->param_land_use();                          //190618
         CropSyst::Soil_components *soil = mod_soil();  // for local reference   //060803
         Crop_CO2_response *CO2_response = 0;                                    //131208
         #ifdef CO2_CHANGE
         // Actually I should pass the CO2 response
         if (meteorology.has_atmospheric_CO2_change())                           //151201
         {
            CO2_response = new
               CropSyst::Crop_CO2_response
                  (crop_params->CO2_response
                  ,meteorology.ref_weather_provider().vapor_pressure_saturation_slope
                  ,meteorology.ref_weather_provider().psychrometric_constant
                  #if (CROPSYST_PROPER==4)
                  ,resistance_of_plants_d_m                                      //180805
                  #else
                  ,meteorology.ET_ref->ref_aerodynamic_resistance_ref_plants()   //160731
                  #endif
                  ,meteorology.ref_CO2_current_conc());
            CO2_response->update_with_new_atmphospheric_CO2();
         }
         #endif
         switch (crop_params->crop_model_clad.get())                             //031121
         {                                                                       //031121
            #ifdef CROPGRO
            case CROPGRO_MODEL :                                                 //031121
               delete CO2_resonse; // Doesn't currently use                      //151201

               new_crop = new Crop_interfaced_CropGro                            //031121
               (*soil
               ,soil->mod_nitrogen() // Soil_nitrogen_interface                  //040604
               ,(weather_local_refSX) // Weather_interface
               ,crop_params
               ,active_model_options->run_salinity);                             //040211
            break;                                                               //031121
            #endif
            #ifdef OTHER_CROP_MODELS
            case NWHEATS_MODEL :                                                 //140714
            {  Nwheat::Crop_parameters_class *nwheat_parameters_relinquished = new Nwheat::Crop_parameters_class;

               CORN::VV_File nwheat_parameter_file(sowing_op.crop_filename.c_str());
               nwheat_parameter_file.get(*nwheat_parameters_relinquished);
               // Warning, need to load nwheat_parameters

               delete CO2_resonse; // Doesnt currently use                       //151201
               //NYI new_crop
               CropSyst::Crop_Nwheat_interfaced* Nwheat_crop
                = new CropSyst::Crop_Nwheat_interfaced
                  (crop_params
                  ,nwheat_parameters_relinquished
                  ,*soil
                  ,meteorology.ref_air_temperature_max()                         //151130
                  ,meteorology.ref_air_temperature_min()                         //151130
                  ,meteorology.ref_air_temperature_avg()                         //151130
                  ,meteorology.ref_solar_radiation()                             //151130
                  ,day_length_hours
                  ,organic_matter_residues_profile_kg_m2 ? organic_matter_residues_profile_kg_m2->get_residues() :0
                  ,today);
               new_crop = Nwheat_crop;
            }
            break;
            #endif

//190613 #ifndef OLD_ORCHARD
// This is new dont delete

            case CROPSYST_FRUIT_MODEL :                                          //190625_071121
            {
               Orchard_interface *orchard =
                  crop_params->ref_morphology().canopy_architecture
                  ? provide_orchard_interception_model (*crop_params)                                                //071117
                  : 0; // may be 0 if not simulating canopy architecture

               new_crop = new                                                    //020409
               Crop_orchard_fruit
                  (crop_params
                  ,scenario_control_and_model_options.soil_root_activity_coef.dry //170518
                  ,scenario_control_and_model_options.soil_root_activity_coef.saturation //170518
                  ,false
                  ,simdate_raw
                  ,performable_operations
                  ,&meteorology.ref_CO2_current_conc()                           //180122
                  ,CO2_response
                  ,meteorology.ref_air_temperature_max()
                  ,meteorology.ref_air_temperature_min()
                  ,meteorology.ref_air_temperature_avg()
                  ,meteorology.ref_solar_radiation()
                  ,meteorology.ref_vapor_pressure_deficit_daytime()
                  ,meteorology.ref_vapor_pressure_deficit_max()
                  ,meteorology.ref_weather_provider().psychrometric_constant
                  #if (CROPSYST_PROPER==4)
                  ,resistance_of_plants_d_m                                      //180805
                  #else
                  ,meteorology.ET_ref->ref_aerodynamic_resistance_ref_plants()   //160731
                  #endif
                  ,meteorology.ref_sun()
                  ,meteorology.ref_weather_spanning()
                  ,soil
                  ,get_soil_chemicals()
                  ,soil->mod_salinity()
                  ,(scenario_control_and_model_options.run_nitrogen ? CROP_NITROGEN_ORIGINAL: CROP_NITROGEN_DISABLED)
                  #ifdef NITROGEN
                  ,get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference()
                  #endif
                  ,meteorology.ref_seasons()                                     //190705
                  ,orchard);
            } break;
               default :
               {
                  Crop_complete  *new_crop_interfaced_CropSyst = new             //190617
                  Crop_complete                                                  //190617
                  (crop_params
                  ,scenario_control_and_model_options.soil_root_activity_coef.dry //170518
                  ,scenario_control_and_model_options.soil_root_activity_coef.saturation //170518
                  ,true //190617
                  ,simdate_raw                                                   //170525
                  ,performable_operations                                        //151004
                  ,&meteorology.ref_CO2_current_conc()                           //180122
                  ,CO2_response
                  ,meteorology.ref_air_temperature_max()
                  ,meteorology.ref_air_temperature_min()
                  ,meteorology.ref_air_temperature_avg()
                  ,meteorology.ref_solar_radiation()
                  ,meteorology.ref_vapor_pressure_deficit_daytime()
                  ,meteorology.ref_vapor_pressure_deficit_max()
                  ,meteorology.ref_weather_provider().psychrometric_constant
                  ,meteorology.ref_weather_spanning()                            //151201
                  ,meteorology.ref_sun().ref_sun_rise_time()                     //150921
                  ,meteorology.ref_sun().ref_day_length_hours_today()            //150921 140812
                  #if (defined (CROPSYST_PROPER) || defined(OFOOT_VERSION))
                  ,&(sowing_op.seeding_parameters)                               //190617
                  #else
                  ,0
                  #endif
                  ,soil                                                          //020715
                  ,(scenario_control_and_model_options.run_nitrogen ? CROP_NITROGEN_ORIGINAL: CROP_NITROGEN_DISABLED)
                  #ifdef NITROGEN
                  ,get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference()
                  #endif
                  );
                  new_crop = (CropSyst::Crop_model_interface *)new_crop_interfaced_CropSyst;                       //080908
               } break; // default
            } // switch

            const CS::Weather_hours *hourly_weather=meteorology.provide_hourly(); //190707
            if (hourly_weather)                                                  //190707
               new_crop->know_weather_hourly(hourly_weather);                    //190707

            new_crop->initialize();                                              //071121
            /*190617  replacing this with explainations
            #if (CS_VERSION==4)
            new_crop->know_event_log(&event_log_ref);                            //130715_071121
            #else
            new_crop->know_event_log(CS_event_log);                              //130715_071121
            // eventually all versions will use explainations
            #endif
            */
            if (scenario_control_and_model_options.organic_matter_and_residue_model_clad.get() == multiple_SOM_multiple_residues)
               // Decomposing canopy is only available in this model             //071121
               new_crop->know_residues(organic_matter_residues_profile_kg_m2->mod_residues()); //071121
            #ifdef MANAGEMENT
            bool auto_N_optimal =                                                //071121
                  curr_management && curr_management->automatic_nitrogen;        //160329_071121
            new_crop->set_automatic_nitrogen_mode(auto_N_optimal);               //071121
            #endif
            sowing_op.crop_description = new_crop->get_description();
            sowing_op.description.brief = sowing_op.crop_description;            //150112_030520
         } // crop loaded
      } // Now we should have a crop
      if ((sowing_op.sowing_date_mode_clad.get() >  FIXED_DATE)                  //000330
          && new_crop)                                                           //120313
      {  // If the crop model does not have thermal time to emegence we can't do computed sowing date
         int32 param_emerg_deg_day = new_crop->param_emergence_deg_day();        //031126
         bool can_do_computed_sowing_date = param_emerg_deg_day;                 //031126
         float64 est_degree_days_X_days =
            sowing_op.expected_days_to_emerge                                    //160803
            ? new_crop->est_degree_days_X_days_from_now                          //020220
                  (ref_today(),sowing_op.expected_days_to_emerge)
            : 0;
         const Weather_spanning_interface &weather_spanning
            = meteorology.ref_weather_spanning();                                //151130
         const CropSyst::Soil_components *soil = ref_soil();                     //160921_060803
         bool planting_conditions_satisfied =
            sowing_op.good_planting_conditions
               (ref_today()
               ,soil->get_plant_avail_water(1)
               ,soil->get_plant_avail_water(2)                                   //000330
               ,est_degree_days_X_days                                           //141216_020220
               ,param_emerg_deg_day                                              //031120
               ,weather_spanning);                                               //151130
         if (can_do_computed_sowing_date                                         //000327
            && (!soil ||planting_conditions_satisfied))
         {  new_crop->plant(fall_sowing);                                        //190621_110824_990421
         } else
         {  event_status = ES_POSTPONE;
            crop_sowing_postponed = new_crop;                                    //070514
            new_crop = 0;
         }
      } else
         if (new_crop)                                                           //111119
            new_crop->plant(fall_sowing);                                        //190621_110824_990421
   // I think always need to do this otherwise we won't pickup no management (days for harvest)  //000313
   if (new_crop)                                                                 //000327
   {                                                                             //991122
      #if (CROPSYST_VERSION < 5)
         // Layout parameters were stored in Crop File
      #else
      /*  NYI   180109
      if (layout) new_crop->know_layout(layout)
      */
      #endif
      #ifdef INTERCROPPING
      CropSyst::Intercrop *existing_intercrop = dynamic_cast<CropSyst::Intercrop *>(crop_active_or_intercrop);
      if (existing_intercrop)
         existing_intercrop->take(new_crop);
      else
         if (crop_active_or_intercrop)
         {  // we have a crop but it is not intercrop
            CropSyst::Intercrop * intercrop = new CropSyst::Intercrop
               (meteorology.ref_air_temperature_avg());                          //151130
            intercrop->take(crop_active_or_intercrop);
            intercrop->take(new_crop);
            crop_active_or_intercrop = intercrop;
         } else
      #endif
         // we dont currently have a crop so the new crop is just a single crop
         crop_active_or_intercrop = new_crop;                                    //020504

      #ifndef VIC_CROPSYST_VERSION
      if (crop_nitrogen_mass_balance_ref)                                        //160803
      {
      new_crop->track_nitrogen_mass_balance(crop_nitrogen_mass_balance_ref);     //130715_070513
      crop_nitrogen_mass_balance_ref->start_season();                            //160803_070513
      }
      #endif
      event_status = ES_COMPLETE;                                                //040702
      // Need to make sure we have at least one harvest event.

      /* Warning NYI
      new_crop->ensure_at_least_one_harvest();                                   //151004
      */

      #ifdef CROPSYST_PROPER
      // FastGraph support
      float64 max_LAI = new_crop->param_max_LAI();
      set_CS_output_variable_max_warning_range(CSVC_crop_base_leaf_area_index ,max_LAI);
      set_CS_output_variable_max_warning_range(CSVC_crop_base_green_area_index,max_LAI);
      set_CS_output_variable_max_warning_range(CSVC_crop_base_root_depth      ,new_crop->param_max_root_depth_m()); // Meters
      #endif
      if (scenario_control_and_model_options.cropping_system_mode_clad.get()
          != established_cropping_system)                                        //071113
      {  respond_to_field_operation( sowing_op.NRCS_operation_description,0);    //060725
      }                                                                          //071113
      float32 param_wilt_leaf_water_pot = new_crop->param_wilt_leaf_water_pot(); //031126
      // For CropGro these parameters are 0 and we dont reinitialize soil we leave real_wilt_point at perm_wilt_point.
      if (!(CORN::is_zero<float32>(param_wilt_leaf_water_pot,0.0001)))           //031126
         mod_soil()->reinitialize_for_crop(param_wilt_leaf_water_pot);           //160921
      // We must initiate crop render inspectors here because we need to provide emanator context
      std::string crop_ID; CORN::wstring_to_string(sowing_op.crop_filename.get_name(),crop_ID);
      #ifdef CROPSYST_PROPER
      #if (CROPSYST_VERSION==5)
      if (emanator_known)                                                        //160614
         new_crop->render_inspectors                                             //150919
            (CS::get_examination_global().inspectors                             //171115_161202
            ,emanator_known,crop_ID);
      #endif
      start_growing_season(*crop_active_or_intercrop);                           //200503
      #endif
      /*190618 I have decided to get rid of the fallow crop state
      The crop should be simply removed (or replaced with a specific fallow plant cover ).
      #if (CROPSYST_VERSION == 4)
      new_crop->set_fallow_deprecated(false); // This is temporary until I get fallow crop class  //020416
      #endif
      */
      if (runoff) runoff->know_crop_land_use(crop_land_use);                     //190617
   }
   return event_status;                                                          //040702
}
//_perform_planting_________________________________________________2002-03-08_/
// Not needed for REACCH or VIC and probably not OFoot
Event_status_indicator Land_unit_simulation_VX::perform_recalibration
(CropSyst::Recalibration_operation &recalib_op)
{  Event_status_indicator event_status = ES_FAIL;                                //040702
   //Only change the layers for which we have data (the recal file may only have a certain number of layers)         //010228
   CropSyst::Soil_components *soil = mod_soil();                                 //160921_060803
   if (soil)                                                                     //060803
   {  switch (recalib_op.var_code_clad.get())
      {                                                                          //980729
      case CropSyst::Recalibration_operation::WC_RECAL  :                        //000313
      {  float64 change_in_WC =                                                  //000313
           soil->recalibrate_water_content(recalib_op.values_by_horizon);        //010228
         if (ANNL_accumulators_active)       ANNL_accumulators_active->water_cont_recalib       += change_in_WC;     //000313
         if (GP_accumulators_active)         GP_accumulators_active->water_cont_recalib         += change_in_WC;     //000313
         #if (CROPSYST_PROPER==4)
         if (FALLOW_accumulators_active)     FALLOW_accumulators_active->water_cont_recalib     += change_in_WC;     //000313
         #endif
         #ifdef CROPSYST_PROPER
         if (CONTINUOUS_accumulators_active) CONTINUOUS_accumulators_active->water_cont_recalib += change_in_WC;     //000313
         #endif
         event_status = ES_COMPLETE;                                             //120624
      } break;                                                                   //980729
      case CropSyst::Recalibration_operation::PAW_RECAL :                        //000202
      {  float64 change_in_WC =                                                  //000313
           soil->recalibrate_plant_available_water(recalib_op.values_by_horizon);//010228
         if (ANNL_accumulators_active)          ANNL_accumulators_active->water_cont_recalib       += change_in_WC;  //000313
         if (GP_accumulators_active)            GP_accumulators_active->water_cont_recalib         += change_in_WC;  //000313
         #if (CROPSYST_PROPER==4)
         if (FALLOW_accumulators_active)        FALLOW_accumulators_active->water_cont_recalib     += change_in_WC;  //000313
         #endif
         #ifdef CROPSYST_PROPER
         if (CONTINUOUS_accumulators_active)    CONTINUOUS_accumulators_active->water_cont_recalib += change_in_WC;  //000313
         #endif
         event_status = ES_COMPLETE;                                             //120624
      } break;
      #ifdef NITROGEN
      case CropSyst::Recalibration_operation::NO3_RECAL:                         //011115
      {  if (soil->chemicals && soil->chemicals->NO3)                            //011115
         {  soil_sublayer_array_64(values_by_layer);                             //051120
            soil->distribute_amount(recalib_op.values_by_horizon.get_array(),values_by_layer); //070607
            soil->chemicals->NO3->recalibrate(values_by_layer,false);            //011115
            event_status = ES_COMPLETE;                                          //120624
         }
      } break;
      case CropSyst::Recalibration_operation::NH4_RECAL:                         //011115
      {  if (soil->chemicals && soil->chemicals->NH4)                            //011115
         {  soil_sublayer_array_64(values_by_layer);                             //070607
            soil->distribute_amount(recalib_op.values_by_horizon.get_array(),values_by_layer); //070607
            soil->chemicals->NH4->recalibrate(values_by_layer,false);            //011115
            event_status = ES_COMPLETE;                                          //120624
         }
      } break;
      #endif
      case CropSyst::Recalibration_operation::POM_RECAL :                        //080226
      {                                                                          //080226
         soil_sublayer_array_64(actual_org_matter_biomass);                      //980729
         for (nat8 horizon = 1                                                   //010202
            ; horizon <= (nat8) std::min<nat8>((nat8)soil->parameters_ownable->horizon_count,(nat8)recalib_op.values_by_horizon.get_count())  //190402_010228
            ; horizon++)
         {  for (int8 layer = 1 ;layer <= soil->layers->count(); layer++)
            if (horizon == soil->layers->get_horizon_at_layer(layer) )           //051116
            {  float64 bulk_density_tonne_m3 =  // tonne/m3 is same as g/cm3     //051204
                  soil->structure->get_bulk_density_g_cm3(layer);                //070725
               actual_org_matter_biomass[layer] =                                //980730
                     (recalib_op.values_by_horizon.get(horizon)/100.0)
                        * soil->ref_layers()-> get_thickness_m(layer)            //160412_150424_010228
                        * tonne_to_kg(bulk_density_tonne_m3);                    //980730
            }
         }
         #ifdef RESIDUES
         if (organic_matter_residues_profile_kg_m2)
         {
               // With the multiple pool mode (Maybe also the new single pool, need to check)
               // we have no option but to recreate the OM and residue pools
               instanciate_organic_matter_and_residues();
               //200404 if (crop_at_start_season )crop_at_start_season ->know_residues(organic_matter_residues_profile_kg_m2->mod_residues());
               if (crop_active_or_intercrop)crop_active_or_intercrop->know_residues(organic_matter_residues_profile_kg_m2->mod_residues()); // 130719
               //200404 if (crop_at_end_seasonX   )crop_at_end_seasonX   ->know_residues(organic_matter_residues_profile_kg_m2->mod_residues());
               if (crop_sowing_postponed)crop_sowing_postponed->know_residues(organic_matter_residues_profile_kg_m2->mod_residues());
               initialize_residues();
         }
         #endif
         event_status = ES_COMPLETE;                                             //040702
      } break;
      default: break;                                                            //170217
      } // switch soil recalibration
   } // if soil
   switch (recalib_op.var_code_clad.get())
   {
      case CropSyst::Recalibration_operation::SNOW_RECAL :                       //990204
      {
         std::cout << "snow depth recalibration has been disabled. email rnelson@wsu.edu" << std::endl;
         assert(false);
      } break;
      case CropSyst::Recalibration_operation::CO2_RECAL :                        //120916
      {  // The weather object already handles CO2 events                        //120916
         event_status = ES_COMPLETE;                                             //120916
      } break;
     #ifdef RESIDUES
     case CropSyst::Recalibration_operation::RBM_RECAL: // residue biomass       //000312
     {
      Residues_interface *local_residue_pools_kg_m2 = organic_matter_residues_profile_kg_m2->mod_residues();   //060115
      CropSyst::Crop_parameters previous_crop_residue_parameter;                 //060225
         // actually only need the residue decomposition parameters
      if (CORN::OS::file_system_engine.exists                                    //181111
          (scenario_control_and_model_options.previous_residue_crop_filename))   //121128_060115
      {                                                                          //060115
         CORN::VV_File prev_residue_crop
            (scenario_control_and_model_options.previous_residue_crop_filename.c_str());//060115
         prev_residue_crop.get(previous_crop_residue_parameter);                 //060115
      // NYI  eventually specify the name of the crop that produced              //060115
      // the initial residue and load these parameters from that.                //060115
      } // else just use the default crop parameters                             //060115
     } break;                                                                    //000312
      #ifdef CONCEPTUAL
      conceptual:  add recalibration event for biomass,
      inorder to do this I need in residue class delete_all(straw), delete_all(manure)
      because currently delete_all() deletes every thing, this wont work
      because we may have both a RBM and a MBM event!
      #endif
     default: break;                                                             //170217
     #endif
   }
   return event_status;                                                          //040702
}
//_perform_recalibration____________________________________________2002-03-09_/
#ifdef MANAGEMENT
Event_status_indicator Land_unit_simulation_VX::perform_irrigation
(Irrigation_operation   &irrig_op)
{
   // If there happens to be multiple irrigation events on this date
   // simply set the irrigation duration to the largest irrigation duration parameter
   // (there is rarely multiple events occuring during the date).                //070107
   management_irrigation_duration_hours = irrig_op.get_duration_hours();         //070107
   CORN::Date_time_clad_64 op_start_datetime                                     //170425
      (simdate_raw,(Seconds)irrig_op.get_start_hour()*3600);                     //170425
   CORN::Seconds op_duration_seconds =                                           //170425
      (Seconds)irrig_op.get_duration_hours()*3600;                               //170425
   Event_status_indicator event_status = ES_COMPLETE;                            //040702
   float64 target_application = mm_to_m(irrig_op.get_amount_mm());               //020313
   {
   // The user can may enter 0.0 irrigation to recharge the profile.
   if ((irrig_op.application_mode >= irrigate_soil_condition)                    //081118
        || ((irrig_op.application_mode == irrigate_fixed_amount)                 //041028
            && CORN::is_zero<float64>(target_application)))
       // <- The or condition is obsolete, we now have irrigate_soil_condition
   {
      CropSyst::Soil_components *soil = mod_soil();                              //160921_060803
      float64 root_depth = crop_active_or_intercrop                              //170923
         ? crop_active_or_intercrop->get_root_length_m()                         //160412_150424_131202
           + soil->ref_layers()->get_thickness_m(1)
         : 0.0;                                                                  //170923
      target_application = soil->determine_recharge_requirements                 //060719
      (irrig_op.get_net_irrigation_mult(),crop_active_or_intercrop
         ? root_depth
         : 0.0,irrig_op.get_refill_point() );                                    //110427
      const Soil::Layers_interface *layers = soil->ref_layers();
      float64 max_allowable_depletion =  0.0; // In irrigate_soil_condition mode, we always apply refill. 081118
      float64 depletion_observe_depth = layers->get_depth_profile_m();           //081118
      {  max_allowable_depletion = irrig_op.get_max_allowable_depletion();       //081118
            // consider depletion mode is not current implemented in the UI.
         depletion_observe_depth = irrig_op.get_depletion_observe_depth();       //060719
         switch (irrig_op.get_depletion_observation_depth_mode())                //070606
         {  case  soil_profile_depth_mode :                                      //070606
            {  depletion_observe_depth = layers->get_depth_profile_m() ;         //070606
               // this is actually set above, but keep here for completeness  //
            } break;                                                             //070606
            case  fraction_of_root_depth_mode :                                  //070606
            if (crop_active_or_intercrop)                                        //060719
               depletion_observe_depth =
                  crop_active_or_intercrop->get_recorded_root_depth_m();         //060719
            break;                                                               //070606
            // This case and default is already set above                        //070606
            case  constant_fixed_depth_mode : default :                          //070606
               depletion_observe_depth = irrig_op.get_depletion_observe_depth(); //070606
            break;
         }
      }
   }
   } // if consideration_mode                                                    //091201
   // Don't forget to accumulate to irrig_appl_target
   // We previously only considered max application for automatic irrigation     //050825
   // Now it can always be considered.                                           //050825
   //The previous default of 0.0 was in the case of no max application           //041118
   float64 max_appliciation_m =irrig_op.get_max_application_m();                 //050825
   if (max_appliciation_m > 0.000001)                                            //041110
      target_application = std::min<float64>
         (target_application,max_appliciation_m);                                //060719
   if (target_application < irrig_op.get_min_application_m())                    //060719
   {  event_status = ES_FAIL;                                                    //060719
      #if (CS_VERSION==4)
      event_log_ref
         << "reason=water demand (" << target_application                        //060719
         <<") is less than minimum application amount ("
         <<irrig_op.get_min_application_m()<< ")" << endl;
      #else
      // eventually all versions will use explainations
      #endif
   }                                                                             //041028
   if (event_status == ES_COMPLETE)
   {                                                                             //060725
      respond_to_field_operation(irrig_op.NRCS_operation_description,0);         //060725
      earliest_irrigation_start_hour =
         (CORN::is_zero<float64>(irrig_appl_target))
         // If this is the first application of the day
         ? irrig_op.start_hour
            // the earliest hour is as specified in this operation
         : std::min<int16>(earliest_irrigation_start_hour,irrig_op.start_hour);
            // otherwise we take the earliest hour
      irrig_appl_target += target_application;
      if (infiltration_submodel)                                                 //200226
          infiltration_submodel->know_directed_irrigation                        //200226
            (irrig_op.wetting_area_surface_fraction);                            //200226
      fixed_irrigation_scheduled_today = true;                                   //090722
      #if (CS_VERSION==4)
      event_log_ref
      << "amount=" << m_to_mm(target_application) << std::endl                   //110704
      << "amount_units=mm" << std::endl;                                         //110704
      #else
      // eventually all versions will use explainations
      #endif
      #ifdef CHEM_PROFILE
      Soil::Chemicals_profile *soil_chemicals = get_soil_chemicals();            //060803
      if (soil_chemicals)                                                        //081202
      {                                                                          //081202
         #ifdef SALINITY
         float64 irrig_salinity_ECw_dSm =  irrig_op.get_salinity();              //080214
         if (!CORN::is_zero<float64>(irrig_salinity_ECw_dSm))                    //080214
         {
            float64 ECw_to_TDS =  irrig_op.get_ECw_to_total_dissolved_solids();  //080402
            Soil::Soil_salt *salinity = (Soil::Soil_salt *)soil_chemicals->salinity_dSm;
            if (salinity)                                                        //081218
            {  salinity->set_daily_irrig_salinity                                //950119
               (irrig_salinity_ECw_dSm    // dS/m
               ,ECw_to_TDS                                                       //080402
               ,target_application /* depth in meter? */
               ,irrig_op.get_start_hour()                                        //081212
               ,irrig_op.get_duration_hours());                                  //071015
            }
         }
         #endif
         float64 NO3_N_conc = irrig_op.get_NO3_N_concentration();                //080214
         if (!CORN::is_zero<float64>(NO3_N_conc))                                //080214
         {  Soil::Chemical_profile *NO3_profile =
               dynamic_cast<Soil::Chemical_profile *>                            //170425
                  (soil_chemicals->chemical_profiles.find_cstr("NO3"));          //120921 080214
            if (NO3_profile)                                                     //081219
            {  float64 NO3_N_mass = NO3_N_conc * target_application;             //080214
               float64 NO3_mass   = NO3_N_mass
                  * NO3_profile->elemental_to_molecular;                         //080214
               #if (CS_VERSION==4)
               event_log_ref
               << "NO3_N" << per_m2_to_per_ha(NO3_N_mass) << std::endl           //110704
               << "NO3_N_units" << "kg/ha" << std::endl;                         //110704
               #else
               // eventually all versions will use explainations
               #endif
               NO3_profile->apply_surface_mass                                   //170425
                  (NO3_mass                                                      //170425
                  ,op_start_datetime.get_datetime64(),op_duration_seconds,3600); //170425
            }                                                                    //081219
         }                                                                       //080214
         float64 NH4_N_conc= irrig_op.get_NH4_N_concentration();                 //080214
         if (!CORN::is_zero<float64>(NH4_N_conc))                                //080214
         {  Soil::Chemical_profile *NH4_profile
               = dynamic_cast<Soil::Chemical_profile *>                          //170425
               (soil_chemicals->chemical_profiles.find_cstr("NH4"));             //120921_080214
            if (NH4_profile)                                                     //081219
            {                                                                    //081219
               float64 NH4_N_mass = NH4_N_conc * target_application;             //080214
               #if (CS_VERSION==4)
               event_log_ref
               << "NH4_N" << per_m2_to_per_ha(NH4_N_mass) << std::endl           //110704
               << "NH4_N_units" << "kg/ha" << std::endl;                         //110704
               #else
               // eventually all versions will use explainations
               #endif
               float64 NH4_mass
                  = NH4_N_mass * NH4_profile->elemental_to_molecular;            //080214
               NH4_profile->apply_surface_mass                                   //170425
                  (NH4_mass                                                      //170425
                  ,op_start_datetime.get_datetime64(),op_duration_seconds,3600); //170425
            }
         }
      } // soil_chemicals
   }
   #endif
   return event_status; // true;
}
//_perform_irrigation_______________________________________________2002-03-08_/
#endif
#if (MANAGEMENT==4)
// In version 5 residues are now simply field operations
// as with tillage.
Event_status_indicator Land_unit_simulation_VX::perform_residue
(Residue_operation   &residue_op)
{
   // Note, residue event's do not terminate an existing crop
   // (I.e. sugar cane).
   nat16 NRCS_SDR_operation_number = 0;                                          //060802
   if (residue_op.NRCS_operation_description.length())                           //070107
       NRCS_SDR_operation_number = USDA::NRCS::RUSLE2::SDR_field_operations
        .lookup_operation_number(residue_op.NRCS_operation_description.c_str()); //070107
   if (  (NRCS_SDR_operation_number == 15)                                       //101014
       ||(NRCS_SDR_operation_number == 16)
       ||(NRCS_SDR_operation_number == 17))
       // Note that these numbers changed in the RUSLE database
   {
      float64 fract_surface_residue_to_remove = 0.7;                             //101014
      Residues_interface *local_residue_pools_kg_m2
         = organic_matter_residues_profile_kg_m2->mod_residues();                //101014
      // 70% of burned residues are lost
      // the remainder remains
      //101014 NYI may want to report CO2 emissions.
      local_residue_pools_kg_m2->remove_from_surface(fract_surface_residue_to_remove
#if ((CROPSYST_VERSION > 0) &&(CROPSYST_VERSION <=4))
           ,SCS_residue_burning
#endif
         );
   }
   if (NRCS_SDR_operation_number == 0)                                           //140515
      std::clog << "Warning unrecognized RUSLE operation:"
                << residue_op.NRCS_operation_description << std::endl;
   Event_status_indicator event_status = perform_tillage(residue_op);            //040702
   return event_status; // performed;
}
#endif
//_perform_residue__________________________________________________2002-03-11_/
#ifdef MANAGEMENT
Event_status_indicator Land_unit_simulation_VX::perform_tillage
(Tillage_operation   &tillage_op)
{
   Crop_model_interface *crop_active =                                           //130719_131009
   #ifdef INTERCROPPING
      get_predominant_crop();                                                    //130719
         //130719 NYI  WARNING this should actually be the crop object assosiated with the event
   #else
      crop_active_or_intercrop;                                                  //130719
   #endif
   if (crop_active) // 980106 added trimming and plowing under.
   {   //  note that soil disturbance is done by perform_operation after this is called.
      if (tillage_op.terminate_crop)                                             //190613
      {  Biomass_fate_parameters tillage_terminate =                             //050822
              {  0      // remove_for_consumption; // rename this to collected (material other wised harvested for consumption (recorded)
              ,  0      // remove_for_grazing;     // as harvest or grazing
              ,100      // remove_to_residue;      // usually 0 if grazing
              ,  0      // remove_as_loss;             // usually 0 if grazing  (I.e. for inefficiency)
              ,  0      // remove_to_standing_dead;  // stubble if terminate crop otherwise live matter
              ,  0      // remain_as_standing_live;  // stubble if terminate crop otherwise live matter
              ,100      // remove_roots_to_dead_residue;
              ,  0      // remain_roots_live;
              ,  0      // remove_roots_as_loss;   // usually 0  may be needed for root crop harvest
              ,  0      // manure_applied_to_field;
              };
         Crop_mass_fate_biomass_and_nitrogen *mass_and_N_fate =
              crop_active->process_biomass_fate(TERMINATION_CAUSE,OTHER_BIOMASS_FATE,0,0,0,tillage_terminate,tillage_op.terminate_crop,0.0);
         if (!crop_mass_fate_to_be_distributed) crop_mass_fate_to_be_distributed = mass_and_N_fate;   //080216
         else
         {  crop_mass_fate_to_be_distributed->accumulate(*mass_and_N_fate);      //080216
            delete mass_and_N_fate;
         }
         #ifdef MANAGEMENT
         distribute_any_crop_biomass_fate();                                     //080216
         #endif
         #ifdef USE_TRIGGER_SYNCS
         trigger_synchronization                                                 //040506
         (TERMINATED
         #ifdef INTERCROPPING
         ,0  // 130719 WARNING I need to pass the crop index for the crop that is terminating
         #endif
         );
         #endif
         crop_active->set_terminate_pending();                                   //040514
      }
   }
   #if (CROPSYST_VERSION != 4)
   { // Burning of residues
    if (tillage_op.NRCS_operation_description.find("Burn") != std::string::npos)
    {
      float64 fract_surface_residue_to_remove = 0.7;                             //101014
      // fraction of burned residues that are lost the remainder remains
      // 0.7 was the original value used in version 4
      // It corresponds nicely with moderate intensity from RUSLE2 values.
      // These intensity fractions are provided by the RUSLE2 field operation table
      if      (tillage_op.NRCS_operation_description.find("low intensity")       != std::string::npos) fract_surface_residue_to_remove =  0.5;
      else if (tillage_op.NRCS_operation_description.find("moderate intensity")  != std::string::npos) fract_surface_residue_to_remove = 0.75;
      else if (tillage_op.NRCS_operation_description.find("mod. high intensity") != std::string::npos) fract_surface_residue_to_remove = 0.85;
      else if (tillage_op.NRCS_operation_description.find("high intensity")      != std::string::npos) fract_surface_residue_to_remove = 0.93;
      else if (tillage_op.NRCS_operation_description.find("Burn sugarcane")      != std::string::npos) fract_surface_residue_to_remove =  0.5;
         // For sugar cane this is actual (dead) residue removal not total (with life) biomass.
         // The RUSLE database list 0% removal but this is
         // must not be right (using default value).
      Residues_interface *local_residue_pools_kg_m2
         = organic_matter_residues_profile_kg_m2->mod_residues();                //101014
      // 70% of burned residues are lost
      // the remainder remains
      //101014 NYI may want to report CO2 emissions.
      local_residue_pools_kg_m2->remove_from_surface(fract_surface_residue_to_remove);
      //101014 NYI may want to report CO2 and NO emissions.
    }
   }
   #endif
   bool performed = respond_to_field_operation(                                  //060725
      tillage_op.NRCS_operation_description,&(tillage_op.oxidation_effect));
   if (tillage_op.dust_mulch_enable                                              //051206
         && (infiltration_model //190707 scenario_control_and_model_options.infiltration_model_clad.get()
               == FINITE_DIFFERENCE_INFILTRATION))
               // Dust mulching only really applies to F.D.                      //071212
   {  Soil_tillage_effect *soil_tillage_effect = provide_soil_tillage_effect();  //060929
      if (soil_tillage_effect)                                                   //060929
         performed &= soil_tillage_effect->dust_mulching
            (cm_to_m(tillage_op.dust_mulch_depth_cm)
            ,(Days)tillage_op.dust_mulch_effective_days
            ,tillage_op.dust_mulch_intensity);                                   //060929
   }
   return performed ? ES_COMPLETE : ES_FAIL;
}
#endif
//_perform_tillage__________________________________________________2002-03-11_/
Event_status_indicator Land_unit_simulation_VX::perform_harvest_or_clipping
(CropSyst::Harvest_or_clipping_operation   &harvest_or_clipping_op)
{  bool performed = false;
   #ifdef USE_CS_SIMULATION_ROTATOR
   if (prevent_multiple_harvests_in_same_day)                                    //141230
      return ES_POSTPONE;                                                        //141230
   prevent_multiple_harvests_in_same_day = true;                                 //141230
   #endif
   Event_status_indicator event_status = ES_FAIL;                                //040702

   // Note clipping events must not redistribute soil so we
   // dont do soil_distrubance
   Crop_model_interface *crop_active =                                           //130719_131009NS
         #ifdef INTERCROPPING
//130719 NYI I should pass the crop_assocated with this harvest operation.
// For now I am only appling the harvest operation to the predominant crop.
// This will only work for single crop or when the secondary crop becomes the predominate crop before it is harvested.

/* NYI  I need to get the associated crop from the event
         dynamic_cast<Crop_interfaced *>(event->associated_object);              //130719

assert active_crop;  //130719 this line is temporary just to make sure the referencing is correct

         if (!active_crop)
*/
            crop_active = get_predominant_crop();                                //130719
         #else
            crop_active_or_intercrop;                                            //130719
         #endif
   if (crop_active_or_intercrop)
   {
      bool remove_a_fixed_amount =                                               //050107
         harvest_or_clipping_op.harvest_amount_mode == HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS;     //050823
      float64 curr_canopy_biomass_kg_m2 =  crop_active_or_intercrop->get_canopy_biomass_kg_m2();//040807
      float64 demand_kg_m2 = 0.0;  // only used for HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS         //060323
      bool can_meet_all_demand = true;                                           //060310
      bool can_meet_some_demand = true;                                          //060310
      float64 acceptable_remove_amount_kg_m2 = curr_canopy_biomass_kg_m2; // default demand for some of the modes.  //060310
      switch (harvest_or_clipping_op.harvest_amount_mode)                        //060310
      {  case HARVEST_CROP_PARAMETER_BASED         :                             //060310
            // This mode uses harvest index to determine the remove amount
         break;                                                                  //060310
         case HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS  :                             //060310
         {                                                                       //060310
            float64 reserve_biomass_kg_m2  = per_ha_to_per_m2(harvest_or_clipping_op.reserve_biomass_kg_ha);   //051020
            float64 remove_amount_kg_m2    = per_ha_to_per_m2(harvest_or_clipping_op.remove_amount_kg_ha );    //051021
            demand_kg_m2 = remove_amount_kg_m2;                                  //060323
            acceptable_remove_amount_kg_m2 = per_ha_to_per_m2(harvest_or_clipping_op.remove_amount_kg_ha);     //051020
            can_meet_all_demand  = true; //applied only to remove_a_fixed_amount //051020
            can_meet_some_demand = true;                                         //051020
            if (remove_a_fixed_amount)                                           //051020
            {  can_meet_all_demand  =(curr_canopy_biomass_kg_m2 -  remove_amount_kg_m2) >=  reserve_biomass_kg_m2; //051020
               can_meet_some_demand = curr_canopy_biomass_kg_m2 >= reserve_biomass_kg_m2;                      //051020
               if (!can_meet_all_demand && (can_meet_some_demand && harvest_or_clipping_op.accept_less))       //051020
               {  acceptable_remove_amount_kg_m2 =  curr_canopy_biomass_kg_m2 - reserve_biomass_kg_m2;         //051020
                  event_status = event_status_limited_commodity;                 //151003
               }
            }
         } break;
         case HARVEST_RETAIN_GAI_obsolete :
            acceptable_remove_amount_kg_m2 = curr_canopy_biomass_kg_m2;          //060310
         break;
         case HARVEST_PERCENT_BIOMASS:                                           //060310
         {
            /*150223 No longer applicable in this mode
            float64 reserve_biomass_kg_m2  = per_ha_to_per_m2(harvest_or_clipping_op.reserve_biomass_kg_ha); //051020
            acceptable_remove_amount_kg_m2 = curr_canopy_biomass_kg_m2- reserve_biomass_kg_m2;             //130426_060313
            */
              // The remaining parameters for this mode are simply the distribution handled by the crop      //060310
         } break;
         default : // OTHER_BIOMASS_FATE:                                        //060310
         break;
      } // switch
      if (!can_meet_some_demand)                                              event_status = ES_FAIL; //051020
      else if (!can_meet_all_demand && !harvest_or_clipping_op.accept_less )  event_status = ES_FAIL; //051026
      else // The following two sufficiencies are the constraint parameters                           //040807
      {

         //140320  Warning  This should be the crop associated with the harvest event
         // not necessarily the predominate crop

         //bool sufficient_LAI = crop_active->get_LAI(true) >= harvest_or_clipping_op.min_LAI_required; //070416
         bool sufficient_LAI = crop_active->get_LAI(true) > harvest_or_clipping_op.min_LAI_required; //200402

         bool sufficient_biomass =  curr_canopy_biomass_kg_m2 >= per_ha_to_per_m2(harvest_or_clipping_op.min_biomass_required_kg_ha); //051020
         if ((sufficient_LAI && sufficient_biomass)                              //040625
          || harvest_or_clipping_op.terminate_crop )//<-was from period clipping //040625
         {                                                                       //040625
            //040807 warning we only check the reserve_biomass in fixed mode because I would have to
            // to calculate the actual amounts from the percent now instead of the percents upfront
            // and pass that to the event_harvest
            Crop_mass_fate_biomass_and_nitrogen *mass_and_N_fate                 //070625
            =  crop_active->harvest                                              //040625
               (acceptable_remove_amount_kg_m2
               ,&harvest_or_clipping_op
               ,harvest_or_clipping_op.terminate_crop);                          //140811
            Crop_mass_fate *biomass_fate_kg_m2 = mass_and_N_fate
               ? mass_and_N_fate->biomass : 0;                                   //060323
            // grazing, residue fate etc have been moved to the end of the day processing        //040625
            performed = true;                                                    //200603
            if (biomass_fate_kg_m2)                                              //060323
            {
               //200603 performed = true;                                                 //060323
               float64 actually_harvested_kg_m2
                  = biomass_fate_kg_m2->get_total_any_harvested();               //060323
               unsatisfied_harvest_BM_today_kg_m2 = 0.0;                         //060323
               if (demand_kg_m2 > 0.0)                                           //060323
                   unsatisfied_harvest_BM_today_kg_m2
                     += (demand_kg_m2 - actually_harvested_kg_m2);               //060323
               // For NRCS  harvest field operations which do indeed disturb the
               // soil we need to postpone the soil, residue and organic matter
               // response to operation to the next day because by mass fate is
               // not currently actually processed until the next day.           //070107
               if (harvest_or_clipping_op.NRCS_operation_description.length())   //070107
               {
                  nat16 NRCS_SDR_operation_number                                //060802
                  = USDA::NRCS::RUSLE2::SDR_field_operations                     //070107
                    .lookup_operation_number
                    (harvest_or_clipping_op.NRCS_operation_description.c_str());
                  if (NRCS_SDR_operation_number)                                 //070107
                  {
                     float32 residue_flatten_ratio = USDA::NRCS::RUSLE2::SDR_field_operations
                        .get_residue_flatten_ratio(NRCS_SDR_operation_number)[0];//070107
                     if (residue_flatten_ratio > 0.000001)                       //070107
                     {  // there will be some residue and/or soil disturbance    //070107
                        #if (CROPSYST_VERSION == 4)
                        Residue_operation *residue_op = new Residue_operation;   //070107
                        #else
                        Tillage_operation *residue_op = new Tillage_operation;   //131105
                        #endif
                        residue_op->terminate_crop = false;                      //130507
                           // actually terminate the crop if the implement terminates the crop
                        residue_op->NRCS_operation_description = harvest_or_clipping_op.NRCS_operation_description; //070107
                        CORN::Date_clad_32 tomorrow(ref_today().get_date32()); tomorrow.inc_day(); //170530_070107
                        Common_event  *soil_disturbance_event = new Common_event(tomorrow,residue_op,true); //070107
                        scenario_event_list.append(soil_disturbance_event);      //131020_070107
                     }
                  } else
                     std::clog << "Warning unrecognized RUSLE operation:"
                     << harvest_or_clipping_op.NRCS_operation_description << std::endl; //140515
               } // else not NRCS description so do nothing.
               if (!crop_mass_fate_to_be_distributed)
                  crop_mass_fate_to_be_distributed = mass_and_N_fate;            //070627
               else                                                              //070627
               {  crop_mass_fate_to_be_distributed->accumulate(*mass_and_N_fate);//070627
                  delete mass_and_N_fate;                                        //070627
               }
            }  // else in the case of fruit crop, there will be no biomass fate
               // because we just have harvested fruit biomass
            #ifdef MANAGEMENT
            if (curr_management)                                                 //040625
            {  curr_management->latest_grazing_percent_N_as_OM  = harvest_or_clipping_op.grazing_percent_N_as_OM;
               curr_management->latest_grazing_percent_N_as_NH3 = harvest_or_clipping_op.grazing_percent_N_as_NH3;
               // Currently presuming that there will usually only be a single grazing event
               // in the day or they have the same OM and NH3 distribution
               // so I can process all grazed biomatter fate at the end of the day;
               // otherwise, I would need to create a manure pool for each event. //040625
            }
            #endif
         } else // not performed because of insufficient LAI or biomass          //040625
         {  // need to postpone the clipping 1 day                               //040625
            // but dont postpone if we have had a latest date to clip            //040721
            bool allow_more_clippings_this_season = !crop_active->get_no_more_clippings_this_season(); //040721
            if (!allow_more_clippings_this_season)                               //040721
               event_status = event_status_out_of_season;                        //151003
                  // I need to check this case
         }
      }
      if (harvest_or_clipping_op.harvest_fate_mode == HARVEST_LAST_CLIPPING)     //120419_040721
      {  crop_active->set_no_more_clippings_this_season();                       //040721
         crop_active->end_season_if_no_dormancy();                               //050818
         // In Elisenda's simulation we found that
         // Her perennial alfalfa dont go into dormancy,
         // but she needed to advance the season
         // because successive years had reduced biomass transpiration
         // we needed a way to advance the season counter.
      }
   }
   else event_status = ES_OUTOFSEASON;                                           //040809
   if (performed)
   {
      event_status = ES_COMPLETE;                                                //040702
      #ifdef USE_CS_SIMULATION_ROTATOR
      prevent_multiple_harvests_in_same_day = true;                              //141230
      #endif
      harvest_today_flag = true;                                                 //160118
      // Wasn't printing harvest events
      harvest_event(*crop_active);                                               //160711
      if (scenario_control_and_model_options.single.stop_at_harvest)             //181031
      {  // This was added for AgMIP were simulation stops at harvest date.
         scenario_control_and_model_options.stop_date_raw = simdate_raw;         //181031
      }
   }
   if (event_status == ES_FAIL)     // March 2006 This is a hack for Mike Rivingtons Excel spreadsheet
      harvest_event_failed_today = true;
   return event_status;
}
//_perform_harvest_or_clipping_________________________________________________/
#ifdef MANAGEMENT
Event_status_indicator Land_unit_simulation_VX::perform_start_automatic_clip_biomass_mode
(Auto_clip_biomass_mode & auto_clip_op)
{  // Note, we dont need to delete current_automatic_mode it is not owned by management
   curr_automatic_clip_mode = &auto_clip_op;
   return ES_START;                                                              //040702
}
//_perform_start_automatic_clip_biomass_mode________________________2001-07-13_/
Event_status_indicator Land_unit_simulation_VX
::perform_start_automatic_irrigation_mode
(Automatic_irrigation_mode &auto_irrig_mode_op)
{  // Note, we dont need to delete current_automatic_irrigation_mode it is not owned by
   curr_automatic_irrigation_mode = &auto_irrig_mode_op;
   //printf("curr_automatic_irrigation_mode is SET!\n");
   if (infiltration_submodel)                                                    //200226
       infiltration_submodel->know_directed_irrigation                           //200226
         (auto_irrig_mode_op.wetting_area_surface_fraction);                     //200226
   return ES_COMPLETE;
}
//_perform_start_automatic_irrigation_mode_____________________________________/
#ifdef NITROGEN
Event_status_indicator Land_unit_simulation_VX::perform_inorganic_nitrogen
(Inorganic_nitrogen_operation &inorganic_N_op)                                   //060717
{  Event_status_indicator event_status = ES_FAIL;
   Soil::Chemicals_profile       *soil_chemicals = get_soil_chemicals();         //170630
   if (soil_chemicals
       && (inorganic_N_op.condition.is_always_applicable()                       //170719
           || (soil_chemicals->is_below_critical_N                               //170630
               (per_ha_to_per_m2(inorganic_N_op.condition.N_soil_critical)       //170630
               ,inorganic_N_op.condition.sampling_depth))))                      //170630
   {
   respond_to_field_operation(
                  inorganic_N_op.NRCS_operation_description,0);
           //    a fertil event, we need to do it first,
           //    because we dont want to immediately
           //    redistribute any applied manures.
   if (scenario_control_and_model_options.run_nitrogen && soil_chemicals)
   {
      const CropSyst::Soil_components *soil = ref_soil();
      float64 fract_inorganic_to_soil  = fract_inorganic_NH4_to_soil
         (  (Ammonium_source)inorganic_N_op.NH4_source
         ,  (NH4_Appl_method)inorganic_N_op.NH4_appl_method
         ,  inorganic_N_op.NH4_volatilization_calculated                         //990302
         ,  inorganic_N_op.param_NH4_volatilization()                            //990302
         #ifdef VIC_CROPSYST_VERSION
         , 20.0                                                                  //150529 Roger TODO for VIC-CropSyst
         , 7.0
         #else
         ,  soil->get_cation_exchange_capacity(1)                                //150820LML_011130
         ,  soil->get_pH(1)                                                      //150820LML_011130
         #endif
         );
     float64 NH4_N_kg_m2     = (inorganic_N_op.get_NH4_N_kg_ha() * over_ha_to_m2);
     float64 non_volatilized_NH4  = NH4_N_kg_m2 * fract_inorganic_to_soil;
     float64 volatilized_NH4      = NH4_N_kg_m2 - non_volatilized_NH4;           //960802
     nitrogen_daily_output.applied_inorganic += NH4_N_kg_m2;                     //131001
     #ifdef MICROBASIN_VERSION
     nitrogen_daily_output.applied_inorganic_NH4_N += NH4_N_kg_m2;               //160323LML
     double NO3_N_kg_m2     = inorganic_N_op.get_NO3_N_kg_ha() * over_ha_to_m2;
     nitrogen_daily_output.applied_inorganic_NO3_N += NO3_N_kg_m2;               //160323LML
     nitrogen_daily_output.applied_inorganic += NO3_N_kg_m2;                     //160323LML
     #endif

     nitrogen_daily_output.applied_total +=  NH4_N_kg_m2;                        //131001
      nitrogen_daily_output.inorganic_atomospheric_loss_NH4                      //131001
         += volatilized_NH4;                                                     //981231
      //981228 Warning we are releasing NH3 to atmosphere into the NH4 pool      //981228
      // We now split the application evenly between layer 1 and layer 2
      // This is needed because when applying only to layer 2 we
      // starve organic matter and residue decomposition                         //090825
      // Now application depth is specified by the operation
      // all goes into the layer at the specified depth.                         //160120
      nat8 layer_at_depth = ref_soil()->ref_layers()->get_layer_at_depth
         (cm_to_m(inorganic_N_op.injection_depth_cm));                           //150120
      soil_chemicals->NO3->application_at_layer                                  //150120
         ((inorganic_N_op.get_NO3_N_kg_ha() * over_ha_to_m2)
         ,layer_at_depth,true);
     soil_chemicals->NH4->volatilized_application                                //150120
         (non_volatilized_NH4
         ,volatilized_NH4
         ,layer_at_depth);
      event_status = ES_COMPLETE;                                                //040702
      #if (CS_VERSION==4)
      event_log_ref
       << "NH4_N_applied=" << inorganic_N_op.get_NH4_N_kg_ha() << " kg/ha" << endl               //060717
       << "non_volatilized_NH4_N=" << non_volatilized_NH4 * over_m2_to_ha << " kg/ha" << endl    //060717
       << "volatilized_NH4_N=" << volatilized_NH4 * over_m2_to_ha<< " kg/ha" << endl;             //060717
      #else
      // eventually all versions will use explainations
      #endif
      soil_chemicals->commit_diffs();                                            //120731
   }
   } else event_status = event_condition_unsatisfied;                            //170630
   return event_status; // performed;
}
//_perform_inorganic_nitrogen_______________________________________2002-03-11_/
Event_status_indicator Land_unit_simulation_VX::perform_organic_nitrogen_common     // volatilization
(Organic_nitrogen_operation_abstract   &organic_N_op)
{  Event_status_indicator event_status = ES_FAIL;                                //040702
   respond_to_field_operation(organic_N_op.NRCS_operation_description,0);        //060725
   //    a fertil event, we need to do it first,
   //    because we dont want to immediately
   //    redistribute any applied manures.
   Soil::Chemicals_profile       *soil_chemicals = get_soil_chemicals();         //060803
   if (scenario_control_and_model_options.run_nitrogen && soil_chemicals)
   {  float64 NH3_N_kg_ha = organic_N_op.get_NH3_N_kg_ha();                      //020525
      float64 org_N_kg_ha = organic_N_op.get_org_N_kg_ha();                      //020525
      float64 NH3_N_kg_m2 = per_ha_to_per_m2(NH3_N_kg_ha);                       //020525
      float64 org_N_kg_m2 = per_ha_to_per_m2(org_N_kg_ha);                       //020525
      float64 long_term_fract_loss                                               //020525
      = organic_N_op.param_long_term_org_N_volatilization_loss_percent() / 100.0;//080903
     // Warning we are applying NH3 to atmosphere into the NH4 pool              //981230
      float64 volatilized_NH3 = long_term_fract_loss * NH3_N_kg_m2;              //020525
      float64 non_volatilized_NH3 = CORN::must_be_0_or_greater<float64>
         (NH3_N_kg_m2 - volatilized_NH3);                                        //020525
      nitrogen_daily_output.volatilization_loss_NH3 += volatilized_NH3;          //131001_050718
/*200813
obsolete
#ifdef QND
QND_manure_ammonia_N_inc_kg_m2 += non_volatilized_NH3;
QND_manure_ammonia_N_loss_kg_m2 += volatilized_NH3;
#endif
*/
      // We now split the application evenly between layer 1 and layer 2.        //090825
      // This is needed because when applying only to layer 2 we starve organic matter and residue decomposition
      float64 half_non_volatilized_NH3 =  non_volatilized_NH3 / 2.0;             //090825
      float64 half_volatilized_NH3     =  volatilized_NH3     / 2.0;             //090825
      soil_chemicals->NH4->volatilized_application                               //981230
            (half_non_volatilized_NH3,half_volatilized_NH3,1);                   //981230
      soil_chemicals->NH4->volatilized_application                               //981230
            (half_non_volatilized_NH3,half_volatilized_NH3,2);                   //981230
      // Warning we are releasing volatilized NH3 to atmosphere.                 //981228
      // Non-volatilized NH3 goes into the NH4 pool.                             //981228
      soil_chemicals->commit_diffs();                                            //120730
      nitrogen_daily_output.applied_organic += org_N_kg_m2;                      //131001_050718
      nitrogen_daily_output.applied_total += org_N_kg_m2;                        //131003
/*200813
obsolete
#ifdef QND
QND_manure_org_N_appl_kg_m2 += org_N_amount_kg_m2;
#endif
*/
      event_status = ES_COMPLETE;                                                //040702
   } else event_status = ES_DISABLE;                                             //040702
   if (organic_N_op.water_depth_mm > 0.0)                                        //110816 added for CAFE_dairy
   {  irrig_appl_target += mm_to_m(organic_N_op.water_depth_mm);
      fixed_irrigation_scheduled_today = true;
   }
   return event_status;
}
//_perform_organic_nitrogen_common__________________________________2002-03-11_/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
Event_status_indicator Land_unit_simulation_VX::perform_organic_nitrogen
(CropSyst::Organic_nitrogen_operation   &organic_N_op)
{
   if (organic_N_op.org_N_volatilization_calculated)                             //080903
   {  // in versions prior to 4.12.04, we could calculate org_N_volatilization at run time,
      // now it is always a parameter. this caluation is provided for campatibility with previous version
      bool is_liquid_manure = organic_N_op.is_liquid();                          //040508
      organic_N_op.long_term_org_N_volatilization_loss_percent =                 //080903
         (1.0 - get_long_term_fraction_N_retained
         ((Org_N_appl_method)organic_N_op.org_N_appl_method
         ,is_liquid_manure)) * 100.0;
   }
   Event_status_indicator event_status =perform_organic_nitrogen_common(organic_N_op); //080903
   if (event_status == ES_DISABLE) return event_status;
   float64 org_N_kg_ha = organic_N_op.get_org_N_kg_ha();                         //020525
   float64 org_N_kg_m2 = per_ha_to_per_m2(org_N_kg_ha);                          //020525
   float64  act_halflife = organic_N_op.halflife;
      /*200220
      (organic_N_op.decomposition_time_calculated)   //040826
      ? convert_decomposition_time_63_to_50(manure_decomp_time_coef[organic_N_op.organic_matter_source])
      :
      */
   float64 fract_manure_to_subsurface =  manure_incorporation_fraction
            [organic_N_op.org_N_appl_method];                                    //090224
   float64 surface_N_content   = org_N_kg_m2 * (1.0-fract_manure_to_subsurface); //090224
   float64 subsurface_N_content= org_N_kg_m2 * fract_manure_to_subsurface;       //090224
   if (org_N_kg_m2 > 0)                                                          //020525
   {
         Residues_interface *local_residue_pools_kg_m2 = organic_matter_residues_profile_kg_m2->mod_residues();   //060115
         float32 real_halflife = std::max<float32>((float32)act_halflife * 0.7,1.0);  // 040826 RLN: I dont know what this is  Not sure why scaling by  0.7 for manure  //990301
         float32 halflife =  real_halflife;  // check used, something may be off here   //981212
         float64 decomposition_constant =
            convert_halflife_to_constant(halflife);
         local_residue_pools_kg_m2->add_manure_pool_original
           (organic_N_op.organic_matter_source
            ,surface_N_content,                                                  //090224
            subsurface_N_content,
            decomposition_constant                                               //060122
            ,50.0   // _default_carbon_nitrogen_ratio  (I need to store this in a table for different types of manure // Used when we dont have a N mass (or for initial residue) //060225
            ,organic_N_op.carbon_fraction                                        //080225
           , manure_area_per_mass);
      nitrogen_daily_output.applied_organic+= org_N_kg_m2;                       //131001
      nitrogen_daily_output.applied_total  += org_N_kg_m2;                       //131003
   }
   return event_status;
}
#endif
//_perform_organic_nitrogen_________________________________________2002-03-11_/
#endif
Event_status_indicator Land_unit_simulation_VX::perform_biomatter_application
(Biomatter_application_operation   &biomatter_appl_op)
{  Event_status_indicator event_status = perform_organic_nitrogen_common(biomatter_appl_op);
   if (!event_status)
   {  float64 org_N_kg_ha = biomatter_appl_op.get_org_N_kg_ha();                 //020525
      float64 org_N_kg_m2 = per_ha_to_per_m2(org_N_kg_ha);                       //020525
         biomatter_appl_op.provide_decomposition_parameters                      //100216
         (biomatter_appl_op.organic_matter_source_cowl.get());                   //110104
      float64 fract_manure_to_subsurface =                                       //090224
         manure_incorporation_fraction[biomatter_appl_op.org_N_appl_method];     //110113
      float64 surface_N_content = org_N_kg_m2 * (1.0-fract_manure_to_subsurface);//090224
      float64 subsurface_N_content= org_N_kg_m2 * fract_manure_to_subsurface;    //090224
      if (org_N_kg_m2 > 0)                                                       //020525
      {  Residues_interface *local_residue_pools_kg_m2
            = organic_matter_residues_profile_kg_m2->mod_residues();             //060115
         nat8 injection_layer                                                    //160601
            = soil->layers->get_layer_at_depth
               (cm_to_m(biomatter_appl_op.injection_depth_cm));
         local_residue_pools_kg_m2->add_manure_pool_cycled
           (*biomatter_appl_op.decomposition_parameters                          //120419_110817_001116
           ,injection_layer                                                      //160601_160120
           ,surface_N_content
           ,subsurface_N_content);
      }
   } else event_status = ES_DISABLE;                                             //040702
   return event_status;
}
//_perform_biomatter_application____________________________________2008-09-03_/
#ifdef NITROGEN
float64 N_application_soil_observation_mode_class                                //120419
::determine_total_automatic_N_application
(float64 sum_N_to_sampling_depth // kg/m2
,float64 estimatied_mineralization_kg_ha) //  kg/m2
{  float64 auto_N_appl = 0.0; // kg/m2
   //{ Optimal mode is handled by the crop }
   switch (automatic_N_mode)
   {   case AUTO_NITRO_BALANCE_BASED :

       auto_N_appl = per_ha_to_per_m2(target_yield_N_uptake_kg_ha)
         - sum_N_to_sampling_depth
         - per_ha_to_per_m2(estimatied_mineralization_kg_ha);

       break;
      #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <= 4))
      //120710 this was a special mode, Claudio decided to remove it
      //161009 we now only provide 'balanced based' mode
      case AUTO_NITRO_LOCAL_CALIBRATION :
      {  float64 critical_soil_N_for_no_response_kg_m2 = per_ha_to_per_m2(critical_soil_N_for_no_response_kg_ha);
         auto_N_appl = (sum_N_to_sampling_depth > critical_soil_N_for_no_response_kg_m2)
         ?   0.0
         :   critical_soil_N_for_no_response_kg_m2 - sum_N_to_sampling_depth;
      } break;
      #endif
      default: break;                                                            //170217
   }
   float64 total_automatic_N_application = CORN::must_be_0_or_greater<float64>
      (auto_N_appl /(fertilizer_use_efficiency/100.0));
   return total_automatic_N_application;
}
//_determine_total_automatic_N_application__________________________1998-08-24_/
Event_status_indicator Land_unit_simulation_VX::perform_automatic_N_application
(CropSyst::N_application_soil_observation_mode_class::Split_application &auto_N_app)
{
   Event_status_indicator event_status= curr_N_application_soil_observation_mode //040702
      ? ES_COMPLETE : ES_DISABLE;                                                //151003
   // We only want to do this if still in the NO3 application mode period
   // We dont do any application when in using auto N to tissue mode,
   // since that is done elsewere
   Automatic_N_mode auto_N_mode =
      curr_N_application_soil_observation_mode                                   //020913
      ? curr_N_application_soil_observation_mode-> automatic_N_mode
      : AUTO_NITRO_DISABLED;
   bool performed = auto_N_mode;
   if (performed)
   {  event_status = ES_COMPLETE;                                                //040702
      if (total_N_application_kg_m2 > 0.00000001)                                //161130
      {
         auto_N_previous_NO3_kg_m2 = auto_N_app.actual_NO3_kg_m2
         = total_N_application_kg_m2                                             //120710
         * (auto_N_app.get_percent_of_application()/100.0 )                      //120710
         * (auto_N_app.NO3_N/100.0);                                             //120710
         auto_N_previous_NH4_kg_m2 = auto_N_app.actual_NH4_kg_m2
         = total_N_application_kg_m2                                             //120710
         * (auto_N_app.get_percent_of_application()/100.0 )                      //120710
         * (auto_N_app.NH4_N/100.0);                                             //120710
         commit_auto_N(auto_N_app);                                              //161201
   }  }
   return event_status;
}
//_perform_automatic_N_application__________________________________2002-03-14_/
bool Land_unit_simulation_VX::commit_auto_N
(CropSyst::N_application_soil_observation_mode_class::Split_application &auto_N_app)
{
   // Now split between layer 1 and 2, previously only applied to layer 2.       //120710
   // This is needed because when applying only to layer 2 we starve organic matter and residue decomposition
   Soil::Chemicals_profile       *soil_chemicals = get_soil_chemicals();          //060803
   if (soil_chemicals)
   {
      const CropSyst::Soil_components *soil = ref_soil();                        //160921_060803
      nat8 num_layers = soil->ref_layers()->count();                             //170126LML_161130
      float64 half_NO3 = auto_N_app.actual_NO3_kg_m2 / 2.0;                      //161205_120710
         soil_chemicals->NO3->application(half_NO3,1,1,num_layers,true);         //120710
         soil_chemicals->NO3->application(half_NO3,2,4,num_layers,true);         //120710
      float64 half_NH4 = auto_N_app.actual_NH4_kg_m2 / 2.0;                      //161205_120710
      soil_chemicals->NH4->application(half_NH4,1,1,num_layers,true);            //120710
      soil_chemicals->NH4->application(half_NH4,2,4,num_layers,true);            //120710
      // Now split between layer 1 and 2, previously only applied to layer 2.    //120710

      float64 split_N_appl = auto_N_app.actual_NO3_kg_m2 + auto_N_app.actual_NH4_kg_m2 ;
         // Record for output                                                    //161130
      nitrogen_daily_output.applied_total     += split_N_appl;                   //161206_161130
      nitrogen_daily_output.applied_inorganic += split_N_appl;                   //161206_161130
      #ifdef MICROBASIN_VERSION
      nitrogen_daily_output.applied_inorganic_NH4_N += auto_N_app.actual_NH4_kg_m2; //161214LML actual_amount_NH4_kg_m2; //161130
      nitrogen_daily_output.applied_inorganic_NO3_N += auto_N_app.actual_NO3_kg_m2; //161214LML actual_amount_NO3_kg_m2; //161130
      #endif
   }
   return true;
}
//_commit_auto_N____________________________________________________2016-12-01_/
Event_status_indicator Land_unit_simulation_VX::perform_start_automatic_N_application_mode
(N_application_soil_observation_mode_class &auto_N_mode_op)
{  Soil::Chemicals_profile  *soil_chemicals = get_soil_chemicals();  // may return 0 if not soil and/or no soil chemicals. //060803

//std::cout << "SFp:" << auto_N_mode_op.supplementalX.fraction << std::endl;

   total_N_application_kg_m2= 0.0;
   if (soil_chemicals)                                                           //020619
   {  float64 sampling_depth = auto_N_mode_op.soil_N_sampling_depth;             //161130
      total_N_application_kg_m2
        = auto_N_mode_op.determine_total_automatic_N_application
            ( soil_chemicals->sum_NO3_to_depth(sampling_depth,true)              //990208
             +soil_chemicals->sum_NH4_to_depth(sampling_depth,true)              //120710
            ,auto_N_mode_op.estimated_mineralization_kg_ha);                     //120710
       auto_N_mode_op.total_N_application_kg_m2 = total_N_application_kg_m2;
   }
   // we now also allow user to split application in local calib mode
   // || (automatic_nitrogen_mode == AUTO_NITRO_LOCAL_CALIBRATION) //011119
   if (!auto_N_mode_op.split_applications)                                       //020511
   {  // Apply everything now (sampling date or planting)
      // just make the application (we can't schedule it for today)
      // because we've already queued up today's events.                         //160328

      // actually just make the application without queue the event.
      N_application_soil_observation_mode::Split_application auto_N_appl         //160328
      =  N_application_soil_observation_mode::Split_application(total_N_application_kg_m2 ); //160328
      curr_N_application_soil_observation_mode = &auto_N_mode_op;                //161009
      perform_automatic_N_application(auto_N_appl);                              //160328
   }
   curr_N_application_soil_observation_mode = &auto_N_mode_op;                   //060804
   return ES_START;                                                              //000526
}
//_perform_start_automatic_N_application_mode___________2012-07-10__2002-03-14_/
#endif
#endif

bool Land_unit_simulation_VX::respond_to_field_operation
(const std::string &NRCS_operation_description                                   //060802
,const Tillage_operation_struct::Oxidation_effect *oxidation_effect)
{  bool performed = false;                                                       //100404
   CropSyst::Soil_components *soil = mod_soil();                                 //160921_060803
   if (!NRCS_operation_description.length()) return true;                        //130710
      // return true because nothing to do
   #ifdef USE_NEW_RUSLE2
   //140917LML  added macro
   const char **composite_operations =  USDA::NRCS::RUSLE2::SDR_field_operations
      .lookup_composite_operations(NRCS_operation_description.c_str());          //130710
   if (composite_operations)                                                     //130710
   {  for (nat8 comp_op_i = 0; composite_operations[comp_op_i]; comp_op_i++)     //130710
         performed |= respond_to_field_operation
            (composite_operations[comp_op_i],oxidation_effect);                  //130710
   }
   else                                                                          //130710
   #endif
   {                                                                             //130710
   nat16 NRCS_SDR_operation_number = 0;                                          //060802
   NRCS_SDR_operation_number = USDA::NRCS::RUSLE2::SDR_field_operations          //060802
         .lookup_operation_number(NRCS_operation_description.c_str());
   if (NRCS_SDR_operation_number)                                                //060802
   {
      float64 disturbance_depth_avg_m = (float64)USDA::NRCS::RUSLE2::SDR_field_operations
         .get_disturbance_depth_avg_m(NRCS_SDR_operation_number);                //060807
      float64 mixing_fraction = USDA::NRCS::RUSLE2::SDR_field_operations
         .get_mixing_fraction(NRCS_SDR_operation_number);                        //060807
      Soil_tillage_effect *soil_tillage_effect = provide_soil_tillage_effect();  //060929
      if (soil_tillage_effect)                                                   //060929
          soil_tillage_effect->                                                  //060929
          respond_to_field_operation                                             //060725
               (disturbance_depth_avg_m                                          //060725
               ,mixing_fraction                                                  //060725
               ,USDA::NRCS::RUSLE2::SDR_field_operations.get_disturbance_rating(NRCS_SDR_operation_number) //060807
               ,oxidation_effect);                                               //100122
      if (organic_matter_residues_profile_kg_m2)                                 //060727
          organic_matter_residues_profile_kg_m2->respond_to_field_operation      //060727
            (disturbance_depth_avg_m                                             //060727
            ,mixing_fraction                                                     //060727
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_soil_disturbance_effect(NRCS_SDR_operation_number) //060727
            ,soil->get_disturbance() // may be 0 if not disturbance              //060807
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_residue_flatten_ratio(NRCS_SDR_operation_number)   //060727
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_residue_burial_ratio(NRCS_SDR_operation_number));  //060727
               // warning may need to pass burial (but dont forget to use 1-burial)
      if (NRCS_operation_description == "Seedbed finisher")                      //060802
      {  // The seedbed finisher operation is a dual operation                   //060802
         NRCS_SDR_operation_number        =         USDA::NRCS::RUSLE2::SDR_field_operations
            .lookup_operation_number(NRCS_operation_description.c_str());        //060802
         float64 disturbance_depth_avg_m = (float64)USDA::NRCS::RUSLE2::SDR_field_operations
            .get_disturbance_depth_avg_m(NRCS_SDR_operation_number);             //060807
         float64 mixing_fraction          =         USDA::NRCS::RUSLE2::SDR_field_operations
            .get_mixing_fraction(NRCS_SDR_operation_number);                     //060807
         if (soil_tillage_effect)                                                //060929
            soil_tillage_effect->                                                //060929
            respond_to_field_operation                                           //060802
               (disturbance_depth_avg_m                                          //060802
               ,mixing_fraction                                                  //060802
               ,USDA::NRCS::RUSLE2::SDR_field_operations.get_disturbance_rating(NRCS_SDR_operation_number)); //060807
         if (organic_matter_residues_profile_kg_m2)                              //060802
            organic_matter_residues_profile_kg_m2->respond_to_field_operation    //060802
            (disturbance_depth_avg_m                                             //060802
            ,mixing_fraction                                                     //060802
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_soil_disturbance_effect(NRCS_SDR_operation_number) //060802
            ,soil->get_disturbance() // may be 0 if not disturbance              //060807
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_residue_flatten_ratio(NRCS_SDR_operation_number)     //060802
            ,USDA::NRCS::RUSLE2::SDR_field_operations.get_residue_burial_ratio(NRCS_SDR_operation_number));  // warning may need to pass burial (but dont forget to use 1-burial)  //060802
      }
      if (NRCS_operation_description.find("mulch") != std::string::npos)         //100610
         adjust_mulching_if_applicable(NRCS_SDR_operation_number);               //100610
      #ifdef RUNOFF
      if (runoff)
          runoff->set_random_roughness_m(USDA::NRCS::RUSLE2::SDR_field_operations.get_roughness_initial_m(NRCS_SDR_operation_number));
      #endif
      performed = NRCS_SDR_operation_number;                                     //100404
   }
   else
   {  std::clog << "Warning unrecognized RUSLE operation:"
                << NRCS_operation_description << std::endl;                      //140515
   }
   }
   return performed;                                                             //060725
}
//_respond_to_field_operation_______________________________________1998-10-18_/
#ifdef MANAGEMENT
bool Land_unit_simulation_VX::process_any_automatic_clipping
(bool clipping_was_just_disabled)                                                //020604
{
   Crop_model_interface *crop_active =                                           //130719
   #ifdef INTERCROPPING
/*130719  I need to get the crop from the associated event
      dynamic_cast<Crop_interfaced *>(event->associated_object);                 //130719

assert(crop_active);  //130719 this line is temporary just to make sure the referencing is correct

      if (!crop_active) crop_active =
*/
         get_predominant_crop();                                                 //130719
         // this if statement is just a sanity check
   #else
      crop_active_or_intercrop;                                                  //130719
   #endif
   bool auto_clipping_harvest = false;                                           //040624
   if (curr_automatic_clip_mode)                                                 //020913
   {  Auto_clip_biomass_mode &clip_biomass_op = *(curr_automatic_clip_mode);
        bool remove_a_fixed_amount =                                             //051020
         clip_biomass_op.harvest_amount_mode== HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS; //051020
      float64 curr_canopy_biomass_kg_m2 =  crop_active->get_canopy_biomass_kg_m2(); //051020
      float64 reserve_biomass_kg_m2 = per_ha_to_per_m2(clip_biomass_op.reserve_biomass_kg_ha);        //051020
      float64 acceptable_remove_amount_kg_m2 = per_ha_to_per_m2(clip_biomass_op.remove_amount_kg_ha); //051020
      bool can_meet_all_demand = true; // applied only to remove_a_fixed_amount                       //051020
      bool can_meet_some_demand = true;                                          //051020
      if (remove_a_fixed_amount)                                                 //051020
      {  can_meet_all_demand = (curr_canopy_biomass_kg_m2                        //051020
                  - per_ha_to_per_m2(clip_biomass_op.remove_amount_kg_ha ))
                  >=  reserve_biomass_kg_m2;
         can_meet_some_demand=curr_canopy_biomass_kg_m2>=reserve_biomass_kg_m2;  //051020
         if (!can_meet_all_demand                                                //051020
            && (can_meet_some_demand && clip_biomass_op.accept_less))
             acceptable_remove_amount_kg_m2 =                                    //051020
               curr_canopy_biomass_kg_m2 - reserve_biomass_kg_m2;
      }                                                                          //051020
      auto_clipping_harvest =                                                    //970219
      crop_active->check_for_automatic_clipping(
          clip_biomass_op                                                        //051105
         ,acceptable_remove_amount_kg_m2                                         //051020
         ,clipping_was_just_disabled  // clip now or never
         /*200122
         #ifdef SOILR
             (float64)SoilR_adjust_relative_growth_rate_for_clipping
            ,(float64)per_ha_to_per_m2(SoilR_minimum_biomass_required_for_clipping)
            ,(float64)per_ha_to_per_m2(SoilR_biomass_forces_clipping)              // kg/m2 input file in (kg/ha) 970521c
            ,(float64)SoilR_biomass_removed_clipping
            ,SoilR_trim_to_residue
            ,(today.get_DOY() > SoilR_latest_date_to_clip.get_DOY())  // clip now or never
         #endif
         */
         );
   }
   return auto_clipping_harvest;
}
#endif
//_process_any_automatic_clipping___________________________________2001-02-02_/
#ifdef CROP_ORCHARD
Orchard_interface *Land_unit_simulation_VX::provide_orchard_interception_model
(Crop_parameters &crop_params)
{
   Orchard_common *orchard = 0; // optional submodel for fruit tree
      Orchard_common::Interception_architectural *interception = 0;
      const Smart_soil_parameters  *parameters_ownable =                         //170307
        #if (!((CROPSYST_VERSION >=5 ) || defined(USE_CS_LAND_UNIT)))
        ref_soil()                                                               //170307_161008
        #else
        soil
        #endif
        ->parameters_ownable;
      float64 terrain_steepness_percent =
         parameters_ownable                                                      //170307
              ? parameters_ownable->get_steepness_percent()                      //060803
              : 0.0;                                                             //170307
      if (crop_params.morphology.canopy_architecture)
      switch (scenario_control_and_model_options.orchard_solid_interception_model_clad.get())
      {  // Although the architecture interception is enabled in the Crop
         // the model is selected in the simulation control.
         // (the user may also at this point override and disable it).

         case Oyarzun_Stockle_orchard_interception  :
         {
            // Warning I am current just creating a new instance of sun_hours,
            // because this is crashing with the weather's instance
            const CS::Weather_hours *hourly_weather =
               meteorology.provide_hourly();                                     //151130
            if (hourly_weather)
            interception = new Interception_architectural_Oyarzun_Stockle
               (crop_params.layout                                               //110613
               ,crop_params.canopy_architecture                                  //110613
               ,terrain_steepness_percent
               ,scenario_control_and_model_options.site_aspect_N0_E90
               ,meteorology.ref_sun_days()                                       //151130
               ,meteorology.ref_sun()                                            //151129
               ,hourly_weather->ref_sun_hours()                                  //151224
               ,*hourly_weather
               ,meteorology.ref_sky().curr_cloud_cover_fraction                  //151128
               ,simdate                                                          //151129
               );
            //NYI else explaination that hourly weather is needed orchard model
         }
         break;
         case Kemanian_Stockle_orchard_interception :
            interception = new Interception_architectural_Kemanian_Stockle
               (crop_params.layout                                               //110613
               ,crop_params.canopy_architecture                                  //110613
               ,terrain_steepness_percent
               ,scenario_control_and_model_options.site_aspect_N0_E90
               ,meteorology.ref_sun_days()                                       //151130
               ,meteorology.ref_sky().curr_cloud_cover_fraction                  //151128
               ,simdate                                                          //151129
               ,meteorology.get_geocoordinate().get_latitude_radians());         //151118
         break;
         default : interception = 0; break; // no_orchard_interception           //091217
      }
      orchard = new Orchard_common
         (crop_params.morphology.light_extinction_coef                           //190625
         ,interception                                                           //200403
         ,crop_params.canopy_architecture                                        //110613
         ,crop_params.phenology.initiation.flowering
         ,crop_params.phenology.initiation.rapid_fruit_growth);
   return orchard; // May be 0 if not an orchard crop;
}
//_provide_orchard_interception_model_______________________________2007-11-17_/
#endif
bool Land_unit_simulation_VX::adjust_mulching_if_applicable
(nat16 NRCS_SDR_operation_number)
{  float64 cover_remove_fraction = 0.0;
   switch (NRCS_SDR_operation_number)
   { // Add mulch
      case   1 : // add mulch (assuming 100 %)
      case 133 : // Plastic mulch applic. 40 inch beds 100 percent cover
      case 135 : // Plastic mulch applic. 48 inch beds 100 percent cover
      case 137 : // Plastic mulch applic. 54 inch beds 100 percent cover
      case 139 : // Plastic mulch applic. 64 inch beds 100 percent cover
      case 141 : // Plastic mulch applicator 100 percent cover
         mulch_cover_fraction = 1.0; break;
      case 134 : // Plastic mulch applic. 40 inch beds 75 percent cover
      case 143:  // Plastic mulch applicator 75 percent cover
         mulch_cover_fraction = 0.75; break;    // add mulch
      case 140:  // Plastic mulch applic. 64 inch beds 85 percent cover
         mulch_cover_fraction = 0.85; break;    // add mulch
      case 136:  // Plastic mulch applic. 48 inch beds 80 percent cover
      case 138:  // Plastic mulch applic. 54 inch beds 80 percent cover
         mulch_cover_fraction = 0.80; break;    // add mulch
      case 142:  // Plastic mulch applicator 40 percent cover
         mulch_cover_fraction = 0.45; break;    // add mulch
      // remove mulch
      case 148 : // Plastic mulch, remove (assuming 100%)
         cover_remove_fraction = 1.0; break;
      case 147 : //  Plastic mulch, 50 percent  removal
         cover_remove_fraction = 0.50; break;
      case 113 : // mulch treader  is 25% removal
      case 146 : // Plastic mulch, 25 percent removal
         cover_remove_fraction = 0.25; break;
      case 145 : // Plastic mulch, 10 percent removal
         cover_remove_fraction = 0.10; break;
      case 144 : // Plastic mulch, 05 percent removal
         cover_remove_fraction = 0.05; break;
  }
   if (cover_remove_fraction > 0.0)
      mulch_cover_fraction -= mulch_cover_fraction * (cover_remove_fraction);
   return true;
}
//_adjust_mulching_if_applicable____________________________________2010-06-10_/
#ifndef YAML_PARAM_VERSION
// Note this may be done differently in V5
Event_status_indicator Land_unit_simulation_VX::perform_crop_adjustment
(const CropSyst::Crop_parameters::Adjustment &adjustment_event
,void *associated_with_obj)  // An event will be associated with a Crop)
{  Event_status_indicator status = ES_FAIL;
   CropSyst::Crop_parameters *associated_crop_param =
      (CropSyst::Crop_parameters *)(associated_with_obj);
   if (associated_crop_param)
   {
      std::string parameter(adjustment_event.parameter);
      size_t left_param_pos = parameter.find("[");
      size_t right_param_pos = parameter.find("]");
      if (left_param_pos != std::string::npos)
      {  size_t stripped_length = (right_param_pos - left_param_pos) - 1;
         std::string stripped_parameter(parameter,left_param_pos+1 ,stripped_length);
         parameter.assign(stripped_parameter);
      }
      FOR_EACH_IN(section,VV_Section,associated_crop_param->sections,each_section)
      {  //for each entry in current section:
         FOR_EACH_IN(entry,VV_abstract_entry,section->entries,each_entry)
         {  // lookup field in actual fields:
            std::string key; entry->key_string(key);
            if (key == parameter)
            {  entry->set_str(adjustment_event.value.c_str());
               status = ES_COMPLETE;
            }
         } FOR_EACH_END(each_entry)
      } FOR_EACH_END(each_section)
   }
   return status;
}
#endif
//_perform_crop_adjustment__________________________________________2012-07-19_/
Event_status_indicator Land_unit_simulation_VX::process_model_specific_event
(const Simple_event *event)
{  Event_status_indicator event_status = ES_FAIL;
   Crop_model_interface *crop_active =                                           //130719
   #ifdef INTERCROPPING
      dynamic_cast<Crop_model_interface *>(event->get_associated_object());      //130719
      if (!crop_active) crop_active = get_predominant_crop();                    //130719
         // this if statement is just a sanity check
   #else
      crop_active_or_intercrop;                                                  //130719
   #endif
   if (crop_active)
   {
      #ifdef YAML_PARAM_VERSION
      assert(false); // NYI  I intend to handle parameter adjustments differently in V5
      #else
      {  const CropSyst::Crop_parameters_class::Yield::Grain_stress_sensitivity_period *stress_period //200423
            = dynamic_cast<const CropSyst::Crop_parameters_class::Yield::Grain_stress_sensitivity_period *>(event); //200423
         bool iniciate_period = stress_period != NULL;
         Crop_complete *crop = dynamic_cast<Crop_complete *>(crop_active);
         if (iniciate_period && crop)                                            //150825
         {
            Yield_grain *grain_yield = dynamic_cast<Yield_grain *>(crop->yield); //150825
            if (grain_yield && grain_yield->temperature_extreme_reponse)         //150825
            {  grain_yield->temperature_extreme_reponse->add_period(*stress_period); //150825
               event_status = ES_COMPLETE;
            }
         }
      }
      #endif
   }
   return event_status;
}
//_process_model_specific_event_____________________________________2013-04-28_/
}//_namespace CropSyst_________________________________________________________/

