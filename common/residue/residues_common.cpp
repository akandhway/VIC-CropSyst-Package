
#include "common/residue/residues_common.h"

#include "soil/soil_I.h"
#include "corn/math/compare.hpp"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif
#include "common/physics/water_depth.h"

// This is the base class for
// Residue_pools_common in V4.1, V4.2, V4.3
// and Residues_V4_4 in V4.4

#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"


#define water_density              1000.0 /* Kg/m3 */
//______________________________________________________________________________
Residues_common::Residues_common
(Soil::Soil_interface                     &soil_
,modifiable_ CORN::Dynamic_array<float64> &evaporation_potential_remaining_      //160717
,const Air_temperature_average      &temperature_air_avg_                        //150427
,const Wind_speed                   &wind_speed_)                                //150427
: Residues_interface()
, CS::Evaporator_intervals       (evaporation_potential_remaining_)              //160717 160523
,soil                            (soil_)                                         //060122
//move soil layers here
,temperature_air_avg             (temperature_air_avg_)                          //150427
,wind_speed                      (wind_speed_)                                   //150427
,curr_water_hold                 (0.0)
,water_storage                   (0.0)                                           //170104_160627
,report_ground_cover             (0.0)                                           //160627
#ifdef RESIDUE_WC_FIX
090302 RLN Dont delete. This should be correct
,water_from_change_in_biomass    (0.0)                                           //090224
#endif
,interception                    (0.0)
,shallow_horizon_thickness       (0.0)                                           //060322
,deep_horizon_thickness          (0.0)                                           //060322
,stubble_contact_fraction        (DEFAULT_STANDING_STUBBLE_CONTACT_FRACTION)
,flat_surface_contact_fraction   (0.3) //190327 (0.0)
   // flat_surface_contact_fraction should actually be updated daily
