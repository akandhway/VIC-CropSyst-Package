#ifndef cs_UED_annualH
#define cs_UED_annualH
#include <options.h>
#include "corn/data_source/datarec.h"
#include "UED/library/UED_tuple_datasrc.h"
#include "cs_accum.h"

#include "organic_matter/OM_residues_profile_abstract.h" //161017
#include "soil.h"
//______________________________________________________________________________
namespace CropSyst                                                               //130309
{  class Pond;
   class Annual_accumulators;
   //161017 class Organic_matter_residues_profile_abstract;                               //161017
}
class Residues_interface;
//using namespace CORN;
class Soil_cropsyst_specific;
//______________________________________________________________________________
namespace CropSyst                                                               //160629
{
//______________________________________________________________________________
//______________________________________________________________________________
class Annual_data_record : public CORN::Data_record
{
#              define annual_section  "annual"
   int32       a_out_ID;       // Used only for LADSS
   int32       alloc_ID;       // Used only for LADSS
   int16       year_LADSS;           // Used only for LADSS
   float32 residue_ground_cover;
   float32 soil_loss;
   float32 RUSLE_C_factor;                                                       //050505

//180108 temp restored      #ifndef CS_INSPECTOR_VERSION
   float32 ET_max; //190812 ET_pot;
   float32 ET_act;
   float32 ET_ref;                                                               //130723
   float32 transp_pot;
   float32 transp_act;
   float32 evap_soil_pot;
   float32 evap_soil_act;
   float32 evap_residue_pot;
   float32 evap_residue_act;
   float32 irrigation;
   float32 precipitation;
   float32 crop_water_intrcpt;
   float32 residue_water_intrcpt;
   float32 surface_water_runoff;
   float32 ponded_water;
   float32 water_enter_soil;
   float32 soil_water_drainage;
//180108 temp restored      #endif
   float32 soil_water_balance;

   float32 biomass_grazed;
  #ifdef NITROGEN
//180108 temp restored      #ifndef CS_INSPECTOR_VERSION
   float32  N_inorganic_applied;  // NO3 and NH4
   float32  N_total_applied;           // was nitrogen_applied_total                                                //160323
   float32  N_leached;
   float32  N_uptake;
   float32  N_auto_uptake;       // added_auto_N_in_uptake
   float32  N_mineralized_residue;       // mineralized_from_residue
   float32  N_mineralized_org_fert;      // mineralized_from_org_fert
   float32  N_mineralized_OM;
   float32  N_immobilization;
   float32  N_mineralization;          // total mineralization
   float32  N_organic_applied;         // organic_N_applied
   float32  N_organic_gaseous_loss;    // organic_N_gaseous_loss  (NH3_volatilization)
   float32  N_inorganic_gaseous_loss;  // inorganic_N_gaseous_loss
   float32  N_fixation;
//180108 temp restored      #endif
   float32  C_soil_surface;                                                      //080225
   float32  C_soil               [key_profile_sum_depths_COUNT];                 //160629
   float32  C_SOM                [key_profile_sum_depths_COUNT];                 //160629
   float32  C_microbial          [key_profile_sum_depths_COUNT];                 //160629
   float32  C_labile             [key_profile_sum_depths_COUNT];                 //160629
   float32  C_metastable         [key_profile_sum_depths_COUNT];                 //160629
   float32  C_passive            [key_profile_sum_depths_COUNT];                 //160629
   float32  C_residues_surface_minimum;                                          //070508
                     // this is the minimum surface residue encountered during the year
   float32  C_residues_profile_minimum;                                          //070521
   float32  C_residues_surface;                                                  //070508
   float32  C_residues           [key_profile_sum_depths_COUNT];                 //160629
   float32  C_residues_minimum   [key_profile_sum_depths_COUNT];                 //160629
   float32  N_OM_mineralization  [key_profile_sum_depths_COUNT];                 //160629
   float32  N_OM_immobilization  [key_profile_sum_depths_COUNT];                 //160629
   float32  C_decomposed_SOM     [key_profile_sum_depths_COUNT];                 //160629
   float32  C_decomposed_residue [key_profile_sum_depths_COUNT];                 //160629

//180108 temp restored      #ifndef CS_INSPECTOR_VERSION
   float32  CO2_C_loss_OM;                                                       //060426
   float32  CO2_C_loss_SOM;                                                      //060811
   float32  CO2_C_loss_residue;                                                  //060811
//180108 temp restored      #endif
   float32  C_decomposed_residue_flat;                                           //060817
   float32  C_decomposed_residue_stubble;                                        //060817
   float32  C_decomposed_residue_attached;                                       //060817
   float32  C_incorporated_by_tillage;                                           //060817
   float32  C_removed_by_harvest;                                                //060817
   float32  plant_C_mass_canopy;                                                 //060818
   float32  plant_C_mass_root;                                                   //060818
   float32  denitrification_X;
   float32  N_soil_profile;      // Mineral N in the soil    (mineral profile N)  // this is only for luca
   float32  N_soil_inputs;       // total N inputs to the soil     (check that N applied include organic N) // this is only for luca
   float32  N_soil_output; // total N outputs from the soil  // this is only for luca
//180108 temp restored      #ifndef CS_INSPECTOR_VERSION
   float32  N_N2O_loss_denitrification;                                          //111207
   float32  N_N2O_loss_nitrification;                                            //111207
//180108 temp restored      #endif
  #endif
  // NITROGEN
//180108 temp restored     #ifndef CS_INSPECTOR_VERSION
   #ifdef PHOSPHORUS
   float32  P_uptake;                                                            //110801
   float32  P_applied;
   #endif
//180108 temp restored     #endif
 public: // constructor
   Annual_data_record();
   virtual bool expect_structure(bool for_write);                                //161025
   void update
      (const CropSyst::Soil_components     *soil                                           //180106_141108 now const
      , CropSyst::Annual_accumulators      &ANNL_accumulators
      , Residues_interface       *residue_pools_kg_m2
      ,Organic_matter_residues_profile_abstract *residue_and_organic_matter_profile //050726
      , CropSyst::Pond           &pond                                           //130309
      , int32                    _a_out_ID  // Only for LADSS
      , int16                    _year);      // Only for LADSS
};
//_Annual_data_record__________________________________________________________/
class Annual_data_source : public UED::Tuple_data_source
, public Annual_data_record
{
   // Keep a single data record which is updated as needed.
   // This will require the structure to be setup only once.
 public: // Constructor
   Annual_data_source(UED::Database_file_indexed *i_database );
   void record(datetime64 date_time);
};
//_Annual_data_source__________________________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

