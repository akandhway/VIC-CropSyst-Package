#ifndef cs_modH
#define cs_modH

#if (CROPSYST_VERSION!=4)
#   error This class is only used in V4, remove from you project
#endif

//200524 #include "arguments_CropSyst.h"
#include "CS_scenario_directory.h"
//define AUXFILE
// put a # before DEFINE to enable the auxillary parameter file.

// This options.h needs to be for the current program being compiled
// is should not necessarily be /cropsyst/cpp/options.h
#include "options.h"
#include "cs_simulation.h"
#include "corn/math/statistical/stats.h"
#include "corn/const.h"
#include "land_unit_output_V4.h"
#include "geodesy/geolocation_item.h"
#include "CS_suite/application/CS_arguments.h"

using namespace std;

// GIS mode parameters
#define LABEL_init_PAW "init_PAW"
#define LABEL_init_NO3 "init_NO3"
//______________________________________________________________________________
namespace CORN       { class Data_source; }

class UED_graph_control;
class CropSyst_project;
class CropSyst_Model_options;
class Storm_database_or_generator;
class Soil_profile_spreadsheet_set;
class Polygon_simulation_parameters;
class CropSyst_DLY_report;
class CropSyst_YLY_report;
class CropSyst_GS_report;
class CropSyst_N_balance_report;
namespace STATSGO { class Database; }
enum value_types {value_str,value_int,value_num};
class management_class;
class Report_formats_parameters;
namespace CropSyst
{
   class Crop_model_interface;                                                   //190618
   //190618 class Crop_interfaced;                                               //050208
   class Simulation_override_record;
}
//______________________________________________________________________________
namespace CropSyst_V4 {                                                          //151003
//______________________________________________________________________________
class  CropSyst_Model
: public Cropping_system_simulation                                              //030616
{
   //200815 CS::Identification ID;                                                        //160312
   #ifdef DETAIL_DENITR
   class Detail_denitr_date_range
   : public extends_ CORN::Item
   {
   public:
      CORN::date32 period_start;
      CORN::date32 period_end;
   public:
      inline Detail_denitr_date_range
         (CORN::date32 period_start_,CORN::date32 period_end_)
         : CORN::Item()
         , period_start (period_start_)
         , period_end   (period_end_)
         {}
      inline bool is_in_range(CORN::date32 today)            const{return true;}
      inline virtual bool is_key_nat32(nat32 key)                   affirmation_ //180820
      { return period_start == key; }
      inline virtual nat32 get_key_nat32()           const{return period_start;}

   };
   //______________________________________________________________________________
   Enumeration_list detail_denitr_dates;
   #endif
 protected:
   CS::Land_unit_meteorological *meteorology;                                    //160311_151128
 public:
   CropSyst::Land_unit_with_output_V4 *land_unit_with_output; // Note that this is also the scenario reference. //141206_020308
      // Actually land_unit_with_output is given to
      // Cropping_system_simulation::LBF_sims which will delete
 public: // Temporarliy public so main program can disable the fast graph if needed
   std::wstring control_file_name_unqual;                                        //151203
   //200525 CropSyst::Arguments    arguments;                                             //160312
   //191104 CropSyst::Arguments_V4 arguments;                                             //160312
   //200815 CS::Arguments arguments;                                                      //200524
   //200815 CS::Arguments &arguments;                                                     //200815
   CropSyst::Scenario_directory scenario_directory;                              //200525

   CropSyst::Scenario scenario_control_and_model_ops;                            //160311
   CropSyst_Model_options       &active_model_options;                           //151203_020531  //130828 was a pointer now using ref because required

   // note that active_model_options model options is probably obsolete because
   // we now keep reference to scenario_control_and_model_ops

   // The model options can come either from the scenario or the project.
   // The project model options are used in GIS, LADSS, and CANMS modes.
/*191104 now just use verbosity
 public:
   bool display_full;
   //160311 now using verbose bool batch_mode;                                                              //970402
  // The next variables are put here for now,
  //   They are temporary values used mainly for output.
*/
 public:  //040815       This is special output for Oumarou's carbonsequestration and grazing regimens model
   CORN::statistical::Descriptive_progressive  grazed_biomass_stats_kg_ha_check_used;    // We currently use the total                                           //040812
   CORN::statistical::Descriptive_progressive  carbon_sequestration_stats_profile_kg_m2; // We currently use the final (current at the end of the simulation)    //040812
   CORN::statistical::Descriptive_progressive  carbon_sequestration_stats_30cm_kg_m2;    // We currently use the final (current at the end of the simulation)    //040826
   CORN::statistical::Sample_dataset_clad<float32,float32,float32> seasonal_grazing_stats_kg_ha;             // Record of all seasons                                                //040815
   CORN::statistical::Sample_dataset_clad<float32,float32,float32> seasonal_ungrazed_stats_kg_ha;            // Record of all seasons                                                //040815
   CORN::statistical::Sample_dataset_clad<float32,float32,float32> seasonal_biomass_production_stats_kg_ha;  // Record of all seasons                                                //050107
   CORN::statistical::Sample_dataset_clad<float32,float32,float32> seasonal_grazing_req_stats_kg_ha;         // Record of all seasons                                       //050107
   CORN::statistical::Descriptive_progressive  season_grazing_req_stats_kg_ha;           // For the current season                                      //050107
   nat16                     days_not_viable;                          // This is the number of days the simulation was marked not viable      //040826
   bool  viable;  //151203 this is here temporarily would be moved to Simulation_engine
 protected: // objects shared by all the simulations
   #ifdef PRINT_PROFILE
   void setup_profile_print_events(const CORN::Date  profile_print_date_data[]   //000526
      ,CORN::Year sim_start_year, CORN::Year sim_end_year );                     //051201
   bool record_soil_profile_today;
   #endif
   geodesy::Geocoordinate_item geocoordinate; // rename to geolocation;          //191010_160311
 protected: // GIS mode
   Data_source *GIS_override_table;                                              //010516
   CropSyst::Simulation_override_record *GIS_record;                             //050921
 public:
   CropSyst_Model                                                                //010202
      (CS::Arguments &arguments_);                                               //200815
      //100815 (Command_options_data_source &command_line_options_);                      //160829_170225_041030
   virtual ~CropSyst_Model();
   inline virtual Location_parameters *load_location_parameters()         const  //020613
      { return 0; } // CropSyst now less CS_simulation load location and weather as needed  //030911
   #ifdef TEMP_DISABLED
   //130828  I Need reimplement this of USE_OS_FS
   #ifndef __linux
   // I haven't yet implemented this for __linux, anyway in V5 GIS methods will probably be implemented as a derived class
   virtual void get_parameters_from_GIS_table(const char *GIS_override_table,nat32 LBF_ID); //021208
   #endif
   #endif
 protected: // Simulation_engine overrides
   virtual const CORN::OS::Directory_name &provide_scenario_directory_name() provision_;//151203
   virtual const std::wstring &get_control_file_name_unqual()             const; //151203
 protected: // simulation overrides
   virtual Event_status_indicator perform_operation                              //020308
      (Common_operation &op
      ,void *associated_obj);                                                    //021109
   virtual bool end_operation_period
      (Common_operation &op, const Date_const &date_period_ends);                //020504
 public: // simulation and land block fragment overrides
   virtual bool start()                                           modification_; //030106
      ///< Start recording to output file
      ///< \return true if start successful
   virtual bool start_day()                                       modification_; //151128
   virtual bool process_day()                                     modification_; //150611_020308
      ///< \return true if successful
   virtual bool end_day()                                         modification_; //050115
      ///< \return true if successful
   virtual bool stop()                                            modification_; //131011
      ///< Stop recording to output file
      ///< \return true if stop successful
   virtual bool end_year()                                        modification_; //010202
      ///< \return true if successful
   inline virtual modifiable_ CS::Arguments *mod_arguments()      modification_  //160311
      { return &arguments; }
   inline virtual CS::Arguments *ref_arguments()                           const //180131
      { return const_cast<CS::Arguments *>(&arguments); }                        //200524
   virtual Common_operation *get_operation(const char *operation_type_str        //030912
      ,const std::string &operation_ID_or_filename                               //030912
         // filename may be updated with qualification so not const
         // (does not apply to cropsyst)
      ,modifiable_ CORN::OS::File_name &operation_filename                       //121121_041215
         // the fully qualified operation filename may be returned
         // (currently CropSyst does not do this)
      ,int32 resource_set_ID,bool &relinquish_op_to_event);                      //030912
   inline virtual const OS::Directory_name &get_output_directory_name()   const;
   // In V4 the scenario and control parameter are in the same object
   virtual const       CS::Simulation_control &ref_simulation_control()   const { return scenario_control_and_model_ops; };
   virtual const       CS::Simulation_control &ref_simulation_scenario()  const { return scenario_control_and_model_ops; };
   virtual modifiable_ CS::Simulation_control &mod_simulation_control()  modification_ { return scenario_control_and_model_ops; };
   virtual modifiable_ CS::Simulation_control &mod_simulation_scenario() modification_ { return scenario_control_and_model_ops; };
 public:
   virtual bool initialize()                                    initialization_;
   bool initialize_soil()                                         modification_; //131113
      ///< \return true if initialization successful
   void process_interrupt();
   void setup_initial_soil_profile_recalibrations();                             //011127
   virtual void record_growing_season
      (CropSyst::Crop_model_interface &crop_to_output);                          //030516_131008
 protected:
   nat16 desire_AgMIP_variables()                               initialization_; //160312
};
//_CropSyst_Model______________________________________________________________/
}//_namespace_CropSyst_V4_______________________________________________________
#endif

