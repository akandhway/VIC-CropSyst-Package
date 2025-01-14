#ifndef land_unit_simH
#define land_unit_simH

/* 010202
   Land block  is an area of homogeneous land use (I.e. a field).
   Land block fragment is a sub area of land blocks of homogeneous
   bio physical properties and uniform management regime.

   This class implements the simulation of a land block fragment.
*/

#include "options.h"
#include "corn/const.h"

#include "CS_suite/simulation/CS_simulation_unit.h"

#include "CS_suite/simulation/CS_identification.h"
#include "cs_scenario.h"
#ifndef VIC_CROPSYST_VERSION
#include "pond.h"
#endif

#include "soil/runoff_SCS.h"
#include "soil/infiltration_A.h"
#include "common/simulation/event_status.h"
#if (MANAGEMENT==4)
#include "mgmt_param.h"
#else
#include "management/management_param_V5.h"
#endif

#ifndef CAMO_VERSION
#include "crop/crop_param_V4.h"
#endif

#include "corn/math/statistical/sample_dataset_T.h"
#if ((CROPSYST_VERSION >=5 ) || defined(USE_CS_SIMULATION_ROTATOR))
#else
#  include "cs_ET.h"
#endif

#ifdef VIC_CROPSYST_VERSION
#include "crop/crop_interface.h"
#include "cs_scenario_directory.h"
#endif
#include "common/biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#include "CS_suite/observation/CS_observation.h"

#ifdef MICROBASIN_VERSION
#  include "CropSyst/source/rot_param.h"
#endif
#include "common/physics/water_depth.h"
#include "organic_matter/OM_residues_profile_abstract.h"

#include "CS_suite/observation/CS_examination.h"
// currently Alteration is declared with Examination

//__Forward_declarations________________________________________________________
class Common_simulation_log;
class LADSS_output_data_source;
class Enumerated_database;

#include "CS_suite/simulation/CS_land_unit_meteorological.h"

namespace CS { namespace meteorology { class Storms; } }                         //140217
namespace STATSGO { class Database;}

class CropSyst_ET;
class ET_Reference_FAO;

namespace Soil                                                                   //181206
{
   interface_ Hydrology_interface;
   interface_ Freezing_profile;
   class      Abiotic_environment_profile;
   class      Chemicals_profile;
   class Infiltration_nominal;
}

class Water_balance_accumulators;
class Crop_mass_fate_biomass_and_nitrogen;
class Soil_tillage_effect;
class Dynamic_water_entering_soil;
//______________________________________________________________________________
namespace CropSyst
{
   class Organic_nitrogen_operation_abstract;
   class Biomatter_application_operation;
   class Irrigation_operation    ;
   class Residue_operation       ;
   class Tillage_operation       ;
   class Harvest_or_clipping_operation      ;
   class Inorganic_nitrogen_operation   ;
   class N_application_soil_observation_mode;
   class Automatic_irrigation_model_class;                                       //120419
   class Auto_clip_biomass_mode_class ;                                          //120419
   class Soil_components;                                                        //181206
   class Sowing_operation;
   class Crop_nitrogen_mass_balancer;
   class Orchard_interface ;
   class Period_accumulators;
   class Annual_accumulators;
   class Management;
   class Scenario_directory;
   class Recalibration_operation ;
//abandoned   class Crop_interfaced_output;                                      //190616_131008NS
   class Crop_model_interface;                                                   //190616
   class Crop_parameters;
}

