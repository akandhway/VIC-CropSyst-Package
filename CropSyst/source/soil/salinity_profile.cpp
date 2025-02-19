#include "soil/chemical_mass_profile.h"
#include "soil/salinity_profile.h"
#include "soil/soil_I.h"
#include "soil/hydrology_I.h"
#include "watertab.h"
#include "corn/chronometry/date_time_64.h"
#include <math.h>
#ifdef CS_OBSERVATION
#include "csvc.h"
#include "CS_suite/observation/CS_inspector.h"
#include "CS_suite/observation/CS_optation.h"
#endif

namespace Soil {                                                                 //181206
//______________________________________________________________________________
Soil_salt::Soil_salt
(const CORN::date32 &today_                                                      //170525
,const Layers_interface               &soil_layers_                              //150925
,const Hydrology_interface            &soil_hydrology_                           //150925
,const Hydraulic_properties_interface &soil_hydraulic_properties_                //150925
,const Structure                      &soil_structure_                           //150925
,Infiltration_model                         infiltration_model_                  //080117
, soil_layer_array64                       (salt_concentration_dSm)              //051120
, Water_table_curve                        *water_table_curve // may be 0        //080213
, float64                                   ECw_to_TDS_soil)                     //080403
: Salt_interface()
, CS::Simulation_element_abstract(today_)                                        //170525
,salinity_profile
   (new
      Salinity_profile                                                           //080403
      (today_
      ,"salt",CT_SALT,ALWAYS_MOLECULAR,ALWAYS_MOLECULAR
      ,1.0                                                                       //981211
      ,salt_concentration_dSm
      ,soil_layers_
      ,soil_hydrology_
      ,soil_hydraulic_properties_
      ,soil_structure_
      ,infiltration_model_                                                       //080117
      ,water_table_curve                                                         //080213
      ,ECw_to_TDS_soil))                                                         //080403
, soil_layers              (soil_layers_)
, soil_hydrology           (soil_hydrology_)
, soil_hydraulic_properties(soil_hydraulic_properties_)
{  clear_layer_array64(osmotic_pot_solution);
   soil_layer_array64(mass_M); clear_layer_array64(mass_M);//kg chem / m^2 soil molecular
   for (nat8 sublayer = 1 ; sublayer <= soil_layers_.count(); sublayer++)
         mass_M[sublayer] =
                  (salinity_profile->salinity_ECw_dSm_input_output[sublayer]     //080402
                     * salinity_profile->uninundated_ECw_to_TDS)                 //080402
      * soil_hydrology_.
         get_liquid_water_content_volumetric(sublayer) // 070813 This was liquid, but I think it need to be total_WC get_liquid_water_content_volumetric(sublayer)
      * soil_layers_.get_thickness_m(sublayer);
   salinity_profile->mass_M->initialize(mass_M);                                 //080116
}
//_constructor______________________________________________________2005-11-18_/
float64 Soil_salt::get_salt_stress_function
(nat8  sublayer
,float64 osmotic_pot_50
,float64 salt_tolerance_P ) const
{  float64 salt_stress_function = 1.0;                                           //991005
   {  // https://books.google.com/books?id=iP7LWpn5oJ0C&pg=SL7-PA13&dq=van-genuchten+salinity+osmotic+potential&hl=en&sa=X&ved=0CCMQ6AEwAWoVChMInsWTndH1xwIVCUWICh0FYwCb#v=onepage&q=van-genuchten%20salinity%20osmotic%20potential&f=false
      float64 osmotic_pot_sat_extract =  calc_osmotic_pot_solution(sublayer)*    //061010
         soil_hydrology.get_liquid_water_content_volumetric(sublayer)            //070813
            // This is liquid but I think it needs to be total
         / (2.0 * soil_hydraulic_properties.get_field_capacity_volumetric(sublayer));//991005
      salt_stress_function = (osmotic_pot_50 == 0.0)                             //991005
      ? 1.0                                                                      //991005
      : (1.0 / (1.0 + pow((osmotic_pot_sat_extract/ osmotic_pot_50)
                         ,salt_tolerance_P)));
    }
   return salt_stress_function;
}
//_get_salt_stress_function_________________________________________1998-07-30_/
void Soil_salt::set_daily_irrig_salinity
(float64 salinity     // dS/m         ECw
,float64 ECw_to_total_dissolved_solids  // Typically 0.64 (used to be a constant)//080402
,float64 irrig_water_amount /* depth in meter? */
,Hour    event_start_hour                                                        //081212
,Hours   event_duration)
{  float64 applied_concentration = salinity * ECw_to_total_dissolved_solids;     //051129
      // dS/m -> kg salt / m� water  (same as g/l) was a constant 0.64

   // Note that this should only be called once a day                              051129
   // I think it may be possible to call this more than once a day
   // if multiple auto irrigations somehow get scheduled on this date.
   float64 applied_amount =  applied_concentration * irrig_water_amount;         //051129
      // Need to make sure that we account for this in balance.
   if (applied_amount > 0.0000001)                                               //060306
   {  CORN::Date_time_clad_64 start_time
         (simdate_raw/*today.get_date32()*/,(Seconds)(event_start_hour*3600));                  //170425
      CORN::Seconds operation_duration = event_duration *3600;                   //170425
      CORN::Seconds interval_duration = 3600;                                    //170425
      salinity_profile->apply_surface_mass                                       //170425
      (applied_amount
      ,start_time.get_datetime64()
      ,operation_duration
      ,interval_duration);
   }
}
//_set_daily_irrig_salinity________________________________________1987-0-7-03_/
float64 Soil_salt::calc_osmotic_pot_solution(nat8  layer)                  const
{  float64 osmotic_pot_solution_lyr =
      -36.0 * salinity_profile->salinity_ECw_dSm_input_output[layer];            //070802
// 080402 WARNING we should not use salinity_ECw_dSm_input_output because that is now really only for
// output instead, we should calculate from mass and probably should also consider water table
// but that may be a little over kill, since water table layer may already be accounted for.  Need to check
/*070802
   36.0 * ECw is  osmotic pressure (kPa)
   0.36 * ECw is  osmotic pressure (bar)
   Note that previously salinity was is expressed as Total Dissolved Solids (concentration)
   Thus the conversion Factor was  36.0/0.64
               (-36.0/0.64)  * salinity_profile->concentration_kg_m3H2O_only_for_salinity_output[layer]; //from Kg/m3 to dS/m}
*/
   return osmotic_pot_solution_lyr;
}
//_calc_osmotic_pot_solution___________________________________________________/
bool Soil_salt::start_day()                                        modification_
{
   for (Layer  lyr = 1; lyr <= soil_layers.count();lyr++)
      osmotic_pot_solution[lyr] = calc_osmotic_pot_solution(lyr);
   return true;                                                                  //140822
}
//_start_day________________________________________________________2006-10-09_/
bool Soil_salt::initialize()                                     initialization_
{  clear_sublayer_array(osmotic_pot_solution);
   return true;                                                                  //131217
}
//_initialize_______________________________________________________2005-11-18_/
Salinity_profile::Salinity_profile
(const CORN::date32                 &simdate_                                    //170523
,const std::string  &chem_name
,chemical_types      chemical_type_
,float64             molecular_to_elemental_
,float64             elemental_to_molecular_
,float64             control_transformation_adjustment_
,soil_layer_array64 (simulation_amount_E_)
,const Layers_interface               &soil_layers_                              //150925
,const Hydrology_interface            &soil_hydrology_                           //150925
,const Hydraulic_properties_interface &soil_hydraulic_properties_                //150925
,const Structure                      &soil_structure_                           //150925
,Infiltration_model infiltration_model                                           //080117
,Water_table_curve *water_table_curve                                            //000504
,float64 _uninundated_ECw_to_TDS)
:Chemical_profile
   (simdate_
   ,chem_name
   ,chemical_type_
   ,molecular_to_elemental_
   ,elemental_to_molecular_
   #if (CROPSYST_VERSION==4)
   ,control_transformation_adjustment_
   #endif
   ,simulation_amount_E_
   #ifndef OLD_N_XFER
   ,0 // no transformation so no receiver
   #endif
   ,soil_layers_
   ,soil_hydrology_
   ,soil_hydraulic_properties_
   ,soil_structure_
   ,infiltration_model                                                           //080117
   ,water_table_curve)                                                           //000504
, uninundated_ECw_to_TDS(_uninundated_ECw_to_TDS)
{  for (int sublayer_clear = 0; sublayer_clear <= MAX_soil_sublayers ; sublayer_clear++)  //981006
      salinity_ECw_dSm_input_output[sublayer_clear] = 0.0;                       //080402
   nat8  soil_num_sublayers = soil_layers.count();
   for (int layer = 1; layer <= soil_num_sublayers; layer ++)
   {  float64 amount_E_sl = simulation_amount_E_[layer];                         //020305
      salinity_ECw_dSm_input_output[layer] = amount_E_sl;                        //080402
            //             0.64 converts dS/m->kg salt/m3 water
   }
}
//_Salinity_profile____________________________________________________________/
void Salinity_profile::update_concentration_for_output
(nat8 num_layers_eff)
{  for (nat8 layer = 1; layer <= soil_layers.count(); layer++)                   //080207
   {  float64 thickness_layer  = soil_layers.get_thickness_m(layer);
         // should be 0 for    num_nodes_eff
      float64 WC_layer = soil_hydrology.get_liquid_water_content_volumetric(layer);
      float64 mass_layer =  mass_M->get_layer(layer);                            //090630
      float64 layer_concentration = mass_layer / (thickness_layer * WC_layer);   //080403_080207_060930
      salinity_ECw_dSm_input_output[layer] = layer_concentration / uninundated_ECw_to_TDS; // convert to decisiemins 080402
#ifdef NYI
//090923 080402 I am not sure if this will be needed
//090923 I think it will,
//090923 I need to get water_table_ECw_to_TDS from somewhere  (originally it is a scenario parameter,
//090923 but I think I will pass it to the water table.
                     if (layer is inundatated)
                        salinity_ECw_dSm_input_output[layer] = water_table_concentration / water_table_ECw_to_TDS;
#endif
   } // for layer
}
//_update_concentration_for_output__________________________________2008-04-03_/
void Salinity_profile::recalibrate
( soil_layer_array64(new_amount_E_by_layer)
, bool for_initialization)
{  nat8  soil_num_layers = soil_layers.count();
   soil_layer_array64(new_mass_layers); clear_layer_array64(new_mass_layers);    //080208
   for (int layer = 1; layer <= soil_num_layers; layer ++)
   {  float64 amount_E_sl = new_amount_E_by_layer[layer];                        //020305
      // Note that amount_E could either be mass or ECw depending on the context
      float64 new_mass_layer = 0.0;                                              //080208
         float64 new_conc_kg_salt_m3_water =                                     //071212
              amount_E_sl * uninundated_ECw_to_TDS;                              //080402
         salinity_ECw_dSm_input_output[layer] = amount_E_sl;                     //080207
         new_mass_layer                                                          //080208
            =  new_conc_kg_salt_m3_water                                         //080208
            *  soil_hydrology.get_liquid_water_content_volumetric(layer)         //080208
            *  soil_layers   .get_thickness_m(layer);                            //080208
      new_mass_layers[layer] = new_mass_layer;                                   //080206
   }
   salinity_ECw_dSm_input_output[soil_num_layers+1]
      = salinity_ECw_dSm_input_output[soil_num_layers];                          //080402
         // Not sure what this was for
   recalibrate_set_mass(new_mass_layers, for_initialization);                    //080402
}
//_recalibrate______________________________________________________2001-11-15_/
RENDER_INSPECTORS_DEFINITION(Soil_salt)
{
   #ifdef CS_OBSERVATION
   if (
      #if ((CS_VERSION > 0) &&(CS_VERSION < 6))
        (CS::optation_global.is_desired(CSVC_soil_base_salinity)) ||
      #endif
        (CS::optation_global.is_desired("salinity&profile/layers")))
      inspectors.append(new CS::Inspector_vector
      (salinity_profile->salinity_ECw_dSm_input_output,soil_layers.count()
      ,UC_dS_per_m,*context,"salinity&profile/layers",value_statistic
      ,CSVC_soil_base_salinity));
   #endif
   return 0;
}
// _RENDER_INSPECTORS_DEFINITION____________________________________2019-01-19_/
}//_namespace_Soil______________________________________________________________

