#include <math.h>
#include "organic_matter/multiple_pool/OM_multiple_pool.h"
#include "soil/texture_I.h"
#include "soil/abiotic_environment.h"
#include "common/biomatter/organic_biomatter_balance.h"
#include "corn/math/compare.h"
#include "corn/measure/measures.h"
//______________________________________________________________________________
Organic_biomatter_pool_multiple::Organic_biomatter_pool_multiple(const Organic_biomatter_pool_multiple &from_copy)
: Organic_biomatter_pool_common     (from_copy)                                  //110929
,  decomposition                    (0)
,  carbon_to_deduct                 (from_copy.carbon_to_deduct)                 //060808
,  carbon_to_receive                (from_copy.carbon_to_receive)                //060808
{  // Warning: the following default copy copies the same receivers
   // but the copy constructor is generally used when moving OM
   // (usually residues) to different layers
   // in this case the receivers need to be for the other layer
   // so if the layer is going to be different, it is necessary
   // for the caller to call contribute_to()
   // For residues, created by Residues_V4_4, call setup_contribution()
   for (int i = 0; i <= 3; i++)
   {  receiver[i] = from_copy.receiver[i];
   }
}  // Copy constructor is used by Residues_V4_4::respond_to_field_operation
//_Organic_biomatter_pool_multiple:copy_constructor_________________2006-07-27_/
// 070707   check to see if this constructor is used.
// 070707   This did not set the carbon_fraction, C/N ratio, or decomposition_constant
//          Not sure how this would work with out.
//          I think this was just a place holder for accumulators
//          so I think can just use Organic_biomatter_common for the accumulators
Organic_biomatter_pool_multiple::Organic_biomatter_pool_multiple    // For stable provide
(Organic_matter_type              type_
,nat8                             layer_
,float64                         &contact_fraction_                              //190327_190215
,const Soil::Texture_interface    &soil_texture_                                 //070707
,const Soil::Abiotic_environment_layer &soil_environment_)
:Organic_biomatter_pool_common
   (type_
   ,layer_
   ,OM_subsurface_position
   ,OM_INTRINSIC_CYCLING                                                         //110930
   ,UNKNOWN_RESIDUE_TYPE  //111001
   ,0  //  biomass
   ,0  //  carbon fraction   // check that this is setup later!
   ,0  //  carbon_nitrogen_ratio    // check that this is setup later!
   ,0  //  decomposition_constant
   ,0.0  // detrition constant
   ,0.0 /*111001 looks like area_to_mass was not initialized in this case */     //111001
   ,contact_fraction_                                                            //111001
   ,soil_texture_
   ,soil_environment_)                                                           //070707
, decomposition            (0)
, carbon_to_deduct         (0)
, carbon_to_receive        (0)
{
   C_fraction_transfered_to_CO2 = 0.0;
   for (int i = 0; i < 4; i++)
   {  C_fraction_transferred_to[i] = 0;
      receiver[i] = 0;
   }
}
//_Organic_biomatter_pool_multiple:constructor_________________________________/
Organic_biomatter_pool_multiple::Organic_biomatter_pool_multiple
// For stable organic matter and microbial
(Organic_matter_type     type_
,nat8                    layer_
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_
,float64                 area_per_mass_       // = 0.0  stubble and surface residue_only?
,float64                &contact_fraction_                                       //190215
,const Soil::Texture_interface &soil_texture_                                    //070707
,const Soil::Abiotic_environment_layer & soil_environment_)
:Organic_biomatter_pool_common
(type_,layer_,OM_subsurface_position
,OM_INTRINSIC_CYCLING                                                            //110930
,UNKNOWN_RESIDUE_TYPE                                                            //111001
,biomass_,carbon_fraction_,carbon_nitrogen_ratio_,decomposition_constant_
,0 /*only residues have detrition */
,area_per_mass_                                                                  //111001
,contact_fraction_
,soil_texture_,soil_environment_)                                                //070707
, decomposition            (0)
, carbon_to_deduct         (0)
, carbon_to_receive        (0)
{  C_fraction_transfered_to_CO2 = 0.0;
   for (int i = 0; i < 4; i++)
   {  C_fraction_transferred_to[i] = 0;
      receiver[i] = 0;
   }
}
//_Organic_biomatter_pool_multiple:constructor_________________________________/
Organic_biomatter_pool_multiple::Organic_biomatter_pool_multiple
// for residue
(Organic_matter_type     type_
,Organic_matter_position position_
,nat8                    layer_
,Organic_matter_cycling  cycling_
,RUSLE2_Residue_type     RUSLE2_residue_type_                                    //060721
,float64                 biomass_
,float64                 carbon_fraction_
,float64                 carbon_nitrogen_ratio_
,float64                 decomposition_constant_
,float64                 detrition_constant_                                     //090702_090513 _decomposition_constant_nonmicrobial
,float64                 area_per_mass_       // = 0.0  stubble and surface residue_only?
,float64                &contact_fraction_                                       //190215
,const Soil::Texture_interface &soil_texture_                                    //070707
,const Soil::Abiotic_environment_layer & soil_environment_)
:Organic_biomatter_pool_common
   (type_,layer_,position_
   ,cycling_                                                                     //110930
   ,RUSLE2_residue_type_                                                         //111001
   ,biomass_,carbon_fraction_                                                    //070707
   ,carbon_nitrogen_ratio_,decomposition_constant_                               //070707
   ,detrition_constant_                                                          //090702
   ,area_per_mass_                                                               //111001
   ,contact_fraction_                                                            //111001
   ,soil_texture_,soil_environment_)                                             //070707
