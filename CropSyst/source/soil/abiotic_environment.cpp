#include "organic_matter/OM_const.h"
#include "soil/abiotic_environment.h"

#include "common/residue/residue_const.h"
#include "soil/hydrology_I.h"
#include "soil/temperature_hourly_I.h"

namespace Soil
{
//______________________________________________________________________________
float64 T_min[Temperature_function_mode_COUNT] = {-5.0,-5.0,-5.0};               //141203
float64 T_opt[Temperature_function_mode_COUNT] = {35.0,35.0,35.0};               //080904
float64 T_max[Temperature_function_mode_COUNT] = {50.0,50.0,50.0};               //080904
/*141230
float64 T_min[Temperature_function_mode_COUNT] = { 0.0, 0.0, 0.0};            //080904
float64 T_opt[Temperature_function_mode_COUNT] = {35.0,35.0,55.0};            //080904
float64 T_max[Temperature_function_mode_COUNT] = {50.0,50.0,80.0};               //080904
*/
//______________________________________________________________________________
float64 calc_temperature_function
(float64 temperature_avg   //rename this to simply temperature  soil layer temperature or air temperature for surface   Hourly value normally
,float64 T_min, float64 T_opt, float64 T_max)
{
   // It appears this function works for any time step
   // May be the case that different time steps use different T_min, T_opt, T_max
   // but I dont see a time factor in this, the time factor appears
   // to be applied in the potential decomposition function that calls this
   static const float64 min_temperature_function = 0.0000001;                    //080903 0.001;
   float64 temp_funct = min_temperature_function;
   if ((temperature_avg >=  T_min) && (temperature_avg <= T_max))
   {  float64 Q = (T_min - T_opt) / (T_opt - T_max);
      temp_funct = (pow((temperature_avg - T_min),Q) * (T_max - temperature_avg))
                 / (pow((T_opt - T_min),Q) * (T_max -T_opt));
   }
   return CORN::must_be_between<float64>(temp_funct,min_temperature_function,1.0);   //070707
//080712       Claudio decided this should be 0.00001 not 0.001
// This was changed when we added Single organic matter pool
// Claudio entered different values in the Single organic matter pool
// spreadsheet and decided on this limit.
} // Same as CSVB 051216
//_calc_temperature_function___________________________________________________/
float64 calc_moisture_function
(float64 water_filled_porosity
,float64 value_at_saturation)
{
#define WFP_min   0.1
#define WFP_low   0.5
#define WFP_high  0.7
   float64 moisture_funct =
   ((water_filled_porosity >= WFP_min) && (water_filled_porosity < WFP_low))
   ? ((water_filled_porosity - WFP_min) /(WFP_low - WFP_min))
   : ((water_filled_porosity >= WFP_low ) && (water_filled_porosity <= WFP_high))
      ? 1.0
      : ((water_filled_porosity > WFP_high) && (water_filled_porosity <= 1.0))   //060425
        ?  value_at_saturation +
         (1.0 - value_at_saturation)
         * CORN_sqr((1.0 - water_filled_porosity) / (1.0 - WFP_high))
        : 0.0;                                                                   //060425
   if (moisture_funct < 0.0) moisture_funct = 0.00001;                           //070707
   return moisture_funct;
}
//_calc_moisture_function_______________________________________________________
Abiotic_environment_profile::Abiotic_environment_profile
(const Soil::Layers_interface                 *soil_layers_ref
,const Soil_temperature_profile_hourly_interface *soil_temperature_profile_hourly_ref
,const Soil::Hydrology_interface                  *soil_hydrology_ref)
: soil_layers                                                  (soil_layers_ref) //181130
, soil_temperature_profile_hourly          (soil_temperature_profile_hourly_ref) //181130
, soil_hydrology                                            (soil_hydrology_ref) //181130
{  for (nat8 layer = 0; layer < MAX_soil_layers_alloc; layer++)
      soil_abiotic_environment_layer[layer].layer_informational = layer;
}
//_Soil_abiotic_environment_profile:constructor_________________________________
bool Abiotic_environment_profile::update
   (float64 residue_water_hold)
{  // Warning may need to iterate over hours to get the surface_temperature_function_daily_avg;
   float64 surface_moisture_function = CORN::must_be_between<float64>
      (residue_water_hold   / MAX_RESIDUE_HOLD_m3_kg,0.1,1.0);
      // Warning need to check if this is current
   soil_abiotic_environment_layer[0].update(surface_moisture_function,0.0);
      // minimized_temperature_moisture_function is not used for surface residue
   if (soil_layers)                                                              //181217
   for (nat8  layer = 1; layer <= soil_layers->count(); layer++)                 //150727RLN
   {  Abiotic_environment_layer &abiotic_env = soil_abiotic_environment_layer[layer]; //190218
      float64 minimized_temperature_moisture_function = 0; // I think only for decomp
      float64 water_filled_porosity_daily = soil_hydrology->get_water_filled_porosity(layer,24);
      float64 moisture_function_daily     = calc_moisture_function
         (water_filled_porosity_daily,organic_matter_moisture_function_value_at_saturation);
      for (int TF_mode = 0; TF_mode< Temperature_function_mode_COUNT; TF_mode++) //080904
      {  abiotic_env.temperature_function_hourly[TF_mode].clear();
         abiotic_env.minimized_temperature_moisture_function_for_mode[TF_mode].clear(); //081024
      }
      for (Hour hour = 0; hour < 24 ; hour++)
      {  float64 water_filled_porosity_hour = soil_hydrology->
            get_water_filled_porosity(layer,hour);
         float64 moisture_function_hour     =
            calc_moisture_function(water_filled_porosity_hour,organic_matter_moisture_function_value_at_saturation);
         float64 soil_temperature_lyr  =soil_temperature_profile_hourly->get_temperature(hour,layer);
         for (int TF_mode = 0; TF_mode< Temperature_function_mode_COUNT; TF_mode++) //080904
         {  float64 temperature_function_hour = calc_temperature_function(soil_temperature_lyr, T_min[TF_mode],  T_opt[TF_mode],  T_max[TF_mode]);
            abiotic_env.temperature_function_hourly[TF_mode].append(temperature_function_hour);  //060510
            float64 minimum_temperature_moisture_function  = std::min<float64>(temperature_function_hour,moisture_function_hour);   //081024
            abiotic_env.minimized_temperature_moisture_function_for_mode[TF_mode].append(minimum_temperature_moisture_function);   //081024
         }
         float64 minimum_temperature_moisture_function_for_decomp // currently only used for decomposition
          = std::min<float64>
            (abiotic_env.temperature_function_hourly
               [decomposition_temperature_function].get(hour)
            ,moisture_function_hour);
         minimized_temperature_moisture_function += minimum_temperature_moisture_function_for_decomp/ 24.0;
      }
      abiotic_env.update
         (moisture_function_daily,minimized_temperature_moisture_function);
   }
   return true;
}
//_update______________________________________________________________________/

const Abiotic_environment_layer &Abiotic_environment_profile
::ref_soil_abiotic_environment_layer(nat8 layer)                           const
{return soil_abiotic_environment_layer[layer] ;}
      Abiotic_environment_layer &Abiotic_environment_profile
      ::mod_soil_abiotic_environment_layer(nat8 layer)
{return soil_abiotic_environment_layer[layer] ;}
//__________________________________________________________________2919-02-15_/

bool Abiotic_environment_layer::update
(float64 _moisture_function
,float64 _minimized_temperature_moisture_function)
{  moisture_function             = _moisture_function;
   minimized_temperature_moisture_function_for_decomp = _minimized_temperature_moisture_function;
   return true;
}
//_update______________________________________________________________________/
float64 Abiotic_environment_layer::get_temperature_function_daily_mean(Temperature_function_mode TF_mode)        const { return temperature_function_hourly[TF_mode].get_mean(); }
float64 Abiotic_environment_layer::get_temperature_function_daily_sum(Temperature_function_mode TF_mode)         const { return temperature_function_hourly[TF_mode].get_sum(); }
float64 Abiotic_environment_layer::get_temperature_function_at(Hour hour,Temperature_function_mode TF_mode )     const { return temperature_function_hourly[TF_mode].get((uint32)hour);}  //060510
float64 Abiotic_environment_layer::get_moisture_function()                      const { return moisture_function;}
float64 Abiotic_environment_layer::get_minimized_temperature_moisture_function()const { return minimized_temperature_moisture_function_for_decomp;}
//______________________________________________________________________________


} // namespace Soil

