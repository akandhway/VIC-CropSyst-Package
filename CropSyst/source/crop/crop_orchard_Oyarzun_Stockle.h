#ifndef crop_orchard_Oyarzun_StockleH
#define crop_orchard_Oyarzun_StockleH

#include "crop/crop_orchard_common.h"
#include "corn/chronometry/time_types.hpp"
#include "corn/chronometry/date_types.hpp"
class Sun_hours;
class Sun;
namespace CS
{  class Weather_hours;
}
namespace CropSyst {
//______________________________________________________________________________
class Interception_architectural_Oyarzun_Stockle
: public Orchard_common::Interception_architectural
{
 private: // parameters
   float64 row_spacing_along;    // Claudio I think this would be crop layout parameter col_spacing
 private: // daily contributes
   const Sun        &sun_position;
   mutable float64 declination_radians_daily;
   mutable float64 limited_plot_width;       // normally this is simply the row width, but (I think) this is limited when branches from rows growth into each other
   mutable float64 limited_plot_breadth; //    "                       ?col width?   "                                              cols   "
 private: // hourly contributes  these must be calculated in the following sequence!!!
   const Sun_hours            &sun_hours;        // Provided by weather
   const CS::Weather_hours    &weather_hours;
   //190721 CORN::Hour hour;   // current hour
   mutable float64 solar_zenith_angle_radians_hourly;
   mutable float64 solar_azimuth_angle_radians_hourly;
   mutable float64 shadow_correction_factor_hourly;
   mutable float64 A_Daily_Dif_transmit_Rs;
   mutable float64 solar_beam_radiation_hour;
   mutable float64 solar_diffuse_radiation_hour;
 private: // daily attributes
   mutable float64 solid_interception_daily;
   mutable Hour sunrise_hour;                                                    //071115
   mutable Hour sunset_hour;                                                     //071115
 private:
   float64 calc_daily_diffuse_transmittance_of_Rs_for_hour()       calculation_; //071115
   float64 calc_hourly_solid_interception()                        calculation_;
   float64 calc_solar_declination(CORN::DOY doy )                         const;
   float64 calc_shadow_correction_factor()                                const;
   float64 calc_fraction_of_shade
      ( float64 solar_zenith_angle_radians_hourly_param    // Must be a parameter not the member value
      , float64 solar_azimuth_angle_radians_hourly_param)                 const;
   float64 calc_shadow_length
      ( float64 solar_zenith_angle_radians_hourly_param
       , float64 solar_azimuth_angle_radians_hourly_param)                const;
   float64 calc_ghost_shadow_length
      ( float64 solar_zenith_angle_radians_hourly_param
      , float64 solar_azimuth_angle_radians_hourly_param)                 const;
   float64 calc_shadow_length_along
      ( float64 solar_zenith_angle_radians_hourly_param
      , float64 solar_azimuth_angle_radians_hourly_param)                 const;
   float64 calc_ghost_shadow_length_along
      ( float64 solar_zenith_angle_radians_hourly_param
      , float64 solar_azimuth_angle_radians_hourly_param)                 const;
 public: // constructor
   Interception_architectural_Oyarzun_Stockle
      ( CropSyst::Crop_parameters_struct::Layout &layout                         //110218
      , CropSyst::Crop_parameters_struct::Canopy_architecture &architecture_      //200403
      , float64  slope_percent
      , float64  slope_aspect_N0_E90_degrees
      , const Sun_days        &sun_days         // Provided by weather
      , const Sun             &sun
      , const Sun_hours       &sun_hours
      , const CS::Weather_hours   &weather_hours
      , const float64         &fract_cloud_cover                                 //151027
      , const CORN::Date_const &today);                                          //151027
 public:
   virtual float64 update_solid_canopy_interception()            rectification_;
      // 081022 Warning this used to calculate and return a daily value
      // this was not working correctly because the aggregated daily value
      // was too high.  We now use hourly values to determine the
      // potential transpiration.
      // This function now calculates the hourly values stored in
      // hour array solid_canopy_interception[24]
      // and this method now returns 0.0.
   inline virtual float64 get_solid_canopy_interception()                 const
      { return solid_interception_daily; }
};
//_Orchard_interception_Oyarzun_Stockle________________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif

