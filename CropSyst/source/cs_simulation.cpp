#include "cs_simulation.h"
#include "common/weather/weather.h"
#include "weather/storms.h"
#include "crop/crop_interfaced.h"
#include "corn/data_source/vv_file.h"
#ifdef _Windows
#  include "common/weather/database/weather_DAT_GEN_database.h"
#endif

#include "common/weather/database/weather_UED_database.h"
#include "common/weather/temperature_amplitude_phase_calibrator.h"
#include "land_unit_sim.h"
#include "corn/OS/directory_entry_name_concrete.h"
#include "corn/OS/file_system_engine.h"
#include "corn/application/explaination/explainations.h"
#include "common/solar/irradiance_extraterrestrial.h"
#include "common/weather/atmospheric_CO2_change.h"

#if ((CS_VERSION > 0) && (CS_VERSION <5) && (!defined(__linux) && (!defined(OFOOT_VERSION))))
#  define SUPPORT_OBSOLETE_LOC_FILE
#endif
//______________________________________________________________________________
namespace CropSyst
{  static const std::string model_name("CropSyst");
}
namespace CropSyst_V4                                                            //151003
{
//______________________________________________________________________________
Cropping_system_simulation::Cropping_system_simulation
(modifiable_  CS::Simulation_control &simulation_control_unused                  //151203
//100815 ,Command_options_data_source &command_line_options_
,CS::Arguments &arguments_)                                                      //200815_160829
: CS::Simulation_engine
   (arguments_)                                                                  //200815
//200815 (command_line_options_)                                   //160829_151203
, LBF_sims(simulation_units)                                                     //151203
, scenario_extension(L"CropSyst_scenario")                                       //160121
{}
//_Cropping_system_simulation:constructor___________________________2003-09-12_/
bool Cropping_system_simulation::add_land_block_fragment
(CropSyst::Land_unit_simulation_VX *LBF_sim)                       modification_ //141206
{  LBF_sims.append(LBF_sim);                                                     //141203
   return true;
}
//_add_land_block_fragment__________________________________________2003-09-12_/
bool Cropping_system_simulation::start_day()
{  bool started = true;
   FOR_EACH_IN(loc_weather,Location_weather,locations_weather,each_loc_weather)
   {
      started &= loc_weather->weather_extension->start_day();                    //150125
      // actually I think  weather_extension already references today (if not see if we can)
      started &= loc_weather->meteorology->start_day();                          //151203
   } FOR_EACH_END(each_loc_weather)
   // while normally I start superclasses first we need to have the weather
   // started/loaded before we can start subelements (LBFs)
   started &= CS::Simulation_engine::start_day();                                //151203_150501
   return started;                                                               //050115
}
//_start_day________________________________________________________2003-09-12_/
bool Cropping_system_simulation::initialize()                    initialization_ //151203
{  // 170626 at this point preinitialize will have loaded scenario and control parameters
   FOR_EACH_IN(sim_item,CS::Simulation_element,LBF_sims,each_LBF)                //170804
   {
      CropSyst::Land_unit_simulation_VX *LBF_sim_item =                          //170804
         dynamic_cast<CropSyst::Land_unit_simulation_VX *>(sim_item);            //170804
      if (LBF_sim_item)                                                          //170804
      LBF_sim_item->load_profile_recal_events                                    //121119
      (LBF_sim_item->scenario_control_and_model_options.recalibration_filename.c_str()); //130827
         // I dont want load_profile_recal_events here, it should be
         // moved to an earlier initalize() method

      //std::clog << "Warning: need to load global events from land unit schedule" << std::endl;
      // NYI (check if still needed) 160706
      // event_queue->transfer_all_from(LBF_sim_item->scenario_control_and_model_options.parameter_event_list); //130827
   } FOR_EACH_END(each_LBF)
   bool initialized = CS::Simulation_engine::initialize();
   // For CropSyst V4 this event scheduler would handle things like
   // report output scheduling
   // Possibly also recalibration
   // NYI create new master event scheduler
   return initialized;
}
//_initialize_______________________________________________________2015-12-03_/
bool Cropping_system_simulation::start()                           modification_
{
   bool started = true;
   FOR_EACH_IN(sim_item ,CS::Simulation_element ,LBF_sims,each_LBF)              //170804
   {
      #if (CROPSYST_VERSION >= 5)
      CropSyst::Land_unit_simulation_VX *LBF_sim_item =                          //170804
         dynamic_cast<CropSyst::Land_unit_simulation_VX *>(sim_item);            //170804
      if (LBF_sim_item)                                                          //170804
      {
      // This was moved from run()
      LBF_sim_item->load_profile_recal_events
         (LBF_sim_item->scenario_control_and_model_options.recalibration_filename.c_str()); //161017_121119
         // I dont want load_profile_recal_events here, it should be
         // moved to an earlier called initalize() method
      event_queue->transfer_all_from(LBF_sim_item->event_list);
      }
      #endif
   } FOR_EACH_END(each_LBF)
   started &= CS::Simulation_engine::start();                                    //151203
   return started;
}
//_start____________________________________________________________2003-09-12_/
nat16 Cropping_system_simulation::process_triggered_syncronizations()
{  nat16 syncs = 0;
   FOR_EACH_IN(sim_item,CS::Simulation_element,LBF_sims,each_LBF)                //170804
   {
      CropSyst::Land_unit_simulation_VX *LBF_sim_item =                          //170804
         dynamic_cast<CropSyst::Land_unit_simulation_VX *>(sim_item);            //170804
      if (LBF_sim_item)                                                          //170804
      syncs += LBF_sim_item->perform_triggered_synchronizations_today();
   } FOR_EACH_END(each_LBF)
   return syncs;
}
//_process_triggered_syncronizations________________________________2010-10-04_/
Cropping_system_simulation::Location_weather::Location_weather
(
#if (CROPSYST_VERSION == 4)
 const char                   *location_filename_
,Location_parameters          *location_,
 #endif
 Weather_database             *weather_DB_                                       //050307
,Weather                      *weather_extension_                                //060922
,CS::meteorology::Storms      *storms_                                           //140217
,CS::Land_unit_meteorological *meteorology_                                      //151203
)
: Item            ()
#if (CROPSYST_VERSION == 4)
,location_filename(location_filename_)
,location         (location_)
#endif
,weather_DB       (weather_DB_)                                                  //050307
,weather_extension(weather_extension_)                                           //050307
,storms           (storms_)                                                      //140217
,meteorology      (meteorology_)
{}
//_Location_weather:constructor________________________________________________/
Cropping_system_simulation::Location_weather::~Location_weather()
{
   #if (CROPSYST_VERSION == 4)
   delete location;
   #endif
/* 151202 crashing
   delete weather_DB;                                                            //050307
*/
}
//_Location_weather:descturcto______________________________________destructor_/
#if (CS_VERSION == 4)
bool Cropping_system_simulation::Location_weather::is_key_string
(const std::string &key)                                            affirmation_ //180820
{
   assert(false); // check if this method is still used
   return location_filename == key;
}
/*
const char *Cropping_system_simulation::Location_weather::get_key()        const
{ return location_filename.c_str();
   // actually the location is not necessarily unique.
   // Several sites may use the same weather
   // This should return the ID
}
*/
//_Location_weather::is_key_string:_________________________________2013-04-06_/
#else
bool Cropping_system_simulation::Location_weather::is_key_wstring
(const std::wstring &key)                                           affirmation_ //180820
{
   assert(false); // check if this method is still used
   return weather_DB==key;
}
#endif
//_Location_weather::is_key_wstring_______________________________________2018_/
#ifndef USE_CS_LAND_UNIT

bool Cropping_system_simulation::setup_location
(CropSyst::Land_unit_simulation_VX *to_lbf                                       //141206
,modifiable_ Geocoordinate       &geocoordinate_                                 //151208
#if (CS_VERSION==4)
,const char                      *location_or_UED_filename
#else
,CORN::OS::File_name             &weather_database_filename                      //130406
#endif
,Location_parameters             *location_to_use  // relinquished to this specified for LADDS otherwise 0
,bool                             extend_available_weather                       //150218
,Snow_pack_model                  snowpack_model
,float32                          soil_steepness
,bool                             hourly                                         //041111
)
{  // Lookup to_lbf in LBF_sims if not found return false
   // lookup  i_location_filename_or_ID in location_weather;
   // If exists then assign_environment() must be called after add_land_block_fragment() and before process_day()
   // If location is specified, this pointer is relinquished by Location weather
   // If the location already loaded then location is deleted.
   // If location_to_use is not specified, then location is created and loaded
   // Returns true if new location weather added
   // Other wise return false indicating the location does not require additional setup
   bool setup = true;                                                            //151115
   CropSyst::Land_unit_simulation_VX  *existing_LBF = 0;                         //141206
   FOR_EACH_IN(LBF_sim_item ,CORN::Item ,LBF_sims,each_LBF)                      //170703
   {
      CropSyst::Land_unit_simulation_VX *as_LBF_sim_item =
         dynamic_cast<CropSyst::Land_unit_simulation_VX *>(LBF_sim_item);
      if (as_LBF_sim_item == to_lbf)
         existing_LBF = to_lbf;
   } FOR_EACH_END(each_LBF)
   if (!existing_LBF) return false;
   Location_weather *existing_loc_weather = (Location_weather *)locations_weather.
      #if ((CS_VERSION > 0) && (CS_VERSION <5))
      find_cstr(location_or_UED_filename );                                      //120921_031023
      #else
      find_wstr(weather_database_filename.w_str());                              //130406
      #endif
   if (existing_loc_weather)
      delete location_to_use;
   else
   {  const Geocoordinate &lbf_geocoord = existing_LBF->meteorology.get_geocoordinate(); //151208
      // currently geocoordinate_ and meteorology geocoorinate should be the
      // same at this point (geocoordinate_ is modifiable)
      Weather_database *weather_database = 0;                                    //050307
      CORN::OS::File_name_concrete weather_database_filename
         #if (CS_VERSION==4)
         (location_or_UED_filename); // First presuming the passed filename is a UED file
         #else
         (weather_database_filename);                                            //150907
         #endif

      #ifdef SUPPORT_OBSOLETE_LOC_FILE
      // The location file is obsolete in version 5
      CORN::OS::File_name_concrete location_fname(location_or_UED_filename);
      if (!location_to_use)
      {  if (location_fname.has_extension_ASCII("loc",true))
         {  location_to_use = new Location_parameters("",true);
               // weather filename will be loaded
            //store location parameter only for backward compatibility
            VV_File location_file(location_fname.c_str());
            setup = location_file.get(*location_to_use);
            CORN::OS::File_name//_concrete
               &database_or_weather_filename = location_to_use->get_database_or_weather_filename();
            #ifdef _Windows
            if (CORN::OS::file_system_engine.exists(location_fname))             //150315
            {
               std::cerr << "Support of old location files has been disabled.  contact rnelson@wsu.edu if this is still needed" << std::endl;
               /* 151128 If I want to support old LOC files then I need to implement get_to()

               weather_database = new Weather_data_text(*location_to_use,database_or_weather_filename );
                  //150125 ,existing_loc_weather->weather_extension->SKY_ local_date_time;  //091217
               // In version 5 The old style daily text weather data files are no longer supported (Use only UED files) 061119
            // the loaded location parameters will be used even if
            // the weather database has location parameters the
            // specified location file parameters have precedence.
               geoloc = weather_database->get_geolocation_constant();            //100218
               */
            } else
               if (database_or_weather_filename.has_extension_ASCII("UED",true)) //130808
                 weather_database_filename.set_DEN(database_or_weather_filename);//121126_050308
            #endif
         }
      #endif
         if (!weather_database)  // wasn't a GEN or DAT file set                 //050307
         {  Weather_UED_database *weather_UED_database=new Weather_UED_database
              (weather_database_filename.c_str(),std::ios_base::in,CORN::measured_quality);//150811_050307

// std::clog << "WARNING: init soil temperature calibration disabled for debug" << std::endl;

//std::clog << "DB:" << weather_database_filename.c_str() << std::endl;


            if (!existing_LBF->scenario_control_and_model_options.annual_temperature.is_calibrated()) //150430
            {  // Do this before enable substitute dates because we dont need
               // to calibrated using substituted date [The calibrator wouldn't actually do that].
               CS::Temperature_amplitude_and_phase_calibrator annual_temperature_calibrator;         //150430
               annual_temperature_calibrator.load_daily_temperature_from_UED
                  (weather_UED_database->ref_database_file());
               annual_temperature_calibrator.calibrate                           //150430
                  (existing_LBF->scenario_control_and_model_options.annual_temperature.avg
                  ,existing_LBF->scenario_control_and_model_options.annual_temperature.phase
                  ,existing_LBF->scenario_control_and_model_options.annual_temperature.avg_half_amplitude);
            }

#ifdef TEMP_DISABLED
            if (existing_LBF->scenario_control_and_model_options.substitute_year_base
               && existing_LBF->scenario_control_and_model_options.extend_available_weather)//170323
               weather_UED_database->
               database_file->enable_substitution
               (existing_LBF->scenario_control_and_model_options.substitute_year_base     //170323
               ,existing_LBF->scenario_control_and_model_options.start_date.get_year()    //170331
                  //// substitute reference year
               ,existing_LBF->scenario_control_and_model_options.substitute_year_offset); //161207
            // if not needed because is pointer if (existing_LBF->scenario_control_and_model_options.xxx)
/*abandonded working on a more generalized adjusting idiom
            weather_UED_database->know_climate_adjustments                       //181011
                  (existing_LBF->scenario_control_and_model_options.climate_adjustments);
*/
            if (existing_LBF->scenario_control_and_model_options.weather_preclude_date)   //170323
            {
               CORN::date32 actual_preclude_date                                 //170310
               = (existing_LBF->scenario_control_and_model_options.weather_preclude_date > 1000000)
               ?  existing_LBF->scenario_control_and_model_options.weather_preclude_date
               : existing_LBF->scenario_control_and_model_options.start_date.get_year() * 1000
               + existing_LBF->scenario_control_and_model_options.weather_preclude_date;
/*tempdisalbed
               weather_UED_database->database_file->enable_preclusion
                  (existing_LBF->scenario_control_and_model_options.weather_preclude_date);
*/
            }
#endif
            weather_database = weather_UED_database;                             //050307
            if (location_to_use)                                                 //050307
            {  // This is for legacy simulation scenarios where the location specified the UED file and the UED file did not have geolocation   //050817
               bool UED_has_geolocation = weather_UED_database->
               #ifdef __unix
                  ref_geolocation_record();                                      //050817
               #else
                  get_geolocation_constant();                                    //050817
               #endif
               if (!UED_has_geolocation)                                         //050817
                  weather_UED_database->use_location_parameters(location_to_use);//050817
               // The following if statement is for old CropSyst locations where the .LOC file does not have the same name as the .UED file 050916
               if (!weather_UED_database->get_location_parameters()) // The UED file does not have a matching .LOC location file //050916
                  weather_UED_database->use_location_parameters(location_to_use); // the LOC file specifies the UED file, but they dont have the same name. 050916
            } else                                                               //050307
            {  // This is the case when the UED is specified in the simulation
               // control use the UED location parameters (if any)
               const UED::Geolocation_record  *UED_loc_rec =
               weather_UED_database->ref_geolocation_record();                   //100218
               if (UED_loc_rec)
               {
                   #if (CS_VERION==4)
                   location_to_use = new Location_parameters(*UED_loc_rec,CS_VERSION_NUMBERS);
                   #else
                   // WARNING  will need to get location from some other source
                   #endif
                   bool geocoor_valid = UED_loc_rec->has_valid_geocoordinates(); //150501
                   if (geocoor_valid)
                   {
                     if (!lbf_geocoord.is_valid())                               //151208
                     {  // Then take geocoordinate from the weather database
                        geocoordinate_.copy_from_geocoordinate(*UED_loc_rec);
                     }
                   } else UED_loc_rec = 0;                                       //150501
               }
            }
         }
      #ifdef SUPPORT_OBSOLETE_LOC_FILE
      // The location file is obsolete in version 5
      }
      #endif
      if (geocoordinate_.get_elevation() < 0.0)
          geocoordinate_.set_elevation_meter(global_elevation_mean_meter);
          // negative elevation is deemed invalid.
          // also AgMIP uses -99 as missing indicator.
      float64 longitude_radians = lbf_geocoord.get_longitude_radians();          //151208_071116
      float64 standard_meridian_radians = longitude_radians; // Currently we do not have standard meridian in geolocation so simply using the longitude. 071116
      CS::Land_unit_meteorological &meteorology
         = const_cast<CS::Land_unit_meteorological &>
            (existing_LBF->meteorology);
      Sun_days *sun_days =
          meteorology.sun_days                                                   //151201
            = new Sun_days( longitude_radians, standard_meridian_radians);       //071116
      Sun_hours *sun_hours = meteorology.sun_hours = new Sun_hours               //151201
         (*sun_days,lbf_geocoord.get_latitude_dec_deg());
      CS::Solar_radiation_extraterrestrial_hourly *ET_solar_irrad               //151215
         = new CS::Solar_radiation_extraterrestrial_hourly
            (lbf_geocoord,*sun_days,*sun_hours,simdate);
      meteorology.submit_solar_radiation_extraterrestrial                       //151215
         (ET_solar_irrad);
      Weather_provider *weather_provider =                                       //151221
         new Weather_provider
               (simdate,lbf_geocoord,*ET_solar_irrad
               ,weather_database->get_location_parameters());
      // At this point there should be weather_provider.location

      if (weather_provider->location_owned)
      {  // V4 still allows ET (and other weather element) calculation.
         // In the case of Priestly Taylor we need VPDmax calculated
         // and this needs aridity factor.
         // Normally we would expect the user to have used ClimGen to determine
         // the aridity factor (so there would be location file).
         // When not using ClimGen we all the user to specify
         // aridity factor in the scenario file (model options).
         weather_provider->VPDmax_parameters.copy(existing_LBF->scenario_control_and_model_options.VPDmax_parameters);
      }
      meteorology.submit_weather_provider(weather_provider);                     //151215
      Weather *weather_extension = new Weather                                   //060922
      (weather_database                                                          //050307
      ,*weather_provider // as cognate_
      ,lbf_geocoord
      ,2.0 //NYI lbf_geoloc->get_screening_height()                              //151128
      ,simdate
      ,sun_days   // relinquished to weather                                     //071116
      ,hourly                                                                    //041110
      ,true // treat all valid data as real       //081109 Need to trace down why true is not working?
      ,true);  // read only                                                      //010207
      UED_File_name storms_filename("storms.UED");
      //we can always check for existing storms file even in LADSS

      /*150217  Storms temp disabled, checking problem in filename, needs to fix

      #if (!defined(__linux) && !defined(OFOOT_VERSION))
      //130808 I haven't yet implemented this in Linux, I will probably redo this for version 5
      {  CORN::OS::File_name_concrete location_or_UED_filename_DEN(location_or_UED_filename); //131011
         CORN::OS::File_name_concrete named_storms_filename(location_or_UED_filename_DEN,CORN::OS::Extension("UED")); //131011
         if (CORN::OS::file_system_engine.exists(named_storms_filename))
             storms_filename.set_DEN(named_storms_filename);                     //121126
         // Note, in V5 it might be better to have a
         // separate file for daily data and subdaily data
         // because the storm data might be exclusively generated data.
      }
      #endif
      */
      Minutes water_entering_soil_timestep_minutes = 60;                         //160511 070116
      const float32*  mean_peak_rainfall
         = existing_LBF->scenario_control_and_model_options.mean_peak_rainfall;
      CS::meteorology::Storms *storms = (CORN::OS::file_system_engine.exists(storms_filename))
         ? (CS::meteorology::Storms *)new CS::meteorology::Storms_database
               (simdate_raw                                                      //170525
               ,weather_extension->curr_precipitation                            //151027
               ,mean_peak_rainfall
               ,storms_filename)
         : (weather_extension->weather_data->get_location_parameters() &&
             mean_peak_rainfall != 0)
            ? (CS::meteorology::Storms *)new CS::meteorology::Storms_hyeotographic
               (simdate_raw                                                      //170525
               ,weather_extension->curr_precipitation                            //151027
               ,mean_peak_rainfall
               ,existing_LBF->scenario_control_and_model_options.default_event_duration_minutes
               ,false)
            : (CS::meteorology::Storms *)new    CS::meteorology::Storms_uniform
               (simdate_raw                                                      //170525
               ,weather_extension->curr_precipitation                            //151027
               ,mean_peak_rainfall
               ,false
               ,water_entering_soil_timestep_minutes
               ,(Minutes)existing_LBF->scenario_control_and_model_options.default_event_duration_minutes);
      Location_weather *new_loc_weather = new Cropping_system_simulation::Location_weather
         (
         #if (CS_VERSION==4)
          location_or_UED_filename
         ,location_to_use,
         #endif
         weather_database,weather_extension,storms                               //140218
         ,&meteorology);                                                         //151203
      locations_weather.append(new_loc_weather);
      meteorology.know_weather_spanning(*weather_extension);                     //151215
      meteorology.take_storms(storms);                                           //151215
      // meteorology will provide hourly weather as needed
      if (existing_LBF->scenario_control_and_model_options.simulate_CO2)
      {
         Atmospheric_CO2_change_element *atmospheric_CO2_change                  //171207
         = meteorology.instanciate_CO2                                           //171207_151130
            (existing_LBF->scenario_control_and_model_options.recalibration_filename
            ,existing_LBF->scenario_control_and_model_options.initial_CO2_conc   //160531
            ,existing_LBF->scenario_control_and_model_options.annual_CO2_change  //160531
            ,ref_start_date()                                                    //151203
            ,ref_stop_date());                                                   //151203
         simulation_units.take(atmospheric_CO2_change);                          //171207
      }
   }
   return setup;
}
//_setup_location___________________________________________________2003-09-12_/
const std::string &Cropping_system_simulation::get_model_name()            const
{ return CropSyst::model_name; };
}//_namespace_CropSyst_V4______________________________________________________/
//_get_model_name___________________________________________________2012-09-25_/
#endif


