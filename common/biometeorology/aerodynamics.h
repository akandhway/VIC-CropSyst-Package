#ifndef aerodynamicsH
#define aerodynamicsH
#include "corn/chronometry/time_types.hpp"
#include "common/weather/parameter/WP_wind_speed.h"
//______________________________________________________________________________

// This class is using the weather provider idiom.
/*
#define SECONDS_PER_METER  1
#define DAYS_PER_METER     86400
*/
#define SECONDS_PER_SECOND 1
#define SECONDS_PER_DAY    86400


class Aerodynamics
{
 private: // referenced state
   const Wind_speed &wind_speed;                                                 //150215
 private: // constant parameters
   float64  wind_measurement_height;       // m  Usually 2 meters above surface
   float64  temperature_measurement_height;// m  Usually 2 meters above surface
 public:  //Temporaly set it public 150608LML
   mutable CS::Parameter_float64 resistance_of_plants_reference_s_m;                         //150531
   mutable CS::Parameter_float64 resistance_of_snow_s_m; // ? not sure if intensive
      // resistance is s/m
 private:
   float64 calc_resistance_general_form
      (float64 wind_speed // any units
      ,float64 momentum_roughness_parameter_num
      ,float64 momentum_roughness_parameter_denom
      ,float64 heat_and_vapor_roughness_parameter_num
      ,float64 heat_and_vapor_roughness_parameter_denom
      ,float64 zero_plane_displacement_height = 0.0    // m usualy 0 meters
      ,float64 von_Karmon_constant = 0.41)                                const; //011009
   float64 calc_resistance_of_snow  // s/m
      (float64 wind_speed_m_s
      ,float64 momentum_roughness_parameter        = 0.001  // m
      ,float64 heat_and_vapor_roughness_parameter  = 0.0002 // m
      ,float64 von_Karmon_constant                 = 0.41);
 public:
   float64 calc_resistance_of_plants_d_m
      (float64 wind_speed_m_d // meters/day
      ,float64 plant_height_m = 0.12)                                     const; //011009
      // meters default plant height is reference crop
   float64 calc_resistance_of_plants_s_m
      (float64 wind_speed_m_s // meters/sec
      ,float64 plant_height_m= 0.12 /* meters */)                         const; //150531
   // Should be consistent with units

 public:
   virtual bool update();                                                        //150215
   inline virtual void invalidate(bool absolutely)                               //091216
      {  //resistance_of_plants_reference_d_m.invalidate(absolutely);              //150531
         resistance_of_plants_reference_s_m.invalidate(absolutely);              //150531
         resistance_of_snow_s_m.invalidate(absolutely); }
 public:  // accessors
   inline float64 get_resistance_of_plants_reference(CORN::Seconds time_step = SECONDS_PER_SECOND) const
      { return resistance_of_plants_reference_s_m.in_prescribed_units() / (float64)time_step; }
   inline float64 get_resistance_of_snow(CORN::Seconds time_step = SECONDS_PER_SECOND) const
      { return resistance_of_snow_s_m.in_prescribed_units() / (float64)time_step; }  //150122
    inline float64 set_resistance_of_snow_s_m(float64 aero_resist_s_m)
      { return resistance_of_snow_s_m.update_amount_in_preset_units(aero_resist_s_m); } //150123

// The following are not implemented because there could be a.r. for different kinds of plants
//NYI   inline float64 get_aerodynamic_resistance_of_plants(CORN_seconds time_step = DAYS_PER_METER) const
//NYI   { return aerodynamic_resistance_of_plants_d_m * 86400.0 / time_step; }  // WARNING need to check if this works for SECONDS_PER_METER
   // timestep/meter or def: seconds/meter

//NYI    inline float64 set_aerodynamic_resistance_of_plants_d_m(float64 aero_resist_d_m) { return aerodynamic_resistance_of_plants_d_m = aero_resist_d_m;}
 public: // constructors
    Aerodynamics
       (const Wind_speed &_wind_speed                                            //150215
       ,float64  _wind_measurement_height        = 10.0                          //201201LML hard coded : changed from 2.0
       ,float64  _temperature_measurement_height = 2.0);
    float64 calc_normalized_wind_speed(float64 wind_speed_m_ts)           const; //011008
    // This will work for any time step
};
//_Aerodynamics________________________________________________________________/
#endif
