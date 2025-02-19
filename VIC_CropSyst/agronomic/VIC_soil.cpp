#ifdef VIC_CROPSYST_VERSION
#include "VIC_soil.h"
#include "global.h"
//160629LML #include "CS_suite/observation/CS_inspection.h"
//160629LML #include "CS_suite/observation/CS_inspector.h"
namespace VIC
{
//______________________________________________________________________________
Soil_components::Soil_components
(const CORN::date32           &simdate_
,const CS::Annual_temperature &annual_temperature_                               //150424
,/*190128RLN const */ layer_data_struct      *VIC_layer // array
,const soil_con_struct        &VIC_soil_con_
)
: CropSyst::Soil_components                                                      //190128RLN
(0
,false
,simdate_                                                                        //190128
,annual_temperature_
#ifdef PROVIDES_SOIL_TEMPERATURE_PROFILE
,meteorological                                                                  //190128
#endif
)
/*190128 RLN
: CropSyst::Soil
(0
 ,false                                                                          //161031LML
#ifdef OLD_EVAPORATE                                                             //160629LML
,evaporation_potential_remaining
#endif
,today
,_annual_temperature                                                             //150424
#ifdef SOIL_TEMPERATURE
,weather
#endif
)
*/

, layers()
, hydraulic_properties(VIC_soil_con_)                                            //150331
, hydrology(layers,hydraulic_properties,*structure,VIC_soil_con_,VIC_layer)      //190131RLN
, temperature(VIC_layer)
, temperature_profile_hourly(temperature)                                        //150424
, VIC_soil_con(VIC_soil_con_)
{
    delete Soil_base::layers;
    delete Soil_base::hydraulic_properties;
    Soil_base::layers = 0;
    Soil_base::hydraulic_properties = 0;

    //200715LML
    //Soil_base::layers = &layers;
    //Soil_base::hydraulic_properties = &hydraulic_properties;
    Soil_base::hydrology = &hydrology;
}
//_Soil_components:constructor__________________________________2014-12-07_RLN_/
Soil_hydrology::Soil_hydrology
(const Soil::Layers_interface                &soil_layers_
,const Soil::Hydraulic_properties_interface  &hydraulic_properties_ref_
,const Soil::Structure                       &soil_structure_                    //190131RLN
,const soil_con_struct                       &VIC_soil_con_
,layer_data_struct                           *VIC_layer_
)
//190131 : Soil::Hydrology_interface()
: Soil::Hydrology_abstract(soil_layers_,hydraulic_properties_ref_,soil_structure_) //190131RLN
, layers                                                          (soil_layers_)
, hydraulic_properties_ref                           (hydraulic_properties_ref_)
, VIC_layer                                                         (VIC_layer_)
, VIC_soil_con                                                   (VIC_soil_con_)
, reference_water_content                                                     ()
, real_wilt_point                                                             ()
, plant_avail_water                                                           ()
{}
//_Soil_hydrology:constructor_______________________________________2019-01-30_/
float64 *Soil_hydrology::get_liquid_water_content_layers_volumetric
(soil_layer_array64(water_content_layers))                                 const
{
   assert(false); return 0; // RLN Need to implement
}
//_get_liquid_water_content_layers_volumetric___________________2014-12-07_RLN_/
float64 Soil_hydrology::get_liquid_water_content_volumetric(nat8 layer)    const
{
   nat8 VIC_layer_index = layer-1; //RLN Need to check
   return VIC_layer[VIC_layer_index].moist / m_to_mm(VIC_soil_con.depth[VIC_layer_index]);
}
//_get_liquid_water_content_volumetric__________________________2014-12-07_RLN_/
float64 Soil_hydrology::get_ice_content_volumetric(nat8 layer)             const
{
   nat8 VIC_layer_index = layer-1; //RLN Need to check
   double ice = 0.0;
#ifdef VCS_V5
   for (int frost_area = 0; frost_area < options.Nfrost; frost_area++) {
      ice += VIC_layer[VIC_layer_index].ice[frost_area] * VIC_soil_con.frost_fract[frost_area];
   }
   ice /= m_to_mm(VIC_soil_con.depth[VIC_layer_index]);
#else
   ice = VIC_layer[VIC_layer_index].ice / m_to_mm(VIC_soil_con.depth[VIC_layer_index]);
#endif
   return ice;
}
//_get_ice_content_volumetric___________________________________2014-12-07_RLN_/
float64 Soil_hydrology::get_water_potential(nat8 layer)                    const
{
   //assert(false); return 0; // RLN Need to implement
    return calc_water_potential(layer, true /*consider_ice*/, false /*FC_PWP_based*/);  //150608LML copied from C:\dev\CropSyst\source\soil\hydrology.cpp Soil_hydrology_abstract::get_water_potential
}
//_get_water_potential__________________________________________2015-06-08_LML_/
float64 Soil_hydrology::calc_water_potential( nat8 layer,bool consider_ice,bool FC_PWP_based) const
{ // Water retention curve.
   //150608 copied from C:\dev\CropSyst\source\soil\hydrology.cpp Soil_hydrology_abstract::calc_water_potential
   float64 water_content_sublayer = get_liquid_water_content_volumetric(layer);  //990316
   float64 saturation_water_content_sublayer =                                   //980730
                 hydraulic_properties_ref.get_saturation_water_content_volumetric(layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);  //051204
   float64 air_entry_pot_sublayer = get_air_entry_pot(layer,consider_ice,FC_PWP_based);     //980730
   float64 b_sublayer = hydraulic_properties_ref.get_Campbell_b(layer,FC_PWP_based);       //980730
   float64 ice_content_sl = consider_ice                                         //990315
     ?get_ice_content_volumetric(layer)                                          //060505
      : 0.0;                                                                     //990315
   float64 sat_water_cont = saturation_water_content_sublayer - ice_content_sl;  //990331
   float64 result =
     (  (water_content_sublayer < saturation_water_content_sublayer)
      &&(water_content_sublayer > 0.0000001)  //prevent FP overflow              //990606
      &&(sat_water_cont > 0.0)) // prevent div 0                                 //041111
     ? (air_entry_pot_sublayer * pow((water_content_sublayer/ sat_water_cont),-b_sublayer)) //990331
     : air_entry_pot_sublayer;
   return result;
}
//_calc_water_potential_________________________________________2016-06-29_LML_/
float64 Soil_hydrology::calc_available_water_fraction(nat8 sublayer, bool real_wilt_based) const
{
   //160629LML Copied (with some adjustments) from C:\dev\CropSyst\source\soil\hydrology.cpp
   float64 field_capacity_sl=hydraulic_properties_ref.get_field_capacity_volumetric(sublayer);
   float64 perm_wilt_sl     =hydraulic_properties_ref.get_permanent_wilt_point_volumetric(sublayer);
   float64 wilt_point_sl = real_wilt_based
      ? real_wilt_point[sublayer]
      : perm_wilt_sl;
   float64 avail_water_sl =  CORN_must_be_between
   (((get_liquid_water_content_volumetric(sublayer) - wilt_point_sl) /
    (field_capacity_sl       - wilt_point_sl))
   ,0.0,1.0);
   if (sublayer > layers.count_uninundated()) //.get_number_uninundated_layers())
      avail_water_sl = 1.0;
   return avail_water_sl;
}
//_calc_available_water_fraction_________________________________2016-06-29LML_/
/*200208 I think this is the obsolete version
float64 Soil_hydrology::calc_available_water_fraction
(nat8 layer , bool real_wilt_based)                                        const
{
    //190806LML float64 field_capacity_sl   = VIC_soil_con.Wcr[layer-1] / m_to_mm(VIC_soil_con.depth[layer-1]);
    float64 field_capacity_sl   = VIC_soil_con.VCS.Field_Capacity[layer-1] / m_to_mm(VIC_soil_con.depth[layer-1]); //190806LML
    float64 wilt_point_sl       = VIC_soil_con.Wpwp[layer-1] / m_to_mm(VIC_soil_con.depth[layer-1]);
    float64 avail_water_sl =  CORN_must_be_between
    (((get_liquid_water_content_volumetric(layer) - wilt_point_sl) /
     (field_capacity_sl       - wilt_point_sl))
    ,0.0,1.0);
    return avail_water_sl;
}
//_calc_available_water_____________________________________________2015-05-29_/
*/

float64 Soil_hydrology::calc_available_water_depth_to_depth(float64 depth) calculation_
{
   //160629LML Copied (with some adjustments) from C:\dev\CropSyst\source\soil\hydrology.cpp
   float64 AW_to_depth = 0;
   float64 remaining_depth = depth;
   nat8 to_layer = layers.get_layer_at_depth(depth);
   for (nat8 lyr = 1; lyr < to_layer; lyr++)
   {  float64 lyr_thickness = layers.get_thickness_m(lyr);
      float64 perm_wilt_lyr = hydraulic_properties_ref.get_permanent_wilt_point_volumetric(lyr);
      float64 WC_lyr        = get_liquid_water_content_volumetric(lyr);
      float64 AW_volumetric = WC_lyr - perm_wilt_lyr;
      float64 AW_depth_layer = AW_volumetric * lyr_thickness;
      AW_to_depth += AW_depth_layer;
      remaining_depth -= lyr_thickness;
   }
   // add bit in final layer
   float64 perm_wilt_last     = hydraulic_properties_ref.get_permanent_wilt_point_volumetric(to_layer);
   float64 WC_last            = get_liquid_water_content_volumetric(to_layer);
   float64 AW_volumetric_last = WC_last - perm_wilt_last;
   float64 AW_depth_layer_last= AW_volumetric_last * remaining_depth;
   AW_to_depth += AW_depth_layer_last;
   return AW_to_depth;
}
//_calc_available_water_depth_to________________________________2016-08-12_LML_/
float64 Soil_hydrology::calc_ice_depth_to(float64 depth)            calculation_
{
   float64 ICE_to_depth = 0;
   float64 remaining_depth = depth;
   nat8 to_layer = layers.get_layer_at_depth(depth);
   for (nat8 lyr = 1; lyr < to_layer; lyr++)
   {  float64 lyr_thickness = layers.get_thickness_m(lyr);
      float64 ICE_lyr        = get_ice_content_volumetric(lyr);
      float64 ICE_depth_layer = ICE_lyr * lyr_thickness;
      ICE_to_depth += ICE_depth_layer;
      remaining_depth -= lyr_thickness;
   }
   // add bit in final layer
   float64 ICE_last            = get_ice_content_volumetric(to_layer);
   float64 ICE_depth_layer_last= ICE_last * remaining_depth;
   ICE_to_depth += ICE_depth_layer_last;
   return ICE_to_depth;
}
//_calc_ice_depth_to____________________________________________2017-05-25_LML_/
float64 Soil_hydrology::get_water_plus_ice_content_volumetric(nat8 layer)  const
{
    float64 ice =
        #ifdef VCS_V5
            VIC_layer[layer-1].ice[0];                                           //170525LML NIY
        #else
            VIC_layer[layer-1].ice;
        #endif
    return (ice + VIC_layer[layer-1].moist) / layers.get_thickness_mm(layer);
}
//_get_water_plus_ice_content_volumetric____________________________2015-03-18_/
float64 Soil_hydrology::get_water_filled_porosity(nat8 layer, CORN::Hour for_hour)                                    const
{  float64 wc =
      (for_hour == 24)                                                           //070117
      ? get_water_plus_ice_content_volumetric(layer) // claudio conf             //070827
      : get_water_plus_ice_content_volumetric_at(layer,for_hour); // claudio conf //070827
   return  wc / hydraulic_properties_ref.get_saturation_water_content_volumetric
   (layer,ICE_WATER_CONTENT_RELEVENCE_UNKNOWN_140822);
}
//_get_water_filled_porosity____________________________________2015-10-22_RLN_/
float64 Soil_hydrology::extract_water(float64 water_uptake_m[] ,nat8 start_layer)
{
    //start_layer: CropSyst layer, start from 1
   double total_wateruptake(0.0);   //(m)
   for (int i = start_layer-1; i <= layers.count()/*200208RLN options.Nlayer*/ -1; i++)
   {
       VIC_layer[i].VCS.transpiration_daily = m_to_mm(water_uptake_m[i+1]);
       total_wateruptake += mm_to_m(VIC_layer[i].VCS.transpiration_daily);
       //printf("VIC_layer(%d) Transpiration(%f)\n",i,VIC_layer[i].transpiration_daily);
   }
   return total_wateruptake;
}
//_extract_water____________________________________________________2015-05-29_/

float64 Soil_hydrology::get_air_entry_pot(nat8 layer,bool consider_ice_content,bool FC_PWP_based) const
{
    //150529LML revised from C:\dev\CropSyst\source\soil\hydrology.cpp
    float64 air_entry_pot_sl = hydraulic_properties_ref.get_air_entry_potential(layer,FC_PWP_based);  //990321
        nat8 num_sublayers = layers.count()/*190128RLN get_number_layers()*/; //060505
       if (num_sublayers && // don't check this condition when initializing the soil //991030
           (layer > num_sublayers))
       // Special case for F.D. which looks a sublayers + 1                      //991030
             air_entry_pot_sl = get_air_entry_pot(layer-1,consider_ice_content,FC_PWP_based);//991030
       else                                                                      //991030
       {  float64 soil_b_sl = hydraulic_properties_ref.get_Campbell_b(layer,FC_PWP_based);  //990321
          float64 ice_content_sl =                                               //990321
             get_ice_content_volumetric(layer);                                  //060505
          if ((ice_content_sl > 0.0) && consider_ice_content )                   //990321
          {  float64 sat_WC_ice_adjusted = hydraulic_properties_ref.get_saturation_water_content_volumetric(layer,ice_content_sl); //140822_051204
             float64 sat_WC_total =        hydraulic_properties_ref.get_saturation_water_content_volumetric(layer,0.0); //051204
             air_entry_pot_sl =                                                  //990321
                (ice_content_sl > sat_WC_ice_adjusted)                           //140822_990331
                ? 0.0 /*air_entry_pot_sl*/                                       //990331
                :  // 050729 Was returning 0.0, but this causes div/0 in caller. //990331
                   // WARNING not sure what to return in this case, but it can't be 0.0
                  air_entry_pot_sl *                                             //990321
                  pow (sat_WC_ice_adjusted / sat_WC_total                        //140822
                      ,- soil_b_sl);                                             //990321
          }
       }
       return air_entry_pot_sl;
}
//_get_air_entry_pot________________________________________________2017-06-28_/
float64 Soil_hydrology::calc_water_depth_profile(bool liquid_and_ice)      const
{  float64 water_depth_profile = 0.0;
   for (uint8 i = 1 ; i <= layers.count(); i++)                                  //980724
   {  float64 wc_sl = liquid_and_ice
            ? get_water_plus_ice_content_volumetric(i)                           //990316
            : get_liquid_water_content_volumetric(i);                            //990316
      water_depth_profile += wc_sl * layers.get_thickness_m(i);
   }
   return water_depth_profile;
}
//_calc_water_depth_profile_________________________________________2015-04-03_/
bool Soil_layers::set_thickness_by_depth(nat8 tolayer)
{
    if (get_depths_provided()) {
        for (nat8 layer = 1; layer <= tolayer; layer++)
        {
            float64 thick = layer > 1 ? get_depth_m(layer) - get_depth_m(layer-1)
                                      : get_depth_m(layer);
            set_thickness_m(layer,thick);
        }
        set_thickness_provided(true);
        return true;
    } else {
        return false;
    }
}
//_set_thickness_by_depth___________________________________________2015-04-03_/
bool Soil_layers::set_depth_by_thickness(nat8 tolayer)
{
    if (get_thickness_provided()) {
        float64 todepth = 0;
        for (nat8 layer = 1; layer <= tolayer; layer++)
        {
            todepth += get_thickness_m(layer);
            set_depth_m(layer,todepth);
        }
        set_depths_provided(true);
        return true;
    } else {
        return false;
    }
}
//_set_depth_by_thickness___________________________________________2015-03-18_/
Soil_layers::Soil_layers()
: depths_provided(false)
,thickness_provided(false)
{}
//_Soil_layers:constructor__________________________________________2015-03-18_/
Soil_layers::~Soil_layers() {}
//_Soil_layers:destructor___________________________________________2015-03-18_/
nat8  Soil_layers::count()                                                 const
{
#ifdef NELSON_DEBUG
// Compiler is not finding options

// WARNING Minglian, I am not finding options

#else
   return options.Nlayer;
#endif
}
//_Soil_layers::count__________________________________________________________/
nat8  Soil_layers::count_uninundated()                                     const
{ return count(); }
//_Soil_layers::count_uninundated______________________________________________/
float64 Soil_temperature::get_temperature(nat8 layer)                      const
{  nat8 VIC_layer_index = layer -1;
   return VIC_layer[VIC_layer_index].T;
}
//_Soil_layers::get_temperature________________________________________________/
float64 Soil_hydrology::change_liquid_water_content_by_volumetric
(nat8 layer, float64 change_WC)                                   modification_
{
assert(false); //need  to impelement

   return 0;
}
//_Soil_hydrology::change_liquid_water_content_by_volumetric
float64 Soil_hydraulic_properties
::get_field_capacity_volumetric (nat8 layer)                               const
{ return mm_to_m(VIC_soil_con.Wcr[layer-1]) / VIC_soil_con.depth[layer - 1]; }
float64 Soil_hydraulic_properties
::get_permanent_wilt_point_volumetric(nat8 layer)                          const
{ return mm_to_m(VIC_soil_con.Wpwp[layer-1]) / VIC_soil_con.depth[layer - 1]; }
float64 Soil_hydraulic_properties
::get_sat_hydraul_cond_kg_s_m3(nat8 layer)                                 const
{ return VIC_soil_con.Ksat[layer-1] * 1.18023e-6; } //Need to check
   //inline virtual float64 get_sat_hydraul_cond_m_hr          ( nat8 layer) const
   //   { return  get_sat_hydraul_cond_kg_s_m3(layer) * 0.00981 * 3600.0; }
      //< \return sat_hydraul_cond converted to m/hr
float64 Soil_hydraulic_properties
::get_Campbell_b(nat8 layer,bool FC_PWP_based)                             const
{ UNUSED_arg(FC_PWP_based); return VIC_soil_con.VCS.b_campbell[layer-1]; }
float64 Soil_hydraulic_properties
::get_air_entry_potential (nat8 layer,bool FC_PWP_based)                   const
{ UNUSED_arg(FC_PWP_based); return VIC_soil_con.VCS.AE[layer-1]; }
float64 Soil_hydraulic_properties::get_water_pot_at_FC (nat8 layer)        const
{ return VIC_soil_con.VCS.water_pot_at_FC[layer-1]; } // -33.0; need to check the value ////170504LML
const float64 *Soil_hydraulic_properties::ref_soil_water_pot_at_FC()       const
{ return VIC_soil_con.VCS.water_pot_at_FC; }                                     //170504LML
float64 Soil_hydraulic_properties
::get_saturation_water_content_volumetric
   (nat8 layer,float64 current_ice_water_content)                          const
{  UNUSED_arg(current_ice_water_content);
   nat8 layer_index = layer-1;
   return mm_to_m(VIC_soil_con.max_moist[layer_index])/VIC_soil_con.depth[layer_index];
}
   //    Usually {  return 1.0 - get_bulk_density_g_cm3(layer) / 2.65;

//_get_________________________________________________________________________/





/*
//LML 150318 copy from C:\dev\VIC_CropSyst\crop\VIC_soil_hydrology.c
int today_is_irrigation(
                        double MAD_crop_specific
                        ,int rec
                        ,double applied_irrigation
                        )
{
//irrigation_pattern_struct *irrig_pattern;
#if (FULL_IRRIGATION==TRUE)
    int i=0;
    double soil_water_deficit=0.0;
    double sum_of_PWP=0.0;
    double sum_of_FC=0.0;
    double accumulative_soil_depth=0.0;
    double total_available_water=0.0;
    for (i=0; i<options.Nlayer-1; i++){ ///we do not count on last layer water for decision of irrigation
        double layer_thickness_mm = soil_con_global->depth[i] *1000;
        double root_depth_mm = VIC_CropSyst_get(VC_root_depth_mm);
        if(root_depth_mm>0){
            int keyvan=1;
        }
        accumulative_soil_depth += layer_thickness_mm;
        //if (root_depth_mm >= accumulative_soil_depth){ ///keyvan just turned it off at this time but this method makes more sense

        sum_of_FC += soil_con_global->Wcr[i];//0.7;//*soil_con_global->depth[i]*1000;
        sum_of_PWP += soil_con_global->Wpwp[i];
        // total_available_water=sum_of_FC - sum_of_PWP;
        //}
    }
    total_available_water=sum_of_FC - sum_of_PWP;
    soil_water_deficit=refill_water_depth_mm();
    //LML 141015 soil_water_deficit=VIC_CropSyst_get_return;
    //if(refill_water_depth_mm()>20){
    if (applied_irrigation>0){
        return (total_available_water>0.0 &&
                ((soil_water_deficit/total_available_water)>MAD_crop_specific));
    }
    else {
        return 0;
    }
#else
    //today_is_deficit_irrigation()
    return (irrig_patt[rec].irrig_amount>0);
#endif
};
*/

RENDER_INSPECTORS_DEFINITION(VIC::Soil_hydrology)
{
   /*NYI
    KNOW_EMANATOR(true);                                                         //170317RLN
    //150922 RLN NYI
    return &emanator;
   */
   return 0;

}
//_RENDER_INSPECTORS_DEFINITION________________________________________________/
}//_namespace_VIC______________________________________________________________/
#endif //VIC_CROPSYST_VERSION
