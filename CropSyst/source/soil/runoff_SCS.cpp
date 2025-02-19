#include "runoff_SCS.h"
namespace CropSyst {
//______________________________________________________________________________
Soil_runoff_SCS::Soil_runoff_SCS
(
//200316 Runoff_model   _runoff_model,
Soil_base     &soil_)                                                            //020527
: Soil_runoff_common                                                     (soil_) //200316_141201
//200316 , runoff_model(_runoff_model)
//200316 ,soil(_soil)                                                                     //020527
//200316 ,random_roughness_m                                         (cm_to_m(0.6))                                                //990422
//200316 ,act_crop_storage(0.0)                                                           //990528
//200316 ,act_residue_storage(0.0)                                                        //990528
{}
//_Soil_runoff_SCS_constructor_________________________________________________/
float64 Soil_runoff_SCS::surface_storage_n
(float64 steepness  // % 0-100
,float64 curve_number )                                                          //990215
{  return cos((atan(steepness)/100.0)) *  /* steepness % to radians*/
               inch_to_m((1000.0 / curve_number - 10));                          //990215
}
//_surface_storage_n___________________________________________________________/
// lookup curve number based on conditions:
float64 Soil_runoff_SCS::lookup_condition
(Land_use              land_use   // <- not a var will be modified only locally
,Land_treatment        treatment
,Hydrologic_condition  hydrocondition
,USDA_NRCS::Hydrologic_group      hydrogroup                                     //110218
,int16                 hydrologic_curve_number // will override curve number selection
,int16                 fallow_curve_number                                       //990216
,CNs                   antecedent_moisture_content
,bool                  soil_frozen_1
,float64               soil_bulk_density_1
,float64               crop_fract_canopy_cover
,bool                  has_crop_cover)                                           //181108
{  if (!has_crop_cover)                                                          //181108
      land_use = FALLOW; // We are currently in fallow conditions Crop not yet growing//990213
   float64 cn_ratio = (fallow_curve_number ) //  user overridden curve fallow curve_number user_fallow_curve_number //990213
   ? // When the users specifies a fallow curve number,                          //990213
      // we adjust the curve numbers for all the other conditions accordingly    //990213
      fallow_curve_number /                                                      //990215
         SCS_lookup_curve_number(FALLOW,treatment,hydrocondition,hydrogroup,CN2) //990213
   : 1.0;                                                                        //990215
   float64 current_normal_curve_number = CORN::must_be_less_or_equal_to<float64>
      (cn_ratio* SCS_lookup_curve_number                                         //990215
       (land_use,treatment,hydrocondition,hydrogroup
       ,antecedent_moisture_content),100.0);                                     //990213
   bool bare_soil = crop_fract_canopy_cover <= 0.9 ;

   float64 bare_soil_curve_number = bare_soil                                    //990215
   ? CORN::must_be_less_or_equal_to<float64>(cn_ratio * SCS_lookup_curve_number  //990213
      (FALLOW,treatment,POOR,hydrogroup,antecedent_moisture_content),100.0)      //990213
   : 1.0;                                                                        //990213
   float64 curve_number = (land_use == FALLOW)
   ? CORN::must_be_less_or_equal_to<float64>(cn_ratio * SCS_lookup_curve_number  //990215p
      (FALLOW,treatment,hydrocondition,hydrogroup,antecedent_moisture_content),100.0)  //990213
   : bare_soil    //{ weight for bare soil and canopy curve }
     ? bare_soil_curve_number
            + (current_normal_curve_number - bare_soil_curve_number)             //990213
             * crop_fract_canopy_cover / 0.9
     : current_normal_curve_number;
   float64 total_water_content_1 =
      soil.ref_hydrology()->get_water_plus_ice_content_volumetric(1);            //160412_130930
   const Soil::Hydraulic_properties_interface *hydraulic_props = soil.ref_hydraulic_properties(); //160412_061108
   float64 perm_wilt_point_1 = hydraulic_props->get_permanent_wilt_point_volumetric(1);   //990722
   if (soil_frozen_1 && (total_water_content_1 > perm_wilt_point_1) && (soil_bulk_density_1 != 2.65))
   {  float64 porosity = (1.0 - (soil_bulk_density_1/2.65));                     //990722
      float64 adjustment = (100.0- curve_number) * (total_water_content_1 -perm_wilt_point_1)/ (porosity - perm_wilt_point_1);   //990722
      curve_number += adjustment ;                                               //990311
   }
   //  Here we can override the curve number
   if (hydrologic_curve_number)
      curve_number = hydrologic_curve_number + (antecedent_moisture_content * 0.00001);
   return curve_number;
}
//_lookup_condition____________________________________________________________/
float64 shape_param1(float64 surface_runoff1,float64 surface_runoff3,float64 shape_2 )
{  return CORN_ln( 1.0 / (1.0 - (surface_runoff3 / surface_runoff1)) - 1.0) + shape_2;
}
//_shape_param1________________________________________________________________/
float64 shape_param2(float64 surface_runoff1, float64 surface_runoff2, float64 surface_runoff3)
{  //what do we really want to do when arg of ln < 0 ????}
   float64 arg1 = fabs(0.5 / (1.0 - (surface_runoff2 / surface_runoff1)));
   float64 arg2 = 1.0 / (1.0 - (surface_runoff3 / surface_runoff1)) - 1.0;
   return 2.0 * (CORN_ln(arg1) - 0.5 - CORN_ln(arg2));
}
//_shape_param2________________________________________________________________/
float64 Soil_runoff_SCS::get_surface_storage
(
/*190617 now member
Land_use land_use ,
*/
 Hydrologic_condition  hydrologic_condition                                      //971011
,Land_treatment  treatment
//181108 ,Normal_crop_event_sequence crop_growth_stage
,bool                   has_crop_cover                                           //181108
,float64 crop_fract_canopy_cover)
{  /// layering is layer weighting factor
   float64 surface_storage = 0.0;                                                //131216
   float64 sublayer_depth = 0.0;
   float64 sum_FFC_layer    = 0.0;  // This is weighted PAW
   float64 sum_layering     = 0.0;
   Layers_abstract *layers = soil.layers;                                        //150424
   for (nat8 sublayer = 1; (sublayer < soil.layers->get_number_sublayers())
                           && (sublayer_depth <= 1.0) ; sublayer++)
   {
      float64 layer_thickness     = layers->get_thickness_m(sublayer);           //150424
      sublayer_depth += layer_thickness;                                         //150424
      float64 layering = layer_thickness / sublayer_depth;                       //150424
      sum_layering += layering;
      float64 FFC = soil.hydrology->calc_available_water_fraction(sublayer,false);//060509_990217
      sum_FFC_layer += layering * FFC;
   }
   float64 FFC_total = sum_FFC_layer / sum_layering;  // PAW factor
   const Soil::Structure *soil_structure = soil.ref_structure();                 //070725
   float64 bulk_density_1 =  soil_structure->get_bulk_density_g_cm3(1);          //070725
   float64 condition1 = lookup_condition
        (crop_land_use,treatment,hydrologic_condition,soil.get_hydrologic_group()
        ,soil.get_override_curve_number()
        ,soil.get_user_fallow_curve_number()
        ,CN1
        ,soil.hydrology->is_frozen(1)
        ,bulk_density_1
        ,crop_fract_canopy_cover
        ,has_crop_cover                                                          //181108
        //181108 ,crop_growth_stage
        );
   float64 condition2 =  lookup_condition
        (crop_land_use,treatment,hydrologic_condition,soil.get_hydrologic_group()
        ,soil.get_override_curve_number()
        ,soil.get_user_fallow_curve_number()
        ,CN2
        ,soil.hydrology->is_frozen(1)
        ,bulk_density_1
        ,crop_fract_canopy_cover
        ,has_crop_cover                                                          //181108
        );
   float64 condition3 =  lookup_condition
        (crop_land_use,treatment,hydrologic_condition,soil.get_hydrologic_group()
        ,soil.get_override_curve_number()
        ,soil.get_user_fallow_curve_number()
        ,CN3
        ,soil.hydrology->is_frozen(1)
        ,bulk_density_1
        ,crop_fract_canopy_cover
        ,has_crop_cover                                                          //181108
        );
   float64 s1 = surface_storage_n(soil.parameters_ownable->get_steepness_percent(),condition1);
   float64 s2 = surface_storage_n(soil.parameters_ownable->get_steepness_percent(),condition2);
   float64 s3 = surface_storage_n(soil.parameters_ownable->get_steepness_percent(),condition3);
   if (!(CORN::is_approximately<float64>(s1,s2,0.000001) &&
         CORN::is_approximately<float64>(s2,s3,0.000001) &&
         CORN::is_approximately<float64>(s1,0.0,0.000001)))                      //980908
   {  float64 w2 = shape_param2(s1,s2,s3);
      float64 w1 = shape_param1(s1,s3,w2);
      surface_storage = s1 * (1.0 -  (FFC_total / (FFC_total + exp(w1 - w2 * FFC_total))));
   }

//   #error turn this back on.

   #ifdef DUMP_CURVE_NUMBERS
   if (SCS_runoff_dat_file) // Special output requested by Rolf sommer           //131216
       (*SCS_runoff_dat_file)
       << '\t' << condition1 << '\t' << condition2 <<  '\t' << condition3
       << '\t' << surface_storage;
   #endif
   return surface_storage;
}
//_get_surface_storage_________________________________________________________/
float64 Soil_runoff_SCS::runoff
(
 /*190617 now member Land_use land_use , */
 Hydrologic_condition  crop_hydrologic_condition                                 //971011
,Land_treatment  management_land_treatment
,bool has_crop_cover                                                             //181108
,float64 crop_fract_canopy_cover
,float64 non_intercept_precip)
{
   if (non_intercept_precip > 0.0)  // put this in runoff unit.
   {    float64 surface_storage = get_surface_storage
         // weighted surface storage  as combination of s1 s2 s3 and PAW weight
        (crop_hydrologic_condition,                                              //971011
         management_land_treatment,
         has_crop_cover,                                                         //181108
         crop_fract_canopy_cover);
     // can be used for output:
     //   adj_curve_num =
     //   1000/(surface_storage/(cos(arctan(soil.steepness/100.0)*0.0254)+10));
   estimated_runoff = (non_intercept_precip <= (0.2 * surface_storage))          //981228
     ? 0.0
     : std::min<float64>(non_intercept_precip,
               CORN_sqr((non_intercept_precip - 0.2 * surface_storage)) /        //981228
                  (non_intercept_precip + 0.8 * surface_storage));               //981228
   } else estimated_runoff = 0.0;
   return estimated_runoff;
}
//_runoff______________________________________________________________________/
/*200316 moved to common
float64 Soil_runoff_SCS::maximum_depression_storage()
{
   // Used in numerical runoff model.
   // called every day  when storage is needed
   float64 random_roughness_cm = m_to_cm(random_roughness_m);
   float64 slope = soil.parameters_ownable->get_steepness_percent();
   float64 term1 = 0.112 * random_roughness_cm;
   float64 term2 = 0.031 * CORN_sqr(random_roughness_cm);
   float64 term3 = 0.012 * random_roughness_cm * slope;
   float64 max_depression_storage_m = cm_to_m((term1  + term2- term3));
   return CORN::must_be_greater_or_equal_to<float64>(max_depression_storage_m,0.001);      //990512
}
//_maximum_depression_storage_______________________________________1999-04-22_/
void Soil_runoff_SCS::adjust_random_roughness
(float64 effective_precipitation_m
,float64 fract_canopy_cover
,float64 fract_residue_cover)
{  if (effective_precipitation_m > 0.0)
   {  float64 random_roughness_ratio = 0.89
      * exp(-0.026 * (m_to_cm(effective_precipitation_m)
            * (1.0-fract_canopy_cover) * (1.0 -fract_residue_cover)));
      random_roughness_m *= random_roughness_ratio;
   }
}
//_adjust_random_roughness__________________________________________1999-04-22_/
float64 pot_crop_storage[5][4] =// mm
{//notreat, straight, contour, terrace
 { 1.0,      1.0    ,  1.0   , 1.0}       // fallow
,{ 1.0,      2.1    ,  3.2   , 4.4}       // row
,{ 1.0,      2.7    ,  3.8   , 4.4}       // small grain
,{ 1.0,      3.2    ,  3.8   , 5.0}       // close seed
,{ 1.0,     60.0    , 60.0   ,60.0}       // pasture
                  // Others ignored at this time
};
//_pot_crop_storage_________________________________________________1999-05-28_/
float64 Soil_runoff_SCS::calculate_crop_storage
(Land_treatment  land_treatment
, float64 fract_canopy_cover)
{  act_crop_storage =
      (crop_land_use == PASTURE) ? mm_to_m(60.0) :  // <- currently redundent
      mm_to_m(pot_crop_storage[crop_land_use][land_treatment])* fract_canopy_cover;
   return  act_crop_storage;
}
//_calculate_crop_storage___________________________________________1999-05-28_/
float64 Soil_runoff_SCS::calculate_residue_storage(float64 fract_residue_cover)
{  act_residue_storage = mm_to_m(60.0) * fract_residue_cover;
   return act_residue_storage ;
}
//_calculate_residue_storage________________________________________1990-05-28_/
*/
void Soil_runoff_SCS::distribute_runoff_in_intervals
(Dynamic_float_array &runoff_in_intervals
,const Dynamic_float_array &precip_pattern)
{  if (estimated_runoff > 0.0)
   {  float64 total_precip = precip_pattern.sum();
      if (CORN::is_zero<float64>(total_precip))  // runoff could be from snow melt only so we just put it in the first interval //990728
         runoff_in_intervals.set((nat32)0,estimated_runoff);                     //010608
      else                                                                       //990728
         for // each interval in the precip pattern
           (uint32 i = 0; i <= precip_pattern.get_terminal_index(); i++)         //010608
         {  // partition the respective ratio of todays runoff
            // into each runoff interval.
            float64 ratio = precip_pattern.get(i) / total_precip;
            float64 interval_runoff = ratio * estimated_runoff;
            runoff_in_intervals.set(i,interval_runoff);
         }
   } else // there is no runoff
      runoff_in_intervals.clear();
}
//_distribute_runoff_in_intervals___________________________________1999-07-22_/
}//_namespace CropSyst_________________________________________________________/

