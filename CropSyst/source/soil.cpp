#include "options.h"
#	include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#  include "soil.h"
#  ifdef CHEM_PROFILE
#     include "soil/chemical_profile.h"
#     include "soil/chemicals_profile.h"
#  endif
#  ifdef EROSION
#     include "soil/erosion_RUSLE.h"
#  endif
#ifdef SALINITY
#     include "soil/salinity_profile.h"
#endif
#ifdef RUNOFF
#  include "soil/runoff.h"
#endif
#include "CS_suite/simulation/CS_land_unit_meteorological.h"
#include "weather/weather_provider.h"
#  ifndef SOIL_TILLAGE_H
#     include "soil/tillage_effect.h"
#  endif
#ifdef SOIL_TEMPERATURE
#  include "soil/temperature_profile.h"
#endif
#include "soil/temperature_hourly.h"
#include "corn/measure/measures.h"
#include "soil/infiltration_I.h"
#include "soil/soil_evaporator.h"
namespace CropSyst {                                                             //130912
//______________________________________________________________________________
bool Soil_components::check_automatic_irrigation
(float64 max_allowable_depletion // 0-1
   , float64 depletion_observe_depth) // m
{  nat8 max_observe_sublayer = ref_layers()->closest_layer_at(depletion_observe_depth);//150701LML_061003
   float64 max_pot_depletion = 0.0; // m
   float64 curr_depletion = 0.0; // m
   const Soil::Hydrology_interface &hydrology = *ref_hydrology();                //160412_160205
   const Soil::Hydraulic_properties_interface &hydraulic_properties              //160205
      = *ref_hydraulic_properties();                                             //160412
   for (int16 sublayer = 2; sublayer <= max_observe_sublayer; sublayer++)
   {  float64 sat_WC = hydraulic_properties                                      //160205_150701LML
         .get_saturation_water_content_volumetric
            (sublayer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);               //051204
      float64 FC_soil_param = hydraulic_properties
         .get_field_capacity_volumetric(sublayer);                               //051204
      float64 PWP = hydraulic_properties
         .get_permanent_wilt_point_volumetric(sublayer);                         //051204
      float64 thickness = ref_layers()->get_thickness_m(sublayer);               //150424_061003
      float64 FC = infiltration_model == FINITE_DIFFERENCE_INFILTRATION          //130301_060117
      ? sat_WC * pow((hydrology.get_air_entry_pot(sublayer,false,false) /-33.0)  //990320
         , 1.0 / hydraulic_properties.get_Campbell_b(sublayer, false))           //990320
      : FC_soil_param;                                                           //990320
      max_pot_depletion += (FC - PWP) * thickness;
      float64 WC = hydrology.get_water_plus_ice_content_volumetric(sublayer);
      curr_depletion += (FC - WC) * thickness;
   }
   bool perform_irrigation = curr_depletion >=
      (max_pot_depletion * max_allowable_depletion);                             //990320
   return perform_irrigation;
}
//_check_automatic_irrigation_______________________________________2006-06-03_/
// This function returns the amount of water required to refill the soil
// profile to the recharge capacity
float64 Soil_components::determine_recharge_requirements
(float64 net_irrigation_mult
, float64 root_length_m

// This probably should be root_depth_m especially since the roots should be the appreciable root depth (limited to soil profile)

,float32 refill_point)                                                     const
{  float64 tot_irrigation = 0.0;
   const Soil::Layers_interface    &layers    = *ref_layers();                   //160205
   const Soil::Hydrology_interface &hydrology = *ref_hydrology();                //160412_160205
   const Soil::Hydraulic_properties_interface &hydraulic_properties              //160205
      = *ref_hydraulic_properties();                                             //160412
   nat8 recharge_to_sublayer = (nat8)
      ((CORN::is_zero<float64>(root_length_m))                                   //030819
      ? layers.count():layers.closest_layer_at(root_length_m) + 1);              //030819
   for (nat8 sublayer = 1; sublayer <= recharge_to_sublayer; sublayer++)
   // Actually this is now layer need to rename sublayer to layer
   {  float64 FC_sublayer = hydraulic_properties.get_field_capacity_volumetric
         (sublayer);
      float64 target_refill_WC_volumetric = FC_sublayer; // This was the original value prior to V4.1.58 //051228
      float64 PWP_sublayer =
         hydraulic_properties.get_permanent_wilt_point_volumetric(sublayer);     //051228
      target_refill_WC_volumetric = PWP_sublayer +
         ((FC_sublayer - PWP_sublayer) * refill_point);                          //051228
         // the refill will now be somewhere between FC and PWP
      float64 required_volumetric = CORN::must_be_0_or_greater<float64>          //051228
      ((target_refill_WC_volumetric - hydrology.
            get_water_plus_ice_content_volumetric(sublayer)));                   //990311 claudio conf.
      float64 required_irrigation_depth_layer =
         required_volumetric * layers.get_thickness_m(sublayer);                 //061003
      tot_irrigation += required_irrigation_depth_layer;
   }
   tot_irrigation *= net_irrigation_mult;
   return tot_irrigation;
}
//_determine_recharge_requirements_____________________________________________/
bool Soil_components::start_day()
{  bool started = true;                                                          //140822
   if (tillage_effect) started &= tillage_effect->start_day();                   //060929
   if (hydrology)      started &= hydrology->start_day();                        //070117
   // Should be moved to soil_base::start_day() but not setup yet.
   started &= Soil_base::start_day();                                            //080702
   return started;                                                               //140822
}
//_start_day________________________________________________________2005-12-06_/
bool Soil_components::end_day()                     modification_
{  bool ended = true;                                                            //140822
   #ifdef CHEM_PROFILE
   if (chemicals) ended &= chemicals->end_day();
   #endif
   for (nat8 sublayer = 1; sublayer <= ref_layers()->count(); sublayer++)        //150424_140212
      record_all_water_content[sublayer] =                                       //140212
         ref_hydrology()->get_water_plus_ice_content_volumetric(sublayer);       //160412_140212
   ended &= Soil_base::end_day();                                                //160719
   return ended;
}
//_end_day__________________________________________________________2013-09-11_/
bool Soil_components::process_day()                                              //150611
{  bool result = Soil_base::process_day();                                       //150611
   if (tillage_effect)                                                           //060929
   {  bool still_have_tillage_effects =tillage_effect->process();
      if (!still_have_tillage_effects)
      {  delete tillage_effect; tillage_effect = 0; }
   }
   return result;
}
//_process_day______________________________________________________2005-12-05_/
Soil_components::Soil_components
   (const Smart_soil_parameters              *parameters_                        //160916
   ,bool                                      parameters_given_                  //160916
   ,const CORN::date32                       &today_                             //170524
   ,const CS::Annual_temperature             &annual_temperature_                //150424
   #ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   ,const CS::Land_unit_meteorological       &meteorological_                    //151022
   #endif
   )                                                                             //131104
: Soil_base                                                                      //160916_131001_980801
   (parameters_,parameters_given_,today_)
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
, temperature_profile(0)                                                         //020620
, temperature_profile_hourly(0)
, meteorological(meteorological_)                                                //151022
#endif
, chemicals(0), erosion(0)                                                       //060712
, tillage_effect(0)                                                              //060929
, annual_temperature(annual_temperature_)                                        //150427
{}
//_constructor__________________________________________________________________
Soil_components::~Soil_components()
{
   #ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   delete temperature_profile;         temperature_profile = 0;
   delete temperature_profile_hourly;  temperature_profile_hourly = 0;
   // For MICRO_VERSION Soil object does not own temperature_profile_hourly.     1150810RLN
   #endif
   delete chemicals;                   chemicals = 0;
   #ifdef EROSION
   delete erosion;                     erosion = 0;
   #endif
   delete tillage_effect;              tillage_effect = 0;                       //060929
}
//_~Soil____________________________________________________________2002-05-27_/
#ifdef CHEM_PROFILE
Soil::Chemicals_profile *Soil_components::provide_chemicals()
{  if (!chemicals)
      chemicals = new Soil::Chemicals_profile
         (*this, *ref_layers()
         #ifdef MBVB_SOIL_TEMPERATURE
         ,*ref_temperature_profile()                                             //160412_141212
         #else
         ,*provide_temperature_profile()
         #endif
         );
   return chemicals;
}
//_provide_chemicals________________________________________________2005-11-20_/
#endif
#ifdef SALINITY
bool Soil_components::set_salinity(Soil::Salt_interface *salinity_dSm,bool keep_it)
{  provide_chemicals();
   chemicals->set_salinity(salinity_dSm, keep_it);
   return salinity_dSm != 0;                                                     //060206
}
//_set_salinity_____________________________________________________2005-11-20_/
const Soil::Salt_interface *Soil_components::ref_salinity()                const //160412
{  return chemicals ? chemicals->ref_salinity() : 0;                             //160412
}
//_ref_salinity_____________________________________________________2005-11-20_/
Soil::Salt_interface *Soil_components::mod_salinity()                            //160412
{  return chemicals ? chemicals->mod_salinity() : 0;                             //160412
}
//_mod_salinity_____________________________________________________2005-11-20_/
#endif
Soil::Nitrogen *Soil_components::mod_nitrogen()
{ // This is currently temporary, I need to create a separate class for soil N
   Soil::Nitrogen *N_interface = 0;
   #ifdef CHEM_PROFILE
   if (chemicals && chemicals->NO3 && chemicals->NH4)                            //060731
      N_interface = chemicals;
   #endif
   return N_interface;
}
//_mod_nitrogen_____________________________________________________2005-11-20_/
const Soil::Nitrogen *Soil_components::ref_nitrogen()                      const //160412
{ // This is currently temporary, I need to create a separate class for soil N
   Soil::Nitrogen *N_interface = 0;
   #ifdef CHEM_PROFILE
   if (chemicals && chemicals->NO3 && chemicals->NH4)                            //060731
      N_interface = chemicals;
   #endif
   return N_interface;
}
//_ref_nitrogen_____________________________________________________2005-11-20_/
float64 Soil_components::get_act_evaporation()                             const
{  float64 dust_mulch_evaporation_today =                                        //060929
   tillage_effect ? tillage_effect->get_dust_mulch_evaporation_today() : 0.0;
   return ref_evaporator()->output_evaporation_actual_daily                      //160719
        + dust_mulch_evaporation_today;
}
//_get_act_evaporation______________________________________________2004-10-27_/
#ifdef PHOSPHORUS

/* NYI
bool  Soil::enable_phosphorus(soil_horizon_array32(horizon_P))
{  provide_chemicals();
return chemicals && chemicals->enable_phosphorus(horizon_P);
}
 */
float64 Soil_components::know_phosphorus_in_profile(float32 soil_P)
{  provide_chemicals();
   return chemicals ? chemicals->know_phosphorus_in_profile(soil_P) : 0.0;
}
//_know_phosphorus_in_profile_______________________________________2011-08-01_/
#endif
#ifdef NITROGEN

bool Soil_components::enable_nitrogen
   (bool enable_denitrification_with_CO2                                         //060508
   , Infiltration_model infiltration_model
   ,Soil::Abiotic_environment_profile *soil_abiotic_environment_profile           //060510
   #if (CROPSYST_VERSION==4)
   , float64 max_transform_depth
   , float64 adjust_denitrify
   ,float64 adjust_nitrify_V4_1
   #endif
   ,const CropSyst::Denitrification_parameters &denitrification_parameters       //150824
   , soil_horizon_array32(N_NO3),
   soil_horizon_array32(N_NH4), Water_table_curve *NO3_water_table_curve         //080213
      // may be 0 if no water table consideration
   , Water_table_curve *NH4_water_table_curve)                                   //080213
      // may be 0 if no water table consideration
{
   provide_chemicals();
   #ifdef CHEM_PROFILE
   return chemicals && chemicals->enable_nitrogen
      (enable_denitrification_with_CO2                                           //060508
      ,infiltration_model, soil_abiotic_environment_profile
      #if (CROPSYST_VERSION==4)
      ,max_transform_depth, adjust_denitrify, adjust_nitrify_V4_1
      #endif
      , denitrification_parameters                                               //150824
      , mod_act_water_entering_soil()                                            //160412_150925
      , N_NO3, N_NH4, NO3_water_table_curve                                      //080213
      , NH4_water_table_curve                                                    //080213
      , simdate_raw);                                                              //170524
   #else
   return false;
   #endif
}
//_enable_nitrogen__________________________________________________2002-06-20_/
#endif
// This function actually removes eroded soil,
// It should only be called when eroding the soil. In CropSyst we do not remove the evaporative layer
void Soil_components::remove_horizon_2()
{
   // BIG WARNING!!  I have moved some of the soil horizon
   // parameters to the soil_hydraulic_properties object (now stored as sublayers instead of using in_layer_of
   // Need to have that object delete the sublayered value!!!! 051204
   if (parameters_ownable->horizon_count > 2)                                    //190402
   {
      #if (CROPSYST_VERSION == 4)
      Smart_soil_parameters *parameters_modifiabled                              //161008
         = const_cast<Smart_soil_parameters *>(parameters_ownable);
      for (nat8 layer = 2; layer <= parameters_ownable->horizon_count; layer++)
      {  parameters_modifiabled->horizon_thickness[layer]
            = parameters_modifiabled->horizon_thickness[layer + 1];
         parameters_modifiabled->horizon_permanent_wilt_point[layer]
            = parameters_modifiabled->horizon_permanent_wilt_point[layer + 1];
         // warning //051204
         parameters_modifiabled->horizon_field_capacity[layer]
            = parameters_modifiabled->horizon_field_capacity[layer + 1]; // warning
         parameters_modifiabled->horizon_bulk_density_g_cm3[layer]
            = parameters_modifiabled->horizon_bulk_density_g_cm3[layer + 1]; // warning
         parameters_modifiabled->horizon_bypass_coef[layer] = parameters_ownable->horizon_bypass_coef[layer + 1];

         parameters_modifiabled->NH4_Langmuir.K.set(layer,parameters_modifiabled->NH4_Langmuir.K.get(layer+1));                  //190404
         parameters_modifiabled->NH4_Langmuir.Q.set(layer,parameters_modifiabled->NH4_Langmuir.Q.get(layer+1));                  //190404
      }
      parameters_modifiabled->horizon_count -= 1;
      #else
      assert(false);
      // soil parameters are now immutable
      // actually we should only be affecting the active soil hydrology parameters

      #endif
   }
}
//_remove_horizon_2_________________________________________________1999-03-18_/
#ifdef EROSION
void Soil_components::erode()
{  float64 soil_loss = erosion->total_soil_loss; // tn/ha                        //980731
   if (CORN::is_zero<float64>(soil_loss))
      return; // Nothing to erode. 080707
   soil_horizon_array32(unused);
   float64 soil_horizon_loss_thickness = tn_ha_to_m(soil_loss,
      structure->get_bulk_density_g_cm3(2));                                     //051204
   float64 soil_sublayer_loss_thickness = soil_horizon_loss_thickness;
   // leave at least 30 cm of soil to avoid patholigic conditions
   if (layers->get_depth_m() > 0.3)                                              //980925
   {  while (soil_horizon_loss_thickness > 0)
      {
         #if (CROPSYST_VERSION == 4)
         if (soil_horizon_loss_thickness > parameters_ownable->horizon_thickness[2])
         // totally remove horizon
         {  soil_horizon_loss_thickness = soil_horizon_loss_thickness -
               parameters_ownable->horizon_thickness[2];
            remove_horizon_2();                                                  //990318
         } else
         {
            Smart_soil_parameters *parameters_modifiabled                        //161008
            = const_cast<Smart_soil_parameters *>(parameters_ownable);

            parameters_modifiabled->horizon_thickness[2] -= soil_horizon_loss_thickness;
            soil_horizon_loss_thickness = 0.0;
         }
         #else
         assert(false);
         // parameters horizon are now immutable,  (the are probably immutable in V4 as well)
         #endif
      }
      nat8 num_layers = layers->count();
      if (layers->get_depth_m(num_layers) > 0.3)
      {
         while (soil_sublayer_loss_thickness > 0.0)
         {
            if ((soil_sublayer_loss_thickness > layers->thickness[2]) &&
               (num_layers >= 2)) // totally remove sublayer
            {  soil_sublayer_loss_thickness -= layers->thickness[2];
               if (num_layers > 2)
               {  layers->thickness[2] = layers->thickness[3];
                  // We need to remove sublayer 2 from the chemical profiles.    //960828
                  #ifdef CHEM_PROFILE
                  Container::Iterator *profile_iterator =
                     chemicals->chemical_profiles.iterator();
                  for (Soil::Chemical_profile * chem_profile = chemicals ?
                     dynamic_cast<Soil::Chemical_profile*>                       //170425
                     (profile_iterator->first()) : 0;
                     chem_profile;
                     chem_profile =
                        dynamic_cast<Soil::Chemical_profile*>                    //170425
                        (profile_iterator->next()))
                     chem_profile->remove_sublayer_2(num_layers);
                  delete profile_iterator;
                  #endif
               }
               num_layers -= 1;
            }
            else
            {  layers->thickness[2] -= soil_sublayer_loss_thickness;
               soil_sublayer_loss_thickness = 0.0;
            }
         }
      }
      // Water table observations must already have taken in account soil loss
      reinitialize(specific,unused, false, true);                                //170320_060229
      // NYI this option to subdivide horizons should be taken from model_options
   }
}
#endif
//_erode__________________________________________________________________1998_/
bool Soil_components::preinitialize
(Infiltration_model           infiltration_model                                 //041116
, Soil::Infiltration_nominal *infiltration_submodel_                             //060206
, Soil::Hydrology_abstract   *hydrology_)                        initialization_ //060512
{  return Soil_base::set_infiltration_hydrology
         (infiltration_model, infiltration_submodel_, hydrology_);
}
//_initialize_______________________________________________________2015-07-24_/
bool Soil_components::reinitialize
(Moisture_characterization moisture                                              //170320
,soil_horizon_array32(H2O) // not const because initialized to avgWC when 0 values 140311
,bool beginning_of_simulation, bool subdivide_horizons)          initialization_
{  bool reinited = Soil_base::reinitialize
      (moisture,H2O, beginning_of_simulation, subdivide_horizons);               //170320
   Soil_temperature_profile_hourly_interface *temp_prof_hrly
      = const_cast<Soil_temperature_profile_hourly_interface *>                  //190131
      (provide_temperature_profile_hourly());                                    //150424
   if (temp_prof_hrly) reinited &= temp_prof_hrly->initialize();                 //131217
#ifdef CHEM_PROFILE
   if (chemicals)
      reinited &= chemicals->Soil::Chemicals_profile::reinitialize();
#endif
   return reinited;
}
//_reinitialize___________________________________________________________2006_/
#ifdef CHEM_PROFILE
bool Soil_components::reinitialize_for_crop
(float64 wilt_leaf_water_pot)                                    initialization_
{  bool reinited = Soil_base::reinitialize_for_crop(wilt_leaf_water_pot);
#ifdef CHEM_PROFILE
   if (chemicals)
      reinited &=chemicals->reinitialize_for_crop();
#endif
   return reinited;
}
//_reinitialize_for_crop_______________________________________________________/
#endif
#ifdef SALINITY
float64 Soil_components::get_salt_stress_function
(nat8 sublayer,float64 osmotic_pot_50, float64 salt_tolerance_P)           const
{ // This will be overloaded by the soil chemical profile class
   // When not simulating chemicals or salinity salt stress has no effect and this function is 1.0
   float32 result = 1.0;
   const Soil::Salt_interface *salinity = ref_salinity();                         //160412_060922
   result = salinity ? salinity->                                                //020527
   get_salt_stress_function(sublayer, osmotic_pot_50, salt_tolerance_P) : 1.0;
   return result;
}
//_get_salt_stress_function______________________________________________2002?_/
float64 Soil_components::get_osmotic_pot_solution(nat8 sublayer)           const
{ // This will be 0 when not running salinity
   const Soil::Salt_interface *salinity = ref_salinity();                         //160412_060922
   return salinity ? salinity->get_osmotic_pot_solution(sublayer) : 0.0;
}
//_get_osmotic_pot_solution_________________________________________2005-11-18_/
#endif
#ifdef SOIL_TEMPERATURE
float64 Soil_components::DeVries_thermal_conductivity
// J / (sec m C) DeVries
(float64 water_content_sl, float64 ice_content_sl, float64 bulk_density_sl)const
{  return temperature_profile                                                    //020527
   ? DeVries_thermal_conductivity(water_content_sl, ice_content_sl,
      bulk_density_sl) : 0.0; // need to check if something else could be passed
}
//_DeVries_thermal_conductivity_____________________________________1999-01-20_/
Soil::Temperature_interface *Soil_components::provide_temperature_profile()const
{  if (!temperature_profile)
      temperature_profile = new Soil::Temperature_profile
         (*parameters_ownable, *ref_layers(), *ref_hydrology()                   //160412
         ,*ref_structure()                                                       //070725
         ,meteorological);                                                       //151022
   return temperature_profile;
}
//_provide_temperature_profile______________________________________2004-07-14_/
Soil::Temperature_profile *Soil_components::relinquish_temperature_profile()
{  Soil::Temperature_profile *relinquished_temperature_profile = temperature_profile;
   temperature_profile = 0;
   return relinquished_temperature_profile;
}
//_relinquish_temperature_profile___________________________________2014-06-10_/
Soil::Temperature_profile_hourly *Soil_components::relinquish_temperature_profile_hourly()
{  Soil::Temperature_profile_hourly *relinquished_temperature_profile_hourly = temperature_profile_hourly;
   temperature_profile_hourly = 0;
   return relinquished_temperature_profile_hourly;
}
//_relinquish_temperature_profile_hourly____________________________2014-06-10_/
#endif
//______________________________________________________________________________
float64 Soil_components::get_temperature(nat8 sublayer)                    const
{
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   provide_temperature_profile();                                                //041207
   return temperature_profile                                                    //020527
   ? temperature_profile->get_temperature(sublayer) : -99999.99; // Just returning junk, if we get here then, soil_temperature is probably required, perhaps return air temperature
#else
   // use the surface air temperature if we arn't simulating temperature
   return -99999.99; // Just returning junk, if we get here then, soil_temperature is probably required, perhaps return air temperature
#endif
}
//_get_temperature_____________________________________________________________/
float64 Soil_components::get_surface_temperature()                         const
{  return
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   temperature_profile ? temperature_profile->get_surface_temperature() :
#endif
   0.0;
}
//_get_surface_temperature_____________________________________________________/
float64 Soil_components::get_temperature_C(nat8 sublayer)                  const
{  return
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
   temperature_profile_hourly ? temperature_profile_hourly->get_avg_temperature
      (sublayer) : temperature_profile ? temperature_profile->get_temperature
      (sublayer) :
#endif
   20.0;                                                                         //041111
   // we should always have one of the above,
   // but this is just incase. need to check default value  20.0
}
//_get_temperature_C___________________________________________________________/
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
modifiable_ Soil_temperature_profile_hourly_interface *Soil_components
::provide_temperature_profile_hourly()                                provision_
{ // This is for the soil_interface override
   if (!temperature_profile_hourly)                                              //150424
      temperature_profile_hourly =                                               //060922
         new Soil::Temperature_profile_hourly
         (*this
         ,meteorological                                                         //151022
         ,meteorological.ref_solar_radiation_extraterrestrial()                  //151201
         ,annual_temperature,simdate);                                           //150424_140724
   return temperature_profile_hourly;
}
#endif
//_Soil_temperature_profile_hourly_interface____________2006-09-22__2006-02-16_/
// NYI   virtual float64 get_liquid_water_content_m3_m3        (nat8  sublayer)  const = 0;

#ifdef NOT_YET_NEEDED
float64 Soil_components::set_NH4_ugN_gSoil(nat8 sublayer,float64 _NH4_mugN_gSoil)
{  float32 NH4_elemental_kg_m2 = micrograms_per_g_to_kg_per_m2
      (sublayer,_NH4_mugN_gSoil);
   return chemicals ? chemicals->set_NH4_from_elemental_N
      (sublayer,NH4_elemental_kg_m2) : 0.0;
}
//_set_NH4_ugN_gSoil___________________________________________________________/
float64 Soil_components::set_NO3_ugN_gSoil(nat8 sublayer,float64 NO3_mugN_gSoil_)
{  float32 NO3_elemental_kg_m2 = micrograms_per_g_to_kg_per_m2
      (sublayer,NO3_mugN_gSoil_);
   return chemicals ? chemicals->set_NO3_from_elemental_N(sublayer,
      NO3_elemental_kg_m2) : 0.0;
}
//_set_NO3_ugN_gSoil___________________________________________________________/
#endif
const Soil::Disturbance *Soil_components::get_disturbance()                const
{  return tillage_effect ? tillage_effect->get_disturbance() : 0;
} // May return 0 if not disturbance
//_get_disturbance__________________________________________________2006-08-07_/
RENDER_INSPECTORS_DEFINITION(CropSyst::Soil_components)
{
   #ifdef CS_OBSERVATION
   emanator_known
      = Soil_base::render_inspectors(inspectors,context,instance);
   if (chemicals) chemicals->render_inspectors(inspectors,emanator_known,"chemical");
   //NYI if (erosion) erosion->render_inspectors(inspectors,&emanator,"erosion");
   //NYI if (tillage_effect) tillage_effect->render_inspectors(inspectors,&emanator,"tillage_effect");
   return emanator_known; // emanator;
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-11_/
}//_namespace CropSyst_________________________________________________________/

