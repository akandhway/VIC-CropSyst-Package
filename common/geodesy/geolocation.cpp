#include "common/geodesy/geolocation.h"
#include <math.h>
#ifndef compareHPP
#  include "corn/math/compare.hpp"
#endif
#include "corn/string/strconv.hpp"
#include "corn/data_source/datarec.h"
#include "corn/measure/measures.h"
#include <stdlib.h>
#include <ctype.h>
// for atof
//______________________________________________________________________________
Geolocation::Geolocation()
: Geocoordinate()
, country_code_ISO3166(0)
, state_code(0)
, county_code(0)
, country_name("")
, state_name("")
, county_name("")
, ID(0)
, station_ID_code("")
, station_name("")
, comment("")                                                                    //100105
, screening_height_32          (DEF_screening_height)                            //990118
,v_screening_height            (screening_height_32             ,UC_meter     ,LABEL_wind_measurement_height   ,1,DEF_screening_height ,   0.0,   3.0,   0.0,  15.0,"m","Screening height")         //990218
, available_data(0) // caller must use set to set these up                       //050601
{}
//_Geolocation:constructor_____________________________________________________/
Geolocation::Geolocation(const Geolocation &copy_source)
: Geocoordinate(copy_source)
, country_code_ISO3166  (copy_source.country_code_ISO3166)
, state_code            (copy_source.state_code)
, county_code           (copy_source.county_code)
, country_name          (copy_source.country_name)
, state_name            (copy_source.state_name)
, county_name           (copy_source.county_name)
, ID                    (copy_source.ID)
, station_ID_code       (copy_source.station_ID_code)
, station_name          (copy_source.station_name)
, comment               (copy_source.comment)                                    //100105
, screening_height_32   (copy_source.screening_height_32)                        //990118
,v_screening_height     (screening_height_32,UC_meter   ,LABEL_wind_measurement_height   ,1,DEF_screening_height ,   0.0,   3.0,   0.0,  15.0,"m","Screening height"/*TU_m,TL_Screening_height*/)         //990218_
, available_data(copy_source.available_data)                                     //050601
{}
//_Geolocation:copy_geolocation_constructor_________________________2005-03-07_/
Geolocation::Geolocation
(float32 _latitude_dec_deg                                                       //130217
,float32 _longitude_dec_deg
,float32 _elevation)
:Geocoordinate(_latitude_dec_deg,_longitude_dec_deg,_elevation)
,v_screening_height            (screening_height_32,UC_meter   ,LABEL_wind_measurement_height   ,1,DEF_screening_height ,   0.0,   3.0,   0.0,  15.0,"m","Screening height"/*TU_m,TL_Screening_height*/)         //990218_
{}
//_Geolocation:constructor__________________________________________2013-02-17_/
Geolocation::Geolocation(const Geocoordinate &copy_source)
: Geocoordinate(copy_source)
,v_screening_height            (screening_height_32,UC_meter   ,LABEL_wind_measurement_height   ,1,DEF_screening_height ,   0.0,   3.0,   0.0,  15.0,"m","Screening height"/*TU_m,TL_Screening_height*/)         //990218_
{  available_data |= avail_geocoordinates;                                       //190516
}
//_Geolocation:copy_geocoordinate_constructor_______________________2015-11-15_/
void Geolocation::copy_from_geolocation(const Geolocation &copy_source)
{  copy_from_geocoordinate(copy_source);                                         //120804_120325
   country_code_ISO3166  =copy_source.country_code_ISO3166;
   state_code            =copy_source.state_code;
   county_code           =copy_source.county_code;
   country_name          =copy_source.country_name;
   state_name            =copy_source.state_name;
   county_name           =copy_source.county_name;
   ID                    =copy_source.ID;
   station_ID_code       =copy_source.station_ID_code;
   station_name          =copy_source.station_name;
   comment               =copy_source.comment;
   screening_height_32   =copy_source.screening_height_32;
   available_data        =copy_source.available_data;
}
//_copy_from_geolocation____________________________________________2011-05-03_/
void Geolocation::copy_from_geocoordinate(const Geocoordinate &copy_source)
{  Geocoordinate::copy_from_geocoordinate(copy_source);
   available_data |= avail_geocoordinates;
}
//_copy_from_geocoordinate__________________________________________2014-05-31_/
bool Geolocation::compares_exactly_to_geolocation(const Geolocation &other) affirmation_
{  return
     ( compares_exactly_to_geocoordinate(other)
     &&(CORN::is_approximately<float32>(screening_height_32,other.screening_height_32,0.000001))
     &&(country_code_ISO3166 == other.country_code_ISO3166)
     &&(state_code       == other.state_code)
     &&(county_code      == other.county_code)
     &&(country_name     == other.country_name)
     &&(state_name       == other.state_name)
     &&(county_name      == other.county_name)
     &&(ID               == other.ID)
     &&(station_ID_code  == other.station_ID_code)
     &&(station_name     == other.station_name)
     &&(comment          == other.comment));
}
//_compares_exactly_to_geolocation__________________________________2010-05-19_/
const char *Geolocation::set_station_ID_code  (const char *station_code_)
{ station_ID_code.assign(station_code_);
   if (station_ID_code.length()) available_data |= avail_station_ID_code;
   return station_ID_code.c_str();
}
//_set_station_ID_code______________________________________________2003-12-17_/
std::string &Geolocation::set_station_ID_code  (const std::string &station_code_)
{  station_ID_code.assign(station_code_);
   if (station_ID_code.length()) available_data |= avail_station_ID_code;
   return station_ID_code;
}
//______________________________________________________________________________
std::string &Geolocation::set_country_name(const std::string &country_name_)
{  country_name.assign(country_name_);
   CORN::strip_string(country_name);                                             //130919
   if (country_name_ .length())   available_data |= avail_geopolitical_names;
   return country_name;
}
//_set_country_name_________________________________________________2003-12-17_/
std::string &Geolocation::set_state_name  (const std::string &state_name_)
{  state_name.assign(state_name_);
   CORN::strip_string(state_name);                                               //130919
   if (state_name_   .length())    available_data |= avail_geopolitical_names ;
   return state_name; }
