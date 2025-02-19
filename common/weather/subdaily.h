#ifndef subdailyH
#define subdailyH
#include "corn/dynamic_array/dynamic_array_T.h"
#include "common/weather/any_timestep/min_max_to_timestep_estimator.h"
#include "corn/container/enumlist.h"
#include "corn/metrology/units_code.hpp"
#include "corn/chronometry/date_32.h"
#include "common/weather/parameter/WC_air_temperature.h"
#include "common/weather/parameter/WC_relative_humidity.h"

#include "common/weather/any_timestep/temperature_est.h"
#include "common/weather/any_timestep/relhumid_est.h"
#include "common/weather/parameter/WC_relative_humidity.h"
#include "common/weather/database/weather_database.h"
#include "common/weather/weather_interface.h"

namespace CS
{
//______________________________________________________________________________
class Weather_subdaily
{
 public: // for Weather_hours
   const CORN::Date_const &today;                                                //150125
 protected:
   contribute_ CORN::Date_clad_32 provided_date_yesterday;
   contribute_ CORN::Date_clad_32 provided_date_tomorrow;

   const Air_temperature_maximum    &air_temperature_max;
   const Air_temperature_minimum    &air_temperature_min;
   const Relative_humidity          &relative_humidity_max;
   const Relative_humidity          &relative_humidity_min;

   const Weather_spanning_interface &weather_spanning;

   contribute_ Air_temperature_minimum Tmin_tomorrow;                            //151126
   contribute_ Air_temperature_minimum Tmax_yesterday;                           //151202
   contribute_ Relative_humidity       RHmax_tomorrow;                           //151126
   //___________________________________________________________________________
 public:
   class Time_step_variable_estimator
   : public CORN::Item
   {
      CORN::Units_code time_step_units;
      Dynamic_array<float32>  curr_values;
      contribute_ Air_temperature_maximum Tmax_yesterday;                        //151204
     public:
      Min_max_to_timestep_estimator *estimator;
    public:
      Time_step_variable_estimator
         (CORN::Units_code time_step_units
         ,Min_max_to_timestep_estimator *estimator);
      virtual ~Time_step_variable_estimator();
      inline virtual bool is_key_nat32(nat32 key)                   affirmation_ //180820
      { return (nat32)time_step_units == key; }
      inline virtual nat32 get_key_nat32() const{return (nat32)time_step_units;} //110315
      float64 get_at(nat32 interval)                                      const; //040116
   };
   //_Time_step_variable_estimator______________________2006-05-01_2004-01-16__/
   mutable Enumeration_list time_step_temperature_estimators;   // May be able to use Enumeration_binary_tree, need to check 040116
   // There may be a temperature estimator for each time step
   // usually there may no estimators or one (I.e. hourly) but
   // the user may request any combination of timesteps.
   mutable Enumeration_list time_step_relative_humidity_estimators;   // May be able to use Enumeration_binary_tree, need to check // 040116
 public:
   Weather_subdaily
      (const Weather_spanning_interface &weather_spanning_interface_
      ,const CORN::Date_const          &today_
      ,const Air_temperature_maximum   &air_temperature_max_
      ,const Air_temperature_minimum   &air_temperature_min_
      ,const Relative_humidity         &relative_humidity_max_
      ,const Relative_humidity         &relative_humidity_min_);
   virtual Time_step_variable_estimator *provide_temperature_time_step_estimator
      (CORN::Units_code time_step)                                   provision_; //060501
   virtual Time_step_variable_estimator *provide_relative_humidity_time_step_estimator
      (CORN::Units_code time_step)                                   provision_; //060501
   // time step units UT_day UT_hour UT_minute, or UT_xxx_minutes, UT_xxx_hours
   virtual bool update();
   virtual bool start_day();                                                     //091216
 protected:
   Minutes get_minutes_in(CORN::Units_code time_step)                     const; //040116
   virtual void get_temperature_by_interval                                      //040115
      (CORN::Units_code time_step,Dynamic_array<float32> &temperature_X_minute); //040115
   virtual float64 get_est_temperature_at
      (CORN::Units_code time_step,nat16 time_index)                       const; //040115
   virtual float64 get_est_temperature_at_hour(Hour hour)                 const; //050817
};
//_Weather_subdaily_________________________________________________2015-12-12_/
}//_namespace CS_______________________________________________________________/
#endif
