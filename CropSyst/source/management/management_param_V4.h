#ifndef management_param_V4H
#define management_param_V4H
#include "options.h"
#include "management/management_param_class.h"
#if (MANAGEMENT==4)
namespace CropSyst {
//______________________________________________________________________________
class Inorganic_nitrogen_operation
: public  Inorganic_nitrogen_operation_class  // CropSyst_field_operation may be sufficient
{public:
   Valid_float32 v_NO3_N;
   Valid_float32 v_NH4_N;
   Valid_float32 v_NH4_volatilization;
 public:
   Inorganic_nitrogen_operation();
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_;   //970709
   virtual void log(std::ostream &log_file)                               const; //020810
};
//_class Inorganic_nitrogen_operation_______________________________1998-12-12_/
}//_namespace CropSyst_________________________________________________________/

//______________________________________________________________________________
#include "common/biomatter/biomass_decomposition_param.h"
#include "corn/validate/validtyp.h"

#define LABEL_automatic_irrigation_period_1 "automatic_irrigation_period_1"

//______________________________________________________________________________
class Harvest_or_clipping_event; // forward declaration  (only for V3 compatibility)
class Automatic_clip_biomass_mode_event; // forward declaration  (only for V3 compatibility)
class Automatic_NO3_appl_mode_event;

