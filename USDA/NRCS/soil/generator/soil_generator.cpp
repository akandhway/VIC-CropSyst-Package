#include "soil_generator.h"
#include "USDA/NRCS/soil/survey/database/tabular/chtexturegrp.h"
#include "USDA/NRCS/soil/survey/database/tabular/chorizon.h"
#include "corn/measure/measures.h"
#include <stdlib.h>
#include "corn/data_source/generic_datarec.h"
#include <assert.h>
#include "CS_Suite/CropSyst/convert/soil/soil_convertor_engine.h"
// I don't really need all of the soil_convert_engine classes,
// only Soil_database which I need to create a separate unit

extern int debug_received_component_count;

#ifndef __CONSOLE__
#  ifdef __BCPLUSPLUS__
      #include <vcl.h>
      extern TLabel *ref_label_STATSGO2_curr_mukey    ;
      extern TCheckBox *ref_checkbox_STATSGO2_continue;
      extern TProgressBar *ref_progress_bar;
#  endif
#endif
using namespace USDA_NRCS;
using namespace USDA_NRCS::STATSGO2_SSURGO;
using namespace CORN;
using namespace version5;
namespace USDA_NRCS{
namespace STATSGO2_SSURGO
{
   Database *STATSGO2_database_ref;
// ______________________________________________________________________________
Soil_generator::Soil_generator
(const USDA_NRCS::STATSGO2_SSURGO::Database::Arguments &arguments_               //170831
,const CORN::Seclusion &mapunit_seclusion_                                       //170903
,CS::Soil_database &target_soil_database_)                                       //190222
: CS::Soil_provider()
, STATSGO2_database
   (arguments_                                                                   //170831
   ,mapunit_seclusion_)                                                          //170903
, target_soil_database(target_soil_database_)                                    //190222
, arguments(arguments_)                                                          //170831
, curr_mapunit_composition                                                   (0) //190927
{  STATSGO2_database_ref = &STATSGO2_database;
}
//_Soil_generator:constructor_______________________________________2011-11-07_/
Mapunit_composition *Soil_generator::provide_mapunit_composition
(const std::string &MUkey)                                            provision_
{
   // currently keeping only one current mapunit composition in memory
   if (curr_mapunit_composition)
   {
      if (curr_mapunit_composition->is_key_string(MUkey))
         return curr_mapunit_composition;
      else
      {
         //190929 commit_mapunit(*curr_mapunit_composition);
         give_to_database(curr_mapunit_composition);                          //190225
         curr_mapunit_composition = 0;
      }
   }
   curr_mapunit_composition = new Mapunit_composition();
   if (!STATSGO2_database.get_mapunit(MUkey,curr_mapunit_composition->mapunit))
     // failed to load from database so invalidate the mapunit_composition.
   {  delete curr_mapunit_composition; curr_mapunit_composition = 0; }

   #ifndef __CONSOLE__
      #  ifdef __BCPLUSPLUS__
         if (ref_label_STATSGO2_curr_mukey)
            ref_label_STATSGO2_curr_mukey->Caption = chorizon.chkey.c_str();
         Application->ProcessMessages();
      #  endif
   #endif

   return curr_mapunit_composition;
}
//_provide_mapunit_composition______________________________________2019-09-27_/
Component_composition *Soil_generator::provide_component_composition
(const std::string & cokey)                                           provision_
{
   // Check if we already have the component given to curr_mapunit_composition
   Component_composition *found_component_comp
      = curr_mapunit_composition
      ? curr_mapunit_composition->find_component_composition(cokey)
      : 0;
   if (!found_component_comp )
   {
      found_component_comp = new Component_composition();
      STATSGO2_database.get_component(cokey,found_component_comp->component);
   }
   else
   {
   std::clog << "mapunit has component" << std::endl;
   }
   return found_component_comp;
}
//_provide_component_composition____________________________________2011-11-07_/
Component_composition *Mapunit_composition::find_component_composition(const std::string &cokey)
{  return dynamic_cast<Component_composition *>(components.find_string(cokey)); //120921
}
//_Mapunit_composition::find_component_composition_____________________________/
nat32 Soil_generator::generate()
rendition_
{  nat32 generated = 0;

//std::clog << "providing chorizon index" << std::endl;
   const Table_indexer &chorizon_index = STATSGO2_database.provide_chorizon_index(true);

   std::string last_chorizon_cokey;

   Component_composition *curr_component_composition = 0;

   FOR_EACH_IN(chorizon_indice, Table_indexer::Indice,chorizon_index,chorizon_iter)
   {
      CHorizon_record chorizon;
      STATSGO2_database.get_chorizon(chorizon_indice->key_unique,chorizon);
      /*191001
      std::string chorizon_index;
      STATSGO2_database.get_chorizon(chorizon_indice->key_string(chorizon_index),chorizon);
      */
      if ((chorizon.cokey != last_chorizon_cokey) && curr_component_composition)
      {  // There are no more chorizons for this component


// std::clog << "C:" << chorizon.cokey << std::endl;
         Mapunit_composition *mapunit_comp = provide_mapunit_composition
            (curr_component_composition->component.mukey);

//std::clog << "M:" << curr_component_composition->component.mukey << std::endl;
         if (mapunit_comp)
         {
             mapunit_comp->take_component_composition(curr_component_composition);
             curr_component_composition = 0;
         }
      }
/*
if (chorizon.cokey == "15885146")
std::clog << "reached" << std::endl;
*/

      if (!curr_component_composition || curr_component_composition->component.cokey != chorizon.cokey)
         curr_component_composition = provide_component_composition(chorizon.cokey);

      if (curr_component_composition)
      {
          curr_component_composition->compose(chorizon);
      } // else component is not member of requested mapunits
         // so ignore
      last_chorizon_cokey = chorizon.cokey;
      generated++;
   } FOR_EACH_END(chorizon_iter)
   return generated;
}
//_generate_________________________________________________________2011-11-07_/
/*190927 predominate_component is now deterumined in take_component_composition
bool Mapunit_composition::commit()
{
   nat8 component_count = 0;
   predominate_component = dynamic_cast<Component_composition *>(components.get_at(0));
   FOR_EACH_IN(component_comp, Component_composition,components, each_component)
   {  component_count += component_comp->compose_meta(mapunit);                  //1101023
      component_comp->soil_params.set_start();   // This is needed to update both sets of PWP and FC
      if (is_more_agricultural_than_predominant(component_comp->component))
          predominate_component = component_comp;
   } FOR_EACH_END(each_component)
   bool commited = (predominate_component);
   if (commited)
     predominate_component_composition = predominate_component;                  //141228
   return commited;
}
//_Mapunit_composition:commit_______________________________________2014-10-14_/
*/
bool Mapunit_composition::take_component_composition
(Component_composition *component_composition_given)
{
   components.take(component_composition_given);
   component_composition_given->compose_meta(mapunit);
   component_composition_given->soil_params.set_start();   // This is needed to update both sets of PWP and FC
   if (predominate_component)
   {
         if (is_more_agricultural_than_predominant(component_composition_given->component)) // (component_comp->component))
             predominate_component = component_composition_given;
   } else // this is the first component encountered,
      // it will be the predominate component
      predominate_component = component_composition_given;
   bool commited = (predominate_component);
   if (commited)
     predominate_component_composition = predominate_component;                  //141228


   // May want to move predominate component selection here (from commit)

   return true;
}
//_take_component_composition_______________________________________2019-09-27_/
/* 190929 obsolete implement in take
bool Soil_generator::commit_mapunit(Mapunit_composition &mapunit_comp) contribution_
{  // process    all chorizons of the predominate component
   bool commited = mapunit_comp.commit();                                        //141014
   switch (arguments.aggregation_option)                                         //170831
   {  case  0 : //Extract map unit soil components
      {
         //NYI
      }break;
      case 1: // Aggregate soil sequences in each map unit to create an aggregated representitive pseudopedon
      {  //NYI
      } break;
      case 2: // Use primary component as a representitive pedon
      {
         Component_composition *predom_comp=mapunit_comp.predominate_component;  //190929
         //190929 if (mapunit_comp.predominate_component != NULL)
         if (predom_comp)                                                        //190929
            predom_comp->soil_params.description.brief.append                    //190929
                  //190929 mapunit_comp.predominate_component
                  (" (derived from predominate component of STATSGO map unit)");
         } break;
   }
   return commited;
}
//_Soil_generator:commit_______________________________________________________/
*/
bool Component_composition::compose(const CHorizon_struct &chorizon)
{  nat8 horizon = chorizon.desgnvert;
   if (horizon &&
         /* There were some cases where desgnvert was 0 */
      !soil_params.bound_by_bedrock
         /* There was a case (667015:990268)
            where layer 4 was bedrock but then a layer 5 with texture
            We are stopping at the first bedrock layer.
         */
      )
   {
   soil_params.horizon_count = std::max<int16>
      (soil_params.horizon_count, horizon);
   //190926 soil_params.properties.thickness[horizon]                                     //180728
   float64 horizon_thickness_m                                                   //190929
      = cm_to_m(
       CORN::is_approximately<float32>(chorizon.hzthk.repr,0.0,0.00001)
      // Some times the horizon thickness is not specified, so
      // need to compute from depth
      ?(chorizon.hzdepb.repr - chorizon.hzdept.repr)
      : chorizon.hzthk.repr);
   // NYI soil_params.bypass_coef_32[horizon] =  chorizon.
   // NYI the bypass coefficient could possible be derived from STATSGO2 cpores
   // chorizon has the following elements, but I am not sure how complete these are
   // so I use the pedotransfer functions (setup_layer_texture_hydraulics)
   soil_params.properties.thickness[horizon] = horizon_thickness_m;              //190929

   soil_params.properties.sand[horizon] = chorizon.sandtotal.repr; // %          //180728
   soil_params.properties.clay[horizon] = chorizon.claytotal.repr; // %          //180728
   soil_params.properties.silt[horizon] = chorizon.silttotal.repr; // %          //180728
   soil_params.properties.pH[horizon] = chorizon.ph1to1h2o.repr;
   // Soil pH in water  (not buffer)
   // chorizon has CEC-7 (CEC normalized at pH 7?)  and ECEC (effective CEC?)
   // I am not sure which type of  cation_exchange_capacity is used by CropSyst
   //NYI soil_params.cation_exchange_capacity[horizon] = chorizon.cec7.repr or ecec.repr // (cMol_c/kg soil) centimole of positive char/ kilogram soil (equivelent to meq/100g) milliequivelents of cations per 100 grams soil this equal to  For ammonium volitalization
   soil_params.properties.organic_matter[horizon] = chorizon.om.repr;            //170828
   soil_params.properties.organic_matter_low[horizon] = chorizon.om.low;         //170828
   soil_params.properties.organic_matter_high[horizon] = chorizon.om.high;       //170828
   soil_params.texture_available_in_layer[horizon] =
      !(  CORN::is_approximately<float32>(soil_params.properties.sand[horizon],0.0,0.0001)  //170828
       || CORN::is_approximately<float32>(soil_params.properties.clay[horizon],0.0,0.0001)  //170828
       || CORN::is_approximately<float32>(soil_params.properties.silt[horizon],0.0,0.0001)) //170828
      // The texture should also sum to at least 95% (this is generous)
      && ((soil_params.properties.sand[horizon]           //170828_111204
          +soil_params.properties.clay[horizon]           //170828
          +soil_params.properties.silt[horizon]) > 95.0); //170828
   if (!soil_params.texture_available_in_layer[horizon])
   {  // The texture is not entered in this record so attempt to
      // derive the texture from the texture description.
      USDA_texture::Texture_code texture_code =
         (lookup_texture_description
            (chorizon.chkey
            ,soil_params.properties.sand[horizon]                                //170828
            ,soil_params.properties.clay[horizon]                                //170828
            ,soil_params.properties.silt[horizon]));                             //170828
      if ((texture_code == USDA_texture::unweathered_bedrock) ||                 //110613
          (texture_code == USDA_texture::weathered_bedrock) ||
          (texture_code == USDA_texture::unknown))
      {
         soil_params.bound_by_bedrock= true;
         soil_params.texture_available_in_layer[horizon] = false;
      } else
      {  soil_params.texture_available_in_layer[horizon] = true;
      }
      if (!soil_params.texture_available_in_layer[horizon])
      {
         soil_params.horizon_count = horizon-1;;
         soil_params.properties.sand[horizon] = 0.0;
         soil_params.properties.clay[horizon] = 0.0;
         soil_params.properties.silt[horizon] = 0.0;
      }
   }
   if ((horizon > 1)
      && CORN::is_approximately<float32>(soil_params.properties.sand[horizon],1.0,0.0001)
      && CORN::is_approximately<float32>(soil_params.properties.clay[horizon],1.0,0.0001)
      && CORN::is_approximately<float32>(soil_params.properties.silt[horizon],1.0,0.0001)) //110606
   {  // There were some wierdo cases where sand silt and clay were all 1
      // often the last layer
      // In this case copy the values from the previous layer
      soil_params.properties.sand[horizon] = soil_params.properties.sand[horizon-1];
      soil_params.properties.clay[horizon] = soil_params.properties.clay[horizon-1];
      soil_params.properties.silt[horizon] = soil_params.properties.silt[horizon-1];
      soil_params.bound_by_bedrock = false;                                      //111204
      soil_params.texture_available_in_layer[horizon] = true;                    //111204
   }
   if (soil_params.texture_available_in_layer[horizon])                          //111205
   {
      // may want to instanciate each layer hydraulics as needed
      //abandoned soil_params.setup_layer_texture_hydraulics();                                       //170904
      soil_params.setup_texture_hydraulics_layer(horizon);                       //170904

      if ((horizon > 1)                                                          //111204
         // && soil_params.texture_available_in_layer[horizon]
          && !soil_params.texture_available_in_layer[horizon-1])
      {
         soil_params.setup_texture_hydraulics_layer(horizon-1);                       //170904
         // This is a wierdo where a horizon above has not been encountered yet (probably missing)
         // It is assumed the previous layer is similar to this layer
         if (CORN::is_approximately<float32>(soil_params.properties.thickness[horizon-1],0.0,0.0001))
            soil_params.properties.thickness[horizon-1] =
               soil_params.properties.thickness[horizon];
         soil_params.properties.sand[horizon-1] = soil_params.properties.sand[horizon];
         soil_params.properties.clay[horizon-1] = soil_params.properties.clay[horizon];
         soil_params.properties.silt[horizon-1] = soil_params.properties.silt[horizon];
         soil_params.bound_by_bedrock = false;                                   //111204
         soil_params.texture_available_in_layer[horizon-1] = true;               //111204
         soil_params.layer_texture_hydraulics[horizon-1]->recompute_every_thing();
         soil_params.user[horizon-1] |= Soil_parameters_class::specified_texture;
      }
      soil_params.user[horizon] |= Soil_parameters_class::specified_texture;
      if (soil_params.layer_texture_hydraulics[horizon])
          soil_params.layer_texture_hydraulics[horizon]->recompute_every_thing();
      if (horizon)
         profile_depth_m += soil_params.properties.thickness[horizon];
   }
   }
   return profile_depth_m > 0.00001;
      // If there are no soil layers, this is not a viable component.
}
//_compose_____________________________________________________________________/
/*190927 obsolete
bool Mapunit_composition::compose(CHorizon &horizon)
{  bool valid = false;
   Component_composition *component = provide_component_composition(horizon.cokey);
   if (component)
   {  valid = component->compose(horizon);
      if (!valid)
         components.remove(component);
   }
  return valid;
}
//_compose__________________________________________________________2011-11-07_/
*/
/*190928
Component_composition *Mapunit_composition::provide_component_composition(const std::string & cokey)
{  Component_composition *found_component_comp
      = dynamic_cast<Component_composition *>(components.find_string(cokey)); //120921
   if (!found_component_comp )
   {  found_component_comp = new Component_composition();
      CORN::MicroSoft::Access::Import_export_table &component_table
         =  dynamic_cast<CORN::MicroSoft::Access::Import_export_table &>
            (STATSGO2_database_ref->provide_component_table());
      const Table_indexer &component_index = STATSGO2_database_ref->provide_component_index(true);
      const Table_indexer::Indice *co_indice
         = dynamic_cast<Table_indexer::Indice *>
            (component_index.find(render_key_ID(cokey.c_str())));
      if (co_indice)
      {  bool component_indexed = component_table.goto_file_position(co_indice->file_pos);
         if (component_indexed)
            component_table.get(found_component_comp->component);
         components.append(found_component_comp);
      }
   }
   return found_component_comp;
}
//_provide_component_composition____________________________________2011-11-07_/
*/
USDA_texture::Texture_code Component_composition::lookup_texture_description
   (const std::string chorizon_chkey
   ,float32 &horizon_sand
   ,float32 &horizon_clay
   ,float32 &horizon_silt)
{
   USDA_texture::Texture_code texture_code = USDA_texture::unknown;
   CORN::MicroSoft::Access::Import_export_table &texturegrp_table
      = dynamic_cast<CORN::MicroSoft::Access::Import_export_table &>
         (STATSGO2_database_ref->provide_chtexturegrp_table());
   const Table_indexer &chtexturegrp_index = STATSGO2_database_ref->provide_chtexturegrp_index(true);
   const Table_indexer::Indice *indice =  dynamic_cast<const Table_indexer::Indice *>
      (chtexturegrp_index.find_string(chorizon_chkey));
      //190930 (chtexturegrp_index.find(render_key_ID(chorizon_chkey.c_str())));
   if (indice)
   {  texturegrp_table.goto_file_position(indice->file_pos);
      USDA_NRCS::CHtexturegrp_record horizon_texture;
      /*bool valid = */texturegrp_table.get(horizon_texture);
         // keyfield
      CS::Soil_texture soil_texture(horizon_texture.texture.c_str());
      horizon_sand = soil_texture.get_sand();
      horizon_clay = soil_texture.get_clay();
      horizon_silt = soil_texture.get_silt();
      texture_code = soil_texture.get_code();
   }
   return texture_code; // chtexturegrp != NULL;
}
//_lookup_texture_description_______________________________________2001-03-24_/
bool Mapunit_composition::is_more_agricultural_than_predominant
   (const Component_struct &new_component)                                 const
{  bool is_agricultural =
       ((new_component.nirrcapcl.get_int32() > 0) &&(new_component.nirrcapcl.get_int32() < 5))
     ||((new_component.irrcapcl.get_int32() > 0)&&(new_component.irrcapcl.get_int32() < 5));
   bool better_agriculturally = // The lower the capability class, the better the soil agriculturally
   (is_agricultural && (new_component.nirrcapcl.get_int32() < predominate_component->component.nirrcapcl.get_int32())
   || (new_component.irrcapcl.get_int32() < predominate_component->component.irrcapcl.get_int32()));
   bool same_agriculturally = // The lower the capability class, the better the soil agriculturally
   ( (new_component.nirrcapcl.get_int32() == predominate_component->component.nirrcapcl.get_int32()))
   ||((new_component.irrcapcl.get_int32() ==predominate_component->component.irrcapcl.get_int32()));
   bool more_coverage =  new_component.comppct.repr >
      predominate_component->component.comppct.repr;
   return (is_agricultural && more_coverage)
      || ((better_agriculturally || same_agriculturally) && more_coverage);      //111111
}
//_is_more_agricultural_than_predominant_______________________________________/
bool Component_composition::compose_meta(const USDA_NRCS::Mapunit_struct &map_unit)
{  bool composed = true;
   if (soil_params.horizon_count)
   {
         // not sure if if is needed
         // if (!soil_params.layer_texture_hydraulics)                           //170904
      soil_params.setup_layer_texture_hydraulics();                              //170904
      if (! soil_params.bound_by_bedrock && profile_depth_m < 3.0)               //110611
      {  // The soil surveys only sample to
         nat8 extended_horizon = soil_params.horizon_count+1;
         soil_params.setup_texture_hydraulics_layer(extended_horizon);           //170904
         soil_params.properties.thickness[extended_horizon] = 3.0 - profile_depth_m;
         soil_params.properties.sand[extended_horizon] = soil_params.properties.sand[extended_horizon-1] ;
         soil_params.properties.clay[extended_horizon] = soil_params.properties.clay[extended_horizon-1] ;
         soil_params.properties.silt[extended_horizon] = soil_params.properties.silt[extended_horizon-1] ;

         soil_params.layer_texture_hydraulics[extended_horizon]->recompute_every_thing();
         soil_params.horizon_count  +=1;
      }
   }
   soil_params.description.brief.assign(component.compname);
   soil_params.steepness_percent    = component.slope.repr;
   soil_params.slope_length         = component.slopelenusle.repr;
   if (soil_params.slope_length) soil_params.slope_length = 100;//meters default
   /* NYI
   soil_params.mukey                = component.mukey;
   soil_params.cokey                = component.cokey;
   */
   soil_params.STATSGO.COMPPCT              = component.comppct.repr;
   soil_params.save_only_specified_layers = true; ;

   // NYI STATSGO_path; // I.e. C:\Simulation\Database\STATSGO

   soil_params.capability_class_irrigated =component.irrcapcl.get_int32();
   soil_params.capability_class_dryland =  component.nirrcapcl.get_int32();
   soil_params.agricultural_dryland = soil_params.capability_class_dryland > 0.0001;
   soil_params.agricultural_irrigated = soil_params.capability_class_irrigated > 0.0001;
   // NYI soil_params.float32 water_holding_capacity_to_1m;            // (value meter depth) to 1.5meter
   // NYI aggregated_water_holding_capacity_to_1m;

   soil_params.hydrologic_group_clad.set(component.hydgrp.get());
   // NYI soil_params.hydrologic_condition_labeled
   // NYI soil_params.override_curve_number_16; // runoff curve number override (also for models without hydro cond/group)
   // NYI soil_params.user_fallow_curve_number_16;
   // NYI soil_params.compute_surface_storage = true;
   // NYI soil_params.surface_storage_mm_32 = map_unit.predominate_component->xxxxx;
   soil_params.albedo_dry = component.albedodry.repr;
   if (CORN::is_approximately<float32>(soil_params.albedo_dry,0.0,0.000001)) soil_params.albedo_dry = 0.08;
   // STATSOGO doesn't have wet albedo but is is usually between 1.6 and 1.8
   // times the dry albedo
   soil_params.albedo_wet = soil_params.albedo_dry  * 1.7;
   // soil_params.water_vapor_conductance_atmosphere_adj = include_mapunit->predominate_component->xxxxx;         // 080213  This may be obsolete
   // soil_params.SLPF_CropGro = include_mapunit->predominate_component->xxxxx;  //   Soil ?Limitation? ?productivity? factor   1.0 = no limitation. used only for CropGro
   return composed;
}
//_compose_meta_____________________________________________________2011-10-25_/
/*190927 obsolete
Mapunit_composition * Soil_generator::render_selected_mapunit(const std::string &MUkey) rendition_
{  return render_selected_mapunit(atoi(MUkey.c_str()));
}
//_render_selected_map_unit_________________________________________2011-10-30_/
Mapunit_composition *Soil_generator::render_selected_mapunit(nat32 MUkey) rendition_
{  searching_for_specific_mapunit = MUkey;
   if (MUkey)
      generate();
   return found_specific_mapunit; //? found_specific_mapunit : 0;
}
//_render_selected_map_unit_________________________________________2011-10-30_/
*/
/*NYI
bool Soil_generator::done_with
(const std::string &MUkey_or_POLY_ID)
{
}
//_done_with________________________________________________________2014-12-25_/
*/
Mapunit_composition *Soil_generator::render_at_geocoordinate
      (const Geocoordinate &geocoordinate)                            rendition_
{  // Currently Database to VIC converter does provide the map unit
   // so this method is not currently used
   assert(false);
   return 0;
}
//_render_at_geocoordinate__________________________________________2015-09-28_/
bool Soil_generator::give_to_database
(Mapunit_composition *mapunit_comp)                                   rendition_
{
   int aggregation_option = 2; //190225 eventually reimplement in arguments
   switch (aggregation_option)
   {  case 0 : //all components
      {
         FOR_EACH_IN(comp_comp,Component_composition,mapunit_comp->components,each_comp)
         {
            std::string cokey(comp_comp->component.cokey);
            size_t colon_pos = cokey.find(":");
            if (colon_pos != std::string::npos)
               cokey.replace(colon_pos,1,"_");
            target_soil_database.know_component_composition
               (mapunit_comp->mapunit.mukey
               ,cokey
               ,*comp_comp);
         } FOR_EACH_END(each_comp)

      } break;
      case 1 : //aggegation
      {
         assert(false); // Not yet implemented
      } break;
      case 2 : //predominate
      {
         if (mapunit_comp->predominate_component != NULL)
            target_soil_database.know_component_composition
               (mapunit_comp->mapunit.mukey
               ,mapunit_comp->mapunit.mukey // just use the MYKEY for the identifier
               ,*mapunit_comp->predominate_component);
         else
         { std::clog << "Warning: No predominant component: "
            << mapunit_comp->mapunit.mukey << std::endl; }
      } break; // case 2
   } // switch aggregation_option
   delete mapunit_comp;
   return true;
}
//_give_to_database____________________________________________________________/
}}//_namespaces STATSGO2_SSURGO_USDA_NRCS______________________________________/

