#include "options.h"
#include "UED/library/record.h"
#include "cs_UED_harvest.h"
#include "crop/crop_interfaced.h"
#include "crop/phenology_I.h"
#include "csvc.h"
/*

Harvest Year / Event date  Just output normal date YYYY/Mmm/DD(DDD) format
Planting / Dormancy ends
Emergence
Maturity
Yield Grain, tuber, leaf, fruit or root
Removed and designated for beneficial use (clipped fodder, silage, etc.)
Consumed by animals Removed from the field and disposed (unused)
Remained in the field laying as surface residue (straw, chaff, etc.)
Remained in the field as dead standing stubble/residue
Remained live biomass (after clipping)
Above ground biomass (before clipping)
Harvest description
// obs Soil water drainage
// obs ET act.
// obs Total nitrogen uptake
// obs Nitrogen leached

Year Day Day Day Day kg/ha kg/ha kg/ha kg/ha kg/ha kg/ha kg/ha kg/ha mm mm kgN/ha kgN/ha Description
*/
//______________________________________________________________________________
void Harvest_data_record::update
(const CropSyst::Crop_model_interface &crop)
{

//   #if ((PHENOLOGY_VERSION==2018))
   const CropSyst::Phenology &phenology = crop.ref_phenology();                            //181111
   planting_date  .set_date32(phenology.get_planting_date());                                  //041205
   planting_date  .set_date32(phenology.get_restart_date());                                   //041205
   emergence_date .set_date32(phenology.get_emergence_date());
//NYI/*_______*/    flowering_date.set (crop.get_flowering_date());
//NYI/*_______*/    grain_filling_date = (
//NYI/*_______*/           (crop.get_crop_model() == CROPSYST_FRUIT_MODEL)
//NYI/*_______*/           ? crop.get_initial_fruit_growth_date()
//NYI/*_______*/          : crop.get_grain_filling_date());
   maturity_date  .set_date32(phenology.get_maturity_date());
//   #endif
/*190616 crop has phenology
   #if (!PHENOLOGY_VERSION || (PHENOLOGY_VERSION==2013))
   // actually this can be the same as 2018, just haven't verified yet
   planting_date.set(crop.get_planting_date());                                  //041205
   planting_date.set(crop.get_restart_date());                                   //041205
   emergence_date.set(crop.get_emergence_date());
//NYI flowering_date.set (crop.get_flowering_date());
//NYI grain_filling_date = (
//NYI           (crop.get_crop_model() == CROPSYST_FRUIT_MODEL)
//NYI           ? crop.get_initial_fruit_growth_date()
//NYI          : crop.get_grain_filling_date());
   maturity_date.set(crop.get_maturity_date());
   #endif
*/
   const Crop_mass_fate *biomass_fate_today=crop.get_biomass_fate_today_record();//070625
   if (biomass_fate_today)
   {  yield                     = biomass_fate_today->yield;                     //041001
      used_biomassX              = biomass_fate_today->useful;                    //040920
      grazed_biomass            = biomass_fate_today->get_removed_for_grazing(); //040920
      residue_biomass           = biomass_fate_today->chaff_or_leaf_litter ;     //040920
      stubble_biomass           = biomass_fate_today->stubble_dead;              //040920
      disposed_biomass          = biomass_fate_today->disposal;                  //040920
      uncut_biomass             = biomass_fate_today->uncut_canopy;              //070207
      above_ground_biomass      = biomass_fate_today->canopy_before;

      reserves_biomass = crop.get_fruit_reserves_biomass_kg_m2();                //200608
   }
}
//_update______________________________________________________________________/
bool Harvest_data_record::expect_structure(bool for_write)
{  bool expected = CORN::Data_record::expect_structure(for_write);
   expect_int32   ("planting_date"        ,planting_date.mod_date32());
   expect_int32   ("emergence_date"       ,emergence_date.mod_date32());
   expect_int32   ("maturity_date"        ,maturity_date.mod_date32());
   /*170525
   expect_date    ("planting_date"                 ,planting_date,false);
   expect_date    ("emergence_date"                ,emergence_date,false);
   expect_date    ("maturity_date"                 ,maturity_date,false);
   */

   expect_float32 ("yield"                ,yield);
   expect_float32 ("used_biomass"         ,used_biomassX);
   expect_float32 ("grazed_biomass"       ,grazed_biomass);
   expect_float32 ("residue_biomass"      ,residue_biomass);
   expect_float32 ("stubble_biomass"      ,stubble_biomass);
   expect_float32 ("disposed_biomass"     ,disposed_biomass);
   expect_float32 ("uncut_biomass"        ,uncut_biomass);  // after clipping
   expect_float32 ("above_ground_biomass" ,above_ground_biomass); // before clipping
   expect_float32 ("reserves_biomass"     ,reserves_biomass);                    //200607
   structure_defined = true;                                                     //120314
   return expected;
}
//_expect_structure____________________________________________________________/
Harvest_data_source::Harvest_data_source(UED::Database_file_indexed *_database )
: UED::Tuple_data_source(_database,UED::Record_type_distinguisher_interface::UED_data_value_date_var_units,0,simulated_quality)
, Harvest_data_record()
{  add_field2(harvest_section,"planting_date"          ,CSVC_crop_base_planting_date_YD               ,UT_date,UT_arbitrary_date);
   add_field2(harvest_section,"emergence_date"         ,CSVC_crop_base_emergence_date_YD              ,UT_date,UT_arbitrary_date);
   add_field2(harvest_section,"maturity_date"          ,CSVC_crop_base_maturity_date_YD               ,UT_date,UT_arbitrary_date);
   add_field2(harvest_section,"yield"                  ,CSVC_crop_biomass_yield                       ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"used_biomass"           ,CSVC_crop_biomass_useful                      ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"grazed_biomass"         ,CSVC_crop_biomass_removed_grazing             ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"residue_biomass"        ,CSVC_crop_biomass_removed_residue             ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"stubble_biomass"        ,CSVC_crop_biomass_stubble                     ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"disposed_biomass"       ,CSVC_crop_biomass_disposal                    ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"uncut_biomass"          ,CSVC_crop_biomass_uncut_canopy                ,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"above_ground_biomass"   ,CSVC_crop_biomass_produced_above_ground_period_sum,UC_kg_m2,UT_arbitrary_date);
   add_field2(harvest_section,"reserves_biomass"       ,CSVC_crop_base_fruit_reserves_biomass         ,UC_kg_m2,UT_arbitrary_date); //200608
}
//_Harvest_data_source:constructor__________________________________2004-09-21_/
void Harvest_data_source::record(datetime64 date_time)
{  set_date(date_time);
   set((*this));
}
//______________________________________________________________________________