namespace CropSyst {                                                             //120419
class N_application_soil_observation_mode_event;

//______________________________________________________________________________
class Tillage_operation
: public Tillage_operation_class
{
 public:
   CORN::Valid_float32 v_dust_mulch_depth_cm;                                    //051206
   CORN::Valid_int16   v_dust_mulch_effective_days;                              //100121
   CORN::Valid_float32 v_dust_mulch_intensity;  // 0 to 1 where 0 is not effect and 1 is most intense value 051206
   CORN::Valid_float32 v_oxidation_effect_sand; // 1.5 is low  6.0 is high       //100121
   CORN::Valid_float32 v_oxidation_effect_clay; // 0.5 is low  2.0 is high       //100121
 public: // values computed during simulation

   float64 straw_to_residue;  // Not sure how used (for schedule output?)  I think obsolete with new biomas fate //990414P
 public:
   Tillage_operation();
   VIRTUAL_VOID_SETUP_PARAMETERS;
};
//_class Tillage_operation__________________________________________2011-01-13_/
class Residue_operation
: public Tillage_operation                                                       //020524
{
 public:  // constructor
   Residue_operation(); // section setup constructor
 public: // virtual methods
   //170424 replaced with label_string virtual const char *label_cstr(char *buffer)                const; //030121
   virtual const char *label_string(std::string &buffer)                  const; //170423
   virtual void log(std::ostream &log_file)                               const; //020810 temporarily abstract so I make sure to implement
   inline virtual int compare(const Item &other) const {UNUSED_arg(other);return 0;} // At this point nothing really to compare eventuall compare phen sync;
   inline virtual int32 /*enumXX */get_type() const {return CS_OP_MGMT_RESIDUE;} //011207
};
//110115 is obsolete in version 5
//_class Residue_operation__________________________________________2004-01-24_/
class Organic_nitrogen_operation_abstract
: public Organic_nitrogen_operation_abstract_class
{
public:
   CORN::Valid_float32 v_org_N;                                                        //060717
   CORN::Valid_float32 v_NH3_N; //   {NH4 (kg/ha), converted to kg/m2 when event table is genrated}  //060717
   CORN::Valid_float32 v_dry_matter;                                                   //060717
   CORN::Valid_float32 v_long_term_org_N_volatilization_loss_percent;                  //020525
 public: // constructor
   Organic_nitrogen_operation_abstract(CropSyst_Op cs_op);
   virtual void log(std::  ostream &log_file)                             const;
   VIRTUAL_VOID_SETUP_PARAMETERS;
};
//_class Organic_nitrogen_operation_abstract________________________2011-01-13_/
#if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION < 5))
// This mode is for compatibility with previous versions
// I was planning on making these deprecated replaced with
// Organic_matter_composition_decomposition in version 5,
// but Claudio is still using this set of parameter in the VB version,
// also for CAFE dairy.
//
// Organic_nitrogen_operation has been replaced with Biomatter_application_operation in V5
class Organic_nitrogen_operation
: public Organic_nitrogen_operation_abstract                                     //080902
{
   friend class Management_parameters;                                           //040830
 public:
   Solid_liquid_form_clad     solid_liquid_form_clad;                            //020322
   bool                       org_N_volatilization_calculated;
      // Force user supplied NH3_volatilization to be used overriding any //020252x
                                 // selections made by the radio buttons and
                                 // computed by the model.
   //200220obs bool                       decomposition_time_calculated;                     //990227P
   /*200220obs
   int16                      decomposition_time_63;
      // The number of days it takes to decompose to 63%
      // (this is being replaced with 50% basis and is here only for import purposes) //040830
   */
   int16                      halflife;                                          //040830
      //  The number of days it takes to decompose to 50%
   float32                    carbon_fraction;  // around 0.5                    //080225
 public: // constructor
   Organic_nitrogen_operation() ;
 public: // virtual methods
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
   virtual const char *label_string(std::string &buffer)                  const; //170434
   virtual void log(std::ostream &log_file)                          stream_IO_; // const; //020810
   inline  virtual int32 get_type() const { return CS_OP_MGMT_ORGANIC_NITROGEN;} //011207
 public:
   CORN::Valid_float32 v_carbon_fraction;                                        //080225
   //200220 CORN::Valid_int16   v_decomposition_time_63;
   CORN::Valid_int16   v_halflife;                                               //040826
 public: //accessors
   virtual bool is_liquid()                                               const; //080903 { return solid_liquid_form_labeled.get() > SOLID; } //020322
};
#endif
//_class Organic_nitrogen_operation_________________________________1998-??-??_/
DECLARE_Biomatter_application_operation
//______________________________________________________________________________
#define HARVEST_CLIP_VALID_ENTRIES                                             \
   Valid_float32  v_min_LAI_required;                                          \
   Valid_float32  v_min_biomass_required_kg_ha;                                \
   Valid_float32  v_reserve_biomass_kg_ha;                                     \
   Valid_float32  v_remove_amount_kg_ha;                                       \
   Valid_float32  v_min_retain_GAI;                                            \
   Valid_int16    v_remove_for_use_percent;                                    \
   Valid_int16    v_remove_for_grazing_percent;                                \
   Valid_int16    v_biomass_returned_as_manure;                                \
   Valid_int16    v_remove_for_disposal_percent;                               \
   Valid_int16    v_remain_as_residue_percent;                                 \
   Valid_int16    v_remain_standing_live_percent;                              \
   Valid_int16    v_remain_standing_dead_percent;                              \
   Valid_int16    v_remain_roots_as_dead_residue_percent;                      \
   Valid_int16    v_remove_roots_as_loss_percent;                              \
   Valid_int16    v_remain_roots_live_percent;                                 \
   Valid_int16    v_grazing_percent_N_as_OM;                                   \
   Valid_int16    v_grazing_percent_N_as_NH3;
//______________________________________________________________________________
class Harvest_or_clipping_operation
: public virtual Harvest_or_clipping_operation_class
{
 public:
#ifndef __linux__
// g++ compiler is having problems with friendship so prior version importing is not supported in Linux
   float32              retain_GAI_obsolete_V4_3;                                //050822 Now just use min         // Amount of GAI m?/m? to leave in the field after cutting.
   float32              max_retain_GAI_obsolete;                                 //051103 remove in V5    // Maximum amount of GAI m?/m? to leave in the field after cutting. (for percent_biomass mode)

// g++ compiler is having problems with friendship so prior version importing is not supported in Linux
 // These are from version 3 they are used only for importing            //051103
   V3_Biomass_fate_clad V3_biomass_fate_clad;  // Rename this to harvest mode   //020322
   int16          V3_trim_biomass_removed;      //{ % 0-100    added trim }      //960106
   int16          V3_trim_removed_to_surface;   //{ % 0-100    of the amount trimmed how much biomatter is returned to the surface (plant material or grazing manure) } 960106
   Valid_float32  v_retain_GAI_obsolete;                                         //050822
   Valid_float32  v_max_retain_GAI_obsolete;  // remove in V5                    //051105
   Valid_int16    v_V3_trim_biomass_removed;
   Valid_int16    v_V3_trim_removed_to_surface;
#endif
   HARVEST_CLIP_VALID_ENTRIES                                                    //120502
   inline virtual int16 get_SCS_code()                                     const { return  89;} //990516
      // Clipping operation is always 89 (This is reserved by CropSyst and not used by SCS).
 public:
   Harvest_or_clipping_operation(); // section setup constructor
   virtual ~Harvest_or_clipping_operation()                                   {}
   VIRTUAL_VOID_SETUP_PARAMETERS;
};
//_class Harvest_or_clipping_operation______________________________2011-01-XX_/
class Auto_clip_biomass_mode
: public virtual Auto_clip_biomass_mode_class
, public virtual Harvest_or_clipping_operation                                   //150404
{public:
   HARVEST_CLIP_VALID_ENTRIES                                                    //120502
   Valid_float32  v_adjust_relative_growth_rate_for_clipping;
   Valid_float32  v_LAI_forces_clipping;
   Valid_float32  v_biomass_forces_clipping;
   Valid_int16    v_flowering_forces_clipping;
   Auto_clip_biomass_mode();                                                     //120419
   V3_Automatic_clipping_mode_clad V3_automatic_clipping_mode_clad; // for V3 compatilibity only 980826P
   virtual void setup_parameters(CORN::Data_record &data_rec,bool for_write) modification_;   //970709
   inline virtual int16 get_SCS_code()                                     const { return  89;} // Clipping operation is always 89 (This is reserved by CropSyst and not used by SCS). // 990516_
   virtual void log(std::ostream &log_file)                                const;
};
//_class Auto_clip_biomass_mode_____________________________________2011-01-XX_/
class Management_parameters
: public Management_parameters_class
{
public:
#ifdef NYI
   Harvest_or_clipping_event         *V3_harvest_date;
      // used only for V3 import (will be 0 if not V3) //040523
   Automatic_clip_biomass_mode_event *V3_auto_clip_biomass_event;
      // used only for V3 import (will be 0 if not V3) //040631
   CORN::Date                         V3_latest_date_to_harvest_relative;
      // Perhap move to Autoclipping? //990421
//             latest date, is now harvest period for automatic clipping, or entereded as a terminating harvest/clipping event

// This mode is for compatibility with previous versions
// These parameters are deprecated and will be removed in version 5
// 110104 However, I need them for CAFE diary
   Common_event_list V3_clipping_operations;  // Used only to import V3 files    //040524
 private: // The following are provided for version 3 import capability          //020922
   bool              V3_auto_irrig_enabled;                                      //020922
#endif
   Common_event_list organic_nitrogen_operations;                                //981228
 public:  // validators
   Management_parameters();                                                      //120419
   Valid_float32  v_soil_conservation_factor;
   virtual bool get_end()                                         modification_;//161025_020922
   virtual nat32 fertilization_nitrogen_multiply_by(float multiplier) modification_;//070716
   virtual bool expect_structure(bool for_write = false);                        //161025_981007
};
//_class Management_parameters______________________________________2011-01-XX_/
#define IRRIGATION_VALID_ENTRIES                                               \
   Valid_float32 v_max_allowable_depletion;                                    \
   Valid_float32 v_depletion_observe_depth;                                    \
   Valid_float32 v_depletion_observe_root_zone_fract;                          \
   Valid_float32 v_min_application;                                            \
   Valid_float32 v_max_application;                                            \
   Valid_float32 v_leaf_water_potential;                                       \
   Valid_float32 v_amount;                                                     \
   Valid_float32 v_salinity;                                                   \
   Valid_float32 v_ECw_to_TDS;                                                 \
   Valid_float32 v_NO3_N_concentration;                                        \
   Valid_float32 v_NH4_N_concentration;                                        \
   Valid_float32 v_net_irrigation_mult;                                        \
   Valid_float32 v_refill_point;                                               \

/*200225
   Valid_float32 v_surface_evaporation_area_fraction;                          \
   Valid_float32 v_profile_volume_fraction;
*/
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Irrigation_operation
: public Irrigation_operation_class
{
public:
   IRRIGATION_VALID_ENTRIES                                                      //120503
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
   virtual void log(std::ostream &log_file)                               const; //020810 // temporarily abstract so I make sure to implement
 public: // constructor
   Irrigation_operation
      (CropSyst_Op irrig_or_auto_irrig_code = CS_OP_MGMT_IRRIGATION);
   virtual ~Irrigation_operation()                                            {} //200325
};
//_class Irrigation_operation_______________________________________2011-01-XX_/
class Automatic_irrigation_mode
: public  Automatic_irrigation_mode_class
   // Now derived from Irrigation operation so we can get the salinity and chemical values  020313
{  // When simulating salinity, the automatic irrigation with have a default concentration.
   IRRIGATION_VALID_ENTRIES                                                      //120503
public: // constructor
   Automatic_irrigation_mode();                                                  //990224
public: // Common operation virtual methods
   virtual void log(std::ostream &log_file)                               const; //020810
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
};
//_class Automatic_irrigation_mode_____________________________________________/
class N_application_soil_observation_mode
: public N_application_soil_observation_mode_class
{public:
   Valid_float32 v_target_yield_N_uptake;// kg/ha  Crop N uptake for target yield
   Valid_float32 v_estimated_mineralization;
   #if ((CROPSYST_VERSION > 0) && (CROPSYST_VERSION <=4))
   Valid_float32 v_critical_soil_N_for_no_response;// kg/ha
      // this is only in V4
   #endif
   Valid_float32 v_soil_N_sampling_depth;      // m
   Valid_float32 v_fertilizer_use_efficiency;  // % 0-100
 public:
   N_application_soil_observation_mode();
   virtual void setup_parameters
      (CORN::Data_record &data_rec,bool for_write)                modification_; //970709
   virtual void log(std::ostream &log_file)                               const; //020810
};
//_class N_application_soil_observation_mode________________________2011-01-XX_/
class Chemical_operation
: public Chemical_operation_class
{public: // public for parameter editor
   Valid_float32 v_chemical_concentration;
   Chemical_operation() ;
};
//_class Chemical_operation_________________________________________2011-01-XX_/
}//_namespace CropSyst_________________________________________________________/
#endif
//management_param_V4.h
#  define org_N_appl_method_cowl_or_labeled        org_N_appl_method_labeled
#  define org_N_source_cowl_or_labeled             organic_matter_source_labeled
#  define automatic_N_mode_cowl_or_labeled         automatic_N_mode_cowl
#  define NH4_source_cowl_or_labeled               NH4_source_labeled
#  define NH4_appl_method_cowl_or_labeled          NH4_appl_method_labeled
#  define harvest_amount_mode_cowl_or_labeled      harvest_amount_mode_labeled
#  define application_mode_cowl_or_labeled         application_mode_labeled
#  define consideration_mode_cowl_or_labeled       consideration_mode_labeled
#  define application_mode_cowl_or_labeled         application_mode_labeled
#  define   depletion_observation_depth_mode_cowl_or_labeled  depletion_observation_depth_mode_labeled
#  define   harvest_fate_mode_cowl_or_labeled      harvest_fate_mode_labeled

#endif

