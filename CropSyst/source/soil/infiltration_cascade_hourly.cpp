﻿#include "options.h"
#include <math.h>
#ifndef moremathH
#  include "corn/math/moremath.h"
   using namespace std;
#endif
#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#include "soil/infiltration_cascade_hourly.h"
#include "soil/surface_infiltration_green_ampt.h"
#include "soil/dynamic_water_entering.h"
#include "soil/chemicals_profile.h"
#include "physics/standard_constants.h"
#include "mgmt_types.h"
#ifdef SCRUTINIZE
nat32 test_hour = 0;
#endif
namespace Soil {                                                                 //181206
//______________________________________________________________________________
  Infiltration_cascade_hourly
::Infiltration_cascade_hourly
(const Soil::Layers_interface     &soil_layers_                                  //140424
,const Soil::Texture_interface    &soil_texture_                                 //140424
,const Soil::Hydraulic_properties_interface &soil_hydraulic_properties_          //140423
,Soil::Hydrology_interface        &hydrology_                                    //060512
,CropSyst::Soil_runoff           *runoff_                                        //131101_130308
,bool                             using_numerical_runoff_model_                 //200316
,CropSyst::Pond                  &pond_                                          //130308
)                                                                                //131001
: Infiltration_cascade_common
   (soil_layers_
   ,soil_hydraulic_properties_                                                   //140423
   ,hydrology_
   ,runoff_
   ,pond_)                                                                       //130308_070116
, soil_texture                                                   (soil_texture_)
, number_layers                                                              (0) //070123
, surface_infilt_Green_Ampt                                                  (0)
, microirrigation_excess                                                     (0) //200309
, using_numerical_runoff_model                   (using_numerical_runoff_model_) //200316
{  clear_layer_array64(hours_to_field_capacity);                                 //080610
}
//_Infiltration_cascade_hourly:constructor_____________________________________/
bool Infiltration_cascade_hourly::initialize()                   initialization_
{  bool inited = Soil::Infiltration_cascade_common::initialize();
   if (runoff && using_numerical_runoff_model)
      //200316 runoff->using_numerical_runoff_model()
      surface_infilt_Green_Ampt =new Soil::Surface_infiltration_Green_Ampt       //140820
         (soil_layers,soil_hydraulic_properties,hydrology
         //200316unused ,runoff
         ,pond);         //170428
   for (nat8 layer = 1;layer <= soil_layers.count(); layer++)
   {  float64 percent_clay = soil_texture.get_clay_percent(layer);
      hours_to_field_capacity[layer]=calc_hours_to_field_capacity(percent_clay); //120716
   }
   return inited;                                                                //131217
}
//_initialize_______________________________________________________2008-06-09_/
bool Infiltration_cascade_hourly::infiltrate
(float64                       water_entering_soil_not_irrigation_not_used_by_this_model
,float64                       management_irrigation_daily_unused
,float64                       management_auto_irrigation_concentration_unused /* this parameter used only by F.D. model */
,Dynamic_water_entering_soil *non_runoff_water_entering_in_intervals_mm          //990204
,Dynamic_water_entering_soil *irrigation_water_entering_in_intervals_mm_optional //070119
,const Dynamic_array<float32> &runon_in_intervals_unused   /* used only by F.D. model */ //990409
,Dynamic_array<float32>       &runoff_in_intervals                               //130127 now used
)
{  // Cascade never gives additional runoff.
   Soil::Infiltration_cascade_common::infiltrate                                  //070118
      (water_entering_soil_not_irrigation_not_used_by_this_model
      ,management_irrigation_daily_unused
      ,management_auto_irrigation_concentration_unused
      ,non_runoff_water_entering_in_intervals_mm
      ,irrigation_water_entering_in_intervals_mm_optional
      ,runon_in_intervals_unused
      ,runoff_in_intervals);
   // Read water entering soil data
   // In C++ version version hours are 0 indexed whereas V.B. Hour = 1
   /*200226
   float64 precip_and_irrigation_hourly_mm[24]; // setup in the next statement
   for (Hour hour = 0; hour < 24; hour++)
   {  float64 precip_hourly = non_runoff_water_entering_in_intervals_mm
         ? non_runoff_water_entering_in_intervals_mm->get(hour)  : 0;            //140226
      float64 irrig_hourly = irrigation_water_entering_in_intervals_mm_optional
         ? irrigation_water_entering_in_intervals_mm_optional->get(hour) : 0;
      precip_and_irrigation_hourly_mm[hour] = precip_hourly + irrig_hourly;
   }
   */
   soil_layer_array64(old_water_content); clear_layer_array64(old_water_content);// used for chemical transport   // 070827 rename to old_liquid_water_content 070116
   number_layers = soil_layers.count();
   for (Hour hour = 0; (hour < 24); hour++)                                               // Hour = 1
   {
      clear_layer_array64(water_flow); // m/hr                                   //130614
      // Retrieve hourly irrigation                                              // Do
      for (Layer layer = 1; layer <= number_layers; layer++)
      { old_water_content[layer] = hydrology.get_liquid_water_content_volumetric(layer);
        if (CORN::is_zero<float64>(old_water_content[layer],/*200127 0.0,*/0.000001))
        old_water_content[layer] = 0.0000001;  //To avoid div by zero  // may occur if freezing
      }  // old_water_content is needed for chemical transport
      float64 precip_hour_m
         = non_runoff_water_entering_in_intervals_mm
         ? mm_to_m(non_runoff_water_entering_in_intervals_mm->get(hour))
         : 0.0;
      float64 irrigation_hour_m
         = irrigation_water_entering_in_intervals_mm_optional
         ? mm_to_m(irrigation_water_entering_in_intervals_mm_optional->get(hour))
         : 0.0;

      /*200226
      float64 precip_and_irrigation_hourly_m                            // Hourly_Irrigation = Hourly_Irrigation_Amount(Hour)
         = mm_to_m(precip_and_irrigation_hourly_mm[hour]);              // Hourly_Irrigation = Hourly_Irrigation / 1000 'Convert mm to m
      */
      float64 runon_interval = runon_in_intervals_unused.get((nat32)hour);       //130308
      float64 pond_depth_m = pond.relinquish_depth();                            //130308

      #ifdef SCRUTINIZE
      // When comparing with FD_and Hourly Cascade Infiltration
      // VB code has no pond storage
      pond_depth_m = 0;
      #endif

      float64 upflow_m = 0; // meter                                             //130308
      float64 natural_water_m = precip_hour_m+runon_interval+pond_depth_m;
      process_hour                                                               //140423
         (hour                                                                   //130325
         ,natural_water_m
         ,irrigation_hour_m                                                      //200226
         //200226 ,precip_and_irrigation_hourly_m +runon_interval+pond_depth_m
         ,upflow_m);                                                             //130127

//if (upflow_m > 0.00001)
//std::clog << "upflow:" << upflow_m << std::endl;

      float64 not_ponded_runoff = pond.set_depth_with_runoff(upflow_m);          //160413RLN
      if (runoff
          //200316&& runoff->using_numerical_runoff_model()                              //130430
          && using_numerical_runoff_model                                        //200316
          && not_ponded_runoff > 0.0)                                            //160413RLN
      {  runoff->add_runoff_from_infiltration(not_ponded_runoff);                //160413_130308
         runoff_in_intervals.set(hour,not_ponded_runoff);                        //160413_130308_130127
      }
      // Now  communicate results to other submodels.
      hydrology.record_water_content_for_hour(hour);                             //070814
      //RLN Not used in C++V       Profile_Water_Depth = Profile_Water_Depth + Drainage_Flow_mm
      if (chemicals)                                                             //070119
      {  soil_layer_array64(liquid_water_content);                               //080127
         hydrology.get_water_content_volumetric_liquid_profile(liquid_water_content);//080127
         chemicals->                                                             //070119
         chemical_transport_and_exchange
         (CASCADE_HOURLY_INFILTRATION
         ,number_layers
         ,number_layers // this parameter is dummy it is used only for F.D.      //080219
         ,3600*hour  // start time                                               //71015
         ,3600 /* 1 hour duration*/                                              //71015
         ,old_water_content                                                      //070813
          // Warning (I dont know if old_water_content should be only liquid water content or total water content - was liquid only)
         ,liquid_water_content                                                   //070813_990311
          // Warning ( should this be only liquid water content or total water content - was liquid only)
         ,irrigation_hour_m + precip_hour_m                                      //200226
            //not sure if precip needs to be natural_water_m
         //200226 ,precip_and_irrigation_hourly_m
         ,water_flow                                                             //080208
         ,0.0 // no deferment in cascade model                                   //071016
         ,override_leaching_depth                                                //981230
         ,soil_layers.get_layer_at_depth( leaching_depth)                        //981230
         );
      }                                                                          //080127
   } // for each hour
   infiltration =  balance_drainage;                                             //070116
   if (!  override_leaching_depth)                                               //990106
            reported_drainage =  balance_drainage;                               //990106
   return  true;                                                                 //080122_070116
}
//_infiltrate__________________________________________________________________/
void Infiltration_cascade_hourly::process_hour                                   //140423
(Hour hour                                                                       //130325
,float64 natural_water_m                                                         //200226
,float64 irrigation_m                                                            //200226
//200226 ,float64 water_input_m /* pass by value only local copy updated */
,float64 &surface_out_flow)                                                      //130127
{  // Infiltrate precipitation and irrigation (saturate top layers)
   // Note that the caller should check that water_input_m > 0
   // If this is not the case, this function need not be called;
   // (it is not erroneous, but this function has no effect when there is no input water flow.)
   soil_layer_array64(water_content_liquid);
   clear_layer_array64(water_content_liquid);

   float64 water_input_m = irrigation_m + natural_water_m;                    //200226

#ifdef SCRUTINIZE
test_hour++;
#endif


   float64 water_potential_after_drainage = 0;                                   //130127
   float64 saturated_cascade_flow_out = 0.0;

   // Hours_To_Field_Capacity is setup in constructor.
   #ifdef MICROBASIN_VERSION
   balance_drainage = 0.0;                                                       //141204LML
   #endif

   const Seconds time_step = 3600;
   // Note that this implies this could probably be made time generic.
   number_layers = soil_layers.count();
   nat8 start_layer = 1;


   for (nat8  layer = start_layer                                                //130313
       ; layer <= number_layers
       ; layer++)
   {  water_content_liquid[layer] = hydrology.get_liquid_water_content_volumetric(layer);
   }                                                                             //Next Layer
   // WARNING  I am not sure  hydraulic_conductivity_saturated_min/*170426 Minimum_Saturated_Hydraulic_Conductivity*/
   // is set to a value when not in Runoff  (stays at default 0.0?)

   float64 Water_Depth_To_Infiltrate = 0;                                        //200309


   float64 irrigation_m_1 = irrigation_m;
   float64 irrigation_m_2 = 0;
   if ((wetting_area_surface_fract < 1.0) && (irrigation_m > 0.0))
   {  // microirrigation
      float64 thickness_1 = soil_layers.get_thickness_m(1);
      float64 FC = soil_hydraulic_properties.get_field_capacity_volumetric(1);
      float64 WC = hydrology.get_liquid_water_content_volumetric(1);
      // Presuming irrigation doesn't occur during freezing.
      float64 to_FC = (FC > WC)                                                  //200309RLN
         ? (FC - WC) * thickness_1
         : 0.0;                                                                  //200309RLN

      float64 ELI = irrigation_m * wetting_area_surface_fract;
      /* Of the irrigation amount (pulse_irrigation),
         the irrigation amount allocated to the top evaporative layer
         (ELI; only evaporation, no roots) is ELI =  pulse_irrigation x  FWSA.
      */
      ELI = CORN::must_be_less_or_equal_to<float64>(ELI,to_FC);
      /* (ELI=< the amount of water to wet the top layer to field capacity;
         I am sure this condition will normally be met, but we should check it).
      */
      irrigation_m_1 = ELI;
      irrigation_m_2 = irrigation_m - ELI;
      irrigation_m = irrigation_m_1;
      water_input_m = natural_water_m + irrigation_m_1;
      Water_Depth_To_Infiltrate = water_input_m;                                 //200309
   } else

   {
      Water_Depth_To_Infiltrate = water_input_m;
      if (surface_infilt_Green_Ampt)
      {
         Water_Depth_To_Infiltrate = surface_infilt_Green_Ampt->update
               (hour, water_input_m, water_content_liquid[1]
               ,surface_out_flow ,number_layers);                                //140819
      }
   }
   irrigation_m_2 += microirrigation_excess;                                     //200309
   microirrigation_excess = 0;                                                   //200309
   water_flow[0] = Water_Depth_To_Infiltrate;

   if (Water_Depth_To_Infiltrate > 0)
   {  // Cascade water available to infiltrate first, leaving layers at saturation. Redistribution is applied later below.
      saturated_cascade_flow_out = 0.0;
      for (nat8 layer = 1; layer <= number_layers; layer++)
      {
         bool last_layer = layer == number_layers;                               //140820
         float64 WC_sat_ice_adjusted
            = soil_hydraulic_properties.get_saturation_water_content_volumetric(layer,hydrology.get_ice_content_volumetric(layer));
         float64 thickness_layer =
            soil_layers.get_thickness_m(layer);
         // reenabled the following block in responding to Claudio's comment     //151013LML
         // This section limits flow down if insufficient saturated hydraulic conductivity

         /*170428 Not in VB code ?
         float64 Sat_Cond_Limited_Flow_In = soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(layer); //'Convert kg*s/m3 to m/h
         if (Water_Depth_To_Infiltrate > Sat_Cond_Limited_Flow_In)
         {
            float64 Excess_Flow_In = Water_Depth_To_Infiltrate - Sat_Cond_Limited_Flow_In;
            Water_Depth_To_Infiltrate = Sat_Cond_Limited_Flow_In;
            water_flow[layer-1] = Water_Depth_To_Infiltrate;                     //COS-FMS May 26 , 2015 ' Recalculate the water flow from layer above; FMS May 27, 2015 changed to module variable SoilStateRef.WaterFlow(Layer)  by Water_Flow
            surface_out_flow += distribute_excess_flow_upwards(layer, Excess_Flow_In,water_content_liquid);
         }
         */
         float64 Available_Storage_To_Saturation
            = (WC_sat_ice_adjusted - water_content_liquid[layer]) * thickness_layer;

        if (Available_Storage_To_Saturation < 0.0)
        {   std::clog << "Warning: Available_Storage_To_Saturation ("
               << Available_Storage_To_Saturation << ") < 0!!!\n";
           Available_Storage_To_Saturation = 0.0;
           // The tilliage effect on soil moisture has errors                    //170501LML
           // (i.e. in some cases water content is greater than saturated WC)
        }
         Available_Storage_To_Saturation                                         //170426COS
         = CORN::must_be_0_or_greater<float64>(Available_Storage_To_Saturation);

         if (using_numerical_runoff_model)                                       //200316
         {  // limit saturation water cascading to the layer infiltration capacity
            // This is part of Green Ampt
            // Section added 170426
            float64 air_entry_pot_layer = soil_hydraulic_properties.get_air_entry_potential(layer,false); //not FC_PWP_based
            float64 b_value_layer =  soil_hydraulic_properties.get_Campbell_b(layer,false);
            /*200316 unused
            float64 Saturated_Hydraulic_Conductivity
               = soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(layer);
               // = K_Sat(Layer) * (Gr / Water_Density) * 3600.0; // 'm/h
            */
            float64 Conductivity_Limited_Water_Depth_To_Infiltrate
               = CalculateInfiltrationCapacity
                  (Water_Depth_To_Infiltrate
                  , soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(layer)
                  , air_entry_pot_layer
                  , water_content_liquid[layer]
                  , WC_sat_ice_adjusted
                  , b_value_layer
                  , thickness_layer);

            if (Conductivity_Limited_Water_Depth_To_Infiltrate < Water_Depth_To_Infiltrate)
            {  // Subtract excess water from saturated water content cascading
               float64 excess // meter
                  = Water_Depth_To_Infiltrate - Conductivity_Limited_Water_Depth_To_Infiltrate;
               if (irrigation_m > 0)                                             //200309
                  microirrigation_excess += excess;                              //200309
                  // Assuming excess is from irrigation
                  // but this is not necessarily the case.
               else                                                              //200309
                  surface_out_flow += excess; // Add_To_Runoff;
               // only for VB balance/output Process.SumRunoff = Process.SumRunoff + m_to_mm(excess)
               Water_Depth_To_Infiltrate = Conductivity_Limited_Water_Depth_To_Infiltrate;
            } // if
         }
         float64 depth_layer = soil_layers.get_depth_m(layer);
         if (Water_Depth_To_Infiltrate > Available_Storage_To_Saturation)
         {  water_content_liquid[layer] = WC_sat_ice_adjusted;                   //130709
            Water_Depth_To_Infiltrate -=  Available_Storage_To_Saturation;       //130709
            water_flow[layer] = Water_Depth_To_Infiltrate; // m/h                //130709
            if (surface_infilt_Green_Ampt)                                       //170426
            {  surface_infilt_Green_Ampt->wetting_front_depth = depth_layer;     //170426
               surface_infilt_Green_Ampt->wetting_front_layer = layer;           //170426
            }
            if (last_layer && (Water_Depth_To_Infiltrate > 0))                   //140419
            {   // Drainage obtained during saturation cascading
                saturated_cascade_flow_out = Water_Depth_To_Infiltrate;          //140419
                water_flow[number_layers] = saturated_cascade_flow_out;          //150922COS
                Water_Depth_To_Infiltrate = 0.0;
            }
         } else
         {  water_content_liquid[layer] += Water_Depth_To_Infiltrate / thickness_layer; //140814COS This was moved from above
            Water_Depth_To_Infiltrate = 0.0;
            water_flow[layer] = 0.0; // 'm/h
            if ((layer == 1) && (irrigation_m_2 > 0.0))
               //remaining directed irrigation goes to layer 2 in next timestep  //200226
               Water_Depth_To_Infiltrate += irrigation_m_2;                      //200226
            else                                                                 //200226
               layer = number_layers;
         } // if  Water_Depth_To_Infiltrate > Available_Storage_To_Saturation
      } // for layer                                                                                                                                                                                      //     Next Layer
   } else
      if (surface_infilt_Green_Ampt) surface_infilt_Green_Ampt->clear_wetting_front();
      //now using clear_wetting_front Wetting_Front_Depth = 0; // 'ín m. Set to zero when water entering the soil is zero (no rainfall or irrigation)
   float64 flow_out = 0.0;
   float64 flow_out_sum = 0.0;
   nat8 first_drainable_layer = 0;
   bool redistribution = false;
   if (water_input_m > 0.0)                                                      //200226_080702
   {  first_drainable_layer = start_layer;                                       //130311
      redistribution = true;                                                     //120716
   } else  /* Although there is no infiltration, there could be drainage:*/      //080702
   {  for (nat8 layer =start_layer                                               //130311
          ; (layer <= number_layers) && (first_drainable_layer == 0)
          ; layer++)                                                             //120716
      {  float64 WC_layer_curr = hydrology.get_liquid_water_content_volumetric(layer); //080702
         float64 field_capacity_layer = get_field_capacity_ice_corrected(layer); //130128 restored 080702
         if (WC_layer_curr> field_capacity_layer + 0.000000100001)               //150926_080702
         {  first_drainable_layer = layer;                                       //080702
            redistribution = true;                                               //120716
         }
      }
   }
   if (redistribution)                                                           //120716
   {  float64 flow_in = 0;                                                       //200226_130325
         // Flow in top layer = 0 because water was already infiltrated via saturation cascading.
         // Now we are only distributing water from one layer to the next layer down
      for (nat8 layer = first_drainable_layer                                    //080702 090417
           ; layer <= number_layers ; layer++)
      {  // This redistribution function was completely rewritten Jan 2013
         float64 thickness_layer = soil_layers.get_thickness_m(layer);
         float64 WC_sat_ice_adjusted
            = soil_hydraulic_properties.get_saturation_water_content_volumetric
               (layer,hydrology.get_ice_content_volumetric(layer));              //140822
         float64 FC_layer =  soil_hydraulic_properties.get_field_capacity_volumetric(layer);
         float64 b_value_layer =  soil_hydraulic_properties.get_Campbell_b(layer,false); //140124
         float64 m_layer = 2.0 * b_value_layer + 3.0;   // This could be moved to hydraulic properties (Not sure what this is)
         float64 one_minus_m_layer = 1.0 - m_layer;
         float64 m_minus_one_layer = m_layer - 1.0;
         float64 hours_to_field_capacity_layer = hours_to_field_capacity[layer];
         float64 apparent_K_sat_layer = thickness_layer * water_density
            * pow(WC_sat_ice_adjusted , m_layer)
            * (pow(FC_layer , one_minus_m_layer)
               - pow(WC_sat_ice_adjusted , one_minus_m_layer))
            / (standard_gravity * hours_to_field_capacity_layer
               * 3600.0 * m_minus_one_layer);
         apparent_K_sat_layer = CORN::must_be_greater_or_equal_to
            (apparent_K_sat_layer,0.000000001);                                  //140120
         // This represents the change in water content from saturation
         //  in one hour. This water will move to the next layer down.
         float64 one_hour_water_content_change_at_saturation
            = WC_sat_ice_adjusted
             - pow((pow(WC_sat_ice_adjusted  ,one_minus_m_layer)
                   +  (standard_gravity * apparent_K_sat_layer * m_minus_one_layer * time_step)
                    / (thickness_layer * water_density * pow(WC_sat_ice_adjusted , m_layer)))
                  , (1.0 / one_minus_m_layer));
         float64 excess_flow_in = 0.0;   // m/h

         // determines if water flow into the current layer does not exceed
         // the maximum infiltration rate into that layer. If exceeded, flow upward is calculated.
         // does not exceed the maximum infiltration rate into that layer.
         // If exceeded, flow upward is calculated.
         // However, no upward flow (runoff) from the top layer is possible
         // since we are only redistributing water already in the soil.
         {  float64 flow_in_max = ((WC_sat_ice_adjusted - water_content_liquid[layer]) + one_hour_water_content_change_at_saturation) * thickness_layer;
            // Now, check how much water can be infiltrated assuming saturation
            float64 Sat_Cond_Limited_Flow_In = //m/hr                            //140116
               soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(layer);
            flow_in_max =std::min<float64>(flow_in_max,Sat_Cond_Limited_Flow_In);//140116

            /*DEBUG！！！
            if (layer == number_layers) {
                std::clog<<"flow_in_max:"<<flow_in_max
                         <<"\tSat_Cond_Limited_Flow_In:"<<Sat_Cond_Limited_Flow_In
                         <<"\tflow_in_max:"<<flow_in_max
                         <<"\tflow_in:"<<flow_in
                         <<std::endl;
            }*/

            if (flow_in > flow_in_max)
            {
               excess_flow_in = flow_in - flow_in_max;
               flow_in = flow_in_max;                                            //140116
               water_flow[layer-1] -= excess_flow_in;                            //150922COS
               surface_out_flow += distribute_excess_flow_upwards
                  (layer, excess_flow_in,water_content_liquid);                  //140116
            }
         }
         float64 one_hour_WC_change = 0.0;                                       //130709

         // This section determines how much water can potentially be moved in
         // one time step (1 hour) from the current layer to the next layer down.
         // Roger 4/28/2017 Change next line (just the variable name)
         float64 WC_before_distribution_layer
            = std::min<float64>
               (water_content_liquid[layer] + flow_in / thickness_layer
               ,WC_sat_ice_adjusted);
         float64 WC_timestep_mean                                                //170428
            = (water_content_liquid[layer] + WC_before_distribution_layer) /2.0;

         if (WC_before_distribution_layer > FC_layer)
         {  one_hour_WC_change
               = WC_timestep_mean
                -pow( (pow(WC_timestep_mean ,one_minus_m_layer)
                   + (standard_gravity * apparent_K_sat_layer * (m_layer - 1) * time_step)
                     / (thickness_layer * water_density * pow(WC_sat_ice_adjusted , m_layer)))
                    , (1.0 / one_minus_m_layer));
            if (one_hour_WC_change > (WC_before_distribution_layer - FC_layer ))
               one_hour_WC_change = WC_timestep_mean - FC_layer;
         } else
            one_hour_WC_change = 0;
            // End of the section that determines how much water can potentially be moved in one time step (1 hour) from the current layer to the next layer down.
         //170428 if (one_hour_WC_change < 0) one_hour_WC_change = 0.0; // must be greater than 0
         one_hour_WC_change = CORN::must_be_0_or_greater<float64>(one_hour_WC_change); //170428
         float64 air_entry_pot_layer = soil_hydraulic_properties.get_air_entry_potential(layer,false); //not FC_PWP_based
         // Determine the actual flow out from each layer
         if (layer > first_drainable_layer)
         {
            if (layer < number_layers)                                           //170426
            { //Determines the water content to preserve the same water potential after drainage as the layer above

               float64 WC_at_water_potential_after_drainage
                  =  WC_sat_ice_adjusted
                    * pow((water_potential_after_drainage / air_entry_pot_layer)
                         , (-1.0 / b_value_layer));
               WC_at_water_potential_after_drainage = CORN::must_be_between<float64>
                  (WC_at_water_potential_after_drainage,FC_layer, WC_sat_ice_adjusted);
               // End of Determination of the water content to preserve the same water potential after drainage as the layer above
               float64 water_content_change_for_equilibrium_water_potential
                  = water_content_liquid[layer] + flow_in / thickness_layer - WC_at_water_potential_after_drainage;
               water_content_change_for_equilibrium_water_potential
                  = CORN::must_be_0_or_greater<float64>
                     (water_content_change_for_equilibrium_water_potential);
               if (water_content_change_for_equilibrium_water_potential > one_hour_WC_change)
               {   water_content_liquid[layer] = WC_at_water_potential_after_drainage;
                   flow_out = water_content_change_for_equilibrium_water_potential * thickness_layer;
               } else
               {  //
                  water_content_liquid[layer] += flow_in / thickness_layer - one_hour_WC_change;
                  flow_out = one_hour_WC_change * thickness_layer;
               }
            }
            else // Process last layer
            {

#ifdef SCRUTINIZE
if (test_hour == 49)
std::clog << ' ';
#endif

               float64 WP_m_layer
                  = J_per_kg_to_m( air_entry_pot_layer                           //170426
                     * pow(water_content_liquid[layer] / WC_sat_ice_adjusted     //170426
                          ,-b_value_layer));                                     //170426
                  //170428  implement this
                  // LayerWaterPotentialMeters(Air_Entry_Potential(Layer), Water_Content(Layer), WC_Sat(Layer), b_Value(Layer))
               float64 air_entry_pot_m_layer=J_per_kg_to_m(air_entry_pot_layer); //170428
               if (WP_m_layer  > air_entry_pot_m_layer)                          //170426
                   WP_m_layer  = air_entry_pot_m_layer;                          //170426
               water_content_liquid[layer] += flow_in / thickness_layer;         //170426
               if (water_content_liquid[layer] < FC_layer)                       //170426
                    flow_out = 0;                                                //170426
               else                                                              //170426
               {  flow_out = std::min<float64>                                   //170426
                     (soil_hydraulic_properties.get_sat_hydraul_cond_m_hr(layer) //170426                                             //170426
                      * pow((air_entry_pot_m_layer / WP_m_layer )                //170426
                           , (2.0 + 3.0 / (-b_value_layer)))                     //170426
                     ,(water_content_liquid[layer]-FC_layer) * thickness_layer); //170426
                  water_content_liquid[layer] -= flow_out / thickness_layer;     //170426
                } // if WC < FC                                                  //170426
            } // if last layer
         } else  //(layer < first_drainable_layer)
         {  water_content_liquid[layer] += flow_in / thickness_layer - one_hour_WC_change;
            flow_out = one_hour_WC_change * thickness_layer;
        }
        water_potential_after_drainage =
               air_entry_pot_layer
                  * pow(water_content_liquid[layer] / WC_sat_ice_adjusted
                        ,- b_value_layer);
        water_potential_after_drainage =
               CORN::must_be_between<float64>                                    //170426
                  (water_potential_after_drainage
                  ,soil_hydraulic_properties.get_water_pot_at_FC(layer)
                  ,0.0);

        water_flow[layer] += flow_out; // flow is in kg/m2/h or mm/h
        bool last_layer = layer == number_layers;                                //170426
        float64 depth_layer = soil_layers.get_depth_m(layer);                    //170426
        if (surface_infilt_Green_Ampt)                                           //170426
        {   // This section determines the new wetting front position, if changed. Only one layer at a time can be the new location of the water front
            if ((layer-1) == surface_infilt_Green_Ampt->wetting_front_layer)     //170426
            {   float64 WC_sat_plus_FC = WC_sat_ice_adjusted + FC_layer;         //170426
               if (   (flow_in > 0)                                              //170426
                   && (water_content_liquid[layer] > FC_layer)                   //170426
                   && (water_content_liquid[layer] < (WC_sat_plus_FC / 2.0)))    //170426
                  surface_infilt_Green_Ampt->update_wetting_front                //170426
                   (layer,last_layer,depth_layer,water_content_liquid[layer+1]); //170427
            }
        }
        flow_in = flow_out;
        flow_out_sum += flow_out;
        //RLN WC is set below Soil.NewWaterContent(layer) = water_content[layer]
        if (override_leaching_depth && (layer==layer_at_reported_drainage_depth))//130205
           reported_drainage += (flow_out + saturated_cascade_flow_out);         //140814
      } // for layer
   } // redistribution
   for (nat8 layer = first_drainable_layer                                       //080702 090417
       ; layer <= number_layers ; layer++)
   {  float64 change_WC =  water_content_liquid[layer] - hydrology.get_liquid_water_content_volumetric(layer);
      hydrology.change_liquid_water_content_by_volumetric(layer,change_WC);
   }

   for (nat8 layer = 1
       ; layer <= number_layers ; layer++)
   {

#ifdef MICROBASIN_VERSION
        //170510LML there are some errors in the above processes. Need to be checked!!!
        //Here is a temporal solution, i.e. put extra water into drainage!!!
        float64 vwc = hydrology.get_liquid_water_content_volumetric(layer);
        float64 vwc_sat = soil_hydraulic_properties.get_saturation_water_content_volumetric(layer,hydrology.get_ice_content_volumetric(layer));
        if (vwc > vwc_sat) {
            std::cerr << "layer:" << (int)layer << "\tvwc:" << vwc << " > vwc_sat:" << vwc_sat << "!!!" << std::endl;
            hydrology.change_liquid_water_content_by_volumetric(layer,vwc_sat - vwc);
            balance_drainage += (vwc - vwc_sat) * soil_layers.get_thickness_m(layer);
        }
#endif

#ifdef SCRUTINIZE
std::clog << test_hour << "\t"<< (int)layer  << "\t" << water_content_liquid[layer] << std::endl;
#endif


   }
#ifdef SCRUTINIZE
if (test_hour == 35)
std::clog << ' ';
#endif
   balance_drainage += (flow_out + saturated_cascade_flow_out);                  //140814
      //  this line hooks in drainage for the rest of the model.
}
//_process_hour_____________________________________________________2008-06-09_/
float64 Infiltration_cascade_hourly::distribute_excess_flow_upwards  modification_
( nat8 from_layer
, float64 &excess_flow_in
, soil_layer_array64(water_content))
{
   for (nat8 lyr = from_layer; (lyr > 1) && (excess_flow_in > 0.0) ; lyr--)
   {  // not computed for layer 1 which is simply the excess
      nat8 lyr_above = lyr-1;
      nat8 lyr_above_2 = lyr-2;
      float64 WC_sat_layer_above
            = soil_hydraulic_properties.get_saturation_water_content_volumetric(lyr_above,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
      float64 thickness_layer_above =  soil_layers.get_thickness_m(lyr_above);
      float64 Available_Porosity = (WC_sat_layer_above - water_content[lyr_above]) * thickness_layer_above; // 'm
      // does nothing if (from_layer > 1)                                        //170426COS
#ifndef MICROBASIN_VERSION
      water_flow[0] -= excess_flow_in;                                           //170508COS
#endif
         // Upflow unlikely to happen because we are generally only moving
         // water down that is already in layers, so usually not runoff.
      if (excess_flow_in > Available_Porosity)
      {  water_content[lyr_above] = WC_sat_layer_above;
         excess_flow_in -= Available_Porosity;
         //if (lyr > 1)                                                          //150922COS
         if (water_flow[lyr_above_2])
             water_flow[lyr_above_2] -= excess_flow_in;                          //150922COS_141010FMS_150603LML
      } else
      {  water_content[lyr_above] +=  excess_flow_in / thickness_layer_above;
         excess_flow_in = 0.0;
         // loop breaks with condition (excess_flow_in > 0.0) above lyr = 1;     //
      }
   }
   //170503 water_flow[0] -= excess_flow_in;                                              //150922COS
   return excess_flow_in;
}
//_distribute_excess_flow_upwards___________________________________2014-01-16_/
float64 Infiltration_cascade_hourly::calc_hours_to_field_capacity           // VB: HoursToFieldCapacity
(float64 percent_clay)                                                     const
{  //Determine number of hours to reach field capacity if the user selected default calculation

// Verhulst-Pearl: f(t)=L0/(L0+(Lm-L0)*e^-rt)

   static const float32 L0 = 0.05;  //initial value
   static const float32 Lm = 1.0;   //max_value
   float32 x_known = (Lm + L0) / 1.1;
   float32 rtm = (70.0  + 0.0) / 2.0; // 140124 was 60.0
   float32 xR = x_known / Lm;
   // calculating r based on the known values
   float32 r = -(1.0 / rtm) * log(((L0 / xR) - L0) / (Lm - L0));
   float32 tt = (percent_clay - 12.0);
   float32 HTFC = (L0 / (L0 + (Lm - L0) * exp(-r * tt))) * 20.0 * 24.0;
   if (HTFC < 24)  HTFC = 24;                                                    //140415
   return HTFC;
}
//_calc_hours_to_field_capacity_____________________________________2013-01-27_/
float64 Infiltration_cascade_hourly::CalculateInfiltrationCapacity
(float64 WaterDepthToInfiltrate
,float64 Saturated_Hydraulic_Conductivity
,float64 Air_Entry_Pot
,float64 WC
,float64 WCsat
,float64 Bvalue
,float64 LayerThickness)
{
   // limits saturation water cascading to the layer infiltration capacity
   float64 Water_Potential_Above_Cascading_Front_m=J_per_kg_to_m(Air_Entry_Pot); //  'Convert J/kg to m
   float64 Water_Potential_Below_Cascading_Front_m
      = J_per_kg_to_m(Air_Entry_Pot
      * pow(WC / WCsat,-Bvalue));
   float64 Average_Water_Potential_m
      = (  Water_Potential_Above_Cascading_Front_m
         + Water_Potential_Below_Cascading_Front_m) / 2.0;
   float64 Layer_Wetting_Front = LayerThickness     / 2.0;
   float64 Conductivity_Limited_Water_Depth_To_Infiltrate
      = std::min<float64>
         (Saturated_Hydraulic_Conductivity
          * (Water_Potential_Above_Cascading_Front_m
              - (Average_Water_Potential_m - Layer_Wetting_Front))
            / Layer_Wetting_Front
         ,WaterDepthToInfiltrate);
   return Conductivity_Limited_Water_Depth_To_Infiltrate;
}
//_CalculateInfiltrationCapacity____________________________________2017-04-27_/
}//_namespace_Soil_____________________________________________________________/

/*
Function LayerWaterPotentialMeters(AEP As Double, WC As Double, WCsat As Double, bValue As Double) As Double
LayerWaterPotentialMeters = AEP * (WC / WCsat) ^ (-bValue) * 0.102               'Convert J/kg to m
End Function
*/

// 359 lines  276, 221
// 071206 210 line
// 080608 340 lines
//______________________________________________________________________________

