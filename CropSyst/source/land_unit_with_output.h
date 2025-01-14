#ifndef land_unit_with_outputH
#define land_unit_with_outputH

//#error obsolete_191212

#include "options.h"
#include "cs_UED_season.h"
#include "cs_UED_annual.h"
#include "cs_UED_harvest.h"
#include "cs_UED_db.h"
#ifndef CS_INSPECTOR_VERSION
#include "cs_UED_daily.h"
#endif
#ifndef REPORTS_IN_CS_MOD
#  if ((CS_VERSION > 0) && (CS_VERSION < 5))
#        include "cs_report_V4.h"
#  endif
#endif

#include "land_unit_sim.h"
//moved to project defines #define EXTENDS_LAND_UNIT_OUTPUT
#ifdef EXTENDS_LAND_UNIT_OUTPUT
#include "land_unit_output.h"
#else
extern std::ofstream *SCS_runoff_dat_file;                                       //131216
enum  CropSyst_period
{SIMULATION_PERIOD,GROWING_SEASON_PERIOD,GROWTH_STAGE_PERIOD,MANAGEMENT_PERIOD};
#endif

#define OUTPUT_NONE     0
#define OUTPUT_COMPLETE 0xFFFFFFFF
#define OUTPUT_ANNUAL   0x00000001
#define OUTPUT_SEASONAL 0x00000002
#define OUTPUT_HARVEST  0x00000004
#define OUTPUT_DAILY    0x00000008


namespace CropSyst {                                                             //141206
//______________________________________________________________________________
class Land_unit_with_output
: public extends_ Land_unit_simulation
#ifdef EXTENDS_LAND_UNIT_OUTPUT
, public extends_ Land_unit_output                                               //170315
#endif
{
#ifndef EXTENDS_LAND_UNIT_OUTPUT
   nat32 output_flags;                                                           //151227
      // For GIS and regional simulations,
      // this indicates that we want to output daily data an other details.
 public:
   UED::Database_file_indexed *harvest_UED_db;                                   //041004
   Harvest_data_source        *harvest_data_src_rec;                             //041004
   UED::Database_file_indexed *season_UED_db;
   Season_data_source         *season_data_src_rec;
   UED::Database_file_indexed *annual_UED_db;
   Annual_data_source         *annual_data_src_rec;
 private:
   datetime64                  latest_sync_date;                                 //041012
      // latest_sync_date was the date of the last triggered sync
   Normal_crop_event_sequence  latest_growth_stage_entered;                      //130925_041012
 private:
   #ifndef CS_INSPECTOR_VERSION
   #if ((CROPSYST_PROPER > 0) && (CROPSYST_PROPER <= 5))
   CropSyst_UED_database               *daily_UED_db;                            //030715
   CropSyst::CS_UED_tuple_data_source  *daily_UED_data_src;                      //030715
   CropSyst::CS_daily_data_record       daily_data_rec;                          //030715
   #endif
   #endif
#endif
 protected:

   // The could actually be float32, but I dont have CS::Parameter_float32 implemented yet

   float32 AgMIP_soil_water_observed;                                            //181101
   float32 AgMIP_soil_water_rootzone;                                            //181101
   float32 AgMIP_soil_N_observed;                                                //181101
   float32 AgMip_soil_N_rootzone;                                                //181101
/* abandoned
   // This is temporary, V5 will used inspection
   CS::Parameter_float64 AgMIP_soil_water_observed_parameter;                    //181101
   CS::Parameter_float64 AgMIP_soil_water_rootzone_parameter;                    //181101
   CS::Parameter_float64 AgMIP_soil_N_observed_parameter;                        //181101
   CS::Parameter_float64 AgMip_soil_N_rootzone_parameter;                        //181101
*/

 public: // constructor
   Land_unit_with_output
      (const CS::Identification &LBF_ID_                                         //151026
      ,nat32                         cycle_shift_                                //151026
      ,CropSyst::Scenario           &scenario_control_and_model_options_         //130827
      ,const CORN::date32 &today_                                                //170525
      ,const Geocoordinate          &geocoordinate_                              //151116
      ,const CS::Land_unit_meteorological &meteorology                           //151128
      #if (CS_VERSION==4)
      ,Common_simulation_log        &_event_log
      #endif
      ,const CropSyst::Scenario_directory *scenario_directory_                   //050619
      //190204 now use optation_global ,CS::Desired_variables  *desired_vars_ = 0
      );                               //170225_041021
         // pass 0 if no variables to record
 public:
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual nat32 perform_triggered_synchronizations_today();                     //131004
   #endif

   #ifdef EXTENDS_LAND_UNIT_OUTPUT
   inline virtual bool is_valid()                                   affirmation_
      { return Land_unit_simulation::is_valid(); } // this is hack probably should return CS::Simulation_element_abstract::is_valid();
   inline virtual bool start_day()                                 modification_
      { return Land_unit_simulation::start_day(); }
   inline virtual bool process_day()                               modification_
      { return Land_unit_simulation::process_day(); }
   inline virtual CS::Emanator *render_inspectors
      (CORN::Container      &inspectors
      ,const CS::Emanator  *context
      ,const std::string   &instance)                                 rendition_
      {  return Land_unit_simulation::render_inspectors
            (inspectors,context,instance);
      }
   #else

   #endif
   virtual bool start()                                           modification_; //130620_080306
   virtual bool start_year()                                      modification_; //130620_010202
   virtual bool start_growing_season                                             //160324
      (modifiable_ CropSyst::Crop_model_interface &crop_at_start_season);
   virtual bool end_day()                                         modification_; //130620_070227
   virtual bool initialize()                                    initialization_; //151227_111221
//200115   #if (defined(USE_CS_SIMULATION_ROTATOR) || (CS_VERSION >= 5))
//200115   virtual void harvest_event
//200115      (const CropSyst::Crop_model_interface &crop_at_harvest)    modification_; //130620_131008NS_130716
//200115   #else
   virtual void harvest_event
      (const CropSyst::Crop_model_interface &crop_at_harvest)     modification_; //130716_130716_131008NS
//200115   #endif
   virtual bool end_year()                                        modification_; //130620_111031_010202
   virtual bool stop()                                            modification_; //160306
   virtual bool output_growing_season
      (const CropSyst::Crop_model_interface &crop_at_end_season);                //130716_131008
   virtual bool end_growing_season
      (CropSyst::Crop_model_interface &crop_at_end_season);                      //130716_131008
 #ifdef OLD_TRIGGER_SYNC
   virtual nat16 perform_triggered_synchronizations
      #ifdef INTERCROPPING
      (CropSyst::Crop_interfaced *crop_active   //eventually when this is permanent rename to triggering_crop //13100
      ,nat16            triggered_synchronizations);                             //130717
      #else
      ();                                                                        //030516
      #endif
 #endif
};
//_Land_unit_with_output_______________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