, decomposition            (0)
, carbon_to_deduct         (0)
, carbon_to_receive        (0)
{  C_fraction_transfered_to_CO2 = 0.0;
   for (int i = 0; i < 4; i++)
   {  C_fraction_transferred_to[i] = 0;
      receiver[i] = 0;
   }
}
//_Organic_biomatter_pool_multiple:constructor_________________________________/
void Organic_biomatter_pool_multiple::clear_decomposition()
{  if (decomposition) delete decomposition; decomposition = 0;
}
//_clear_decomposition_________________________________________________________/
float64 Organic_biomatter_pool_multiple::transfer_carbon_and_nitrogen()
{  float64 C_transfer_to_CO2 = 0;
   if (decomposition)                                                            //060426
   {  float64 decomposed_carbon =  decomposition->act_carbon;
      C_transfer_to_CO2 = decomposed_carbon * C_fraction_transfered_to_CO2;
      deduct_carbon_pending(C_transfer_to_CO2);
      for (int i = 0; i <=3; i++)
      {  if (receiver[i])
         {
            float64 C_transfer_to_i =   decomposed_carbon *  C_fraction_transferred_to[i];
            deduct_carbon_pending(C_transfer_to_i);
            receiver[i]->receive_carbon_pending(C_transfer_to_i);
         }
      }
   // Decomposition is now cleared (deleted) at the start of the day so we have values to record. 061117
   }
   return C_transfer_to_CO2;
}
//_transfer_carbon_and_nitrogen________________________________________________/
Organic_biomatter_pool_common::Decomposition *Organic_biomatter_pool_multiple
::provide_decomposition_residue
(float64 moisture_function
,float64 temperature_function)                                        provision_
{  return decomposition = new Organic_biomatter_pool_multiple::Carbon_decomposition_residue
      (*this,temperature_function,moisture_function,true );  // is_a_residue
}
//_provide_decomposition_residue____________________________________2011-10-03_/
Organic_biomatter_pool_common::Decomposition *Organic_biomatter_pool_multiple
::provide_decomposition_SOM
(float64 tillage_decomposition_rate_adj
,float64 minimized_temperature_moisture_function)                     provision_
{  return decomposition = new Organic_biomatter_pool_multiple::Carbon_decomposition_common
      (*this,tillage_decomposition_rate_adj,minimized_temperature_moisture_function,false);
}
//_provide_decomposition_SOM________________________________________2011-10-03_/
// This is not specialized so this method can be deleted (it is the same as common)
float64 Organic_biomatter_pool_multiple::Carbon_decomposition_common::actual
(float64 N_deficit_for_immobilization
,float64 decomposition_reduction_factor)
{  // Begin reduction to potential decomposition amount

      if ((net_N_mineralization < 0.0)   // negative net_N_mineralization implies immobilization
         || is_a_residue)                                                        //060424
           act_carbon = pot_carbon * decomposition_reduction_factor ;
      else act_carbon = pot_carbon; // updated from VB 040715
      act_carbon = std::min<float64>(act_carbon,pool.get_carbon_kg());           //120312
   return act_carbon;
}
//_Carbon_decomposition_common::actual_________________________________________/
Organic_biomatter_pool_multiple::Carbon_decomposition_residue::Carbon_decomposition_residue
(const Organic_biomatter_pool_multiple &_pool                                    //110930
,float64 _temperature_function
,float64 _moisture_function
,bool    _is_a_residue)
:Organic_biomatter_pool_multiple::Carbon_decomposition_common
      (_pool                                                                     //111003
      , 1.0                                                                      // 070707
      // Claudio decided tillage adjustment should only affect SOM pools, so 1.0 is used for residues
      , 1.0  // _minimized_temperature_moisture_function  // does not apply to residue V4.3
      , _is_a_residue)
