#ifdef VIC_CROPSYST_VERSION
#ifndef VIC_land_unit_simulationH
#define VIC_land_unit_simulationH
// CropSyst/source should be added to the include paths                          //141206
#include "CropSyst/source/land_unit_sim.h"
//200817 #include "CropSyst/source/arguments_CropSyst.h"
#include "CS_suite/application/CS_arguments.h"
#ifdef VCS_V5
#include "vic_def.h"
#else
#include "vicNl_def.h"
#endif
#include "VIC_soil.h"                                                           //LML 150327
//200325RLN #include "common/simulation/log.h"
#include "common/solar/irradiance_extraterrestrial.h"
#include "common/weather/weather_interface.h"
#include "common/weather/day/sun_days.h"
#include "common/weather/hour/sun_hours.h"
#include "management/management_param_V4.h"
#include "common/weather/atmospheric_CO2_change.h"
#include "common/biometeorology/parameter/CO2_atmospheric_conc.h"
namespace VIC
{
//______________________________________________________________________________
//LML 150318
enum CropSyst_Variables{
  CANOPY_COVER_FRACTION_GREEN,
  CANOPY_COVER_FRACTION_TOTAL,
  ET_COEF_FULL_SHADING,
  MAD_IRRIGATION,
  RAD_ATTEN,
  LAI,
  GAI,
  CANOPY_HEIGHT,
  CANOPY_ALBEDO,                                                                 //vegetation albedo (added for full energy) (fraction)
  DSPLACEMENT,                                                                   //vegetation displacement height (m)
  ROUGHNESS,                                                                     //vegetation roughness length (m)
  WDEWMAX,                                                                       //maximum monthly dew holding capacity (mm)
  MIN_STOMATA_CONDUCTANCE,                                                       //minimum stomatal resistance (s/m)
  EMISSIVITY,                                                                    //vegetation emissivity (fraction)
  RA_ARCHITECTURAL,                                                              //architectural resistance (s/m)
  TRUNK_RATIO,                                                                   //ratio of trunk height to tree height (0.2 (fraction))
  WIND_ATTEN,                                                                    //wind attenuation through canopy (0.5 (N/A))
  WIND_H,                                                                        //height at which wind is measured (m)
  RGL,                                                                           //Value of solar radiation below which there
                                                                                 //will be no transpiration (ranges from
                                                                                 //~30 W/m^2 for trees to ~100 W/m^2 for crops)
  VC_root_depth_mm,
  FRACT_RESIDUE_COVER,                                                           //160509LML
  SURFACE_RESIDUE_C_MASS,                                                        //surface: straw_residue_om, manure_residue,flat_surface,stubble_surface
  SUBSURFACE_RESIDUE_C_MASS,                                                     //subsurface: straw_residue_om,manure_residue_om,subsurface residue
  PROFILE_SOM_C_MASS,                                                            //SOM and MB
  SURFACE_RESIDUE_N_MASS,
  SUBSURFACE_RESIDUE_N_MASS,
  PROFILE_SOM_N_MASS,
  PROFILE_NO3_N_MASS,
  PROFILE_NH4_N_MASS,
  CO2_C_LOSS_RESIDUE,                                                            //residue and manure
  CO2_C_LOSS_SOM,                                                                //SOM and MB
  NITRIFICATION_N2O_N,
  DENITRIFICATION_N2O_N,
  Water_Stress_Index,
  CropSyst_Variables_counts
};
//______________________________________________________________________________
class Atmospheric_CO2_change
: public Atmospheric_CO2_change_element
{
 public:
   CS::CO2_atmospheric_concentration  curr_CO2_conc;                             //150507
 public:
   Atmospheric_CO2_change
   (const CORN::date32 &simdate_raw_);                                          //171207
   //virtual bool end_year()                                        modification_;
   inline virtual float64 get_current_CO2_conc()                           const
      { return curr_CO2_conc.ppm(); }
   inline virtual const CS::CO2_atmospheric_concentration
      &ref_CO2_current_conc()                                              const //150507
      { return curr_CO2_conc; }
};
//_Atmospheric_CO2_change_linear_______________________________________________/
//______________________________________________________________________________
class Land_unit_simulation
:public extends_ CropSyst::Land_unit_simulation
{
 private:
   nat32                rotation_or_crop_veg_class_code;                         //150521
   nat8                 rotation_cycle_index;                                    //150521RLN

   /*LML 141022 these pointers are references, they do not change during simulation period.*/

   //141207RLN All these references should probably should be const
   crop_data_struct    &VIC_crop;
   soil_con_struct     &VIC_soil_con;
   option_struct       &VIC_options;
   veg_con_struct      &VIC_veg_con_item;
   atmos_data_struct   *VIC_atmos;                                               //150608LML
   cell_data_struct    &VIC_cell;
   snow_data_struct    &VIC_snow;                                                //150608LML
   veg_var_struct      &VIC_veg_var;                                             //150609LML
   #ifdef USE_CROPSYST_CROP_IRRIGATION
   CropSyst::Irrigation_operation *irrigation_operation_for_VIC_next_day;        //150720LML
   #endif
   //200817 CropSyst::Arguments *arguments;
   CS::Arguments       *arguments;                                               //200817RLN
   CropSyst::Scenario   scenario_control_and_model_options;                      //150415RLN
   Geolocation          geolocation;                                             //150604RLN
   CS::Land_unit_meteorological meterology;                                      //160217RLN
   Location_parameters  location_params;                                         //150604RLN
   //190723LML Sun_days       sun_days;                                                      //150707RLN
   //190723LML Sun_hours      sun_hours;                                                     //150707RLN
   //190723LML CS::Solar_irradiance_extraterrestrial_hourly extraterrestrial_solar_irradiance;   //150707RLN
   Weather_provider *weather_provider;                                           //160118RLN
   Atmospheric_CO2_change atmospheric_CO2;                                      //210401
   double air_temperature_min_tomorrow;
   double air_temperature_max_yesterday;
   CropSyst::Irrigation_operation *curr_irrigation_operation;                    //170221_150718LML

 private:
   mutable float64 reference_canopy_resistance_d_m;                              //150603RLN
   Slope_saturated_vapor_pressure_plus_psychrometric_resistance
      *slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference;   //150601RLN
   float64 snow_pack_thickness; //meter value known by VIC                       //141207
 private: // Cell output management
   CORN::OS::Directory_name_concrete CWD_temporary;
 public:
   bool database_valid;
   Soil_components /*190128RLN Soil*/ vic_soil_for_CropSyst;                     //150327LML
 private:
//200325RLN    Common_simulation_log log_unused;                                             //150415RLN
   contribute_ CORN::Unidirectional_list recognized_crop_irrig_type_assocations; //151007RLN
   provided_   nat8 zone;                                                        //151008RLN
 private:
   bool setup_weather_from_VIC();                                                //141023LML
   bool setup_soil_property_from_VIC();                                          //141023LML
   
   // 141208RLN: not necessary to separate these function.
   // also I think these pointers only need to be set once
   // (this can be done in the know_VIC_environment() method
   void CropSyst_know_VIC_layer(layer_data_struct *_VIC_layer);
   void CropSyst_know_VIC_atmos(atmos_data_struct *_VIC_atmos);
   void CropSyst_know_VIC_cell(cell_data_struct *_VIC_cell);

   double get_VIC_liq_water_content_volumetric_layer(int VIC_layer_index);
   double get_VIC_ice_water_content_volumetric_layer(int VIC_layer_index);
   double cal_psychrometric_constant();                                          //(kPa_per_C) 150608LML

 public:
    class Weather_spanning : public implements_ Weather_spanning_interface
    {
       //190130 const CORN::Date_cowl_32 &today;                                          //170525RLN
       const CORN::date32 &simdate;                                              //190130
       atmos_data_struct   *VIC_atmos;
       const double &air_temperature_max_yesterday;
       const double &air_temperature_min_tomorrow;

     public:
      inline Weather_spanning
         (
          //190130 const CORN::Date_cowl_32 &today_                                       //170525RLN
          const CORN::date32 &simdate_                                           //190130
         ,atmos_data_struct *VIC_atmos_
         ,const double &air_temperature_max_yesterday_
         ,const double &air_temperature_min_tomorrow_)
         : Weather_spanning_interface()                                          //190130
         //190130,today(today_)
         ,simdate(simdate_)                                                      //190130
         ,VIC_atmos(VIC_atmos_)
         ,air_temperature_max_yesterday(air_temperature_max_yesterday_)
         ,air_temperature_min_tomorrow(air_temperature_min_tomorrow_)
         {}
      inline virtual ~Weather_spanning() {}
      virtual bool get_parameter_on
            (modifiable_ CS::Parameter &parameter
            ,nat32  variable_code // Typically the variable code will be UED variable code
            ,const CORN::Date_const &on_date)                    rectification_;
      virtual float64 get_air_temperature_min_tomorrow()                  const;
      virtual float64 get_air_temperature_max_yesterday()                 const;
    };
    Weather_spanning weather_spanning;
 public:
   Land_unit_simulation
      (
      CS::Arguments *arguments_given                                             //200817RLN
      //200817 CropSyst::Arguments *arguments_given
      , nat32 rotation_or_crop_veg_class_code                                    //150521RLN
      , nat8  rotation_shift_index                                               //150521RLN
      , const CORN::date32           &sim_date                                   //190130RLN
      //190130 , const CORN::Date_cowl_32 &_today
      , crop_data_struct &_VIC_crop
      , soil_con_struct  &_VIC_soil_con
      , option_struct    &_VIC_options
      , veg_con_struct   &_VIC_veg_con_item
      , atmos_data_struct *_VIC_atmos
      , cell_data_struct &_VIC_cell
      , snow_data_struct &_VIC_snow
      , veg_var_struct   &_VIC_veg_var
      );
   virtual ~Land_unit_simulation();
 public: // Land unit overrides
   inline virtual const float64 &ref_snow_pack_thickness()                 const //141207
      {  return snow_pack_thickness;
      }
   inline virtual bool evaporate_soil()                             modification_//160408
      {  //VIC handles soil evaporation
         return true;
      }
 public: // Methods the VIC interface calls to provide data to CropSyst
   void know_VIC_environment
      (crop_data_struct *  _VIC_crop
      , soil_con_struct *   _VIC_soil_con
      , veg_lib_struct *    _VIC_veg_lib
      , option_struct *     _VIC_options
      , veg_con_struct *    _VIC_veg_con
      , atmos_data_struct*   _VIC_atmos                                          //150422LML
      , cell_data_struct*    _VIC_cell                                           //150422LML
      , int _VIC_Crop_count_index);
   bool start_day_VIC_CropSyst
      (atmos_data_struct *_VIC_atmos
      ,layer_data_struct _VIC_layer[]
      ,cell_data_struct *_VIC_cell);
   virtual bool initialize()                                      modification_; //150427
   virtual bool start()                                           modification_;
   virtual bool start_day()                                       modification_;
   virtual bool end_day()                                         modification_; //150718LML
   double update_PET_from_VIC()                                   modification_;
   //bool is_current_crop_fruit_tree()                                affirmation_;//150720
   inline virtual float64 update_ET_coef_full_shading()            modification_ {assert(false); return 0.0;}     //141206RLN
   virtual bool process_day()                                      modification_;//150611_150609
   inline virtual bool load_rotation_planting_events() { return true;}           //020309
      // For VIC we setup the rotation in initialize()

   virtual nat8 chemical_transport_and_exchange                                  //150726
      (Seconds               at_time
      ,Seconds               duration
      ,soil_layer_array64   (old_liquid_water_content)
      ,soil_layer_array64   (new_liquid_water_content)
      ,float64               water_entering_soil
      ,soil_layer_array64   (water_flow))                          modification_;
   virtual float64 get(nat32 variable_code)                                const;
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   //141208 The following 'know' methods could be eliminated
   // if the values can be setup in start_day() from
   // the VIC structures.
   inline bool know_transpiration_max_mm(double transp_max_mm)     modification_
      { UNUSED_arg(transp_max_mm); assert(false); return false; }  //141206RLN
          virtual void know_pychrometrics                                        //140508
         (float64 vapor_pressure_saturation_slope_kPa_C
         ,float64 psychrometric_constant_kPa_C
         ,float64 aerodynamic_resistance_ref_plants)               modification_;//141207
   inline virtual void know_snow_pack_thickness                                  //141207
      (float64 thickness_meter)                                    modification_
      { snow_pack_thickness = thickness_meter; }
   //_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
   inline virtual Common_operation *get_operation
      (const char *operation_type_str
      ,const std::string &operation_ID                     // filename may be updated with qualification so not const
      ,modifiable_ CORN::OS::File_name   &operation_filename    // the fully qualified operation filename may be returned (to be output to the schedule)
      ,int32 resource_set_ID
      ,bool &relinquish_op_to_event)
      {  UNUSED_arg(operation_type_str); UNUSED_arg(operation_ID);
         UNUSED_arg(operation_filename); UNUSED_arg(resource_set_ID);
         UNUSED_arg(relinquish_op_to_event);
         assert(false); return 0;}                                                 //141206RLN
   virtual Event_status_indicator perform_operation                              //151007RLN
      (Common_operation        &op);
   virtual Event_status_indicator perform_irrigation                             //170221RLN
       (CropSyst::Irrigation_operation &irrig_op);
   virtual nat32 perform_triggered_synchronizations_today() {std::cerr << "TBD perform_triggered_synchronizations_today!\n";}                 //200518LML
   virtual const CORN::OS::Directory_name &get_output_directory_name()     const
      { return CWD_temporary; }
      // Currently the cell scenario outputs are stored in the current working directory.
      // but we should create a cell output directory
      // (Presumably the cell output directory would be a subdirectory
      //  of the run directory (CWD).
      // If VIC has cell (sub unit) ID (as char[]) then the output directory
      // name can be composed using this.
 public: // site specific lookups
   const CropSyst::Irrigation_operation_struct *get_irrigation_parameters()const;//150413RLN
   nat8  provide_zone()                                               provision_;//151008RLN
   int initialize_at_start_of_day();                                             //171201LML
 public: // reference accessors
   virtual const Air_temperature_average &ref_air_temperature_avg()        const;//150527RLN
   virtual const Wind_speed              &ref_wind_speed()                 const;//150527RLN
   veg_con_struct          &ref_VIC_veg_con()      const {return VIC_veg_con_item;};  //150609LML
   crop_data_struct        &ref_VIC_crop()         const {return VIC_crop;};          //150609LML
   soil_con_struct         &ref_VIC_soil_con()     const {return VIC_soil_con;};      //150609LML
   cell_data_struct        &ref_VIC_cell()         const {return VIC_cell;};          //150609LML
   snow_data_struct        &ref_VIC_snow()         const {return VIC_snow;};          //150609LML
   veg_var_struct          &ref_VIC_veg_var()      const {return VIC_veg_var;};       //150609LML
   atmos_data_struct       *ref_VIC_atmos()        const {return VIC_atmos;};         //150703LML
   inline Weather_provider *get_weather_provider() const {return weather_provider;};  //170725LML
   CropSyst::Irrigation_operation *get_irrigation_operation_for_VIC_next_day(); //150720LML

 protected:
   virtual const Slope_saturated_vapor_pressure_plus_psychrometric_resistance    //150602RLN
      *get_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference() const; //150602RLN
   inline virtual float64 get_snow_pack_thickness()                        const
      { return snow_pack_thickness; }
      //150604RLN Snow pack not currently implemented,
      // If VIC has snow pack that return VIC's value, otherwise
      // Roger can used the CropSyst snow pack model object
      // (Roger should moved it out of the Weather object to the Land_unit)
      // at which point this override of this method would be obviated.
   /*200208RLN appears to be obsolete
   bool get_cell_irrigation_type_code_append_to                                  //151007RLN
      (const       std::string  &crop_type_name
      ,modifiable_ std::string  &irrigation_type_code)            contribution_;
   */
};
//_VIC::Land_unit_simulation________________________________________2014-12-07_/
}//_namespace_VIC_crop_________________________________________________________/
#endif
// VIC_CROP_LAND_UNIT_SIMULATION_H
#endif //VIC_CROPSYST_VERSION
