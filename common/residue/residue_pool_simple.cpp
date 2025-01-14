#include "common/residue/residue_pool_simple.h"
#include "common/residue/residue_decomposition_param.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
//   using namespace std;
#endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "common/biomatter/decomposition_const.h"
#include "common/residue/residue_decomposition_limitations.h"
#include "common/weather/parameter/WP_air_temperature.h"
#include "soil/soil_I.h"
#include "soil/nitrogen_I.h"
#include "soil/hydrology_I.h"
#  ifdef NITROGEN
#     include "organic_matter/simple/OM_simple.h"
#  endif

//______________________________________________________________________________
#ifdef NITROGEN
soil_sublayer_array_64(default_profile_array);                                   //990217
//______________________________________________________________________________
Residue_pool_simple::Residue_pool_simple // CONSTRUCTOR init_manure
(Organic_matter_source OM_source_
,float64 new_shallow_N_content
,float64 new_deep_N_content
,float64 new_decomposition_constant                                              //060122
,float64 new_area_per_mass
,nat8 horizons[]                                                                 //9702xx
,Soil::Soil_interface &soil_                                                           //060122
,const Air_temperature_average &temperature_air_avg_)                            //150427
:Item                                                                         () //090416
,soil                                                                    (soil_) //060122
,soil_layers                                               (*soil_.ref_layers()) //060122
,soil_hydrology                                         (*soil_.ref_hydrology()) //160412_060122
,soil_hydraulic_props                        (*soil_.ref_hydraulic_properties()) //160412_060122
,soil_nitrogen_modifiable                                 (soil_.mod_nitrogen()) //160412_060122
,temperature_air_avg                                      (temperature_air_avg_) //150427
,type                                                           (MANURE_RESIDUE)
//190124,is_manure                                                               (true)
,OM_source                                                          (OM_source_) //170725
,decomposition_constant                             (new_decomposition_constant) //060122
,area_per_mass(new_area_per_mass)
,fract_ground_cover                        (1.0 - exp(-new_area_per_mass * 0.0))
   // manure goes to the shallow / deep layers }
   //For manure everything to the ground, no surface}
 // Only for start of simulation:
, biomatter_profile                                                (soil_layers) //150424_060214
#ifdef NITROGEN
, mineralization                                                             (0)
   // Units of N mass per unit area have been factored out of this function
