#ifndef crop_commonH
#define crop_commonH
#include "crop/crop_element.h"
#ifndef primitiveH
#  include "corn/primitive.h"
#endif
#include "crop/crop_transpiration_interface.h"
#include "corn/math/statistical/sample_cumulative_T.h"
#include "crop/event_scheduler_crop.h"
#ifdef YAML_PARAM_VERSION
#  include "corn/math/numerical/interpolatoin/interpolator_temporal.h"
#endif
// This class provides methods common to CropSyst and CropGro  based crops

#ifndef CROP_TYPES_H
#  include "crop/crop_types.h"
#endif
#include "crop_interfaced.h"
//_____________________________________________________________________________/
namespace Soil                                                                   //181206
{
class Soil_interface;                                                            //050720
class Layers_interface;                                                          //181206_050720
class Salt_interface;                                                            //181206_051118
}
class Weather;                                                                   //130901
class Weather_for_crop_interface;                                                //121115
class Vapor_pressure_deficit_daytime;                                            //140812

namespace CropSyst {
   class Crop_parameters;
   class Crop_nitrogen_common;                                                   //151029
   class Phenology;                                                              //150914
   class Event_scheduler_crop_complete;                                          //151127
   class Crop_CO2_response;                                                      //160418
   class Auto_clip_biomass_mode;                                                 //190616
//______________________________________________________________________________
#ifdef YAML_PARAM_VERSION
class Parameter_adjustment_cowl
: public implements_ CORN::Item
{
   float32                       &value; // reference of value to be adjusted
   Phenology                     &phenology;
   //NYI Parameter_adjustment_settings &settings;
   CORN::Interpolation_temporal interpolator;
      // Dates are relative (year 0 is sowing year).
 public:
   bool update()                                                 rectification_;
};
#endif
//_class Parameter_adjustment_cowl__________________________________2015-09-04_/
class Crop_common
: public extends_ Crop_element                                                   //171212
, public implements_ Crop_model_interface                                        //190617
// Composite because I may want to have submodels as simulation elements
{
 protected:
      friend class CropSyst::Crop_nitrogen_common;                               //151029
      friend class CropSyst::Event_scheduler_crop_complete;                      //151127
 public:
   Crop_parameters   *parameters;                                                //020409
   bool               owns_parameters;                                           //040121
      // Normally a crop will own it's own parameters
      // This is not the case for Crop Calibrator
      // Also simulators which shart parameter files will not be owned
 protected:
   virtual bool get_name(std::wstring &name)                              const; //200324
   modifiable_ ::Soil::Soil_interface *soil;                                     //160415
   const Vapor_pressure_deficit_daytime &daytime_vapor_pressure_deficit;         //140812
 protected: // submodels
   CropSyst::Crop_CO2_response *CO2_response; //will be 0 if not simulating CO2  //020409
 protected:
   float64 intercepted_precipitation_m_today;                                    //160416
   float64 intercepted_precipitation_m_stored;
      // now interception is carried over to next day
   // obsolete Intercepted precipitation is released back to the atmosphere (presumably evaporation)  //140320 moved from Crop_CropSyst
 protected: // Data needed from derived crop class
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   virtual Normal_crop_event_sequence get_growth_stage_sequence()       const=0;
   #endif
   virtual float64 get_GAI_vital()                                      const=0; //981207
   virtual float64 get_GAI_vital_plus_effete()                          const=0; //981207
   virtual float64 get_LAI(bool from_canopy)                            const=0; //981207
 public: // transpiration
   mutable float64 adjusted_ET_coef;                                             //200127
      //200127 Claudio wants to update this now
   mutable float64 ET_ref; // meter                                              //200127_171030_080501
   // Provided by the ET model, but we need to keep for   // in meter
   // 080721 This could be moved to Transpiration_dependent_growth_common class
   // because that is the only place where it is used.

   mutable float64 pot_transpiration_m_unadj;
   mutable float64 pot_transpiration_m_CO2_adj;
   float64 act_transpiration_m; // This could become negative                    //011022
 public: // Reported values for output only
   mutable Reported reported;
 protected:                                                                      //050331
   Crop_transpiration  *transpiration;                                           //050331
 public: // Fract root length
   virtual const float64 *get_total_fract_root_length()                 const=0; //061208
 public: // Stress
   mutable CORN::statistical::Sample_cumulative<float64,float64> water_stress_stats; //180408_010726
 protected:
   float64 process_transpiration_m
      (float64 &limited_pot_transpiration)                        modification_; //051118
      // returns act_transpiration
      // soil_salinity is not const because we update the osmotic potential
   virtual float64 update_adjusted_ET_crop_coefficient()       rectification_=0; //200127_080808
      //200127 was get...
   virtual float64 get_adjusted_ET_crop_coefficient() const {return adjusted_ET_coef;} //200323
 public:  // 'structors
   Crop_common(Crop_parameters *parameters,bool owns_parameters
      ,const Vapor_pressure_deficit_daytime &daytime_vapor_pressure_deficit      //140812
         //now just providing daytime_vapor_pressure_deficit                     //140812
      ,::Soil::Soil_interface *soil                                              //160415
      ,modifiable_ Crop_CO2_response         *Crop_CO2_response_given_optional   //160418
      ,const CORN::date32 &simdate_                                              //170525
      ,CORN::Unidirectional_list &performable_operations);                       //170518
   virtual ~Crop_common();                                                       //040121
 public: // Simulation_element method implementation
   virtual bool initialize()                                    initialization_; //190721
   virtual bool clear_day()                                       modification_; //200413
   virtual bool start_day()                                       modification_; //130903_051028
 public:
   virtual float64 update_evapotranspiration_max                                 //190812_010910
      (float64 ET_ref)                                           rectification_; //121125
      // probably could be moved to Transpiration_dependent_growth_common
   float64 intercept_precipitation(float64 water_reaching_plant)  modification_; //140230
 public: // interface
   inline virtual const char *get_description()                            const
                               { return parameters->description.brief.c_str(); } //150112RLN
   inline virtual bool end_day()             { return Crop_element::end_day(); } //190617
 public:
   RENDER_INSPECTORS_DECLARATION;                                                //150916
 protected:
   virtual float64 get_canopy_interception_global_green()               const=0; //200228
   virtual float64 get_canopy_interception_global_total()               const=0; //200228
};
//_Crop_common__________________________________________________________________
}//_namespace_CropSyst_________________________________________________________/
#endif

