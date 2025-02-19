#ifndef crop_N_V4H
#define crop_N_V4H
#include "crop/crop_N_common.h"
#include "common/biometeorology/parameter/slope_SVP_curve_psychrometric_resistance.h"
namespace CropSyst
{
namespace deprecated // This NS is temporary to see if Crop_nitrogen_V4 is still used DELETE THIS NS
{
//______________________________________________________________________________
class Crop_nitrogen_V4
: public extends_ Crop_nitrogen_common
{
 private:
   Slope_saturated_vapor_pressure_plus_psychrometric_resistance                  //owned
      *slope_VPsat_plus_psychrometric_resistance_stressed;                       //150601
      // This may be only V4 151029
 protected:
   const Slope_saturated_vapor_pressure_plus_psychrometric_resistance            //referenced
      *slope_VPsat_plus_psychrometric_resistance_reference;                      //150601
      // This was in Crop_nitrogen_common, but only used in V4

 public:
   Crop_nitrogen_V4
      (Crop_parameters::Nitrogen &_parameters
      ,Crop_complete             &_crop                                          //050721
      ,Soil::Soil_interface      *_soil                                          //181206_050721
      ,Soil::Nitrogen            *_chemicals                                     //181206_050721
      ,const Slope_saturated_vapor_pressure_plus_psychrometric_resistance        //150601
         *_slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference//150601
         // although _slope_saturated_vapor_pressure_plus_psychrometric_resistance_reference
         // is optional, if it is not specified, there will be no correction for transpiration
      ,const float64 &transpiration_use_efficiency_unused); // Not used in this version //151104
   virtual ~Crop_nitrogen_V4();
 public: // Simulation element implementations
   bool initialize()                                            initialization_; //150601
   #if (CROPSYST_VERSION==4)
   // 130408 obsolete in V5 because now change is iniciated by new end_dilution_curve parameter
   virtual void know_start_flowering()                            modification_;
   #endif
 public: // processing
   virtual bool start_season()                                    modification_; //061214
   virtual bool start_day()                                       modification_; //040206
   virtual float64 calc_limited_growth   // actually update                      //011022
      (float64 water_limited_growth
      ,float64 act_to_pot_transpiration_ratio                                    //020719
      ,float64 root_activity_factor)                              modification_; //080728
 public:
   virtual float64 get_N_max_conc_at_emergence()                          const; //061212
   virtual float64 update_limited_pot_transpiration()             modification_; //011117
};
//_Crop_nitrogen_V4_________________________________________________2015-10-29_/
}
} // namespace deprecated // This NS is temporary to see if Crop_nitrogen_V4 is still used DELETE THIS NS
// namespace CropSyst
#endif
