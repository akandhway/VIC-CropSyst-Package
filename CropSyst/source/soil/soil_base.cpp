#  include <math.h>
#  include "corn/math/moremath.h"
#  include "soil/soil_base.h"
#  include "corn/dynamic_array/dynamic_array_T.h"
//190702 #  define Dynamic_float_array Dynamic_array<float32>
#  include "soil/hydraulic_properties.h"
#  include "soil/hydrology_A.h"
#  include "common/soil/txtrhydr.h"
#  include "common/physics/standard_constants.h"
#  include "soil/infiltration_A.h"
#  include "soil/soil_evaporator.h"
//______________________________________________________________________________
const Soil::Hydrology_interface *Soil_base::ref_hydrology_at(nat16 seconds)const //150612
{
   UNUSED_arg(seconds);
   // Implement this when Claudio eventually provides hourly cascade
   return 0;  //NYI
   // NYI  /*_______*/    return  get_hydrology();
}
//_ref_hydrology_at_________________________________________________2006-05-04_/
bool Soil_base::set_infiltration_hydrology
(Infiltration_model         infiltration_model_                                  //041116
,Soil::Infiltration_nominal *infiltration_submodel_                              //071012
,Soil::Hydrology_abstract   *hydrology_)                                         //060512
{
   infiltration_submodel = infiltration_submodel_;                               //060206
   infiltration_model = infiltration_model_;                                     //130301
   hydrology = hydrology_;                                                       //060512
   return true;
}
//_set_infiltration_hydrology_______________________________________1998-08-01_/
void Soil_base::expand_last_layer_for_boundary_conditions
(nat8 number_layers,nat8 number_expanded_layers)                   modification_
{  // 080212 Although this currently applies only to F.D., F.D. only has access to some of these by reference
   layers               ->expand_last_layer_for_boundary_conditions(number_layers,number_expanded_layers);  //061003
#ifndef MICROBASIN_VERSION
   structure            ->expand_last_layer_for_boundary_conditions(number_layers,number_expanded_layers);  //070725
#endif
   hydraulic_properties ->expand_last_layer_for_boundary_conditions(number_layers,number_expanded_layers);  //051204
   hydrology            ->expand_last_layer_for_boundary_conditions(number_layers,number_expanded_layers);  //060504
}
//_expand_last_layer_for_boundary_conditions________________________2008-01-30_/
bool Soil_base::reinitialize
(Moisture_characterization moisture                                              //170320
,soil_horizon_array32(H2O) // not const because initialized to avgWC when 0 values 140311
,bool beginning_of_simulation
,bool subdivide_horizons)
{  bool reinited = true;                                                         //131217
#ifndef MICROBASIN_VERSION
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
   reinited &= Soil_properties_sublayers::reinitialize                           //101104
      (beginning_of_simulation,subdivide_horizons
         ,infiltration_model == FINITE_DIFFERENCE_INFILTRATION );
   if (beginning_of_simulation)                                                  //101104
   {  for (nat8 sublayer = 1;sublayer < layers->count();sublayer++)              //101104
      {  nat8 horizon = layers->get_horizon_at_layer(sublayer);                  //101104
         hydrology->initialize_moisture(sublayer,moisture,H2O[horizon]);         //170320
      }
   }
#else
   float64 soil_depth = 0.0;
   for (nat8 horizon = 1; horizon <= parameters_ownable->horizon_count ; horizon++)
      soil_depth += parameters_ownable->get_horizon_thickness(horizon);
   float64 sublayer_depth = 0.0; //Start subdivisions from the second soil layer
   nat8 sublayer = 1;
   for (nat8 horizon = 1; horizon  <= (nat8)parameters_ownable->horizon_count; horizon ++)
   {  if (subdivide_horizons)                                                    //060229
      {  //    We now have a new sublayering scheme:
         //    - Sublayer 1 is always what the user specifies.
         //    - until 50cm subdivide the layers into approx. 5cm
         //    - until 1.2cm subdivide the layers into approx. 10cm
         //    - remaining will be 20cm.
         //   {16 bit version doesn't split well unless we give a little tolerance}
     layers->divisions[horizon ] = 0;
     float64 thickness_to_divide = parameters_ownable->get_horizon_thickness(horizon );

     // thickness_to_divide is the remaining amount of
     // the sublayer thickness to divide.
      while (! CORN::is_zero<float64>(thickness_to_divide/*200127,0.0*/,0.0001)
         && (sublayer < MAX_soil_sublayers))                                     //000323
      {  float64 sublayer_thickness = thickness_to_divide;
         if (sublayer == 1)
         {  float64 evaporative_layer_thickness =
                  (infiltration_model==FINITE_DIFFERENCE_INFILTRATION
                  ? parameters_ownable->evaporative_layer_thickness_finite_difference
                  : parameters_ownable->evaporative_layer_thickness_cascade);
            sublayer_thickness = std::min<float64>(thickness_to_divide,evaporative_layer_thickness);
         } else
         {
            {   float64 MAX_sublayer_thickness  =
                 ((sublayer_depth > 1.2001)
                  ? 0.2001
                    : (sublayer_depth > 0.5001)
                       ? 0.1001
                       : 0.0501);
                {  int divs = thickness_to_divide / MAX_sublayer_thickness;
                   int divs_1 =(thickness_to_divide/MAX_sublayer_thickness) +1;
                   if (divs <= 0) divs = 1;  // must be at least one division
                   sublayer_thickness = thickness_to_divide / divs;
                   float64 sublayer_thickness_1 = thickness_to_divide / divs_1;
                   if (fabs(sublayer_thickness_1 -MAX_sublayer_thickness) <
                         fabs(sublayer_thickness   -MAX_sublayer_thickness))
                      sublayer_thickness = sublayer_thickness_1;
                }
            }
        }
        sublayer_depth += sublayer_thickness;
        thickness_to_divide -= sublayer_thickness;
        subdivide_horizon(horizon ,sublayer,sublayer_thickness,sublayer_depth
           ,moisture,H2O[horizon],beginning_of_simulation);                      //170320_140311
        layers->divisions[horizon ]++;
        sublayer++;
        layers->num_uninundated_sublayers =                                      //080211
        layers->num_sublayers = (nat8)(sublayer - 1);
        float64 K = parameters_ownable->NH4_Langmuir.K.get(horizon);
        float64 Q = parameters_ownable->NH4_Langmuir.Q.get(horizon);
        NH4_Langmuir.K.set(sublayer,K);
        NH4_Langmuir.Q.set(sublayer,Q);
     }
     } else // dont subdivide horizons
     {   float64 layer_thickness = parameters_ownable->get_horizon_thickness(horizon );
         sublayer_depth += layer_thickness;
         subdivide_horizon(horizon,horizon,layer_thickness,sublayer_depth
            ,moisture,H2O[horizon],beginning_of_simulation);                     //170320_140311
         layers->divisions[horizon]=1;
         layers->num_uninundated_sublayers =                                     //080211
         layers->num_sublayers = (nat8)parameters_ownable->horizon_count;
     }
   }
#endif
   clear_sublayer_array(record_all_water_content);
   //  Recompute layer depths because FD subdivision currently isn't doing it right
   reinited &= infiltration_submodel->reinitialize(H2O);                         //030710
#endif
   return reinited;                                                              //131217
}
//_reinitialize________________________________________________________________/
bool Soil_base::start_day()
{  bool started = true;
   started &= mod_evaporator()->start_day();                                     //160606
   return started;                                                               //160608
}
//_start_day________________________________________________________2005-12-06_/
bool Soil_base::end_day()
{
   bool ended = true;
   #if (!defined(OLD_EVAPORATE) && !defined(VIC_CROPSYST_VERSION))
   ended &= mod_evaporator()->end_day();
   #endif
   return ended;
}
//_end_day__________________________________________________________2016-07-19_/
void Soil_base::clear_sublayer_array(soil_sublayer_array_64(target))       const //161008
{  for (nat8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer++)
      target[sublayer] = 0;
}
//_clear_sublayer_array_____________________________________________1999-02-14_/
nat8 Soil_base::horizon_at(float64 a_given_depth )                         const
{  float64 horizon_depth = 0.0;
   nat8 horizon = 0;
   do
   {  horizon++;
      horizon_depth += parameters_ownable->get_horizon_thickness(horizon);       //981119
   } while ((horizon_depth <= a_given_depth) && (horizon <= MAX_soil_horizons));
   return (horizon > (nat8)parameters_ownable->horizon_count) ? MAX_soil_horizons : horizon;
}
//_horizon_at__________________________________________________________________/
/*190707 unused/obsolete
float64 Soil_base::tally_to_depth
(soil_sublayer_array_64(values), float64 a_given_depth , bool get_average) const
{  float64 sum = 0.0;
   nat8 sublayer_at_depth = layers->sublayer_at(a_given_depth,false);
   for (nat8 sublayer = 1; sublayer <= sublayer_at_depth; sublayer++)
      sum += values[sublayer];
   return get_average
   ? sum / sublayer_at_depth //  sublayer_at_depth is the number of sublayers
   : sum;
}
//_tally_to_depth___________________________________________________1999-02-08_/
*/
//The return value was never actually used 
float64 Soil_base::current_plant_available_water
(const soil_sublayer_array_64(fract_root_length))  // May be 0 if no crop
{  float64 curr_plant_avail_water =0.0 ;  // If there is not crop then this is just the available water.
   for (nat8 sublayer=1;sublayer <= ref_layers()->count();sublayer++)            //150817LML
   {  float64 plant_avail_water_sl = ref_hydrology()->calc_available_water_fraction(sublayer,true);   //160412_150817LML_990217
      mod_hydrology()->set_plant_avail_water(sublayer, plant_avail_water_sl);    //160412_150817LML_140610_060504
      float64 fract_root_length_sl = fract_root_length ?  fract_root_length[sublayer] : 1.0; //031124
      curr_plant_avail_water += plant_avail_water_sl * fract_root_length_sl;     //031124
   }
   return curr_plant_avail_water;
}
//_current_plant_available_water_______________________________________________/
bool Soil_base::reinitialize_for_crop
(float64 wilt_leaf_water_pot)
{  nat8 num_layers = ref_layers()->count();                                      //150529LML
   for (nat8 layer = 1; layer <= MAX_soil_layers; layer++)
   {  float64 real_wilt_point = 0.0;                                             //190707
      if (layer <= num_layers)                                                   //981209
      {  float64 sat_water_cont_sl  =                                            //981209
            ref_hydraulic_properties()->get_saturation_water_content_volumetric  //051204
               (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);               //160412_150529LML
         float64 air_entry_pot_sl= ref_hydrology()->get_air_entry_pot(layer,false,false); //160412_150529LML_990321
         float64 b_sl          = ref_hydraulic_properties()->get_Campbell_b(layer,false); //160412_150529LML_990321
         /*190707
         mod_hydrology()->                                                          //160412_150529LML
            set_real_wilt_point(layer,                                              //140610
               sat_water_cont_sl*pow(wilt_leaf_water_pot/air_entry_pot_sl,(-1.0/b_sl)));//980529
         */
         real_wilt_point = sat_water_cont_sl*pow(wilt_leaf_water_pot/air_entry_pot_sl,(-1.0/b_sl)); //190707
      } else
         real_wilt_point =  mod_hydrology()->get_real_wilt_point(num_layers);    //190707
        /*190707
         mod_hydrology()->                                                       //160412_150529LML
            set_real_wilt_point(layer,                                           //140610
               mod_hydrology()->get_real_wilt_point(num_layers));                //160412_150529LML_060509
        */
      mod_hydrology()->set_real_wilt_point(layer,real_wilt_point);               //190707
   }
   return true;                                                                  //131217
}                                                                                //980724
//_reinitialize_for_crop________________________________________________________
bool Soil_base::initialize_water_content_from_PAW(float32 soil_profile_plant_available_water)
{  Dynamic_array<float32> PAW_profile(12,12,0);
   PAW_profile.set(1,soil_profile_plant_available_water);
   recalibrate_plant_available_water(PAW_profile);
   return true;                                                                  //131217
}
//_initialize_water_content_from_PAW___________________________________________/
Soil_base::Soil_base
(const Smart_soil_parameters        *parameters_
,bool                                parameters_given_                           //160916
,const CORN::date32                 &simdate_)                                   //131001
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
#error reached
: Soil_properties_sublayers                                   (parameters,false) //130328_101104
#else
: parameters_ownable                                               (parameters_) //061005
, parameters_owned                                           (parameters_given_) //160916_140609
, layers(new Soil_sublayers                               (*parameters_ownable)) //061004
, hydraulic_properties           (new Soil::Hydraulic_properties(*ref_layers())) //051204
, NH4_Langmuir                                                    (2000.0,0.003) //190405
#endif
,CS::Simulation_element_abstract                                      (simdate_) //150724
,evaporator                                                                  (0)
   // will be given later
