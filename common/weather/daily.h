#ifndef dailyH
#define dailyH
#ifndef wthrbaseH
#  include "common/weather/wthrbase.h"
#endif
// included because it has the super class.

#include "common/weather/database/CS_weather_daily_UED.h"

/*
   This is a weather class that operates on a daily time step.
*/

namespace CS
{
//______________________________________________________________________________
class Weather_daily
{
   const Geocoordinate              &geocoordinate;
   const CORN::Date_const           &today;
   provided_ Weather_provider       *weather_provider;
   provided_ Sun_days               *sun_days;
   provided_ Sun_hours              *sun_hours;

   // sun_days and sun_hours should actually be cognate
   // because these could be shared regionally

   provided_ CS::Solar_radiation_extraterrestrial_hourly *ET_solar_irrad;
 public:
   Weather_daily
      (const Geocoordinate              &geocoordinate
      ,const CORN::Date_const           &today);
   inline virtual ~Weather_daily() {}                                            //170217
   Sun_days &provide_sun_days()                                      provision_;
   Sun_hours &provide_sun_hours()                                    provision_;
   CS::Solar_radiation_extraterrestrial_hourly &provide_ET_solar_irrad() provision_;
   Weather_provider &provide_weather_provider()                      provision_;
   virtual bool start()                                           modification_;
};
//_Weather_daily____________________________________________________2016-07-29_/
}//_namespace_CS_______________________________________________________________/
class Weather_daily_deprecated
: public Weather_base                                                            //011020
{
 public: // constructor
   inline Weather_daily_deprecated                                               //011020
      (Weather_database    *weather_database_                                    //050307
      ,modifiable_ Weather_provider &provider_                                   //151201
      ,const CORN::Date_const       &today_                                      //150125
      ,const Geocoordinate &geocoordinate_                                       //150629
      ,float64              screening_height                                     //151128
      ,bool                 treat_valid_data_as_real_                            //081109
      ,bool                 read_only_ = false)                                  //000711
      :Weather_base                                                              //011020
      (weather_database_                                                         //050307
      ,provider_                                                                 //151201
      ,today_                                                                    //150125
      ,geocoordinate_                                                            //150629
      ,screening_height                                                          //151128
      ,treat_valid_data_as_real_                                                 //081109
      ,read_only_                                                                //000711
      ,seconds_per_day)
      {}
   inline virtual ~Weather_daily_deprecated() {}                                 //151204
 public: // The following accessors get the weather data with the best quality available
        // These are normally only used by this class when advancing the time step
        // but may also be used to get values for non current dates
 public:  // The following are currently temporary for Weather_interface
   // they should be overridden by derived classes a they are here
   // to allow me to compile ClimGen while in
   // the transition to the new weather classes
   // These should never be reached.

   inline virtual float64 get_curr_act_vapor_pressure()      const { return 9999999.9; }
   inline virtual float64 get_atmospheric_pressure_est_kPa() const { return 9999999.9; } //011008
};
//_Weather_daily_deprecated____________________________________________________/
#endif

