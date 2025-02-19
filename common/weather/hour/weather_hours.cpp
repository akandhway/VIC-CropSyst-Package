#include "common/weather/hour/weather_hours.h"
#include "common/weather/subdaily.h"
#include "common/weather/any_timestep/temperature_est.h"
#include "common/weather/any_timestep/relhumid_est.h"
//______________________________________________________________________________
namespace CS {
//______________________________________________________________________________
Weather_hours_store::Weather_hours_store()
: Weather_hours()
, temperature_by_hour(24,1,0.0)
, relative_humidity_by_hour(24,1,0.0)
{}
//______________________________________________________________________________
Weather_hours_calculated::Weather_hours_calculated
(const CS::Weather_subdaily   &subdaily_
,const Geocoordinate    &geocoordinate_
,const CS::Solar_radiation_extraterrestrial_hourly &ET_solar_radiation_new_
,const Solar_radiation  &solar_radiation_daily_                                  //140715
,const Sun_hours        &sun_hours_)                                             //160117
: Weather_hours_store                                                         ()
, subdaily                                                           (subdaily_)
, extraterrestrial_solar_radiation                     (ET_solar_radiation_new_) //150527
, solar_radiation_MJ_m2         (solar_radiation_daily_,ET_solar_radiation_new_)
, sun_hours                                                         (sun_hours_) //160117
{}
//______________________________________________________________________________
bool  Weather_hours_calculated::update(DOY doy)
{  bool updated = true;
   CS::Weather_subdaily::Time_step_variable_estimator
      *timestep_temperature_estimator
         = subdaily.provide_temperature_time_step_estimator(UT_hour);
   Temperature_estimator
      *hourly_temp_est = (Temperature_estimator *)timestep_temperature_estimator->estimator;
   if (hourly_temp_est)
       updated &= hourly_temp_est->get_by_interval(temperature_by_hour);
   CS::Weather_subdaily::Time_step_variable_estimator *timestep_relhumid_estimator
      = subdaily.provide_relative_humidity_time_step_estimator(UT_hour);
   Relative_humidity_estimator
      *hourly_rel_humid_est=(Relative_humidity_estimator *)
            timestep_relhumid_estimator->estimator;
   if (hourly_rel_humid_est)
       updated &= hourly_rel_humid_est->get_by_interval(relative_humidity_by_hour);
   updated &= solar_radiation_MJ_m2.update();                                    //140715
   return updated;                                                               //140709
}
//_update______________________________________________________________________/
float64 Weather_hours_store::get_air_temperature(Hour hour)                const
{  return (float64)temperature_by_hour.get(hour);
}
//______________________________________________________________________________
float64 Weather_hours_store::get_relative_humidity(Hour hour)              const
{  return (float64)relative_humidity_by_hour.get(hour);
}
//______________________________________________________________________________
float64 Weather_hours_calculated::get_solar_zenith_angle_rad(Hour hour)    const
{  return sun_hours.get_solar_zenith_angle_rad(subdaily.today.get_DOY(), hour);
}
//______________________________________________________________________________
float64 Weather_hours_calculated::get_solar_radiation_MJ_m2(Hour hour)    const
{  return solar_radiation_MJ_m2.get_solar_irradiation(hour);
}
//______________________________________________________________________________
float64 Weather_hours_calculated::get_solar_direct_radiation_MJ_m2(Hour hour)  const
{  return solar_radiation_MJ_m2.get_direct_solar_radiation(hour);
}
//______________________________________________________________________________
float64 Weather_hours_calculated::get_solar_diffuse_radiation_MJ_m2(Hour hour) const
{  return solar_radiation_MJ_m2.get_diffuse_solar_radiation(hour);
}
//______________________________________________________________________________
float64 Weather_hours_calculated::get_insolation_fraction(CORN::Hour hour) const
{  return extraterrestrial_solar_radiation.get_insolation_fraction_today(hour);
}
//_2015-12-23___________________________________________________________________
}//_namespace CS_______________________________________________________________/

