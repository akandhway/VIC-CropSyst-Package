#ifndef cs_accumH
#define cs_accumH
#include "options.h"
#include "corn/chronometry/date_I.h"
#include "corn/math/statistical/sample_cumulative_T.h"
#define key_profile_sum_depths_COUNT 7
namespace CropSyst {
class Land_unit_simulation_VX;
class Pond;                                                                      //160408
extern float32 key_profile_sum_depths[];                                         //160629
//______________________________________________________________________________
class Water_balance_accumulators
{
 protected:
   CropSyst::Land_unit_simulation_VX &simulation;                                //141206
   #ifdef VIC_CROPSYST_VERSION
   float64     ponded;
   #else
   const Pond &pond;                                                             //160408_991102
   #endif
   bool        run_nitrogen;                                                     //190709
 public:
   float64 init_total_water_content;  //131004 actually this is water depth
   float64 water_depletion;
   float64 initial_ponded;
   float64 eff_precipitation;
   float64 irrigation;
   float64 runoff;
   float64 act_transpiration;
   float64 act_uptake;                                                           //160414
   float64 crop_intercept;                                                       //160419
   float64 residue_interception;
   float64 act_residue_evaporation;
   float64 act_soil_evaporation;
   float64 act_pond_evaporation;                                                 //160724
   float64 water_table_input;                                                    //981019
#ifdef WSHEDMOD
   float64 lateral_flow_in;
   float64 lateral_flow_out;
#endif
   float64 reported_drainage;
   float64 balance_drainage;                                                     //990106
   float64 balance_microirrigation_excess;                                       //200309
   float64 finite_diff_error; //  for water balance
   float64 water_cont_recalib; // For recalibration                              //000313
   float64 water_balance;
   float64 water_balance_last;                                                   //151225
 public:
   Water_balance_accumulators
      (CropSyst::Land_unit_simulation_VX &simulation                             //010202
      ,float64 init_total_water_content
      ,float64 initial_ponded);
   inline virtual ~Water_balance_accumulators() {}                               //170217
   virtual void accumulate();                                                    //190709
   void compute_water_balance
      (float64 total_soil_water_content_today
      ,float64 daily_finite_diff_error_today
      //160408 now member reference to pond ,float64 ponded
      ,float64 deferred // F.D.                                                  //071024
      ,bool  main_water_balance                                                  //000626
      ,bool show_water_balance_errors);                                          //051115
};
//_Water_balance_accumulators_______________________________________1999-03-10_/
class Period_accumulators
: public Water_balance_accumulators
{
 public: // Mass units are all in kg/m�
      float64
      gaseous_N_loss,
      evapotranspiration_max, //190812 pot_evapotranspiration,
      evapotranspiration_act,
      evapotranspiration_ref,                                                    //130723
      pot_transpiration,
      pot_soil_evaporation,
      pot_residue_evaporation,
      precip_m,
      act_water_entering_soil,  // <- This is now what we want to report
      #ifdef MICROBASIN_VERSION
      water_stress_index,                                                        //161101LML
      nitrogen_stress_index,                                                     //161101LML
      #endif
      N_nitrification,                                                           //170331LML
      N_denitrification,                                                         //170207LML
      soil_infiltration,
      total_N_applied, // inorganic and organic                                  //150323
      inorganic_N_applied,                                                       //160323
      organic_N_applied,
      NH3_volatilization_loss,                                                   //021212 170414LML note: N-element from organic fertilizer
      inorganic_NH4_to_atmosphere,
      N_fert_requirements,
      N_fixation,
      N_uptake,                                                                  //120514

      //050118 we now only have mineralization from organic matter
      straw_mineralization_V4_1,    // These only apply to version 4.1
      manure_mineralization_V4_1,
      OM_N_mineralization,                                                       //110918
      N_immobilization,                                                          //040209
      biomass_grazed,                                                            //040209
      biomass_unsatisfied,                                                       //040627
      gaseous_CO2_C_loss_OM,                                                     //060426
      gaseous_CO2_C_loss_SOM, // Including microbial                             //060811
      gaseous_CO2_C_loss_residue,                                                //060811

      C_decomposed_SOM_profile,                                                  //090520
      C_decomposed_SOM_5cm,                                                      //090520
      C_decomposed_SOM_10cm,                                                     //090520
      C_decomposed_SOM_15cm,                                                     //090520
      C_decomposed_SOM_30cm,                                                     //090520
      C_decomposed_SOM_60cm,                                                     //090520
      C_decomposed_SOM_90cm,                                                     //090520

      C_decomposed_residue_profile,  // was C_decomposed_residue_subsurface      //090520
      C_decomposed_residue_5cm,                                                  //090520
      C_decomposed_residue_10cm,                                                 //090520
      C_decomposed_residue_15cm,                                                 //090520
      C_decomposed_residue_30cm,                                                 //090520
      C_decomposed_residue_60cm,                                                 //090520
      C_decomposed_residue_90cm,                                                 //090520

      C_decomposed_residue_flat,                                                 //060811
      C_decomposed_residue_stubble,                                              //060811
      C_decomposed_residue_attached,                                             //060811
      C_incorporated_by_tillage,                                                 //060811
      C_removed_by_harvest,                                                      //060811
      plant_C_mass_canopy,                                                       //060811_110915
      plant_C_mass_roots,                                                        //060811_110915

      N_mass_yield,                                                              //120117

      N2O_loss_denitrification,                                                  //060606
      N2O_loss_nitrification                                                     //090722
      ;
   //180108 temp restored       #ifndef CS_INSPECTOR_VERSION
      //170326 Now mineralization vector is stored,
      // this output is now processed from collator using new strata classes
      float64 N_OM_mineralization[key_profile_sum_depths_COUNT];                 //160629
      float64 N_OM_immobilization[key_profile_sum_depths_COUNT];                 //160629
   //180108 temp restored       #endif
   CORN::statistical::Sample_cumulative<float32,float32>air_temperature_stats;    //160324
      // Tina wanted this
 public:
   float64 N_uptake_today ;                                                      //120514
      // this is not an accumulator, just storying it so we don't have duplicate
 public:
   Period_accumulators
      (CropSyst::Land_unit_simulation_VX &simulation
      ,float64 init_total_water_content
      ,float64 initial_ponded);
   virtual void accumulate();
 };
//_class Period_accumulators________________________________________2001-02-02_/
class Annual_accumulators : public Period_accumulators
{
public:
   Annual_accumulators
      (CropSyst::Land_unit_simulation_VX & simulation
      ,float64 init_total_water_content
      ,float64 initial_ponded);
};
//_class Annual_accumulators________________________________________2001-02-02_/
}//_namespace_CropSyst_________________________________________________________/
#endif

