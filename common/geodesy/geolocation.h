#ifndef geolocationH
#define geolocationH
// Geolocation is not currently a Data_record, because it is used both
// By location parameters and UED::Location_data_record
#include "common/geodesy/geocoordinate.h"

#include "corn/math/compare.hpp"
#define   LABEL_wind_measurement_height   "wind_measurement_height"
#define   LABEL_state_code                "state_code"
#define   LABEL_station_name              "station_name"

#define LABEL_country "country"
#define LABEL_state "state"
#define LABEL_county "county"
//______________________________________________________________________________
class Tgeolocation_form;   // Fiend of geolocation form                          //100108
namespace UED {class Geolocation_record; }                                       //100108
//______________________________________________________________________________
class Geolocation
: public extends_ Geocoordinate
{
 public:
   nat32   available_data;  // Specifies what following data is available (currently used only by UED)
   // Available data masks:
#define        avail_geocoordinates     1
#define        avail_geopolitical_codes 2
#define        avail_geopolitical_names 4
#define        avail_station_number     8
#define        avail_station_name      16
#define        avail_station_ID_code   32
#define        avail_comment           64
#define        avail_screening_height 128

/*
 protected: friend class Tgeolocation_form; friend class UED::Geolocation_record;
            friend class UED_database_interpolate_data_record;
*/
   float32     screening_height_32;    // m   The wind measurement   // This only applies to weather station geolocation probably should put this in a separate subclass   //990218
               // Note that ICASA weather files have a separate wind measurement height and temperature measurement height
   CORN::Valid_float32 v_screening_height;                                             //990118
   nat16      country_code_ISO3166;     // International standard country code ISO-3166
                                  // (may be 0 indicating not avail)
   nat16      state_code;       // USA FIPS state number (or province  other contries may have other enumerations )
                                  // (may be 0 indicating not avail)
   #if (CS_VERSION >=6 )
   nat32
   #else
   nat16
   #endif
      county_code;        // County/region number (Municipio in Spain).
                                  // (may be 0 indicating not avail)
                                  //  For the USA: 5 digit FIPS6-4 state and county code

         // County_code should be 32bit but UED5 used 16bit

   std::string country_name;    // (May be empty if country code specified.)
   std::string state_name;      // or Province (May be empty if state (province) code specified. )
   std::string county_name;     // (Municipio in Spain) (may be empty if county code specified.)

   nat32 ID;   // (was station_number) Application specific For USA, can use NCDC station code.

   mutable std::string station_ID_code;  // Some stations (I.e. in Canada are alpha numeric)  mutable because will assign station ID the station number if there is no station ID value specified
   std::string station_name;     // location or station name.
   std::string comment;          // Any additional descriptive text            //090922
 public: // constructor
   Geolocation();
   Geolocation(const Geolocation &copy_source);                                  //050307
   Geolocation(float32 _latitude_dec_deg                                         //130217
              ,float32 _longitude_dec_deg
               ,float32 _elevation);
   Geolocation(const Geocoordinate &copy_source);                                //151115
   virtual void copy_from_geolocation(const Geolocation &copy_source);           //120325_110503
   virtual void copy_from_geocoordinate(const Geocoordinate &copy_source);       //140531
   virtual bool compares_exactly_to_geolocation(const Geolocation &other) affirmation_;//120325_100516
   //170423 virtual const char *label_cstr(char *buffer)                  const;
   virtual const char *label_string(std::string &buffer)                  const; //170423
 public: // Accessors
   virtual float32 set_latitude_dec_deg_32  (float32 _latitude);
   virtual float32 set_longitude_dec_deg_32 (float32 _longitude);
   // updates available data

   bool set_from_ISO_6709(const std::string &from_string)         modification_; //160125
   float64  set_screening_height     (float32 _screening_height);                //060729
   inline float64  get_screening_height()                                  const { return screening_height_32;} //990118
   inline const float32 &ref_screening_height()                            const { return screening_height_32;} //150427
   float64 set_screening_height_cstr   (const char * _screening_height);         //060729

   inline bool has_geocoordinates()                                        const { return (available_data & avail_geocoordinates)     > 0; }
   inline bool has_screening_height()                                      const { return (available_data & avail_screening_height)   > 0; }   //100519
   inline bool has_geopolitical_codes()const { return (available_data & avail_geopolitical_codes) > 0; }
   inline bool has_geopolitical_names()const { return (available_data & avail_geopolitical_names) > 0; }
   inline bool has_station_number()    const { return (available_data & avail_station_number)     > 0; } //031217
   inline bool has_station_ID_code()   const { return (available_data & avail_station_ID_code)    > 0; } //031217
   inline bool has_station_name()      const { return (available_data & avail_station_name)       > 0; }
   inline bool has_comment()           const { return (available_data & avail_comment)            > 0; }  //100105
   // Get
   inline float32 get_latitude_dec_deg_32()    const { return latitude_dec_deg;}
   inline float32 get_longitude_dec_deg_32()   const { return longitude_dec_deg;}
//moved to geocoordinate   inline float32 get_elevation()              const { return elevation;}
   inline nat16   get_country_code_ISO3166()   const { return country_code_ISO3166;}
   inline nat16   get_state_code()             const { return state_code; }
   inline nat32   get_county_code()            const { return county_code; }
   inline nat32   get_station_number()         const { return ID; }              //031217
   inline nat32   get_ID()                     const { return ID; }              //120607
   std::string    get_ID_string()              const;                            //131013

   const char *   get_station_ID_code()        const;                            //031217
   inline const std::string &get_country_name() const { return country_name;}
   inline const std::string &get_state_name()   const { return state_name; }
   inline const std::string &get_county_name()  const { return county_name; }
   inline const std::string &get_station_name() const { return station_name; }
   inline const std::string &get_comment()      const { return comment; }     //100105
   // The following set_xxx() methods override geolocation making available data
   virtual inline float32 set_elevation_meter      (float32 _elevation_m)        //100117
      {  if (!CORN::is_approximately<float32>(_elevation_m,0.0,0.000001)) available_data |= avail_geocoordinates;
         return Geocoordinate::set_elevation_meter(_elevation_m);
      }
   inline nat16  set_country_code_ISO3166(nat16 _country_code)      assignation_
      {  country_code_ISO3166 =_country_code;
         if (_country_code) available_data |= avail_geopolitical_codes ;
         return country_code_ISO3166;
      } // Use ISO 3166 codes
   inline nat16  set_state_code(nat16 _state_code)                  assignation_
      {  state_code =_state_code;
         if (_state_code) available_data |= avail_geopolitical_codes ;
         return state_code;
      }
      // For the US, use FIPS code
   inline nat32  set_county_code(nat32 _county_code)                assignation_ //140531
      {  county_code =_county_code;
         if (_county_code) available_data |= avail_geopolitical_codes ;
         return county_code;
      }
   nat32   set_station_number       (nat32  station_num_);                       //031217
   nat32   set_ID                   (nat32  ID_);                                //031217
   const char *set_station_ID_code  (const char *station_code_)    assignation_; //031217
   std::string &set_country_name(const std::string &country_name_) assignation_;
   std::string &set_state_name  (const std::string &state_name_)   assignation_;
   std::string &set_county_name (const std::string &county_name_)  assignation_;
   std::string &set_station_name(const std::string &station_name_) assignation_;
   std::string &set_station_ID_code(const std::string &station_code_)assignation_;//031217
   std::string &set_comment     (const std::string &comment_)      assignation_;//100105
/* 190923  DONT DELETE need to reimplement as Geolocation_record
 public: // Data record and Data source setup
   virtual bool setup_structure(CORN::Data_record &data_rec,bool for_write);  //080326

*/

};
//_Geolocation______________________________________________________1990-01-18_/
// NYI implement Geolocation_record

#endif