,evaporator_owned                                                        (false) //160608
,infiltration_model                                 (CASCADE_DAILY_INFILTRATION) //130301
,drainage_layer                                                              (0)
,initial_drainage                                                          (0.0)
#ifdef WSHEDMOD
,lateral_flow_in                                                           (0.0)
,lateral_flow_out                                                          (0.0)
#endif
,infiltration_submodel                                                       (0) //030110
#if (!defined(MICROBASIN_VERSION) || defined(VIC_CROPSYST_VERSION))
,structure                           (new Soil::Structure_common(*ref_layers())) //070725
#endif
,hydrology                                                                   (0) //060504
   // instanciated in initialize()
,hydrology_timestep_resolution_today                                     (86400)
   // May be updated daily depending on the infiltration model used //060508
,emanator_known                                                              (0) //160616
{  for (nat8 sublayer = 0; sublayer <= MAX_soil_sublayers; sublayer ++)
   {  record_all_water_content[sublayer] = 0;                                    //011109
      required_freezing_index[sublayer] = 0;                                     //011109
   }
}
//_Soil_base:constructor_______________________________________________________/
Soil_base::Soil_base
(const Soil_base &copy_from
,float32 fraction)
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
: Soil_properties_sublayers                 (copy_from.parameters_ownable,false)
#else
: parameters_ownable                              (copy_from.parameters_ownable)
, parameters_owned                                                       (false)
,layers(new Soil_sublayers                                (*parameters_ownable))
,hydraulic_properties            (new Soil::Hydraulic_properties(*ref_layers()))
, NH4_Langmuir                                                    (2000.0,0.003) //190405
#endif
,CS::Simulation_element_abstract                           (copy_from.simdate_raw) //150724
,evaporator                                               (copy_from.evaporator) //160606
,infiltration_model                                (CASCADE_HOURLY_INFILTRATION)
   // Directed irrigation only supports hourly cascade
