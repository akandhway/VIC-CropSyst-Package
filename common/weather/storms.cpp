#include "weather/storms.h"
#include "UED/library/UED_fname.h"
#include "UED/library/database_file.h"
#include "UED/library/std_codes.h"

namespace CS
{
namespace meteorology
{
//______________________________________________________________________________
Storms_common::Storms_common
(const CORN::date32  &today_
,const Precipitation &precipitation_                                             //151015
,const float32        mean_peak_rainfall_[])
: Storm_generator(today_,precipitation_,mean_peak_rainfall_)                     //151015
{}
//_Storms_common:constructor___________________________________________________/
float64 Storms_common::get_eff_precipitation_mm_by_interval
(CORN::Dynamic_array<float32> &precip_X_minute
,float64             &eff_precip_mm
,CORN::Time          &event_start_time
,nat16               &intervals)                                           const
{
   float64 total_precip_mm = precip_X_minute.sum(); // Includes snow             //990501
   if (eff_precip_mm == 0)                                                       //990501
         precip_X_minute.clear(); // Either no precip or snow                    //990501
   else                                                                          //990501
      if (total_precip_mm > 0.0)                                                 //000306
      {  // This could be melting or snow storage                                //990501
         float64 ratio = eff_precip_mm/total_precip_mm;                          //000306
         precip_X_minute.multiply_by(ratio);                                     //000306
      }
      else
      {  float64 diff = eff_precip_mm - total_precip_mm;                         //990501
         for (int i = 0; i < intervals ; i++)                                    //990501
            precip_X_minute.increment_by(i,diff/intervals);                      //990501
                    // spread it out over intervals
      }                                                                          //990501
   float64 distributed_eff_precip_mm = precip_X_minute.sum();                    //000217
   bool effective_distribution_sum_matches
      = CORN::is_approximately(distributed_eff_precip_mm,eff_precip_mm,0.00001);
   assert(effective_distribution_sum_matches);
   return distributed_eff_precip_mm;
}
//_get_eff_precipitation_mm_by_interval________________________________________/
Storms_database::Storms_database
(const CORN::date32          &today_                                             //170524
,const Precipitation         &precipitation_                                     //151015
,const float32                mean_peak_rainfall_[]
,const File_name             &storm_database_filename_)
: Storms_common
(today_
,precipitation_
,mean_peak_rainfall_)
,storm_database
   ( new UED::Database_file_indexed
      (storm_database_filename_.c_str()
      ,(std::ios_base::in )
      ,true)   // Storms are all generated data and potentially large dataset so allowing float16
    )
{
   if (storm_database) storm_database->initialize();
}
//_Storms_database:constructor_________________________________________________/
float64 Storms_database::get_eff_precipitation_mm_by_interval
(CORN::Dynamic_array<float32> &precip_X_minute
,float64             &eff_precip_mm
,CORN::Time          &event_start_time   // time the event starts is generated.  //990505
,nat16               &intervals)                                           const
{
   Storm_event_search search_date
      (simdate_raw);                                                             //170524
      //170524 (today);
   UED::Binary_data_record_cowl *target_date_storm_record =
      storm_database->locate_data_record
         (UED::Record_type_distinguisher_interface::UED_data_set_date_time_var_units
         ,UED::STD_VC_precipitation.get()
         ,search_date);
      // If there is no such record then there is not precip for the day
   if (target_date_storm_record)
   {
      precip_X_minute.copy(target_date_storm_record->ref_data_values());
      nat32 zero_value_count = precip_X_minute.count_occurances(0.0,0.000000);
      nat32 total_count = precip_X_minute.get_count();
      intervals = total_count - zero_value_count;
   } else  precip_X_minute.clear();
   return  Storms_common::get_eff_precipitation_mm_by_interval
      (precip_X_minute
      ,eff_precip_mm
      ,event_start_time,intervals);
}
//_get_eff_precipitation_mm_by_interval________________________________________/
Storms_hyeotographic::Storms_hyeotographic
(const CORN::date32  &today_
,const Precipitation &precipitation_                                             //151015
,const float32        mean_peak_rainfall_[]
,Minutes              interval_minutes_
,bool                 generate_intensity_
)
:Storms_generation
   (today_
   ,precipitation_
   ,mean_peak_rainfall_
   ,interval_minutes_)
,generate_intensity(generate_intensity_)
{}
//_Storms_hyeotographic:constructor____________________________________________/
float64 Storms_hyeotographic::get_eff_precipitation_mm_by_interval
(CORN::Dynamic_array<float32> &precip_X_minute
,float64             &eff_precip_mm
,CORN::Time          &event_start_time
,nat16               &intervals)                                           const
{
   //000303_         We could record the generated precip in the newly created
   //                database for future reference and to be used by successive
   //                simulation runs.
   // but that would be a function of ClimGen (which would used this class)
   get_precipitation_mm_by_interval
      (generate_intensity,interval_minutes,event_start_time,precip_X_minute);    //151015
   return  Storms_common::get_eff_precipitation_mm_by_interval
      (precip_X_minute,eff_precip_mm,event_start_time,intervals);
}
//_get_eff_precipitation_mm_by_interval________________________________________/
float64 Storms_uniform::get_eff_precipitation_mm_by_interval
(CORN::Dynamic_array<float32> &precip_X_minute
,float64             &eff_precip_mm
,CORN::Time          &event_start_time
,nat16               &intervals)                                           const
{
   intervals = default_event_duration_minutes/interval_minutes;
   float32 precip_in_ts = eff_precip_mm / intervals;
   for (int i = 0 ; i< intervals; i++)
       precip_X_minute.set(i,precip_in_ts);
   return  Storms_common::get_eff_precipitation_mm_by_interval
      (precip_X_minute,eff_precip_mm,event_start_time,intervals);
}
//_get_eff_precipitation_mm_by_interval________________________________________/
Storm_event_search::Storm_event_search
(const date32 &target_date_time_)
:UED::Time_search
   (target_date_time_
   ,target_date_time_
   ,target_date_time_
   ,UED::Time_query::between)
{  low_date_time = target_date_time_;
   Date_time_clad_64 smart_high_date_time(low_date_time);
   smart_high_date_time.inc_day();
   high_date_time = smart_high_date_time.get_datetime64();
// There may already be 30minute precip generated, this new storm event
// might have more or less values old one in the file, so we will replace it.
// Warning, this is not save for real data in the database, we must do something
// different to prevent unexpected erase.
}
//_Storm_event_search_______________________________________________2003-07-14_/
Storm_energy::Storm_energy
(const CORN::Dynamic_array<float32> &precip_mm_in_intervals_
,CORN::Minutes                       interval_duration_)
: precip_mm_in_intervals    (precip_mm_in_intervals_)
, interval_duration  (interval_duration_)
, energy    ()
{}
//_Storm_energy:constructor_________________________________________2016-05-11_/
float64 Storm_energy::get_intensity_mm_per_hour_at(nat16 interval)         const
{  float64 precip_mm_hour
      = precip_mm_in_intervals.get(interval) * 60.0 / interval_duration ;
   return precip_mm_hour;
}
//_get_intensity_mm_per_hour_at_____________________________________2016-05-11_/
float64 Storm_energy::get_energy_MJ_per_ha_at(nat16 interval)              const
{  if (!energy.get_count()) provide_energy();
   return energy.get(interval);
}
//_get_energy_MJ_per_ha_at__________________________________________2016-05-11_/
bool Storm_energy::provide_energy()                                   provision_
{  nat16 intervals =  precip_mm_in_intervals.get_count();
   for (nat16 interval = 0; interval < intervals ; interval++)
   {  float32 intensity // mm per hour
         = get_intensity_mm_per_hour_at(interval);
      float32 e // MJ per ha per mm
         = (intensity > 76.0 /*mm/hr*/)
         ? 0.283
         : 0.29 * (1.0 - 0.72 * exp (-0.05 * intensity));
      float32 E // MJ/ha
         = e * precip_mm_in_intervals.get(interval);
      energy.set(interval,e);
   }
   return energy.get_count() == precip_mm_in_intervals.get_count();
}
//_provide_energy___________________________________________________2016-05-11_/
float64 Storm_energy::get_energy_MJ_per_ha()                               const
{  if (!energy.get_count()) provide_energy();
   return energy.provide_sum();
}
//_get_energy_MJ_per_ha_____________________________________________2016-05-11_/
}}//_namespace CS______________________________________________________________/

