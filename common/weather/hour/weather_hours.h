#ifndef weather_hoursH
#define weather_hoursH
#include "corn/const.h"
#include "corn/chronometry/time_types.hpp"
#include "common/weather/hour/solar_irrad_hour.h"
#include "common/weather/subdaily.h"
#include "common/weather/hour/sun_hours.h"
#ifndef dynamic_array_TH
#  include "corn/dynamic_array/dynamic_array_T.h"
#  define Dynamic_float_array Dynamic_array<float32>
#endif
#include "common/solar/irradiance_extraterrestrial.h"

class Weather_day;
namespace CS
{
//______________________________________________________________________________
interface_ Weather_hours
{
   virtual bool update(CORN::DOY doy)                                        =0;
 public: // meterologic accessors
   virtual float64 get_air_temperature(CORN::Hour hour)                 const=0;
   // To get precipitation, use storm event database or generator
   virtual float64 get_relative_humidity(CORN::Hour hour)               const=0;
 public: // Sun accessors
   virtual float64 get_solar_zenith_angle_rad(CORN::Hour hour)          const=0;
 public: // solar rad accessors
   // replace with Solar_radiation_extraterrestrial_hourly::insolation_fraction
   virtual float64 get_solar_radiation_MJ_m2(CORN::Hour hour)           const=0;
   virtual float64 get_solar_direct_radiation_MJ_m2(CORN::Hour hour)    const=0;
   virtual float64 get_solar_diffuse_radiation_MJ_m2(CORN::Hour hour)   const=0;
   virtual float64 get_insolation_fraction(CORN::Hour hour)             const=0;
   virtual const Sun_hours &ref_sun_hours()                             const=0; //151224
   virtual const CORN::Dynamic_array<float32> &ref_temperature_by_hour()const=0; //151224
};
//_Weather_hours____________________________________________________2015-12-23_/
class Weather_hours_store
: public implements_ Weather_hours
{
 public:
   CORN::Dynamic_array<float32>           temperature_by_hour;       // 24 max entries  (0 based index)
   CORN::Dynamic_array<float32>           relative_humidity_by_hour; // 24 max entries  (0 based index)
 public: // Constructor
   Weather_hours_store();
   inline virtual bool update(CORN::DOY doy)                    { return true; }
 public: // meterologic accessors
   virtual float64 get_air_temperature(CORN::Hour hour)                   const;
   // To get precipitation, use storm event database or generator
   virtual float64 get_relative_humidity(CORN::Hour hour)                 const;
   inline virtual const CORN::Dynamic_array<float32> &ref_temperature_by_hour() const
      { return temperature_by_hour; }
};
//_Weather_hours_store______________________________________________2015-12-23_/
class Weather_hours_calculated  // essentially was Weather_hour
: public extends_ Weather_hours_store
{
 public:
   const CS::Weather_subdaily &subdaily;                                         //151212
   const CS::Solar_radiation_extraterrestrial_hourly &extraterrestrial_solar_radiation; //150625
   Solar_radiation_hour       solar_radiation_MJ_m2;
   const Sun_hours            &sun_hours;                                        //160811
 public:
   Weather_hours_calculated
      (const CS::Weather_subdaily &subdaily
      ,const Geocoordinate   &geocoordinate                                      //151116
      ,const CS::Solar_radiation_extraterrestrial_hourly &ET_solar_radiation
      ,const Solar_radiation &solar_radiation_daily                             //140715
      ,const Sun_hours       &sun_hours);                                        //160811_160117
   virtual bool update(CORN::DOY doy);
 public: // Sun accessors
   virtual float64 get_solar_zenith_angle_rad(CORN::Hour hour)            const;
 public: // solar rad accessors
// replace with Solar_radiation_extraterrestrial_hourly::insolation_fraction
   virtual float64 get_solar_radiation_MJ_m2(CORN::Hour hour)             const;
   virtual float64 get_solar_direct_radiation_MJ_m2(CORN::Hour hour)      const;
   virtual float64 get_solar_diffuse_radiation_MJ_m2(CORN::Hour hour)     const;
   virtual float64 get_insolation_fraction(CORN::Hour hour)               const;
   virtual inline const Sun_hours &ref_sun_hours()   const { return sun_hours; } //151224
};
//_Weather_hours_calculated_________________________________________2015-12-23_/
}//_namespace CS_______________________________________________________________/
#endif