,drainage_layer                                                              (0)
,initial_drainage                                                          (0.0)
#ifdef WSHEDMOD
,lateral_flow_in                                                           (0.0)
,lateral_flow_out                                                          (0.0)
#endif
,infiltration_submodel         (copy_from.infiltration_submodel->copy(fraction))
#if (!defined(MICROBASIN_VERSION) || defined(VIC_CROPSYST_VERSION))
,structure                           (new Soil::Structure_common(*ref_layers()))
#endif
,hydrology                                 (copy_from.hydrology->copy(fraction))
,hydrology_timestep_resolution_today                                     (86400)
   // May be updated daily depending on the infiltration model used
{  // Copy constructure used in directed irrigation.
   clear_sublayer_array(record_all_water_content);
}
//_Soil_base:constructor____________________________________________2014-06-09_/
Soil_base::~Soil_base()
{  delete parameters_ownable;                                                    //140609
   delete infiltration_submodel;
   delete hydraulic_properties;                                                  //051204
#ifndef MICROBASIN_VERSION
   delete structure;                                                             //070725
#endif
   //200715LML
#if (!defined(VIC_CROPSYST_VERSION))
   delete hydrology;                                                             //060504
#endif
   delete evaporator;                                                            //160608
   if (emanator_known) emanator_known->set_status(CS::Emanator::deleted_status); //160614
}
//_Soil_base:destructor_____________________________________________2003-07-10_/
#ifdef SOIL_INFILTRATION
bool Soil_base::infiltrate
(Infiltration_model            infiltration_mode
,float64                       water_entering_soil_not_irrigation                //070107
,float64                       management_irrigation
,float64                       management_auto_irrigation_concentration
,Dynamic_water_entering_soil  *non_runoff_water_entering_in_intervals_mm         //990204
,Dynamic_water_entering_soil  *irrigation_water_entering_in_intervals_mm         //990204
   // optional may be 0 when no irrigation
,const Dynamic_array<float32> &runon_in_intervals                                //990409
,Dynamic_array<float32>       &runoff_in_intervals)                              //990409
{  // The cascade model can only infiltrate a single water
   // entering for the day, not intervals
   // The runon is immediately set to runoff, we dont try to infiltrate it.
   // In cascade model all pot_water_entering_soil is infiltrated.
   infiltration_submodel->act_water_entering_soil =                              //150925_991207
              water_entering_soil_not_irrigation + management_irrigation;        //070107
   return infiltration_submodel->infiltrate                                      //030710
      (water_entering_soil_not_irrigation // management_irrigation
      ,management_irrigation
      ,management_auto_irrigation_concentration
      ,non_runoff_water_entering_in_intervals_mm
      ,irrigation_water_entering_in_intervals_mm
      ,runon_in_intervals
      ,runoff_in_intervals);
}
#endif
//_infiltrate__________________________________________________________________/
float64 Soil_base::recalibrate_water_content
(const Dynamic_array<float32> &WC_values_by_horizon)
{ // This is used when using recalibration files.
   float64 profile_water_change_depth = 0.0;                                     //110823
   for (nat8 horizon = 1; horizon <= std::min<nat8>
      ((nat8)parameters_ownable->horizon_count
      ,(nat8)WC_values_by_horizon.get_count()); horizon++)                       //120804_091217
   {  for (nat8 sublayer = 1 ;sublayer <= layers->get_number_sublayers(); sublayer++) //010710
      {  if (horizon == layers->in_horizon_of[sublayer])
         {  float64 value = WC_values_by_horizon.get(horizon);                   //010228
            float64 recalibration_WC_change = value -  hydrology->get_water_plus_ice_content_volumetric(sublayer);  //070813
            profile_water_change_depth +=  recalibration_WC_change * layers->thickness[sublayer];
            hydrology->change_liquid_water_content_by_volumetric(sublayer,recalibration_WC_change);   //070813
   }  }  }
   // Caller needs to adjust water balance with recalibration_WC_change!
   return profile_water_change_depth;                                            //110823
}
//_recalibrate_water_content___________________________________________________/
float64 Soil_base::recalibrate_plant_available_water
(return_ Dynamic_array<float32> &PAW_values_by_horizon)                                     //011116
{  // This method will recalibrate the water content to the specified plant available water.
   // If the user only enters a single value, then all layers get this value.
   if (PAW_values_by_horizon.get_count() == 1)
   {  float32 PAW_1 = PAW_values_by_horizon.get(1);
      for (nat8 horizon=2;horizon<=parameters_ownable->horizon_count;horizon++)  //010710
         PAW_values_by_horizon.set(horizon,PAW_1);                               //010228
   }
   Dynamic_array<float32> WC_values_by_horizon(PAW_values_by_horizon.get_count(),1,0); //011117
   for (nat8 horizon = 1                                                         //010710
         ; horizon <= std::min<nat8>(parameters_ownable->horizon_count,(nat8)PAW_values_by_horizon.get_count())  //010710
         ; horizon++)
   {  float64 field_capacity_h= parameters_ownable->get_horizon_field_capacity_volumetric(horizon);
      float64 wilt_point_h    = parameters_ownable->get_horizon_permanent_wilt_point_volumetric(horizon);
      float64 thickness_h     = parameters_ownable->get_horizon_thickness(horizon);      //070813
      float64 WC_h= PAW_values_by_horizon.get(horizon)* thickness_h              //070813
          * ( field_capacity_h- wilt_point_h) + wilt_point_h;                    //010228
      WC_values_by_horizon.set(horizon,WC_h);                                    //010228
   }
   return recalibrate_water_content(WC_values_by_horizon);
}
//_recalibrate_plant_available_water________________________________2000-02-02_/
#ifdef WSHEDMOD
//______________________________________________________________________________
bool Soil_base::subsurface_flow_out(float64 cell_width, soil_sublayer_array_64(lateral_flow_depth_m))
{    // returns true if there was flow in any sublayer.
   bool flow_encountered = false;
   float64 theta_radians = parameters->get_steepness_percent() * 1.57 / 100.0; // convert steepness to radians.
   float64 sin_theta = sin(atan(theta_radians));
   float64 sin_theta_g = sin_theta * gravitation_const;
   lateral_flow_out = 0.0;
   for (nat8 sublayer = 1; sublayer <= layers->get_number_sublayers(); sublayer++)
   {  if (hydrology->is_saturated(sublayer))
      {  float64 lateral_flow =

               may want to use new parameters
               hydraulic_properties->get_sat_hydraul_cond_kg_s_m3(sublayer) * sin_theta_g;   //051204

         float64 flow_area = get_thickness_m(sublayer) * cell_width;
         float64 flow_volume = lateral_flow * flow_area / water_density * seconds_per_day;
         float64 sat_WC =
               hydraulic_properties->get_saturation_water_content_volumetric(sublayer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);      //051204
         float64 WC_at_10_J_kg = parameters->layer_texture_hydraulics[layers->in_horizon_of[sublayer]]->calc_WC_at
         (-10.0,hydrology->get_air_entry_pot(sublayer,true,false),hydraulic_properties->get_Campbell_b(sublayer,false),sat_WC);
         float64 max_flow_volume = (sat_WC    - WC_at_10_J_kg) * get_thickness_m(sublayer) * flow_area;  //060302
         flow_volume = std::min<float64>(flow_volume,max_flow_volume);
         float64 flow_depth = flow_volume / CORN_sqr(cell_width);
         lateral_flow_depth_m[sublayer] = flow_depth;
         if (flow_depth > 0.0001)
         {  flow_encountered = true;
            lateral_flow_out += flow_depth;
   } } }
   return flow_encountered;
}
//_subsurface_flow_out______________________________________________1999-05-13_/
void Soil_base::subsurface_flow_in(soil_sublayer_array_64(lateral_flows_in))
{  lateral_flow_in = 0.0;
   for (nat8 sl = 1; sl <= layers->get_number_sublayers(); sl++)
   {  float64 flow_in_sl = lateral_flows_in[sl];
      lateral_flow_in += flow_in_sl;
      float64 change_WC = 0.0;   // WARNING NYI need to convert flow to change in WC   //070813
// 070813 BIG WARNING need to check the units of lateral_flows_in
// I would not think it would be in m no volumetric water content also is it change or set
      hydrology->change_liquid_water_content_by_volumetric(sl,change_WC);        //070813
   }
   // NYI return  lateral_flow_in (for all layers)
}
//_subsurface_flow_out______________________________________________1999-05-25_/
#endif
float64 Soil_base::get_albedo()                                            const
{  float64 AW_1 =
      ref_hydrology()->calc_available_water_fraction(1,false);                   //160801_060809
   float64 current_albedo =
      parameters_ownable
         ? parameters_ownable->albedo_wet  + (parameters_ownable->albedo_dry - parameters_ownable->albedo_wet)
         : 0.1 // Microbasin soil parameter file currently doesn't have albedo   //160801
      * (1.0 - AW_1);
   return current_albedo;
}
//_get_albedo_______________________________________________________2004-12-03_/
float64 Soil_base::distribute_amount
(const      soil_horizon_array32  (horizon_amount)
,modifiable_  soil_sublayer_array_64(sublayer_amount))                     const
{  return layers->distribute_amount(horizon_amount,sublayer_amount);             //070607
}
//_distribute_amount________________________________________________2005-11-20_/
float64 Soil_base::distribute_property
(const        soil_horizon_array32  (horizon_amount)
,modifiable_  soil_sublayer_array_64(sublayer_amount))                     const
{  return layers->distribute_property(horizon_amount,sublayer_amount);           //070607
}
//_distribute_property______________________________________________2005-11-20_/
void Soil_base::subdivide_horizon
(nat8 horizon
,nat8 sublayer
,float64 sublayer_thickness
,float64 sublayer_depth
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
#else
,Moisture_characterization moisture                                              //170320
,float64 H2O_l_                                                                  //990305
,bool beginning_of_simulation
#endif
)
{
#ifdef MICROBASIN_VERSION
   assert(false); // Not used in MicrBasin version
#else
   static const float64 reference_water_potential = -10.0;  // J/kg              //070726
   if (sublayer > MAX_soil_sublayers)
   {  std::cerr << "sublayer exceeds maximum sublayers" << std::endl;            //090318
      return;
   }
   structure->set_bulk_density_g_cm3(sublayer,parameters_ownable->get_horizon_bulk_density_g_cm3(horizon)); //070725
#ifdef USE_SOIL_PROPERTIES_SUBLAYERS
   Soil_properties_sublayers::subdivide_horizon(horizon,sublayer,sublayer_thickness,sublayer_depth);    //101104
   float64 FC_sublayer = hydraulic_properties->get_field_capacity_volumetric(sublayer);         //101104
   float64 PWP_sublayer = hydraulic_properties->get_permanent_wilt_point_volumetric(sublayer);  //101104
   float64 Campbell_b_parameter_sublayer = parameters_ownable->get_horizon_b(horizon);                  //101104
   float64 sat_water_content = hydraulic_properties->get_saturation_water_content_volumetric(sublayer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822); //101104
   float64 air_entry_pot_parameter_sublayer =  hydraulic_properties->get_air_entry_potential(sublayer,false); //101104
#else
   layers->in_horizon_of[sublayer] = horizon;
   //The following 3 are constant after initialization
   layers->depth[sublayer] = sublayer_depth;
   layers->thickness[sublayer] = sublayer_thickness;
   float64 sat_water_content = parameters_ownable->get_horizon_saturation_WC(horizon); //160915
   hydraulic_properties->set_saturation_water_content_volumetric(sublayer,sat_water_content);  //080313
   float64 FC_sublayer = hydraulic_properties->set_field_capacity_volumetric
      (sublayer,parameters_ownable->get_horizon_field_capacity_volumetric(horizon)); //051204
   float64 PWP_sublayer = hydraulic_properties->set_permanent_wilt_point_volumetric
      (sublayer,parameters_ownable->get_horizon_permanent_wilt_point_volumetric(horizon)); //051204
   float64 water_pot_at_FC = parameters_ownable->get_horizon_water_pot_at_FC(horizon);
   if (CORN::is_zero<float64>(water_pot_at_FC/*200127 ,0.0,0.0001*/))            //130228
      water_pot_at_FC = -33.0; // This is in case we are reading older files that didn't have water_pot_at_FC as a parameter
   hydraulic_properties->set_water_pot_at_FC(sublayer,water_pot_at_FC);          //130228
   hydraulic_properties->set_bypass_coef(sublayer,parameters_ownable->get_horizon_bypass_coef(horizon));                                                                  //070117
   if (beginning_of_simulation)
   {  float64 clean_H20  =
         #if (CROPSYST_VERSION == 4)
         CORN_clean_precision(H2O_l_,DEF_CLEAN_PRECISION); //051209  needs to be clean for F.D. //051209
         #else
         H2O_l_;
         #endif
      hydrology->initialize_moisture(sublayer,moisture,clean_H20);               //170320_060509
      texture.set_sand_clay_silt
         (sublayer
         ,parameters_ownable->get_horizon_sand(horizon)
         ,parameters_ownable->get_horizon_clay(horizon)
         ,parameters_ownable->get_horizon_silt(horizon));                        //160915_060724
   }  // else  water_content stays the same
   float64 Campbell_b_FC_PWP_based_sublayer = parameters_ownable->layer_texture_hydraulics
      [layers->in_horizon_of[sublayer]]->calc_Campbell_b(FC_sublayer ,PWP_sublayer);
   float64 Campbell_b_parameter_sublayer = parameters_ownable->get_horizon_Campbell_b(horizon);
   //    air_entry_pot[] is constant after initialization}
   //    Soil files prior to 2.02.00 did not have Campbell B so used calculated value.                                                                            //980527
   //    This will also apply to soil properties loaded from databases that dont have the B value.                                                               //980527
   Campbell_b_parameter_sublayer = CORN::is_zero<float64>(Campbell_b_parameter_sublayer)                                                    //980527
      ? Campbell_b_FC_PWP_based_sublayer
      : Campbell_b_parameter_sublayer;                                                                                                                            //980527
   hydraulic_properties->set_Campbell_b_parameter     (sublayer,Campbell_b_parameter_sublayer);                                                                   //051205
   hydraulic_properties->set_Campbell_b_FC_PWP_based  (sublayer,Campbell_b_FC_PWP_based_sublayer);                                                                //051205
   float64 air_entry_pot_FC_PWP_based_sublayer = parameters_ownable->layer_texture_hydraulics[layers->in_horizon_of[sublayer]]->calc_air_entry_pot                        //051205
      (FC_sublayer ,sat_water_content,Campbell_b_FC_PWP_based_sublayer);
   float64 air_entry_pot_parameter_sublayer = parameters_ownable->get_horizon_air_entry_pot(horizon);                                                                     //051205
   air_entry_pot_parameter_sublayer  =  (CORN::is_zero<float64>(air_entry_pot_parameter_sublayer))                                          //051205
      //     { This must be a pre 2.02.00 soil file, there is no parameter so used calculated }                                                                   //980527
      ? air_entry_pot_FC_PWP_based_sublayer
      : air_entry_pot_parameter_sublayer;                                                                                                                         //980527
   hydraulic_properties->set_air_entry_potential_FC_PWP_based(sublayer,air_entry_pot_FC_PWP_based_sublayer);                                                      //051205
   hydraulic_properties->set_air_entry_potential_parameter(sublayer,air_entry_pot_parameter_sublayer);                                                            //051205
   {  float64 sat_hydraul_cond_m_d_horizon = parameters_ownable->get_horizon_sat_hydraul_cond_m_d(horizon); //160916
      hydraulic_properties->set_saturated_hydraulic_conductivity_m_d(sublayer,sat_hydraul_cond_m_d_horizon); //150926
   }
   NH4_Langmuir.K.set(sublayer,parameters_ownable->NH4_Langmuir.K.get(horizon));         //190404
   NH4_Langmuir.Q.set(sublayer,parameters_ownable->NH4_Langmuir.Q.get(horizon));         //190404
#endif
   float64 initial_WC_limit = 9999.9;
      // For cascade modes we will throw the excessive amount to drainage
      // Initialization is total water content, not necesarily liquid water content
      // FD has no limits
   switch (infiltration_model)
   {  case CASCADE_DAILY_INFILTRATION : initial_WC_limit = FC_sublayer; break;
         // The cascade model cannot handle initial water contents that
         // exceed field capacity.  We will throw the excessive amount to drainage
         // Initialization is total water content, not necesarily liquid water   //070813
      case CASCADE_HOURLY_INFILTRATION: initial_WC_limit = sat_water_content; break;
         // The cascade hourly model cannot handle initial water contents that
         // exceed saturation.
      default: break;                                                            //170217
   }
   {
      // The cascade hourly model cannot handle initial water contents that
      // exceed saturation.  We will throw the excessive amount to drainage
      // Initialization is total water content, not necesarily liquid water content
      float64 WC_as_initialized =  hydrology->get_water_plus_ice_content_volumetric(sublayer);
      if (WC_as_initialized > initial_WC_limit )
      {  float64 excess_WC = WC_as_initialized - initial_WC_limit;
         float64 excess_water_depth = excess_WC * layers->get_thickness_m(sublayer);
         initial_drainage += excess_water_depth;
         hydrology->change_liquid_water_content_by_volumetric(sublayer,-excess_WC); // reduce the WC
      }
   }
   hydrology->set_real_wilt_point(sublayer,PWP_sublayer);                        //140610_051204
   hydrology->
      set_reference_water_content(sublayer,                                      //140610
         sat_water_content
        *pow( reference_water_potential/air_entry_pot_parameter_sublayer
            ,-1.0/Campbell_b_parameter_sublayer));
#endif
}
//_subdivide_horizon___________________________________________________________/
USDA_NRCS::Hydrologic_group Soil_base::get_hydrologic_group()              const
{ return (USDA_NRCS::Hydrologic_group) parameters_ownable->hydrologic_group_clad.get();}
//_get_hydrologic_group________________________________________________________/
Seconds Soil_base::get_hydrology_timestep_resolution_today()               const
{ return hydrology_timestep_resolution_today;}
//_get_hydrology_timestep_resolution_today__________________________2006-05-10_/
float64 Soil_base::get_reported_drainage()                                 const
{  return infiltration_submodel
   ? infiltration_submodel->get_reported_drainage() : 0.0;
}
//_get_reported_drainage____________________________________________2007-01-18_/
float64 Soil_base::get_infiltration()                                      const
{  return infiltration_submodel
   ? infiltration_submodel->get_infiltration() : 0.0;
}
//_get_infiltration_________________________________________________2007-01-18_/
float64 Soil_base::get_water_content_at(nat8 sublayer,float64 potential)   const
{  Soil_texture_hydraulics::Status WC_status = Soil_texture_hydraulics::unknown; //000405
   float WC_to_get = 0.0; // to be calculated                                    //000405
   return parameters_ownable->layer_texture_hydraulics
      [layers->in_horizon_of[sublayer]]->get_WC_at
         (potential,(float32)WC_to_get,WC_status,false);
}
//_get_water_content_at_____________________________________________1999-11-19_/
float64 Soil_base::get_act_water_entering_soil()                           const
{ return infiltration_submodel->get_act_water_entering_soil();
}
//_get_act_water_entering_soil______________________________________2015-09-25_/
float64 &Soil_base::mod_act_water_entering_soil()
{  return infiltration_submodel->mod_act_water_entering_soil();
}
//_mod_act_water_entering_soil__________________________2016-04-12__2015-09-30_/
RENDER_INSPECTORS_DEFINITION(Soil_base)
{
   #ifdef CS_OBSERVATION
   KNOW_EMANATOR(simdate,CS_NOT_EPHEMERAL);                                      //171114
   emanator_known = &emanator;
   evaporator->render_inspectors(inspectors,&emanator,"evaporation");
   // Should call the evaporator
   const Soil::Hydrology_interface *hydrology = ref_hydrology();                 //160412
   if (hydrology) hydrology->render_inspectors(inspectors,&emanator,"hydrology");
   return emanator_known; // NYI
   #else
   return 0;
   #endif
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-09_/
//060306 890 lines
//060731 621 lines
//090114 531 lines
//170224 615 lines

