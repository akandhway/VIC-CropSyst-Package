#ifndef cs_chem_pot_mineral_N_V4H
#define cs_chem_pot_mineral_N_V4H

#ifdef NITROGEN
#include "soil/chemical_profile.h"
// 040519  Mineralization of organic matter is now processed by organic_matter_pools
class Organic_matter_simple;
//______________________________________________________________________________
class Pot_mineralizable_N_profile : public Soil::Chemical_uptake_profile
{private:
   Organic_matter_simple &organic_matter_pool;                                   //980729
      // The pot. min. N is derived from the organic matter pool
public:
   Pot_mineralizable_N_profile
      (const CORN::date32    &simdate_                                           //170525
      ,nat8                   trans_sublayers // <- may be nitrogen specific }
      ,float64                control_transformation_adjustment_
      #ifndef OLD_N_XFER
      ,Soil::Chemical_pool    &receiver_NH4_                                      //170502
      #endif
      ,Soil::Soil_interface   &soil_                                             //060504
      ,Infiltration_model      infiltration_model_                               //080117
      ,Organic_matter_simple  &organic_matter_pool_);                            //980729
   virtual float64 moisture_function
      (float64 water_filled_porosity                                             //060629
      ,float64 function_value_at_saturation_not_applicable);                     //060504
   virtual float64 transformation_rate
      (float64 soil_temperature
      ,float64 water_content_correct);
   virtual void transformation
      (nat8  sublayer
      ,float64 sublayer_water_content
      ,float64 sublayer_saturation_water_content
      ,float64 sublayer_reference_water_content
      ,float64 sublayer_temperature
      ,float64 sublayer_bulk_density
      ,Seconds preferred_transformation_time_step_used_by_subclasses             //060510
      ,float64 &transformed_to_M);
};
//_Pot_mineralizable_N_profile_________________________________________________/
#endif
#endif

