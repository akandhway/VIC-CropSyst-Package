#ifndef hydrology_AH
#define hydrology_AH

#include "options.h"
#include "soil/hydrology_I.h"
#include "common/soil/layering.h"
#include "common/CS_parameter_float64.h"
#include "common/physics/standard_constants.h"
//______________________________________________________________________________
using namespace CORN;
namespace Soil {                                                                 //181206
interface_ Layers_interface;
interface_ Freezing_profile;                                                     //181206
interface_ Structure;                                                            //181206
//______________________________________________________________________________
class Hydrology_abstract
: public Hydrology_interface
{
public:

 protected:
   const Layers_interface  &soil_layers_ref;                                     //060505
   cognate_ const Freezing_profile *freezing_profile_optional;                   //070824
      // 0 if freezing not enabled Owned by Soil_cropsyst_specific  (but may want to move here?)
   const Structure &soil_structure;                                              //071001
      // Although normally considered fairly constant,
            // hydraulic_properties_ref can change at any time
            // (I.e. in response to tillage, dust mulching.)
            // This is why we are using a pointer.
            // We must always use accessor to get the hydraulic properties.
   const Hydraulic_properties_interface &hydraulic_properties_ref;               //051204
      // The current soil hydraulic properties (these may be redone when there is erosion, or dust mulching ). owned by soil.
 public:
      // The following represents the current water storage.
      // This should be moved to another class (Water profile)
   soil_layer_array64(ice_water_content);      // m3/m3 volumetric               //990213
   soil_layer_array64(ice_content_yesterday);  // m3/m3 volumetric               //990315
   soil_layer_array64(water_flux_moved_to_FD);                                   //990210
      // m for cascade (In Finite difference: liquid water flux density Kg /(m2*sec)  in daystep and saltdaystep only)
   soil_layer_array64(plant_avail_water);       // 0-1    current
   soil_layer_array64(reference_water_content);
   soil_layer_array64(real_wilt_point);
   // The liquid water content by hour is recorded for moisture function used in denitrification, and soil temperature hourly
   bool     water_content_hourly_available;                                      //070117
   float64  water_content_hourly[24][MAX_soil_layers_alloc];  // [Hour][layer]   //070117
 private:
   contribute_ float64 depth_profile_water[2]; contribute_ CORN::Quality_code depth_profile_water_quality[2];                 //150123_130930
      /* index 0 is liquid only, 1 total including ice (liquid water equivelent) */
public:   // 'structors
   Hydrology_abstract
      (const Layers_interface               &soil_layers_ref_
      ,const Hydraulic_properties_interface &hydraulic_properties_ref_
      ,const Structure                      &soil_structure_);
   Hydrology_abstract
      (const Hydrology_abstract &copy_from
      ,float32 fraction = 1.0);                                                  //140610
   // cannot copy because abstract
public:   // setup
   inline void know_freezing_profile                                             //070824
      (const Freezing_profile *_freezing_profile)                     cognition_ { freezing_profile_optional = _freezing_profile; }
   inline const Layers_interface &get_soil_layers()                        const { return soil_layers_ref; } //070117
   inline const Hydraulic_properties_interface                                   //070117
      &get_hydraulic_properties()                                          const { return hydraulic_properties_ref; } //140425
         // The current soil hydraulic properties. owned by soil.
         // (these may be adjusted when there is erosion, or dust mulching )
   virtual void expand_last_layer_for_boundary_conditions
      ( nat8 number_layers, nat8 number_expanded_layers)          modification_;
public: //accessors
   inline virtual float64 get_plant_avail_water                 ( nat8 layer)    const { return plant_avail_water[layer]; };
          virtual float64 get_liquid_water_content_volumetric   ( nat8 layer)    const { UNUSED_arg(layer); return 0; }; // Actually an abstract function but compiler is complaining
          virtual float64 get_water_plus_ice_content_volumetric ( nat8 layer)    const;  // was get_all....
          virtual float64 get_water_plus_ice_content_gravimetric( nat8 layer)    const;//071001
   inline virtual float64 get_real_wilt_point  (nat8 layer)                const //140610
      { return real_wilt_point[layer]; }
   inline virtual float64 get_reference_water_content(nat8  sublayer)      const //140610
      { return reference_water_content[sublayer]; }
   virtual float64 initialize_moisture(nat8 layer                                //170320
      ,Moisture_characterization moisture ,float64 WC_initial)    modification_;
   virtual float64 set_water_content_volumetric_layer
            (nat8 layer, float64 new_water_content)             modification_=0;
                     // set_water_content_volumetric must be used sparingly.
                     // Only use it for initialization and sudden significant
                     // change of the soil profile
                     // (I.e. recalibration, watertable changes, dustmulching).
   virtual void record_water_content_for_hour(Hour hour);
            // Note that water content should have been previously stored.
   virtual float64 get_water_plus_ice_content_volumetric_at
      ( nat8 layer,Hour hour)                                             const; //070117
   virtual float64 change_liquid_water_content_by_volumetric
      ( nat8 layer, float64 change_WC)                          modification_=0; //070813
          // This adjusts only the liquid water content
          // (because presumably we are moving water, and I does not move).
          // returns the total water content, including ice.
   inline virtual float64* ref_plant_avail_water()
      { return plant_avail_water; }                                              //140610
 public: // Set accessors
   inline virtual float64 set_plant_avail_water
      (nat8 layer, float64 _plant_avail_water_sl)                                //140610
      { return plant_avail_water[layer] = _plant_avail_water_sl; }
   inline virtual float64 set_real_wilt_point
      (nat8 layer, float64 _real_wilt_point)                                     //140610
      { return real_wilt_point[layer] = _real_wilt_point; }
   inline virtual float64 set_reference_water_content
      (nat8  layer, float64 _reference_water_content) modification_
      {   return reference_water_content[layer] = _reference_water_content; }    //140610
 public: // Accessors for freezing
   virtual bool    is_frozen                           (nat8 layer)affirmation_; //061005
   virtual float64 get_ice_content_volumetric          (nat8 layer)       const; //140826_061005
   virtual float64 get_ice_content_volumetric_yesterday(nat8 layer)       const; //140824_061005
 protected: friend class Soil_freezing_profile_jumakis; // set state accessors
 public: // status
   virtual bool is_saturated                           (nat8 layer)affirmation_; //060509
 public: // process
   virtual bool start_day()                                       modification_; //131125_070117
   virtual void update_liquid_and_ice_water_content()             modification_; //070825
 public: // calculated
   virtual float64 get_water_filled_porosity ( nat8 layer, Hour for_hour)  const; // use for_hour for daily value
   virtual float64 calc_available_water_fraction(nat8 layer , bool real_wilt_based) calculation_; //160601_060509
   virtual float64 calc_available_water_depth_to_depth(float64 depth) calculation_;//181105_160601
   virtual float64 calc_available_water_depth_to_layer(nat8 layer) calculation_; //160601

   inline virtual float64 calc_available_water_depth_to (float64 depth) calculation_//181105_160601
   // this method name is osbolete
   { return calc_available_water_depth_to_depth(depth); }

   virtual float64 calc_water_depth_to_layer(bool all_water, nat8 layer=PROFILE_LAYER_INDEX)  const; //181105
   virtual float64 calc_water_depth_to_depth(bool all_water, float32 depth=9999) const; //181105
   virtual float64 calc_ice_depth_to(float64 depth = 9999)         calculation_; //160724
 public: //
   virtual float64 get_air_entry_pot
      (nat8 layer,bool consider_ice_content,bool FC_PWP_based)            const; //990321
 private:
   virtual bool set_upper_and_lower_liquid_water_content_volumetric
      ( nat8 layer, float64 layer_liquid_WC)
      { UNUSED_arg(layer); UNUSED_arg(layer_liquid_WC); return false; }          //200208_070827
 public:
   virtual float64 calc_water_potential                                          //990316
      ( nat8 layer,bool consider_ice,bool FC_PWP_based)                   const;
   inline virtual float64 get_water_potential                                    //990316
      ( nat8 layer) const
      { return calc_water_potential(layer, true /*consider_ice*/, false /*FC_PWP_based*/) ; }
   //181105 now use calc_water_depth_to_layer(PROFILE_LAYER_INDEX); virtual float64 calc_water_depth_profile(bool all_water)               const; //130930
   virtual float64 provide_water_depth_profile(bool all_water)       provision_; //130930
      // Water depth (m) when including all water (ice) is
      // liquid water equivelent.
   void invalidate()                                              contribution_; //130930
   virtual float64 get_K_Theta
      (nat8 layer
      , float64 Theta, float64 Theta_s)                                   const;
   float64 calc_Theta_b
      (nat8 layer
      ,float64 Campbell_b)                                                const;
   float64 calc_h_Theta_b(float64 air_entry_potential)                    const;
   float64 calc_Theta_h_Campbell
      (float64 h, float64 Air_Entry,float64 Theta_s, float64 Campbell_b)  const;
   float64 extract_water_mm(float64 water_uptake_mm[] , nat8 start_layer) modification_;
 public:
   RENDER_INSPECTORS_DECLARATION;                                                //150916
};
//_class_Hydrology_abstract_________________________________________2006-05-04_/
} // namespace Soil                                                              //181206
#endif

