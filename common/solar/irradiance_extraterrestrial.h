#ifndef irradiance_extraterrestrialH
#define irradiance_extraterrestrialH
//check needed#include "CS_suite/simulation/CS_simulation_element.h"
#ifndef geolocationH
#  include "geodesy/geolocation.h"
#endif
#include "corn/dynamic_array/dynamic_array_T.h"
#ifndef sun_daysH
#  include "common/weather/day/sun_days.h"
#endif
#ifndef sun_hoursH
#  include "common/weather/hour/sun_hours.h"
#endif
#define extraterrestrial_to_max_solar_irradiance_factor_typical 0.75
#define extraterrestrial_to_max_solar_irradiance_factor 0.85

   // extraterrestrial irradiance is also known as:
   // extraterrestrial radiation (as previously called in CropSyst)
   // and top of atmosphere (TOA) irradiance.
   // irradiance refers to solar power (instantaneous energy) falling on unit area per time (I.e. W/m2)
   // irradiation refers to (cummulative) irradiance for a specific interval.

namespace CS
{
//______________________________________________________________________________

//200505 Actually I think this is Solar_radiance_extraterrestrial
// because these classes report integrated value (I had misnamed these)

interface_ Solar_radiation_extraterrestrial
{
 public:
   virtual bool start()                                            modification_ = 0;
 public: // accessors
   virtual float64 get_MJ_per_m2_day(CORN::DOY doy = 0)                    const = 0;
   virtual float64 get_J_per_m2_day (CORN::DOY doy = 0)                    const = 0;
   inline virtual float64 get_MJ_per_m2_hour  (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 0; }
   inline virtual float64 get_J_per_m2_hour   (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 0; }
   inline virtual float64 get_Watt_per_m2     (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 0; } //151214

   virtual float64 get_insolation_fraction(CORN::DOY doy, CORN::Hour hour) const = 0;
   virtual float64 get_insolation_fraction_today(CORN::Hour hour)         const = 0;

};
//_class:Solar_radiation_extraterrestrial___________________________2015-10-22_/
class Solar_radiation_extraterrestrial_irrelevent
: public implements_ Solar_radiation_extraterrestrial
{  // This class is just a place holder for use with Weather_provider
   // when I don't need to calculate ETref or Solar_radiation_extraterrestrial
   virtual bool start()                            modification_ { return true;}
 public: // accessors
   virtual float64 get_MJ_per_m2_day(CORN::DOY doy = 0)   const { UNUSED_arg(doy);return 9999; }
   virtual float64 get_J_per_m2_day (CORN::DOY doy = 0)   const { UNUSED_arg(doy);return 9999; }
   inline virtual float64 get_MJ_per_m2_hour  (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 9999; }
   inline virtual float64 get_J_per_m2_hour   (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 9999; }
   inline virtual float64 get_Watt_per_m2     (CORN::DOY doy = 0, CORN::Hour hour = 0) const { UNUSED_arg(doy);UNUSED_arg(hour); return 9999; }

   inline virtual float64 get_insolation_fraction(CORN::DOY doy, CORN::Hour hour) const { UNUSED_arg(doy);  UNUSED_arg(hour); return 9999; }
   inline virtual float64 get_insolation_fraction_today(CORN::Hour hour)          const { UNUSED_arg(hour); return 9999; }
};
//_Solar_radiation_extraterrestrial_irrelevent______________________2018-04-02_/
class Solar_radiation_extraterrestrial_daily
: public implements_ Solar_radiation_extraterrestrial                           //151022
{
 protected:
   const CORN::Date_const &today;                                                //170524
   const Geocoordinate    &geocoordinate;
 protected:
   float64  irradiance_MJ_m2_day[367];
      // Index 0 is unused
      // At hourly resoluation this array is empty
 public: // structors
   Solar_radiation_extraterrestrial_daily
      (const Geocoordinate &geocoordinate
      ,const CORN::Date_const &today);                                           //170524
 public: // setup
   virtual bool start()                                           modification_;
 public: // accessors
   virtual float64 get_MJ_per_m2_day(CORN::DOY doy = 0)                   const;
   virtual float64 get_J_per_m2_day (CORN::DOY doy = 0)                   const;
 public:
   virtual float64 get_insolation_fraction(CORN::DOY doy, CORN::Hour hour)const;
   virtual float64 get_insolation_fraction_today(CORN::Hour hour)        const;
 private:
   float64 calculate(CORN::DOY doy)                                calculation_;
};
//_Solar_radiation_extraterrestrial_daily___________________________2015-06-25_/
class Solar_radiation_extraterrestrial_hourly
: public extends_ Solar_radiation_extraterrestrial_daily
{
 private:
   const Sun_days    &sun_days;
   const Sun_hours   &sun_hours;
 protected:
   // Index 0 is unused for day but is used by hour
   float64 radiation_J_m2_s  [367][24];
   float64 insolation_fraction[367][24];
      // I think insolation_fraction is extraterrestrial
   float64 radiation_J_m2_day[367];
 public: // structors
   Solar_radiation_extraterrestrial_hourly
      (const Geocoordinate &geocoordinate
      ,const Sun_days      &sun_days
      ,const Sun_hours     &sun_hours
      ,const CORN::Date_const &today);                                           //170524
 public:
   virtual bool start()                                           modification_;
   //150901 Now all days are setup in start()
 public: // accessors
   virtual float64 get_MJ_per_m2_day   (CORN::DOY doy=0)                  const;
   virtual float64 get_J_per_m2_day    (CORN::DOY doy=0)                  const;
   virtual float64 get_MJ_per_m2_hour  (CORN::DOY doy=0,CORN::Hour hour=0)const;
   virtual float64 get_J_per_m2_hour   (CORN::DOY doy=0,CORN::Hour hour=0)const;
   virtual float64 get_Watt_per_m2_hour(CORN::DOY doy=0,CORN::Hour hour=0)const; //151214
   virtual float64 get_insolation_fraction(CORN::DOY doy, CORN::Hour hour)const;
   virtual float64 get_insolation_fraction_today(CORN::Hour hour)         const;
      // water get_extraterrestrial_fract()
 private:
   float64 calculate(CORN::DOY doy,CORN::Hour hour)                calculation_;
   bool start_DOY(CORN::DOY doy)                                  modification_;
};
//_Solar_radiation_extraterrestrial_hourly__________________________2015-06-25_/
}//_namespace CS_______________________________________________________________/
#endif
