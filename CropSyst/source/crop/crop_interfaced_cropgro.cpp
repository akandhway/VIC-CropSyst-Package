#ifdef TEMP_DISABLED
#ifdef USE_PCH
#  include "simulation/CropSyst_sim_pch.h"
#else
#include <options.h>
#include <math.h>
#include "crop/crop_interfaced_cropgro.h"
#include <corn/measure/measures.h>
#include <common/weather/weather_interface.h>
#  include "organic_matter/OM_residues_profile_abstract.h"
#include "soil.h"
#include "soil/chemicals_profile.h"
#endif
#include <corn/dynamic_array/dynamic_array_T.h>
#define Dynamic_float_array Dynamic_array<float32>
#include "crop/growth_stages.hpp"

extern CORN::Date simulation_today;
extern bool load_ecotype(const char *econum,const char *ECO_filename,Ecotype &ecotype_params);
extern bool load_species(const char *SPE_filename,CropGro_species_parameters &species_param);

//namespace_CropSyst_begin
//______________________________________________________________________________
Crop_interfaced_CropGro::Crop_interfaced_CropGro
(Soil_interface            &soil_ref
,Soil_nitrogen_interface   &soil_chemicals_ref                                   //040604
,Weather_interface         &weather_ref
,Crop_parameters           *_parameters)                                         //050331
:CropSyst::Crop_interfaced(weather_ref.air_temperature_avg)                      //141208RLN_131008NS
, Crop_common
(_parameters,true,weather_ref)                  //121115_0503311
, crop(0)
, weather(weather_ref)
, soil(soil_ref)
, model_input(_parameters->cropgro.model_input_file.c_str())
, parameters(_parameters)
, intercepted_precipitation_m(0.0)
, EOP(1.75)
, PAR(21.7)
, TRWUP(1.75)
, hourly_temperature_estimator(60)
, emergence_date((Date32)0)
, flowering_date((Date32)0)
, tuber_init_date((Date32)0)                                                     //081111
, grain_filling_date((Date32)0)
, maturity_date((Date32)0)
, GAI_yesterday(0)
, new_GAI(0)                                                                     //031126
, growth_stage(PREPLANTING)
{
   model_input.get();
   load_species(model_input.species_filename.c_str(),species_parameters);
   load_ecotype(model_input.cultivar.ECONO.c_str(),model_input.ecotype_filename.c_str(),ecotype_parameters);
   CONTROL.MESIC[0] = 0;                // char
   CONTROL.RNMODE[0] = 0;               // char  [1]
   CONTROL.CROP[0] = 0;               // char  [2]
   CONTROL.MODEL[0] = 0;                // char  [8]
   #ifdef USE_OS_FS
   std::wstring name_ext;                                                        //130826
   parameters->cropgro.model_input_file.get_name_with_extension(name_ext);
   wstring_to_ASCIIZ(name_ext,CONTROL.FILEIO);//  "IBSNAT35.INP" //120614_050207
   #else
   std::string name_ext;                                                       //120614
   strcpy(CONTROL.FILEIO,parameters->cropgro.model_input_file.get_name_with_extension(name_ext)/*120331 (true)*//*get_file_ext()*/.c_str());//  "IBSNAT35.INP" //120614_050207
   #endif
   CONTROL.DAS = 0;                 // int32          // Days after start of simulation (d)
   CONTROL.DYNAMIC = 0;                 // int32
   CONTROL.FROP = 1;                 //               // Printout every FROP days
   CONTROL.LUNIO = 0;                 // int32
    CONTROL.MULTI_unused = 1;                 // int32        // 1 = Single simulation run //040202
   CONTROL.RUN = 1;                 // int32          // 1 = First run of series
   CONTROL.YRDOY = 0;                 // int32
   CONTROL.YRSIM = 1978166 ;          // int32        // Start of simulation date
   CONTROL.NYRS = 0;                 // int32
   CONTROL.YRDIF = 0;                 // int32

     ISWITCH.IDETC =' '; //
     ISWITCH.IDETD =' '; //
     ISWITCH.IDETG ='Y'; //   //Print plant growth output file
     ISWITCH.IDETL ='N'; //   //Don't print detailed output (PlantNBal.out)
     ISWITCH.IDETN ='N'; //   //Don't print N output
     ISWITCH.IDETO ='N'; //   //Don't print overview file
     ISWITCH.IDETR =' '; //
     ISWITCH.IDETS =' '; //
     ISWITCH.IDETW =' '; //
     ISWITCH.IHARI ='M'; //   //Harvest at maturity
     ISWITCH.IPLTI =' '; //
     ISWITCH.IIRRI =' '; //
     ISWITCH.ISWCHE =' '; //
     ISWITCH.ISWDIS =' '; //
     ISWITCH.ISWNIT ='Y'; //  //Nitrogen processes are simulated
     ISWITCH.ISWSYM ='Y'; //  //Nitrogen fixation is simulated
     ISWITCH.ISWTIL =' '; //
     ISWITCH.ISWWAT ='Y'; //  //Water processes are simulated
     ISWITCH.MEEVP =' '; //
     ISWITCH.MEPHO =' '; //
     ISWITCH.MESOM =' '; //
     ISWITCH.IFERI =' '; //
     ISWITCH.IRESI =' '; //
     ISWITCH.NSWI =' '; //

   crop = new Crop_CropGro
   (soil_ref
   ,weather_ref
   ,model_input
   ,species_parameters
   ,ecotype_parameters
   ,CONTROL
   ,ISWITCH
   ,EOP
   ,PAR
   ,TRWUP);
   // Set up the hourly temperature estimator
   float64 Tmax_today      = weather.get_curr_max_air_temperature();             //021124
   float64 Tmin_today      = weather.get_curr_min_air_temperature();             //021124
   Date    yesterday(weather.get_today_date32()); yesterday.dec_day(1);          //030722
   float64 Tmax_yesterday  = weather.get_max_temperature(yesterday);             //021124
   Date    tomorrow(weather_ref.get_today_date32()); tomorrow.inc_day(1);        //030722
   float64 Tmin_tomorrow   = weather.get_min_temperature(tomorrow);              //021124
   hourly_temperature_estimator.xreset(Tmax_today, Tmax_yesterday, Tmin_today, Tmin_tomorrow);  //030610
}
//_Crop_interfaced_CropGro:constructor_________________________________________/
Crop_interfaced_CropGro::~Crop_interfaced_CropGro()
{  if (crop) delete crop; crop = 0;
}
//______________________________________________________________________________
bool Crop_interfaced_CropGro::sow()
{  bool sowed = (crop->sow(simulation_today.get_date32()
      ,simulation_today.get_date32() // Although simulation start date is the name of the variable in CropSyst,
         // I think it is really the planting date
          ));                                                                    //041206
   if (trigger_synchronization(PLANTING))
         growth_stage = PLANTING;
   return sowed;                                                                 //041206
}
//_sow_________________________________________________________________________/
bool Crop_interfaced_CropGro::process(const CORN::Date &today)
{
   bool processed = true;
   float64 solar_rad_MJ_m2 = weather.get_curr_solar_radiation_MJ_m2();
   float64 photosynthetic_active_radiation_MJ_m2   // MJ/m2 ?                    //011022
      = (solar_rad_MJ_m2 / 2.0);                                                 //011022
//             PAR in Crop Gro is  photon flux density	moles[quanta]/m2-d
   float PAR_moles_per_m2_d = photosynthetic_active_radiation_MJ_m2 * 1000000.0 / 235000.0; //011022
//             There are 2.35E5 J/mol.  Therefore, to convert MJ/m2/day to mol/m2/day,
//             multiply by 1E6 (J/MJ) and divide by 2.35E5 (J/mol).

   PAR = PAR_moles_per_m2_d;
//             RLN: PAR should be about 20-30
               // calculate hourly temperature as in cropsyst?
   float64 max_temp = weather.get_curr_max_air_temperature();                    //100420
   float64 min_temp = weather.get_curr_min_air_temperature();                    //100420
   float64 avg_temp = weather.get_curr_avg_air_temperature();
   Date    tomorrow(today); tomorrow.inc_day(1);                                 //030713
   float64 min_temp_tomorrow   = weather.get_min_temperature(tomorrow);          //021124
   hourly_temperature_estimator.set_max_today_min_tomorrow(max_temp, min_temp_tomorrow);  //060511
   Dynamic_float_array hourly_temperature(24,24,0);                              //030610
   hourly_temperature_estimator.get_temperature_by_interval(hourly_temperature); //030610
   float32 TGROAV = 0;
   float32 TGRO[25];
   for (int h = 1; h <= 24; h++)
   {  // CropSyst hourly temperature is 0 indexed, CropGro is 1 indexed (I am pretty sure)
      TGRO[h] = hourly_temperature.get(h-1);
      TGROAV += TGRO[h];
   }
   TGROAV /= 24.0;
   // Calculate variables needed by CropSyst that are not in CropGro
//N/A/*031130_*/    update_fraction_of_canopy_cover();

   crop->set_TGRO(TGRO);
   crop->set_TGROAV(TGROAV); // avg daily temperuatre?
   if (today.get() >= crop->yrplt.get() && crop->yrplt.get() != -99)
   {
      soil_sublayer_array_64(root_density);
      for (int i=1 ; i < NL_alloc; i++)
          root_density[i] = crop->RLV[i];
      // NYI update_root_fractions(/**soil.get_layers(),NL,1,get_root_length_m(),*/root_density,0.0);
      // Presuming CropGro expects rooting at soil surface.
      act_transpiration_m = process_transpiration_m
      (pot_transpiration_m
      ,&soil
      ,soil.mod_salinity());                                                     //180110_051118
//             RLN Note, I dont know the difference between EOP and TRWUP
//             In the sample program these were the same value
      EOP =    m_to_mm(pot_transpiration_m);
      TRWUP =  m_to_cm(act_transpiration_m);
      processed = crop->process(today);
   }
   // Trigger any phenologic events for
   // phenologic syncronizations
   // CropGro uses 10000 to indicate a stage has not been reached yet.
   // or 999999 for STGDOY
   // or -99 for dates that have not been achieved.
//N/A          NOCROP
//N/A          PREPLANTING
   if (crop->rstages.STGDOY[15] != 999999)if (trigger_synchronization(PLANTING)) growth_stage = GERMINATION; // May also be set in sow() method
   if (crop->rstages.STGDOY[1] != 999999) if (trigger_synchronization(EMERGENCE)) growth_stage = EMERGENCE;
//NYI          RESTART     not sure how CropGro handles dormancy
//NYI          BUD_BREAK   not sure how CropGro handles dormancy
   if ((crop->rstages.STGDOY[3] != 999999) || (crop->rstages.STGDOY[3] != 999999))  if (trigger_synchronization(ACTIVE_GROWTH)) growth_stage = ACTIVE_GROWTH; // Using end of juvenile phase
   if (crop->rstages.STGDOY[5] != 999999) if (trigger_synchronization(FLOWERING)) growth_stage = FLOWERING;  // Maybe use NR1?
   if ((crop->rstages.STGDOY[7] != 999999) || (crop->rstages.STGDOY[8] != 999999))  if (trigger_synchronization(FILLING_OR_BULKING)) growth_stage = FILLING_OR_BULKING;
//NYI          INITIAL_FRUIT_GROWTH for crop model only
//NYI          VERAISON for crop model only
//NYI          RAPID_FRUIT_GROWTH  for crop model only
   if (crop->rstages.STGDOY[10] != 999999) if (trigger_synchronization(MATURITY)) growth_stage = MATURITY;
   if (crop->rstages.STGDOY[11] != 999999) if (trigger_synchronization(HARVESTABLE)) growth_stage = HARVESTABLE;
// NYI         DORMANT Not sure how CropGro handles dormancy
   if (crop->rstages.STGDOY[16] != 999999) if (trigger_synchronization(HARVESTED)) growth_stage = HARVESTED;
   if (crop->rstages.STGDOY[16] != 999999) if (trigger_synchronization(TERMINATED)) growth_stage = TERMINATED;  // Currently presuming

//NYI this is not in CropSyst thermal time units   DTX_accum += crop->phenol.DTX;

//VPD is only for output    vapor_deficit_stats.append(get_VPD_daytime()); // used only for output
   uptake_N_from_soil();

   // moved the following to  to end_day()
   // setup for tomorrow
   new_GAI = get_GAI() - GAI_yesterday;
   GAI_yesterday = get_GAI();
   N_stress_stats.append(crop->veggr.NSTRES);
   SWFAC_stats.append(crop->SWFAC);
   reported.daily.transpiration_act_m = get_act_transpiration_m();               //190705

   return processed;
}
//_process__________________________________________________________2003-11-26_/
bool Crop_interfaced_CropGro::trigger_synchronization(uint32 event_sync)
{  //Note if this is merged with a common crop, make sure to keep previously_triggered_synchronizations;
   bool triggered = false;                                                       //031203
   if (!(previously_triggered_synchronizations & event_sync))                    //031126
   {  triggered_synchronizations |= event_sync;
      previously_triggered_synchronizations |= event_sync;                       //031126
      triggered = true;                                                          //031203
   }                                                                             //031126
   return triggered;                                                             //031203
}
//_trigger_synchronization__________________________________________2002-03-10_/
const char  *Crop_interfaced_CropGro::get_description()                    const
{  return crop->model_input.cultivar.VRNAME.c_str();
}
//______________________________________________________________________________
/*
The following stress values in CropSyst are 0 to 1 where 1 is no stress
(Stress index values in CropSyst are 1 to 0 where 0 is no stress).
- water stress            CUMTUR ?

- temperature stress       (probably not in CropGro)
- overall growth stress    (this is reported as output in CropSyst)


*/
//______________________________________________________________________________
bool Crop_interfaced_CropGro::in_growing_season() const
{  return true;  // Will need to refine this, but dont know if CropGro has dormancy.
}
//______________________________________________________________________________
bool  Crop_interfaced_CropGro::is_terminated() const
{  return false;  // Need to figure out how a crop is terminated.
}
//______________________________________________________________________________
nat32 Crop_interfaced_CropGro::get_growth_stage_index() const
{  return growth_stage;
}
//______________________________________________________________________________
void *Crop_interfaced_CropGro::get_unique_identifier() const // I think I can return this
{  return (void *)this;
}
//______________________________________________________________________________
/*131204 no longer used
Harvested_part Crop_interfaced_CropGro::get_harvested_part() const
{  return grain_part; // I am presuming CropGro only has seed crops, I dont know how to determine if it is anything else
}
*/
//______________________________________________________________________________
/*131204 no longer used
void Crop_interfaced_CropGro::set_harvest_date(Date32 harvest_date)
{
   //   NYI
}
*/
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_pot_transpiration_m() const
{  return pot_transpiration_m;
}
//______________________________________________________________________________
const char  *Crop_interfaced_CropGro::describe_growth_stage() const
{ char *growth_stage_phrase = 0;
  switch (growth_stage)
  { case PREPLANTING         : growth_stage_phrase ="Preplanting fallow"; break;
    case GERMINATION         : growth_stage_phrase ="Preemergence";       break;
    case BUD_BREAK           : growth_stage_phrase ="Bud break";          break;//030521
    case ACTIVE_GROWTH: growth_stage_phrase ="Active growth";             break;
  //case CANOPY_EXPANDED       : growth_stage_phrase="Full canopy";       break; //130429  // This isn't accually a growth stage, it is an event
    case FLOWERING           : growth_stage_phrase ="Flowering";          break;
    case FILLING_OR_BULKING  : growth_stage_phrase ="Gilling/Bulking" ;   break;
    case INITIAL_FRUIT_GROWTH:growth_stage_phrase ="Initial fruit growth";break;//030521
    case VERAISON            :  growth_stage_phrase ="Veraison";          break;//030521
    case RAPID_FRUIT_GROWTH  : growth_stage_phrase ="Rapid fruit growth"; break;//030521
    case MATURITY            : growth_stage_phrase ="Maturity";           break;
    case HARVESTABLE         : growth_stage_phrase ="Harvestable";        break;
    case DORMANT_or_INACTIVE : growth_stage_phrase ="Dormant or Inactive";break;
    case HARVESTED           : growth_stage_phrase ="Harvest";            break;//030516
  }
  return (growth_stage_phrase);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_recorded_root_depth_m()               const
{  return cm_to_m(crop->roots.RTDEP);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_root_length_m()                       const
{  return cm_to_m(CORN_must_be_greater_or_equal_to
      (crop->roots.RTDEP - crop->model_input.planting_details.SDEPTH,0.0));
}
//______________________________________________________________________________
float64  Crop_interfaced_CropGro::get_plant_height_m()                     const
{  return crop->canopy.CANHT;  // CANHT is in meters
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_active_fract_root_length_m(uint8 sublayer) const
{  return 0.0; // NYI  total_fract_root_length[sublayer]; // CropGrow doesn't use active_fract_root_length, so we just return total_fract_root_length.
   //   active_fract_root_length[sublayer];
}
//______________________________________________________________________________
#ifdef NYN
total_fract_root_length is used to get PAW,
but I think I can just use 1.0 for all since CropGro does not have
this total_fract_root length.
If it is needed, the following is from Crop_base

we may get this from RLV  claudio is looking into this
/*________*/ void Crop_interfaced_CropGro::update_total_fract_root_length()
/*________*/ {
/*030722_*/  	soil_sublayer_array_64(L);
/*030722_*/  	soil_sublayer_array_64(density_area);
/*030722_*/  	soil_sublayer_array_64(root_density_at_layer_bottom);

/*990221_*/    for (uint8 sublayer = 0; sublayer <= crop->soil.get_max_number_sublayers(); sublayer++)
/*010333v*/	   {
/*010333v*/	      total_fract_root_length[sublayer] = 0.0;
/*030722_*/			L[sublayer] = 0;
/*030722_*/			density_area[sublayer] = 0;
/*030722_*/			root_density_at_layer_bottom[sublayer]= 0;
/*010333v*/    }
   float64 sum_fract_root = 0.0;
/*_______*/
/*030722_*/ 	float64 bottom_soil_depth = 0;
/*030722_*/ 	float64 density_area_sum = 0;
/*030722_*/ 	float64 max_root_density = parameters->surface_root_density * root_length / (parameters->max_root_depth - start_rooting_depth);
/*030722_*/ 	L[soil.rooting_sublayer-1] = -(1.0 - exp(parameters->root_density_distribution_curvature  * ( root_length - bottom_soil_depth)));
/*030722_*/    root_density_at_layer_bottom[soil.rooting_sublayer-1] = max_root_density;
/*030722_*/   	for (sublayer = soil.rooting_sublayer; sublayer <= SOIL_get_num_sublayers_eff(); sublayer++)
/*030722_*/		{
/*030722_*/			bottom_soil_depth += soil.get_thickness(sublayer);
/*030722_*/  		L[sublayer] =
/*030722_*/			(root_length >= bottom_soil_depth)
/*030722_*/			? -(1.0 - exp(parameters->root_density_distribution_curvature  * ( root_length - bottom_soil_depth)))
/*030722_*/       : 0.0 ;
/*030722_*/       root_density_at_layer_bottom[sublayer] = max_root_density * L[sublayer] / L[soil.rooting_sublayer-1];
/*030722_*/		   root_density[sublayer] = (root_density_at_layer_bottom[sublayer] + root_density_at_layer_bottom[sublayer-1])/2.0;
/*_______*/			density_area[sublayer] = soil.get_thickness(sublayer) * (L[sublayer] + (L[sublayer-1] - L[sublayer])/2);
/*_______*/			density_area_sum += density_area[sublayer];
  	}

/*981021P*/    for (sublayer = SOIL_rooting_sublayer; sublayer <= SOIL_get_num_sublayers_eff(); sublayer++)
/*010322v*/    {  active_fract_root_length[sublayer] =
/*010322v*/       total_fract_root_length[sublayer] =
/*030721_*/        density_area[sublayer] /  density_area_sum;
/*010322v*/       sum_fract_root += total_fract_root_length[sublayer];
   }
/*_______*/
/*981023P*/    if (is_zero(sum_fract_root,0.0000001))
/*981023P*/       sum_fract_root = 1.0;
/*981009P*/    float64 soil_depth_limit = SOIL_get_depth(SOIL_get_num_sublayers_eff()) - (SOIL_get_thickness(SOIL_get_num_sublayers_eff())/2.0);
/*981009P*/    if (((root_length + SOIL_get_depth(SOIL_rooting_sublayer-1)) > soil_depth_limit) &&
/*_______*/	    (! is_essentially(sum_fract_root,1.0,0.000000001)))
   {
/*981021P*/      for (sublayer = SOIL_rooting_sublayer; sublayer <= SOIL_get_num_sublayers_eff(); sublayer++)
/*010322v*/         total_fract_root_length[sublayer] *= 1.0 / sum_fract_root;
/*981021P*/      for (sublayer = SOIL_get_num_sublayers_eff()+1; sublayer <=  MAX_soil_sublayers ;sublayer++)
/*010322v*/         total_fract_root_length[sublayer] = 0.0;
   }
}
#endif
//______________________________________________________________________________
float64 const *Crop_interfaced_CropGro::get_total_fract_root_length_m() const
{
   return 0; // NYI total_fract_root_length;  // 0 should turn off the fraction or root length adjustment in soil PAW calculation (since CropGro doesn't have total_fract_root_length
}

               // This will need to return an array, not sure what
               // to fill it with in the case of CropGro
//______________________________________________________________________________
float64  Crop_interfaced_CropGro::get_latest_yield_kg_m2() const
{  // I dont know what is the yield biomass, I presume the total seed weight.
   return crop->grow.SDWT;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_planting_or_restart_date() const
{  // - Planting date   (YRPLT)
   return crop->yrplt;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_emergence_date()            const
{  // - emergence date   (YRPLT with offset of NVEG0 days ?)
   emergence_date.set(crop->yrplt);
   emergence_date.inc_day(crop->rstages.NVEG0);
   return emergence_date;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_flowering_date()            const
{  // - start of flowering date   (YRNR1 ?)
   flowering_date.set_date32(crop->rstages.YRNR1);
   return flowering_date;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_tuber_init_date()           const
{  //- start of grain filling date  (YRNR5 ?)
   tuber_init_date.set_date32(crop->rstages.YRNR5);
   return tuber_init_date;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_grain_filling_date()        const
{  //- start of grain filling date  (YRNR5 ?)
   grain_filling_date.set_date32(crop->rstages.YRNR5);
   return grain_filling_date;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_maturity_date()             const
{  // - start of physiological maturity (YRNR7 ?)
   maturity_date.set_date32(crop->rstages.YRNR7);
   return maturity_date;
}
//______________________________________________________________________________
const CORN::Date &Crop_interfaced_CropGro::get_harvest_date()              const
{  // - Harvest date (MDATE)
//             //   (It appears that the variable mdate is used both for maturity harvest and premature termination)float64 result = 0;
   return crop->mdate;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_accum_degree_days()                   const
{  // Warning I am not sure if DTX is already accumulated
//             // also I dont think DTX is in CropSyst thermal time units.
   return 999999.9;
}
//______________________________________________________________________________
/* now handled by interface (default)
float64 Crop_interfaced_CropGro::get_dry_biomass_kg_ha()                   const
{  return
      #ifdef CROP_ORCHARD
      get_fruit_dry_biomass_kg_ha() +
      #endif
      get_canopy_biomass_kg_ha();
}
//______________________________________________________________________________
*/
float64 Crop_interfaced_CropGro::get_act_root_biomass_kg_ha()              const
{
   return per_m2_to_per_ha(get_act_root_biomass_kg_m2());
}
//______________________________________________________________________________
#ifdef CROP_ORCHARD
float64 Crop_interfaced_CropGro::get_fruit_dry_biomass_kg_ha() const
{  // CropGro does not have fruit model so simply return 0.0
   return 0.0;
}
#endif
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_canopy_biomass_kg_ha() const
{  // current above ground (canopy) biomass  TGROW or  TOPWT  (I think TOPWT)
   return g_m2_to_kg_ha(crop->grow.TOPWT);
}
//______________________________________________________________________________
float64  Crop_interfaced_CropGro::get_VPD_daytime() const
{  // This is only for output currently we dont output this from CropGro interface
//   float64 daytime_VPD      = weather.calc_avg_vapor_pressure_deficit(true);
   return 9999.9; // daytime_VPD;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_VPD_daytime_mean() const
{  return  9999.9;  // This is only for output currently we dont output this from CropGro interface
//                vapor_deficit_stats.get_mean();
}
//______________________________________________________________________________

/// BIG WARNING:  Claudio is going to provide PAR conversion

float64 Crop_interfaced_CropGro::get_intercepted_PAR_MJ_m2() const
{  float64 result = 9999.9;
//   NYI
   return result;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_intercepted_PAR_accum_MJ_m2() const
{  float64 result = 9999.9;
//   NYI
   return result;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_intercepted_PAR_season_accum_MJ_m2() const
{  float64 result = 9999.90;
//   NYI
   return result;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_peak_LAI() const
{  // float64 peak_LAI = CORN_max(get_GAI(),get_LAI());
   return crop->grow.LAIMX;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_total_yield_kg_m2() const
{  float64 result = per_ha_to_per_m2(get_canopy_biomass_kg_ha()) * 0.5;  // WARNING, this is only a stub, I need to know what is the yield variable in CropGro
   return result;
}
//______________________________________________________________________________
/*_______*/
float64 Crop_interfaced_CropGro::get_N_stress_index() const
{  // - nitrogen stress         NSTRES
   return N_stress_stats.index;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_nitrogen_stress() const
{  return N_stress_stats.get_current(); }
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_N_stress_index_mean() const
{
   return 1.0 - N_stress_stats.get_mean();
}
//______________________________________________________________________________
//   virtual float64      get_N_uptake_accum_kg_ha() const;
float64 Crop_interfaced_CropGro::get_N_above_ground_accum_kg_ha() const
{  //current above ground nitrogen content            WTNCAN ?
//                WTNCAN //Mass of N in canopy (g[N] / m2[ground])
   return g_m2_to_kg_ha(crop->grow.WTNCAN);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_N_concentration_kgN_kgBM() const
{  // current plant N concentration   RNITP ?
//_______//    RNITP True nitrogen concentration in leaf tissue for photosynthesis reduction. (%)
   return crop->grow.RNITP;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::update_root_depth(float64 soil_rooting_depth,float64 total_soil_depth )
{  // NYI  I think I can just return root depth, cropsyst recalculates this
   return get_root_length_m();
}
//_1998-19-23___________________________________________________________________
bool Crop_interfaced_CropGro::setup_structure(CORN::Data_record &data_rec,bool for_write)
//190204 ,const Desired_variables &desired_vars)
{
//   NYI
   return true;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_gaseous_N_loss_kg_m2() const
{  // gaseous N loss (for the day and/or accumulated)
   float64 total_loss_g_m2 = crop->grow.WTNLO + crop->grow.WTNNO  + crop->grow.WTNRO + crop->grow.WTNSDO + crop->grow.WTNSHO + crop->grow.WTNSO;
   return g_m2_to_kg_m2(total_loss_g_m2);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_N_fixation_kg_m2() const
{  // N fixation (for the day and accumulated)         NFIXN  and WTNFX (accum)
   return g_m2_to_kg_m2(crop->nfix.NFIXN);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_daily_N_fert_requirements_kg_m2() const
{  // I think it would be simply the uptake, this is done differently in CropSyst
   return g_m2_to_kg_m2(crop->nuptak.TRNU);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_N_content_kg_m2() const
{  // total currrent plant N
//_______//    WTNTOT Total plant N content (g[N] / m2[ground])
   return g_m2_to_kg_m2(crop->grow.WTNTOT);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_seedling_N_kg_m2() const
{  // initial N amount from seedling                   SEEDNI
//_______//    //Seed or transplant N at planting (g[N] / m2)
   return g_m2_to_kg_m2(crop->grow.SEEDNI);
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_act_transpiration_m() const
{
// I can't find any transpration related variables just returning the potential, (I think)
   return cm_to_m(TRWUP);
}
//______________________________________________________________________________
/*190705 moved to reported
float64 Crop_interfaced_CropGro::get_recorded_act_transpiration_m() const
{  // CropSyst crop has a little adjustment for act_transpiration cropgro probably does not have this so we just return AT.
   return get_act_transpiration_m();
}
//______________________________________________________________________________
*/
float64 Crop_interfaced_CropGro::get_canopy_biomass_kg_m2() const
{
   return g_to_kg(crop->grow.TOPWT);;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_act_root_biomass_kg_m2() const
{  // RTWT;  / Dry mass of root tissue, including C and N (g[root] / m2[ground])
   return g_to_kg(crop->grow.RTWT);
}
//______________________________________________________________________________
#ifdef CHECK_USED
080728
float64 Crop_interfaced_CropGro::get_GAI_canopy_biomass_kg_m2() const
{  // I think this is only for output, CropGro probably doesn't have this variable so I just output the canopy_biomass
   return get_canopy_biomass_kg_m2();
}
#endif
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_LAI(bool from_canopy) const
{  // I dont think any units conversion
   return crop->grow.XLAI;
}
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_GAI() const
{ // I dont think any units conversion
   return crop->grow.XHLAI;
}
//______________________________________________________________________________
/*obsolete unused
float64 Crop_interfaced_CropGro::get_GAI_today() const
{  return new_GAI; }
//______________________________________________________________________________
*/
float64 Crop_interfaced_CropGro::get_water_stress() const
{  return water_stress_stats.get_current(); }
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_water_stress_index() const
{  return water_stress_stats.index; }
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_water_stress_index_mean() const
{  return (1.0-water_stress_stats.get_mean()); }
//______________________________________________________________________________

// Note, at this time, I am dumping the water stress computed by CropGro to temperature stress
// CropGro does not have temperature stress.

//______________________________________________________________________________
float64 Crop_interfaced_CropGro::get_temperature_stress() const
{  // Don't think cropgro has this // this is for output only anyway
   return 1.0; // SWFAC_stats.current;
}
float64 Crop_interfaced_CropGro::get_temperature_stress_index() const
{  // Don't think cropgro has this // this is for output only anyway
   return 0.0; //SWFAC_stats.index;
}
float64 Crop_interfaced_CropGro::get_temperature_stress_index_mean() const
{ // Don't think cropgro has this // this is for output only anyway
   return 0.0; // (1.0-SWFAC_stats.get_mean());
}
float64 Crop_interfaced_CropGro::get_overall_growth_stress() const
{  // Don't think cropgro has this // this is for output only anyway
   float64 overall_stress = CORN_min(get_water_stress(),get_nitrogen_stress());
   return overall_stress;
}
//______________________________________________________________________________
Land_use Crop_interfaced_CropGro::param_land_use()                         const
{ return parameters->land_use_labeled.get(); }
//______________________________________________________________________________
float64 Crop_interfaced_CropGro::param_max_LAI() const
{  // This is exposed only for fast graph scaling
//             // CropGro may already have this, I haven't found it
//             // I could also get it from parameters as entered for CropSyst
   return 5.0;
}
//_param_max_LAI_______________________________________________________________/
void Crop_interfaced_CropGro::biomass_to_residue
(bool include_roots
,float64 straw_to_residue)
{
//   NYI
}
//_biomass_to_residue_______________________________________________1997-01-30_/
float64 Crop_interfaced_CropGro::remove_biomass_X
(float64 percent_biomass_removed                                                 //970622
,Biomass_fate_parameters &biomass_fate_parameters                                //040610
,bool due_to_freezing                                                            //000320
,bool due_to_death                                                               //000320
,const CORN::Date &  today)
{  float64 result = 0;
//   NYI
   return result;
}
//_remove_biomass_X____________________________________________________________/
bool Crop_interfaced_CropGro::harvest
(Date32 today
,const Biomass_fate_parameters &harvest_biomass_fate_parameters)                 //041206
{
   //NYI
   return true;
   //020311_*/    Returns true if could harvest
}
//_harvest_____________________________________________________________________/
void Crop_interfaced_CropGro::harvest_crop(const Biomass_fate_parameters &harvest_biomass_fate_parameters,bool terminate)
{
}
//_harvest_crop_____________________________________________________2004-12-06_/
nat32 Crop_interfaced_CropGro::relinquish_triggered_synchronizations()
{  // returns the current triggered syncs and also clears resets them.
   // This is actually identical to  Crop_interfaced_complete
   nat32 triggered_syncs = triggered_synchronizations;
   triggered_synchronizations = 0;
   return triggered_syncs;
}
//_relinquish_triggered_synchronizations____________________________2003-11-20_/
float64 Crop_interfaced_CropGro::uptake_N_from_soil()
{  float64 total_N_uptake_kg_m2 = 0;
   Soil_nitrogen_interface *soil_N = soil.mod_nitrogen();                        //060206
   if (soil_N)  //  If CropSyst simulation has nitrogen enabled then chemicals will exist
   {
      float64 total_N_NO3_kg_m2 = 0;
      float64 total_N_NNH_kg_m2 = 0;
      for (int sublayer = 1; sublayer <= NL_alloc; sublayer++)
      {
         float64 N_NO3_uptake_sublayer  //kg N/m2 soil
         = g_m2_to_kg_m2(crop->nuptak.UNO3[sublayer]);
         total_N_NO3_kg_m2 += soil_N->uptake_N_mass_from_NO3(sublayer,N_NO3_uptake_sublayer);
         float64 N_NH4_uptake_sublayer  //kg N/m2 soil
         = g_m2_to_kg_m2(crop->nuptak.UNH4[sublayer]);
         total_N_NNH_kg_m2 += soil_N->uptake_N_mass_from_NH4(sublayer,N_NH4_uptake_sublayer);
      }
      soil_N->commit_diffs();                                                    //120730
      total_N_uptake_kg_m2 = total_N_NO3_kg_m2 + total_N_NNH_kg_m2;
/*
//total_N_uptake_kg_m2 should match TRNU
// The following output is for debugging only:
float64 TRNU_kg_m2 = g_m2_to_kg_m2(crop->nuptak.TRNU);
if (!is_essentially(total_N_uptake_kg_m2,TRNU_kg_m2,0.0001))
cout << "Total N extracted does not match total N uptake" << total_N_uptake_kg_m2 << "<>" << TRNU_kg_m2<< endl;
*/
   }
   return total_N_uptake_kg_m2;
}
//_uptake_N_from_soil_______________________________________________2003-11-29_/
float64 Crop_interfaced_CropGro::get_N_mass_canopy_kg_m2(bool before_harvest) const
{  //NYI return xxxx
   return 0.0;
}
//______________________________________________________________________________
//_namespace_CropSyst__________________________________________________________/
#endif

