#ifndef crop_cropsystH
#define crop_cropsystH
#include "options.h"
#include "crop_N_interface.h"
#include "common/simulation/synchronization.h"
#include "mgmt_types.h"
#include "crop/crop_common.h"
#include "crop/quiescence.h"
#if (CROPSYST_VERSION == 4)
#  include "crop/crop_root_V4.h"
#endif
// root_V5 is in both V4 and V5
#  include "crop/crop_root_V5.h"

#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
#include "crop/phenology_2013.h"
#define THERMAL_TIME_SOURCE thermal_time->
#endif
#if ((PHENOLOGY_VERSION==2018))
#include "crop/phenology_2018.h"
#define THERMAL_TIME_SOURCE phenology.
#endif
#if ((CROPSYST_VERSION >=5 ) || defined(USE_CS_LAND_UNIT))
#include "common/weather/parameter/WP_air_temperature.h"
#include "common/weather/parameter/WP_solar_radiation.h"
#include "common/weather/parameter/WC_vapor_pressure_deficit.h"
#include "common/weather/any_timestep/temperature_est.h"
#else
#include "common/weather/wthrbase.h"
#endif
#include "common/weather/weather_interface.h"

#ifdef HOURLY_THERMAL_TIME
#include "common/weather/weather.h"
#endif

#ifdef YAML_PARAM_VERSION
#include "crop/event_scheduler_crop.h"
#endif
#ifdef YAML_PARAM_VERSION
#  include "crop/transpiration_dependent_growth.h"
#else
#  include "crop/transpiration_dependent_growth_V4.h"
#endif
#include "corn/container/bilist.h"
#include "corn/math/compare.hpp"
#include "corn/math/statistical/sample_cumulative_T.h"
#include "crop/crop_param.h"
#include "common/recorder/recorder.h"

#define vapor_def_limit  0.40
#include "crop/thermal_time_daily.h"
#include "CS_suite/observation/CS_observation.h"
#define USE_WEATHER_PARAMETERS

#ifdef OLD_BIOMASS_GROWTH
#else
#include "crop/biomass_growth.h"
#endif

#ifdef NITROGEN
#include "common/biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
#endif

#include "corn/chronometry/time_conversion.h"