//_set_state_name______________________________________________________________/
std::string &Geolocation::set_county_name (const std::string &county_name_)
{  county_name.assign(county_name_)  ;
   CORN::strip_string(county_name);                                              //130919
   if (county_name_  .length())    available_data |= avail_geopolitical_names;
   return county_name;
}
//_set_county_name_____________________________________________________________/
std::string &Geolocation::set_station_name(const std::string &station_name_)
{  std::string local_station_name(station_name_);                                //051218
   CORN::strip_string(local_station_name);                                       //130919
   station_name.assign(local_station_name);                                      //130919
   CORN::strip_string(station_name);                                             //130919
   if (local_station_name .length())    available_data |= avail_station_name;
   return station_name;
}
//_set_station_name____________________________________________________________/
std::string &Geolocation::set_comment(const std::string &comment_)
{  std::string local_comment(comment_);                                          //051218
   CORN::strip_string(local_comment);                                            //130919
   comment.assign(local_comment);
   if (local_comment.length())    available_data |= avail_comment ;
   return comment;
}
//_set_comment_________________________________________________________________/
float32 Geolocation::set_latitude_dec_deg_32  (float32 latitude_)
{  if (latitude_) available_data |= avail_geocoordinates;
   return Geocoordinate::set_latitude_dec_deg_f32(latitude_);                    //120325
}
//_set_latitude_dec_deg_32_____________________________________________________/
float32 Geolocation::set_longitude_dec_deg_32 (float32 longitude_)
{  if (longitude_)               available_data |= avail_geocoordinates;
   return Geocoordinate:: set_longitude_dec_deg_f32(longitude_);                 //120325
}
//______________________________________________________________________________
const char *Geolocation::label_string(std::string &buffer)                 const
{  // Note the station ID must come first and be space separated
   buffer = station_ID_code
      + " (" + station_name + ", " + country_name + ", " + state_name + ", "
      + county_name + ", ";
   Geocoordinate::label_string(buffer);
   buffer += ")";
   return buffer.c_str();
}
//_label_string_____________________________________________________2017-04-23_/

