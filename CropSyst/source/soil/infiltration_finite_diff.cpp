#include "soil/infiltration_finite_diff.h"
#include "pond.h"
#include "watertab.h"
#include "soil/chemicals_profile.h"
#include "soil/dynamic_water_entering.h"
#include "soil/soil_base.h"
#include "runoff.h"
#include "common/physics/standard_constants.h"
#include "common/weather/parameter/WP_relative_humidity.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
using namespace std;
#endif
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif

#define match_VB_FD
#define maxints 10000
namespace Soil {                                                                 //181206
//______________________________________________________________________________
Infiltration_finite_difference
::Infiltration_finite_difference
(
#ifndef MICROBASIN_VERSION
Soil_base                           &soil_,                                      //001116
#endif
 /*const*/ Soil::Layers_interface   &soil_layers_                                //140529
,const Soil::Texture_interface      &soil_texture_                               //140509
,const Soil::Hydraulic_properties_interface &soil_hydraulic_properties_          //140423
,Soil::Evaporation_without_vapor_path &evaporator_  //NYI probably should be Evaporator_soil_FD     //160606
,CropSyst::Soil_runoff              *runoff_                                     //130308
,CropSyst::Pond                     &pond_                                       //071010
,Soil::Hydrology_finite_difference  &hydrology_                                  //060509
,const Relative_humidity            &relative_humidity_min_                      //151216
,bool                                numerical_runoff_mode_                      //140509
)
:Infiltration_abstract
   (soil_layers_                                                                 //140509
   ,soil_hydraulic_properties_                                                   //140423
   ,hydrology_
   ,runoff_,pond_)
,evaporator                                                        (evaporator_) //160606
#ifndef MICROBASIN_VERSION
,soil_base                                                               (soil_) //070119
#endif
,soil_texture                                                    (soil_texture_) //140509
,soil_layers                                                      (soil_layers_) //140509
,hydrology                                                          (hydrology_) //060509
,relative_humidity_min                                  (relative_humidity_min_) //151216
,daily_error                                                              ( 0.0)
,time_step                                                                (3600) //071010
#ifdef WATERTABLE
,water_table_depth_curve                                                     (0) //020620
#endif
,water_table_boundary_condition                                          (false) //041115
// Percent  (we added this to calculate atmospheric_water_potential).
// Previous we used a constant for atmospheric_water_potential.
,deferred_water_to_enter_soil_mm                                             (0) //071010
,numerical_runoff_mode                                  (numerical_runoff_mode_) //140509_071010
,runoff_this_interval                                                      (0.0) //071024
,cascade_drainage                                                          (0.0)
,number_of_real_plus_extended_layers                                         (0) //080211
,number_of_real_plus_extended_nodes                                          (0) //080211
,number_FD_solution_nodes                                                    (0) //080212
,number_FD_solution_layers                                                   (0) //080212
{  clear_layer_array64(water_potential);                                         //080125
}
//_Infiltration_finite_difference:constructor_______________________1998-07-30_/
float64 Infiltration_finite_difference::typical_water_potential_below_rootzone() const
{  static const float64 slightly_below_field_capacity_term = 6.0;   //   (11.401 instead of 6.0 would be about field capacity)
   float64 clay_percent =
      soil_texture.get_clay_percent(soil_layers.count());
   float64 water_pot_below_root_zone                                 //     Expected_Lower_Boundary_Water_Potential = -14.219 * Log(Soil.PercentClay(nn)) + 6
      = -14.219 * CORN_ln(clay_percent) + slightly_below_field_capacity_term;
    // natural logrithm
   //This will give a value be slightly less than field capacity (between -50 and - 30.
   return water_pot_below_root_zone;
}
//_typical_water_potential_below_rootzone___________________________2008-03-25_/
bool Infiltration_finite_difference::infiltrate
(float64                         water_entering_soil_not_irrigation              //070109
,float64                          management_irrigation_daily_unused
,float64                          management_auto_irrigation_concentration
,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm          //990204
,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm          //070119
   // optional (may be 0)
,const Dynamic_float_array       &runon_in_intervals                             //990409
,Dynamic_float_array             &runoff_in_intervals                            //990409
)
{
#ifdef WATERTABLE
   if (water_table_depth_curve)                                                  //071023
   {  water_table_limitations();
   } else
#endif
   {  water_table_boundary_condition = false;                                                                                 //981216
      number_FD_solution_layers           = number_of_real_plus_extended_layers;                                              //080212
      number_FD_solution_nodes            = number_FD_solution_layers+1;                                                      //080212
      hydrology.update_soil_volume
       (number_of_real_plus_extended_layers,number_of_real_plus_extended_nodes); //080131RLN
   }                                                                                                                         //071214
   Infiltration_abstract::infiltrate
      (water_entering_soil_not_irrigation
      ,management_irrigation_daily_unused
      ,management_auto_irrigation_concentration
      ,non_runoff_water_entering_in_intervals_mm
      ,irrigation_water_entering_in_intervals_mm
      ,runon_in_intervals
      ,runoff_in_intervals);                                                     //070118
   return infiltration_finite_difference
      (management_irrigation_daily_unused                                        //070107
      ,non_runoff_water_entering_in_intervals_mm                                 //990402
      ,irrigation_water_entering_in_intervals_mm                                 //070119
      ,runon_in_intervals                                                        //990409
      ,runoff_in_intervals );                                                    //990409
}
//_infiltrate__________________________________________________________________/
#ifdef WATERTABLE
void Infiltration_finite_difference::set_water_table
(const char *water_table_filename
,const CORN::date32 &today                                                       //170525_151128
,const CORN::Date_const & first_date
,const CORN::Date_const & last_date)
{  water_table_depth_curve    = new Water_table_curve
      (water_table_filename,"depth"
      ,today,first_date,last_date,100.0);                                        //131004
}
//_set_water_table__________________________________________________2002-06-20_/
void Infiltration_finite_difference::water_table_limitations() modification_
{  static nat8 uninundated_layer_count_prev = 255;                             //160830
   water_table_depth
      = water_table_depth_curve                                                  //020620
      ? water_table_depth_curve->get_for_today()
      : 100.0;
      // If no water table depth observations, then presume a deep watertable.   //020620
   nat8  top_layer_in_watertable = soil_layers.get_layer_at_depth_or_last_layer
      (water_table_depth);                                                       //080131
   nat8  number_uninundated_layers = top_layer_in_watertable - 1;                //080211
   bool change_in_inundation =  number_uninundated_layers != uninundated_layer_count_prev; //080213
   water_table_input = 0.0;                                                      //000504
   // Applying water table conditions and bottom boundary conditions if the
  // water table rises up.This is the default option
   //--- when the water table is too deep, out of the profile
   //free drainage if water table is under the bottom layer
   if (water_table_depth <= soil_layers.get_depth_profile_m())
   {  // then there is a water table
      water_table_boundary_condition = true;                                     //981216
      number_FD_solution_layers           = number_uninundated_layers;           //080212
      number_FD_solution_nodes            = number_FD_solution_layers+1;         //080212
      for (nat8  node = number_uninundated_layers +1; node <= number_of_real_plus_extended_nodes ; node++) //080219
         // Claudio THIS IS THE WRONG WAY TO SETUP THIS LOOP, BUT IT MATCHES TO V.B. CODE!!!!!
      {  nat8  layer = node;                                                     //990312
         water_potential[node]
            = hydrology.get_air_entry_pot(layer,true,false);
         float64 sat_water_content =  soil_hydraulic_properties.get_saturation_water_content_volumetric //071217
            (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);                  //140822
         hydrology.set_water_content_volumetric_layer(layer,sat_water_content);  //990311
     }
   } else //for the layers under the water table:
   { // Free drainage
      water_table_boundary_condition = false;                                    //981216
      number_uninundated_layers = soil_layers.count();                           //080211
      number_of_real_plus_extended_layers =  number_uninundated_layers + 2;      //080211
      number_of_real_plus_extended_nodes = number_of_real_plus_extended_layers+1;//080211
      number_FD_solution_layers           = number_of_real_plus_extended_layers; //080212
      number_FD_solution_nodes            = number_FD_solution_layers+1;         //080212
      water_potential[number_of_real_plus_extended_nodes +1] //  was   p(nn + 1) = p(nn)
         = typical_water_potential_below_rootzone();                             //080325
      // below root zone soil tends to stay wet (near field capacity or slightly lower)
   }
   soil_layers.set_number_uninundated_layers(number_uninundated_layers );        //080211
   if (change_in_inundation)                                                     //080201
   {  hydrology.update_soil_volume(number_of_real_plus_extended_layers,number_of_real_plus_extended_nodes); //080131RLN
      uninundated_layer_count_prev = number_uninundated_layers;                  //080213
      if (chemicals)                                                             //071214
         chemicals->adjust_water_table_concentration                             //150925
         (number_FD_solution_layers                                              //080220
         ,number_of_real_plus_extended_nodes);                                   //080220
   }
}
#endif
//_water_table_limitations_____________________________________________________/
float64 Infiltration_finite_difference::Mass_balance_iteration::element_flux
(nat8     i    // I believe this is node
,float64 &djdpu
,float64 &djdpl
,float64 &unused_ku)                                                       const //990220
{ // i is element number; upper p is i, lower is i+1
   float64 kbar;
   float64 dkdpu;
   float64 dkdpl;
   float64 phiu;
   float64 phil;
   float64 ku = 0;                                                               //090504
   float64 kl;   //  lower_conduct
   float64 ks_i = soil_hydraulic_properties.get_sat_hydraul_cond_kg_s_m3(i);
   float64 air_entry_pot_i = hydrology.get_air_entry_pot(i,false,false);         //990315
   float64 n_i = soil_hydraulic_properties.get_unsaturated_conductivity_function_slope(i);   //041115
   // Need to avoid div 0:                                                       //991107
   float64 water_pot_i = water_pot[i];                                           //991107
   if (CORN::is_zero<float64>(water_pot_i/*200127,0.0*/,0.000000001))            //991107
      // These are optimizations are not in the V.B. code
      water_pot_i = -0.000000001;                                                //991107
//   float64 water_pot_ip1 = (i == number_of_real_plus_extended_nodes) ? water_pot[i] : water_pot[i+1]; //140604
   float64 water_pot_ip1 = water_pot[i+1];                                       //140604
      // _ip1 stands for i+1                                                     //991107
   if (CORN::is_zero<float64>(water_pot_ip1,/*200127 0.0,*/0.000000001))         //991107
      water_pot_ip1 = water_pot[i] ; // -0.000000001;                            //991107
//cout << (int) i << '\t' <<  soil_vapor_conductance_i << endl;
   if (water_pot_ip1 < air_entry_pot_i)                                          // If p(i + 1) < pe(i) Then
   {  kl = ks_i * pow((air_entry_pot_i / water_pot_ip1) , n_i);                  //        kl = ks(i) * (pe(i) / p(i + 1)) ^ n(i)
      //090504obs kl *= soil_vapor_conductance_i / (kl + soil_vapor_conductance_i); //        kl = (kl * Soil_Vapor_Conductance(i)) / (kl + Soil_Vapor_Conductance(i))
      phil = kl * water_pot_ip1 / (1 - n_i);                                     //    phil = kl * p(i + 1) / (1 - n(i))
   } else                                                                        //  Else
   {  kl = ks_i;                                                                 //        kl = ks(i)
      //090504obs kl *= soil_vapor_conductance_i / (kl + soil_vapor_conductance_i); //        kl = (kl * Soil_Vapor_Conductance(i)) / (kl + Soil_Vapor_Conductance(i))
      phil = kl * (air_entry_pot_i * n_i / (1 - n_i) + water_pot_ip1);           //    phil = kl * (pe(i) * n(i) / (1 - n(i)) + p(i + 1))    //990311
   }                                                                             //  End If
   if (water_pot_i < air_entry_pot_i)                                            //  If p(i) < pe(i) Then
   {  ku = ks_i * pow((air_entry_pot_i / water_pot_i) , n_i);                    //        ku = ks(i) * (pe(i) / p(i)) ^ n(i)
      //090504obs ku *= soil_vapor_conductance_i / (ku + soil_vapor_conductance_i); //        ku = (ku * Soil_Vapor_Conductance(i)) / (ku + Soil_Vapor_Conductance(i))
      phiu = ku * water_pot_i / (1 - n_i);                                       //    phiu = ku * p(i) / (1 - n(i))
   } else                                                                        //  Else
   {  ku = ks_i;                                                                 //        ku = ks(i)
      //090504obs ku *= soil_vapor_conductance_i / (ku + soil_vapor_conductance_i); //        ku = (ku * Soil_Vapor_Conductance(i)) / (ku + Soil_Vapor_Conductance(i))
      phiu = ku * (air_entry_pot_i * n_i / (1 - n_i) + water_pot_i);             //    phiu = ku * (pe(i) * n(i) / (1 - n(i)) + p(i))  //990312
   }                                                                             //  End If
   if (CORN::is_approximately<float64>(water_pot_i,water_pot_ip1, 0.1 ))         //  If Abs(p(i) - p(i + 1)) < 0.1 Then
   {  // the water potentials are essentially equal
      kbar = 0.5 * (ku + kl);                                                    //    kbar = 0.5 * (ku + kl)
      dkdpu = -n_i * ku /water_pot_i;                                            //    dkdpu = -n(i) * ku / p(i)
      if (dkdpu == 0.0) dkdpu =  1E-20; // Cannot be 0.0                         //    If dkdpu = 0 Then dkdpu = 1E-20  //051128
      dkdpl = -n_i * kl / water_pot_ip1;                                         //    dkdpl = -n(i) * kl / p(i + 1)
   } else                                                                        //  Else
   {  kbar = (phiu - phil) / (water_pot_i - water_pot_ip1);                      //    kbar = (phiu - phil) / (p(i) - p(i + 1))
      dkdpu = (kbar - ku) / (water_pot_ip1 - water_pot_i);                       //    dkdpu = (kbar - ku) / (p(i + 1) - p(i))
      dkdpl = (kl - kbar) / (water_pot_ip1 - water_pot_i);                       //    dkdpl = (kl - kbar) / (p(i + 1) - p(i))
      if (dkdpl == 0.0) dkdpl =  1E-20; // cannot be 0.0                         //    If dkdpl = 0 Then dkdpl = 1E-20     //051128
   }                                                                             //  End If
   float64 node_thickness_i = node_depth[i+1] - node_depth[i];                   // 080215
      //soil_layers.get_thickness_m(i);
   float64 elementflux =                                                         //  elementflux = (phil - phiu) / (z(i + 1) - z(i)) - Gr * kbar   //080215
         (phil - phiu) / node_thickness_i
         - standard_gravity * kbar;
   djdpl = kl / node_thickness_i + standard_gravity * dkdpl;                    //  djdpl = kl / (z(i + 1) - z(i)) + Gr * dkdpl
   djdpu = ku / node_thickness_i + standard_gravity * dkdpu;                    //  djdpu = ku / (z(i + 1) - z(i)) + Gr * dkdpu
   return  elementflux;
}  //090106 Checked against V.B. version (Infiltration_Clean_010209)    End Function
//_element_flux________________________________________________________________/
float64 Infiltration_finite_difference
::calc_atmospheric_water_potential  // appears to be kg.s/m^4
(float64 relative_humidity_TS_or_daily_min_percent)                        const
{  //    prefer relative humidity during the time step, but the daily minimum would suffice
   // Sanity check
   float64 min_relative_humidity = relative_humidity_TS_or_daily_min_percent;
   if (CORN::is_approximately<float64>(min_relative_humidity,100.0,0.0001)) // Not sure why this is needed now but sometimes estimate RHmin is giving 100.0
             min_relative_humidity = 0.99;
   float64 relative_humidity_TS_or_daily_min                                           // Hourly_Relative_Humidity = 0.5 'Assume 50% RHmin; actual value probably not needed
      = min_relative_humidity / 100.0;  // The V.B. version uses this constant relative humidity, C++ we use the actual
   float64 atmospheric_water_pot  =                                                    // Atmospheric_Water_Potential = 137000# * (Hourly_Relative_Humidity - 1)
      137000.0   // This is an average value for 25'C probably should use hourly temperature Claudio has the equation.
      * (relative_humidity_TS_or_daily_min - 1.0);
   return atmospheric_water_pot;
}
//_calc_atmospheric_water_potential_________________________________2007-10-10_/
Infiltration_finite_difference::Mass_balance_iteration
::Mass_balance_iteration
(Infiltration_finite_difference ::FD_status      status_
, float64                                        time_step_  //number of seconds in this timestep
,const Soil::Layers_interface                   &soil_layers_
,const Soil::Hydrology_finite_difference        &hydrology_
,const Soil::Hydraulic_properties_interface     &hydraulic_properties_
,float64                                         atmospheric_water_potential_
,Soil::Evaporation_without_vapor_path           &evaporation_         // reference to Infiltration_finite_difference /*181206 Soil_infiltration_finite_difference*/ //080125
,float64                                        *water_pot_committed_            //080215
,float64                                        *node_depth_
,float64   typical_water_potential_below_rootzone_)
: status                      (status_)                                          //990220
, soil_layers                 (soil_layers_)
, soil_hydraulic_properties   (hydraulic_properties_)
, hydrology                   (hydrology_)
, finite_diff_error           (0.0)
, actually_infiltrated_m_ts   (0.0)    //  m returned
, atmospheric_water_potential (atmospheric_water_potential_)
, evaporation                 (evaporation_)                                     //090107
, water_pot_committed         (water_pot_committed_)                             //080125
, time_step                   (time_step_)
, node_depth                  (node_depth_)                                      //080215
, typical_water_potential_below_rootzone(typical_water_potential_below_rootzone_)
{
   for (nat8  layer = 0; layer < MAX_soil_layers_alloc ; layer++)
   {  new_up_WC[layer]       = hydrology.get_upper_water_content_liquid(layer);  //080213
      new_low_WC[layer]      = hydrology.get_lower_water_content_liquid(layer);
      water_pot[layer] = water_pot_committed[layer];                             //071010
   }
   if (status != FLUX_BOUNDARY)
   {  water_pot[1] = hydrology_.get_air_entry_pot(1,true,false);
     // this introduces an error = water content difference at original water pot and and air entry pot //990215
   }
   clear_sublayer_array(water_flux_mm);                                          //060504
}
//_Mass_balance_iteration:constructor_______________________________2007-10-10_/
Infiltration_finite_difference::FD_status
Infiltration_finite_difference::Mass_balance_iteration
::resolve
( Hour hour                                                                      //051130
, nat16 max_iterations
, float64 water_entering_soil_m_ts   // Note these value are all per time step
, bool water_table_boundary_condition                                            //071010
, nat8  number_FD_solution_layers                                                //080211
, nat8  number_FD_solution_nodes                                                 //080131
, float64 &flux_term_driving_drainage_m_ts)                                const //080227
{
   Infiltration_finite_difference::FD_status mode = status;                      //080122
   float64 water_flux_entering_soil_mm  = // this looks to be mm/sec             //990215
      (-m_to_mm(water_entering_soil_m_ts) / time_step);                          //990215
   soil_sublayer_array_64(phil)      ;// lower matric flux potential J*sec/m3
   soil_sublayer_array_64(phiu)      ;// lower derivative of conductivity dk/dpot
   soil_sublayer_array_64(cpl)       ;// upper matric flux potential J*sec/m3
   soil_sublayer_array_64(cpu)       ;// upper coef. newton raphson system
   soil_sublayer_array_64(dkdpu)     ;// upper derivative of conductivity dk/dpot
   soil_sublayer_array_64(dkdpl)     ;// lower coef. newton raphson system
   soil_sublayer_array_64(kv);
   soil_sublayer_array_64(bx)        ;// Main diagonal of tridiag matrix. used in system of equation (NR) for finite diff.
   soil_node_array64     (f)         ;// mass balance error in system of equations for finite diff.
   soil_sublayer_array_64(a)         ;// Lower diagonal of tridiag matrix. used for zero init. also, used in system of equation for finite diff.
   soil_sublayer_array_64(c)         ;// upper diagonal of tridiag matrix. used in system of equation (NR) for finite diff.
   soil_sublayer_array_64(dp);
   for (nat8  layer = 0; layer <= MAX_soil_sublayers; layer++)
   {  phil[layer]    = 0.0;
      phiu[layer]    = 0.0;
      cpl[layer]     = 0.0;
      cpu[layer]     = 0.0;
      dkdpu[layer]   = 0.0;
      dkdpl[layer]   = 0.0;
      kv[layer]      = 0.0;
      bx[layer]      = 0.0;
      f[layer]       = 0.0;
      a[layer]       = 0.0;
      c[layer]       = 0.0;
      dp[layer]      = 0.0;
   }
   // saturation vapor concentration at 20 C, kg/m3
   float64 air_entry_pot_1 = hydrology.get_air_entry_pot(1,true,false);          //990321
   // The following is already setup in Mass_balance_iteration
   // and or passed as below.
   //if (mode == SATURATED_BOUNDARY)                                             //  If Sat_BC Then      as of 090106 This V.B. code appears obsolete
   //{  water_pot[1] = air_entry_pot_1;                                          //    p(1) = pe(1)
   //   water_flux_mm[0] = 0;                                                    //    jl(0) = 0
   //} else                                                                      //  Else
   //{  water_flux_mm[0] = water_flux_entering_soil_mm;                          //    jl(0) = -flux
   //}                                                                           //  End If
                                                                                 // Actual_Evaporation = 0
   /* Evaporation is performed by caller. */                                     // Call CalculateSoilWaterEvaporation(Flux, Actual_Evaporation, Atmospheric_Water_Potential, dt, Hour, Soil, Weather)
   water_flux_mm[0] = water_flux_entering_soil_mm;                               //    jl(0) = -flux   'Flux downward is negative by convention
   soil_sublayer_array_64(djdpl); clear_layer_array64(djdpl);
   soil_sublayer_array_64(djdpu); clear_layer_array64(djdpu);
   // This is an attempt to Avoid problems if soil is very dry:
   //170217unused bool dry_boundary = false;                                                                                                                                                    //990220
   int probably_wont_converge = 0;                                                                                                                                               //990304
   // Number of nodes and water potential setup in water table limitations                                                                                                       //080131
                                                                                 //'  nn = m '+ 1 'Number of nodes (nn) is equal to number of effective soil layers + 1                      //080122
   // In the V.B. code WC_init (used to calculate drainage) is calculated here, in C++ this is calculated by the caller.                                                         //080227
   // unused float64  temporary_flux = 0                                         //  temporary_flux = 0                                                                                      //080118
   int iteration_no = 0;                                                         //  nits = 0                                                                                                //960119
   float64 max_mass_balance_error =0.000001;      //080215 RLN                   //  im = 0.000001                                  'maximum allowable mass balance error                    //990220
   // Evaporation is calculated outside                                                                                                                                          //080131
   // if (mode == saturated boundary)                                            // If Sat_BC Then                                                                                           //080118
   //    max_act_evaporation_mm_ts is 0                                          //    Actual_Evaporation = 0                                                                                //080118
   // otherwise                                                                  //    Else                                                                                                  //080118
   //    The actual evaporation is calculated in caller                          //    Actual_Evaporation = ActualEvaporation(dt, Hour)//    Actual_Evaporation = ActualEvaporation(dt, Hour)//080118
   //                                                                            // End If                                                                                                   //080118

   // V.B. code has this but Claudio said it is not really needed.
   for (nat8  node = 1; node <= number_FD_solution_nodes; node++)                            //080215
   {  nat8  layer = node;                                                                    //080215
      float64 wu =hydrology.get_upper_water_content_liquid(layer) ;                          //080215
      if ((layer > 1) || (mode == FLUX_BOUNDARY))                                            //090505
         water_pot[layer] = hydrology.calc_water_potential_with_respect_to_layer(wu,layer);  //090107
   }                                                                                        //080215
   float64 se;                                                                               //080118
   do                                                                            // Do 'Begin to iterate and find a solution at given time step
   {  iteration_no++;                                                            // nits = nits + 1
      actually_infiltrated_m_ts = 0.0;                                           //990215
      se = 0;                                                                    // se = 0   //990220
      finite_diff_error = 0;                                                     // fde = 0  //991028
      for (nat8  i = 1; i <= number_FD_solution_nodes ; i++)                     //    For i = 1 To Number_FD_Solution_Nodes
      {  nat8  layer = i;
         nat8  node  = i;
         float64 unused_upper_conduct_sl =0.0;
         water_flux_mm[i]
            = element_flux(i, djdpu[i], djdpl[i],unused_upper_conduct_sl);       //      jl(i) = elementflux(i, djdpu(i), djdpl(i))

         if (   (mode == FLUX_BOUNDARY)                                          //090504
             && CORN::is_zero<float64>(water_entering_soil_m_ts,0.00001))
         {  // (Do not apply this in saturated boundary mode)
            if (  (water_pot[node] < -40.0)    // if is just below field capacity
               && (water_flux_mm[node] > 0)  // and flux is positive (upward)
               && (node <= 2)              // in the case of the evaporative layer involved node fluxes.
               )
            {  water_flux_mm[node] = 0.0;  // No upflow
               djdpu [node] = 0.0;
               djdpl [node] = 0.0;
         }  }
         float64 dwdpu;                                                          //990219
         new_up_WC[layer]
            = calc_water_content(i,water_pot[node], dwdpu);                      //      wnu(i) = watercontent(i, p(i), dwdpu)
         float64 dwdpl;                                                          //990219
         new_low_WC[layer]
            = calc_water_content(i,water_pot[node+1],dwdpl);                     //      wnl(i) = watercontent(i, p(i + 1), dwdpl)
                                                                                 //LML 14/06/02 need check water_pot[number_FD_solution_nodes+1]

         float64 v_i = hydrology.get_soil_volume(node);                          //080130
         cpl[layer] = v_i  * dwdpl / (2.0 * time_step);                          //      cpl(i) = v(i) * dwdpl / (2 * dt)
         cpu[layer] = v_i  * dwdpu / (2.0 * time_step);                          //      cpu(i) = v(i) * dwdpu / (2 * dt)
         a[i] = -djdpu[i - 1];                                                   //      A(i) = -djdpu(i - 1)
         c[i] = -djdpl[i];                                                       //      C(i) = -djdpl(i)
         bx[i] = djdpl[i - 1] + djdpu[i] + cpu[i] + cpl[i - 1];                  //     bx(i) = djdpl(i - 1) + djdpu(i) + cpu(i) + cpl(i - 1)
         float64 denominator = (i == 1) ? time_step : (2.0 *time_step);                      //071012
         float64 upper_WC_layer = hydrology.get_upper_water_content_liquid(i);   //071012
         float64 lower_WC_layer_above = hydrology.get_lower_water_content_liquid(i - 1);    //LML 14/06/02 need check get_lower_water_content_liquid(0)
         if(fabs(lower_WC_layer_above) > 1.0e8)
             lower_WC_layer_above = lower_WC_layer_above;
         //LML 14/06/02 initialize node 0
         if(i == 1){
             new_low_WC[i-1] = 0;
             lower_WC_layer_above = 0;
         }
         f[i] =                                                                  //       If (i = 1) Then
            +    water_flux_mm[i - 1]                                            //        f(i) = jl(i - 1) - jl(i) + v(i) * (wnu(i) - wu(i) + wnl(i - 1) - wl(i - 1)) / (dt)
            -    water_flux_mm[i]                                                //        Else
            + v_i * (new_up_WC[i] - upper_WC_layer                               //        f(i) = jl(i - 1) - jl(i) + v(i) * (wnu(i) - wu(i) + wnl(i - 1) - wl(i - 1)) / (2 * dt)
                        +  new_low_WC[i - 1] - lower_WC_layer_above)             //      End If
               /(denominator);
         se = se + fabs(f[i]);                                                   //      se = se + Abs(f(i))
         // Unlike VB the fde is reported in meters                              //      fde = fde + f(i)
         finite_diff_error += mm_to_m(f[i])                                      //990221
                *soil_layers.get_thickness_m(i) * time_step; // RLN If I recall correctly, adjusted the error with respect to soil layer thickness.

      }                                                                          // Next i
      if ((status == SATURATED_BOUNDARY))                                        //    If Sat_BC Then //990220
      {  se -= fabs(f[1]);                                                       //       se = se - Abs(f(1)):
         // Unlike VB the fde is reported in meters
         finite_diff_error -=
            mm_to_m(f[1])*soil_layers.get_thickness_m(1) * time_step;            //       fde = fde - f(1)        // C++ fde is different units //990221
          f[1] = 0;                                                              //      f(1) = 0
          c[1] = 0;                                                              //      C(1) = 0
      }                                                                          //    End If
      for (nat8  i_down = 1; i_down <= number_FD_solution_nodes - 1; i_down++)   //For i = 1 To nn - 1  //991028
      {  c[i_down] /= bx[i_down];                                                //      C(i) = C(i) / bx(i)
         f[i_down] /= bx[i_down];                                                //      f(i) = f(i) / bx(i)
         bx[i_down + 1] -=  a[i_down + 1] * c[i_down];                           //      bx(i + 1) = bx(i + 1) - A(i + 1) * C(i)          //990302
         bx[i_down + 1] =                                                        //      If bx(i + 1) < 0.00001 Then bx(i + 1) = 0.00001  //051209
            CORN::must_be_greater_or_equal_to<float64>(bx[i_down + 1],0.00001);                                                         //051209
         f[i_down + 1] -= a[i_down + 1] * f[i_down];                             //      f(i + 1) = f(i + 1) - A(i + 1) * f(i)
      }                                                                          //    Next i
      dp[number_FD_solution_nodes]                                               //991028
         = f[number_FD_solution_nodes] / bx[number_FD_solution_nodes];           //dp(nn) = f(nn) / bx(nn)
      water_pot[number_FD_solution_nodes] -= dp[number_FD_solution_nodes] ;      //    p(nn) = p(nn) - dp(nn)
//080325 Now this is set to a value and never changes
//080325/*080131_*/       if (!water_table_boundary_condition)                   //    If Not WaterTable Then
//080325/*080131_*/          water_pot[number_FD_solution_nodes+1] = water_pot[number_FD_solution_nodes];      //    If Not WaterTable Then p(nn + 1) = p(nn) 'Roger
      for (nat8  i_up = number_FD_solution_nodes - 1                             //991028
          ; i_up >= 1; i_up--)                                                   //    For i = nn - 1 To 1 Step -1
      {  dp[i_up] = f[i_up] - c[i_up] * dp[i_up + 1];                            //      dp(i) = f(i) - C(i) * dp(i + 1)
         float64 lim = 0.8 * fabs(water_pot[i_up]) ;                             //      lim = 0.8 * Abs(p(i))
         float64 abv = fabs(dp[i_up]);                                           //      abv = Abs(dp(i)):
         if (abv > lim)                                                          //      If abv > lim Then dp(i) = lim * dp(i) / abv
             dp[i_up] = lim * dp[i_up] / abv;
         water_pot[i_up] -= dp[i_up];                                            //      p(i) = p(i) - dp(i)
         float64  air_entry_pot_div_10000
            =  (hydrology.get_air_entry_pot(i_up,true,false) / 10000.0);         //080118
         if (water_pot[i_up] > air_entry_pot_div_10000)                          //      If p(i) > (pe(i) / 10000) Then p(i) = pe(i) / 10000 'NEW CONDITION //010111
             water_pot[i_up] = air_entry_pot_div_10000;
      }                                                                          //    Next i
      float64 water_potential_limit = atmospheric_water_potential;                     //    If p(1) < Atmospheric_Water_Potential Then p(1) = Atmospheric_Water_Potential //051118
      if (water_pot[1] < water_potential_limit)                                  // 90000 May need to be AirDryWater potential?                                                        //990220
          water_pot[1] = water_potential_limit;                                  // dry_boundary condition                                                                            //990220
      float64 air_entry_pot_num_node_eff = hydrology.get_air_entry_pot(number_FD_solution_nodes,true,false);                                             //080118
      /* WARNING THIS V.B. line was not in c++ on 20080106!!!*/      //    If Not Soil.WaterTableBoundary Then p(nn + 1) = Minimum(p(nn), Expected_Lower_Boundary_Water_Potential)
      if (!water_table_boundary_condition) water_pot[number_FD_solution_nodes+1 ] =
         std::min<float64>(water_pot[number_FD_solution_nodes ],typical_water_potential_below_rootzone);
      // water pot can never be
      if (water_pot[number_FD_solution_nodes ] > air_entry_pot_num_node_eff)     //    If p(nn) > pe(nn) Then p(nn) = pe(nn)  //080118
          water_pot[number_FD_solution_nodes ] = air_entry_pot_num_node_eff;                                               //080118
     probably_wont_converge +=                                                                                             //990313
         (water_pot[1] > air_entry_pot_1 );
      if ((mode != SATURATED_BOUNDARY) && probably_wont_converge)                //     If Not Sat_BC And pe(1) < p(1) Then                     //080214
      {  if ((se < max_mass_balance_error) || (iteration_no > 50))               //        If (se < im) Or (nits > 50) Then nits = maxits + 1   //080214
            status = MAXED_OUT_ITERATIONS;                                                                                          //______
      } else                                                                     //    End If                                                   //080214
      {                                                                                                                            //990302
        if (iteration_no >= 200) max_mass_balance_error = 0.00001;               //    If nits > 200 Then im = 0.00001                          //990303
        if (iteration_no >= 300) max_mass_balance_error = 0.0001;                //    If nits > 300 Then im = 0.0001                           //990303
        if (iteration_no >= 400) max_mass_balance_error = 0.0005;                //    If nits > 400 Then im = 0.0005                           //990303
      }                                                                                                                            //990302
      /*This stop statement in the V.B. code is comment*/                        //    If nits = 2000 Then Stop
      if (iteration_no > max_iterations)                                                                                           //990219
          status = MAXED_OUT_ITERATIONS;                                                                                                              //990219
   } while                                                                       //   Loop Until                                                                  //______
      (!((((se < max_mass_balance_error)                                          //    (((se < im)                                                                //080121
        && (air_entry_pot_1 >= water_pot[1]))                                     //       And (pe(1) >= p(1)))                                                    //080121
       || (status == MAXED_OUT_ITERATIONS))));                                   //  Or (nits > maxits)) 'End of iteration to find a solution at given time step  //080121
   if (status != MAXED_OUT_ITERATIONS)                                           //  If nits <= maxits Then                                                       //080118
   {  // set up for successful completion of iterative solution                                                                                       //080118
      status = CONVERGED;                                                        //    Success = True                                                             //______
      // C++ the evaporation is summed outside this function.                    //    SumEvap = SumEvap + Actual_Evaporation 'in mm/hr                           //______
      // Set new water content at solution water potentials                                                                                           //080121
      for (int8 layer = 1; layer <= number_FD_solution_layers+1; layer++)        //     For i = 1 To m + 1                                                   //080206
      {  float64 dwdpu;                                                                                                                               //051209
         new_up_WC[layer] = calc_water_content(layer,water_pot[layer], dwdpu);   //       wnu(i) = watercontent(i, p(i), dwdpu)                       //051209
         float64 dwdpl;                                                                                                                               //051209
         new_low_WC[layer] = calc_water_content(layer,water_pot[layer+1],dwdpl); //       wnl(i) = watercontent(i, p(i + 1), dwdpl)                   //051209
                                                                                 // V.B. code had the following
         // in the C++ code this is handled outside this method                  //        Old_Upper_Water_Content(i) = wu(i)
                                                                                 //        Old_Lower_Water_Content(i) = wl(i)
                                                                                 //        New_Upper_Water_Content(i) = wnu(i)
                                                                                 //        New_Lower_Water_Content(i) = wnl(i)
      }
#ifdef match_VB_FD
                                                                                 //'Needed for chemical transport
      // In C++ code this is handled by caller                                   //    'set water content above first soil layer (an abstraction) to zero
                                                                                 //    Old_Upper_Water_Content(0) = 0
                                                                                 //    Old_Lower_Water_Content(0) = 0
                                                                                 //    New_Upper_Water_Content(0) = 0
                                                                                 //    New_Lower_Water_Content(0) = 0
                                                                                 //   'set water content below last soil layer (an abstraction) to zero
                                                                                 //    Old_Upper_Water_Content(m + 1) = 0
                                                                                 //    Old_Lower_Water_Content(m + 1) = 0
                                                                                 //    New_Upper_Wvaater_Content(m + 1) = 0
                                                                                 //    New_Lower_Water_Content(m + 1) = 0
                                                                                 //'End of needed for chemical transport
      // C++ sets up flux for next iteration in the main loop                    //    If Sat_BC Then
                                                                                 //        flux = (-jl(1) + v(1) * (wnu(1) - wu(1)) / dt)
                                                                                 //    Else
                                                                                 //        flux = (-jl(0))
                                                                                 //    End If
#endif
      // In VB, the drainage calculation is done here                                                             //080201
      // part of drainage calculation is moved to caller, part is moved below (flux_term_driving_drainage_m_ts)   //080201
      // In VB, water contents are updated here, in C++ this is done by caller using commit()                     //080201
      float64 water_flux_1    = water_flux_mm[1];                                //060504
      float64 soil_volume_1   = hydrology.get_soil_volume(1);                    //080627
      float64 new_up_WC_1 = new_up_WC[1];                                        //080122
      float64 old_up_WC_1 = hydrology.get_upper_water_content_liquid(1);                                          //080122
      if (!((status == MAXED_OUT_ITERATIONS) || (status == TURNING_INTO_SATURATED_BOUNDARY))) // Convergence
      {
         actually_infiltrated_m_ts = (mode   == SATURATED_BOUNDARY)              //080315
          ? - mm_to_m( (water_flux_1 > 0.0 ? 0.0 : water_flux_1) * time_step)    //990408
            + mm_to_m( soil_volume_1 * (new_up_WC_1 - old_up_WC_1)  )            //990302
          : water_entering_soil_m_ts; // m
#ifdef CHECK_NEEDED
Check with Claudio if needed
         finite_diff_error /= soil_layers.get_depth_m(number_FD_solution_layers);//990308
#endif
         status = CONVERGED;
      }
      flux_term_driving_drainage_m_ts = (mode   == SATURATED_BOUNDARY)           //080315
         ? (-mm_to_m(water_flux_1) + soil_volume_1 * (new_up_WC_1 - old_up_WC_1) //080227
                                          / time_step
         //080314  Not sure why the time step is a factor in this case, need to check with Claudio
         // NOTE if I restore time step, then probably must also restore the time step
         // in the caller where flux_term_driving_drainage is used
                           )
         : -water_entering_soil_m_ts;                                            //080227
   }
   return status;                                                                //981012
}
//_Mass_balance_iteration::resolve__________________________________1998-10-12_/
bool Infiltration_finite_difference::Mass_balance_iteration                      //071010
::commit                                                                         //071010
( modifiable_ Soil::Hydrology_finite_difference   &hydrology                     //071010
, soil_layer_array64                            (water_flow_nodal)               //080202
, nat8                                           num_layers_eff)           const //080202
{                                                                                //071010
   for (nat8  layer = 1; layer <= num_layers_eff +1; layer++)                    //080206
      hydrology.set_upper_and_lower_water_content_liquid(layer,new_up_WC[layer],new_low_WC[layer]);
   // The set_upper_and_lower_water_content_liquid may be unnecessary:
   hydrology.set_upper_and_lower_water_content_liquid                            //070110
      (num_layers_eff + 1,new_up_WC[num_layers_eff + 1],new_low_WC[num_layers_eff + 1]);
   copy_layer_array64(water_pot_committed,water_pot);                            //080125
   for (nat8  node = 1; node <= num_layers_eff + 1  ; node++)
   {  /* RLN should be: num_effective_nodes*/
      // convert flux to conventional flow for chemical transport
      float64 water_flow =  (- water_flux_mm[node] * time_step)  / water_density;
         // convert to m3 water/m2 soil in the timestep.
         // Note sign change because fluxes are opposite of flow
      water_flow_nodal[node] = water_flow;                                       //080208
   }
   return true;
}
//_Mass_balance_iteration::commit___________________________________2007-10-10_/
Infiltration_finite_difference::FD_status
   Infiltration_finite_difference
      ::mass_balance_time_step_control
(float64 water_to_enter_soil_mm                                                  //070319
,uint16  max_intervals_per_day      // 24  for water entering
,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm          //990204
,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm          //070119
   // optional (0)
,float64 &runoff_m
,const Dynamic_float_array &runon_in_intervals                                   //990409
   // continue here, make this optional
,      Dynamic_float_array &runoff_in_intervals                                  //990409
   // continue here, make this optional
)
{
   float64 actually_infiltrated_today = 0.0;                                     //991204
   act_water_entering_soil = 0.0;                                                //150925_990716
   float64 act_water_entering_soil = 0.0;                                        //051013
   float64 act_evaporation = 0.0;                                                //051013
      hydrology.calc_water_depth_profile(true);                                  //130930
//             We may want to modify runon in intervals to account for
//             travel time adding the remainder to runoff.

//             If there is no water entering today, we allows 24 hour
//             intervals to handle saturated boundary conditions
//             This is implicitly similar to the old Pascal version.

//Even in the case that there is no preciptation, it it can't converge in full day, let it try over 24 hours
   runoff_m = 0.0;
   /*now a member Seconds */    time_elapsed = 0;
   FD_status   status = START;
   daily_error = 0.0;
   balance_drainage =  0;                                                        //990602
   reported_drainage =  0; // Finite diff can only report drainage for whole profile //990602
   // Now never full day FULL_DAY_FLUX_BOUNDARY;                                 //051130
   uint16   interval_index =  0;
   do   /* for each timestep */
   {
      float64 eff_precip_interval =
               non_runoff_water_entering_in_intervals_mm
               ? mm_to_m(non_runoff_water_entering_in_intervals_mm->get
                           ((uint16)interval_index))
               : 0.0;
      float64 irrigation_entering_this_time_step =                               //070119
               irrigation_water_entering_in_intervals_mm                         //070119
               ? mm_to_m(irrigation_water_entering_in_intervals_mm
                           ->get((uint16)interval_index))                        //070119
               : 0.0 ;                                                           //070119
      float64 runon_interval = runon_in_intervals.get((nat32)interval_index);   //origin
      time_step = 86400.0 / max_intervals_per_day;
      float64 hour_fract = ((float64) time_step / 3600.0);
      int whole_hour = (int) hour_fract;
      hour = (interval_index) * whole_hour;        // should be                  //090123
      runoff_this_interval = 0.0;
      float64 actually_infiltrated_time_step = 0.0;
      float64 actually_evaporated_time_step_m_ts = 0.0;
      //float64 act_reported_drain_m_ts = 0.0;                                   //141022LML
      #ifdef MICROBASIN_VERSION
      float64 act_balanced_drain_m_ts = 0.0;                                     //141022LML
      #endif
      status = infiltrate_and_transport_timestep                                 //140724
         (hour
         ,eff_precip_interval
         ,irrigation_entering_this_time_step
         ,runon_interval
         // Return values
         ,actually_infiltrated_time_step
         ,actually_evaporated_time_step_m_ts
         #ifdef MICROBASIN_VERSION
         //,act_reported_drain_m_ts                                              //141022LML
         ,act_balanced_drain_m_ts                                                //141022LML
         ,runoff_this_interval                                                   //141031LML
         #endif
          );
      actually_infiltrated_today += actually_infiltrated_time_step;              //991204
      act_evaporation += actually_evaporated_time_step_m_ts;                     //051013
      act_water_entering_soil = actually_infiltrated_today; //  Put this here in case we bail out latter //051013
      // Moved from above
      if (runoff_this_interval > 0.0)                                            //990423
      {  runoff_m += runoff_this_interval;
         if (interval_index < 1)
            interval_index = 0;
         runoff_in_intervals.increment_by(interval_index-1,runoff_this_interval);//990409
      }
      time_elapsed += time_step;                           //    Hour = Hour + 1
      interval_index ++;   // advance to next time step.
   } while (time_elapsed < 86400);                         //Loop Until Hour > 24
   //170217unused float64 final_profile_water_depth =
       hydrology.calc_water_depth_profile(true);                                 //130930
//NYI add deferred water to water balance
   return status;
}
//_mass_balance_time_step_control______________________________________________/
Infiltration_finite_difference ::FD_status
Infiltration_finite_difference ::flux_boundary_infiltration
(float64 water_to_enter_this_time_step_m
,float64 &actually_infiltrated_time_step
,float64 &flux_term_driving_drainage                                             //080227
,float64 &finite_diff_error)
{  convergence_info.append("(FlxBndy:");
   float64 water_to_enter_m_ts = water_to_enter_this_time_step_m;
   FD_status   status = FLUX_BOUNDARY;
   #ifdef MICROBASIN_VERSION
   for (nat8  node = 1; node <= number_FD_solution_nodes; node++)                //140603
   {  nat8 layer = node;                                                         //140603
      if (layer > soil_layers.count())                                           //140603
         hydrology.set_water_content_volumetric_layer(layer , hydrology.get_liquid_water_content_volumetric(layer)); //140603
   }
   #endif
   Mass_balance_iteration mass_balance_iteration
         (status,time_step,soil_layers,hydrology,soil_hydraulic_properties
         ,atmospheric_water_potential,evaporator,water_potential,node_depth
         ,typical_water_potential_below_rootzone());
   status = mass_balance_iteration.resolve
         (hour                                                                   //080214
         ,maxints
         ,water_to_enter_m_ts
         ,water_table_boundary_condition
         ,number_FD_solution_layers,number_FD_solution_nodes                     //080213
         ,flux_term_driving_drainage);                                           //080227
   finite_diff_error             =mass_balance_iteration.finite_diff_error;      //071010
   actually_infiltrated_time_step=mass_balance_iteration.actually_infiltrated_m_ts; //071010
   if (status == CONVERGED)
   {  convergence_info.append("converged");
      mass_balance_iteration.commit(hydrology,water_flow_nodal,number_FD_solution_layers+1); // RLN 080215  was number_of_real_plus_extended_layers
   } else
   {  // Recover saved values
               // We will be staying in interval restore time step
      convergence_info.append("failed->");
      actually_infiltrated_time_step   = 0;
      flux_term_driving_drainage       = 0;                                      //080227
      if  ((water_to_enter_this_time_step_m > 0.0))                              //071008
      {  status = saturation_boundary_infiltration
               (water_to_enter_m_ts,actually_infiltrated_time_step
               ,flux_term_driving_drainage                                       //080227
               ,finite_diff_error);
      } else
      {
         status = (water_table_boundary_condition)
            ?  FAILED_ALL_CONDITIONS // There is no way the cascade mode only works with free drainage conditions.
            :  cascade_redistribution                                            //080122
               (actually_infiltrated_time_step
               ,finite_diff_error);
         }
      } // if converged / else not converted
   convergence_info.append(")");
   return status;
}
//_flux_boundary_infiltration_______________________________________2007-10-10_/
Infiltration_finite_difference::FD_status
   Infiltration_finite_difference::saturation_boundary_infiltration
         (float64 water_to_enter_this_time_step_m  // Not by reference
         ,float64 &actually_infiltrated_time_step  // return
         ,float64 &flux_term_driving_drainage      // return
         ,float64 &finite_diff_error )             // return
{  convergence_info.append("(SatBndy:");
   // Save water contents from beginning of timestep
   //170217unused float64 water_to_enter_m_ts = 0.0;
   FD_status status = SATURATED_BOUNDARY; // Tex
   Mass_balance_iteration mass_balance_iteration
         (status,time_step,soil_layers,hydrology
         ,soil_hydraulic_properties
         ,atmospheric_water_potential
         ,evaporator
         ,water_potential,node_depth
         ,typical_water_potential_below_rootzone());
   status = mass_balance_iteration.
       resolve                                                                   //071010
         (hour, maxints, 0.0                                                     //051130
         ,water_table_boundary_condition
         ,number_FD_solution_layers,number_FD_solution_nodes                     //080213
         ,flux_term_driving_drainage ) ;                                         //080227
   bool retry_this_timestep_flux_boundary_with_water_deferred = 0;
   float64 retry_water_entering_this_time_step_m = 0;                            //071010
   if (status == CONVERGED)
   {  float64 maximum_potential_infiltration = mass_balance_iteration.actually_infiltrated_m_ts;
      convergence_info.append("converged-");                                     //000307
      if (maximum_potential_infiltration > water_to_enter_this_time_step_m)
      {  // Saturated boundary may actually determine that more
         // water could be infiltrated than was needed.
         // So we try infiltrating with flux boundary
         // with half as much to enter.
         // Note that this will done recursively
         // until only the amount needed is infiltrated.
         float64 half_water_to_enter_m_ts = water_to_enter_this_time_step_m / 2.0;
         if (half_water_to_enter_m_ts > 0.001) // Less than 1 mm?
         {  // We try with half as much water to enter and defer the other half.
            convergence_info.append("defer_half_EWS_and_retry");
            retry_this_timestep_flux_boundary_with_water_deferred = true;
            retry_water_entering_this_time_step_m = half_water_to_enter_m_ts;
            deferred_water_to_enter_soil_mm += m_to_mm(half_water_to_enter_m_ts);
         } else
         {  // In this case we just defer the water to the next time step.
            convergence_info.append("miniscule_WES_defer_and_retry");
            deferred_water_to_enter_soil_mm += m_to_mm(water_to_enter_this_time_step_m);
            retry_this_timestep_flux_boundary_with_water_deferred = true;
            // (retry in this case should always be successful)
         }
      } else  // (actually_infiltrated_time_step < water_to_enter_this_time_step_m)
      {  // we have excess
         convergence_info.append("infiltrated");
         actually_infiltrated_time_step = maximum_potential_infiltration;        //071019
         float64 non_infiltrated_mm = m_to_mm(water_to_enter_this_time_step_m - actually_infiltrated_time_step);  //991204
         if (numerical_runoff_mode)
         {  float64 runoff_now = pond.set_depth_with_runoff(mm_to_m(non_infiltrated_mm));    //130803_071024
            runoff_this_interval += runoff_now;                                  //991204
         } else  // not numerical defer all excess to next hour.
            deferred_water_to_enter_soil_mm += non_infiltrated_mm;
         retry_this_timestep_flux_boundary_with_water_deferred = false; // Already cleared above, but assigned again for clarity
         mass_balance_iteration.commit(hydrology,water_flow_nodal,number_of_real_plus_extended_layers);  //071017
         // Just proceed to next timestep
      }
   } else // Mass balance did not converge
   {  // Defer all water entering to next hour
         convergence_info.append("failed->defer");
         retry_this_timestep_flux_boundary_with_water_deferred = true;
         retry_water_entering_this_time_step_m = 0;
         deferred_water_to_enter_soil_mm += m_to_mm(water_to_enter_this_time_step_m);
   } // if converged / else not converted
   if (retry_this_timestep_flux_boundary_with_water_deferred)
   {  // Note that we may need to run flux boundary again,
      // even if the saturated boundry converged
      actually_infiltrated_time_step   = 0;
      flux_term_driving_drainage = 0;                                            //080228
      status = flux_boundary_infiltration
         (retry_water_entering_this_time_step_m
         ,actually_infiltrated_time_step
         ,flux_term_driving_drainage                                             //080227
         ,finite_diff_error
         //140509 ,convergence_info
         );
         // The recusive call may defer additional water
         // so we accumulate what we may have deferred above.
   }
   convergence_info.append(")");
   return status;
}
//_saturation_boundary_infiltration_________________________________2007-10-10_/
Infiltration_finite_difference::FD_status
   Infiltration_finite_difference::cascade_redistribution
      (return_ float64 &actually_infiltrated_time_step
      ,return_ float64 &finite_diff_error)
{
   convergence_info.append("(Cascade:");
   FD_status status = CASCADE_REDISTRIBUTION;
   nat8   num_layers_eff = number_of_real_plus_extended_layers;
   Layer first_redistribution_layer = 1;
   Layer last_redistribution_layer = num_layers_eff;
   //170217unused float64  time_steps_to_field_capacity =  ((float64)(86400 - time_elapsed)) / (float64)time_step;
            // Warning  hours_to_field_capacity might need to be calculated
            // like hourly cascaded (moved calc_hours_to_field_capacity to Soil_infiltration_common).
            // However, the daily cascade model allows a time frame that may span multiple days
            // the F.D. model currently works within 1 day, there fore
            // I am using the number of hours remaining in the day for this.
            // Need to confirm with Claudio -- Roger
   float64 redistribution_drainage = redistribute_water // V.B.  SoilWaterRedistribution //070125
      ( first_redistribution_layer
      , last_redistribution_layer                                                //070125
      , water_flow_nodal                                                         //080208
      , 5 // Hours to field this should actually call the function calc_hours_to_field_capacity
      , time_step);                                                              //080311
   // Note, need to verify that upper_and_lower have been set to the same value as the layer as updated by redistribute water
   cascade_drainage =  redistribution_drainage;                                  //071025
   convergence_info.append(")");
   return status;
}
//_cascade_redistribution___________________________________________2007-10-10_/
float64 Infiltration_finite_difference
::Mass_balance_iteration::calc_water_content
   (nat8  layer , float64 water_pot_sl, float64 &dwdp )                    const
{  float64 w ;
   float64 air_entry_pot_sl = hydrology.get_air_entry_pot(layer,true,false);     //990312
   float64 ws=soil_hydraulic_properties.get_saturation_water_content_volumetric
      (layer,hydrology.get_ice_content_volumetric(layer));                       //140902
   if (water_pot_sl < air_entry_pot_sl)
   {  float64 b1 = 1.0 / soil_hydraulic_properties.get_Campbell_b(layer,false);  //990321
      w = ws * pow((air_entry_pot_sl / water_pot_sl) , b1);                      //990312
      dwdp = -w * b1 / water_pot_sl;
   } else
   {  w = ws;                                                                    //990312
      dwdp = 0.00001;
   }
   return w;
} //080118 Checked against V.B. version (Infiltration_Clean_010908)
//_calc_water_content__________________________________________________________/
bool  Infiltration_finite_difference::infiltration_finite_difference
(float64                      irrigation_unused    // m
   // Irrigation is now distributed to water_entering_in_intervals_mm
,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm          //990204
,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm          //070119
   // optional (may be 0 when no irrigation)
,const Dynamic_float_array   &runon_in_intervals                                 //990409
,Dynamic_float_array         &runoff_in_intervals)                               //990409
{

   // WARNING  directed irrigation has not be implemented here.                  //200226
   // so irrigation is processed as if applied fully accrossed the surface

   float64 additional_runoff  = 0.0;
   FD_status status           = START;                                           //981012
   float64 non_runoff = non_runoff_water_entering_in_intervals_mm ?  non_runoff_water_entering_in_intervals_mm->sum() : 0.0; //140226
   float64 pot_water_entering_soil_mm                                            //070107
      = non_runoff                                                               //140226
      + (  irrigation_water_entering_in_intervals_mm
         ? irrigation_water_entering_in_intervals_mm->sum() : 0.0);              //070107
   nat16 max_intervals_for_water_entering_soil =
      (pot_water_entering_soil_mm == 0) ? 1  : 24;                               //070319
   nat16 non_runoff_intervals =
      non_runoff_water_entering_in_intervals_mm
      ? non_runoff_water_entering_in_intervals_mm->get_current_size() :0;        //140226_070319
   nat16 irrigation_intervals = irrigation_water_entering_in_intervals_mm
      ? irrigation_water_entering_in_intervals_mm->get_current_size() :0;        //070319
   nat16 max_intervals_per_day =
      std::max<nat16>(non_runoff_intervals
     ,std::max<nat16>(runon_in_intervals.get_current_size()                      //070319
     ,std::max<nat16>(max_intervals_for_water_entering_soil                      //070319
     ,std::max<nat16>(irrigation_intervals,24))));                               //070319
   status = mass_balance_time_step_control                                       //990215
      (pot_water_entering_soil_mm                                                //070319
      ,max_intervals_per_day                                                     //990403
      ,non_runoff_water_entering_in_intervals_mm                                 //990403
      ,irrigation_water_entering_in_intervals_mm                                 //990403
      ,additional_runoff   //110330 I think additional_runoff is simply the sum of runoff_in_intervals
      ,runon_in_intervals                                                        //990409
      ,runoff_in_intervals);                                                     //990409
   bool successful = (status == CONVERGED) || (status == CASCADE_REDISTRIBUTION);//080122
   if (!successful)                                                              //110330
   {  // The finite difference failed to converge.
      // Most likely the soil is saturated (and probably with a water table).
      // In this case we will assume everything will be runoff.
      additional_runoff = 0.0;
      nat32 max_index =
         non_runoff_water_entering_in_intervals_mm                               //140226
         ? non_runoff_water_entering_in_intervals_mm->get_count()
         : 0;                                                                    //140226
      if (irrigation_water_entering_in_intervals_mm)
         max_index = std::max<nat32>
         (max_index
         ,irrigation_water_entering_in_intervals_mm->get_count());
      for (nat32 i = 0; i < max_index; i++)
      {  float32 runoff_i =
            non_runoff_water_entering_in_intervals_mm                            //140226
            ? mm_to_m(non_runoff_water_entering_in_intervals_mm->get(i))
            : 0.0;
         if (irrigation_water_entering_in_intervals_mm)
             runoff_i += mm_to_m(irrigation_water_entering_in_intervals_mm->get(i));
         runoff_in_intervals.set(i,runoff_i);
         additional_runoff += runoff_i;
      }
   }
   if (runoff) runoff->add_runoff_from_infiltration(additional_runoff);          //130308
   return successful;                                                            //080122
}
//_infiltration_finite_difference________________________________________1999?_/
bool Infiltration_finite_difference::initialize()
{  bool inited = Soil::Infiltration_abstract::initialize();                      //071012
   nat8  number_real_layers = soil_layers.count();                               //080211
   nat8  extended_layers_count = 2;                                              //080212
   number_of_real_plus_extended_layers=number_real_layers+extended_layers_count; //080211
      // Initially assume free drainage
   number_of_real_plus_extended_nodes =number_of_real_plus_extended_layers + 1;  //080211
   number_FD_solution_layers          =number_of_real_plus_extended_layers;      //080212
   number_FD_solution_nodes           =number_of_real_plus_extended_nodes;       //080212
      // number_FD_solution_nodes same as number_FD_solution_layers+1;
   #ifndef MICROBASIN_VERSION
   soil_base.expand_last_layer_for_boundary_conditions
      (number_real_layers,number_of_real_plus_extended_layers+1);                //080213
   #endif
   clear_node_array64(node_depth);                                               //080215
   node_depth[0] = 0.0;                                                          //080215
   node_depth[1] = 0.0;                                                          //080215
   for (nat8  node = 1; node <= number_of_real_plus_extended_nodes; node++)      //080215
   {  nat8  layer = node;                                                        //080215
      node_depth[node+1]= node_depth[node] + soil_layers.get_thickness_m(layer); //080215
   }
   hydrology.initialize(node_depth);                                             //080215
   evaporator.initialize(number_of_real_plus_extended_nodes);                    //090417
   for (nat8  node = 1; node <= number_of_real_plus_extended_nodes; node++)      //080125
   {  nat8  layer = node;                                                        //080125
      float64 upper_WC        = hydrology.get_liquid_water_content_volumetric(layer); //070808
      water_potential[node]   = hydrology.calc_water_potential_with_respect_to_layer(upper_WC,layer); //090107
      // At this point we simply use the respective layer for the water potential
      // This is consistent with the V.B. version, but the V.B. uses the upper water content.
      // It shouldn't matter since the upper and lower will be the same at initialization.//080125
   }
   return inited;
}
//_initialize__________________________________________________________________/
bool Infiltration_finite_difference::reinitialize
(const soil_horizon_array32(H2O))
{  water_table_boundary_condition = false;
   nat8  num_sublayers_eff = soil_layers.count();                                //991026
   soil_layers.set_number_uninundated_layers(num_sublayers_eff);                 //080211
   {
      water_potential[num_sublayers_eff+1] = typical_water_potential_below_rootzone(); //080325
      // This will cause bottom of the soil profile to be a sink
      // below root zone soil tends to stay wet (near field capacity or slightly lower)//080325
   }
   return true;
}
//_reinitialize_____________________________________________________1998-07-29_/
Infiltration_finite_difference::FD_status Infiltration_finite_difference
::infiltrate_and_transport_timestep
(Hour _hour
,float64 eff_precip_interval
,float64 irrigation_entering_this_time_step
,float64 runon_interval
// return values
,float64 &actually_infiltrated_time_step
,float64 &actually_evaporated_time_step_m_ts
#ifdef MICROBASIN_VERSION
,float64 &actually_balance_drainage_m_ts        //LML 141022
,float64 &surface_runoff_m_ts                   //LML 141031
#endif
)
{
   hour = _hour;
   FD_status   status = START;
      cascade_drainage = 0.0;
      float64 water_depth_init_for_bal_m = hydrology.get_liquid_water_depth_profile(); //080201
      clear_node_array64(water_flow_nodal);                                      //080208
      soil_node_array64(old_water_depth_nodal); clear_node_array64(old_water_depth_nodal); //080114
      hydrology.get_liquid_water_depth_nodes(old_water_depth_nodal,number_of_real_plus_extended_nodes); //080201
      float64 pond_depth_m =   pond.relinquish_depth();                          //071012
      float64 water_to_enter_this_time_step_m =
                 eff_precip_interval
               + irrigation_entering_this_time_step                              //070119
               + runon_interval
               + pond_depth_m                                                    //991102
                  // It should be ok to put the current pond depth for each interval,
                  //it is updated at the end of each interval when converged
               + mm_to_m(deferred_water_to_enter_soil_mm) ;                      //071008
      deferred_water_to_enter_soil_mm = 0.0;                                     //071012
      // Irrigation is already distributed in the method infiltration_finite_difference
      actually_infiltrated_time_step = 0.0;
      float64 flux_term_driving_drainage = 0.0;                                  //080228
      float64 finite_diff_error = 0.0;                                           //071010
      convergence_info.clear();                                                  //140509
      atmospheric_water_potential =
         #ifdef MICROBASIN_VERSION
         //  correct this, the calc_atmospheric_water_potential would now be available 151215 RLN
         -70000.0;   //  the MB version needs to be corrected

         assert(false); // should now use calc_atmospheric_water_potential()

         #else
         calc_atmospheric_water_potential                                        //071010
            (relative_humidity_min.percent());
            // Note that currently we pass the daily value, but we could pass by hour if available.
         #endif
      evaporator.know_atmospheric_water_potential(atmospheric_water_potential); //160606
      actually_evaporated_time_step_m_ts = 0.0;                                  //041203
      float64 water_to_enter_this_time_step_mm = m_to_mm(water_to_enter_this_time_step_m);   //090112
      evaporator.evaporate_hour(hour);                                           //160606
      water_to_enter_this_time_step_m = mm_to_m(water_to_enter_this_time_step_mm);
      status = flux_boundary_infiltration                                        //071010
         (//hour,                                                                //071010
          water_to_enter_this_time_step_m                                        //071010
         ,actually_infiltrated_time_step                                         //071010
         ,flux_term_driving_drainage                                             //080227
         ,finite_diff_error);                                                                      //071010
      //actually_reported_drainage_m_ts = flux_term_driving_drainage;              //LML 141022
      bool successful=(status==CONVERGED)||(status==CASCADE_REDISTRIBUTION);     //080122
      if (!successful)                                                           //080122
         return status;                                                          //080122
      // should always be successful                                             //071010
      daily_error += finite_diff_error;
      #ifdef MICROBASIN_VERSION
      current_error_dt = finite_diff_error;                                      //LML 141028
      surface_runoff_m_ts = water_to_enter_this_time_step_m - actually_infiltrated_time_step;   //LML 141031
      #endif
      nat8  leaching_sublayer = soil_layers.get_layer_at_depth_or_last_layer(leaching_depth);   //990606
      // Must be after commit
      float64 water_depth_final_for_bal_m = hydrology.get_liquid_water_depth_profile();   //080122
         //  FWC = 0
         //  For i = 1 To m
         //    FWC = FWC + (wnu(i) * V_Upper(i) + wnl(i) * V_Lower(i)) / (V_Upper(i) + V_Lower(i)) * dz(i) * wd
         //  Next i
      float64 bal_drainage_this_time_step =                                      //990602
            (water_depth_init_for_bal_m - water_depth_final_for_bal_m)           //990602
            + actually_infiltrated_time_step                                     //991102
            - actually_evaporated_time_step_m_ts;                                //990602
      if (cascade_drainage > 0.000001)                                           //071025
         bal_drainage_this_time_step =cascade_drainage;                          //071025
      #ifdef MICROBASIN_VERSION
      actually_balance_drainage_m_ts = bal_drainage_this_time_step;              //LML 141022
      #endif
      balance_drainage +=  bal_drainage_this_time_step;                          //990602
      reported_drainage +=  bal_drainage_this_time_step;                         //990602
      
      // Finite diff can only report drainage for whole profile
      hydrology.record_water_content_for_hour(hour);                             //070814
      if (chemicals)                                                             //070117
      {  float64 deferment_fraction = CORN::is_zero<float64>                     //071016
            (actually_infiltrated_time_step)
            ? 1.0
            : mm_to_m(deferred_water_to_enter_soil_mm) / water_to_enter_this_time_step_m;
         soil_node_array64(new_water_depth_nodal); clear_node_array64(new_water_depth_nodal);               //080114
         hydrology.get_liquid_water_depth_nodes(new_water_depth_nodal,number_of_real_plus_extended_nodes);  //080201
         chemicals->chemical_transport_and_exchange                              //980724
            (FINITE_DIFFERENCE_INFILTRATION                                      //080116
            ,number_FD_solution_nodes // number_of_real_plus_extended_nodes      //061004
               // RLN Need to check this
               // 080211 number_effective_nodes
               // 080202   soil_layers.get_number_effective_layers()
            ,number_of_real_plus_extended_nodes                                  //080219
            ,time_elapsed                                                        //071015
            ,time_step                                                           //990320
            ,old_water_depth_nodal
            ,new_water_depth_nodal
            ,actually_infiltrated_time_step                                      //070119
            ,water_flow_nodal                                                    //080208
            ,deferment_fraction                                                  //071016
            ,override_leaching_depth                                             //990213
            ,leaching_sublayer                                                   //990213
            ,water_table_boundary_condition                                      //080207
            );
      }  // chemicals                                                            //071016
   return status;
}
//_infiltrate_and_transport_timestep________________________________2014-05-09_/
}//_namespace_Soil__________________________________________________2018-12-06_/

