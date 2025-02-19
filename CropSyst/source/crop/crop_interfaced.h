#ifndef crop_interfacedH
#define crop_interfacedH
#include "corn/const.h"
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "common/soil/SCS/SCS.h"
#include "common/residue/residue_decomposition_param.h"
#include "mgmt_types.h"
#include "crop/crop_types.hpp"
#include "crop/crop_N_interface.h"                                               //170220LML
namespace CS
{  class Weather_hours;
}
//______________________________________________________________________________
namespace CropSyst {
   class Phenology;                                                              //130903
   class Auto_clip_biomass_mode;
   class Harvest_or_clipping_operation_struct;                                   //140811
   class Crop_nitrogen_mass_balancer;
}//_namespace CropSyst_________________________________________________________/
class Residues_interface;

#include "options.h"

#include "crop/crop_types.h"
#include "corn/chronometry/date_types.hpp"
#include "common/soil/SCS/SCS.h"
#include "crop/growth_stages.hpp"
#include "common/weather/parameter/WP_air_temperature.h"
#include "corn/chronometry/time_types.hpp"
#include "CS_suite/observation/CS_observation.h"

/* This class defines all the methods for
   connecting a crop model with CropSyst framework.                              031119

   In order to incorporate a new crop model into
   the CropSyst frame work, it must be derived from this
   class and provide overrides to all these methods.
*/
//______________________________________________________________________________
namespace CORN
{
   class Data_record;       // Forward declaration
}
//______________________________________________________________________________
class Residue_decomposition_parameters;                                          //060219
namespace CS {  // only V4
   class Desired_variables; // Forward declaration
}
class Crop_nitrogen_state_abstract;
class Common_simulation_log;                                                     //050725
class Residues_interface;                                                        //060331
class Crop_nitrogen_interface;
namespace CropSyst {
//200325 extern CORN::Date_clad_32 no_date;                                               //190620
//______________________________________________________________________________
struct Reported
{
   struct Daily
   {
      float64 LAI;     // Leaf area index
      float64 GAI;     // Green area index
      float64 canopy_biomass;
      float32 canopy_cover_green; // fraction
      float32 canopy_cover_total; //fraction                                     //200127
      float64 root_depth;
      float64 LAI_for_self_shading_response;
      float64 transpiration_act_m;     // was  report_transpiration_act_m
      // The reported act_transpriation is constrained to > 0.
      float64 ET_max_m; // was reported_ET_pot_m;
      float64 ET_act_m; // was reported_ET_act_m
    public:
      inline Daily()                                                { clear(); }
      inline bool clear()
      {
         LAI                           = 0;
         GAI                           = 0;
         canopy_biomass                = 0;
         canopy_cover_green           = 0;
         canopy_cover_total            = 0;                                      //200127
         root_depth                    = 0;
         LAI_for_self_shading_response = 0;
         transpiration_act_m           = 0;
         ET_max_m                      = 0;
         ET_act_m                      = 0;
         return true;
      }
   } daily;
   //_Daily____________________________________________________________________/
   struct Season
   {
      float64 LAI_max;// The actual peak_LAI gets adjusted at the harvest event //040929
      float64 GAI_max;
      float64 N_mass_yield;
      float64 biomass_production;      // kg/m2  / was total_season_biomass_production_output
         // Essentially the current canopy biomass plus any biomass removed.
      float64 biomass_running_accum;   // kg/m2 //was  total_season_biomass_running_accum_output; //040719
         // Essentially the current canopy biomass plus any biomass removed.
      CORN::Date_clad_32 GAI_max_date;                                           //180413_171115
      float32 canopy_cover_max;    // fraction
        // reported_canopy_cover_max is just a temporary hack
        // because I already have and inspector, but Claudio want this
        // in the old season report
    public:
      inline Season()                                                 {clear();}
      inline bool clear()
      {
         LAI_max                 = 0;
         GAI_max                 = 0;
         N_mass_yield            = 0;
         biomass_production      = 0;
         biomass_running_accum   = 0;
         GAI_max_date            .clear();
         canopy_cover_max        = 0;
         return true;
      }
   } season;
   //_Season___________________________________________________________________/
 public:
   inline Reported()                                                          {}
};
// the reported data structure is used to store values which are not
// necessarily computed to be used by the model itself, but are special values
// typically used for output.
//_struct_Reported__________________________________________________2019-07-05_/
interface_ Crop_model_interface
{  // Eventually rename this but currently I have an enumeration named Crop_model
   virtual bool check_for_automatic_clipping                                     //990913
      (const Auto_clip_biomass_mode &clip_biomass_op                             //970521
      ,float64 remove_amount_kg_m2                                               //050822
      ,bool   clip_now_or_never)                                             =0; //980724
      // Returns true if clipping actually performed.                            //990913
   virtual                                                                       //991029
      Crop_mass_fate_biomass_and_nitrogen *                                      //070625
      clip_biomass
      (Harvest_amount_mode harvest_amount_mode                                   //050822
      ,float64 remove_amount_kg_m2                                               //050822
      ,float64 min_retain_GAI                                                    //051102
      ,float64 min_retain_biomass_kg_m2                                          //110603
      ,const Biomass_fate_parameters &biomass_fate_parameters
      ,Biomass_fated_cause fated_cause)=0;                                         //040610
      // Returns the fate of the biomass (must be deleted by the caller)         //060323

   virtual bool         set_no_more_clippings_this_season()     modification_=0; //040721

   virtual float64 update_evapotranspiration_max(float64 ET_ref_m) rectification_=0; //190812_010910
      //190812 update_pot_evapotranspiration
   virtual bool get_name(std::wstring &name)                            const=0; //200324
 public: // simulation element
   virtual ~Crop_model_interface()                                            {} //190721
   virtual bool initialize()                                  initialization_=0; //151005
   virtual bool start_day()                                     modification_=0; //130903_040205
   virtual bool end_day()                                       modification_=0; //130903_070221
   virtual bool process_day()                                   modification_=0; //150611_020711
 public:
   // The following methods provided daily parameters required by optional submodels
   // When using any of these models,
   // the respective methods must be called prior to calling process().
   virtual float64 know_snow_pack_thickness
      (float64 snow_pack_thickness_)                               cognition_=0; //161110
 public: // specific processes
   virtual float64 intercept_precipitation
      (float64 water_reaching_plant)                            modification_=0; //140320
      /// Deducts from the water reaching the plant, water intercepted
      /// by the plant.
      /// (I need to check the fate of this intercepted water
      //   is it just lost?)
      /// \return water not stored on plant.
   virtual bool end_season_if_no_dormancy()                                  =0; //050818
   virtual void set_terminate_pending()                                      =0;
 public:  // The following are used to implement operations and events that remove biomatter from the plant
   virtual                                                                       //040621
      Crop_mass_fate_biomass_and_nitrogen *                                      //070625
      process_biomass_fate
         (Biomass_fated_cause fated_cause
         ,Harvest_amount_mode harvest_amount_mode                                //050822
         ,float64 remove_amount_kg_m2                                            //050822
         ,float64 retain_GAI                                                     //050822
         ,float64 retain_biomass_kg_m2                                           //110603
         ,const Biomass_fate_parameters  &biomass_fate_parameters
         ,bool terminate                                                         //040817
         ,float64 yield_now)                                                 =0;
         // from harvest index, may be 0.0
      // This moves plant biomass to various pools, harvest yield, residue, loss etc..
      // \returns the fate of the material if processed otherwise 0 (call must delete the returned fate object)
 public:
      /// The following are processes used by the calling program.
      /// They are almost always used.
      /// Some are required!
   virtual bool plant/*190621 sow*/(bool fall_sowing)                        =0; //190621_140813_020220
      ///< plant() must be called once on the date the crop is to be planted.
      /// used to be called sow, but it could be any type of planting
   virtual Crop_mass_fate_biomass_and_nitrogen *harvest                          //070625
      (float64 remove_amount_kg_m2                                               //050822
      ,const CropSyst::Harvest_or_clipping_operation_struct *harvest_or_clipping_params
      ,bool terminate)                                                       =0; //051105_140811
      ///< For annuals harvest() is usually called once at some point at
      /// the end of the season (I.e. 10 days after maturity) (usually specify terminate).
      /// For perennials or clipped crops, this can be called throughout the
      /// season (based on favorable clipping conditions or scheduled dates)
      /// with terminate=false  (unless you finally terminate the crop).
      /// See also terminate() which will be called if the terminate parameter is true.
      /// In CropSyst, this method is called when management parameters
      /// schedule harvest for the day.
      /// \return the fate of the biomass if it could harvest (relinquished to caller)
      // otherwise 0 //060323
   virtual float64 update_root_depth
      (float64 soil_rooting_depth,float64 total_soil_depth)     modification_=0; //981023
 public: // Useful calculations
   virtual float64 est_degree_days_X_days_from_now                               //000330
      (const CORN::Date_const & Date_const, CORN::Days days_from_now) const{return 0.0;} //010202
      ///< This returns an estimate (in this case the actual)
      /// of the thermal time the crop will have in the specified
      /// number of days from the current date.
      /// It doesn't do anything to the crop.
      /// It is used by CropSyst to predict emergence or
      /// anticipate crop growth stages.

 public: // access parameters
   virtual float64  param_max_root_depth_m()                            const=0;
   virtual float64  param_emergence_deg_day()                 const{return 0.0;}
      // 0 indicates this parameter is not available and will disable
      // computed planting date mode
      // Derived class should implement if

   virtual float64  param_max_LAI()                                     const=0;
   inline virtual float64  param_wilt_leaf_water_pot()        const{return 0.0;}

 public: // access values
   virtual float64 get_plant_height_m()                                 const=0;
   virtual float64 get_canopy_interception_global_total()               const=0; //200228_171006
   virtual float64 get_adjusted_ET_crop_coefficient()                   const=0; //200323
   //200228 virtual float64 get_fract_canopy_cover_total()                       const=0; //171006
   virtual float64 get_LAI(bool from_canopy)                            const=0;
   virtual float64 get_intercepted_precipitation_m()                    const=0; //160419
   virtual float64 get_leaf_water_pot()                                 const=0; //071115
   virtual float64 get_root_length_m()                                  const=0;
   virtual float64 get_recorded_root_depth_m()                          const=0;
   virtual float64 const *get_total_fract_root_length_m()               const=0;
   virtual Harvested_part get_harvested_part()                          const=0; //170820
   virtual float64 get_canopy_biomass_kg_ha()                           const=0;
   virtual float64 get_canopy_biomass_kg_m2()                           const=0;
   //190705 moved to reported virtual float64 get_total_season_biomass_production_kg_m2()          const=0; //040719
   virtual float64 get_act_root_biomass_kg_m2()                         const=0;
   virtual float64 get_water_stress()                                   const=0;
   virtual float64 get_water_stress_index()                             const=0;
   virtual float64 get_temperature_stress()                             const=0;
   virtual float64 get_temperature_stress_index()                       const=0;
   #ifdef THERMAL_TIME_OBSOLETE_2020
   virtual float64 get_accum_degree_days(bool adjusted_for_clipping)    const=0; //040830
   #else
   virtual float64 get_accum_degree_days()                              const=0; //200211
   #endif
 public: // used only for output
   virtual const Crop_mass_fate *get_biomass_fate_today_record()        const=0; //040624
      ///< get_biomass_fate_today() may be called usually once a day at the end of the daily time step loop
      /// to get the daily total biomatter fates.
   virtual float64      get_active_fract_root_length_m(nat8 sublayer)   const=0;
   virtual float64 get_root_biomass
      (float64 *output_root_biomass_by_layer=0)                         const=0; //071215
   inline virtual float64 get_dry_biomass_kg_m2()                       const
      { return get_canopy_biomass_kg_m2(); }                                     //200609
   inline virtual float64 get_dry_biomass_kg_ha()                       const
      { return over_m2_to_ha * (get_dry_biomass_kg_m2()); }                      //200609_190618
   //200609   { return over_m2_to_ha * (get_canopy_biomass_kg_m2()); }                   //190618
   virtual float64 get_latest_yield_kg_m2()                             const=0;
   virtual float64 get_live_green_canopy_biomass_kg_m2(nat8 include_biomass) const=0; //060313
   inline virtual float64 get_live_green_canopy_biomass_kg_ha()         const
      { return over_m2_to_ha
         * (get_live_green_canopy_biomass_kg_m2(include_vital|include_effete));}
   virtual float64 get_GAI(nat8 include_GAI)                            const=0;
   virtual float64 get_VPD_daytime()                                    const=0;

   virtual float64 get_intercepted_PAR_MJ_m2()                          const=0;
   virtual float64 get_intercepted_PAR_accum_MJ_m2()                    const=0;
   // warning these need to be renamed to get_intercepted_radiation_XXXX

   // season output
   virtual float64      get_canopy_interception_global_green()          const=0; //200228_171006
   //200228 virtual float64      get_fract_canopy_cover_green()                  const=0; //171006
   virtual float64      get_pot_transpiration_m(bool CO2_adjusted)      const=0; //151031
   virtual const CORN::Date_const &get_harvest_date()                   const=0;
   virtual float64 get_peak_LAI(bool reported)                          const=0; //040929
   virtual float64 get_VPD_daytime_mean()                               const=0;
   virtual float64 get_intercepted_PAR_season_accum_MJ_m2()             const=0;
   virtual float64 get_water_stress_index_mean()                        const=0;
   virtual float64 get_temperature_stress_index_mean()                  const=0;
   //#ifdef NITROGEN
   virtual float64 get_N_mass_canopy_kg_m2(bool recorded_before_harvest) const{return 0;} //070209
   //#endif
   virtual float64 get_C_mass_canopy()                          const{return 0;} //110915
   virtual float64 get_C_mass_roots()                           const{return 0;} //110915
   virtual float64 get_act_transpiration_m()                            const=0;
   virtual float64 get_act_uptake_m()                                   const=0; //160414

 public: // fruit will implement this

   // used only for fruit output
   virtual float64 get_fruit_dry_biomass_kg_ha()                const{return 0;}
   virtual float64 get_fruit_reserves_biomass_kg_m2()           const{return 0;} //200608
   virtual float64 get_fruit_reserves_biomass_kg_ha()           const{return 0;} //200608
   virtual const CORN::date32 get_bud_break_date()              const{return 0;}
   virtual float32 get_fruit_chill_hours_remaining()            const{return 0;}
   virtual float64 get_act_root_biomass_kg_ha()                         const=0;
   virtual float64 get_orchard_solid_canopy_interception()      const{return 0;}
   virtual float64 get_orchard_shaded_area_GAI()                const{return 0;}
   virtual float64 get_orchard_canopy_porosity()                const{return 0;}

 #ifdef VIC_CROPSYST_VERSION
 #ifdef CROPSYST_HANDLE_CANOPY_EVAP
 public:
   virtual float64 get_intercepted_precipitation_m_stored()             const=0; //180523LML
   virtual bool    set_intercepted_precipitation_m_stored
      (float64 intercepted_water_m)                             modification_=0; //180523LML
 #endif
 #endif

 public: // access structures
   virtual CropSyst::Phenology &mod_phenology()                              =0; //130903
   virtual const CropSyst::Phenology &ref_phenology()                   const=0; //130903
   #ifdef RESIDUES
   virtual Residue_decomposition_parameters &get_residue_decomposition_parameters() const=0;
   #endif
   virtual const Crop_nitrogen_interface *get_nitrogen()                const=0; //050722
   virtual Crop_mass_fate *get_biomass_fate_season()                    const=0; //040624 //150803 was not const (GCC may complain)
      ///< get_biomass_fate_season() may be called (usually once) at the end of the season
      /// to get the seasonal total biomatter fates.
 public: // affirmations
   virtual bool is_terminate_pending()                           affirmation_=0; //040514
   virtual bool is_fruit()                                       affirmation_=0; //190707
   virtual bool is_harvestable()                     affirmation_{return false;} //190710
      // Derived classes (fruit) would override when harvestable conditions are satisified.
      // The generic CropSyst crop model doesn't use this
   virtual bool get_no_more_clippings_this_season()              affirmation_=0; //040721
   virtual bool is_at_max_fruit_load()               affirmation_{return false;} //190620_081103
   virtual bool is_at_season_start()                             affirmation_=0; //200404
   virtual bool is_at_season_end()                               affirmation_=0; //200404
 public: // assignations
   #ifdef NITROGEN
   virtual void set_automatic_nitrogen_mode(bool enable) = 0;                    //040525
   #endif
 public: // metadata
   virtual const char *get_description()                                const=0;
   /*200420 replaced by  describe_periods
   virtual const char *describe__growth_stage
          (std::string &growth_stage_phrase)                            const=0; //190613
   */
   virtual nat8 describe_periods(CORN::Text_list &descriptions)         const=0;

 public: // probably replace with inspectors
   virtual const Reported &ref_reported()                               const=0; //190705
   virtual bool track_nitrogen_mass_balance
      (CropSyst::Crop_nitrogen_mass_balancer *_mass_balance)            const=0; //070220
 public: // connecting components
   inline virtual void know_residues(Residues_interface *residues)               //060531
      { UNUSED_arg(residues); }
      // This method allows the crop to interact and exchange material with residue pools.
      // This function is usually called once immediately after instanciating the crop object. //050725
      // This is optional, it allows the decomposing canopy submodel to be enabled.
      // In CropSyst, if the crop does not know about residues,
      // scenesced biomass in the plant canopy will not decompose while the plant is alive (I.e. for perennials)
      // (although biomass will be available for decomposition on crop termination.)
   inline virtual void know_weather_hourly
      (const CS::Weather_hours *_hourly_weather)                              {}

 public: // The following are added from intercropping
   inline virtual Crop_model_interface *get_predominant_crop() modification_{return this;} //130715
   inline virtual Crop_model_interface *get_secondary_crop()   modification_{return 0;}    //130715
 public: // inspection
   #if (CROPSYST_VERSION==5)
   RENDER_INSPECTORS_DECLARATION_PURE;                                           //20508
   #endif
};
//_interface_Crop_interfaced_model__________________________________2019-06-16_/
}//_namespace CropSyst_________________________________________________________/
#endif

