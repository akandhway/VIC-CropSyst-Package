#include "crop/phenology_A.h"
#include "CS_suite/observation/CS_inspector.h"
#include "corn/parameters/parameter.h"
#include "crop/thermal_time_daily.h"
namespace CropSyst
{
//______________________________________________________________________________
Phenology_abstract::Phenology_abstract
(const Crop_parameters_struct::Phenology &parameters_ref                         //190701
,const float64                   &stress_adjusted_temperature_C_        //190812
,const float64                   &air_temperature_min_C_                //190812
/*190812
,const Physical::Temperature               &stress_adjusted_temperature_         //190701
,const Air_temperature_minimum             &air_temperature_min_                 //190701
*/
,Thermal_time_common::Vernalization        *vernalization_given                  //190701
,Thermal_time_common::Photoperiodization   *photoperiodization_given             //190701
,bool              is_perennial_
,bool              is_fruit_tree_
,const CORN::date32 &simdate_raw_ref                                                //181108
)
:parameters                                                     (parameters_ref) //190701
, is_perennial                                                   (is_perennial_)
, is_fruit_tree                                                 (is_fruit_tree_)
, accrescence                                                                (0)
, culminescence                                                              (0)
, senescence                                                                 (0)
, root_elongation                                                            (0) //191121
, simdate_raw                                                  (simdate_raw_ref) //181113
, stress_adjusted_temperature_C                 (stress_adjusted_temperature_C_)
, air_temperature_min_C                                 (air_temperature_min_C_)
, vernalization                                            (vernalization_given)
, photoperiodization                                  (photoperiodization_given)
, season_duration_days                                                       (0) //181115
, planting_date                                                              (0)
//190710 , harvestable                                                            (false)
, harvest_DOY                                                                (0)
{}
//_Phenology_abstract:constructor___________________________________2013-09-02_/
Phenology_abstract::~Phenology_abstract()
{
   delete vernalization;
   delete photoperiodization;
}
//_Phenology_abstract:destructor____________________________________2019-10-22_/
bool Phenology_abstract::end_day()                                 modification_
{

/*200502 now done here
   // note that Phenology_2013 increments in start day
   // this is preserved for compatibility, but it should be incremented
   // at the end of the day. (the number of full days in the season)
*/

   // Warning need to check if need to accresss culm and scen end_day


   season_duration_days++;
   return true;
}
//_end_day__________________________________________________________2018-11-14_/
bool Phenology_abstract::start_season()                            modification_
{  season_duration_days    = 0;                                                  //181115
   harvest_DOY             = 0;
   return reset();                                                               //110506
}
//_start_season_____________________________________________________2013-09-02_/
bool Phenology_abstract::reset
(Normal_crop_event_sequence from
,Normal_crop_event_sequence to)                                    modification_
{
   harvest_DOY             = 0;
   season_duration_days = 0;
      // Make sure that start_day is called after reset
      // or set this to 1;
   return true;
}
//_Phenology_common::reset__________________________________________2013-09-02_/
bool Phenology_abstract::activate_accrescence()
{  // currently no generalization
   if (culminescence) delete culminescence;                                      //201129LML
   if (senescence) delete senescence;                                            //201129LML
   culminescence = 0;                                                            //200504
   senescence = 0;                                                               //200504

   // Warning currently I am keeping these periods in memory until
   // the crop it terminated.
   // However multiple season will instancate the periods
   // so they are lost.
   // I should give the periods to period output handler
   // where/when they can be disposed.

   return true;
}
//_activate_accrescence_____________________________________________2019-07-01_/
bool Phenology_abstract::activate_culminescence()
{
   if (accrescence) delete accrescence;                                          //201129LML
   accrescence = 0;
   // Warning currently I am keeping these periods until
   // the crop it terminated.
   // However multiple season will instancate the periods
   // so they are lost.
   // I should give the periods to period output handler
   // where/when they can be disposed.

   return true;
}
//_activate_culminescence___________________________________________2019-07-01_/
bool Phenology_abstract::activate_senescence()
{
   if (accrescence) delete accrescence;                                          //201129LML
   if (culminescence) delete culminescence;                                      //201129LML
   accrescence = 0;
   culminescence = 0;
   // Warning currently I am keeping these periods until
   // the crop it terminated.
   // However multiple season will instancate the periods
   // so they are lost.
   // I should give the periods to period output handler
   // where/when they can be disposed.

   return true;
}
//_activate_senescence______________________________________________2019-07-01_/
/*abstract
Phenology::Period *Phenology_abstract::activate_root_elongation()
{  return root_elongation = activate_period("phenology/root_elongation");
}
//_activate_root_elongation_________________________________________2018-11-18_/
*/

bool Phenology_abstract::culminate_senescence()
{
   if (accrescence) delete accrescence;                                          //201129LML
   if (culminescence) delete culminescence;                                      //201129LML
   if (senescence) delete senescence;                                            //201129LML

   accrescence = 0;
   culminescence = 0;
   senescence = 0;
   // Warning currently I am keeping these periods until
   // the crop it terminated.
   // However multiple season will instancate the periods
   // so they are lost.
   // I should give the periods to period output handler
   // where/when they can be disposed.

   return true;
}
//_culminate_senescence_____________________________________________2020-04-09_/
CORN::Parameter_properties_literal PP_crop_season_duration=
{"season_duration","duration"/*,"phenology"*/,"Season duration","hint_NYN","URL_NYN",UT_day,"days"};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
RENDER_INSPECTORS_DEFINITION(CropSyst::Phenology_abstract)
{
   #ifdef CS_OBSERVATION
   // using crop emanator                                                        //160616
   /*NYI
   for each stages (that is relevent to this type of crop)
   {
      std::string ontology_days_since_start_of_XXX("phenology/days_since_start_of/");
      ontology_days_since_start_of_XXX.append(growth_stages[gs]);
      inspectors.append(new CS::Inspector(days_since_start_of[gs],UT_day,context,ontology_days_since_start_of_XXX, value_statistic));//CSVP_crop_base_season_duration,PP_crop_season_duration,UT_day,STAT_value_bit,CORN::mean_statistic) ); // not sure if I want max or mean for the annual aggregation, I think it is not applicable to annual aggregation/
   */

//NYI   inspectors.append(new CS::Inspector(days_since_start_of[NGS_GERMINATION],UT_day,*context/*emanator*/,"phenology/days_since_start_of/season", CORN::value_statistic));//CSVP_crop_base_season_duration,PP_crop_season_duration,UT_day,STAT_value_bit,CORN::mean_statistic) ); /* not sure if I want max or mean for the annual aggregation, I think it is not applicable to annual aggregation*/
   #endif
   return 0; // doesn't have its own emanator emanator;
}
//_RENDER_INSPECTORS_DEFINITION_____________________________________2013-09-18_/
Phenology_abstract::Period_thermal::Period_thermal
(const Crop_parameters_struct::Phenologic_period &parameters_ref
,const CORN::date32                       &simdate_raw_
,const float64                            &stress_adjusted_temperature_C_        //190812
,const float64                            &air_temperature_min_C_                //190812
,bool                                      is_fruit_tree_
,Thermal_time_common::Vernalization       *vernalization_optional
,Thermal_time_common::Photoperiodization  *photoperiodization_optional
)
//200409: CORN::Item()
: CS::Period_clad                                                                //200409
   (simdate_raw_
   ,0 // NYN
   ,parameters_ref.ontology)
, parameters                                                   (&parameters_ref)
, parameters_owned                                                       (false)
, start_day_in_season                                                        (0)
/*200409 already in CS::Period
, initiated                                                       (simdate_raw_)
, culminated                                                           (9999365)
, day_count                                                                  (0)
*/
, thermal_time                                                               (0)
, thermal_time_relative_elapsed                                              (0)
, thermal_time_relative_remaining                                            (0)
//200304 , relative_thermal_time                                             (0)
, simdate_raw                                                     (simdate_raw_)
{  construction_common                                                           //190630
      (stress_adjusted_temperature_C_
      ,air_temperature_min_C_
      ,is_fruit_tree_
      ,vernalization_optional
      ,photoperiodization_optional);

/*
   if (parameters->thermal_time)
   switch (parameters->thermal_time->thermal_response)
   {
      case nonlinear :
          thermal_time =
          new Thermal_time_daily_nonlinear
            (*parameters->thermal_time
            ,stress_adjusted_temperature_
            ,air_temperature_min_
            ,is_fruit_tree_// parameters.crop_model_labeled.get() == CROPSYST_ORCHARD_MODEL
            ,vernalization_optional
            ,photoperiodization_optional);
      break;
      case linear :
         thermal_time =
         new Thermal_time_daily_linear
            (*parameters->thermal_time
            ,stress_adjusted_temperature_
            ,air_temperature_min_
            ,is_fruit_tree_//parameters.crop_model_labeled.get() == CROPSYST_ORCHARD_MODEL
            ,vernalization_optional
            ,photoperiodization_optional);
      break;
   } // switch
*/
}
//_Period::constructor______________________________________________2018-11-14_/
void Phenology_abstract::Period_thermal::dump(std::ostream &strm)
{
   float64 TTday   = thermal_time ? thermal_time->get_growing_degree_day() : 999;
   float64 TTaccum = thermal_time ? thermal_time->get_accum_degree_days
      #ifdef THERMAL_TIME_OBSOLETE_2020
      (false,false)
      #else
      ()
      #endif
      : 99999;
   strm << TTday << "\t" << TTaccum << "\t";
}
//_Period::dump________________________________________________________________/
/*abandoned
Phenology_abstract::Period::Period
(const Crop_parameters_struct::Phenologic_period *parameters_ref
,const CORN::date32                       &simdate_raw_
,const Physical::Temperature              &stress_adjusted_temperature_
,const Air_temperature_minimum            &air_temperature_min_
,bool                                      is_fruit_tree_
,Thermal_time_common::Vernalization       *vernalization_optional
,Thermal_time_common::Photoperiodization  *photoperiodization_optional)
: CORN::Item()
, parameters                                                    (parameters_ref)
, parameters_owned                                                        (true)
, start_day_in_season                                                        (0)
, initiated                                                       (simdate_raw_)
, culminated                                                           (9999365)
, day_count                                                                  (0)
, thermal_time                                                               (0)
, simdate_raw                                                     (simdate_raw_)
{  construction_common();
}
//_Period::constructor______________________________________________2019-06-30_/
*/
Phenology_abstract::Period_thermal::~Period_thermal()
{
   if (parameters_owned) delete parameters;
   if (thermal_time) delete thermal_time;                                        //201128LML
   thermal_time = 0;
}
//_Period::destructor______________________________________________2019-06-30_/
bool Phenology_abstract::Period_thermal::construction_common
(
 const float64                            &stress_adjusted_temperature_C_        //190812
,const float64                            &air_temperature_min_C_                //190812
/*190812
 const Physical::Temperature              &stress_adjusted_temperature_
,const Air_temperature_minimum            &air_temperature_min_
*/
,bool                                      is_fruit_tree_
,Thermal_time_common::Vernalization       *vernalization_optional
,Thermal_time_common::Photoperiodization  *photoperiodization_optional
)
{
   if (parameters->thermal_time)
   switch (parameters->thermal_time->thermal_response)
   {
      case nonlinear :
          thermal_time =
          new Thermal_time_daily_nonlinear
            (*parameters->thermal_time
            ,stress_adjusted_temperature_C_
            ,air_temperature_min_C_
            ,is_fruit_tree_// parameters.crop_model_labeled.get() == CROPSYST_ORCHARD_MODEL
            ,vernalization_optional
            ,photoperiodization_optional);
      break;
      case linear :
         thermal_time =
         new Thermal_time_daily_linear
            (*parameters->thermal_time
            ,stress_adjusted_temperature_C_
            ,air_temperature_min_C_
            ,is_fruit_tree_//parameters.crop_model_labeled.get() == CROPSYST_ORCHARD_MODEL
            ,vernalization_optional
            ,photoperiodization_optional);
      break;
   } // switch
   return true;
}
//_Period::construction_common______________________________________2019-06-30_/
bool Phenology_abstract::Period_thermal::start_day()               modification_
{  if (thermal_time) thermal_time->accumulate_degree_days();

   // only need to calculate relative thermal time once here
   // replace other occurances of calc_thermal_time_relative_XXXX
   // with get_thermal_time_relative_xxxxx

   thermal_time_relative_elapsed   = calc_thermal_time_relative_elapsed();       //200304
   thermal_time_relative_remaining = calc_thermal_time_relative_remaining();     //200304

   return true;
}
//_start_day________________________________________________________2018-11-07_/
bool Phenology_abstract::Period_thermal::end_day()                 modification_
{
   return CS::Period_clad::end_day()                                             //200409
      &&  thermal_time && !has_expired()                                         //191022
        ? thermal_time->end_day() : true;
/*200409
   day_count++;
   return
      thermal_time && !has_expired()                                             //191022
      ? thermal_time->end_day() : true;
*/
}
//_end_day__________________________________________________________2018-11-14_/
bool Phenology_abstract::Period_thermal::has_expired()              affirmation_
{  float64 period_TT_accum = get_thermal_time_accum();//(true);                  //200211
   return (period_TT_accum > parameters->duration_GDDs);
}
//_Period_elapsed___________________________________________________2018-11-07_/
float64 Phenology_abstract::Period_thermal
::calc_thermal_time_relative_elapsed()                              calculation_
{
   float64 period_accum_GDDs = get_thermal_time_accum();//aways unclipped (true); //200211
   float64 period_duration_GDDs = parameters->duration_GDDs;
   return period_accum_GDDs  / period_duration_GDDs;
   // Note that this assumes the thermal time is based on this period
}
//_calc_relative_thermal_time_______________________________________2018-11-07_/
float64 Phenology_abstract::Period_thermal
::calc_thermal_time_relative_remaining()                            calculation_
{  return
    CORN::must_be_0_or_greater<float64>                                          //200312
      ((parameters->duration_GDDs - get_thermal_time_accum())
            //always unclipped (true))                                           //200211
      / (parameters->duration_GDDs));
   /*This is modelled after equations of the form:
    ( (TT_end - TT_current)
     /(TT_end - TT_start))
   */
}
//_calc_relative_thermal_time_remaining_____________________________2018-11-07_/
float64 Phenology_abstract::Period_thermal::get_thermal_time_accum()       const
//200211obs (bool adjusted_for_clipping)               const
{  return thermal_time ? thermal_time->get_accum_degree_days()
      //200211 (adjusted_for_clipping)                                           //200211
      //200211 (false,false)  //WARNING check if need clipping adjustment        //190701
      : 0.0;
}
//_get_thermal_time_accum___________________________________________2018-11-14_/
nat8 Phenology_abstract::Period_thermal::respond_to_clipping()
{
   nat8 reset_count = 0;
   if (clipping_resets())
   {
      thermal_time->clear_accum_degree_days();                                   //200122

      //200211 thermal_time->respond_to_clipping(0);
      // probably should be the termal time to reset to (parameter) but Claudio says 0

      // Not sure if these are to be reset
      //start_day_in_season;
      //CORN::date32 initiated;
      CORN::date32 culminated = 0;
      reset_count = 1;
   }
   return reset_count;
}
//_respond_to_clipping______________________________________________2020-02-10_/
}//_namespace_CropSyst_________________________________________________________/

