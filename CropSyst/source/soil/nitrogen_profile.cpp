#include "corn/math/compare.h"
#include "soil/nitrogen_profile.h"
#include "soil/soil_I.h"
#include "soil/abiotic_environment.h"
#include "soil/hydrology_I.h"
#include "soil/texture_I.h"
#include "soil/structure_I.h"
#include "soil/chemical_mass_profile.h"
#include "common/physics/standard_constants.h"
#include "soil/chemical_balance.h"
#include "static_phrases.h"
#include <math.h>
#include "corn/math/moremath.h"
#include "corn/measure/measures.h"
#include "corn/math/compare.hpp"
#include "CS_suite/observation/CS_inspector.h"
#include "csvc.h"

//#define DETAIL_DENITR
#include <assert.h>
#ifdef DETAIL_DENITR
#include <fstream>
extern std::ofstream *denitr_detail_stream; // optional
#endif
//______________________________________________________________________________
namespace Soil {                                                                 //181206
#if (CROPSYST_VERSION==4)
float64 NO3_Profile::transformation_rate
(float64 soil_temperature
,float64 water_content_correct)
{  static const float64 DENITRIFY_RATE_15  = 0.005; // was 0.01                  //000515
   float64 temperature_correction = (soil_temperature>= 10.0)
      ? exp(0.08 * (soil_temperature - 15))
      : (0.67 * exp(0.43 * (soil_temperature - 10.0)));                          //070827 WARNING this is a problem with very low temperatures
   float64 transform_rate = control_transformation_adjustment *                  //070813
        (DENITRIFY_RATE_15 * water_content_correct * temperature_correction);
   return CORN::must_be_0_or_greater<float64>(transform_rate);
}
//_transformation_rate_____________________________________________________________________________
#endif
float64 NO3_Profile::get_N_mass(nat8 layer) const
{  return   molecular_to_elemental * mass_M->get_layer(layer);
}
//_get_N_mass_______________________________________________________2006-05-03_/
float64 NH4_Profile::get_N_mass(nat8 layer) const
{  return   molecular_to_elemental * mass_M->get_layer(layer);
}
//_get_N_mass_______________________________________________________2006-05-03_/
float64 NH4_Profile::moisture_function
(  float64 water_filled_porosity
,  float64 function_value_at_saturation)
{  float64 fract_saturation = CORN::must_be_between<float64>
      (water_filled_porosity,0.0,1.0);
   return (fract_saturation < 0.9)
   ?  (1.111 * fract_saturation)
   :  (10.0 - 10.0 * fract_saturation);
}
//_moisture_function______________________________________________________1998_/
#if (CROPSYST_VERSION==4)
float64 NH4_Profile::transformation_rate
(float64 soil_temperature
,float64 )//unused  water_content_correct  // unused parameter given for virtual compatibility
{  static const float64 NITRIFY_RATE_35  = 0.8; // was 3.0
   float64 adjustable_nitrification_rate = (soil_temperature < 10.0)
   ? (0.0105 * soil_temperature + 0.00095 * CORN_sqr(soil_temperature))
   : (soil_temperature < 35.0)
      ? (0.032 * soil_temperature - 0.12)
      : ( -0.1 * soil_temperature + 4.5);
   return CORN::must_be_0_or_greater<float64>
         (control_transformation_adjustment
          * (NITRIFY_RATE_35 * adjustable_nitrification_rate));
}
//_transformation_rate_____________________________________________________________________________
#endif
NO3_Profile::NO3_Profile
(const CORN::date32                 &today_                                      //170523
#if (CROPSYST_VERSION==4)
,float64                control_transformation_adjustment_
#endif
,soil_layer_array64     (simulation_amount_E_)                                   //051120
/*NYN
#ifndef OLD_N_XFER
,Chemical_pool          *receiver_emmission_                                     //170502
#endif
*/
,const Layers_interface               &soil_layers_                              //150925
,const Hydrology_interface            &soil_hydrology_                           //150925
,const Hydraulic_properties_interface &soil_hydraulic_properties_                //150925
,const Structure                      &soil_structure_                           //150925
,Infiltration_model      infiltration_model_                                     //080117
,Water_table_curve      *water_table_curve_)                                     //000504
:Chemical_uptake_profile(today_,std::string(LABEL_NO3)
   ,CT_NO3
   ,NO3_to_N_conv
   ,N_to_NO3_conv
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment
   #endif
   ,simulation_amount_E_
   #ifndef OLD_N_XFER
   ,0 // NYN receiver_emmission_                                                 //170502
   #endif
   ,soil_layers_
   ,soil_hydrology_
   ,soil_hydraulic_properties_
   ,soil_structure_
   ,infiltration_model_                                                          //080117
   ,water_table_curve_)                                                          //000504
//#ifndef OLD_N_XFER
, denitrified_N_output(transformed_E_output)                                     //170502
//#endif
{}
//_NO3_Profile:constructor______________________________________________________
NH4_Profile::NH4_Profile
(const CORN::date32                       &simdate_                              //170523
#if (CROPSYST_VERSION==4)
,float64                 control_transformation_adjustment_
#endif
,soil_layer_array64     (simulation_amount_E_)                                   //051120
#ifndef OLD_N_XFER
,Chemical_pool                            &receiver_NO3_                         //170502
#endif
,const Layers_interface               &soil_layers_                              //150925
,const Hydrology_interface            &soil_hydrology_                           //150925
,const Hydraulic_properties_interface &soil_hydraulic_properties_                //150925
,const Structure                      &soil_structure_                           //150925
,Infiltration_model        infiltration_model_                                   //080117
,Water_table_curve        *NH4_water_table_curve) // may be 0 if no water table consideration
:Chemical_uptake_profile
   (simdate_
   ,std::string(LABEL_NH4)
   ,CT_NH4
   ,NH4_to_N_conv, N_to_NH4_conv
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment_
   #endif
   ,simulation_amount_E_
   #ifndef OLD_N_XFER
   ,&receiver_NO3_                                                               //170502
   #endif
   ,soil_layers_
   ,soil_hydrology_
   ,soil_hydraulic_properties_
   ,soil_structure_
   ,infiltration_model_                                                          //080117
#ifdef WATERTABLE
   ,NH4_water_table_curve                                                        //080213
#else
   ,0                                                                            //090818
#endif
)
#ifndef OLD_N_XFER
, nitrified_N_output(transformed_E_output)                                       //170502
#endif
{ }
//_NH4_Profile:constructor______________________________________________________
NH4_Profile_with_pH::NH4_Profile_with_pH
   (const CORN::date32                 &simdate_                                 //170523
   #if (CROPSYST_VERSION==4)
   ,float64                            control_transformation_adjustment_V4_1_
   #endif
   ,soil_layer_array64                 (simulation_amount_E_)
//   #ifndef OLD_N_XFER
//   ,Chemical_pool                             &receiver_NO3_                     //170502
//   #endif
   ,const Layers_interface               &soil_layers_                           //150925
   ,const Hydrology_interface            &soil_hydrology_                        //150925
   ,const Hydraulic_properties_interface &soil_hydraulic_properties_             //150925
   ,const Structure                      &soil_structure_                        //150925
   ,Infiltration_model                 infiltration_model_                       //080117
   ,Soil::Abiotic_environment_profile   *abiotic_environment_profile_
   ,NO3_Profile                        &NO3_
   ,soil_layer_array64                 (pH)
   ,Water_table_curve                *NH4_water_table_curve_optional
   ,float64                            pH_min
   ,float64                            pH_max)
: NH4_Profile(simdate_
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment_V4_1_
   #endif
   ,simulation_amount_E_
   #ifndef OLD_N_XFER
   ,NO3_ // receiver_NO3_                                                        //170502
   #endif
   ,soil_layers_
   ,soil_hydrology_
   ,soil_hydraulic_properties_
   ,soil_structure_
   ,infiltration_model_
   ,NH4_water_table_curve_optional)
,NO3(NO3_)
,abiotic_environment_profile(abiotic_environment_profile_)
, N2O_N_loss_daily(0.0)                                                          //090722
{
   clear_layer_array64(pH_function);
   for (nat8 layer = 1; layer <= soil_layers.count(); layer++)
      pH_function[layer] = (pH[layer] - pH_min) / (pH_max - pH_min);
   for (nat8 layer = soil_layers.count()+1; layer <= soil_layers.count_max(); layer++)
      pH_function[layer] = 1.0;
// WARNING THIS SHOULD BE LIMITED TO 0 (at least)
}
//_NH4_Profile_with_pH:constructor__________________________________2001-05-10_/
float64 NO3_Profile::water_content_correction
(float64 //unused sublayer_saturation_water_content
,float64 sublayer_water_content
,float64 //unused sublayer_reference_water_content // unused (parameter given for virtual compatibility)
,float64 sublayer_bulk_density)
{  float64 fract_saturation = sublayer_water_content / (1.0 - sublayer_bulk_density /2.65);
   fract_saturation = CORN::must_be_between<float64>(fract_saturation ,0.0,1.0);
   return pow(fract_saturation,10.0);
}
//_water_content_correction_________________________________________2000-05-15_/
NO3_with_CO2_Profile::NO3_with_CO2_Profile
(const CORN::date32                 &today_                                      //170523
,const CropSyst::Denitrification_parameters &denitrification_parameters_         //150824
#if (CROPSYST_VERSION==4)
,float64              control_transformation_adjustment_
#endif
,soil_layer_array64  (simulation_amount_E_) // const                             //051120
#ifndef OLD_N_XFER
//NYN ,Chemical_pool                             &receiver_emmissions            //170502
#endif
,const Layers_interface               &soil_layers_                              //150925
,const Hydrology_interface            &soil_hydrology_                           //150925
,const Hydraulic_properties_interface &soil_hydraulic_properties_                //150925
,const Structure                      &soil_structure_                           //150925
,Infiltration_model   infiltration_model_                                        //080117
,const float64          &ref_water_flux_in_m_                                    //150925
,Soil::Abiotic_environment_profile *abiotic_environment_profile_                 //060510
,Water_table_curve   *water_table_curve_)                                        //000504
:NO3_Profile
   (today_
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment_
   #endif
   ,simulation_amount_E_ // const                                                //051120
   #ifndef OLD_N_XFER
   //NYN ,&receiver_emmissions                                                   //170502
   #endif
   ,soil_layers_
   ,soil_hydrology_
   ,soil_hydraulic_properties_
   ,soil_structure_
   ,infiltration_model_                                                          //080117
   ,water_table_curve_)                                                          //000504
, denitrification_parameters(denitrification_parameters_)                        //150824
, N2O_N_loss_daily(0.0)                                                          //060607
, abiotic_environment_profile(abiotic_environment_profile_)                      //060510
, CO2_C_loss_subsurface(0)                                                       //110920
, infiltration_model(infiltration_model_)
, ref_water_flux_in_m(ref_water_flux_in_m_)                                      //150925
{  for (uint8 lyr = 0; lyr <= MAX_soil_layers; lyr++)
   {  denitrification_event            [lyr] = false;
      denitrification_event_second_day [lyr] = false;
   }
   // potential_denitrification_constant is usually about 1mgN/kgSoil/day
   // Adjustment and power factor needs to be done once for the number of the intervals run (once for the simulation if intervals dont change)

//std::cout << std::endl << "Warning: debugging denitrification. nitrate and moisture functions have been disabled"<< std::endl << std::endl;

#ifdef DETAIL_DENITR
if (denitr_detail_stream)
(*denitr_detail_stream) << "date" << '\t'
 <<  "layer" << '\t'
 << "interval(hour)" <<  '\t'
 << "N_mass kg/ha" <<  '\t'
 << "N_denitr_interval kg/m2" << '\t'
 << "N2O-N_from_denitr" << '\t'  //
 << "CO2_C_loss (per_unit_soil_mass_mgC_kgsoil_daily)"  << '\t'
 << "moisture_funct" << '\t'
 << "nitrate_func mg/kg/hr" << '\t'
 << "resp_func mg/kg/hr" << '\t'
 << "N2ToN2O_moist_funct" << '\t'
 << "N2ToN2O_nitrate_funct" << '\t'
 << "N2ToN20_resp_function" << '\t'
 << "N2O_fraction_of_denitrification" << '\t'
 << "denitrified_daily_layer" <<std::endl;
// << "N2_to_N20_ratio" << '\t'
#endif
}
//_NO3_with_CO2_Profile:constructor____________________________________________/
void NO3_with_CO2_Profile::setup_for_transformations()
{  N2O_N_loss_daily = 0.0;                                                       //060608
   if (infiltration_model == CASCADE_DAILY_INFILTRATION)
   {   // Because cascade model is only daily (Claudio may add an hourly model (I.e. Darcian).
      //150925  moved to infilration and now referenced float64 water_flux_in_m = soil.get_act_water_entering_soil();              //060608
      float64  denitrification_water_influx_mm = m_to_mm
         (ref_water_flux_in_m);                                                  //150925
      for (uint8 lyr = 1; lyr <= soil_layers.count(); lyr++)
      {  float64 water_content_lyr     = soil_hydrology.get_liquid_water_content_volumetric(lyr); // 080813 RLN warning presuming liquid only
         bool &denitrification_event_lyr = denitrification_event[lyr];
         bool &denitrification_event_second_day_lyr = denitrification_event_second_day[lyr];
         if (denitrification_water_influx_mm > 0)
         {  float64 sat_WC_lyr = soil_hydraulic_properties.get_saturation_water_content_volumetric
               (lyr,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
            float64 FC_lyr     = soil_hydraulic_properties.get_field_capacity_volumetric(lyr);
            float64 thickness_lyr = soil_layers.get_thickness_m(lyr);
            float64 near_air_filled_porosity_mm = ((0.75 * sat_WC_lyr + 0.25 * FC_lyr) - water_content_lyr) * thickness_lyr * water_density;
            // Doesn't need to be fully saturated for the condition:
            // Somewhere in between saturation and FC.
            if (denitrification_water_influx_mm < near_air_filled_porosity_mm )
            {  denitrification_event_lyr = false;
               denitrification_water_influx_mm = 0.0;
            } else
            {  denitrification_event_lyr = true;
               denitrification_event_second_day_lyr = false;
               denitrification_water_influx_mm = denitrification_water_influx_mm - near_air_filled_porosity_mm;
               if (denitrification_water_influx_mm < 0)  denitrification_water_influx_mm = 0;
            }
         } else
            denitrification_event_second_day_lyr  = false;
      }
   }
}
//_setup_for_transformations___________________________________________________/
float64 NO3_with_CO2_Profile::moisture_function_daily(uint8 layer,bool for_N2_N2O_ratio) const // was moisture_function_for_cascade_infiltration
{  float64 moisture_funct = 0.0;
   //std::cout<<"denitrification_event is disabled!!!\n";
   if (denitrification_event[layer])
      moisture_funct = for_N2_N2O_ratio ?  0.5 :  0.6;
   return moisture_funct;
}
//_moisture_function_daily__________________________________________2008-08-29_/
float64 NO3_with_CO2_Profile::moisture_function_hourly(nat8 layer,Hour hour,bool for_N2_N2O_ratio) const  // was moisture_function_for_finite_difference_infiltration
{  float64 WC_sat = soil_hydraulic_properties.get_saturation_water_content_volumetric(layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
   float64 FC  = soil_hydraulic_properties.get_field_capacity_volumetric(layer);
   float64 PWP = soil_hydraulic_properties.get_permanent_wilt_point_volumetric(layer); //150831
   const float32 &alpha = denitrification_parameters.alpha;                      //150831
   float64 weighted_WC_threshold = FC * alpha + PWP * (1.0 - alpha);             //150831
   float64 WFP_threshold = weighted_WC_threshold / WC_sat                        //150831
      + (for_N2_N2O_ratio ? -0.05 : 0.0);                                        //150831_080903
   float64 WFP = soil_hydrology.get_water_filled_porosity(layer,hour);
   //141203 float64 Upper_Threshold = 1.0 * 0.75 + 0.250 * WFP_threshold;
   //150831 float64 Upper_Threshold = 1.0 * 0.7 + 0.3 * WFP_threshold;           //141203
   float64 Upper_Threshold = 0.9;                                                //150831
   float64 moisture_funct =
      (WFP > WFP_threshold)
      ? (WFP < Upper_Threshold)
        ?  pow (((WFP - WFP_threshold) / (Upper_Threshold - WFP_threshold)),2.0) //090720
        : 1.0
      : 0.0;
/*150831
   float64 FC  = soil_hydraulic_properties.get_field_capacity_volumetric(layer);
   float64	WFP_threshold = FC / WC_sat                                          //080903
      + (for_N2_N2O_ratio ? -0.05 : 0.01);                                       //080903
   float64 water_filled_porosity = soil_hydrology.get_water_filled_porosity(layer,hour);
   //141203 float64 Upper_Threshold = 1.0 * 0.75 + 0.250 * WFP_threshold;
   //150831 float64 Upper_Threshold = 1.0 * 0.7 + 0.3 * WFP_threshold;                    //141203
   float64 Upper_Threshold = 0.9;                                                //150831
   float64 moisture_funct =
		(water_filled_porosity > WFP_threshold)
      ? (water_filled_porosity < Upper_Threshold)
        ?  pow (((water_filled_porosity - WFP_threshold) / (Upper_Threshold - WFP_threshold)) ,2.0)   //090720
        : 1.0
      : 0.0;
*/
/*
if (!for_N2_N2O_ratio && (moisture_funct > 0.000001))
std::clog << "MF:" << moisture_funct<< std::endl;
*/
   return moisture_funct;
}
//_moisture_function_hourly_________________________________________2008-08-28_/
float64 NO3_with_CO2_Profile::calc_N2_to_N2O_respiration_based_potential
(float64 respiration_KgC_ha_day)                                           const
{     //  kg C/ha/day  respiration is CO2_C_loss_per_unit_soil_mass
   // Calculate respiration-based potential N2/N20
   // From Parton et al, 1996 Global Biogeochemical Cycles
   float64 N2_to_N2O_resp = 13.0 + (30.78 * atan(CORN_pi * 0.07 * (respiration_KgC_ha_day - 13.0))) / CORN_pi;
   return N2_to_N2O_resp;
}
//_calc_N2_to_N2O_respiration_based_potential_______________________2008-08-28_/
float64 NO3_with_CO2_Profile::calc_NO3_based_potential_N2_N20_ratio
(float64 NO3_N_ppm)                                                        const
{ // From Parton et al, 1996 Global Biogeochemical Cycles
   // Nitrate content must be in PPM NO3-N
   float64 N2N2ONitrate = (1.0 - (0.5 + atan(CORN_pi * 0.01 * (NO3_N_ppm - 190.0)) / CORN_pi)) * 25.0;
   return N2N2ONitrate;
}
//_calc_NO3_based_potential_N2_N20_ratio____________________________2008-08-28_/
float64 NO3_with_CO2_Profile::calc_potential_denitrification_rate(float64 WFP) const
{
   //150226 float64 scale = 0.15;
//150824 now advanced parameter   float64 scale = 0.75;
//std::cout << "denit scale:" << global_denitrification_scale << std::endl;
   return (WFP < 0.1)
      ? denitrification_parameters.rate_max                                      //150824 5.0 //150326 1.2
      : denitrification_parameters.scale                                         //150824
         + (0.4622 * WFP * WFP - 0.0893 * WFP + 0.0523); // <- shape
}
//_calc_potential_denitrification_rate______________________________2015-03-09_/

/*
float64 debug_CO2_C_loss_per_unit_soil_mass_daily_kgC_ha_accum[MAX_soil_layers_alloc] = {
 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
//,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
*/

void NO3_with_CO2_Profile::transformation         // denitrification
(nat8 layer
,float64 sublayer_water_content
,float64 sublayer_saturation_water_content
,float64 sublayer_reference_water_content
,float64 sublayer_temperature
,float64 sublayer_bulk_density
,Seconds preferred_transformation_time_step                                      //060510
   // The original transformation time step is 86400 seconds (1 day)
   // When using  Organic_matter_residues_profile_multiple_cycling,
   // the nitrification and denitrification transformation timestep
   //  is 1 hour (or the timestep of the infiltration model).
#ifdef OLD_N_XFER
,float64 &transformed_to_M
#endif
)
{  // Denitrification
   float32 standardized_respiration_for_potential_denitrification;               //151110
   const float64 denitrification_pot_max_mgN_kgsoil_hour = 24.0;
      standardized_respiration_for_potential_denitrification
         = pow((denitrification_pot_max_mgN_kgsoil_hour / 0.1), (1.0 / 1.3));

   #if (CROPSYST_VERSION==4)
   if (CO2_C_loss_subsurface && abiotic_environment_profile)  // if we know the CO2 losses by sublayer
   {
   #endif
      float64 N_denitrified_daily_layer_output_bal = 0.0;
      float64 N2O_N_loss_daily_layer = 0.0;                                      //060606
      float64 sublayer_thickness = soil_layers.get_thickness_m(layer);
      float64 soil_mass_dry_kg_m2= sublayer_bulk_density * 1000.0 * sublayer_thickness;
         // kg/m2 (with respect to the thickness of  layer)
      float64 nitrate_N_mass     = get_N_mass(layer); // kg N/m2
      if (nitrate_N_mass < 0.00000001)                                           //130617_120517
         nitrate_N_mass = 0.0;                                                   //120517
      float64 nitrate_concentration_dry_soil_kg_kg
         = nitrate_N_mass / soil_mass_dry_kg_m2; // kg N/m2 / kg Soil/ m2  -> concentration
      float64 nitrate_concentration_dry_soil_ppm
         = nitrate_concentration_dry_soil_kg_kg *1000000.0;                      //090720
       // Threshold of nitrate concentration for potential denitrification.
      float64 Threshold_Nitrate_Conc_For_Potential_Denitrification =
         pow(denitrification_pot_max_mgN_kgsoil_hour / 1.15,(1/0.57));           //151204
      float64 nitrate_function_1 =
         pow((nitrate_concentration_dry_soil_ppm / Threshold_Nitrate_Conc_For_Potential_Denitrification),0.57);
         //170217 static const float64 denitrification_half_rate_ppm = 22.0; // ppm   (similar to the constant used in Memis model)
         //151204 nitrate_concentration_dry_soil_ppm / (nitrate_concentration_dry_soil_ppm + denitrification_half_rate_ppm); //141203

      // Warning nitrate function 2 is currently a test
      // and may not be permanent.

      float64 nitrate_function_2 =
         1.0 / (1.0 + (((1.0/0.001) - 1.0)
                       * exp(- 1.015 * nitrate_concentration_dry_soil_ppm)));
      float64 nitrate_function = std::min<float64>(nitrate_function_1,nitrate_function_2);
      nat16 intervals = seconds_per_day/preferred_transformation_time_step;
      // Setup for daily mode                                                    //080904
      float64 CO2_C_loss_per_unit_soil_mass_kgC_kgsoil_daily = CO2_C_loss_subsurface[layer] / soil_mass_dry_kg_m2;  //080904
      float64 CO2_C_loss_per_unit_soil_mass_mgC_kgsoil_daily = CO2_C_loss_per_unit_soil_mass_kgC_kgsoil_daily * 1000000.0; // convert kg C/kg Soil /day to mg C/kg Soil /day       //080905
         //  for potential denitrication of 30mgN/kgSoil/day                     //081009
         // power_coef line is to is to modify the coefficient "1.3" in del Grosso et al 2000
         // this to solve for x in R^x = Pot_Den/0.1
         // where R=100 mg C/kg Soil/ day and Pot_Den is in mg N/kg Soil/day
      float64 moisture_funct = moisture_function_daily(layer,false);  // default with daily value
      float64 moisture_funct_for_N2_N20_partition = moisture_function_daily(layer,true);  // default with daily value
      Seconds seconds_into_day = 0;
      //170306 no longer used const Soil_abiotic_environment_layer &abiotic_environment = abiotic_environment_profile->ref_soil_abiotic_environment_layer( layer);   //060510
      // Functions for N2 to N2O (always daily not hourly)
      float64 N2_to_N20_NO3_function  = calc_NO3_based_potential_N2_N20_ratio(nitrate_concentration_dry_soil_ppm); //081010
      float64 CO2_C_loss_kgC_ha_day = per_m2_to_per_ha(CO2_C_loss_subsurface[layer]) ; //081010
         // kgC/ha/day
//      float64 CO2_C_loss_per_unit_soil_mass_daily_kgC_ha = per_m2_to_per_ha(CO2_C_loss_subsurface[layer]) ; //081010

         // this is respiration
      bool use_daily_moisture_function_for_hours = infiltration_model ==CASCADE_DAILY_INFILTRATION; //080110

      // Respiration functions are now computed daily (but used hourly)
      float64  respiration_function =                                            //141203
               pow(CO2_C_loss_per_unit_soil_mass_mgC_kgsoil_daily //respiration  //151117
                   / (float64)standardized_respiration_for_potential_denitrification
                  ,1.3 ); //was 1.5                                              //150326
     respiration_function
        = CORN::must_be_less_or_equal_to<float64>(respiration_function,1.0);
      float64 N2_to_N20_resp_function = calc_N2_to_N2O_respiration_based_potential
               (CO2_C_loss_kgC_ha_day);                                          //151117
      bool exhausted = CORN::is_zero<float64>(nitrate_N_mass);
      // This is because the hourly moisture function calculation can not be resolved with daily infiltration
      for (nat16 interval = 0; interval <intervals && !exhausted; interval++)
      {  Hour hour = seconds_into_day / 3600;                                    //060510
         if (!use_daily_moisture_function_for_hours)  // (intervals > 1)         //080110
         {  moisture_funct                       = moisture_function_hourly(layer,hour,false); //060510
            moisture_funct_for_N2_N20_partition  = moisture_function_hourly(layer,hour,true);  //080829
         }// else moisture_funct stays as the daily value                       //080829
         //170503 now checking exhausted if (nitrate_N_mass > 0)
         {
            float64 pot_denitrification_rate_mgN_kgSoil_hour =                   //150309
               calc_potential_denitrification_rate(soil_hydrology.get_water_filled_porosity
                  (layer ,(hour ? hour-1 : 0)));
                  // from previous hour (I currently dont store WFP from previuous day, so just use same value as midnight)
                  //150309 was 1.0;   // tests show about 1.0
                 // Potential Denitrification Rate (mg N/kg Soil/hour)

//std::cout << (int)hour << '\t' << (int)layer << '\t' <<  pot_denitrification_rate_mgN_kgSoil_hour << std::endl;


//pot_denitrification_rate_mgN_kgSoil_hour =  1.2;
// Temporarily overriding with the original value 1.0 until confirm with Claudio.

            float64 denitrification_factor = std::min<float64>                   //141203
               (respiration_function,nitrate_function);                          //141203
            float64 total_denitrification_gN_m2_h =                              //141203
               (mg_to_g(pot_denitrification_rate_mgN_kgSoil_hour)                //141203
                  * soil_mass_dry_kg_m2 * denitrification_factor)                //141203
               * moisture_funct;
            float64 total_denitrification_kgN_m2_h_pot                           //141203
               =  g_to_kg(total_denitrification_gN_m2_h);
            if (total_denitrification_kgN_m2_h_pot > 0.0000001)                  //170503RLN
            {
            float64 transformed_M_hour_pot                                       //170503RLN
               = elemental_to_molecular*total_denitrification_kgN_m2_h_pot;      //170503RLN
            float64 transformed_M_hour_act                                       //170503RLN
               = mass_M->dec_layer(layer,transformed_M_hour_pot);                //170503RLN
            exhausted = CORN::is_zero
               (transformed_M_hour_act,0.0000000001);                            //170503RLN
            float64 denitrification_act_kgN_m2_h                                 //170503RLN
               = molecular_to_elemental * transformed_M_hour_act;                //170503RLN
            N_denitrified_daily_layer_output_bal +=denitrification_act_kgN_m2_h; //170503RLN
            float64 N2_to_N20_ratio
               = std::min<float64>(N2_to_N20_NO3_function,N2_to_N20_resp_function)
                  * moisture_funct_for_N2_N20_partition;                         //080829
            float64 N2O_fraction_of_denitrification = 1.0/(1.0+N2_to_N20_ratio); //080829
            float64 N2O_N_loss_interval =
               denitrification_act_kgN_m2_h                                      //170503_141203
               * N2O_fraction_of_denitrification;

#ifdef DETAIL_DENITR
//if (CORN::is_zero<float64>(total_denitrification_kgN_m2_h, 0.000000001))       // 170214
//if (simulation_today.get()==1860004)
//(std::clog)<< (int) simulation_today.get() << '\t'
if (denitr_detail_stream)
(*denitr_detail_stream)<< (int) simdate.get_date32() << '\t'                     //170525
<< (int)layer << '\t'
<< interval << '\t'
<< nitrate_N_mass*10000.0 << '\t'
<< denitrification_act_kgN_m2_h << '\t'
<< N2O_N_loss_interval << '\t'
<< CO2_C_loss_per_unit_soil_mass_mgC_kgsoil_daily  << '\t'
<< moisture_funct << '\t'
<< nitrate_function << '\t'
<< respiration_function << '\t'
<< moisture_funct_for_N2_N20_partition << '\t'
<< N2_to_N20_NO3_function << '\t'
<< N2_to_N20_resp_function << '\t'
<< N2O_fraction_of_denitrification << '\t'
<< N_denitrified_daily_layer_output_bal

//<< N2_to_N20_ratio << '\t'
<<std::endl;
#endif
               N2O_N_loss_daily_layer += N2O_N_loss_interval;
            }
         } // endif
         seconds_into_day += preferred_transformation_time_step;
      } // for interval

/* for debugging
if (total_denitrification_kgN_m2_h > nitrate_N_mass)
{
std::cerr << "shouldn't occur" << std::endl;
N_denitrified_daily_layer = 0;
}
*/
      // Register soil layer N transformations
      float64 transformed_M = elemental_to_molecular*N_denitrified_daily_layer_output_bal;
      inc_transformed_to(transformed_M);                                         //990317
      // The total denitrification is gaseous losses and accounted for in the
      // balance so dont inc_gasseous_losses N2O losses which are only
      // recorded for output (This is different from nitrification
      // because soil N2O losses are deducted from the nitrified amount)         //141022
/*
if (N_denitrified_daily_layer_output_bal > 0.0001)
std::clog << "reached" << std::endl;
*/
      denitrified_N_output.set(layer,N_denitrified_daily_layer_output_bal);

      #ifdef OLD_N_XFER
      transformed_to_M += transformed_M;
      #else
      if (receiver) // NYN
      receiver->take_elemental_from_transformation
         (layer,N_denitrified_daily_layer_output_bal); //170502
      #endif
      // Incremented because there may be two sources of transformation
      N2O_N_loss_daily += N2O_N_loss_daily_layer;                                //060608

//if (N2O_N_loss_daily > 0.0)
//std::clog << "N2O_N_loss_daily>0 reached" << std::endl;

   #if (CROPSYST_VERSION==4)
   } else // just use the old (daily) transformation
      NO3_Profile::transformation
      (layer
      ,sublayer_water_content
      ,sublayer_saturation_water_content
      ,sublayer_reference_water_content
      ,sublayer_temperature
      ,sublayer_bulk_density
      ,86400 // NO3_profile is daily only  preferred_transformation_time_step    //060510
   // The original transformation time step is 86400 seconds (1 day)
   // When using  Organic_matter_residues_profile_multiple_cycling,
   // the nitrification and denitrification transformation timestep is 1 hour
   // (or the timestep of the infiltration model).
      #ifdef OLD_N_XFER
      ,transformed_to_M
      #endif
      );
   #endif
}
//_transformation______________________________________________________________/
void NH4_Profile_with_pH::setup_for_transformations()
{  N2O_N_loss_daily = 0.0;                                                       //060608
}
//_setup_for_transformations___________________________________________________/
void NH4_Profile_with_pH::transformation
(nat8 layer
,float64 sublayer_water_content
,float64 sublayer_saturation_water_content
,float64 sublayer_reference_water_content
,float64 sublayer_temperature
,float64 sublayer_bulk_density
,Seconds preferred_transformation_time_step                                      //060510
   // The original transformation time step is 86400 seconds (1 day)
   // When using  Organic_matter_residues_profile_multiple_cycling,
   // the nitrification and denitrification transfor mation timestep is 1 hour
   // (or the timestep of the infiltration model).
)
{
   static const float64 nitrification_constant_hourly = 0.00208; //160223 0.0083333333333;         //060814
   static const float64 Nitrification_NO3_NH4_Ratio   = 8.0; // NO3-N / NH4-N = 27 NO3/NH4
   #if (CROPSYST_VERSION==4)
   if (abiotic_environment_profile)
   {  // Do not use the Soil_abiotic_environment moisture function used by the organic matter decomposition.
   #endif

      float64 nitrate_N_mass                 = NO3.get_N_mass(layer);
      float64 ammonium_N_mass                = get_N_mass(layer);
      float64 N_oxidized_to_NO3_day = 0.0;                                       //141022
      float64 N_oxidized_total_day = 0.0;                                        //170502
      bool exhausted = CORN::is_zero<float64>(ammonium_N_mass);                  //170503
      for (Hour hour = 0; (hour < 24) && !exhausted; hour ++)                    //170503
      {  // RLN warning this is using the N masses from the beginning of the day.
         {  float64 nitrification_moisture_function =  calc_moisture_function    //070117
               (soil_hydrology.get_water_filled_porosity(layer,hour)             //070117
               ,0.5);  // not using saturation for this moisture function        //070117 141205:was(0.0)
            const Soil::Abiotic_environment_layer &abiotic_environment
               = abiotic_environment_profile->ref_soil_abiotic_environment_layer(layer);   //060510
            float64 temperature_funct_hour = abiotic_environment.
                get_temperature_function_at(hour,nitrification_temperature_function); //080904
            float64 N_oxidized_hour_pot = // was N_nitrified_hour                //141022
               ((nitrate_N_mass / ammonium_N_mass) < Nitrification_NO3_NH4_Ratio )
               ? (ammonium_N_mass - (nitrate_N_mass / Nitrification_NO3_NH4_Ratio))
                 * (1.0 - exp(-nitrification_constant_hourly * pH_function[layer] * temperature_funct_hour))
                 * nitrification_moisture_function
               :  0.0;
            float64 N_oxidized_hour_M_act = mass_M->dec_layer
               (layer,N_oxidized_hour_pot*elemental_to_molecular);
            float64 N_oxidized_hour_act
               = molecular_to_elemental * N_oxidized_hour_M_act;
            N_oxidized_total_day += N_oxidized_hour_act;                         //170502
            float64 N2O_temperature_function = 1.197 - 0.0439 * sublayer_temperature; //090722
            N2O_temperature_function = CORN::must_be_between<float64>(N2O_temperature_function,0.1,1.0);  //090722
            float64 N2O_N_loss = N_oxidized_hour_act * 0.01;                     //160323
            N2O_N_loss_daily +=  N2O_N_loss;                                     //141022
            float64 N_oxidized_to_NO3_hour = N_oxidized_hour_act - N2O_N_loss;   //141022
            nitrate_N_mass        += N_oxidized_to_NO3_hour;                     //141022
            N_oxidized_to_NO3_day += N_oxidized_to_NO3_hour;                     //141022
         }
      }
      // Register soil layer N transformations                                   //060503
      float64 transformed_M = elemental_to_molecular * N_oxidized_to_NO3_day;
      inc_transformed_to(transformed_M);                                         //990317
      //#ifdef OLD_N_XFER
      //transformed_to_M += transformed_M;
      //#else
      receiver->take_elemental_from_transformation(layer,N_oxidized_to_NO3_day);  //170502
      nitrified_N_output.set(layer,N_oxidized_to_NO3_day);                    //170502
      //#endif

      // Incremented because there may be two sources of transformation
   #if (CROPSYST_VERSION==4)
   } else
   {
      NH4_Profile::transformation
      (layer
      ,sublayer_water_content
      ,sublayer_saturation_water_content
      ,sublayer_reference_water_content
      ,sublayer_temperature
      ,sublayer_bulk_density
      ,86400 // NO3_profile is daily only  preferred_transformation_time_step    //060510
   // The original transformation time step is 86400 seconds (1 day)
   // When using  Organic_matter_residues_profile_multiple_cycling,
   // the nitrification and denitrification transformation timestep is 1 hour
   // (or the timestep of the infiltration model).
      //#ifdef OLD_N_XFER
      //,transformed_to_M
      //#endif
      );
      //#ifndef OLD_N_XFER
      assert(false);
      /*NYI need to implement

      nitrified_N_output.set(layer,xxxxx);                                       //170502

      */
      //#endif
   }
   #endif
}
//_NH4_Profile_with_pH::transformation_________________________________________/
void NH4_Profile::volatilized_application                                        //960802
(float64 non_volatilized_addition_element // <- need convert to molecular in caller
,float64 volatilized_addition_element     // <- need convert to molecular in caller
,nat8 layer)
{  application(non_volatilized_addition_element,layer
      ,1 // 3
      ,MAX_soil_sublayers,true);                                                 //000512
   // here we accumulate the amount applied NH4 that was volatilized,
   //  but it doesn't go into the soil!
   float64 addition_M = elemental_to_molecular*(volatilized_addition_element);
   inc_applied(addition_M);                                                      //990317
}
//_volatilized_application________________________________________________1999_/
RENDER_INSPECTORS_DEFINITION(NO3_Profile)
{
   #ifdef CS_OBSERVATION
   // uses soil emanator                                                         //160616
   //
   #endif
   return 0; // emanator;
}
//_RENDER_INSPECTORS_DEFINITION(NO3_Profile)___________________________________/
RENDER_INSPECTORS_DEFINITION(NO3_with_CO2_Profile)
{
   #ifdef CS_OBSERVATION
   // uses soil emanator                                                         //160616
   inspectors.append(new CS::Inspector_scalar
      (N2O_N_loss_daily,UC_kg_m2,*context,"N_N2O_emission/denitrification"
      ,value_statistic,CSVP_soil_N_N2O_loss_denitrification ));
   #endif
   return 0; // emanator;
}
//_RENDER_INSPECTORS_DEFINITION(NO3_with_CO2_Profile)__________________________/
RENDER_INSPECTORS_DEFINITION(NH4_Profile_with_pH)
{
   #ifdef CS_OBSERVATION
   // uses soil emanator                                                         //160616
   inspectors.append(new CS::Inspector_scalar(N2O_N_loss_daily,UC_kg_m2,*context,"N_N2O_emission/nitrification", value_statistic,CSVP_soil_N_N2O_loss_nitrification));
   #endif
   return 0; // emanator;
}
//_RENDER_INSPECTORS_DEFINITION(NH4_Profile_with_pH)___________________________/
}//_namespace_Soil_____________________________________________________________/