#ifdef CROPSYST_PROPER
#include "CS_suite/CropSyst/extreme/stress_ascertaination.h"
#endif
//______________________________________________________________________________
// forward declarations
namespace CORN {
class Data_record;
}
using namespace CORN;
namespace CS
{
   class Desired_variables;
   class Weather_hours;                                                          //131101
}
namespace Soil
{  class Nitrogen;
}
class ET_reference_interface;
class Seeding_parameters;
class Crop_mass_fate;
class Residues_interface;
class Crop_emergence_thermal_time;
class Common_simulation_log;
class Vapor_pressure_saturation_slope;                                           //140507
class Psychrometric_constant;                                                    //140508
namespace CropSyst
{
   class Harvest_or_clipping_operation_struct;                                   //140811
   interface_ Yield_model;                                                       //150825
   class Crop_CO2_response;                                                      //020424
   class Canopy_leaf_growth;
   class Crop_emergence_abstract;
}
#if (CROPSYST_VERSION >= 5)
namespace CropSyst {
#endif
   class Crop_nitrogen_interface;
   class Biomass_growth;
//______________________________________________________________________________
class Stress_adjusted_temperature_estimator      // rename to Stress_adjusted_maximum_temperature_estimator
: public  Temperature_estimator
{
   float64 stress_adjusted_temperature;  // degrees C     rename to stress_adjusted_max_temperature
public:
   inline Stress_adjusted_temperature_estimator(Minutes interval_minutes=60)
      : Temperature_estimator(interval_minutes)
      {}
   inline void set_stress_adjusted_temperature(float64 _stress_adjusted_temperature) //030610
      { stress_adjusted_temperature = _stress_adjusted_temperature;}             //030610
   inline virtual float64 get_max_today()                                  const
      { return stress_adjusted_temperature; }
};
//_class Stress_adjusted_temperature_estimator______________________2003-06-09_/
// forward declarations
#if (CROPSYST_VERSION >= 5)
}//_namespace CropSyst_________________________________________________________/
#endif
namespace CropSyst {                                                             //141206
   class Crop_emergence_hydrothermal_time;

//______________________________________________________________________________
class Crop_complete
: public extends_ Crop_common                                                    //031131
{  // parameters have been moved to Crop_common

 protected:
   const Weather_spanning_interface &weather;
   CropSyst::Event_scheduler_crop_complete *as_event_scheduler_crop;
      // alias to event_scheduler cast as Event_scheduler_crop so
      // we have access to crop specific event operations.
   const CORN::Local_time &sun_rise_time;                                        //150907
   //snow pack is ephermeral we cannot reference it                              //141205
   cognate_ float64 snow_pack_thickness; // meter                                //161110
   friend class Crop_nitrogen_interface;                                         //050816
   #ifdef NITROGEN
   friend class Crop_nitrogen_common;         // eventually obsolete because using interface
//NYN   friend class Crop_nitrogen;         // eventually obsolete because using interface
   #endif
   friend class CropSyst_DLY_report;
   friend class CropSyst_GS_report;                                              //030530
   friend class Biomass_yield_calibrator;
 protected: // External objects frequently accessed.
   const Air_temperature_maximum                &air_temperature_max;            //140225
   const Air_temperature_minimum                &air_temperature_min;            //140225
   const Air_temperature_average                &air_temperature_avg;            //140225
   const Solar_radiation                        &solar_radiation;                //140813
   const Psychrometric_constant                 &psychrometric_constant;         //140508
   const Vapor_pressure_deficit_max             &vapor_pressure_deficit_max;     //140225
   Air_temperature_average                       temperature_with_est_night;
      // daily temp with est. night time temps.
 public: // Soil interfaces
   // These soil pointers may be 0 indicating the crop is not limited by the soil.
   // This is the case with the Crop calibrator and the simulation option.
   const ::Soil::Salt_interface  *soil_salinity;                                 //051118
 protected:
   provided_ Canopy_leaf_growth *canopy_leaf_growth;                            //080725
 protected: // state provided to canopy                                          //080506
   float64 water_canopy_expansion_factor;                                        //080506
      // value between 0 and 1.
private :
   Stress_adjusted_temperature_estimator stress_adj_hourly_temperature_estimator;//030609
      //071127 Actually this should be optional if hourly thermal time is selected
   CORN::Dynamic_array<float32> stress_adj_hourly_temperature;                   //030610
      //071127 Actually this should be optional if hourly thermal time is selected
   CORN::Dynamic_array<float32> stress_adj_hourly_temperature_not_applicable;    //150826
private:
   Seeding_parameters *seeding_parameters; // may be 0 if not using that model   //041201
   bool estimate_root_biomass;                                                   //060612
protected:
   float64  yesterday_deg_day;
   mutable  CORN::statistical::Sample_cumulative<float64,float64>
            vapor_pressure_deficit_stats;              // used only for output  //030530
   float64  intercepted_radiation;                                               // MJ/m2 ?  (may be PAR or total radiation based depending on the mode)   //030530
   float64  intercepted_radiation_accum;                                         // MJ/m2 ? // used only for output                                        //030530
   float64  intercepted_radiation_season_accum;                                  //030602
      // MJ/m2 ? // used only for output to season report,
      // it is actually the same as intercepted_PAR_accum but that is
      // currently cleared before the season report is generate,
      // this is not cleared until after the report detail is output.
 //  The following are daily computed variables:
 protected: // Growth limitations passed from potential growth to actual growth
 #if (CROPSYST_VERSION == 5)
 public: // This should be protected, but there appears to be friend namespace issue
 #endif
 protected:
   float64  attainable_top_growth;                                               //011022
   provided_ Biomass_growth *biomass_growth;                                     //180111
 protected:
   float64 biomass_at_filling;                                                   //131022
   float64 biomass_at_maturity; // kg/m2  130510
 #if (CROPSYST_VERSION == 5)
 public: // This should be protected, but there appears to be friend namespace issure
 #endif
   float64  unclipped_biomass;                                                   //010329
   // unclipped_biomass used for clipped perennials otherwise always 0.0  in crop nitrogen
   // Do not use the reported for any calculations,
   // they are provided only because the data record requires fixed members to record the values
 public: // Leafs and canopy
      // was Date_time_clad_64
   float64  plant_height;                                                        //990201
 #if (CROPSYST_VERSION == 5)
 public: // This should be protected, but there appears to be friend namespace issure
 #else
 protected: // root related variables
 #endif
   // should be local to limited_growth but it is needed by nitrogen
   CropSyst::Crop_root           *roots_current; // 0 if there is no soil        //061208
#ifndef CROP_CALIBRATION
   CropSyst::Crop_root_dormant   *roots_dormant; // 0 if there is no soil        //061208
#endif
   #if (CROPSYST_VERSION==4)
   CropSyst::
   #endif
   Crop_root_vital      *roots_vital; // May be 0 if there is no soil            //061208
 public: // root related variables
   float64  pot_root_biomass           ;  // used for N limited growth           //010713
   float64  act_root_biomass           ;  // used for output and roots           //010713
 public:// Stress
   mutable CORN::statistical::Sample_cumulative<float64,float64>                   //180408
            report_temperature_stress_stats;                                     //010726
   float64  overall_growth_stress;  //  This is the light purple stress curve in the runtime display
 protected:
   bool harvestable;                                                             //190710
   public: Yield_model *yield;                                                   //150825
      // public so parameter scheduler can access
 public: // Growth stages, dates, and special conditions
   bool     initialized;                                                         //031121
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   Normal_crop_event_sequence growth_stage_at_harvest;                           //040526
   #endif
   nat16    emergence_count;                                                     //190411
   bool     fall_sowing;   // true if sown in the fall, used for perennials to not give too high restart GAI the season after fall sowing.   //040114
   int16    harvests_this_season;                                                //040625
   bool     no_more_clippings_this_season;  // when a latest clipping event is encountered, we stop any postponed clippings   //040721

   // for clipping was in Crop_interfaced
   nat16    consecutive_low_growth_days_for_clipping;                            //190619_970219
   nat16    days_since_last_clipping;                                            //190616
   float64  max_daily_GAI_rate;                                                  //190616
   bool     automatic_clipping_enabled;                                          //190619_980724

 protected: // The following are used only to show the respective growth stage labels on the reports for a few days
   struct Linger_label                                                           //200420
   {  sint16 planting;                                                           //030521
      sint16 tuber_init;                                                         //030521
      sint16 harvest;                                                            //030521
      //200420 sint16 anthesis;                                                           //191022
   } linger_label;                                                               //200420
   CORN::Date_time_clad_64 inactive_end_date;                                    //081013
   float64 limited_pot_transpiration_m;                                          //160821
 public:

   // transpiration_dependent_growth is sort of only used by
   // biomass growth, it probably could be moved

   bool before_WUE_change;
   #ifdef YAML_PARAM_VERSION
   // Using adjustable parameter idiom
   CropSyst::Biomass_production_transpiration_dependent_V5 transpiration_dependent_growth; //120614 100729
   #else
   CropSyst::Biomass_production_transpiration_dependent_V4 transpiration_dependent_growth; //120614 100729
   #endif
   CORN::Date_clad_32 latest_harvest_date;
   CORN::Date_clad_32 unexpected_termination_date;                               //000428
      // Due to lethal freezing or latest date to harvest
   Crop_mass_fate *biomass_fate_today;                                           //050822
      // 0 unless harvest or biomatter loss today relenquished to owner.
   Crop_mass_fate *biomass_fate_season;                                          //050822
      // 0 until any biomatter fate processed
   bool     terminate_pending;                                                   //040514
      // The crop will set terminate_pending flag to indicate self induced termination conditions occurred, state variables are suitable for output
 protected: // Special events and conditions
   //200603 move to Crop_orchard_fruit bool           fruit_harvested;                                               //030529_000626
   int16          days_of_expected_dormancy_remaining;                           //980723
 protected:  // Estimated carbon assimilation for output only
   float64  C_mass_canopy_est;                                                   //110915_060818
   float64  C_mass_root_est;                                                     //110915_060818
 public:
   const CS::CO2_atmospheric_concentration *CO2_current_conc_optional;           //151201
      // Can be 0 if not simulating CO2
   float64 CO2_TUE_adjustment;                                                   //150907
 protected: // Synchronization
   CS::Temperature               stress_adjusted_temperature;                    //150217_140625
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   Thermal_time_common::Vernalization        *vernalization;      // optional    //181118
   Thermal_time_common::Photoperiodization   *photoperiodization; // optional    //181118
   Thermal_time_common          *thermal_time;
   CropSyst::Phenology_2013 phenology;                                           //130904
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   CropSyst::Phenology_2018 phenology;                                           //130904
   #endif

   virtual void trigger_synchronization(Normal_crop_event_sequence event);       //130903
 protected: // Emergence models
   Crop_emergence_abstract *emergence; // Will be 0 before and after emergence   //041201
   Crop_emergence_hydrothermal_time *emergence_hydrothermal_time;                //041201

   CropSyst::Quiescence_start_requirement    *quiescence_start_requirement;      //160326_021202
   CropSyst::Quiescence_end_requirement      *quiescence_end_requirement;        //160326_021202
   inline virtual bool waiting_for_fruit_bud_break() affirmation_{return false;}
 protected: // Fruit virtual methods (may eventually be moved to fruit crop
   inline virtual bool is_limit_max_LAI()                           affirmation_ //081103
      { return false; } // only applies to fruit crops (which will override)
 public: // public so reports can  access
   Crop_nitrogen_interface  *nitrogen;  // currently assume kg_m2 (may be able to factor out units, haven't checked) //050722
   float64 N_mass_canopy_recorded_before_harvest;                                //110915_070209
 public: // external object references
   mutable Common_simulation_log    *ref_event_log;    // The event log is optional, it is owned by caller   //030624
   Residues_interface       *ref_residues;     // Residues is optional (used by the canopy decomposition submodel);
   // In V5 will use inspectors
   float64 param_saturated_soil_root_activity_coef;                              //191011
   float64 param_dry_soil_root_activity_coef;                                    //191011
 #ifdef CROPSYST_PROPER
 public:
   // Stress ascertaination is a special output.
   // Eventually these will be moved to an external standalone program.
   float32 report_degree_hours_above_30_today;
   float32 report_degree_hours_above_34_today;

   CS::Stress_ascertaination *above_30C_season;                                  //180814
   CS::Stress_ascertaination *above_35C_season;                                  //180814
   CS::Stress_ascertaination *above_30C_flowering;                               //180814
   CS::Stress_ascertaination *above_34C_flowering;                               //180814
 #endif
 public: // Special reported output. These are not used by the model itself
   float32 report_transpiration_use_eff; // transpiration_dependent_growth.ref_transpiration_use_efficiency_CO2_adjusted()
   //___________________________________________________________________________
   struct Season_tally
   {
      bool window_14_days_achieved;
      #ifdef CROPSYST_PROPER
      CORN::Dynamic_array<float32> degree_hours_above_30_flowering;
      CORN::Dynamic_array<float32> degree_hours_above_34_flowering;
      #endif
      CORN::statistical::Sample_cumulative<float32,float32> Srad;
      CORN::statistical::Sample_cumulative<float32,float32> Tmax;
      CORN::statistical::Sample_cumulative<float32,float32> Tmin;
      CORN::statistical::Sample_cumulative<float32,float32> Tavg;
      float32 transpiration_act; // cumulative used for calculation
      // Calculated at end of season
      float32 water_use_efficiency; // kg biomass/kg water
         // (season biomass/ season transpiration)
    public:
      inline Season_tally()
         : window_14_days_achieved  (false)
         , transpiration_act        (0)
         , water_use_efficiency     (0)
         {}
   } output_season_tally;
   //_Season_tally______________________________________________________________
 public: // structors
   Crop_complete
      (CropSyst::Crop_parameters             *parameters                         //010110
      ,float64 param_dry_soil_root_activity_coef_                                //170518
      ,float64 param_saturated_soil_root_activity_coef_                          //170518
      ,bool                                   owns_parameters                    //040121
      ,const CORN::date32                    &today_                             //170525
      ,CORN::Unidirectional_list             &performable_operations             //151004
      ,const CS::CO2_atmospheric_concentration *atmospheric_CO2_conc_ppm_optional//180122
      ,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional   //151201
      ,const Air_temperature_maximum         &air_temperature_max                //140813
      ,const Air_temperature_minimum         &air_temperature_min                //140813
      ,const Air_temperature_average         &air_temperature_avg                //140813
      ,const Solar_radiation                 &solar_radiation                    //140813
      ,const Vapor_pressure_deficit_daytime  &daytime_vapor_pressure_deficit     //140813
      ,const Vapor_pressure_deficit_max      &vapor_pressure_deficit_max         //140813
      ,const Psychrometric_constant          &psychrometric_constant             //140508
      ,const Weather_spanning_interface      &weather                            //151130
      ,const CORN::Local_time                &sun_rise_time                      //150907
      ,const float64                         &ref_day_length_hours               //140812
      ,Seeding_parameters                    *seeding_parameters = 0             //041201
      ,::Soil::Soil_interface                *soil = 0                           //050721
      ,Crop_nitrogen_model              nitrogen_model = CROP_NITROGEN_DISABLED  //050722
      #ifdef NITROGEN
      ,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance            //referenced
       *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference = 0
      #endif
      );                                                                         //170518
   virtual ~Crop_complete();                                                     //020409
 public: // Simulation_element method implementations
   virtual bool initialize()                                    initialization_;
   virtual bool start()                                           modification_; //071120
   virtual bool clear_day()                                       modification_; //200413
   virtual bool start_day()                                       modification_; //040205
   virtual bool process_day()                                     modification_; //110824_020711
   virtual bool end_day()                                         modification_; //070221
 public: // optional setup
   inline virtual void know_event_log(Common_simulation_log *_event_log)         //170315_050725
      { ref_event_log = _event_log; }
   //   Use set_event_log if you want to record crop event and change of status. //050725
   virtual void know_residues(Residues_interface *_residues) ;                   //060531
   inline virtual float64 know_snow_pack_thickness(float64 snow_pack_thickness_) cognition_ //161110
      { return snow_pack_thickness = snow_pack_thickness_; }
   #ifdef CROPSYST_PROPER
   bool stress_ascertation_determination(); // special                           //180914
   #endif
 public:
   bool in_growing_season()                                        affirmation_;
   virtual bool end_season_if_no_dormancy()                       modification_; //050818
   virtual bool end_season()                                      modification_; //160629
   virtual bool is_at_season_start()                               affirmation_; //200404
   virtual bool is_at_season_end()                                 affirmation_; //200404
 public: // The following functions are used in cases where the
   // plant dies or operations that removed parts of the plant
   virtual void commit_biomass_fate()                             modification_; //040610
   virtual
      Crop_mass_fate_biomass_and_nitrogen *
      process_biomass_fate                                                       //070625
      (Biomass_fated_cause fated_cause
      ,Harvest_amount_mode harvest_amount_mode                                   //050822
      ,float64 remove_amount_kg_m2                                               //050822
      ,float64 retain_GAI                                                        //050822
      ,float64 retain_biomass                                                    //110603
      ,const Biomass_fate_parameters  &biomass_fate_parameters
      ,bool terminate                                                            //040817
      ,float64 yield_now); // from harvest index, may be 0.0
   // This moves plant biomass to various pools, harvest yield, residue, loss etc..
   // Returns the fate of the material (call must delete the returned fate object)  //060323
 private:
   float64 record_biomass_fate
      (const Crop_mass_fate &fate_now,float64 curr_canopy_biomass_kg_m2);        //040809
 public: // Leaf biomass uptake
   virtual float64 distribute_todays_non_leaf_stem_growth                        //071120
      (float64 todays_growth) modification_;
      // returns todays growth that is available to goto leaf and stem
      // For normal crops this simply returns todays_growth
      // For orchard fruits, the biomass will be distributed
   float64 est_root_biomass()                                             const; //060612
      // This was the old way of calculating root biomass using a fraction of canopy biomass
                                       // this is deprecated, now use calc_root_biomass();
   void update_root_density_and_root_fractions();
 public: // Special conditions and events
   virtual void check_hardiness();                                               //190703_131104
   bool start_season();
   virtual bool start_inactive_period(bool apply_dormancy);                      //140625_030523
   virtual bool senesce_at_dormancy();                                           //190709_071120
   virtual bool consider_inactive_period();                                      //200402_020626
      //200402 (float64 avg_temperature); // was daily temp with est. night time temps.
   virtual bool setup_quiescence_end_requirement();                              //200402_160326_080401
      //200402 (CORN::DOY doy);

   //Not sure if I need both restart_after_inactive_period
   // because when else is there restart (check if anything else calls restart)
   virtual bool restart_after_inactive_period();                                 //970219
   inline virtual float64 get_solid_canopy_interception()                  const //021211
      { return 0.0;}
   virtual float64 est_degree_days_X_days_from_now                               //000330
      (const CORN::Date_const &simdate, Days days_from_now)               const; //010202
   // This is currently overridden by Crop_interfaced_CropSyst
   // Probably want to move this to Crop_common
   virtual float64 update_adjusted_ET_crop_coefficient()         rectification_; //200127_080808
      //200127 was get_...
public: // Growth
   #ifdef OLD_BIOMASS_GROWTH
   //191025 replaced TUE RUE model
   float64 calc_RUE_temperature_limitation()                              const; //130513
   virtual float64 calc_attainable_top_growth();
   #else
   Biomass_growth &provide_biomass_growth()                          provision_; //180612
   #endif
   virtual float64 calc_act_biomass_growth                                       //011023
      (float64 planting_depth                                                    //010202
      ,float64 &LAI_related_growth);

public: // Phenology / Growth stage functions
   /*200420 replaced by  describe_periods
   virtual const char *describe__growth_stage
      (std::string &growth_stage_phrase)                                  const;
   */
   virtual nat8 describe_periods(CORN::Text_list &descriptions)           const;

   inline virtual bool has_leaf_senescence()                       affirmation_  //140619
      { return parameters->has_leaf_senescence();}
   inline virtual bool is_perennial()                              affirmation_  //140810
      { return parameters->is_perennial(); }

   bool is_established()                                           affirmation_; //190708

   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   #ifdef THERMAL_TIME_OBSOLETE_2020
   bool thermal_time_matchs(float64 deg_day_event,bool clipping_adjusted) const;
   #else
   bool thermal_time_matchs(float64 deg_day_event)                        const;
   #endif
   #endif

   virtual bool thermal_time_event()                              modification_;
      // This function checks if the current thermal time matches the
      // thermal time needed to acheive and of the parameters for
      // phenology and if so, performs an update for
      // the respective growth stage initiation. 010723
   #ifdef SEED_EMERGENCE_MODEL
   bool check_for_emergence();                                                   //041201
      // Returns true if this was the first day (first cohorts)
      // Note returns false after the first day event if aditional cohorst
      // (so this doesn't return true if any emergence, it return true only the first day).
   #endif
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual bool initiate_tuber      ();
   #endif
   virtual bool initiate_flowering  ();
   virtual bool initiate_filling    ();
   virtual bool initiate_accrescence();   // 181231 was initiate_emergence  ()
   virtual bool initiate_culminescence(); // 181109 was initiate_max_canopy      //130429
   virtual bool initiate_senescence ();                                          //121224
   virtual bool initiate_maturity   ();
   virtual bool culminate_senescence();                                          //191022
   virtual
      Crop_mass_fate_biomass_and_nitrogen *                                      //070625
      harvest                                                                    //051105
      (float64 remove_amount_kg_m2                                               //050822
      ,const CropSyst::Harvest_or_clipping_operation_struct                      //140811
      *harvest_or_clipping_params
      ,bool terminate);                                                          //140811
      // \return fate of biomass if could harvest otherwise 0 caller must delete //020311
   bool natural_expiration()                                      modification_; //040805
   virtual bool allow_natural_expiration()                        modification_; //040805
   virtual bool respond_to_clipping()                             modification_; //051103
public: // Daily leaf history methods
   // The following are daily_history total accessors
   // The daily history will be tallied if not yet done today.
   float64 get_GAI(nat8 include_GAI)                                      const;
   float64 get_GAI_vital()                                                const;
   float64 get_GAI_vital_plus_effete()                                    const; //070320
   float64 get_canopy_biomass_kg_m2_including(nat8 include_biomass)       const; //120614
   float64 get_live_green_canopy_biomass_kg_m2(nat8 include_biomass)      const;
 public: // Virtual methods to be supplied by derived classes
   // Related to nitrogen
   virtual float64 get_min_water_N_stress()                               const; //010726
      // Probably no longer needs to be virtual
 protected:
   float64 get_water_productivity(bool before_WUE_change)                 const; //080428
 #if (CROPSYST_VERSION == 5)
 public: // This should be protected, but there appears to be friend namespace issue
 #endif
   #ifdef OLD_BIOMASS_GROWTH
   #error obsolete
   float64 get_radiation_use_efficiency_global()                          const; //080429
   #endif
   #if (CROPSYST_VERSION < 5)
   float64 get_radiation_use_efficiency_PAR()                             const; //080429
   #endif
      // Returns the maximum radiation use efficiency for the season which may be adjusted
      // for C02 simulations.                                                    //010723
 public: // Interface operations called by external calling routine (for setup or daily loop)
   virtual bool plant(bool fall_sowing);                                         //190621_140813_110824_020220
   virtual void log_event(const char *description)                        const; //110824_020912
   virtual Crop_mass_fate_biomass_and_nitrogen *harvest_crop                     //070625
      (Harvest_amount_mode harvest_amount_mode                                   //031015
      ,float64 remove_amount_kg_m2                                               //050822
      ,float64 retain_GAI                                                        //050822
      ,const Biomass_fate_parameters &harvest_biomass_fate_parameters
      ,bool terminate);                                                          //050822
      // Returns the fate of biomass if if could harvest otherwise 0 caller must delete
   virtual void set_automatic_nitrogen_mode(bool enable);                        //040525
   virtual float64 damage_GAI(float64 GAI_damage)                 modification_; //151226
      ///< kills the specified GAI_damage.
      ///  \returns the amount of live GAI remaining.
 protected:
   float64 tuber_initiation_temperature_stress_factor(float64 min_air_temperature) const; //081109
   float64 tuber_bulking_temperature_stress_factor(float64 min_air_temperature) const;    //081109
   virtual bool actual_growth();                                                 //991029
   virtual void update_lingers()                                  modification_; //071120
   virtual bool respond_to_freezing_damage                                       //081103
      (float64 fract_reduction_due_to_freezing)                   modification_  //081103
         { return true; }  // Currently normal crops simply die_back (done by caller) this is overridden by fruit crops
 protected:
   virtual Canopy_leaf_growth &provide_canopy()                      provision_;//190702

      // rename to provide_leaf_canopy

   virtual bool drop_leaves();                                                   //190624
   bool create_roots();                                                          //190102
public: // State variable accessors that are actually used by CropSyst
   virtual float64 get_canopy_resistance()                                const; //020409
      // The following accessors are not necessarily currently used by CropSyst.
      // They are provided for use by other models or output
          float64 get_carbon_fraction()                                   const; //060818
 public: // State variable accessors
   inline virtual const float64 *get_total_fract_root_length()             const { return roots_vital? roots_vital->get_total_fract_length_m()  : 0; } //061208
   inline float64 get_total_fract_root_length(nat8 sublayer)               const { return roots_vital? roots_vital->get_total_fract_length_m()[sublayer] : 0; }
   inline float64 get_active_fract_root_length(nat8 sublayer)              const { return transpiration ? transpiration->get_active_fract_root_length(sublayer) : 0; }
   // Stress
   inline float64 get_water_stress()                                       const { return water_stress_stats.get_current(); }
   inline float64 get_water_stress_index()                                 const { return water_stress_stats.index; }
   inline float64 get_temperature_stress()                                 const { return report_temperature_stress_stats.get_current(); }
   inline float64 get_temperature_stress_index()                           const { return report_temperature_stress_stats.index; }
   inline float64 water_stress_stats_append(float64 value)                       { return water_stress_stats.append(value); }
   inline float64 temperature_stress_stats_append(float64 value)                 { return report_temperature_stress_stats.append(value); }
   // Used for stress harvest index
   // Options
   // Growth limitations passed from potential growth to actual growth
   inline float64 get_attainable_top_growth()                              const { return attainable_top_growth; } //011023
   // Canopy cover
   /*200228
   float64 get_fract_canopy_cover_green()                                 const;
   float64 get_fract_canopy_cover_total()                                 const;
   */
   float64 get_canopy_interception_global_green()                         const; //200228
   float64 get_canopy_interception_global_total()                         const; //200228
   //200228 renamed float64 get__interception_global_green_canopy()                         const;
   // Leafs and canopy
   float64 get_peak_LAI(bool reported )                                   const; //141203
          float64 get_LAI(bool from_canopy)                               const; //060824
          float64 get_GAI_today_vital()                                   const; //050823
   inline float64 get_plant_height()                                       const { return plant_height; }
   // Root related variables
   inline float64 get_root_length()                                        const { return roots_current ? roots_current-> get_length()       : 0.0;} //190102
   virtual inline float64 get_recorded_root_depth_m()                      const { return roots_current ? roots_current->get_apparent_depth() : 0.0;} //190617_061208

   inline virtual float64     get_active_fract_root_length_m                     //050331
      (nat8 sublayer)                                                      const { return transpiration ? transpiration->get_active_fract_root_length(sublayer) : 0; }
   inline virtual const float64 *get_total_fract_root_length_m()           const
      { return
         #ifndef CROP_CALIBRATION
            Crop_complete::roots_current
            ? Crop_complete::roots_current->get_total_fract_length_m() :         //190102
         #endif
         0;
      }

   #ifdef NITROGEN
   virtual float64 get_N_mass_canopy_kg_m2(bool recorded_before_harvest)  const; //070209
   #endif
   inline float64 get_pot_root_biomass             ()                     const  { return pot_root_biomass; }
   inline float64 get_act_root_biomass             ()                     const  { return act_root_biomass; }
   // Transpiration
   inline float64 get_pot_transpiration            (bool CO2_adjusted)    const
   {
      //190826  actually already have get_pot_transpiration_m which replaces this
      return
      CO2_adjusted ? pot_transpiration_m_CO2_adj : pot_transpiration_m_unadj;
//190826      pot_transpiration_m[CO2_adjusted];
   } //151031
   inline float64 get_act_transpiration            ()                     const  { return act_transpiration_m; } //011023
   inline float64 get_act_uptake                   ()                     const; //NYI  { return act_uptake_m; } //011023
   // Stress
   inline float64 get_overall_growth_stress        ()                     const  { return overall_growth_stress; }
   // Daily phenology data:
   // Growth stages, dates, and special conditions
   inline virtual modifiable_ CropSyst::Phenology &mod_phenology()               { return phenology; } //130903
   inline virtual const       CropSyst::Phenology &ref_phenology()        const  { return phenology; } //130903
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   inline virtual Normal_crop_event_sequence get_growth_stage_sequence()  const  //110902
      { return phenology.get_growth_stage_sequence(); }                          //120820
   #endif
   #if ((PHENOLOGY_VERSION==2018))
   // caller should now reference phenology
   #endif
   inline const CORN::Date_const &get_harvest_date              ()         const { return latest_harvest_date; }
   inline const CORN::Date_const &get_unexpected_termination_date()        const { return unexpected_termination_date; }
   virtual inline bool is_terminate_pending() affirmation_{return terminate_pending;} //040514
   // Special events and conditions
   inline virtual bool is_fruit()                 affirmation_ { return false; } //190707
   inline virtual bool is_harvestable()        affirmation_{return harvestable;} //190710
   inline int16 get_days_of_expected_dormancy_remaining()                  const
                                 { return days_of_expected_dormancy_remaining; }
   virtual inline bool use_emergence_hydrothermal_time()            affirmation_
                                    { return emergence_hydrothermal_time != 0; } //140619
   inline const Crop_nitrogen_interface *get_nitrogen() const {return nitrogen;} //050722
   // Growth limitations passed from potential growth to actual growth
   inline float64 set_attainable_top_growth (float64 _attainable_top_growth)
                      { return attainable_top_growth = _attainable_top_growth; } //011023
   // Leafs and canopy

   inline float64 set_plant_height(float64 _plant_height)
      { plant_height = _plant_height ; return plant_height; }
   // Root related variables
   // Transpiration
   virtual float64 update_evapotranspiration_max(float64 ET_ref_m);              //010910

   // //             Calculates the pot_ET for this crop
   // //             Updates the pot_transpiration
   // //             Returns the pot ET for this crop.
   // Stress
   inline float64 set_overall_growth_stress(float64 overall_growth_stress_)
      { return overall_growth_stress = overall_growth_stress_ ; }
   // Daily phenology data:
   // Growth stages, dates, and special conditions
   inline const CORN::Date_const &set_latest_harvest_date(date32 harvest_date_)  //040624
      { latest_harvest_date.set_date32(harvest_date_); return latest_harvest_date; }
   inline const CORN::Date_const &set_unexpected_termination_date
      (const CORN::Date_const &unexpected_termination_date_)
      {  unexpected_termination_date.set(unexpected_termination_date_);
         return unexpected_termination_date; }
   inline virtual void set_terminate_pending()     { terminate_pending = true; } //040514
   // Special events and conditions
   inline int16 set_days_of_expected_dormancy_remaining
      (int16 days_of_expected_dormancy_remaining_)
      { days_of_expected_dormancy_remaining = days_of_expected_dormancy_remaining_ ;
        return days_of_expected_dormancy_remaining; }

   virtual float64 get_thermal_time_cumulative(/*200211obs bool adjusted_for_clipping*/) const;
 public: // Parameter data source house keeping
   #if (!defined(CROP_CALIBRATION))
   // This is output not needed for CropCalibration.
   // I probably will eventually create a subclass which includes these two function
   // since they the recording option is probably only applicated to CropSyst
   #if (CROPSYST_VERSION==4)
   virtual bool setup_structure
      (CORN::Data_record &data_rec,bool for_write);
   #endif
   inline virtual const char *get_record_section_label() const { return "crop_base";}
   #endif
   inline CropSyst::Crop_parameters *get_parameters() const {return parameters;} //020409
                  // Will be owned by the crop object
   inline virtual void      *get_unique_identifier() const { return parameters;}
 protected:
   float64 no_nitrogen_calculate_limited_pot_transpiration()      modification_; //160321
   bool update_temperatures()                                /*rectification_*/; //181228
   virtual bool check_for_automatic_clipping                                     //190616
   (const CropSyst::Auto_clip_biomass_mode &clip_biomass_op                      //051105
   ,float64 remove_amount_kg_m2              // if harvest_amount_mode=HARVEST_REMOVE_FIXED_AMOUNT_BIOMASS  else 0.0 //050822
   ,bool clip_now_or_never);
   virtual                                                                       //991029
      Crop_mass_fate_biomass_and_nitrogen *                                      //070625
      clip_biomass
      (Harvest_amount_mode harvest_amount_mode                                   //050822
      ,float64 remove_amount_kg_m2                                               //050822
      ,float64 min_retain_GAI                                                    //051102
      ,float64 min_retain_biomass_kg_m2                                          //110603
      ,const Biomass_fate_parameters &biomass_fate_parameters
      ,Biomass_fated_cause fated_cause);                                         //040610
      // Returns the fate of the biomass (must be deleted by the caller)         //060323
 public: // interface implementations  (moved from Crop_interfaced_complete)     //190617
   inline virtual const Reported &ref_reported()      const { return reported; } //190705
   inline virtual const Crop_mass_fate *get_biomass_fate_today_record()   const
      { return biomass_fate_today;}                                              //040624
      ///< get_biomass_fate_today() may be called usually once a day at the end of the daily time step loop
      /// to get the daily total biomatter fates.
   inline virtual bool        set_no_more_clippings_this_season()  modification_ //040721
      {  return no_more_clippings_this_season = true;                          } //071111
   inline virtual float64    param_max_root_depth_m()                     const  //190617
      {  return get_parameters()->root.max_depth; }
   inline virtual float64     get_plant_height_m()                        const  { return plant_height; }
   inline virtual float64     get_intercepted_precipitation_m()           const  { return intercepted_precipitation_m_today; }   //190617 intercepted_precipitation_m; }
   inline virtual float64 get_leaf_water_pot()                            const  { return transpiration ? transpiration->get_leaf_water_pot() : 0; } //071115
   inline virtual float64     get_root_length_m()                         const  { return get_root_length(); }
   inline virtual Harvested_part get_harvested_part()                     const  //170820
      { return get_parameters()->harvested_part_clad.get(); }
   #ifdef RESIDUES
   inline virtual Residue_decomposition_parameters &get_residue_decomposition_parameters() const
     { return get_parameters()->residue_decomposition; }                         //060219_110613
   #endif
   inline virtual Crop_mass_fate *get_biomass_fate_season()               const  { return biomass_fate_season;}   //040624
         // was not const so GCC may complain
   inline virtual float64 get_canopy_biomass_kg_m2()                      const  { return get_canopy_biomass_kg_m2_including(include_vital|include_effete); }
   inline virtual float64 get_canopy_biomass_kg_ha()                      const  { return per_m2_to_per_ha(get_canopy_biomass_kg_m2()); }
   inline virtual bool        get_no_more_clippings_this_season()         const  { return no_more_clippings_this_season; }   //040721
   float64 param_emergence_deg_day()                                      const;
   inline virtual float64    param_max_LAI()                              const  { return get_parameters()->canopy_growth_LAI_based.max_LAI;}
   virtual bool track_nitrogen_mass_balance                                      //070220
      (Crop_nitrogen_mass_balancer *_mass_balance)                    const; //200715LML mutation_;
   virtual float64   param_wilt_leaf_water_pot()                          const  { return get_parameters()->param_wilt_leaf_water_pot(); }
   inline virtual float64 get_C_mass_roots()                              const  { return C_mass_root_est; }      //110915_060818
   inline virtual float64 get_act_root_biomass_kg_m2()                    const  { return act_root_biomass; }
   #ifdef THERMAL_TIME_OBSOLETE_2020
   inline virtual float64 get_accum_degree_days(bool adj_for_clipping)    const  { return THERMAL_TIME_SOURCE get_accum_degree_days(adj_for_clipping); } //040830
   #else
   inline virtual float64 get_accum_degree_days()                         const  { return THERMAL_TIME_SOURCE get_accum_degree_days(); } //040830
   #endif
   inline virtual float64 get_latest_yield_kg_m2()                        const  { return biomass_fate_today ? biomass_fate_today->get_total_harvested() : 0.0; }
   virtual float64 get_root_biomass
      (float64 *output_root_biomass_by_layer=0)                           const; //071215
   inline virtual float64      get_VPD_daytime()                                 const { return daytime_vapor_pressure_deficit.get_kPa(); } //140813
   virtual float64 update_root_depth
      (float64 soil_rooting_depth,float64 total_soil_depth)       modification_; //190819_981023
   inline virtual float64 get_intercepted_PAR_MJ_m2()                      const { return intercepted_radiation; }
   inline virtual float64 get_intercepted_PAR_accum_MJ_m2()                const { return intercepted_radiation_accum; }
   inline virtual float64 get_pot_transpiration_m(bool CO2_adjusted)       const
   { return
        CO2_adjusted ? pot_transpiration_m_CO2_adj : pot_transpiration_m_unadj;
   } //151031
   inline virtual float64 get_VPD_daytime_mean()                           const { return vapor_pressure_deficit_stats.get_mean(); }
   inline virtual float64 get_intercepted_PAR_season_accum_MJ_m2()         const { return intercepted_radiation_season_accum; }
   inline virtual float64 get_water_stress_index_mean()                    const { return 1.0- water_stress_stats.get_mean(); ; }
   inline virtual float64 get_temperature_stress_index_mean()              const { return 1.0- report_temperature_stress_stats.get_mean(); }
   inline virtual float64 get_C_mass_canopy()                              const { return C_mass_canopy_est; }    //110915_060818
   inline virtual float64 get_act_transpiration_m()                        const { return act_transpiration_m; }
   inline virtual float64 get_act_uptake_m()                               const { return transpiration ? transpiration->get_uptake_actual() : 0; } //191011_160414
   inline virtual float64 get_act_root_biomass_kg_ha()                     const {  return over_m2_to_ha * act_root_biomass; }

   #ifdef VIC_CROPSYST_VERSION
   #ifdef CROPSYST_HANDLE_CANOPY_EVAP
   virtual float64 get_intercepted_precipitation_m_stored()               const; //180523LML
   virtual bool    set_intercepted_precipitation_m_stored
      (float64 intercepted_water_m)                               modification_; //180523LML
   #endif
   #endif
   #if ((DIRECTED_IRRIGATION>=2014) && (DIRECTED_IRRIGATION<=2016))
   virtual bool know_directed_irrigation_soil_wetting
      (const Directed_irrigation_soil_wetting *_soil_wetting)        cognition_;
   #endif
 protected:
    #ifdef REACCH_VERSION
    Local_time dummy_sun_rise_time;                                              //130711
    // For some unknown reason ref_sun_rise_time() is not referening correctly
    // We are currently using the sun_rise_time option for the exteme temperature response so this is disabled
    // This will be obsolete when I implement Weather_provider
    #endif
   Crop_complete();
   RENDER_INSPECTORS_DECLARATION;                                                //150916
   CORN::OS::Directory_name *crop_ID_directory; //owned                          //160609
   bool take_crop_ID_directory(CORN::OS::Directory_name *crop_ID_directory);     //160609
};
//_Crop_complete_______________________________________________________________
}//_namespace CropSyst_________________________________________________________/
#endif

