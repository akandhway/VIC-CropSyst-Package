
#include <math.h>
#include "corn/math/compare.hpp"
#include "options.h"
#include "cs_chem_pot_mineral_N_V4.h"
#include "soil/chemical_mass_profile.h"
#include "corn/measure/measures.h"
#include "soil/soil_I.h"

// Potential mineralizable N is now being processed
// with the new organic matter carbon nitrogen model.
#ifdef NITROGEN
#include "organic_matter/simple/OM_simple.h"
//______________________________________________________________________________
static soil_layer_array64(unused_amount_E) =
{0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0,0,
 0,0,0,0,0,0,0,0,0};
//_unused_amount_E__________________________________________________2005-11-20_/
Pot_mineralizable_N_profile::Pot_mineralizable_N_profile
(const CORN::date32 &simdate_                                                    //170525
,nat8                   trans_sublayers // <- may be nitrogen specific }
,float64                control_transformation_adjustment_
#ifndef OLD_N_XFER
,Soil::Chemical_pool         &receiver_NH4_                                      //170502
#endif
,Soil::Soil_interface        &soil_                                              //060504
,Infiltration_model     infiltration_model                                       //080117
,Organic_matter_simple &organic_matter_pool_)                                    //980729
:Soil::Chemical_uptake_profile
   (simdate_
   ,std::string(LABEL_POT_MINERALIZABLE_N )
   ,CT_PMN
   ,ALWAYS_MOLECULAR,ALWAYS_MOLECULAR   /*Actually I think this is always elemental, I am not sure. */
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment_
   #else
   // V5 only uses new transformations
   #endif
   ,unused_amount_E
   #ifndef OLD_N_XFER
   ,&receiver_NH4_                                                               //170502
   #endif
         ,*(soil_.ref_layers())                                                  //150925
         ,*(soil_.ref_hydrology())                                               //160412_150925
         ,*(soil_.ref_hydraulic_properties())                                    //160412_150925
         ,*(soil_.ref_structure())                                               //160412_150925
   ,infiltration_model                                                           //080117
   , 0) // pot min N has no water table concentration curve                      //000504
,organic_matter_pool(organic_matter_pool_)                                       //980729
{  soil_layer_array64(amount_M); clear_layer_array64(amount_M);                  //080117
   organic_matter_pool.get_molecular_N(amount_M,trans_sublayers);                //080117
   mass_M->set_layers(amount_M);                                                 //080117
}
//_Pot_mineralizable_N_profile:constructor_______________________________1998?_/
float64 Pot_mineralizable_N_profile::transformation_rate
(float64 soil_temperature
,float64)  //unused water_content_correct given for virtual compatibility
{  float64 trans_rate = CORN::must_be_0_or_greater<float64>
      (
         #if (CROPSYST_VERSION==4)
         control_transformation_adjustment *
         #else
         // actually I think this entire class would be obsolete in V5
         #endif
            (seconds_per_day  * exp(17.753 - 6350.5 /
                (C_to_kelvin(soil_temperature))) / 604800.0));
   return trans_rate;
}
//_transformation_rate___________________________________________________1998?_/
float64 Pot_mineralizable_N_profile::moisture_function
(float64 water_filled_porosity
,float64 function_value_at_saturation_not_applicable)                            //060504
{  float64 fract_saturation = water_filled_porosity;                             //060629
   fract_saturation = CORN::must_be_between<float64>(fract_saturation ,0.0,1.0); //981212
   return (fract_saturation < 0.9)
   ? (1.111 * fract_saturation)
   : (10.0 - 10.0 * fract_saturation);
}
//_moisture_function_____________________________________________________1998?_/
void Pot_mineralizable_N_profile::transformation
(nat8 sublayer
,float64 sublayer_water_content
,float64 sublayer_saturation_water_content
,float64 sublayer_reference_water_content
,float64 sublayer_temperature
,float64 sublayer_bulk_density
,Seconds preferred_transformation_time_step_used_by_subclasses                   //060510
,float64 &transformed_to_M)                // returned
{  // Mineralization:
   soil_layer_array64(amount_M); clear_layer_array64(amount_M);                  //080117
   organic_matter_pool.extract_active_molecular_N(amount_M);                     //080117
   mass_M->set_layers(amount_M);                                                 //080117
   Soil::Chemical_pool::transformation
     (sublayer
      ,sublayer_water_content
      ,sublayer_saturation_water_content
      ,sublayer_reference_water_content
      ,sublayer_temperature
      ,sublayer_bulk_density
      ,86400                                                                     //060510
      #ifdef OLD_N_XFER
      ,transformed_to_M
      #endif
      );
   clear_layer_array64(amount_M);                                                //080117
   mass_M->get_layers(amount_M);                                                 //080117
   organic_matter_pool.intromit_active_molecular_N(amount_M);                    //980719
   mass_M->set_layers(amount_M);                                                 //080117
   //120731 check to make sure set_layers is calling commit_diffs();
   // We dont store PMN, it is always extracted from organic matter              //980719
   // So intromit clears the amount_M array
}
//_transformation________________________________________________________1998?_/
#endif

