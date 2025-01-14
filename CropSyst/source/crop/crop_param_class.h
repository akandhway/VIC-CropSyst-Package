#ifndef crop_param_classH
#define crop_param_classH

#include "corn/enum.h"
#include "CropSyst/source/crop/crop_param_struct.h"
#include "crop/crop_types.hpp"
#include "corn/const.h"
#include "corn/parameters/parameters_datarec.h"
#include "soil/SCS/SCS.h"
#ifndef VIC_CROPSYST
#include "simulation/event.h"
#endif
#  include "residue/residue_decomposition_param.h"
#  include "corn/container/unilist.h"
//______________________________________________________________________________
#define LABEL_crop               "crop"
#define LABEL_enabled "enabled"
//______________________________________________________________________________
class Crop_thermal_time;
class Crop_base;
class Crop_with_management;
namespace CORN
{  class Data_record;
}

DECLARE_ENUM_COWL(Thermal_response_cowl,Thermal_response,Thermal_response_label_table) //151109
//______________________________________________________________________________
namespace CropSyst {
#define VIRTUAL_BOOL_SETUP_STRUCTURE \
   virtual bool setup_structure(CORN::Data_record &record,bool for_write)    modification_
//________________________________________________________forward declarations_/
class Crop_parameters_class
: public Crop_parameters_struct
, public Common_parameters_data_record                                           //010109
{
//______________________________________________________________________________
public:
class Thermal_time
: public Crop_parameters_struct::Thermal_time                                    //100325
{public:
   Thermal_response_cowl thermal_response_cowl;                                  //151109
 public: // constructor
   Thermal_time();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Thermal_time_______________________________________________2002-06-28_/
//#if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
class Phenology
: public Crop_parameters_struct::Phenology
{public:  // until I provide accessors                                           //020629
 public:  // constructor
   Phenology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
 public:  // Used by CropCalibrator
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   void    offset_by(int16 offset_deg_day);                                      //051205
   #endif
   // PHENOLOGY_2018 doesn't need this because thermal time periods
   // are now intrinsically relative.  181107

};
//_class Phenology__________________________________________________2008-07-22_/
//#endif
class Transpiration
: public Crop_parameters_struct::Transpiration
{public:
   Transpiration();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Transpiration______________________________________________2011-02-08_/
DECLARE_ENUM_COWL
   (TUE_Equation_cowl_obsolete,TUE_Equation_obsolete,TUE_equation_label_table)
DECLARE_ENUM_COWL
   (RUE_Basis_cowl_obsolete ,RUE_Basis_obsolete,RUE_basis_label_table)
DECLARE_ENUM_COWL
   (Canopy_growth_cowl,Canopy_growth_model,canopy_growth_label_table)
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Biomass_production
: public  Crop_parameters_struct::Biomass_production
{public:
   #if (CROPSYST_VERSION< 6)
   TUE_Equation_cowl_obsolete TUE_equation_cowl_obsolete;                        //080505
   #endif
   #if (CROPSYST_VERSION==4)
   RUE_Basis_cowl_obsolete    RUE_basis_cowl_obsolete;                           //080731
   #endif
 public:
   Biomass_production();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Biomass_production_________________________________________2011-02-08_/
class Vernalization
: public Crop_parameters_struct::Vernalization
{public: //friend class Crop_thermal_time;
   Vernalization();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Vernalization______________________________________________2002-04-05_/
DECLARE_ENUM_COWL
   (Photoperiodism_cowl
   ,Photoperiodism
   ,photoperiodism_label_table)
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Photoperiod
: public Crop_parameters_struct::Photoperiod                                     //100512
{public:  // friend class Crop_thermal_time;
   Photoperiodism_cowl photoperiodism_cowl;                                      //151122
 public:
   Photoperiod();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Photoperiod________________________________________________2002-04-05_/
DECLARE_ENUM_COWL
   (Emergence_model_cowl
   ,Emergence_model
   ,emergence_model_label_table)
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Emergence_hydro_thermal
: public Crop_parameters_struct::Emergence_hydro_thermal
{public:
   Emergence_model_cowl emergence_model_cowl;
 public:
   Emergence_hydro_thermal();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Emergence_hydro_thermal____________________________________2004-12-01_/
class Quiescence // was Inactive_period                                          //160326
: public Crop_parameters_struct::Quiescence
{public:
   Quiescence();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Quiescence_________________________________________________2008-04-01_/
class Dormancy          // eventually rename this to inactive_period
: public Crop_parameters_struct::Dormancy
{public:
   Dormancy();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Dormancy______________________________________________________________/
class Root
: public Crop_parameters_struct::Root
{public:
   Root();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Root__________________________________________________________________/
class Morphology
: public Crop_parameters_struct::Morphology
{public:
   Canopy_growth_cowl   canopy_growth_cowl;                                      //080725
 public:
   Morphology();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Morphology_________________________________________________2011-02-19_/
class Canopy_growth_cover_based
: public Crop_parameters_struct::Canopy_growth_cover_based
{public:
   Canopy_growth_cover_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Canopy_growth_cover_based_____________________________________________/
class Canopy_growth_LAI_based
: public Crop_parameters_struct::Canopy_growth_LAI_based
{public: // constructor
   Canopy_growth_LAI_based();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Canopy_growth_LAI_based____________________________________2008-07-25_/
class Yield     //200423 was Harvest
: public Crop_parameters_struct::Yield
{public:
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

   //150319 Code::Blocks appears to be having problems with Multiple Inheritence
   #if (VIC_CROPSYST_VERSION != 2)
   class Grain_stress_sensitivity_period
   : public implements_ Simple_event
   , public extends_    Crop_parameters_struct::Temperature_sensitivity          //150319RLN
   {  // The period remains in effect until replaced
      //170423 mutable char key[100];
      mutable std::string key;                                                   //170423
    public:
      inline Grain_stress_sensitivity_period()
         : Simple_event()
         , Temperature_sensitivity()
         { set_default(); }
      inline void copy_from(const Grain_stress_sensitivity_period &from_copy)
         {  Simple_event::copy(from_copy);
            Temperature_sensitivity::copy_from(from_copy);
         }
      inline virtual const char *label_string(std::string &buffer)        const  //170423
         {  begin_sync.append_to_string(buffer _ISO_FORMAT_DATE_TIME);
            return buffer.c_str();
         }
   inline virtual bool is_key_string(const std::string &key_)      affirmation_  //180820
      { return key == key_; }

/*180820  was probably only used for find_cstr now using is_key

      inline virtual const char *get_key()                                 const
         {  key.clear();                                                         //170424
            label_string(key);                                                   //170424
            return key.c_str();                                                  //170424
         }
*/
      bool setup_structure(CORN::Data_record &data_rec,bool for_write);
   };
   //_Grain_stress_sensitivity_period_______________________________2013-04-26_/
   Grain_stress_sensitivity_period editor_grain_stress_sensitivity_period;       //130426
   Common_event_list grain_stress_sensitivity_periods;                           //130426
   #endif
   Yield();
   inline virtual bool setup_structure
      (CORN::Data_record &,bool,Harvested_part)                    modification_
      { return true;}                                                            //150904
};
//_Yield_______________________________________________________________________/
class Salinity
: public Crop_parameters_struct::Salinity
{public:
   Salinity();
};
//_class Salinity______________________________________________________________/
class CO2_Response
: public Crop_parameters_struct::CO2_Response
{public:
   CO2_Response();
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class CO2_Response__________________________________________________________/
class Tolerance
: public Crop_parameters_struct::Tolerance
{public:
   Tolerance();
   virtual bool setup_structure
      (CORN::Data_record &record,bool for_write
      , Harvested_part harvested_part)                            modification_;
};
//_class Tolerance_____________________________________________________________/
DECLARE_ENUM_COWL(Harvest_condition_mode_cowl,
   Harvest_condition_mode, harvest_condition_mode_label_table)
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Fruit
: public Crop_parameters_struct::Fruit
{public:
   Harvest_condition_mode_cowl harvest_condition_mode_cowl;
public:
   Fruit();
   inline virtual ~Fruit() {}                                                    //170217
   VIRTUAL_BOOL_SETUP_STRUCTURE;
};
//_class Fruit______________________________________________________2011-01-21_/
class Layout
: public Crop_parameters_struct::Layout
{ public:
   Layout();
};
//_class Layout_____________________________________________________________________________
class Canopy_architecture // Canopy_characteristics_hedge_row //200402
: public Crop_parameters_struct::Canopy_architecture // Canopy_characteristics_hedge_row
{public:
   // Canopy_characteristics_hedge_row();
   Canopy_architecture();
};
//______________________________________________________________________________
DECLARE_ENUM_COWL
   (Nitrogen_leaf_stress_mode_cowl
   ,Nitrogen_leaf_stress_mode
   ,nitrogen_leaf_stress_model_label_table)
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class Nitrogen
: public Crop_parameters_struct::Nitrogen
{
/*200529
 public:
   Nitrogen_leaf_stress_mode_cowl nitrogen_leaf_stress_mode_cowl;
*/
 public:
   Nitrogen();
   void set_fallow_conditions();                                                 //020408
   VIRTUAL_BOOL_SETUP_STRUCTURE;
   float32 provide_concentration_slope
      (Photosynthetic_pathway photosynthetic_pathway)                provision_;//120507
};
//_class Nitrogen___________________________________________________2011-01-21_/
//#ifndef YAML_PARAM_VERSION
//151016  Once I Implement parameter adjustments idiom move this back to V4 only
class Adjustment
{
 public:
   std::string parameter;
   std::string value;
      // value is store as a string because we could potentially
      // adjust/reset any parameter including enums and text fields
 public:
   inline Adjustment()
      : parameter("")
      , value("")
      {}
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write) modification_;
};
//_class Adjustment____________________________________________________________/
class Adjustment_event
: public extends_ Simple_event
, public extends_ Adjustment
{public:
   inline Adjustment_event()
      : Simple_event()
      , Adjustment()
      {}
   inline Adjustment_event(const Synchronization &i_date_phen_sync)
      : Simple_event(i_date_phen_sync)
      , Adjustment()
      {}
   inline bool setup_structure
      (Data_record &data_rec ,bool for_write)                       modification_
      {  return Simple_event::setup_structure(data_rec,for_write)
         &&     Adjustment::setup_structure(data_rec,for_write);
      }
   virtual const char *label_string(std::string &buffer)                  const; //170423
};
//#endif
//_class Adjustment_event______________________________________________________/

 public: // We now save these to the file replacing the "stages" variable
   bool  parameter_editor_mode;   // This may be set to true to keep all submodel parameters in memory for the parameter editor //030520
 public:  // Values assigned at runtime (Not written to data source!)            //051129
   std::string sowing_event_ID;  // This is the ID of the sowing event that planted this crop. (Currently only used for LADSS) //051129
 public:  // Options
   int32       LADSS_land_use_ID;                                                //030331
   std::string feed_name_immature_early;  // Used by CANMS                       //021114
   std::string feed_name_midmature_headed_normal;  // Used by CANMS              //021114
   std::string feed_name_mature_normal;  // Used by CANMS                        //021114

   // Eventually use cowls instead of labeled enums
   Crop_model_clad      crop_model_clad;                                         //020408
   Harvested_part_clad  harvested_part_clad;                                     //020405
   Land_use_clad        land_use_clad;                                           //020324
   Photosynthetic_pathway_clad       photosynthetic_pathway_clad;                //110504_090715
   Life_cycle_clad      life_cycle_clad;                                         //020716
   bool                    senesces;  // I think this is obsolete (always true)  //040808
   Abscission_clad      abscission_clad;    // will replace deciduous            //081113
   Continuation_clad    continuation_mode_clad;                                  //200212_080331
   //200212 Season_dormancy_mode_clad season_dormancy_mode_clad;                          //080331
      //160701 rename to Continuation_clad
   int16 linger_days;                                                            //080910
   // name is currently used only for CAFE_dairy
   // it will match the crop file name (unqualified)
   std::string             RUSLE2_name;                                          //110817
   std::string             RUSLE2_description;                                   //110812
   std::wstring            name;                                                 //150720
      // name will match the name portion of the filename.
      // is it used searching/indexing
   // Eventually adjustments will be obsolete once I implement parameter adjustments idiom
  // #ifndef YAML_PARAM_VERSION
   Common_event_list       adjustments;                                          //120716
  // #endif
   contribute_ bool has_max_water_uptake_adjustments;                            //120725
 public: // Not saved to file
   Common_event_list associated_events;                                          //151004
      // list used to tranfer management operations loaded at sowing
      // to crop event scheduler.
 public: // 'structor
   Crop_parameters_class();
   virtual ~Crop_parameters_class();                                             //180116
 public:
   bool is_perennial()                                             affirmation_; //020716
   bool is_annual()                                                affirmation_; //020716
   inline virtual bool is_deciduous()                              affirmation_
                                  { return abscission_clad.get() == deciduous; } //150904
   bool is_fruit_tree()                                            affirmation_;
      // rename to is_fruit because not necessarily tree
   bool has_leaf_senescence()                                      affirmation_; //030521
   bool applies_harvest_index()                                    affirmation_; //040614
      // Returns true if the crop type selections indicate harvest index used to determine yield.
 public: // Data record overrides
   virtual bool expect_structure(bool for_write = false);                        //161023
   virtual bool get_end();                                                       //161023_120725
   inline virtual bool preserve_unexpected_entries() affirmation_{ return true;} //020108
      /* When the crop editor is launched from the Crop calibrator
         we need to preserve those entries added by the crop calibrator*/
   inline virtual const char *get_type_label()  const { return "CropSyst crop";} //051011
   inline virtual const char *get_primary_section_name()const{return LABEL_crop;}//020911
 public: // reference modifiers
   virtual  Crop_parameters_struct::Quiescence                 &mod_quiescence()                =0;
   virtual  Crop_parameters_struct::Thermal_time               &mod_thermal_time()              =0;
   virtual  Crop_parameters_struct::Biomass_production         &mod_biomass_production()        =0;
   virtual  Crop_parameters_struct::Vernalization              &mod_vernalization()             =0;
   virtual  Crop_parameters_struct::Photoperiod                &mod_photoperiod()               =0;
   virtual  Crop_parameters_struct::Transpiration              &mod_transpiration()             =0;
   virtual  Crop_parameters_struct::Morphology                 &mod_morphology()                =0;
   virtual  Crop_parameters_struct::Canopy_growth_LAI_based    &mod_canopy_growth_LAI_based()   =0;
   virtual  Crop_parameters_struct::Canopy_growth_cover_based  &mod_canopy_growth_cover_based() =0;
   virtual  Crop_parameters_struct::Root                       &mod_root()                      =0;
   virtual  Crop_parameters_struct::Fruit                      &mod_fruit()                     =0;
   virtual  Crop_parameters_struct::Dormancy                   &mod_dormancy()                  =0;//110504
   virtual  Crop_parameters_struct::Phenology                  &mod_phenology()                 =0;
 public: // reference accessors
   //ref_quiescence
   virtual const Crop_parameters_struct::Thermal_time          &ref_thermal_time()              const=0;
   virtual const Crop_parameters_struct::Biomass_production    &ref_biomass_production()        const=0;
   virtual const Crop_parameters_struct::Vernalization         &ref_vernalization()             const=0;
   virtual const Crop_parameters_struct::Photoperiod           &ref_photoperiod()               const=0;
   virtual const Crop_parameters_struct::Transpiration         &ref_transpiration()             const=0;
   virtual const Crop_parameters_struct::Morphology            &ref_morphology()                const=0;
   virtual const Crop_parameters_struct::Canopy_growth_LAI_based &ref_canopy_growth_LAI_based() const=0;
   virtual const Crop_parameters_struct::Canopy_growth_cover_based  &ref_canopy_growth_cover_based()  const=0 ;
   virtual const Crop_parameters_struct::Root                  &ref_root()                      const=0;
   virtual const Crop_parameters_struct::Fruit                 &ref_fruit()                     const=0;
   virtual const Crop_parameters_struct::Dormancy              &ref_dormancy()                  const=0;//190606
   virtual const Crop_parameters_struct::Phenology             &ref_phenology()                 const=0;
   #ifdef CROPSYST_PROPER
   //NYN virtual const Crop_parameters_struct::Emergence_hydro_thermal    &ref_emergence() const=0;
   #endif
   //NYN virtual const Crop_parameters_struct::Inactive_period            &ref_inactive_period() const=0;
   //NYN virtual const       Crop_parameters_struct::Transpiration &transpiration = ref_transpiration() = 0;
 public:
   virtual Crop_parameters_class::Emergence_hydro_thermal      &mod_emergence_class()     =0;
   virtual Crop_parameters_class::Tolerance                    &mod_tolerance_class()     =0; //150913
   virtual Crop_parameters_class::Nitrogen                     &mod_nitrogen_class()      =0;
   virtual Crop_parameters_class::Transpiration                &mod_transpiration_class() = 0;
   virtual Crop_parameters_class::Yield                        &mod_yield_class()       =0;
   virtual Crop_parameters_class::Morphology                   &mod_morphology_class()    =0;
   virtual Crop_parameters_class::Phenology                    &mod_phenology_class()     =0; //190701
   virtual Crop_parameters_class::CO2_Response                 &mod_CO2_response_class()  = 0;
   virtual Crop_parameters_class::Fruit                        &mod_fruit_class()         =0;
   virtual Crop_parameters_class::Biomass_production           &mod_biomass_production_class() = 0;
public:
   #ifndef YAML_PARAM_VERSION
   virtual float64 param_TUE_at_1kPa_VPD          (bool before_WUE_change) const; //080428
   virtual float64 param_TUE_scaling_coef         (bool before_WUE_change) const; //091201
   #endif
           float64 param_base_temp()                                      const;
   virtual float64 param_max_water_uptake_m                                      //170524
      (Normal_crop_event_sequence growth_stage)                           const; //091208
 #if ((PHENOLOGY_VERSION==2018))
 private:
   Phenologic_period *render_phenologic_period                                   //181109
      (const VV_Section &phenologic_period_section_pot)              rendition_;
 #endif
};
//_class Crop_parameters_class______________________________________2011-01-21_/
#if (VIC_CROPSYST_VERSION!=2)
DECLARE_CONTAINER_ENUMERATED_SECTION
   (Crop_parameters_class::Yield::Grain_stress_sensitivity_period
   ,grain_stress_sensitivity_period_section_VV,true);
#endif
//______________________________________________________________________________
}//_namespace CropSyst_________________________________________________________/

#define  LABEL_LADSS_land_use_ID  "land_use_ID"
#define  LABEL_class_obs          "class"
#define  LABEL_harvested_part     "harvested_part"
#define  LABEL_C_species          "C_species"
#define  LABEL_perennial_obs      "perennial"
#define  LABEL_life_cycle         "life_cycle"
#define  LABEL_deciduous          "deciduous"
#define  LABEL_senesces           "senesces"
#define  LABEL_shed_dead_residue  "shed_dead_residue"
#define  LABEL_grain_filling_obs  "grain_filling"
#define  LABEL_filling            "filling"
#define  LABEL_has_flowering      "has_flowering"
#define  LABEL_flowering          "flowering"
#define  LABEL_flowers            "flowers"
#define  LABEL_land_use           "land_use"

#define LABEL_photoperiod        "photoperiod"
#define LABEL_vernalization      "vernalization"

#define LABEL_legume             "legume"
#define LABEL_N_fixation_obs     "N_fixation"
#define LABEL_fixation           "fixation"
#define LABEL_stages             "stages"

#define LABEL_growth             "growth"

#define  LABEL_biomass_water_ratio_V3     "biomass_water_ratio"
#define  LABEL_biomass_transpiration_coef "biomass_transpiration_coef"
#define  LABEL_at_pt_limit             "at_pt_limit"
#define LABEL_transpiration      "transpiration"
#define  LABEL_kc                            "kc"
#define  LABEL_ET_crop_coef                  "ET_crop_coef"
#define  LABEL_max_water_uptake              "max_water_uptake"
#define  LABEL_critical_leaf_water_pot_obs   "critical_leaf_water_pot"
#define  LABEL_stomatal_closure_leaf_water_pot  "stomatal_closure_leaf_water_pot"
#define  LABEL_stomatal_closure_LWP_LADSS_alias "stomatal_closure_LWP"
#define  LABEL_wilt_leaf_water_pot           "wilt_leaf_water_pot"
#define  LABEL_wilt_LWP                      "wilt_LWP"

#define LABEL_leaf               "leaf"
#define  LABEL_max_LAI                           "max_LAI"
#define  LABEL_fract_LAI_mature                  "fract_LAI_mature"
#define  LABEL_initial_GAI                       "initial_GAI"
#define  LABEL_specific_leaf_area                "specific_leaf_area"
#define  LABEL_stem_leaf_partition               "stem_leaf_partition"
#define  LABEL_leaf_duration_sensitivity_stress_obs  "leaf_duration_sensitivity_stress"
#define  LABEL_duration_stress_sensitivity  "duration_stress_sensitivity"

#define LABEL_phenology          "phenology"
/*191108
#define  XLABEL_deg_day_emerge_obs     "deg_day_emerge"
#define  LABEL_deg_day_leaf_duration_obs             "deg_day_leaf_duration"
#define  LABEL_deg_day_begin_flower_obs              "deg_day_begin_flower"
#define  LABEL_deg_day_tuber_initiation_obs          "deg_day_tuber_initiation"
#define  LABEL_deg_day_begin_grain_filling_obs       "deg_day_begin_grain_filling"
#define  LABEL_deg_day_begin_filling_obs            "deg_day_begin_filling"
#define  LABEL_deg_day_begin_maturity_obs            "deg_day_begin_maturity"
#define  LABEL_deg_day_begin_veraison_obs            "deg_day_begin_veraison"
*/

#define  LABEL_emergence            "emergence"
#define  LABEL_duration        "duration"
#define  LABEL_flowering            "flowering"
#define  LABEL_tuber_init           "tuber_init"
#define  LABEL_grain_filling_obs    "grain_filling"
#define  LABEL_filling              "filling"
#define  LABEL_maturity             "maturity"
#define  LABEL_rapid_fruit_growth   "rapid_fruit_growth"

#define LABEL_morphology         "morphology"
#define  LABEL_max_root_depth                      "max_root_depth"
#define  LABEL_surface_root_density					   "surface_root_density"
#define  LABEL_root_density_distribution_curvature	"root_density_distribution_curvature"
#define  LABEL_root_density_distr_curv_LADSS_alias	"root_density_distr_curv"

#define  LABEL_max_canopy_height                 "max_canopy_height"

#define LABEL_dormancy           "dormancy"
#define  LABEL_induce_dormancy_temp_obs            "induce_dormancy_temp"
#define  LABEL_start_dormancy_valid_date_obs       "start_dormancy_valid_date"
#define  LABEL_leave_dormancy_valid_date_obs       "leave_dormancy_valid_date"

#define  LABEL_inducement_temperature              "inducement_temperature"
#define  LABEL_start_valid_date                    "start_valid_date"
#define  LABEL_leave_valid_date                    "leave_valid_date"
#define  LABEL_deg_day_bud_break_chill_req_sat     "deg_day_bud_break_chill_req_sat"
#define  LABEL_deg_day_bud_break_chill_req_not_sat "deg_day_bud_break_chill_req_not_sat"

#define  LABEL_chill_requirement                   "chill_requirement"

#define LABEL_harvest_index      "harvest_index"
#define  LABEL_harvest_ndx_unstressed_obs                "harvest_ndx_unstressed"
#define  LABEL_harvest_ndx_tuber_growth_sensitivity_obs  "harvest_ndx_tuber_growth_sensitivity"

#define  LABEL_unstressed                                "unstressed"
#define  LABEL_growth_sensitivity                        "growth_sensitivity"
#define  LABEL_tuber_init_sensitivity                    "tuber_init_sensitivity"
#define  LABEL_tuber_growth_sensitivity                  "tuber_growth_sensitivity"
#define  LABEL_translocation_factor                      "translocation_factor"

#define LABEL_vernalization                  "vernalization"
#define  LABEL_vernalization_low_temp_obs         "vernalization_low_temp"
#define  LABEL_vernalization_high_temp_obs        "vernalization_high_temp"
#define  LABEL_vernalization_start_obs            "vernalization_start"
#define  LABEL_vernalization_end_obs              "vernalization_end"
#define  LABEL_vernalization_min_factor_obs       "vernalization_min_factor"

#define  LABEL_v_end     "v_end"
#define  LABEL_v_start   "v_start"

#define  LABEL_low_temp         "low_temp"
#define  LABEL_high_temp        "high_temp"
#define  LABEL_start            "start"
#define  LABEL_end              "end"
#define  LABEL_min_factor       "min_factor"

#define LABEL_photoperiod                     "photoperiod"
#define  LABEL_photoperiod_start_obs          "photoperiod_start"
#define  LABEL_photoperiod_end_obs            "photoperiod_end"

#define  LABEL_pp_start              "pp_start"
#define  LABEL_pp_end                "pp_end"

#define  LABEL_start              "start"
#define  LABEL_end                "end"

#define LABEL_nitrogen           "nitrogen"
#define  LABEL_adjust_N_uptake_obs                "adjust_N_uptake"
#define  LABEL_adjust_N_availability_obs          "adjust_N_availability"
#define  LABEL_max_N_stubble_obs                  "max_N_stubble"

#define  LABEL_uptake_adj                    "uptake_adj"
#define  LABEL_availability_adj              "availability_adj"
#define  LABEL_residual                      "residual"
#define  LABEL_max_conc_emerge      "max_conc_emerge"
#define  LABEL_max_conc_mature      "max_conc_mature"
#define  LABEL_min_conc_mature      "min_conc_mature"

#define  LABEL_adjust_uptake_obs                "adjust_uptake"
#define  LABEL_adjust_availability_obs          "adjust_availability"
#define  LABEL_max_concentration_emerge_obs     "max_conconcentration_emerge"
#define  LABEL_max_concentration_mature_obs     "max_conconcentration_mature"
#define  LABEL_min_concentration_mature_obs     "min_conconcentration_mature"

#define  LABEL_max_stubble                  "max_stubble"

#define LABEL_fruit                           "fruit"
#define  LABEL_max_fruit_load                  "max_fruit_load"
#define  LABEL_fract_total_solids                   "fract_total_solids"
#define  LABEL_development_fraction_to_fruit_obs   "development_fraction_to_fruit"
#define  LABEL_veraison_fraction_to_fruit_obs      "veraison_fraction_to_fruit"

#define  LABEL_initial_growth_fraction_to_fruit    "initial_growth_fraction_to_fruit"
#define  LABEL_rapid_growth_fraction_to_fruit      "rapid_growth_fraction_to_fruit"

#define  LABEL_salinity          "salinity"
#define  LABEL_osmotic_pot_50                 "osmotic_pot_50"
#define  LABEL_salt_tolerance_P_obs           "salt_tolerance_P"
#define  LABEL_tolerance_P                    "tolerance_P"

#define LABEL_CO2                             "CO2"
#define  LABEL_growth_ratio_elevated_to_baseline_conc   "growth_ratio_elevated_to_baseline_ppm"
#define  LABEL_baseline_reference_conc                 "baseline_reference_conc"
#define  LABEL_elevated_reference_conc                 "elevated_reference_conc"

#define LABEL_base_temp   "base_temp"
#define LABEL_cutoff_temp "cutoff_temp"
#define LABEL_optimal_temp "optimal_temp"
#define LABEL_maximum_temp "maximum_temp"

#define LABEL_hardiness                               "hardiness"
#define LABEL_sensitive_to_cold_obs                   "sensitive_to_cold"
#define LABEL_damage_cold_temp                        "damage_cold_temp"
#define LABEL_lethal_cold_temp                        "lethal_cold_temp"
#define LABEL_salvageable_days_before_maturity        "salvageable_days_before_maturity"
#define LABEL_salvagable_days_LADSS_alias             "salvagable_days"

#define  LABEL_LAI_sensitivity_stress              "LAI_sensitivity_stress"
#define  LABEL_opt_temp_for_growth_obs                 "opt_temp_for_growth"
#define  LABEL_early_growth_limit_temp             "early_growth_limit_temp"
#define  LABEL_phenologic_sensitivity_water_stress_obs "phenologic_sensitivity_water_stress"
#define  LABEL_water_stress_sensitivity            "water_stress_sensitivity"

// rename this to phenologic_water_stress_sensitivity

#define LABEL_canopy_characteristics        "canopy_characteristics"
#define  LABEL_initial_canopy_diameter       "initial_canopy_diameter"
#define  LABEL_final_canopy_diameter         "final_canopy_diameter"
#define  LABEL_max_LAI_of_canopy_projection  "max_LAI_of_canopy_projection"
#define  LABEL_initial_row_width             "initial_row_width"
#define  LABEL_final_row_width               "final_row_width"
#define  LABEL_initial_row_height            "initial_row_height"
#define  LABEL_final_row_height              "final_row_height"

#define LABEL_layout       "layout"
#define  LABEL_row_spacing     "row_spacing"
#define  LABEL_row_azimuth     "row_azimuth"
#define  LABEL_col_spacing     "col_spacing"

#if ((CS_VERSION > 0) && (CS_VERSION < 5))
#define  LABEL_light_to_biomass        "light_to_biomass"
#endif

#endif

//crop_param.h