, temperature_function  (_temperature_function)
, moisture_function     (_moisture_function)
{}
//_Carbon_decomposition_residue:constructor____________________________________/
 Organic_biomatter_pool_multiple::Carbon_decomposition_common::Carbon_decomposition_common
(const Organic_biomatter_pool_multiple &_pool                                    //110930
,float64 _tillage_decomposition_rate_adj
,float64 _minimized_temperature_moisture_function
,bool    _is_a_residue)
: Organic_biomatter_pool_common::Decomposition
(_pool
,_tillage_decomposition_rate_adj
,_minimized_temperature_moisture_function
,_is_a_residue)
{}
//_Carbon_decomposition_common:constructor_____________________________________/
float64 Organic_biomatter_pool_multiple::Carbon_decomposition_common
::N_mineralization_or_immobilization_demand
(float64 carbon_nitrogen_ratio)                                                  //110930
{  // In VB this is done for each organic matter by SoilOrganicCarbonClass::MineralizationAndImmobilizationDemand
   N_immobilization_demand = 0;                                                  //061219
   //110930 Claudio thinks there should not be mineralization of residues
   // so no special case for residue, just the else
   // before I commit this, I want to test it.
   if (net_N_mineralization < 0)
           N_immobilization_demand = -net_N_mineralization;
   else N_mineralization = net_N_mineralization;
/*
if (net_N_mineralization > 0.000000001)  // the else is debug only
std::cout << "positive mineralization" << std::endl;
*/
   return N_immobilization_demand;
}
//_N_mineralization_or_immobilization_demand___________________________________/
float64 Organic_biomatter_pool_multiple::Carbon_decomposition_residue::potential()
{
#ifdef DEBUG_CARBON_TEST
temperature_function = 0.5;
moisture_function    = 0.5;
contact_fraction     = 0.3;
#endif

   // This is now actually identical to single pool (should just have a common residue potential decomposition

   float64 temperature_term = (1.0 - exp(-pool.get_decomposition_constant()
       * temperature_function));                                                 //040806
   float64 contact_fraction = pool.get_contact_fraction();                       //190327
   float64 microbial_term  = pool.get_carbon_kg()
      * contact_fraction
      * temperature_term * moisture_function;                                    //090413
   // Now nonmicrobial decomposition has been replaced by detrition              //090702
   pot_carbon  = microbial_term;                                                 //090409
   //printf("dec_const(%f)\ttemperature_function(%f)\ttemperature_term(%f)\tmicrobial_term(%f)\n",
   //       pool.get_decomposition_constant(),temperature_function,temperature_term,microbial_term);
   // Unlike SOM there is no tillage decomposition rate adjustment (Claudio decided tillage adjustment should only affect SOM pools 070707)
   // Actual carbon decomposition will be the potential unless it is reduced
   act_carbon = pot_carbon; // nonmicrobial?
   return pot_carbon;
}
//_Carbon_decomposition_residue::potential_____________________________________/
Organic_biomatter_pool_multiple::~Organic_biomatter_pool_multiple()
{  clear_decomposition();  }
//_Organic_biomatter_pool_multiple:destructor__________________________________/
void Organic_biomatter_pool_multiple::deduct_carbon_pending(float64 removed_carbon_mass)
{  carbon_to_deduct += removed_carbon_mass;
}
//_deduct_carbon_pending_______________________________________________________/
void Organic_biomatter_pool_multiple::receive_carbon_pending(float64 added_carbon_mass)
{  carbon_to_receive += added_carbon_mass; }
//_receive_carbon_pending______________________________________________________/
float64 Organic_biomatter_pool_multiple::post_carbon_transfer()    modification_
{
   float64 net_C = 0.0;                                                          //190219
   if (carbon_to_deduct > 0.0)
      net_C -= deduct_carbon(carbon_to_deduct);
   if (carbon_to_receive > 0.0)
      net_C += receive_carbon(carbon_to_receive);                                //190219
   carbon_to_deduct = 0;
   carbon_to_receive= 0;
   return net_C;
}
//_post_carbon_transfer________________________________________________________/

