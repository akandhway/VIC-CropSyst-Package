
#ifndef land_unit_outputH
#define land_unit_outputH

#include "CS_suite/simulation/CS_simulation_element.h"

#include "options.h"
#include "cs_UED_season.h"
#include "cs_UED_annual.h"
#include "cs_UED_harvest.h"
#include "cs_UED_db.h"
//180118 restored #ifndef CS_INSPECTOR_VERSION
#include "cs_UED_daily.h"
//180118 restored #endif
/*190119 now using optation global
#include "CS_suite/observation/CS_desired_vars.h"
*/
#ifndef REPORTS_IN_CS_MOD
#  if ((CS_VERSION > 0) && (CS_VERSION < 5))
#     ifndef REACCH_VERSION
#        include "cs_report_V4.h"
#     endif
#  endif
#endif

#include "land_unit_sim.h"

#define OUTPUT_NONE     0
#define OUTPUT_COMPLETE 0xFFFFFFFF
#define OUTPUT_ANNUAL   0x00000001
#define OUTPUT_SEASONAL 0x00000002
#define OUTPUT_HARVEST  0x00000004
#define OUTPUT_DAILY    0x00000008

extern std::ofstream *SCS_runoff_dat_file;                                       //131216
enum  CropSyst_period { SIMULATION_PERIOD,GROWING_SEASON_PERIOD,GROWTH_STAGE_PERIOD,MANAGEMENT_PERIOD};
namespace CropSyst {                                                             //141206
//______________________________________________________________________________
class Land_unit_output
: public extends_ CS::Simulation_element_abstract                                //161219
{
 protected:
   Land_unit_simulation &land_unit_simulation;                                   //161219

   nat32 output_flags;                                                           //151227
      // For GIS and regional simulations,
      // this indicates that we want to output daily data and other details.
 public:
   UED::Database_file_indexed *harvest_UED_db;                                   //041004
   Harvest_data_source        *harvest_data_src_rec;                             //041004
   UED::Database_file_indexed *season_UED_db;
   Season_data_source         *season_data_src_rec;
   UED::Database_file_indexed *annual_UED_db;
   Annual_data_source         *annual_data_src_rec;
 protected:
   datetime64                  latest_sync_date;                                 //041012
      // latest_sync_date was the date of the last triggered sync
   Normal_crop_event_sequence  latest_growth_stage_entered;                      //130925_041012

 private:
//180118 restored  #ifndef CS_INSPECTOR_VERSION
   #if ((CROPSYST_PROPER > 0) && (CROPSYST_PROPER <= 5))
   CropSyst_UED_database               *daily_UED_db;                            //030715
   CropSyst::CS_UED_tuple_data_source  *daily_UED_data_src;                      //030715
   CropSyst::CS_daily_data_record       daily_data_rec;                          //030715
   #endif
//180118 restored  #endif
 #if (CROPSYST_VERSION==4)
// Actually probably obsolete all version,
// Now TDF files are discovered and CS Explorer offers export functions          //190121
 public:
  CS::Desired_variables          *daily_desired_vars;
    // Now stored in calling program (CANMS or CropSyst) if 0 then no desired output
    // Will be zero if no daily output.
 #endif
 public: // constructor
   Land_unit_output
      (Land_unit_simulation   &land_unit_simulation_                             //161219
      ,const CS::Identification     &LBF_ID_                                     //151026
      ,nat32                         cycle_shift_                                //151026
      ,CropSyst::Scenario           &scenario_control_and_model_options_         //130827
      ,const CORN::date32 &today_                                                //170525
      ,const Geocoordinate          &geocoordinate_                              //151116
      ,const CS::Land_unit_meteorological &meteorology                           //151128
      #if (CS_VERSION==4)
      ,Common_simulation_log        &_event_log
      #endif
      ,const CropSyst::Scenario_directory *scenario_directory_                   //050619
      /*190205 now using optation_global
      ,CS::Desired_variables  *desired_vars_ = 0                               //041021
      */
      );
         // pass 0 if no variables to record
 public:
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual nat32 perform_triggered_synchronizations_today();                     //131004
   #endif
   virtual bool start()                                           modification_; //130620_080306
   virtual bool start_year()                                      modification_; //130620_010202
   virtual bool start_growing_season                                             //160324
      #if ((REACCH_VERSION==2) || (CS_VERSION >= 5) || defined(INTERCROPPING) || defined(OFOOT_VERSION))
      (modifiable_ CropSyst::Crop_model_interface /*200115 Crop_interfaced*/ &crop_at_start_season);
      #else
      ();
      #endif
   virtual bool end_day()                                         modification_; //130620_070227
   virtual bool initialize()                                    initialization_; //151227_111221
   virtual void harvest_event
      (const CropSyst::Crop_model_interface/*200115 Crop_interfaced*/ &crop_at_harvest)          modification_; //130716_130716_131008NS
   virtual bool end_year()                                        modification_; //130620_111031_010202
   virtual bool stop()                                            modification_; //160306

   virtual bool output_growing_season
      #if ((REACCH_VERSION==2) || (CROPSYST_VERSION >= 5) || defined(INTERCROPPING)|| defined(OFOOT_VERSION))
      (const CropSyst::Crop_model_interface/*200115 Crop_interfaced*/ &crop_at_end_season);                     //130716_131008
      #else
      ();                                                                        //050114
      #endif
   virtual bool end_growing_season
      #if ((REACCH_VERSION==2) || (CROPSYST_VERSION >= 5) || defined(INTERCROPPING)|| defined(OFOOT_VERSION))
      (CropSyst::Crop_model_interface/*200115 Crop_interfaced*/ &crop_at_end_season);                           //130716_131008
      #else
      ();
      #endif
 #ifdef OLD_TRIGGER_SYNC
 #error reached
   virtual nat16 perform_triggered_synchronizations
      #ifdef INTERCROPPING
      (CropSyst::Crop_interfaced *crop_active   //eventually when this is permanent rename to triggering_crop //13100
      ,nat16            triggered_synchronizations);                             //130717
      #else
      ();                                                                        //030516
      #endif
 #endif
};
//_Land_unit_output____________________________________________________________
}//_namespace CropSyst_________________________________________________________/
#endif
