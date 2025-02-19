#include "soil/soil_param.h"
#include "soil/txtrhydr.h"
#include "soil/texture.h"
#include "corn/math/compare.hpp"
#include "soil/SCS/SCS.ph"
using namespace CORN;                                                            //001115
//______________________________________________________________________________
bool Smart_soil_parameters::delete_horizon(nat8 layer)
{  bool deleted = Soil_parameters_class_parent::delete_horizon(layer);
   if (deleted) // we need to set up the calculator for the new layering
   {  delete_layer_texture_hydraulics();
      setup_layer_texture_hydraulics();
   }
   return deleted;
}
//_delete_horizon___________________________________________________2004-12-14_/
bool Smart_soil_parameters::insert_horizon(nat8 layer)
{  bool inserted = Soil_parameters_class_parent::insert_horizon(layer);
   if (inserted) // we need to set up the calculator for the new layering
   {  delete_layer_texture_hydraulics();
      setup_layer_texture_hydraulics();
   }
   return inserted;
}
//_insert_horizon___________________________________________________2004-12-14_/
////////////////////////////////////////////////////////////////////////////////
Smart_soil_parameters::Smart_soil_parameters()
:Soil_parameters()
{
   for (int layer = 0; layer <= MAX_soil_horizons ; layer++)
   {  layer_texture_hydraulics[layer] = 0;
      texture_available_in_layer[layer] =false;                                  //111204
   }
   #ifdef VERSION3
   set_user_specified_flags();
   #endif
}
//_Smart_soil_parameters:constructor___________________________________________/
bool Smart_soil_parameters::get_end()
{  bool started = Soil_parameters_class_parent::get_end(); // probably can be moved to end
#ifdef VERSION3
   set_user_specified_flags();
#endif
   // After reading the file we need to set up the hydraulics to set the user array markers
   delete_layer_texture_hydraulics();                                            //020402
   setup_layer_texture_hydraulics();                                             //020402
   return started;                                                               //161023
}
//_get_end__________________________________________________________2001-01-09_/
Smart_soil_parameters::~Smart_soil_parameters()
{  delete_layer_texture_hydraulics();
}
//_~Smart_soil_parameters___________________________________________1998-07-22_/
void Smart_soil_parameters::delete_layer_texture_hydraulics()
{  for (int layer = 0; layer <= MAX_soil_horizons ; layer++)
   {  if (layer_texture_hydraulics[layer])
      { delete layer_texture_hydraulics[layer]; layer_texture_hydraulics[layer] = 0; };
   }
}
//_delete_layer_texture_hydraulics__________________________________2002-04-02_/
void Smart_soil_parameters::setup_texture_hydraulics_layer(nat8 layer)
{
   if (!layer_texture_hydraulics[layer])
      layer_texture_hydraulics[layer] = new Soil_texture_hydraulics
      #if (CROPSYST_VERSION==4)
         (horizon_sand[layer],horizon_clay[layer],horizon_silt[layer]
         ,horizon_saturation_WC[layer]        ,user[layer] & specified_saturation //080313
         ,horizon_permanent_wilt_point [layer],user[layer] & specified_PWP        //130128
         ,horizon_field_capacity       [layer],user[layer] & specified_FC         //130128
         ,horizon_sat_hydraul_cond_m_d [layer],user[layer] & specified_sat_hydraul_cond
         ,horizon_bulk_density_g_cm3   [layer],user[layer] & specified_bulk_density
         ,horizon_Campbell_b           [layer],user[layer] & specified_soil_B         //050401 was false  Now allowing users to enter their own value
         ,horizon_air_entry_pot        [layer],user[layer] & specified_air_entry_pot  //990401 was false
         );
      #else
         (properties.sand.ref(layer),properties.clay.ref(layer),properties.silt.ref(layer)
         ,properties.saturation_WC        .ref(layer),user[layer] & specified_saturation
         ,properties.permanent_wilt_point .ref(layer),user[layer] &specified_PWP
         ,properties.field_capacity       .ref(layer),user[layer] &specified_FC
         ,properties.sat_hydraul_cond_m_d .ref(layer),user[layer] & specified_sat_hydraul_cond
         ,properties.bulk_density_g_cm3   .ref(layer),user[layer] & specified_bulk_density
         ,properties.Campbell_b           .ref(layer),user[layer] & specified_soil_B
         ,properties.air_entry_pot        .ref(layer),user[layer] & specified_air_entry_pot
         );
      #endif

}
//_setup_texture_hydraulics_layer___________________________________2017-09-04_/
void Smart_soil_parameters::setup_layer_texture_hydraulics()
{  for (nat8 layer = 1; layer <= MAX_soil_horizons ; layer++)
      setup_texture_hydraulics_layer(layer);                                     //170904
}
//_setup_layer_texture_hydraulics___________________________________2002-04-02_/
bool Smart_soil_parameters::set_start()
{  for (int layer = 1; layer <= MAX_soil_horizons ; layer++)
   {  bool need_correct_PWP = !(user[layer] & specified_PWP) || CORN::is_approximately<float32>(get_horizon_permanent_wilt_point_volumetric(layer),0.0,0.00001); // 040513_110217
      bool need_correct_FC  = !(user[layer] & specified_FC)  || CORN::is_approximately<float32>(get_horizon_field_capacity_volumetric      (layer),0.0,0.00001); // 040513_110217
   }
   return Soil_parameters::set_start();                                          //161023
}
//_set_start________________________________________________________2001-12-17_/
void  Smart_soil_parameters::fix_bulk_density(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |= specified_bulk_density ;
   else     user[layer] &= ~specified_bulk_density ;
   layer_texture_hydraulics[layer]->bulk_density_status
      = fix ? Soil_texture_hydraulics::fixed : Soil_texture_hydraulics::unknown;
}
//_fix_bulk_density____________________________________________________________/
void Smart_soil_parameters::fix_perm_wilt_point(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |=  specified_PWP;
   else     user[layer] &= ~specified_PWP;
}
//_fix_perm_wilt_point_________________________________________________________/
void Smart_soil_parameters::fix_field_capacity(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |=  specified_FC;
   else     user[layer] &= ~specified_FC;
}
//______________________________________________________________________________
void Smart_soil_parameters::fix_water_pot_at_FC(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |=  specified_water_pot_at_FC;
   else     user[layer] &= ~specified_water_pot_at_FC;
}
//_fix_water_pot_at_FC______________________________________________2013-03-26_/
void  Smart_soil_parameters::fix_sat_hydraul_cond_m_d(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |= specified_sat_hydraul_cond;
   else     user[layer] &= ~specified_sat_hydraul_cond;
   layer_texture_hydraulics[layer]->sat_hydraul_cond_m_d_status
      = fix ? Soil_texture_hydraulics::fixed : Soil_texture_hydraulics::unknown;
}
//_fix_sat_hydraul_cond_m_d____________________________________________________/
void  Smart_soil_parameters::fix_saturation_WC(nat8 layer,bool fix) contribution_
{  if (fix) user[layer] |= specified_saturation;
   else     user[layer] &= ~specified_saturation;
   layer_texture_hydraulics[layer]->saturation_status =
      fix ? Soil_texture_hydraulics::fixed : Soil_texture_hydraulics::unknown;
}
//_fix_saturation_WC________________________________________________2008-03-13_/
float64 Smart_soil_parameters::get_horizon_Campbell_b(nat8 layer)          const
{  float64 file_Campbell_b = Soil_parameters_class_parent::get_horizon_Campbell_b(layer);
   return
      (is_approximately(file_Campbell_b,0.0,0.000001))                           //990217
      ?  layer_texture_hydraulics[layer]->get_Campbell_b()
      : file_Campbell_b;
}
//_get_horizon_Campbell_b___________________________________________1998-05-28_/
float64 Smart_soil_parameters::get_profile_depth()                         const
{  float64 depth = 0;
   for (nat8 layer = 1; layer <= horizon_count; layer++)
      depth += get_horizon_thickness(layer);
   return depth;
}
//_get_profile_depth________________________________________________1998-12-17_/
float64 Smart_soil_parameters::get_water_holding_capacity(float64 to_depth) const
{  float64 curr_depth = 0;
   bool done = false;
   float64 available_water = 0.0;
   for (nat8 layer = 1; !done && (layer <= horizon_count) ; layer++)
   {  float64 curr_thickness = get_horizon_thickness(layer);
      if (curr_depth + curr_thickness  >= to_depth)
      {  // Only take the portion of the sublayer upto the desired depth
         curr_thickness = to_depth - curr_depth;
         done = true;
      };
      curr_depth += curr_thickness;
      available_water += (get_horizon_field_capacity_volumetric(layer)
                       - get_horizon_permanent_wilt_point_volumetric(layer))
             * curr_thickness;
      if (layer == horizon_count)
         done = true;
   }
   return available_water;
}
//_get_water_holding_capacity_______________________________________1998-12-17_/
float64 Smart_soil_parameters::get_organic_matter(float64 to_depth_m)      const
{  float64 curr_depth = 0;
   bool done = false;
   float64 OM = 0.0;
   for (nat8 layer = 1; !done && (layer <= horizon_count) ; layer++)
   {  float64 curr_thickness = get_horizon_thickness(layer);
      if (curr_depth + curr_thickness  >= to_depth_m)
      {  // Only take the portion of the sublayer upto the desired depth
         curr_thickness = to_depth_m - curr_depth;
      };
      curr_depth += curr_thickness;
      OM += curr_thickness * get_horizon_organic_matter(layer);                  //160915
      if ((layer == horizon_count) || (curr_depth >= to_depth_m))
         done = true;
   }
   if (curr_depth > 0.00001)
      OM /= curr_depth;
   return OM;
}
//_get_organic_matter_______________________________________________2010-12-08_/
void Smart_soil_parameters::get_avg_water_content(soil_horizon_array32(avg_WC)) const
{  for (nat8 layer = 1; layer <= horizon_count; layer++)
      avg_WC[layer] = (get_horizon_field_capacity_volumetric(layer) + get_horizon_permanent_wilt_point_volumetric(layer))/ 2.0;
}
//_get_avg_water_content____________________________________________2002-05-04_/
float64 Smart_soil_parameters::get_water_holding_capacity()                const
{  return get_water_holding_capacity(get_profile_depth());                       //020920
}; // of whole profile
//_get_water_holding_capacity_______________________________________1998-12-17_/
float64 Smart_soil_parameters::get_horizon_bulk_density_g_cm3(nat8 layer)  const
{  return user[layer] & specified_bulk_density
     ? Soil_parameters_class_parent::get_horizon_bulk_density_g_cm3(layer)
     : layer_texture_hydraulics[layer]->get_bulk_density();
} // ? g/cm3  and/or Mg/m3 ?
//_get_horizon_bulk_density_g_cm3______________________________________________/
float64 Smart_soil_parameters::get_horizon_sat_hydraul_cond_m_d(nat8 layer)const
{  return user[layer] & specified_sat_hydraul_cond
      ? Soil_parameters_class_parent::get_horizon_sat_hydraul_cond_m_d(layer)
      : layer_texture_hydraulics[layer]->get_sat_hydraul_cond_m_d();
} // m/day
//_get_horizon_sat_hydraul_cond_m_d_________________________________1998-05-28_/
float64 Smart_soil_parameters::get_horizon_saturation_WC(nat8 layer)       const
{  return user[layer] & specified_saturation
      ? Soil_parameters_class_parent::get_horizon_saturation_WC(layer)
      : layer_texture_hydraulics[layer]->get_saturation();
}  // m3/m3
//_get_horizon_saturation_WC________________________________________2008-03-13_/
float64 Smart_soil_parameters::get_horizon_air_entry_pot(nat8 layer)       const
{  float64 file_air_entry_pot = Soil_parameters_class_parent::get_horizon_air_entry_pot(layer);
   return
      CORN::is_approximately<float64>(file_air_entry_pot,0.0,0.0000001)          //990218
      ? layer_texture_hydraulics[layer]->get_air_entry_pot()
      : file_air_entry_pot;
}
//_get_horizon_air_entry_pot________________________________________1998-05-28_/
void Smart_soil_parameters::copy_horizon(nat8 from_horizon,nat8 to_horizon)
{  Soil_parameters_class_parent::copy_horizon( from_horizon, to_horizon);
} // J/kg
//______________________________________________________________________________
bool Smart_soil_parameters::is_agricultural(bool include_dryland,bool include_irrigated) const
{  return (include_dryland	  && agricultural_dryland)
      ||  (include_irrigated && agricultural_irrigated);
}
//_is_agricultural__________________________________________________2008-07-29_/

