#ifndef transpirationH
#define transpirationH

#include "crop/crop_transpiration_interface.h"
interface_ Soil_interface;
namespace Soil{
   interface_ Salt_interface;
   interface_ Hydrology_interface;
}

// This crop transpiration has both top and root hydraulic conductance
// by soil sublayer.
//______________________________________________________________________________
class Crop_transpiration_2
: public implements_ Crop_transpiration                                          //050316
{
   CropSyst::Crop_parameters_struct::Transpiration &parameters;                  //120726
   float64 param_salinity_osmotic_pot_50;                                        //160415
   float64 param_salinity_salt_tolerance_P;                                      //160415
 private:
   #ifdef __BCPLUSPLUS
   const float64 &CO2_transpiration_adjustment_factor;                           //160314
   // else gcc looses this float64 reference                                     //160418
   #endif
      // ref to CO2 model.
   soil_sublayer_array_64(Root_Activity_Factor);
   mutable soil_sublayer_array_64(active_fract_root_length_output_only);         //050331
      // This is used only for output, it is simply the current total_fract_root_length
   mutable float64 uptake_act_m;                                                 //160414

   float64 dry_soil_root_activity_coef;                                          //170518
   float64 saturated_soil_root_activity_coef;                                    //170518
      // This is probably temporary until Claudio is satisfied with a value
      // This is currently in scenario file.

 private: // external references
   const Soil::Soil_interface *soil;                                             //181206_160415
   const Soil::Salt_interface *soil_salinity;                                    //160415
   const Soil::Hydrology_interface *soil_hydrology;                              //170501
   const float64              *ref_soil_water_pot_at_FC; // profile array        //170501
 protected:
   mutable float64 leaf_water_pot; // Leaf water potential  (made available for output report otherwise it would be local. //071115
 protected:
   float64 calc_leaf_water_potential
      ( float64 plant_hydraulic_conductance
      , float64 leaf_water_pot_onset_of_stress                                   // VB LeafWaterPot_OnsetofStress
      , float64 leaf_water_pot_wilt                                              // VB LeafWaterPot_Wilt
      , float64 pot_transpiration                                                // VB Pot_Transp
      , float64 soil_avg_water_pot)                                       const; // VB Average_Soil_WP
 public:
   Crop_transpiration_2
      (CropSyst::Crop_parameters_struct::Transpiration &parameters_
      ,const Soil::Soil_interface *soil_                                         //160415
      ,float64 param_salinity_osmotic_pot_50                                     //160415
      ,float64 param_salinity_salt_tolerance_P                                   //160415
      ,float64 param_dry_soil_root_activity_coef                                 //170519
      ,float64 param_saturated_soil_root_activity_coef);                         //170518
   virtual float64 calc_transpiration
      (float64 limited_pot_transpiration // pot_ET // N_limited_pot_transpiration
      ,float64 yesterday_fract_green_canopy_interception_solrad                  //020712
      ,float64 param_max_water_uptake_mm                                         //091208
      ,modifiable_ float64 &interception_canopy_m                                //160414_160318
      ,const soil_sublayer_array_64(fract_root_length)                           //010322
      ,soil_sublayer_array_64(water_uptake)                                      //010724
      ,float64 CO2_transpiration_adjustment_factor                               //160418
      ) const;
   inline virtual float64 get_active_fract_root_length(nat8 sublayer)      const
      { return active_fract_root_length_output_only[sublayer]; }
   inline virtual float64 get_leaf_water_pot()                             const
      { return leaf_water_pot; }                                                 //071114
   inline virtual float64 get_uptake_actual()                              const //160414
      { return uptake_act_m; }
 protected:
   float64 uptake_actual
      (nat8  soil_num_sublayers
      ,float64 pot_crop_water_uptake_mm
      ,float64 max_crop_water_uptake_mm
      ,const soil_sublayer_array_64(fract_root_length_)
      ,soil_sublayer_array_64(water_uptake_m_returned))                   const; //160415
 public: // observation
   RENDER_INSPECTORS_DECLARATION;                                                //160627
};
//______________________________________________________________________________
#endif