/* 190923  DONT DELETE need to reimplement as Geolocation_record derived from Geocoordinate_record

bool Geolocation::setup_structure(CORN::Data_record &data_rec,bool for_write) //contribution_
{  // Note that the calling routine should set the current section
   if (!for_write)                                                               //100108
      available_data = avail_geocoordinates|avail_geopolitical_codes
      |avail_geopolitical_names|avail_station_number|avail_station_name
      |avail_station_ID_code|avail_comment|avail_geocoordinates;                 //100108
   data_rec.expect_nat32("LOC_ID",ID,10);
   data_rec.expect_string(LABEL_station_name,station_name,128);
   data_rec.expect_string(LABEL_country,country_name,0,0);
   data_rec.expect_string(LABEL_state,state_name,0,0); // or province
   data_rec.expect_string(LABEL_county,county_name,0,0);
   data_rec.expect_string("comment", comment,0,0);                               //100105
   Geocoordinate::setup_structure(data_rec,for_write);                           //120325
   data_rec.expect_valid_float32(v_screening_height);
   data_rec.expect_uint16("country_code",country_code_ISO3166,10,"ISO3166");
   data_rec.expect_uint16(LABEL_state_code,state_code,10,"(FIPS code for USA)");
   return true;
}
//_setup_structure__________________________________________________2008-03-26_/

*/


float64 Geolocation::set_screening_height_cstr(const char * screening_height_)
{  return set_screening_height(atof(screening_height_));
}
//_set_screening_height_cstr________________________________________2009-09-22_/
float64  Geolocation::set_screening_height(float32 _screening_height)
{  if (_screening_height) available_data |= avail_screening_height;              //100519
      return screening_height_32 = _screening_height;
}
//_set_screening_height_____________________________________________2006-07-29_/
const char *Geolocation::get_station_ID_code()                         mutation_
{  if (!has_station_ID_code())                                                   //130529
   {  char ID_cstr[20];                                                          //130529
      if (ID)                                                                    //160124
      {  CORN::nat32_to_cstr(ID,ID_cstr,10);                                     //130529
         station_ID_code.assign(ID_cstr);                                        //130529
   }  }
   return station_ID_code.c_str();
}
//_get_station_ID_code______________________________________________2003-12-17_/
nat32  Geolocation::set_station_number(nat32 station_num_)
{  ID=station_num_;
   if (station_num_)
      available_data |= avail_station_number;
   if (!station_ID_code.length())                                                //100116
   {  // If no ID code string, but there is a station number, use the station number as the ID 100116
      {  char station_number_cstr[20];                                           //100116
         CORN::nat32_to_cstr(ID,station_number_cstr,10);                         //101210_100116
         station_ID_code.assign(station_number_cstr);                            //100116
      }                                                                          //100116
      available_data |= avail_station_ID_code;
   }                                                                             //100116
   return ID;
}
//_set_station_number_______________________________________________2003-12-17_/
nat32  Geolocation::set_ID(nat32 ID_)
{ return set_station_number(ID_);
}
//_set_ID______________________________________________________________________/
std::string Geolocation::get_ID_string()                                  const
{  char cell_ID_str[20];
   CORN::nat32_to_cstr (ID,cell_ID_str,10);
   std::string result(cell_ID_str);
   return result;
}
//_get_ID_string____________________________________________________2013-10-13_/
bool Geolocation::set_from_ISO_6709
(const std::string &from_string)
{  Geocoordinate_format surmised_format;                                         //161122
   bool sat = Geocoordinate::set_from_ISO_6709(from_string,surmised_format);     //161122
   if (sat) available_data |= avail_geocoordinates;
   return sat;
}
//_set_from_ISO_6709________________________________________________2016-01-25_/

