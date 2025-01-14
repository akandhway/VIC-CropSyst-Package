#ifndef CS_land_unit_meteorologicalH
#define CS_land_unit_meteorologicalH
#include "corn/primitive.h"
#include "corn/OS/directory_entry_name.h"
#include "common/weather/parameter/WP_air_temperature.h"
#include "common/weather/parameter/WP_wind_speed.h"
#include "common/weather/weather_provider.h"
#include "common/weather/seasons.h"
#include "common/weather/sun.h"
#include "CS_suite/simulation/CS_simulation_element.h"
#include "common/biometeorology/parameter/CO2_atmospheric_conc.h"
#include "common/solar/irradiance_extraterrestrial.h"
#include "common/weather/parameter/WC_ET_reference.h"
#include "CS_suite/observation/CS_observation.h"
#include "CS_suite/observation/CS_examination.h"

//______________________________________________________________________________
class Geocoordinate;                                                             //151115

class Weather;
class Weather_provider;                                                          //151022
class Solar_radiation;                                                           //151022
class Snow_pack;                                                                 //151022
class Precipitation;                                                             //151022
class Sky;
class Sun;
class Sun_days;                                                                  //151130
class Weather_spanning_interface;                                                //151130
class Atmospheric_CO2_change_element;                                            //151201
//______________________________________________________________________________
namespace CS {
   class Weather_subdaily;                                                       //151212
   class Weather_hours;                                                          //151022
   namespace meteorology {
      class Storms;                                                              //140214
   } // namespace meterology
} // namespace CS
//______________________________________________________________________________
namespace CORN
{  class Date;
}
//______________________________________________________________________________
namespace CS
{
class Land_unit_meteorological
: public extends_ CS::Simulation_element_abstract                                //151026
// Actually this should be renamed Environment_meteorological
{
   // This has the weather (including storms generation)
   // and soil parameters (but not soil state).
   const Geocoordinate &geocoordinate;
 public:
   provided_ Sun *sun; provided_ bool sun_owned;                                 //151027_150921
   provided_ Sky *sky; provided_ bool sky_owned;                                 //151027
   provided_ Sun_days  *sun_days;   provided_ bool sun_days_owned;               //160811_151201
   provided_ Sun_hours *sun_hours; provided_ bool sun_hours_owned;               //160811_151201
   Seasons season;
 protected:
   cognate_ Solar_radiation_extraterrestrial_hourly *solar_radiation_extraterrestrial; //151130
      contribute_ bool solar_radiation_extraterrestrial_owned;
      // For CropSyst proper, OFoot  this will own ET solar radiation
      // MicroBasin and other regional simulations simultaneously simulating
      // multiple cells/polygons it will be owned by
      // by the regional simulation because these values dont change much
      // for the region.
      // currently points to weather which is derived from Sun
      // but will eventually be a member
   cognate_ Weather_provider *weather_provider; contribute_ bool weather_provider_owned;
      // Although ownable this is not reponsible for populating elements with
      // current values.

 #if (CS_VERSION>=5)
 public:
   provided_ ET_Reference_calculator *ET_ref;                                    //160731
 #endif

 public:
   provided_ CS::Weather_subdaily   *subdaily;contribute_ bool subdaily_owned;   //151212
   provided_ CS::Weather_hours      *hourly;  contribute_ bool hourly_owned;     //151212
#ifdef VIC_CROPSYST_VERSION
 public:
   cognate_
#else
 private: // could be protected
#endif
   Atmospheric_CO2_change_element *atmospheric_CO2_change;                       //171207
      // optional NULL when no CO2 change                                        //090316
      // Not currently owned (now given to simulation elements list)             //171207
 protected:
   cognate_ ::Weather_spanning_interface *weather_spanning;
      // Optional but needed for subdaily weather generation                     //151212
 public: // temporarily public and  mutable_ so V4 CS simulation can provide.
   mutable meteorology::Storms *storms; bool storms_owned;                       //140217
   // These are
 private:
   provided_ Snow_pack           *snow_pack; // owned                            //151023
   float64                        reported_snow_storage;                         //160629
 public:
   Snow_pack_model                snow_pack_model;                               //151023
      // The snow pack only exists on days where there is freezing and
      // precipitation, and remains only as long as it hasn't fully melted.
 public:
   Precipitation                  liberated_water;                               //151023
      // from precip and melted snow
 public:
   virtual float64 get_snow_pack_thickness()                              const; //141215_141207
   virtual float64 get_snow_water_equivelent_depth()                      const; //151026
 public:
   Land_unit_meteorological
      (const CORN::date32  &simdate_raw                                          //170524
      ,const Geocoordinate &geocoordinate
      ,Snow_pack_model      snow_pack_model = SIMPLE_SNOW_PACK);                 //151202
   virtual ~Land_unit_meteorological();                                          //151023
 public: // Simulation element implementations
   virtual bool initialize()                                    initialization_; //151027
   virtual bool start()                                           modification_; //151130
   virtual bool start_day()                                       modification_; //151023
   virtual bool start_day_subdaily()                              modification_; //190515
 public:
   bool take_weather_hourly                                                      //151215
      (CS::Weather_hours *hourly_weather_given)                  appropriation_; //151223_140217
      // This take_weather_hourly is currently only used by MicroBasin
      // CropSyst proper, OFoot and REACCH let this provide as needed.
   bool know_weather_hourly(CS::Weather_hours &hourly_weather_given) cognition_; //151223
      // optional, used by Microbasin
      // If model doesn't provide hourly, this class will provide default.
   bool take_storms(meteorology::Storms *storms_given)           appropriation_; //151215
      // Storms are optional
   bool submit_weather_provider                                                  //151215
      (Weather_provider *weather_provider_submitted)                submission_;
   bool know_weather_provider                                                    //151222
      (Weather_provider *weather_provider_known)                     cognition_;
   bool know_sun(Sun *sun_)                                          cognition_; //160801
   bool know_sky(Sky *sky_)                                          cognition_; //160801
   bool know_solar_radiation_extraterrestrial                                    //151215
      (const Solar_radiation_extraterrestrial_hourly
         &solar_radiation_extraterrestrial_known)                    cognition_;
      // Used by REACCH and MicroBasin
   const Solar_radiation_extraterrestrial_hourly &
      provide_solar_radiation_extraterrestrial()                     provision_;  //160811
   bool submit_solar_radiation_extraterrestrial
      (Solar_radiation_extraterrestrial_hourly                                   //151215
         *solar_radiation_extraterrestrial_submitted)               submission_;
      // Used by CropSyst proper, OFoot
   bool know_weather_spanning                                                    //151215
      (Weather_spanning_interface &weather_known)                    cognition_;
   bool know_Atmospheric_CO2                                                    //04012021LML
      (Atmospheric_CO2_change_element *atmospheric_CO2_change_)      cognition_;
   Atmospheric_CO2_change_element *                                              //171207
      instanciate_CO2
      (const CORN::OS::File_name &recalibration_filename
      ,float64 initial_CO2_conc
      ,float64 annual_CO2_change
      , const CORN::Date_const &start_date
      , const CORN::Date_const &stop_date)                      initialization_; //150315
   inline virtual bool has_atmospheric_CO2_change()                       const
      { return atmospheric_CO2_change != NULL; }                                 //090316
   const CS::CO2_atmospheric_concentration      &ref_CO2_current_conc()   const; //151201
   virtual const Sun                            &ref_sun()                const; //150921
   virtual const Sun_days                       &ref_sun_days()           const; //151130
   virtual const Sun_days                       &provide_sun_days()  provision_; //160811
   virtual const Sun_hours                      &provide_sun_hours() provision_; //160811
   virtual const Sky                            &ref_sky()                const; //151022
   inline virtual const Geocoordinate           &get_geocoordinate()      const  { return geocoordinate; } //151118
   virtual const Precipitation                  &ref_precipitation_actual()      const;//151022
   virtual const Precipitation                  &ref_precipitation_liberated()   const;//160414 //170321 was ref_precipitation_effective
   virtual const Vapor_pressure_deficit_max     &ref_vapor_pressure_deficit_max() const;//151023
   virtual const Vapor_pressure_daytime         &ref_vapor_pressure_daytime() const;//151023
   virtual const Vapor_pressure_nighttime       &ref_vapor_pressure_nighttime() const;//151023
   virtual const Air_temperature_maximum        &ref_air_temperature_max() const;//151023
   virtual const Air_temperature_minimum        &ref_air_temperature_min() const;//151023
   virtual const Air_temperature_average        &ref_air_temperature_avg() const;//150527_141208
   virtual const Solar_radiation                &ref_solar_radiation()     const;//151022
   virtual const Dew_point_temperature          &ref_dew_point_temperature_max() const;//151023
   virtual const Dew_point_temperature          &ref_dew_point_temperature_min() const;//151023
   virtual const Dew_point_temperature          &ref_dew_point_temperature_avg() const;//151023
   virtual const Relative_humidity              &ref_relative_humidity_max() const;//151023
   virtual const Relative_humidity              &ref_relative_humidity_min() const;//151023
   //NYN virtual const XXXX                     &ref_vapor_pressure_actual() const;//151023
   virtual const Vapor_pressure_deficit_fullday &ref_vapor_pressure_deficit_fullday_avg() const;//151023
   virtual const Vapor_pressure_deficit_daytime &ref_vapor_pressure_deficit_daytime() const;//151023
   virtual const Wind_speed                     &ref_wind_speed()         const; //150527_150427
   virtual const Aerodynamics                   &ref_aerodynamics ()      const; //151026

   virtual const ET_Reference                   &ref_ET_reference()       const; //160731
   #if (CS_VERSION==5)
   virtual const ET_Reference                   &provide_ET_reference()   const; //160731
   #endif

   virtual bool                                  is_winter_time()  affirmation_; //151022
   inline const Weather_provider                &ref_weather_provider()   const  //151023
      { return *weather_provider; }
   inline Weather_provider                      &mod_weather_provider()   const  //160804
      { return *weather_provider; }
   // subdaily and hourly can only be provided if weather_spanning is known/given
   virtual const CS::Weather_hours *provide_hourly()                 provision_; //151022
   virtual const CS::Weather_subdaily  *provide_subdaily()           provision_; //151212
   virtual const Snow_pack    *get_snow_pack()                            const; //151022
   const Weather_spanning_interface &ref_weather_spanning()               const; //151130
   virtual const CS::Solar_radiation_extraterrestrial
      &ref_solar_radiation_extraterrestrial()                             const; //151201
   virtual const meteorology::Storms  *ref_storms()                       const; //151215
      // storms are optional
   inline virtual const Seasons &ref_seasons()           const { return season;} //190705
 public: // provisions
   const Sun &provide_sun()                                          provision_; //160801
   const Sky &provide_sky()                                          provision_; //160801
 public:
   RENDER_INSPECTORS_DECLARATION;                                                //160626
   inline const char *get_ontology()              const { return "meteorology";} //160626
};
//_class Land_unit_meteorological___________________________________2013-06-20_/
}//_namespace_CS_______________________________________________________________/
#endif

