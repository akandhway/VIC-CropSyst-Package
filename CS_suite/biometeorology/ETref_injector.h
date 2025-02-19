#ifndef ETref_injectorH
#define ETref_injectorH
#include "common/weather/database/CS_weather_daily_UED.h"
#include "common/weather/sun.h"
#include "common/solar/irradiance_extraterrestrial.h"
#include "common/weather/parameter/WC_ET_reference.h"
namespace CS
{
//______________________________________________________________________________
/*unused
interface_ Daily_weather_source
{

//   bool get_daily_weather

};
*/
//______________________________________________________________________________

class ETref_Injector
{
 protected:
   CORN::Date_clad_32 today;
   CORN::Date_clad_32 start_date;
   CORN::Date_clad_32 stop_date;
   Sun               sun;
   Sun_days          sun_days;   // for hourly aggregation mode
   Sun_hours         sun_hours;  // for hourly aggregation mode

   //CS::Solar_irradiance_extraterrestrial *ET_solar_irradiance;

   CS::Solar_radiation_extraterrestrial_hourly *ET_solar_irradiance;

   Weather_provider  provider;
   ET_Reference_calculator ET_ref_calc;
 public:
   ETref_Injector
      (bool                    hourly_aggregation_
      ,const Geocoordinate    &geocoordinate_
      ,CORN::date32            start_date_
      ,CORN::date32            stop_date_);
   virtual ~ETref_Injector();
 protected:
   virtual bool start_day()                                       modification_;
   virtual bool end_day()                                         modification_ = 0;

   virtual bool set_ETref_mm(float64 ETref_mm) = 0;
 public:
   virtual bool process()                                         modification_;
   Weather_provider  &ref_provider() { return provider;}

};
//______________________________________________________________________________

} // namespace CS
#endif
