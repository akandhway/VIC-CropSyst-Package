
#  include "soil/erosion_RUSLE.h"
#  include "cs_UED_harvest.h"
#  include <math.h>
#  include "corn/math/moremath.h"
#  include "corn/math/compare.h"
#  include "soil/erosion_RUSLE.h"

   // Includes for precipitation/rain factor
#  include "common/weather/daily.h"
#  include "common/weather/storms.h"
#  include "common/weather/database/weather_database.h"
#  include "corn/const.h"
#include "UED/library/std_codes.h"
namespace RUSLE {
//______________________________________________________________________________
float32 coverfraction_16[3][8] =
 {{0,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0 ,0.0},
  {0,1.0 ,0.75,0.5 ,0.25,0.19,0.13,0.0},
  {0,0.02,0.15,0.32,0.55,0.7 ,1.0 ,1.0}};
//_coverfraction_16____________________________________________________________/
float64 coverfactor(float64 cover)
{
   int i = 1;
   while (cover < (float64)(coverfraction_16[1][i])) i += 1;
   return (i == 1)
      ? 0.02
       : (float64)(coverfraction_16[2][i] -
        (coverfraction_16[2][i] - coverfraction_16[2][i - 1])  *
        ((float32)cover - coverfraction_16[1][i]) /
        (coverfraction_16[1][i-1] - coverfraction_16[1][i]));
}
//_coverfactor_________________________________________________________________/
float64 mulchfactor(float64 residue_ground_cover,float64 shallowresidue)
{  // data from McCool (pers. comm.) as used in RUSLE; aug90 by FvE
  float64 sr = exp(-5 * residue_ground_cover);
  if (sr > 1)
     sr = 1;
  float64 ir = exp(-2.655 * shallowresidue);
  return sr * ir;
}
//_mulchfactor______________________________________________________1997-06-17_/
float32 r_factors16[3][10] =
     {{0,0,0,0,0,0,0,0,0,0},
      {0,    1  ,29 ,57 ,92 ,113,176,204,330,365},
      {0,    4.4,4.9,4.4,0.3,1.1,0.2,0.5,4.4,4.4}};
//_r_factors16_________________________________________________________________/
//RUSLE_K_Factor
#define SLOPE_COLUMN 0
#define LOW 1
#define MODERATE 2
#define HIGH 3
//RUSLE_L_Factor
float32 rill_interrill_ratio_16[20][4] =
       {{  0  ,   0  ,  0   ,  0   },   //  <- Spacer
        { 0.2 , 0.02 , 0.04 , 0.07 },
        { 0.5 , 0.04 , 0.08 , 0.16 },
        { 1.0 , 0.08 , 0.15 , 0.26 },
        { 2.0 , 0.14 , 0.24 , 0.39 },
        { 3.0 , 0.18 , 0.31 , 0.47 },
        { 4.0 , 0.22 , 0.36 , 0.53 },
        { 5.0 , 0.25 , 0.40 , 0.57 },
        { 6.0 , 0.28 , 0.43 , 0.60 },
        { 8.0 , 0.32 , 0.48 , 0.65 },
        {10.0 , 0.35 , 0.52 , 0.68 },
        {12.0 , 0.37 , 0.55 , 0.71 },
        {14.0 , 0.40 , 0.57 , 0.72 },
        {16.0 , 0.41 , 0.59 , 0.74 },
        {20.0 , 0.44 , 0.61 , 0.76 },
        {25.0 , 0.47 , 0.64 , 0.78 },
        {30.0 , 0.49 , 0.66 , 0.79 },
        {40.0 , 0.52 , 0.68 , 0.81 },
        {50.0 , 0.54 , 0.70 , 0.82 },
        {60.0 , 0.55 , 0.71 , 0.83 }};
//_rill_interrill_ratio_16_____________________________________________________/
float64 lookup_rill_interrill_ratio(float32 grade_16 , int  compactness)
{  float64 result = 0;
  if (grade_16 <= 0.2)
        result = rill_interrill_ratio_16[1][compactness];
  else
  if (grade_16 > 60.0 )
        result = rill_interrill_ratio_16[19][compactness];
  else
  { // Compute values for intermediate slopes
    int i;
    for (i = 2;  i <= 19; i++)
      if (grade_16 < rill_interrill_ratio_16[19][SLOPE_COLUMN] )
         return result = rill_interrill_ratio_16[i-1][compactness] +
            ((grade_16 - rill_interrill_ratio_16[i-1][SLOPE_COLUMN]) *
             (rill_interrill_ratio_16[i][compactness] - rill_interrill_ratio_16[i-1][compactness])) /
            (rill_interrill_ratio_16[i][SLOPE_COLUMN] - rill_interrill_ratio_16[i-1][SLOPE_COLUMN]);
  }
  return result;
}
//_lookup_rill_interrill_ratio_________________________________________________/
Soil_erosion::Soil_erosion
(Soil_base                       &soil_
,const CS::meteorology::Storms   &storms_                                        //160506
,const Precipitation             &precipitation_eff_ //used only for output      //160510
,const CORN::date32              &today_)
:CS::Simulation_element_abstract(today_)                                         //160503
,soil                      (soil_)
,precipitation_eff         (precipitation_eff_)                                  //160510
,rainfall_energy_intensity (0.0)
,soil_conservation_factor  (1.0)                                                 //160506
,C_factor_tally            ()                                                    //160506
,total_soil_loss           (0.0)
,clod_factor_rate          (0.0)
,clodding_factor           (1.0)
,storm_energy()                                                                  //160506
,storm_R_tally()
{}
//_Soil_erosion:constructor____________________________________________________/
void Soil_erosion::know_weather
(const float32       *month_mean_peak_halfhour_precipitation_check_units_)
{  month_mean_peak_halfhour_precipitation_check_units
   = month_mean_peak_halfhour_precipitation_check_units_;
}
//_know_weather_____________________________________________________2009-11-30_/
float64 Soil_erosion::calc_slope_length_factor()
{  static const float64 unit_plot_length_m = 22.12848;                           //050324
   // 22.12848m is 72.6ft  (unit plot length used by RUSLE) was 22.13
   float64 rill_interrill_erosion_ratio = lookup_rill_interrill_ratio
      (soil.parameters_ownable->get_steepness_percent(),MODERATE);
   float64 variable_slope_length_component = rill_interrill_erosion_ratio
      / (1+rill_interrill_erosion_ratio);
   return pow(soil.parameters_ownable->get_slope_length() / unit_plot_length_m
             ,variable_slope_length_component);                                  //050324
}
//______________________________________________________________________________
float64 Soil_erosion::calc_steepness_factor()
{  //RUSLE_S_Factor
   float64 sin_theta = sin(atan(soil.parameters_ownable->get_steepness_percent()/100));
   //The conditions of this function modified                                     //960828
   return
   (soil.parameters_ownable->get_slope_length() < 5.0)
      ?  3.0 * pow(sin_theta,0.8) + 0.56
      : (soil.parameters_ownable->get_steepness_percent() >= 9.0)
         ? (16.8 * sin_theta - 0.5)
         : (10.8 * sin_theta + 0.03);
}
//_calc_steepness_factor_______________________________________________________/
float64 Soil_erosion::calc_annual_soil_loss()
{  float64 slope_length_factor   = calc_slope_length_factor();                   //990218
   float64 steepness_factor      = calc_steepness_factor();                      //990218
   float64 erodibility_factor    = calc_soil_erodibility_factor();               //990218
   float64 C_factor_sum = C_factor_tally.get_sum();

   // This can only be done for the year
   total_soil_loss =
      rainfall_energy_intensity // R
    * slope_length_factor       // L
    * steepness_factor          // S
    * erodibility_factor        // K
    * C_factor_sum              // C
    * soil_conservation_factor; // P
   return total_soil_loss;
}
//_calc_annual_soil_loss_______________________________________________________/
float64 Soil_erosion::calc_soil_erodibility_factor()
{  float64 sand = soil.get_sand_percent(1) ;
   float64 clay = soil.get_clay_percent(1) ;
   float64 silt = soil.get_silt_percent(1);
   float64 particle_diameter =
     exp((sand/100.0) * CORN_ln(1.025) +
         (silt/100.0) * CORN_ln(0.026) +
         (clay/100.0) * CORN_ln(0.001));
   return
       // 7.594 *   This was would be to convert to US customary units
      (0.0034 + 0.0405 * exp( -0.5 * CORN_sqr((log10(particle_diameter) + 1.659) /0.7101) ));
}
//_calc_soil_erodibility_factor_____________________________________1998-01-12_/
bool Soil_erosion::start_year()                                    modification_
{  total_soil_loss      = 0.0;
   C_factor_tally       .reset();                                                //160506
   storm_energy         .reset();
   storm_R_tally        .reset();
   for (CORN::DOY doy = 0; doy <=366; doy++)
   {
      //160513 storm_energy_fract_contrib [doy] = 0.0;                           //160513
      R_fract_contrib            [doy] = 0.0;                                    //160503
      mulch_factor               [doy] = 0.0;
      cover_factor               [doy] = 0.0;
      moisture_factor            [doy] = 0.0;
      output_precip_eff_mm       [doy] = 0.0;
      output_peak_intensity      [doy] = 0.0;
   }
   return true;
}
//_start_year_______________________________________________________2009-11-30_/
bool Soil_erosion::process_day()                                   modification_
{
   if (month_mean_peak_halfhour_precipitation_check_units)
   {  CORN::DOY doy = simdate.get_DOY();
      float64 precipitation_eff_m = precipitation_eff.get_m();
      output_precip_eff_mm[doy] = m_to_mm(precipitation_eff_m); // only for output //160510
      float64 storm_energy_today = 0;  // MJ_per_ha
      CS::meteorology::Storm_generator storms
         (simdate_raw,precipitation_eff                                          //170524_151015
         ,month_mean_peak_halfhour_precipitation_check_units);                   //000615
      if (precipitation_eff_m > 0.000001)
      {
         CORN::Dynamic_array<float32> precipitation_in_intervals_mm;             //160512
         storms.start_day();
         CORN::Time_second_clad event_start_time((datetime64)0);                 //990505
            // time the event starts is generated.
         storms.get_precipitation_mm_by_interval(true,30 /*60 */,event_start_time,precipitation_in_intervals_mm);
         CS::meteorology::Storm_energy storm_energy_calculator                   //160511
            (precipitation_in_intervals_mm,30);
         storm_energy_today = storm_energy_calculator.get_energy_MJ_per_ha();
            // kinetic energy
         float32 intensity_30min_max = storms.get_peak_precip_intensity_mm();
         output_peak_intensity[doy] = intensity_30min_max;                       //160506
         float32 R_daily = intensity_30min_max * storm_energy_today;
         storm_R_tally.append(R_daily);
         //I30_tally.append(intensity_30min_max);
      } else // no storm energy today;
         storm_R_tally.append(0.0);
      storm_energy.set(doy,storm_energy_today);
   } // else no data for storm generation (erosivity factor 0.0)  RLN Not sure if return 1.0 //or 0.0 (has been return 0.0)
   return month_mean_peak_halfhour_precipitation_check_units;
}
//_process_day______________________________________________________2016-05-06_/
bool Soil_erosion::end_day()                                       modification_
{  return CS::Simulation_element_abstract::end_day();
}
//_end_day__________________________________________________________2016-05_11_/
bool Soil_erosion::end_year()                                      modification_
{
   nat16 days_in_this_year = simdate.days_in_this_year();
   //160513 float32 storm_energy_annual = storm_energy.get_sum();
   float32 R_annual = storm_R_tally.get_sum();                                   //160513
   for (CORN::DOY doy = 1; doy <= days_in_this_year; doy ++)
      R_fract_contrib[doy] = storm_R_tally.get(doy) / R_annual;                  //160513
   calc_cover_factor_annual(); // must be called after getting storm_energy_fract_contrib
   rainfall_energy_intensity = calc_rainfall_erosivity_factor(simdate.get_year());
   calc_annual_soil_loss();                                                      //160506
   return true;
}
//_end_year_________________________________________________________2016-05-06_/
float64 Soil_erosion::calc_daily_moisture_factor()                  calculation_
{  float64 moisture_influence_depth // m
      = std::min<float64>(1.8,soil.layers->get_depth_m());
   nat8 significant_sublayers = soil.layers->sublayer_at(moisture_influence_depth,false);
   float64 total_weighted_available_water = 0;
   for (nat8 sublayer = 1 ; sublayer <= significant_sublayers; sublayer++)
      total_weighted_available_water
       += soil.get_plant_avail_water(sublayer) * soil.layers->get_thickness_m(sublayer); //150424
   return total_weighted_available_water/moisture_influence_depth;
}
//_calc_daily_moisture_factor_______________________________________1998-07-31_/
bool Soil_erosion::know_cover
(float64 crop_fract_canopy_cover
,float64 daily_residue_surface
,float64 daily_residue_shallow)                                       cognition_
{
   CORN::DOY doy = simdate.get_DOY();
   mulch_factor   [doy] = mulchfactor(daily_residue_surface, daily_residue_shallow);
   cover_factor   [doy] = coverfactor(crop_fract_canopy_cover);
   moisture_factor[doy] = calc_daily_moisture_factor();
   return true;
}
//_know_cover_______________________________________________________2016-05-06_/
float64 Soil_erosion::calc_cover_factor_annual()
{  // calculation of factors FOR crop factor of USLE
#ifdef DEBUG_EROSION
std::ofstream cfactors("output\\cfactors.dat",std::ios::app);
cfactors
<< "DOY" << '\t'
<< "soil_loss_ratio" << '\t'
<< "mulch_factor" << '\t'
<< "cover_factor" << '\t'
<< "clodding_factor" << '\t'
<< "moisture_factor" << '\t'
<< "Rfract" << '\t'
<< "C_factor"
<< std::endl;
#endif

   for (CORN::DOY doy = 1; doy <= 365; doy++)
   {
      float32 soil_loss_ratio
         = mulch_factor   [doy] // includes surface and subsurface residue
               // replaces Prior Land Use subfactor * surface cover subfactor
         * cover_factor   [doy]
         * clodding_factor // NYI
         * moisture_factor[doy];
      float32 C_factor                                                           //050505
      =  R_fract_contrib[doy]                                                    //160513
       * soil_loss_ratio;
      C_factor_tally.append(C_factor);                                           //160506
   // Warning I think this may need to be a running stat tally of daily rusc
   // however REI_fract_contrib is day/annual fraction so that is probably why it is OK
#ifdef DEBUG_EROSION
cfactors
<< (int) doy << '\t'
<< soil_loss_ratio << '\t'
<< mulch_factor[doy] << '\t'
<< cover_factor[doy] << '\t'
<< clodding_factor << '\t'
<< moisture_factor[doy] << '\t'
<< R_fract_contrib[doy] << '\t'
<< C_factor
<< std::endl;
#endif
   }
  return C_factor_tally.get_sum();                                               //050505
}
//_calc_cover_factor_annual_________________________________________2016-05-06_/
float64 Soil_erosion::calc_rainfall_erosivity_factor
(CORN::Year year)                                                          const
{  // R factor
   return storm_R_tally.get_sum();
}
//_calc_rainfall_erosivity_factor___________________________________2016-05-06_/
bool Soil_erosion::dump_year_days(std::ostream &detail_text_file)
{
   #ifdef RUSLE_DETAIL
   for (CORN::DOY doy = 1; doy <= simdate.days_in_this_year(); doy++)
   {
      CORN::Date_clad_32 dump_date(simdate.get_year(),doy);
      detail_text_file
      << dump_date.get_date32()
      << '\t' << output_precip_eff_mm[doy]
      << '\t' << storm_energy.get(doy)
      << '\t' << output_peak_intensity[doy]
      << '\t' << C_factor_tally.get(doy)
      << '\t' << mulch_factor    [doy]
      << '\t' << moisture_factor [doy]
      << std::endl;
   }
   #endif
   return true;
}
//_calc_cover_factor_annual_________________________________________2016-05-06_/
}//_namespace_RUSLE____________________________________________________________/

