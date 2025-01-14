#ifndef validatorH
#define validatorH
#include "UED/library/codes.h"
#include "UED/library/varcodes.h"
#include "corn/measure/unitconv.h"
#include "solar/irradiance_extraterrestrial.h"
#include <string>
namespace CORN
{  class Quality;
}
class Geolocation; // forward declaration
class Validated_variable;
//______________________________________________________________________________
class Weather_validator
{  // Eventually add daily potential solar radiation so we can
   // validate solar radiation daily
   CORN::Units_convertor convertor;
   CS::Solar_radiation_extraterrestrial *geolocated_solar_irradiance;     //150909
      // allocated only as needed
   CORN::Date_clad_32 date_unused;                                               //150130
public:
   Weather_validator();
   float32 validate_float32
      (CORN::DOY          doy
      ,float32            value
      ,UED::Variable_code variable
      ,CORN::Units_code   units
      ,CORN::Units_code   timestep
      ,CORN::Quality_code file_quality   // This is the general expected quality of data in the file (I.e. is the entire file measured/generated etc..
      ,CORN::Quality_code column_quality // This is the general expected quality of data in the column (I.e. the file may be mostly measured but there might be an estimated column.
      ,CORN::Quality_cowl &identified_quality // return
      ,std::string        &quality_description)                           const;
   float32 validate_string
      (CORN::DOY           doy
      , const std::string &value_string // Value represented as a string
      , UED::Variable_code variable
      , CORN::Units_code   units
      , CORN::Units_code   timestep
      , CORN::Quality_code file_quality   // This is the general expected quality of data in the file (I.e. is the entire file measured/generated etc..
      , CORN::Quality_code column_quality // This is the general expected quality of data in the column (I.e. the file may be mostly measured but there might be an estimated column.
      , CORN::Quality_cowl &identified_quality // return
      , std::string        &quality_description)                          const;
      ///< validates a string representation of the value.
      /// \return the value_string as a float.
      /// if the value_string is some sort of recognizable missing indicator,
      /// the return value will be set to some arbitrary value clearling indicating an invalid value
      /// (As well as setting the quality which should always be checked.
   int8 compare_timestep_units
      (CORN::Units_code timestep1,CORN::Units_code timestep2)             const;
   void set_geolocation(Geolocation *_geolocation);
   inline bool has_geolocation()    const { return geolocated_solar_irradiance;}
private:
   Validated_variable *identify_variable_definition
      (UED::Variable_code variable)                                       const;
};
//______________________________________________________________________________

#endif

