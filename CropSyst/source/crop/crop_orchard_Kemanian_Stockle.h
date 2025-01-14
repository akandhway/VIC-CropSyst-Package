#ifndef crop_orchard_kemanian_StockleH
#define crop_orchard_kemanian_StockleH

#include "crop/crop_orchard_common.h"
//020611 Armen has confirmed that ascpect is
// That I had a note that aspect (degrees) was 0=South 90=East
// I have converted this from North = 0 90=East which is the ArcView convention.

// Note if compiler complains about the following foward declarations.
// Include crop_params.h before including this class's header file

namespace CropSyst {
//______________________________________________________________________________
class Interception_architectural_Kemanian_Stockle
: public Orchard_common::Interception_architectural
{private: // Constant parameters
   float64  latitude_rad;
 private: // References
   const float64 &fract_cloud_cover;                                                   //151027
 private:
   float64 calc_sine_solar_elevation      (float64 sin_solar_dec,Hour hour)               const;
   float64 calc_cosine_solar_azimuth      (float64 sin_solar_dec,float64 sin_solar_elev)  const;
   float64 calc_shadow_angle              (float64 Solar_Elev,float64 Solar_Azimuth)      const;
   float64 calc_shadow_length             (float64 shadow_angle)                          const;
   float64 calc_hourly_incident_angle     (float64 Solar_Elev, float64 Solar_Azimuth )    const;
   float64 calc_corrected_shadow_length   (float64 Shadow_Length,float64 Solar_Elev,float64 Solar_Azimuth) const;
   float64 calc_hourly_direct_radiation_potential(float64 sin_solar_elev)                 const;
   float64 RSRD  /*unused*/               (float64 sin_solar_elev,float64  shadow_angle)  const;
 public:
   void get_hourly_interception
      (CORN::Hour hour
      ,float64    &Hourly_Direct_Row_Int
      ,float64    &Hourly_Direct_Rad_Pot
      ,float64    &Hourly_Diffuse_Row_Int
      ,float64    &Hourly_Diffuse_Rad_Pot
      ,float64    Sine_Solar_Elev)                                        const;
   void get_daily_interception
      (float64 &Daily_Direct_Row_Int
      ,float64 &Daily_Diffuse_Row_Int)                                    const;
 private:
   mutable float64 solid_canopy_interception;                                    //071117
 public: // constructor
   Interception_architectural_Kemanian_Stockle
      ( CropSyst::Crop_parameters_struct::Layout &layout                         //110218
      , CropSyst::Crop_parameters_struct::Canopy_architecture &architecture      //110218
      , float64  slope_percent
      , float64  slope_aspect_N0_E90_degrees
      , const Sun_days &sun_days         // Provided by weather
      , const float64 &fract_cloud_cover                                         //151027
      , const CORN::Date_const &today
      , float64 latitude_rad);
   virtual float64 update_solid_canopy_interception()            rectification_; //151027
   inline virtual float64 get_solid_canopy_interception()                  const
      { return solid_canopy_interception;}
};
//_class_Orchard_interception_Kemanian_Stockle_________________________________/
}//_namespace CropSyst_________________________________________________________/
#endif
//croprows.h