,manure_surface_contact_fraction (DEFAULT_SURFACE_MANURE_CONTACT_FRACTION)       //090225
,subsurface_contact_fraction     (1.0)
,soil_layers                     (*soil_.ref_layers())
,fract_canopy_cover              (0)                                             //160523
{  identify_residue_horizons();                                                  //981001
   clear_totals();                                                               //060228
}
//_Residues_common:constructor______________________________________2002-03-07_/
void Residues_common::identify_residue_horizons()
{
   // This is usually only called by the constructor,
   // but could be called when the soil profile is changed.
   // I.e. in the event of soil erosion.
   static const float64 SHALLOW_DEPTH  = 0.1500001;
   static const float64 DEEP_DEPTH     = 0.3000001;
   shallow_horizon_thickness = 0;                                                //990217
   deep_horizon_thickness = 0;                                                   //990217
   horizons[0] = SURFACE_HORIZON;                                                //011021
   const Soil::Layers_interface *soil_layers = soil.ref_layers();                //060122
   for (nat8 sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)           //9702??
   if (sublayer > soil_layers->count())                                          //991206
       horizons[sublayer] = NULL_HORIZON;                                        //991206
   else                                                                          //991206
   {  float64 soil_depth = soil_layers->get_depth_m(sublayer);                   //040520
      float64 soil_thickness =  soil_layers->get_thickness_m(sublayer);          //040520
      if (CORN::is_approximately<float64>(soil_depth,0.0,0.0000001))             //9702??
        horizons[sublayer] = UNREACHABLE_HORIZON;                                //9702??
      else if ((soil_depth <= SHALLOW_DEPTH) || (sublayer == 1))                 //000323
      {  horizons[sublayer] = SHALLOW_HORIZON;                                   //9702??
         shallow_horizon_thickness += soil_thickness;                            //990217
      } else if (soil_depth <= DEEP_DEPTH)                                       //9702??
      {  horizons[sublayer] = DEEP_HORIZON;                                      //9702??
         deep_horizon_thickness += soil_thickness;                               //990217
      } else                                                                     //9702??
         horizons[sublayer] = UNREACHABLE_HORIZON;                               //9702??
   }
}
//_identify_residue_horizons________________________________________1998-10-01_/
bool Residues_common::clear_totals()
{  total_residue_area_index_both_flat_and_standing = 0;
   for (int i = 0; i < OM_POSITION_COUNT; i++)
   {  total_residue_area_index[i] = 0.0;
      total_fract_ground_cover[i] = 0.0;
   }
   return true;
}
//_clear_totals_____________________________________________________2006-02-28_/
bool Residues_common::clear_all()                                  modification_
{
   stubble_contact_fraction = 0.0;
   flat_surface_contact_fraction = 0.3;                                          //190327
   subsurface_contact_fraction = 1.0;
   manure_surface_contact_fraction = 1.0; //0.0;
   total_residue_area_index_both_flat_and_standing = 0.0;
   evaporation_potential.clear();                                                //160719
   evaporation_actual.clear();                                                   //160719
   curr_water_hold =    0.0;
   water_storage =      0.0;                                                     //170104
#ifdef RESIDUE_WC_FIX
090302 RLN Dont delete. This should be correct
   water_from_change_in_biomass = 0;
                  // m When residue is redistributed/removed by tillage/residue operation
                           // we need to account for the corresponding moisture loss
#endif
   interception = 0.0;
   return clear_totals();
}
//_clear_all________________________________________________________2016-01-26_/
void Residues_common::update_totals_all()
{
   clear_totals();
#ifdef NYI
These totals will be done in a different way 011005
   total_surface_biomatter_ref.clear();
   total_subsurface_biomatter_ref.clear();
   total_all_biomatter_ref.clear();
#endif
   update_totals_sum_pools();
   float64 sum_residue_area_index_flat_surface    = total_residue_area_index[OM_flat_surface_position]; //131218
   float64 sum_residue_area_index_stubble_surface = total_residue_area_index[OM_stubble_surface_position]; //131218_131205
   float64 mass_flat_surface                 = get_surface_plant_biomass(INCLUDE_POSITION_FLAT_SURFACE); //170107
   float64 mass_stubble_surface              = get_surface_plant_biomass(INCLUDE_POSITION_STUBBLE_SURFACE); //170107
   float64 area_to_mass_ratio_flat_surface                                       //170107
      = (mass_flat_surface > 0.000001)
         ? sum_residue_area_index_flat_surface / mass_flat_surface : 0.0;
   float64 area_to_mass_ratio_stubble_surface                                    //170107
      = (mass_stubble_surface > 0.000001)
         ? sum_residue_area_index_stubble_surface  / mass_stubble_surface : 0.0;
   float64 area_index_flat = area_to_mass_ratio_flat_surface * mass_flat_surface; // should be the same as sum_residue_area_index_flat_surface
   float64 area_index_stubble = area_to_mass_ratio_stubble_surface * mass_stubble_surface; // sould be the same as sum_residue_area_index_stubble_surface
   // area_to_mass_ratio_flat_surface should be about 0-4
   // area_to_mass_ratio_stubble_surface should be about 0-2
   float64 total_ground_cover_flat_surface
      = 1.0 - exp(-0.6 * area_index_flat);                                       //131205
   total_fract_ground_cover[OM_flat_surface_position] = total_ground_cover_flat_surface;
   total_fract_ground_cover[OM_stubble_surface_position]
      = 1.0 - exp(-0.2 * area_index_stubble);
   flat_surface_contact_fraction =
      (sum_residue_area_index_flat_surface > 0.00000001) // avoid div zero when none
      ? CORN::must_be_less_or_equal_to<float64>
         (pow(total_ground_cover_flat_surface,0.75)
          /sum_residue_area_index_flat_surface,1.0)
      : DEFAULT_SURFACE_ORGANIC_RESIDUE_CONTACT_FRACTION;                        //090409

   //The larger the conver the less the contact because residues are piling on top of each other
   flat_surface_contact_fraction = 0.3
      + 0.7 * exp(-2.0 * total_fract_ground_cover[OM_flat_surface_position]);
   // currently manure has a constant contact fraction                           //090225
}
//_update_totals_all________________________________________________2006-02-28_/
float64 Residues_common::water_interception
(float64 water_entering_residues /*m*/)
{  float64 water_leaving_residues = 0.0; // m
   float64 water_retained_in_residue_m = 0.0;
   //070118    Note that in the V.B. version there is separate interception
   //          for standing and flat, this will eventually be implemented here.
   float64 total_surface_biomass = get_total_surface_biomass(); // kg/m2
   if (water_entering_residues > 0.0)
   {
      {
#ifdef RESIDUE_WC_FIX
090302 RLN Dont delete. This should be correct
            if (total_water_hold > MAX_RESIDUE_HOLD_m3_kg);
            {  // This is a special case that may occur with a tillage operations
               // residue pools may be redistributed, but the water
               // is not causing an inconsistency in holding capacity.
               // This hack simply sends any excess water to water_leaving_residues
               // (since this goes into the soil anyway).
               water_from_change_in_biomass += total_water_hold - MAX_RESIDUE_HOLD_m3_kg;
               total_water_hold = MAX_RESIDUE_HOLD_m3_kg;
            }
#endif
         float64 maximum_storage_m = (MAX_RESIDUE_HOLD_m3_kg * total_surface_biomass );
         float64 maximum_retention_m =  maximum_storage_m - water_storage;
         maximum_retention_m = CORN::must_be_0_or_greater<float64>(maximum_retention_m);
         if (water_entering_residues >= maximum_retention_m)
         {  water_retained_in_residue_m = maximum_retention_m;    // m The water going in and staying there
            curr_water_hold  = MAX_RESIDUE_HOLD_m3_kg;
            water_leaving_residues = water_entering_residues - water_retained_in_residue_m;
            water_storage += water_retained_in_residue_m;
         } else
         {  // Residue can take everything
            water_retained_in_residue_m = water_entering_residues;               //170104
            water_storage += water_entering_residues;                            //170104
            curr_water_hold = water_storage / total_surface_biomass;             //170104
               // m3/kg = m / kg/m2
            water_leaving_residues = 0.0; // none leaving(can hold all entering) //090311
         }
      }
   }
   interception = water_retained_in_residue_m;
   return water_retained_in_residue_m;
}
//_water_interception__________________________________________________________/
float64 Residues_common::take_pool_water(float64 pool_water)      appropriation_
{
   water_storage += pool_water;
   return water_storage;
   // Warning probably need to accomodate water balance!
}
//_take_pool_water__________________________________________________2017-01-04_/
float64 Residues_common::calc_evaporation_potential(nat8 interval)  calculation_
{
   float64 fract_residue_cover = get_total_fract_ground_cover
      (INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE);         //090312
   float64 evap_pot
      = fract_residue_cover
      * evaporation_potential_remaining.get(interval);
   return evap_pot;
}
//_calc_evaporation_potential_______________________________________2016-06-03_/
bool Residues_common::start_day()                                  modification_
{  // Don't try to delete because this needed for multiple inheritence
   return CS::Evaporator_intervals::start_day();
}
//_start_day________________________________________________________2016-05-23_/
bool Residues_common::end_day()                                    modification_
{  bool ended = true;
   report_ground_cover = get_total_fract_ground_cover
      (INCLUDE_POSITION_FLAT_SURFACE| INCLUDE_POSITION_STUBBLE_SURFACE);
   report_surface_biomass = get_surface_all_biomass();

   // Continue here
   //for (nat8 plant_pos = 0; plant_pos < OM_POSITION_COUNT; plant_pos++)
   report_plant[OM_subsurface_position]
   = get_biomass_output(INCLUDE_POSITION_SUBSURFACE      ,INCLUDE_STRAW_RESIDUE_OM);
   report_plant[OM_flat_surface_position]
   = get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE    ,INCLUDE_STRAW_RESIDUE_OM);
   report_plant[OM_stubble_surface_position]
   = get_biomass_output(INCLUDE_POSITION_STUBBLE_SURFACE ,INCLUDE_STRAW_RESIDUE_OM);
   /*This has not been output before
   report_plant[OM_attached_to_plant_position]
   = get_biomass_output(OM_attached_to_plant_position,INCLUDE_STRAW_RESIDUE_OM);
   */

   report_plant_surface // report_surface_biomass report_plant_surface
      = report_plant[OM_flat_surface_position]
       +report_plant[OM_stubble_surface_position];

   //for (nat8 manure_pos = 0; manure_pos < MANURE_POSITION_COUNT; manure_pos);
   report_manure[OM_subsurface_position]
   =get_biomass_output(INCLUDE_POSITION_SUBSURFACE  ,INCLUDE_MANURE_RESIDUE_OM);
   report_manure[OM_flat_surface_position]
   =get_biomass_output(INCLUDE_POSITION_FLAT_SURFACE,INCLUDE_MANURE_RESIDUE_OM);

   ended &= CS::Evaporator_intervals::end_day();
   return ended;
}
//_end_day__________________________________________________________2016-06-27_/
float64 Residues_common::evaporate_interval(nat8 interval)         modification_
{
   float64 total_surface_mass = get_total_surface_biomass(); // kg/m2
   float64 evap_act = 0.0;
   float64 evap_pot = calc_evaporation_potential(interval);
   evaporation_potential_remaining.deduct_at(interval,evap_pot);                 //200225
   if (water_storage == 0.0)
   {
      evap_act = 0.0;
      //moved above evaporation_potential_remaining.deduct_at(interval,evap_pot);              //200225
   }
   else
   {  //200225 float64 evap_pot = calc_evaporation_potential(interval);
      evap_act = std::min<float64>(water_storage,evap_pot);                      //170104
      //moved above evaporation_potential_remaining.deduct_at(interval,evap_pot);              //160103
      water_storage -= evap_act;                                                 //170104
      curr_water_hold =
         CORN::is_approximately<float64>(total_surface_mass,0.0)                 //181105
         ? 0 :                                                                   //181105
         water_storage / total_surface_mass;                                     //170104
      //200225 evaporation_potential.set(interval,evap_pot);
      //200225 evaporation_actual   .set(interval,evap_act);
   }
   evaporation_potential.set(interval,evap_pot);                                 //200225
   evaporation_actual   .set(interval,evap_act);                                 //200225
   return evap_act;
}
//_evaporate_interval_______________________________________________2016-07-29_/
float64 Residues_common::evaporate_day()  // m                                   //010910
{  bool evaporated = true;
   if (is_daily_enabled())
      evaporate_interval(0);                                                     //160719
   return evaporated;
}
//_______________________________________________________________evaporate_day_/
float64 Residues_common::evaporate_hour(CORN::Hour hour)           modification_
{  return CS::Evaporator_intervals::evaporate_hour(hour);
}
//_evaporate_hour___________________________________________________2016-06-03_/
float64 Residues_common::get_total_fract_ground_cover(nat32 include_position) const
{  float64 total = 0;
   float64 product = 1.0;                                                        //131217
   for (nat8 b = 0; b < OM_POSITION_COUNT; b++)
   {  nat8 mask = (nat8)(1 << b);
      if (mask & include_position)
         product *= (1.0 - total_fract_ground_cover[b]);
   }
   total = 1.0 - product;
   if (total > 1.0) total = 1.0;
   // Note that it can't really get to 1.0 so this should never happen
   return total;
}
//_get_total_fract_ground_cover_____________________________________2006-03-28_/
float64 Residues_common::get_total_area_index
(nat32 include_position)                                                   const
{  // include_position specifies which residues (surface, standing, or both) that we are intested in.
   // Note that it is intended for the caller to request the value
   // for only one position
   // it is non-sensical to attempt to sum the values
   // for both pools, that would require some other (multiplicative>) aggregation
   // similar to what is done for get_total_fract_ground_cover.
   float32 total = 0;
   for (nat8 b = 0; b < OM_POSITION_COUNT; b++)
   {  nat8 mask = (nat8)(1 << b);
      if (mask & include_position)
         total += total_residue_area_index[b];
   }
if (total > 1.0)
total = 1.0;
   return total;
}
//_get_total_area_index_____________________________________________2004-11-08_/
extern float64 friction_velocity
(float64 momentum_roughness_param   // m
,float64 zero_plane_displacement    // m
,float64 windspeed_m_s              // m/s
,float64 screening_height           // m
);
//_friction_velocity___________________________________________________________/
#define specific_density 170.0
float64 Residues_common::calc_surface_windspeed
(float64 depth)                                                     calculation_
{
   #define von_karman_const 0.4
   float64 momentum_roughness_param = 0.1 * depth; // m
   float64 zero_plane_displacement = 0.65 * depth; // m
   float64 surface_WS =
            friction_velocity
            (momentum_roughness_param
            ,zero_plane_displacement
            ,wind_speed.get_m_s()
            ,wind_speed.get_reference_height_m()
            ) / von_karman_const
              * CORN_ln((depth - zero_plane_displacement)
                        / momentum_roughness_param);
   return surface_WS;
}
//_calc_surface_windspeed___________________________________________1999-01-30_/
float64 Residues_common::calc_wind_attenuation_coef
(float64 bulk_density)                                              calculation_
{  float64 wind_atten_coef = CORN::must_be_greater_or_equal_to<float64>
         (0.1 * (bulk_density - 20.0),0.0);
   return wind_atten_coef;
}
//_calc_wind_attenuation_coef__________________________________________________/
float64 Residues_common::calc_wind_speed_within_residue
( float64 bulk_density
, float64 depth)                                                    calculation_
{  float64 WS_withing_residue = calc_surface_windspeed(depth)  *
      exp(calc_wind_attenuation_coef(bulk_density)*(((depth/2.0)/depth) -1.0));
   return WS_withing_residue;
}
//_calc_wind_speed_within_residue______________________________________________/
float64 Residues_common::calc_convective_heat_transfer_coef
( float64 bulk_density
, float64 depth
, bool    snow_or_residue_cover)                                    calculation_
{  const float64 still_air_thermal_conductivity = 0.025; //  J/(m s C )
   float64 conv_heat_trans_coef = (snow_or_residue_cover)
   ? 0.0
   : still_air_thermal_conductivity
             * (1.0 + 0.007 * temperature_air_avg.Celcius())
             * (1.0 + 4.0 * calc_wind_speed_within_residue(bulk_density,depth))
             * (1.0 - bulk_density / specific_density);
   return  conv_heat_trans_coef;
}
//_calc_convective_heat_transfer_coef__________________________________________/
float64 Residues_common::calc_thermal_conductivity_conduction
(float64 bulk_density)                                              calculation_
{  const float64 solid_thermal_conductivity = 0.05;   // J/(m s C) Thermal conductivity of residue given no spaces
   const float64 thermal_conductivity_water = 0.5811; // J/(m s C)
   float64 thermal_conduct_cond =
         solid_thermal_conductivity * (bulk_density / specific_density)
       + thermal_conductivity_water
         * curr_water_hold // m3/kg
         * water_density
         * (bulk_density/water_density);
   return   thermal_conduct_cond;
}
//_calc_thermal_conductivity_conduction________________________________________/
float64 Residues_common::calc_thermal_conductance
(bool    snow_or_residue_cover)                                     calculation_
{  // { kg/m3 }
   float64 depth = get_depth();
   float64 therm_cond = 99999.99;
   if (depth > 0.0)
   {  float64 bulk_density =  get_total_surface_biomass() / depth;   // kg/m3
      therm_cond =
      (calc_convective_heat_transfer_coef
          (bulk_density, depth, snow_or_residue_cover)
       + calc_thermal_conductivity_conduction(bulk_density)) / depth;
   }
   return therm_cond;
}
//_calc_thermal_conductance_________________________________________1999-01-28_/
bool Residues_common::has_such_horizon(Residue_horizons search_for_horizon) affirmation_
{  bool result = true;
   for (nat8 sublayer = 1; sublayer <= MAX_soil_sublayers; sublayer++)
      if (horizons[sublayer] == search_for_horizon )
             result = false;
   return result;
}
//_has_such_horizon_________________________________________________1998-10-18_/
RENDER_INSPECTORS_DEFINITION(Residues_common)
{
   #ifdef CS_OBSERVATION
   CS::Evaporator_intervals::render_inspectors(inspectors,context,"residue");
      // the evaporation/actual and evaporation/potential
      // CSV were  CSVP_residue_pot_evap  CSVP_residue_act_evap

   // Residue
   inspectors.append(new CS::Inspector_scalar(report_ground_cover     ,UC_fraction   ,*context,"residue:ground_cover"         ,sum_statistic /*value_statistic */,CSVP_residue_ground_cover));
   inspectors.append(new CS::Inspector_scalar(water_storage           ,UC_m          ,*context,"residue:water_storage"        ,sum_statistic ,CSVP_residue_water_storage));
   inspectors.append(new CS::Inspector_scalar(report_surface_biomass  ,UC_kg_m2      ,*context,"residue:mass&total&surface"   ,sum_statistic ,CSVP_residue_surface_biomass));
   inspectors.append(new CS::Inspector_scalar(interception            ,UC_m          ,*context,"residue:interception"         ,value_statistic /*mabe sum_statistic*/ ,CSVP_crop_base_water_intrcpt/*171222 CSVC_residue_water_intrcpt_time_step_sum*/));

   // Plant
   inspectors.append(new CS::Inspector_scalar(report_plant_surface                     ,UC_kg_m2   ,*context,"residue:mass&plant&surface"   ,sum_statistic,CSVP_residue_plant_surface));
   inspectors.append(new CS::Inspector_scalar(report_plant[OM_flat_surface_position]   ,UC_kg_m2   ,*context,"residue:mass&plant&flat"      ,sum_statistic,CSVP_residue_plant_flat));
   inspectors.append(new CS::Inspector_scalar(report_plant[OM_stubble_surface_position],UC_kg_m2   ,*context,"residue:mass&plant&stubble"   ,sum_statistic,CSVP_residue_plant_stubble));
   inspectors.append(new CS::Inspector_scalar(report_plant[OM_subsurface_position]     ,UC_kg_m2   ,*context,"residue:mass&plant&subsurface",sum_statistic,CSVP_residue_plant_incorporated));
   // This is outputting total There is actually a value for each layer

   // Manure
   //NYI inspectors.append(new CS::Inspector_scalar(member_variable                          ,UC_kg_m2   ,*context,"residue:manure&mineralization" ,sum_statistic,CSVP_residue_manure_mineralized));
   inspectors.append(new CS::Inspector_scalar(report_manure[OM_flat_surface_position]  ,UC_kg_m2   ,*context,"residue:mass&manure&surface"        ,sum_statistic,CSVP_residue_manure_surface));
   inspectors.append(new CS::Inspector_scalar(report_manure[OM_subsurface_position]    ,UC_kg_m2   ,*context,"residue:mass&manure&subsurface"     ,sum_statistic,CSVP_residue_manure_incorporated));
   // This is outputting total There is actually a value for each layer
   #endif
   return 0;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2016-06-27_/