, N_immobilization                                                           (0)
#endif
{  // Organic matter is entered as weight of molecular N, we need to
   //  get the bulk wt just so that the residue object has some thing
   //  to decompose, the bulk wt is not critical to the simulation. }
   soil_sublayer_array_64(new_biomass_subsurface);                               //990217
   soil_sublayer_array_64(new_N_content_subsurface);                             //990217
   for (int8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)           //980504
   {                                                                             //980504
       new_biomass_subsurface[sublayer] = 0.0;                                   //980504
       new_N_content_subsurface[sublayer] = 0.0;                                 //980504
   }
   // There is no surface assignment                                             //060225
   float64 N_in_manure_fraction = 100.0 / N_in_manure_percent_dry_wt[OM_source_];//070627
   new_biomass_subsurface[1]   = new_shallow_N_content * N_in_manure_fraction;   //990217
   new_biomass_subsurface[2]   = new_deep_N_content    * N_in_manure_fraction;   //990217
   new_N_content_subsurface[1] = new_shallow_N_content;  //shallow               //990217
   new_N_content_subsurface[2] = new_deep_N_content;     //deep                  //990217
   get_biomatter_profile().initialize(false                                      //040517
      ,decomposition_constant                                                    //060225
      ,50.0                                                                      //060225
      ,default_straw_residue_fraction_carbon  // This version used a default constant later versions can get the default from the crop parameter file 060227
      ,new_biomass_subsurface
      ,new_N_content_subsurface,horizons);
}
#endif
//_Residue_pool_simple:constructor_manure______________________________________/
Residue_pool_simple::Residue_pool_simple
(bool                   standing_stubble
,float64                surface_mass_
,soil_sublayer_array_64(subsurface_mass_)                                        //9702xx
#if (CROPSYST_VERSION < 5)
,float64                new_water_hold     // m3/kg
#else
// Not sure why I had removed this for V5
#endif
#ifdef NITROGEN
,float64                new_N_surface_conc
,float64                new_N_subsurface_conc
#endif
,float64                new_decomposition_constant                               //060122
,float64                new_area_per_mass
,nat8  horizons[]                                                                //970522
,Soil::Soil_interface &soil_
,const Air_temperature_average &temperature_air_avg_)                            //150427
:CORN::Item                                                                   () //171212_980303
, biomatter_profile                                      (*(soil_.ref_layers())) //150424_060214
#ifdef NITROGEN
,type                                                            (STRAW_RESIDUE) //190124
//190124 ,Xis_manure(false)
,OM_source                                                       (plant_residue) /*straw*/
#endif
,decomposition_constant                             (new_decomposition_constant)                              //060122
,area_per_mass(new_area_per_mass)
,fract_ground_cover              (1.0 - exp(-new_area_per_mass * surface_mass_))
,soil                                                                    (soil_) //060122
,soil_layers                                               (*soil_.ref_layers()) //060122
,soil_hydrology                                         (*soil_.ref_hydrology()) //160412_060122
,soil_hydraulic_props                        (*soil_.ref_hydraulic_properties()) //160412_060122
,soil_nitrogen_modifiable                                 (soil_.mod_nitrogen()) //160412_060122
,temperature_air_avg                                      (temperature_air_avg_) //150427
{  subsurface_mass_[0] = surface_mass_;                                          //011011
#ifdef NITROGEN
   soil_sublayer_array_64(new_N_content_subsurface);                             //990217
   for (nat8  sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)          //970421
      new_N_content_subsurface[sublayer] =
         new_N_subsurface_conc * subsurface_mass_[sublayer];                     //970421
#endif
   Residue_biomatter_profile_simple &biomatter_prof = get_biomatter_profile();   //060601
   biomatter_prof.initialize(standing_stubble                                    //011011
      ,decomposition_constant
      ,50.0                                                                      //060225
      ,default_straw_residue_fraction_carbon                                     //060227
      // This version used a default constant later versions
      // can get the default from the crop parameter file.
      ,subsurface_mass_,new_N_content_subsurface,horizons);
   // We temporarily used index 0 for the surface to initialize biomatter profile.
   // We need to undo the temporary assignment. //040207
   subsurface_mass_[0]     = 0;                                                  //020403
}
//_Residue_pool_simple:straw___________________________________________________/
float64 Residue_pool_simple::decompose
(float64 water_hold
,float64 &decomposed_biomass_recalculated_from_C
,Organic_matter_simple *organic_matter_pool)
{  return decompose_surface(water_hold,organic_matter_pool)
   +decompose_subsurface(decomposed_biomass_recalculated_from_C,organic_matter_pool);
}
//_decompose________________________________________________________2006-01-16_/
float64 Residue_pool_simple::decompose_surface
(float64 water_hold_m3_kg
,Organic_matter_simple *organic_matter_pool)
{  // Units of N mass per unit area have been factored out of this function
   float64 water_limiting;
   float64 water_temp_limiting1;
   float64 decomposed_surface = 0.0;
   #ifdef NITROGEN
   mineralization = 0.0;
   N_immobilization = 0.0;
   #endif
  // decomposition of surface residuel
   float64 current_surface_biomass = biomatter_profile.get_surface_mass();       //011004
   if (!CORN::is_approximately<float64>(current_surface_biomass,0.0,0.00000001)) //990217
   {
      water_limiting = CORN::must_be_between<float64>
         (water_hold_m3_kg / MAX_RESIDUE_HOLD_m3_kg,0.1,1.0);
      water_temp_limiting1 = std::min<float64>
         (water_limiting
         ,temp_limiting(temperature_air_avg.Celcius()));                         //150427
      // Previously I stored the decomposition time but then I decided to store only
      // the decomposition constant.
      // But in this version the surface decomposition is still calculated using the time to 63% decomposition
      // so I need to back calculate this (since the user now enters time to 50% decomposition  060227_
/*200220 obsolete
#if (CS_VERSION==4)
      float64 decomposition_time_63 = convert_constant_to_decomposition_time_63(decomposition_constant); //060227
      decomposed_surface = current_surface_biomass * water_temp_limiting1 /decomposition_time_63;  //990217
#else
//NYI 110916  need to calculate decomposed surface in terms of decomposition_time_50
#endif
*/
      #ifdef NITROGEN
      float64 NH4_amount_change_M = 0;
      if (soil_nitrogen_modifiable)                                              //981019
      {
         // Move the portion of surface N from decomposed material to the top soil layer for
         // since it is now liberated for other processes.
         float64 net_mineralized_N_surface = biomatter_profile.get_surface_N_content()                                           //011004
            * (decomposed_surface / current_surface_biomass); // <- fraction of surface residue decompose                        //990217
         biomatter_profile.set_surface_N_content( biomatter_profile.get_surface_N_content() - net_mineralized_N_surface);        //011004
         float64 mineralized_N_to_organic_matter = 0;                                                                            //011012
         float64 NH4_amount_M_1 = soil_nitrogen_modifiable->get_NH4_N_mass_kg_m2(1); // This is a fixup for FORTE 6.1            //030110
         mineralization       +=                                                                                                 //981018
         mineralize_surface_residue                                                                                              //981019
         (net_mineralized_N_surface                                                                                              //981019
         ,NH4_amount_M_1
         ,NH4_amount_change_M                                                                                                    //981019
         ,soil_nitrogen_modifiable->ref_ACC_NH4_released_to_atmosphere()                                                         //060122
         ,mineralized_N_to_organic_matter);                                                                                      //011012
         soil_nitrogen_modifiable->set_NH4_molecular_mass_kg_m2(1,NH4_amount_M_1);                                               //060122
         if (organic_matter_pool)                                                                                                //130603
             organic_matter_pool->receive_mineralized_N(1,mineralized_N_to_organic_matter);                                      //011012
         #ifdef CROPSYST_PROPER
         soil_nitrogen_modifiable->NH4_dec_residue(NH4_amount_change_M); // For balance verification                             //990318
         #endif
      }                                                                                                                         //981019
      #endif
      biomatter_profile.set_surface_mass(current_surface_biomass - decomposed_surface);   //990217
      fract_ground_cover = 1.0 - exp(-area_per_mass * current_surface_biomass);  //990217
   }
   return decomposed_surface;                                                    //040428
}
//_decompose_surface________________________________________________2004-04-29_/
float64 Residue_pool_simple::decompose_subsurface
(float64                &decomposed_biomass_recalculated_from_C                  //040207
,Organic_matter_simple  *organic_matter_pool)
{  // Units of N mass per unit area have been factored out of this function
   soil_sublayer_array_64(new_subsurface);                                       //9702xx
   #ifdef NITROGEN
   soil_sublayer_array_64(C_N_decomp_ratio);                                     //9702xx
   #endif
   soil_sublayer_array_64(C_N_decomp);                                           //9702xx
   for (nat8  reset_sublayer = 0; reset_sublayer <= MAX_soil_sublayers;reset_sublayer++)  //9702xx
   {  C_N_decomp        [reset_sublayer] = 1.0;                                  //9702xx
      new_subsurface    [reset_sublayer] = 0.0;                                  //981208
      #ifdef NITROGEN
      C_N_decomp_ratio  [reset_sublayer] = 0.0;
      #endif
   }
   float64 decomposed_profile = 0.0;                                             //980718
      // Previously I stored the decomposition time but then I decided to store only
      // the decomposition constant;
      // but in this version the surface decomposition is still calculated using the time to 63% decomposition
      // so I need to back calculate this (since the user now enters time to 50% decomposition 060227
   /*200220 obs
   float64 decomposition_time_63 = convert_constant_to_decomposition_time_63(decomposition_constant); //060227
   //  decomposition of subsurface residues
   */
   for (nat8  sublayer = 1; sublayer <=soil_layers.count(); sublayer++)          //060122_991207
   {  {  float64 subsurface_biomass = biomatter_profile.get_subsurface_mass(sublayer);
         if (!CORN::is_approximately<float64>(subsurface_biomass,0,0.000000001)) //011004
                        // temp_limiting should be soil temperature
         {
            #ifdef NITROGEN
            float64 N_content = biomatter_profile.get_subsurface_N_content(sublayer);  //011004
            if (  soil_nitrogen_modifiable                                       //990217
                  && (biomatter_profile.get_subsurface_N_content(sublayer) > 0.0 ) )
            {
               if (type == MANURE_RESIDUE) //190124 (is_manure)
                  C_N_decomp[sublayer] = 1.0;
               else
               {  float64 elemental_NO3_sl = soil_nitrogen_modifiable->get_NO3_N_mass_kg_m2(sublayer);
                  float64 elemental_NH4_sl = soil_nitrogen_modifiable->get_NH4_N_mass_kg_m2(sublayer);   //981212
                  float64 total_N_sublayer =
                        N_content + elemental_NO3_sl + elemental_NH4_sl;         //990217
                  C_N_decomp_ratio[sublayer] = default_straw_residue_fraction_carbon * //040225
                     /*this version uses only the default CF not the specified crop parameter*/ //
                         subsurface_biomass /total_N_sublayer;                   //990217
                  C_N_decomp[sublayer] =  exp(-0.693 * (C_N_decomp_ratio[sublayer] - 25.0) /25.0);
               }
            }
            #endif
            float64 field_capacity_sl = soil_hydraulic_props.get_field_capacity_volumetric(sublayer); //981207
            float64 water_content_sl = soil_hydrology.get_water_plus_ice_content_volumetric(sublayer);  // 070813_990311
              float64 temperature_sl = soil.get_temperature_C(sublayer);         //981207
              float64 temp_limit = temp_limiting(temperature_sl);                //981212
              float64 temp_limit2 =  water_temp_limiting2                        //981212
                    (field_capacity_sl,water_content_sl,temp_limit);
              #if (CS_VERSION==4)
              /*200220 obsolete
              new_subsurface[sublayer] = subsurface_biomass * (1- temp_limit2/
                    (decomposition_time_63/C_N_decomp[sublayer]));               //990217
              */
              #else

              assert(false);

              /*NYI   110915, need to calculate interms of halflife
              new_subsurface[sublayer] = subsurface_biomass * (1- temp_limit2/
                    (decomposition_time_63/C_N_decomp[sublayer]));               //990217
              */
              #endif
              float64 pot_decomposed_from_layer_kg_m2 = subsurface_biomass - new_subsurface[sublayer];   //040207
              float64 biomass_loss_as_CO2 = 0;                                   //040207
              float64 biomass_transfered_to_OM = 0;                              //040207
              float64 biomass_sl_before =  subsurface_biomass;                   //040207
              #ifdef NITROGEN
               if (soil_nitrogen_modifiable && (N_content > 0.0 ))                //060122
               {  float64 NO3_amount_change_M = 0;
                  float64 NH4_amount_change_M = 0;
                  float64 N_immobilized_sl = 0;                                   //040207
                  float64 mineralized_N_to_organic_matter = 0;                    //011012
                  float64 NO3_molecular_mass_kg_m2_layer = soil_nitrogen_modifiable->get_NO3_molecular_mass_kg_m2(sublayer);     //080117
                  float64 NH4_molecular_mass_kg_m2_layer = soil_nitrogen_modifiable->get_NH4_molecular_mass_kg_m2(sublayer);     //080117
                  mineralization +=
                     mineralize_subsurface_residue
                     (subsurface_biomass                                         //011004
                     ,N_content                                                  //011004
                     ,N_immobilized_sl                                           //040207
                     ,pot_decomposed_from_layer_kg_m2
                     ,biomass_loss_as_CO2                                        //040207
                     ,biomass_transfered_to_OM                                   //040207
                     ,soil_nitrogen_modifiable->ref_ACC_NH4_released_to_atmosphere()      //981211
                     ,NO3_molecular_mass_kg_m2_layer
                     ,NH4_molecular_mass_kg_m2_layer
                     ,NO3_amount_change_M                                        //970422
                     ,NH4_amount_change_M                                        //970422
                     ,mineralized_N_to_organic_matter );                         //011012
                  soil_nitrogen_modifiable->set_NO3_molecular_mass_kg_m2(sublayer,NO3_molecular_mass_kg_m2_layer);   //080117
                  soil_nitrogen_modifiable->set_NH4_molecular_mass_kg_m2(sublayer,NH4_molecular_mass_kg_m2_layer);   //080117
                  organic_matter_pool->receive_mineralized_N(sublayer,mineralized_N_to_organic_matter);  //011012
                  biomatter_profile.set_subsurface_mass(sublayer,subsurface_biomass);  //011004
                  biomatter_profile.set_subsurface_N_content(sublayer,N_content);      //011004
                  #ifdef CROPSYST_PROPER
                  soil_nitrogen_modifiable->NO3_dec_residue(NO3_amount_change_M);      //990318
                  soil_nitrogen_modifiable->NH4_dec_residue(NH4_amount_change_M);      //990318
                  #endif
                  N_immobilization += N_immobilized_sl;                                //040617
              } else
              #endif
                  biomatter_profile.set_subsurface_mass(sublayer,new_subsurface[sublayer]);     //060309
            decomposed_biomass_recalculated_from_C = biomass_loss_as_CO2 + biomass_transfered_to_OM;   //040207
            float64 biomass_sl_after =  subsurface_biomass;                      //040207
            decomposed_profile +=  (biomass_sl_before - biomass_sl_after);       //040207
         }
      }
   }
   return decomposed_profile;
}
//_decompose_subsurface_____________________________________________2004-04-29_/
void Residue_pool_simple::exhaust()
{  get_biomatter_profile().exhaust(); }
//_exhaust__________________________________________________________2004-05-23_/
#ifdef NITROGEN
float64 Residue_pool_simple::mineralize_subsurface_residue
(
 float64 &residue                                                                //970616
,float64 &residue_N_content_SL    // kg/m2                                       //970616
,float64 &N_immobilized                                                          //040207
,float64 &decomposed   // kg/m2
,float64 &soil_ACC_NH4_released_to_atmosphere                                    //981211
,float64 &biomass_loss_as_CO2                                                    //040207
,float64 &biomass_transfered_to_OM                                               //040207
,float64 & NO3_amount_M  // kg NO3/m2
,float64 & NH4_amount_M  // kg NH4/m2
,float64 & NO3_amount_change_M                                                   //960902
,float64 & NH4_amount_change_M                                                   //960902
,float64 &mineralized_N_to_organic_matter)                                       //011012
{  // returns kg/m2
   static const float64 Synthesis_Efficiency_Constant = 0.5;                     //070627
   static const float64 Humification_Fraction         = 0.2;                     //070627
   static const float64 Microbe_CN_Ratio              =10.0;                     //070627
   static const float64 Organic_Matter_CN_Ratio       =10.0;                     //070627
   // Fraction of carbon in organic mat.
   // Using the same value or residue and manure,
   // but need to check.
   float64 Ammonium_amount_Immobilized= 0.0;                                     //04020&
   float64 Nitrate_amount_Immobilized = 0.0;                                     //04020&
   float64 C_N_residue_ratio =
      default_straw_residue_fraction_carbon
      /*this version uses only the default CF not the specified crop parameter*/
      * residue / residue_N_content_SL;
   float64 carbon_decomposed = decomposed * default_straw_residue_fraction_carbon;
   float64 carbon_decomposed_available_to_microbes = carbon_decomposed * Synthesis_Efficiency_Constant;
   float64 net_residue_N_mineralized =
      (carbon_decomposed * ( 1.0 / C_N_residue_ratio)
      - carbon_decomposed_available_to_microbes/Microbe_CN_Ratio);
   float64 OLD_NO3_amount_M = NO3_amount_M;
   float64 OLD_NH4_amount_M = NH4_amount_M;
   // if negative we are taking N from the soil
   if (net_residue_N_mineralized < 0)
   {  float64 N_deficit_for_immobilization = fabs(net_residue_N_mineralized);
      net_residue_N_mineralized = 0.0;
      float64 avail_soil_N_amount = (NO3_amount_M * NO3_to_N_conv + NH4_amount_M*NH4_to_N_conv);
      if (N_deficit_for_immobilization < avail_soil_N_amount)
      {  N_immobilized = N_deficit_for_immobilization;
         residue_N_content_SL += N_immobilized;
         //Now take the mineralization out of soil N pools, but first the NH4 pool
         // We should actually do this to the transformed_from array.
         Ammonium_amount_Immobilized = (N_immobilized * N_to_NH4_conv);
         NH4_amount_M -= Ammonium_amount_Immobilized;
         if (NH4_amount_M < 0)
         {  Nitrate_amount_Immobilized =(NH4_amount_M  * NH4_to_N_conv * N_to_NO3_conv );
            NO3_amount_M += Nitrate_amount_Immobilized;
            // if daily_NO3 < 0.0 THEN Taken care of above
            NH4_amount_M = 0;
         }
      } else
      {  // Not enough N in soil to immobilize
         N_immobilized = avail_soil_N_amount;
         residue_N_content_SL += N_immobilized;
         N_deficit_for_immobilization -= N_immobilized;
         NH4_amount_M = 0;
         NO3_amount_M = 0;
         // Not enough N to support full decomposition, so
         // recalculate actual decomposition
         float64 undecomposable_carbon = N_deficit_for_immobilization
           / (C_N_residue_ratio - Synthesis_Efficiency_Constant/Microbe_CN_Ratio);
         carbon_decomposed -= undecomposable_carbon;
         decomposed = carbon_decomposed / default_straw_residue_fraction_carbon;  // biomass decomposed
      }
   } else // net_residue_N_mineralized > 0 mineralization
      residue_N_content_SL -= net_residue_N_mineralized;
   float64 Organic_Residue_Carbon_Loss_As_CO2
      = carbon_decomposed * (1.0 - Synthesis_Efficiency_Constant);
   // Carbon transferred to the stable organic matter (humus) pool, kg/m2
   float64 Carbon_Decomposed_Transferred_To_Organic_Matter
      = carbon_decomposed * Synthesis_Efficiency_Constant * Humification_Fraction;
   // Nitrogen transferred to the stable organic matter (humus) pool, kg/m2
   float64 Nitrogen_Transferred_To_Organic_Matter
      = Carbon_Decomposed_Transferred_To_Organic_Matter / Organic_Matter_CN_Ratio;
   float64 Carbon_Mass_In_Organic_Residue_Plus_Microbes
      = residue * default_straw_residue_fraction_carbon;
   float64 New_Carbon_Mass_In_Organic_Residue_Plus_Microbes
      = Carbon_Mass_In_Organic_Residue_Plus_Microbes
      - Carbon_Decomposed_Transferred_To_Organic_Matter - Organic_Residue_Carbon_Loss_As_CO2;
   residue_N_content_SL -= Nitrogen_Transferred_To_Organic_Matter;
   // Currently all
   NH4_amount_M += net_residue_N_mineralized * N_to_NH4_conv;
   mineralized_N_to_organic_matter = Nitrogen_Transferred_To_Organic_Matter;
   float64 layer_released_to_atmosphere = 0.0;
   soil_ACC_NH4_released_to_atmosphere +=
            layer_released_to_atmosphere; //{ separate organic and in organicloss }
   Carbon_Mass_In_Organic_Residue_Plus_Microbes
   = New_Carbon_Mass_In_Organic_Residue_Plus_Microbes;
   // Update C/N ratio of organic residues plus microbes pool
    residue = Carbon_Mass_In_Organic_Residue_Plus_Microbes / default_straw_residue_fraction_carbon;
    biomass_loss_as_CO2 = Organic_Residue_Carbon_Loss_As_CO2/default_straw_residue_fraction_carbon;
    biomass_transfered_to_OM =  Carbon_Decomposed_Transferred_To_Organic_Matter/  default_straw_residue_fraction_carbon;
   NO3_amount_change_M = (OLD_NO3_amount_M - NO3_amount_M);
   NH4_amount_change_M = (OLD_NH4_amount_M - NH4_amount_M);
   return  net_residue_N_mineralized ;
}
//_mineralize_subsurface_residue____________________________________2004-02-07_/
void Residue_pool_simple::apportion_mineralized_N
(float64 net_residue_N_mineralized
,float64 fract_released_to_soil_ammonium
,float64 fract_released_to_OM
,float64 fract_released_to_atmosphere
,float64 &soil_ACC_NH4_released_to_atmosphere   // to be modified                //981211
,float64 &NH4_amount_M             // kg NH4/m2
,float64 &mineralized_N_to_organic_matter)                                       //011012
{  NH4_amount_M +=
         net_residue_N_mineralized * fract_released_to_soil_ammonium * N_to_NH4_conv;               //980924
   mineralized_N_to_organic_matter = fract_released_to_OM * (net_residue_N_mineralized);            //011012
   float64 layer_released_to_atmosphere = net_residue_N_mineralized * fract_released_to_atmosphere; //980908
   soil_ACC_NH4_released_to_atmosphere +=                                        //960814
      layer_released_to_atmosphere; // separate organic and inorganic loss       //960802
}
//_apportion_mineralized_N__________________________________________1998-10-18_/
float64 Residue_pool_simple::mineralize_surface_residue
(float64 net_residue_N_mineralized
,float64 &NH4_amount_M              // kg NH4/m2
,float64 &NH4_amount_change_M
,float64 &soil_ACC_NH4_released_to_atmosphere                                    //981211
,float64 &mineralized_N_to_organic_matter)                                       //011012
{  static const float64 surf_fract_released_to_soil_ammonium = 0.8;              //070627
   static const float64 surf_fract_released_to_OM            = 0.2;              //070627
   float64 OLD_NH4_amount_M = NH4_amount_M;
   apportion_mineralized_N
      (net_residue_N_mineralized
      ,surf_fract_released_to_soil_ammonium
      ,surf_fract_released_to_OM
      ,0.0
      ,soil_ACC_NH4_released_to_atmosphere
      ,NH4_amount_M
      ,mineralized_N_to_organic_matter);                                         //011012
   NH4_amount_change_M  = (OLD_NH4_amount_M - NH4_amount_M);
   return net_residue_N_mineralized * 0.8;  // not sure if this is surf_fract_released_to_soil_ammonium
}
//_mineralize_surface_residue_______________________________________1998-10-18_/
#endif
//070627 668 lines
//130604 458