namespace CropSyst {
//______________________________________________________________________________
class Land_unit_simulation_VX
: public extends_ CS::Simulation_unit
{
 public:
   CropSyst::Scenario &scenario_control_and_model_options;                       //130827
   Common_event_list   scenario_event_list;                                      //131020
   // the model options are generally const, but we do
   // adjust parameters depending on available options and
   // simulation conditions and available data
 #ifdef DETAILED_BALANCES
 public:
   std::ostream                 *C_balance_file;
 #endif
 public:
   const CropSyst::Scenario_directory *scenario_directory; // not owned by this  //060627
 public:
      // soil moved out of environment because each simulation
      // event of the same land unit is going to have it own
      // soil water profile
   const CS::Identification            &ID;
   nat8                                 cycle_shift; // index
      // Identifies rotation cycle shifting (REACCH OFoot)
   const CS::Land_unit_meteorological  &meteorology;
 protected:
   provided_ Smart_soil_parameters *soil_parameters;
   contribute_ bool                       soil_parameters_known;
      // soil_parameters not owned by this, they are either owned by
      // Databases or instanciated and relinquished to soil.
   contribute_ bool                       initialize_residues_needed;
      // When initial residues are provided in organic_matter file
      // these residues override initialization from scenario file parameters.
 public:
   Soil_components  *soil;
 public:
   CORN::statistical::Sample_dataset_clad<float32,float32,float32>
      *sowing_precipitation_window_meter;                                        //170220_141105
 protected:
   friend class                   Water_balance_accumulators;
 public:
   float64                        daily_liquid_precipitation ; // Made available so we can accumulate  //141205 was daily_eff_precipitation
 public: // REACCH needs this public                                             //120924
   ::Soil::Infiltration_nominal  *infiltration_submodel;                         //181206_130313
   Soil_runoff                   *runoff;                                        //141201_131101_130308
   //200316 Soil_runoff_SCS     *runoff;                                         //141201_131101_130308
   bool using_numerical_runoff_model;                                            //200316
      // eventually weather provider will be a base of Weather_daily
 public:
   #ifdef VIC_CROPSYST_VERSION
   bool ET_ref; // VIC model provides ET_ref
   #else
   #if (CROPSYST_PROPER == 4)
   ET_Reference     *ET_ref;                                                     //180805
 private:
   provided_ Slope_saturated_vapor_pressure_plus_psychrometric_resistance
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance;             //180805
      //  This is only instanciated when ETref is loaded from weather
      // Otherwise it is available in CropSyst_ET.
   #else
   #endif
   float64 resistance_of_plants_d_m;                                             //180805
      //  This is only instanciated when ETref is loaded from weather
      // Otherwise it is available in CropSyst_ET.
   #endif
 public: // submodels
   ::Soil::Hydrology_interface         *soil_hydrology_ref;
      // Currently this is owned by soil so don't delete    //070825
   ::Soil::Freezing_profile           *soil_freezing_optional;
      // May be zero if soil freezing model is not enabled  //070824
   //181229 not really used anymore date32 latest_simulated_date;
      // This is used only for output, this is the date the simulation actually ends to       //041107
   // Output database must preceed ET_weather and all other instances
   // So it is available to their constructors.
   #ifndef VIC_CROPSYST_VERSION
   CropSyst::Pond     pond;                                                      //991102
   #endif
   Organic_matter_residues_profile_abstract *organic_matter_residues_profile_kg_m2; //050726
   ::Soil::Abiotic_environment_profile *soil_abiotic_environment_profile; // Allocated if needed by provide_soil_abiotic_environment_profile()  //060510
   CORN::Date_clad_32 auto_irrig_start_date;                                     //000406
   CORN::Date_clad_32 auto_irrig_end_date;                                       //000406
   // Active crop is for the current growing crop.
   // When the crop is terminated, it is moved to crop_completed.
   // crop_completed is used for output at the end of the season.
   // crop_completed is not further managed by this class and will
   // be deleted by a descendent class (land_unit_with_output)
   // However, It will be deleted by this class if it hasn't already been delete when:
   // 1. The start of the next day
   // 2. The program terminates.

//200404   contribute_ Crop_model_interface *crop_at_start_season;                       //070227_131008
   contribute_ Crop_model_interface *crop_active_or_intercrop;// but could be dormant //031119 //131008NS
//200404    contribute_ Crop_model_interface *crop_at_end_seasonX;                         //070227_131008
   contribute_ Crop_model_interface *crop_sowing_postponed;          // If computed sowing mode failed, we hold the created crop until we can sow //070514 //131008NS
   contribute_ Crop_model_interface *predominant_crop;                           //160407
   float64 crop_fract_canopy_cover;                                              //160407
   bool has_any_snow;                                                            //160407
      // This is used in a number of places (was local)
//190826 now local    float64  crop_evapotranspiration_max_m;                                       //151113
      //160629 rename to evapotranspiration_crop_pot_m
   CORN::Dynamic_array<float64> evaporation_potential_remaining;                 //160717
   #if (CS_VERSION==4)
   Common_simulation_log  &event_log_ref;                                        //030624
   #else
   // Eventually all versions will use explainations
   #endif
   //__________________________________________________________________________
   inline bool is_fallow()  affirmation_ {return crop_active_or_intercrop == 0;} //070228
   #ifdef VIC_CROPSYST_VERSION
   virtual bool is_current_crop_fruit_tree()                       affirmation_; //150720
   #endif
 #ifdef MANAGEMENT
   CropSyst::Management *curr_management;                                        //151019
      // Owned by sowing operation event // soilr does not need management
 protected: // LBF does not own the following operation mode pointers and must not delete them!  (They are owned by event_list events).
   CropSyst::N_application_soil_observation_mode_class                           //120625
       *curr_N_application_soil_observation_mode; // will be 0 if not currently in auto mode   //060804
 public: // this should be protected with an accessor, I haven't gotten to it yet.                                                        //040514
   CropSyst::Auto_clip_biomass_mode           *curr_automatic_clip_mode;         // will be 0 if not currently in auto clip mode                      //011207
   CropSyst::Automatic_irrigation_mode        *curr_automatic_irrigation_mode;   //020312
      // Will be 0 if not currently in auto irrigation mode
      // Owned by simulation (management event list).
   bool fixed_irrigation_scheduled_today;                                        //090722
 #endif
   bool harvest_today_flag;                                                      //160118
      // used by pest model
 public:
   float64  mulch_cover_fraction;  // Usually for inorganic plastic cover        //100610
   float64  automatic_irr_amount;   // meter today?                              //020310
   float64  total_N_application_kg_m2;                                           //161129

   float32  auto_N_previous_NO3_kg_m2; // kg/m2                                  //161205
   float32  auto_N_previous_NH4_kg_m2; // kg/m2                                  //161205
      // auto N now has option to scan the soil and if critical
      // apply a supplemental fraction of what had been previously applied.

   float64  earliest_irrigation_start_hour;                                      //081212
   float64  irrig_appl_target; // m //200226 table_irr_amount;       // today?
   float64  total_irrigation_today;                                              //050428
   Hours    management_irrigation_duration_hours;                                //070107
   Annual_accumulators *ANNL_accumulators_active;                                //030912
   Period_accumulators *GP_accumulators_active;                                  //030912
   Period_accumulators *CONTINUOUS_accumulators_active;                          //030912

   #if (CROPSYST_PROPER==4)
   Period_accumulators *FALLOW_accumulators_active;                              //030912
   #endif

   #ifndef VIC_CROPSYST_VERSION
   CropSyst::Crop_nitrogen_mass_balancer *crop_nitrogen_mass_balance_ref;        //070220
      // May be 0 if nitrogen simulation not enabled
      // (or N balace values not needed for output)
      // crop_nitrogen_mass_balance is relinquished to the common_simulation
      // we retain the reference and process the season.
   #endif
 public: // Grazing model
   float64 unsatisfied_harvest_BM_today_kg_m2;                                   //060323
      // For clipping/grazing events this is the amount
      // of demanded biomass that was not satisfied
   float64 grazing_BM_demand_today_kg_m2;                                        //050107
      // Demand includes everything including losses
   struct Grazing_hold_manure_application                                        //041220
   { float32 org_N_amount_kg_ha;
     float32 NH3_amount_kg_ha;
   } grazing_hold_manure_application;
 protected:
   Crop_mass_fate_biomass_and_nitrogen *crop_mass_fate_to_be_distributed;        //070627
      // There could be several operations and/or events
      // for the day that produce crop biomass
      // These are all accumulated and distributed to the
      // correponding target pools at the beginning of the next day.
   #ifdef NITROGEN
   CORN::Dynamic_array<float64> wettings;                                        //131106
      // Used for inorganic_NH4 volatilization
   #endif
 public:
   CS::Alteration scenario_alteration;                                           //181015
 private:
   Infiltration_model infiltration_model;                                        //190707
      // This is check very freqently so setup in initialize

 public: // constructors
   Land_unit_simulation_VX
      (const CS::Identification     &LBF_ID_                                     //151026
      ,nat32                         cycle_shift_                                //151026
      ,CropSyst::Scenario           &scenario_control_and_model_options          //130827
      ,const CORN::date32           &simdate                                     //130619
      ,const Geocoordinate          &geocoordinate_                              //151116
      ,const CS::Land_unit_meteorological &meteorology                           //151022
      #if (CS_VERSION==4)
      ,Common_simulation_log        &event_log                                   //030308
      #endif
      ,const CropSyst::Scenario_directory *scenario_directory);                  //120112_050619
 public: // destructors
   virtual ~Land_unit_simulation_VX();
 public: // Item implementations
   inline virtual bool is_key_string(const std::string &key)       affirmation_; //180820
   inline virtual bool is_key_nat32(nat32 key)                     affirmation_; //180820

/*180820  was probably only used for find_cstr now using is_key
   virtual const char *get_key()                                          const; //150917
*/
   virtual nat32 get_key_nat32()                                          const; //150917
   friend class Period_accumulators;                                             //010202
 public: // Simulation element implementations
   virtual bool initialize()                                    initialization_;
   virtual bool start()                                           modification_; // Start recording to output file
   virtual bool start_day()                                       modification_; //130620_020309
   virtual bool end_day()                                         modification_; //130620_050115
   virtual bool process_day()                                     modification_; //040331
   virtual bool stop()                                            modification_;
      // Stop recording to output file
      // Returns true if start successful
      // Process one day in the simulation
      // Returns true if daily process process successful
   inline virtual const char *get_ontology()       const { return "land_unit"; } //160620

 public: // introduced methods
   virtual bool evaporate()                                       modification_; //160408
   virtual bool evaporate_residues()                              modification_; //160408
   virtual bool evaporate_pond()                                  modification_; //160408
   virtual bool evaporate_soil()                                  modification_; //160408

   virtual float64 evaporate_pond_hour(Hour hour)                 modification_; //160408
   #ifdef RESIDUES
   virtual float64 evaporate_residues_hour(Hour hour)             modification_; //160408
   #endif
   virtual float64 evaporate_soil_hour(Hour hour)                 modification_; //160408

   virtual bool setup_cropping_system();                                         //060103
   virtual bool load_soil();
 protected: // Events saved in various files.
   virtual bool load_rotation_planting_events();                                 //020309
   bool resolve_and_schedule                                                     //160512
      (const CORN::Date_clad_32 &planting_date
      ,modifiable_ CropSyst::Sowing_operation &sowing_op
      ,const std::string &ID
      ,const CORN::OS::Directory_name &rotation_dir);
 public:
   void load_profile_recal_events(const char *filename);                         //020309
   void load_events_for_simulation(const CORN::OS::File_name &mgs_filename);     //020310
 public:
   void schedule_planting_event(date32 planting_date,
      CropSyst::Sowing_operation &sowing_op,const char * sowing_event_ID );      //050103
   bool setup_initial_soil_profile_recalibrations();                             //160303
   virtual bool start_growing_season
      (modifiable_ CropSyst::Crop_model_interface &crop_at_start_season);        //130716
   virtual bool end_growing_season                                               //130716
      (CropSyst::Crop_model_interface &active_crop);
      // when this becomes permanent rename to crop_ending_its_season
   virtual date32 get_today_date32()                                      const; //030714
   inline bool watershed_simulation()             const { return ID.number > 0;} //030530
 private: // initializations
   virtual bool initialize_soil();                                               //030527
      // Returns true if start successful
   #ifdef RESIDUES
   bool initialize_residues();                                                   //110919
   #endif
   #ifdef SALINITY
   bool initialize_salinity                                                      //080213
      (CropSyst::Soil_components *soil                                           //181206
      ,bool                    water_table_file_exists
      ,const char             *water_table_filename
      ,float64                 ECw_to_TDS_water_table                            //080402
      ,Infiltration_model      infiltration_model_check_used)     modification_;
   #endif
   bool distribute_evaporation_potential_daily_to_intervals                      //160719
      (float64 evaporation_pot_daily)          modification_;
 public: // operations
   virtual Event_status_indicator perform_operation                              //020308
      (Common_operation &op)                                      modification_;
   virtual Event_status_indicator terminate_operation                            //170310
      (Common_operation &op)                                      modification_;
   #ifndef CAMO_VERSION
   #ifndef YAML_PARAM_VERSION
   // For Crop parameter adjustments, version 5 will have adjustable parameters
   virtual Event_status_indicator perform_crop_adjustment                        //120719
      (const CropSyst::Crop_parameters::Adjustment &adjustment_event             //140318
      ,void *associated_with_obj);//An event will be associated with a Crop
   #endif
   #endif
   virtual Event_status_indicator process_model_specific_event                   //130428
      (const Simple_event *event);
   virtual bool end_operation_period
      (Common_operation &op, const CORN::Date_const &date_period_ends);          //020504
   void log_automatic_irr_event(const CORN::Date_const &date, float64 amount);   //020913
 public:    // operations accessed by CANMS
   #ifdef MANAGEMENT
   #ifdef NITROGEN
   Event_status_indicator perform_organic_nitrogen_common
      (CropSyst::Organic_nitrogen_operation_abstract  &organic_N_op);
   Event_status_indicator perform_biomatter_application
      (CropSyst::Biomatter_application_operation      &biomatter_appl_op);
   #endif
   #endif
 protected: // operations
   Event_status_indicator perform_planting      (CropSyst::Sowing_operation        &planting_op); //020308
   Event_status_indicator perform_recalibration (CropSyst::Recalibration_operation &recalib_op);  //020309
   #ifdef MANAGEMENT
   Event_status_indicator perform_irrigation    (CropSyst::Irrigation_operation    &irrig_op);    //020310
   #if (MANAGEMENT==4)
   Event_status_indicator perform_residue       (CropSyst::Residue_operation       &residue_op);  //020311
   #endif
   Event_status_indicator perform_tillage       (CropSyst::Tillage_operation       &tillage_op);  //020311
   #endif
   Event_status_indicator perform_harvest_or_clipping
      (CropSyst::Harvest_or_clipping_operation &clipping_op);                    //031014

   #ifdef MANAGEMENT
   #ifdef NITROGEN
   #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
   Event_status_indicator perform_organic_nitrogen    (CropSyst::Organic_nitrogen_operation &organic_N_op);
   #endif
   Event_status_indicator perform_inorganic_nitrogen  (CropSyst::Inorganic_nitrogen_operation &inorganic_N_op);
   virtual Event_status_indicator perform_start_automatic_N_application_mode
      (CropSyst::N_application_soil_observation_mode_class  &auto_N_mode_op);
   Event_status_indicator perform_automatic_N_application
      (CropSyst::N_application_soil_observation_mode_class::Split_application &auto_N_app);
   bool commit_auto_N(CropSyst::N_application_soil_observation_mode_class::Split_application &auto_N_app); //161201                                                        //161201
   float64 fract_inorganic_NH4_to_soil
      (Ammonium_source ammonium_source
      ,NH4_Appl_method NH4_appl_method
      ,bool NH4_volatilization_calculated
      ,float64 NH4_volatilization   //When NH4 is applied with no organic N
                                       //A number here not 0 will override any
                                       //selections made by the radio buttons and
                                       //computed by the model.
      ,float64 soil_cation_exchange_capacity
      ,float64 soil_pH);
   #endif
   Event_status_indicator perform_start_automatic_irrigation_mode  (CropSyst::Automatic_irrigation_mode &auto_irrig_mode_op);//020313 150701LML added VIC macro
   Event_status_indicator perform_start_automatic_clip_biomass_mode(CropSyst::Auto_clip_biomass_mode & auto_clip_op );       //020314
   #endif
   bool respond_to_field_operation                                               //981018
      (const std::string &NRCS_operation_description                             //060802
      ,const CropSyst::Tillage_operation::Oxidation_effect *oxidation_effect);
 public: // management conditions methods
/* OBSOLETE
200404 we already handle ending operation period in end season

 #ifdef OLD_TRIGGER_SYNC
   #ifdef INTERCROPPING
   Bitmask_crop_event triggered_synchronizations[2];                             //130717
      // Index 0 is predominant crop, 1 is secondary crop
   virtual void trigger_synchronization(uint32 trigger,nat8 crop_index);         //030605
   #else
   nat32 triggered_synchronizations;                                             //020310
   virtual void trigger_synchronization(uint32 trigger);                         //030605
   #endif
      // We need a way to communicate any syncronizations reached today to CropSyst model
      // to perform the syncronization
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual nat16 perform_triggered_synchronizations
      #ifdef INTERCROPPING
      (modifiable_ Crop_model_interface *crop_active
         //eventually when this is permanent rename to triggering_crop
      ,nat32            triggered_synchronizations);
      #else
      ();                                                                        //131005_030516
      #endif
   #endif

#else


//   virtual void trigger_synchronization(Normal_crop_event_sequence event,const Crop_model_interface &tiggering_crop);  //130902
  #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
//200404
//   virtual nat16 perform_crop_synchronizations
//      (CropSyst::Crop_model_interface &crop_active)               modification_; //130903

   virtual nat32 perform_triggered_synchronizations_today() = 0;                 //200404_131004
  #endif

#endif
*/

   #ifdef RESIDUES
   bool distribute_any_crop_biomass_fate();                                      //040624
   #endif
   virtual bool start_year()                                      modification_; //130620_040923
   virtual bool end_year()                                        modification_; //130620_111021_040923
   void         start_fallow();                                                  //021114
   #if (defined(USE_CS_SIMULATION_ROTATOR) || (CS_VERSION >= 5))
   inline virtual void harvest_event
      (const CropSyst::Crop_model_interface &crop_at_harvest)     modification_  //130620_131008NS_130716
   #else
   inline virtual void harvest_event
      (const CropSyst::Crop_model_interface &crop_at_harvest)     modification_  //130716_130716_131008NS
   #endif
      { UNUSED_arg(crop_at_harvest); return ; } //currently just to hookup output
   //200404 Crop_model_interface *get_crop_at_end_of_season()                      const;  //070227  // dont' return const crop because we may adjust the biomatter fate //131008NS
   /*200404
   Crop_model_interface *get_crop_active_or_at_end_of_season()            const; //070229_131008
         // get_crop_active_or_at_end_of_season is used by CS_mod daily output   //070229
   */
   inline Crop_model_interface *get_crop()                                const  //200404
      { return crop_active_or_intercrop; }
      // may return 0 if fallow

   ::Soil::Abiotic_environment_profile *
      provide_soil_abiotic_environment_profile()                  modification_; //060810
 protected:
   #ifdef MANAGEMENT
   bool process_any_automatic_clipping                                           //010202
      (bool clipping_was_just_disabled);                                         //020604
      // Returns true if there was an automatic clipping harvest.
   #endif
 public:

   #if ((CS_VERSION>=5) || defined(USE_CS_LAND_UNIT))
   inline virtual bool is_valid()                                   affirmation_
      { return true; } // this is hack probably should return CS::Simulation_element_abstract::is_valid();
   // V5 and REACCH this is moved to Land_unit
   #else
   #endif
      // Weather should be set before calling setup_CO2

   #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION<=2016))
   virtual bool take_directed_irrigation_soil_wetting                            //130313
      (const Directed_irrigation_soil_wetting *_soil_wetting)    appropriation_;
   #endif
 protected: // Output file handling methods
   void update_accumulators();                                                   //151028
 public:
   // The following methods are used in regional/GIS runs where the parameters
   // are loaded and stored by the engine and shared with the land units.
   bool know_soil_parameters                                                     //160916
      (Smart_soil_parameters *parameters_known)                      cognition_;
      // know_soil_parameters
   const Smart_soil_parameters *provide_soil_parameters()            provision_; //160916

 public:  // Usually  there will be a soil but it could be omitted if no soil water and no nitrogen limitiations.
   virtual const CropSyst::Soil_components *ref_soil()                    const; //160920_060803
   virtual       CropSyst::Soil_components *mod_soil()                    const; //160920_060803
   ::Soil::Chemicals_profile *get_soil_chemicals()                        const; //060803
      // may return 0 if no soil and/or no soil chemicals.
 private:
   Soil_tillage_effect *provide_soil_tillage_effect();                           //060929
   #ifdef CROP_ORCHARD
   Orchard_interface *provide_orchard_interception_model(Crop_parameters &crop_params);//071117
   #endif
   bool instanciate_organic_matter_and_residues();                               //110915
 protected:
   bool process_any_automatic_irrigation()                        modification_; //071017
   Dynamic_water_entering_soil *create_irrigation_in_intervals_mm                //070119
      (Minutes water_entering_soil_timestep_minutes )                     const;
   // returns 0 if no irrigation today
   bool adjust_mulching_if_applicable(nat16 NRCS_SDR_operation_number);          //100610
   virtual const Slope_saturated_vapor_pressure_plus_psychrometric_resistance    //150602
      *get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference() const; //150602
#ifdef INTERCROPPING
 public:
   Crop_model_interface *get_predominant_crop();                                 //130716
   Crop_model_interface *get_secondary_crop();                                   //130716
   Crop_model_interface *get_intercrop(nat8 crop_index);                         //130717
#endif
 public:
   struct Nitrogen_daily_output                                                  //131001
      {  // All values in this structure are element N  kg/m2
         float64 applied_inorganic;
         float64 applied_organic;
         float64 applied_total; /*inorganic + organic*/
         float64 volatilization_loss_NH3;          // was global daily_NH3_volatilization_loss
            // elemental N from NH3 from manure decomposition may be lost to the atmosphere.
         float64 inorganic_atomospheric_loss_NH4;  // was global daily_inorganic_NH4_to_atmosphere
            // elemental N from mineral NH4 application may be lost to the atmosphere.
         float64 volatilization_total;
         #ifdef MICROBASIN_VERSION
         float64 applied_inorganic_NH4_N;                                        //(kgNH4-N/m2) 160323LML
         float64 applied_inorganic_NO3_N;                                        //(kgNO3-N/m2) 160323LML
         #endif
       inline void clear()
         {  applied_inorganic = 0;
            applied_organic = 0;
            applied_total = 0;                                                   //160323
            volatilization_loss_NH3 = 0;
            inorganic_atomospheric_loss_NH4 = 0;
            volatilization_total = 0.0;
            #ifdef MICROBASIN_VERSION
            applied_inorganic_NH4_N = 0;                                         //160323LML
            applied_inorganic_NO3_N = 0;                                         //160323LML
            #endif
         }
       inline bool end_day()
         {
            applied_total = applied_inorganic + applied_organic;
            volatilization_total += volatilization_loss_NH3 + inorganic_atomospheric_loss_NH4;
            return true;
         }
      } nitrogen_daily_output;
   struct Soil_profile_daily_output
   {
      float64 sum_profile;
      //NYN float64 sum_5cm;
      //NYN float64 sum_10cm;
      //NYN float64 sum_15cm;
      float64 sum_30cm;
   };
   //_Soil_profile_daily_output________________________________________________/
   Soil_profile_daily_output profile_carbon_output;
   // This is currently only used by REACCH but it will very likely
   // be applicable to other models where there is
   // a continuation from a previous run.
   float64 output_crop_evapotransp_max_m;                                        //190812
      //190812 output_pot_crop_evapotransp_m;                                    //151218
   float64 output_act_crop_evapotransp_m;                                        //151218
      // These ET value should be move to crop when I have implemented inspection


     // rename output_act_crop_evapotransp_m to output_crop_evapotransp_act_m


 public:
   bool parameterize_organic_matter                                              //120909
      (const char *initial_or_equilibrated_OM_file_name_unqual)           const; //160830
 public: // output
   bool get_active_crop_name(modifiable_ std::wstring &active_crop_name)  const; //150720
      // sets current_crop_name to the name of the currently growing crop.
      // (its unqualified file name without extension).
      // Returns false if the land unit is in fallow (no crop).
      // This was added for VIC but is of general use
      // Note that the crop file name could be a symbolic link (in VIC)
      // this will be dereferenced.
 public: // Simulation_engine implementation
   virtual const CORN::OS::Directory_name &get_output_directory_name()    const;
 public:
   RENDER_INSPECTORS_DECLARATION;                                                //150916
   contribute_ CS::Emanator *emanator_known;                                     //160616
      // emanator_known is actually an aspect
      // emanator_known is not_owned, don't delete
   inline virtual const CS::Emanator *get_emanator()                      const { return emanator_known; }  //160622
   inline virtual const CS::Emanator *get_emanator_annual()               const { return emanator_known; }  //160622
      // There isn't actually a special annual emanator.
      // It just returns the emanator indicating that annual
      // inspection is relevant to to this model's output regimen.
};
//_class Land_unit_simulation_VX_______________________________________________/
#if (CROPSYST_VERSION == 4)
//______________________________________________________________________________
class Land_unit_simulation
: public Land_unit_simulation_VX
{
#ifdef WSHEDMOD
 public: // Watershed model
   // friend class CropSyst_model;  // this friendship is not working
   Enumerated_database  *cell_in_db;                                             //990409
   Enumerated_database  *cell_out_db;                                            //990409
#endif
#ifdef LADSS_MODE
   bool                        LADSS_daily_database_table_enabled;               //030415
   LADSS_output_data_source   *LADSS_daily_database_table;
   LADSS_output_data_source   *LADSS_harvest_database_table;
   LADSS_output_data_source   *LADSS_annual_database_table;
#endif

 public:
#if defined (PRINT_PROFILE) || defined(DISPLAY_PROFILE)
   void print_profile(std::ostream &profile
      ,bool limit_layers // If displaying to screen
      ,const CORN::Date_const & today
      ,const char *crop_description,const char */*growth_stage*/,float64 root_depth); //000511
#endif

 public:
   Land_unit_simulation
      (
      const CS::Identification &LBF_ID_                                          //151026
      ,nat32                         cycle_shift_                                //151026
      ,CropSyst::Scenario           &scenario_control_and_model_options          //130827
      ,const CORN::date32           &simdate_
      ,const Geocoordinate &geocoordinate                                        //151116
      ,const CS::Land_unit_meteorological &meteorology                           //151022
      #if (CS_VERSION==4)
      ,Common_simulation_log        &event_log
      #endif
      ,const CropSyst::Scenario_directory *scenario_directory);                  //131213
   virtual ~Land_unit_simulation();

   virtual bool load_soil();
   virtual bool start_year()                                      modification_; //130620
   virtual bool end_year()                                        modification_; //130620_111021_040923
   virtual bool end_growing_season(Crop_model_interface &active_crop);
      // when this becomes permanent rename to crop_ending_its_season  //130716
   virtual bool initialize()                                    initialization_;
   inline virtual bool stop()                                     modification_  //160306
      { return Land_unit_simulation_VX::stop(); }

};
//_class Land_unit_simulation_______________________________________2011-10-27_/
#endif

#if (!defined(CROPSYST_VERSION) || CROPSYST_VERSION >= 5)
class Land_unit_simulation
: public Land_unit_simulation_VX
{
 public:
   //NYI instead of event log use explaination
   Land_unit_simulation
      (const CS::Identification &LBF_ID_                                         //151026
      ,nat32                     cycle_shift_                                    //151026
      ,CropSyst::Scenario       &scenario_control_and_model_options              //130827
      ,const CORN::date32       &today_
      ,const Geocoordinate      &geocoordinate                                   //151116
      ,const CS::Land_unit_meteorological &meteorology                           //151022
      ,const CropSyst::Scenario_directory *scenario_directory
      );
   #if (defined(CROPSYST_PROPER) || defined(OFOOT_VERSION) || defined (REACCH_VERSION))
   virtual bool load_soil();                                                     //111030_020620
   #endif
   virtual bool initialize()                                    initialization_; //151207
    #ifdef __linux__
   bool initialize_linux();
    #endif
   virtual bool start_year()                                      modification_; //130620
};
//_class Land_unit_simulation_______________________________________2011-10-27_/
#endif
}//_namespace CropSyst_________________________________________________________/
#endif

