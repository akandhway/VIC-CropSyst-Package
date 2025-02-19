#ifndef WC_vapor_pressure_deficitH
#define WC_vapor_pressure_deficitH
#include "weather/parameter/WP_vapor_pressure_deficit.h"
#include "weather/parameter/WC_vapor_pressure.h"
#include "weather/parameter/WP_vapor_pressure_act.h"
#include "weather/parameter/WP_dew_point_temperature.h"
#include "common/CS_parameter_float64.h"
//______________________________________________________________________________
namespace CORN                                                                   //170808
{  class Data_record;
}
//______________________________________________________________________________
class Vapor_pressure_deficit_max_calculator
: public extends_ Vapor_pressure_deficit_max
, public extends_ Vapor_pressure_parameter
{
 public:
   //___________________________________________________________________________
   class Parameters
   {public:
      float32 aridity_factor  ;                                                  //000715
      float32 slope           ;                                                  //081025_000715
      float32 intercept       ;                                                  //081025_000715
      /*provided_ */CS::Parameter_float64 VPDmax_adj;  // (0.8 � 1.3)                //170808
      /*provided_ */CS::Parameter_float64 VPDavg_seasonal_fullday_observed; // kPa   //170808
      /*provided_ */CS::Parameter_float64 RHavg_seasonal_observed;          // %     //170808
         // Default values of 0 indicated not provided, not specified
    public:
      Parameters();
      void invalidate();
      void copy(const Parameters &copy_from);
      CORN::Quality_code get_curve_quality()                              const; //050610
      CORN::Quality_code get_aridity_factor_quality()                     const; //050609
      virtual bool setup_structure                                               //170808
         (CORN::Data_record &data_rec,bool for_write)             modification_;
      virtual bool get_end()                                      modification_; //170811
   };
   //_Parameters____________________________________________________1999-12-14_/
   const enum Season_S_W_SF &season;                                             //101208 made const
   contribute_ const Parameters *generation_parameters;                          //170803_091214
 public:
   inline Vapor_pressure_deficit_max_calculator
      (const Season_S_W_SF &_season
      ,Dew_point_temperature_maximum   *_dew_point_temperature_max // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
      ,Relative_humidity               *_relative_humidity_min     // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
      ,Air_temperature_maximum         *_air_temperature_max       // May be 0 if unknown
      ,Air_temperature_minimum         *_air_temperature_min)      // May be 0 if unknown
      : Vapor_pressure_deficit_max()                                             //140220
      , Vapor_pressure_parameter
         (_dew_point_temperature_max
         ,_relative_humidity_min
         ,_air_temperature_max
         ,_air_temperature_min)
      , season(_season)
      , generation_parameters(0)                                                 //091214
      {}
      // Use this constructor when there is a possibility of any of these parameters to be available
   inline Vapor_pressure_deficit_max_calculator
      ( const Season_S_W_SF &_season
      , Relative_humidity       &_relative_humidity_min
         // May be 0 if unknown
         // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
      , Air_temperature_maximum         &_air_temperature_max )        // May be 0 if unknown
      : Vapor_pressure_deficit_max()                                             //140220
      , Vapor_pressure_parameter
         (0
         ,&_relative_humidity_min
         ,&_air_temperature_max
         ,0 /*_air_temperature_min */)                                           //070212
      , season(_season)
      , generation_parameters(0)                                                 //091214
      {}
      // Use this constructor when relative humidity and air temperature are known.
   inline Vapor_pressure_deficit_max_calculator
      ( const Season_S_W_SF &_season
      , Air_temperature_maximum  &_air_temperature_max   // May be 0 if unknown
      , Air_temperature_minimum  &_air_temperature_min)  // May be 0 if unknown
      : Vapor_pressure_deficit_max()                                             //140220
      , Vapor_pressure_parameter // Vapor_pressure_deficit_calculator //140220 Vapor_pressure_deficit_parameter
         (0 // dewpoint not available
         ,0 // RH not available
         ,&_air_temperature_max
         ,&_air_temperature_min)
      , season(_season)
      , generation_parameters(0)
      {}
      // Use this constructor when only air temperature is known.
   inline Vapor_pressure_deficit_max_calculator(const Season_S_W_SF &_season)    //101208
      : Vapor_pressure_deficit_max()                                             //140220
      , Vapor_pressure_parameter(0,0,0,0)
      , season(_season)
      , generation_parameters(0)
      {}
   void know_generation_parameters
      (const Parameters &_generation_parameters) contribution_;                  //091214
   virtual const CORN::Quality &update()                         rectification_; //080908
   virtual float64 saturation_at_max_dew_point(bool over_water_only)      const;
public: // Normally the following functions will not be call by user of this class
   float64 calc_from_relative_humidity_min        (CORN::Quality &quality)const;
   float64 calc_from_dew_point_temperature_max    (CORN::Quality &quality)const;
   float64 est_from_temperature                   (CORN::Quality &quality)const; //kPa
   float64 calc_rel_humid                                                        //970710
      (float64 max_or_min_sat_vapor_press
      ,float64 min_or_max_relative_humidity)                              const;
};
//_Vapor_pressure_deficit_max_calculator____________________________2005-06-02_/
class Vapor_pressure_deficit_fullday_calculator
: public extends_ Vapor_pressure_deficit_fullday
, public extends_ Vapor_pressure_parameter
{
protected:
   Vapor_pressure_deficit_max_calculator      *vapor_pressure_deficit_max;
   Vapor_pressure_actual           *vapor_pressure_act;
public:
   inline Vapor_pressure_deficit_fullday_calculator
      (Vapor_pressure_actual           *_vapor_pressure_act
      ,Vapor_pressure_deficit_max_calculator      *_vapor_pressure_deficit_max      //050701
      ,Dew_point_temperature_maximum   *_dew_point_temperature_max        // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
      ,Relative_humidity       *_relative_humidity_min            // May be 0 if unknown  // used by Vapor_pressure_deficit_parameter and Vapor_pressure_actual_parameter
      ,Air_temperature_maximum         *_air_temperature_max              // May be 0 if unknown
      ,Air_temperature_minimum         *_air_temperature_min)             // May be 0 if unknown
      : Vapor_pressure_deficit_fullday()                                            //140220
      , Vapor_pressure_parameter
         (_dew_point_temperature_max
         ,_relative_humidity_min
         ,_air_temperature_max
         ,_air_temperature_min)
      ,vapor_pressure_deficit_max(_vapor_pressure_deficit_max)                      //050801
      , vapor_pressure_act(_vapor_pressure_act)
      {}
   virtual const CORN::Quality & update()                        rectification_; //080908
   float64 calculate_from_temperature_saturated_vapor_pressure
      (CORN::Quality &identified_quality)                                 const; //011008
   float64 calculate_from_max_vapor_pressure_deficit
      (CORN::Quality &identified_quality)                                 const; //011008
};
//_Vapor_pressure_deficit_fullday_calculator________________________2005-06-02_/
class Vapor_pressure_deficit_daytime_calculator
: public extends_ Vapor_pressure_deficit_daytime
{
      //I think intensive
protected:
   Vapor_pressure_deficit_max      *vapor_pressure_deficit_max;
   Vapor_pressure_deficit_fullday  *vapor_pressure_deficit_fullday;              //081111
public:
   inline Vapor_pressure_deficit_daytime_calculator
      (Vapor_pressure_deficit_max      *_vapor_pressure_deficit_max
      ,Vapor_pressure_deficit_fullday  *_vapor_pressure_deficit_fullday)         //081111
      : Vapor_pressure_deficit_daytime()                                         //140220
      , vapor_pressure_deficit_max(_vapor_pressure_deficit_max)
      , vapor_pressure_deficit_fullday(_vapor_pressure_deficit_fullday)          //081111
      {}
   virtual const CORN::Quality & update()                        rectification_; //080908
   virtual float64 estimate_from_VPD_max(CORN::Quality &quality)          const; //011008
};
//_Vapor_pressure_deficit_daytime_calculator________________________2005-06-02_/
#endif

