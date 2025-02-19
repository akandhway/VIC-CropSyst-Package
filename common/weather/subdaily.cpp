#include "common/weather/subdaily.h"
#include "UED/library/std_codes.h"
namespace CS
{
//______________________________________________________________________________
Weather_subdaily::Weather_subdaily
(const Weather_spanning_interface &weather_spanning_interface_
,const CORN::Date_const          &today_
,const Air_temperature_maximum   &air_temperature_max_
,const Air_temperature_minimum   &air_temperature_min_
,const Relative_humidity         &relative_humidity_max_
,const Relative_humidity         &relative_humidity_min_
)
: today(today_)
, weather_spanning         (weather_spanning_interface_)
,provided_date_yesterday   (today_.get_date32())                                 //170524
,provided_date_tomorrow    (today_.get_date32())                                 //170524
,air_temperature_max       (air_temperature_max_)
,air_temperature_min       (air_temperature_min_)
,relative_humidity_max     (relative_humidity_max_)
,relative_humidity_min     (relative_humidity_min_)
{}
//_Weather_subdaily:constructor_____________________________________2015-12-12_/
bool Weather_subdaily::update()
{  bool updated = true;

   float64 Tmax_today_Celcius    = air_temperature_max.Celcius();
   float64 Tmin_tomorrow_Celcius = weather_spanning.get_air_temperature_min_tomorrow();
   FOR_EACH_IN(variable_est,Time_step_variable_estimator,time_step_temperature_estimators,each_est)  //060501
   {                                                                             //040116
      Temperature_estimator *temperature_est = (Temperature_estimator *)variable_est->estimator;
      temperature_est->set_max_today_min_tomorrow
         (Tmax_today_Celcius,Tmin_tomorrow_Celcius);                             //151126
   } FOR_EACH_END(each_est);                                                     //040116
   FOR_EACH_IN(variable_est,Time_step_variable_estimator
               ,time_step_relative_humidity_estimators,each_est)                 //060501
   {  Relative_humidity_estimator *rel_humid_est
         = (Relative_humidity_estimator *)variable_est->estimator;               //060501
      weather_spanning.get_parameter_on                                          //151126
         (RHmax_tomorrow
         ,UED::STD_VC_max_relative_humidity.get()
         ,provided_date_tomorrow);
      float64 RHmax_tomorrow_pcnt = RHmax_tomorrow.percent();                    //150203_110910
      float64 RHmin_today_pcnt = relative_humidity_min.percent();                //150203_081106
      rel_humid_est->set_min_today_max_tomorrow
         (RHmin_today_pcnt, RHmax_tomorrow_pcnt);                                //060511
   } FOR_EACH_END(each_est);                                                     //060501
   return updated;
}
//______________________________________________________________________________
bool Weather_subdaily::start_day()                                               //091216
{
   provided_date_yesterday.set(today); provided_date_yesterday.dec_day();
   provided_date_tomorrow .set(today); provided_date_tomorrow.inc_day();
   weather_spanning.get_parameter_on                                             //151201
      (Tmax_yesterday,UED::STD_VC_max_temperature.get(),provided_date_yesterday);//151201
   weather_spanning.get_parameter_on
      (Tmin_tomorrow ,UED::STD_VC_min_temperature.get(),provided_date_tomorrow); //151126
   return true;
}
//______________________________________________________________________________
Minutes Weather_subdaily::get_minutes_in(Units_code time_step_units) const
{  Minutes minutes_in_timestep=60;
   switch (time_step_units)
   {  case UT_minute  : minutes_in_timestep = 1;   break;
      case UT_hour    : minutes_in_timestep = 60;  break;
      default :
      {  CORN::Units_clad smart_time_step(time_step_units);
         CORN::Units_component time_step_component(smart_time_step.get_primary());
         switch (time_step_component.get_measure_index())
         {
            case UM_minutes: minutes_in_timestep = time_step_component.get_submeasure_index(); break;
            case UM_hours  : minutes_in_timestep = 60.0 * time_step_component.get_submeasure_index(); break;
         }
      } break;
   }
   return minutes_in_timestep;
}
//_update______________________________________________________________________/
float64 Weather_subdaily::Time_step_variable_estimator::get_at(nat32 interval) const
{  return curr_values.get(interval); }
//_Time_step_variable_estimator::get_at________________________________________/
void  Weather_subdaily::get_temperature_by_interval
(Units_code time_step_units,Dynamic_array<float32>&temperature_X_minute)
{  Time_step_variable_estimator *timestep_estimator =
      provide_temperature_time_step_estimator(time_step_units);
   timestep_estimator->estimator->get_by_interval(temperature_X_minute);
}
//_get_temperature_by_interval______________________________________2004-11-15_/
float64 Weather_subdaily::get_est_temperature_at
(Units_code time_step_units,nat16 time_index)                              const
{  Time_step_variable_estimator *timestep_estimator
      = provide_temperature_time_step_estimator(time_step_units);
   float64 temperature = timestep_estimator->get_at(time_index);
   return temperature;
}
//_get_est_temperature_at___________________________________________2004-11-15_/
float64 Weather_subdaily::get_est_temperature_at_hour(Hour hour)           const
{  return get_est_temperature_at(UT_hour,hour);
}
//_get_est_temperature_at_hour_____________________________________2005-008-16_/
Weather_subdaily::Time_step_variable_estimator::Time_step_variable_estimator
(Units_code _time_step_units,Min_max_to_timestep_estimator *_estimator)
: Item()
, estimator(_estimator)                                                          //060501
, curr_values(_estimator->get_intervals(),24,0.0)                                //060501
, time_step_units(_time_step_units)
{}
//_Time_step_variable_estimator:constructor_________________________2004-11-15_/
Weather_subdaily::Time_step_variable_estimator::~Time_step_variable_estimator()
{  delete estimator; }
//_Time_step_variable_estimator:destructor__________________________2004-11-15_/
Weather_subdaily::Time_step_variable_estimator * Weather_subdaily
::provide_temperature_time_step_estimator(Units_code time_step_units) provision_
{  Time_step_variable_estimator *time_step_estimator
   = (Time_step_variable_estimator *)time_step_temperature_estimators.find((uint32)time_step_units);
   if (!time_step_estimator)
   {  // There has not be a request for estimation at this time step yet so create one.
      Temperature_estimator * estimator = new Temperature_estimator(get_minutes_in(time_step_units));
      weather_spanning.get_parameter_on
         (Tmin_tomorrow,UED::STD_VC_min_temperature.get(),provided_date_tomorrow);  //151126
      float64 tmin_tomorrow_Celcius = Tmin_tomorrow.Celcius();                      //151126
      weather_spanning.get_parameter_on
         (Tmax_yesterday,UED::STD_VC_max_temperature.get(),provided_date_yesterday);//151126
      float64 tmax_yesterday_Celcius = Tmax_yesterday.Celcius();                    //151126
      estimator->reset
            (air_temperature_max.Celcius()   ,tmax_yesterday_Celcius
            ,air_temperature_min.Celcius()   ,tmin_tomorrow_Celcius);
      time_step_estimator = new Time_step_variable_estimator(time_step_units,estimator);
      time_step_temperature_estimators.append(time_step_estimator);
   }
   return time_step_estimator;
}
//_provide_temperature_time_step_estimator__________________________2004-11-15_/
Weather_subdaily::Time_step_variable_estimator *Weather_subdaily::
provide_relative_humidity_time_step_estimator(Units_code time_step_units) provision_
{  Time_step_variable_estimator *time_step_estimator
   = (Time_step_variable_estimator *)time_step_relative_humidity_estimators.find((uint32)time_step_units);
   if (!time_step_estimator)
   {  // There has not be a request for estimation at this time step yet so create one.
      Relative_humidity_estimator * estimator = new Relative_humidity_estimator(get_minutes_in(time_step_units));
      float64 RHmax = relative_humidity_max.percent();                           //150126_081106
      float64 RHmin = relative_humidity_min.percent();                           //150126_081106
      estimator->reset
            (RHmax  // max today
            ,RHmax  // actually normally we want max_tomorrow
            ,RHmin   // min today
            ,RHmin); // actually normally we want min_yesterday
      time_step_estimator = new Time_step_variable_estimator(time_step_units,estimator);  //060511
      time_step_relative_humidity_estimators.append(time_step_estimator);
   }
   return time_step_estimator;
}
//_get_relative_humidity_time_step_estimator________________________2004-11-15_/
}//_namespace CS_______________________________________________________________/

