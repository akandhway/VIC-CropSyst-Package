#include "crop/yield_response.h"
#include "common/weather/any_timestep/temperature_est.h"
namespace CropSyst
{
//______________________________________________________________________________
bool Yield_response::end_day()                                     modification_
{  if (!is_satisfied() && is_acheived()) commit();
   return true;
}
//______________________________________________________________________________
bool Observations_yield::is_acheived()                              affirmation_
{  return after.get_count() >= (nat32)wing_size;
}
//______________________________________________________________________________
bool Observations_yield::is_satisfied()                             affirmation_
{  return stats!=0;
}
//______________________________________________________________________________
bool Observations_yield::know_flowering()
{
   advance_flowering = true;
   return true;
}
//______________________________________________________________________________
bool Observations_yield::know_filling()
{
   advance_filling = true;
   return true;
}
//______________________________________________________________________________
bool Yield_grain_water_response::know_flowering()
{  return stress_obs.know_flowering();
}
//______________________________________________________________________________
bool Yield_grain_water_response::know_filling()
{  return stress_obs.know_filling();
}
//______________________________________________________________________________
bool Yield_grain_water_response::is_satisfied()                     affirmation_
{  return stress_obs.is_satisfied(); }
//______________________________________________________________________________
bool Yield_grain_water_response::is_acheived()                      affirmation_
{  return stress_obs.is_acheived();
}
//_____________________________________________________________________________
bool Observations_yield::end_day()
{
   current->append(observation_ref);
   if (advance_flowering)
   {  current = &during;
      advance_flowering = false;
   }
   if (advance_filling)
   {  current = &after;
      advance_filling = false;
   }
   if (is_acheived())  current = 0;
   return true;
}
//______________________________________________________________________________
bool Yield_grain_water_response::end_day()                         modification_
{  if (!is_satisfied())
   {  stress_obs.end_day();
      //abanded satisifed = stress_index_after.count() >= wing_size;
   }
   return Yield_response::end_day();
}
//_Yield_grain_water_response::end_day_________________________________________/
bool Observations_yield::commit()                                  modification_
{  stats = new CORN::statistical::Sample_dataset_clad<float64,float32,float32>;
   stats->append_dataset_spanning_float32(before,Span(before.get_count()).last(3));
   stats->append_dataset_float32(during);
   stats->append_dataset_spanning_float32(after,Span(after.get_count()).first(3));
   stats->recompute();
   return true;
}
//_Observations::commit________________________________________________________/
bool Yield_grain_water_response::commit()                         modification_
{  return stress_obs.commit();
}
//_Yield_grain_water_response::commit__________________________________________/
float64 Yield_grain_water_response::get_factor()                           const
{
   float64 stress_factor_mean =
      stress_obs.stats ? stress_obs.stats->get_mean() : 1.0;                     //200519

   return is_satisfied()
   ? pow(stress_factor_mean// mean index
        ,stress_sensitivity)
   : 1.0;
}
//_Yield_grain_water_response::get_factor______________________________________/
Yield_grain_temperature_response::Yield_grain_temperature_response
(float64 stress_sensitivity_
,float64 temperature_optimum_
,float64 temperature_damaging_
,const Air_temperature_maximum &air_temperature_max_
,const Air_temperature_minimum &air_temperature_min_
, nat8 wing_size_)
: Yield_response                                                              ()
, temperature_optimum                                    (temperature_optimum_)
, temperature_damaging                                   (temperature_damaging_)
, Tmax_obs                                   (air_temperature_max_,wing_size_+1)
, Tmin_obs                                   (air_temperature_min_,wing_size_+1)
   // we add 1 day to the wing because the hourly temperature
   // needs the day before and the day after
, factor                                                                   (1.0)
{}
//______________________________________________________________________________
bool Yield_grain_temperature_response::know_flowering()
{  return Tmax_obs.know_flowering()
      &&  Tmin_obs.know_flowering();
}
//______________________________________________________________________________
bool Yield_grain_temperature_response::know_filling()
{  return Tmax_obs.know_filling()
      &&  Tmin_obs.know_filling();
}
//______________________________________________________________________________
bool Yield_grain_temperature_response::end_day()                   modification_
{  if (!is_satisfied())
   {
      Tmax_obs.end_day();
      Tmin_obs.end_day();
   }
   return Yield_response::end_day();
}
//_Yield_grain_water_response::end_day_________________________________________/
bool Yield_grain_temperature_response::is_satisfied()               affirmation_
{  return Tmax_obs.is_satisfied()
      &&  Tmin_obs.is_satisfied();
}
//_Yield_grain_temperature_response::is_satisfied______________________________/
bool Yield_grain_temperature_response::is_acheived()                affirmation_
{  return Tmax_obs.is_acheived()
     &&   Tmin_obs.is_acheived();
}
//_Yield_grain_temperature_response::is_acheived_______________________________/
float64 Yield_grain_temperature_response::get_factor()                     const
{  return factor;
}
//_Yield_grain_temperature_responseget_factor__________________________________/
bool Yield_grain_temperature_response::commit()                    modification_
{
   Tmax_obs.commit();
   Tmin_obs.commit();

   Temperature_estimator temperature_hourly(60);
   temperature_hourly.reset
      (Tmax_obs.stats->get(1),Tmax_obs.stats->get(0)
      ,Tmin_obs.stats->get(1),Tmin_obs.stats->get(2));

   statistical::Sample_cumulative<float32,float32> HSF_stats_flowering;
   for (nat8 d = 1; d < Tmax_obs.stats->get_count(); d++)
   {
      temperature_hourly.set_max_today_min_tomorrow
         (Tmax_obs.stats->get(d),Tmin_obs.stats->get(d+1));
      CORN::Dynamic_array<float32> T_hourly;
      temperature_hourly.get_by_interval(T_hourly);
      statistical::Sample_cumulative<float32,float32> HSF_stats_day;
      for (nat8 h = 0; h < 24; h++)
      {
         float64 T_hour = T_hourly.get(h);
         float64 HSF_hour // Heat stress factor hour
            = (T_hour <= temperature_optimum)   ? 1.0
            : (T_hour >= temperature_damaging)  ? 0.0
            : pow(1.0 - ((T_hour - temperature_optimum)
                     / (temperature_damaging - temperature_optimum)),stress_sensitivity);
         HSF_stats_day.append(HSF_hour);
      }
      float32 HSF_day_mean = HSF_stats_day.get_mean();
      HSF_stats_flowering.append(HSF_day_mean);
   }
   factor = HSF_stats_flowering.get_mean();
   return true;
}
//_Yield_grain_temperature_response::commit_________________________2020-04-24_/
/*
rename
Water adjustement to unstressed harvest index
->
Adjustment based on biomass production prior and during grain filling

This is used to the adjust HI after
the Water and heat stress during flowering is applied

Only applicabled to grain (cereal) crops
Corn and green beans
which are determinate


potatos are affected by heat stress

*/

}//_namespace CropSyst_________________________________________________________/

