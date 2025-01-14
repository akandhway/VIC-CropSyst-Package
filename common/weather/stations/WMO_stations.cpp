#include "corn/std/std_fstream.h"
#include "common/weather/stations/WMO_stations.h"
#include <stdlib.h>
   // for atoi

/*
                  *** WORLDWIDE STATION LIST ***

NUMBER = STATION NUMBER
CALL = STATION CALL LETTERS IF ASSIGNED
NAME = NAME OF LOCATION (& INDICATES PREVIOUSLY UNDER DIFFERENT NUMBER)
COUNTRY/STATE = 2-CHARACTER COUNTRY ABBREVIATION, FOR U.S.--
                2-CHARACTER STATE ABBREVIATION
                (SEE COUNTRY LIST)
LAT = LATITUDE IN DECIMAL DEGREES (HUNDREDTHS)
LON = LONGITUDE IN DECIMAL DEGREES (HUNDREDTHS)
ELEV = ELEVATION IN METERS

NUMBER CALL   NAME + COUNTRY/STATE   LAT   LON    ELEV

691804 KQLG   (GAF)TROGIR(ASOS)   ** +4353 +01625
789940        BOGUS CARIBBEAN     **
990061 DB061  ENVIRONM BUOY 52079 ** +0000 +14700 +0000
134900        RESERVED FOR NAVY   ** -9999 -99999 -9999
993194        SHIP BLOCK 19       ** -9999 -99999 -9999
691810 KQLH   TUZLA               ** +4453 +01871
136110        DURRES              AB +4130 +01945 +0012
*/
//______________________________________________________________________________
bool WMO_location::read(istream &stnlist_sorted_file)
{  std::string /*140516 CORN::UIO_string*/ buffer_str;
   getline(stnlist_sorted_file,buffer_str);
   if (buffer_str.length() == 55)
   {  geolocation.set_station_ID_code(buffer_str.substr(0,6).c_str());
      geolocation.set_station_number(atoi(geolocation.get_station_ID_code()));
      // Ignore CALL sign
      geolocation.set_station_name(buffer_str.substr(14,20));
      geolocation.set_country_name(buffer_str.substr(34,2));
      // Note that this field could have US state codes, but the file I have currently only has foreign countries.
      geolocation.set_latitude_dec_deg_32 (atof(std::string(buffer_str,37,5).c_str()) /100.0);
      geolocation.set_longitude_dec_deg_32(atof(std::string(buffer_str,43,6).c_str()) /100.0);
      geolocation.set_elevation_meter_cstr(std::string(buffer_str,50,5).c_str());

/*091102
We now need to use set accessor to make sure the available data is set
		geolocation.station_ID_code.assign(buffer_str.substr(0,6));
      geolocation.station_number = atoi(geolocation.station_ID_code.c_str());
		// Ignore CALL sign
      geolocation.station_name.assign(buffer_str.substr(14,20));
      geolocation.country_name.assign(buffer_str.substr(34,2));
      // Note that this field could have US state codes, but the file I have currently only has foreign countries.
      geolocation.latitude_dec_deg_32 = atof(Ustring(buffer_str,37,5).c_str()) /100.0;
      geolocation.longitude_dec_deg_32 = atof(Ustring(buffer_str,43,6).c_str()) /100.0;
      geolocation.elevation =  atof(Ustring(buffer_str,50,5).c_str());
*/
   } // ignore the line because it is missing fields
   // this is not an error condition
   return buffer_str.length() > 0;
}
//______________________________________________________________________________
const char *WMO_location::label_string(std::string &buffer)                const
{  buffer.append(geolocation.get_station_ID_code());
   return buffer.c_str();                                                        //170725
}
//_2017-04-24___________________________________________________________________
bool WMO_location::is_key_nat32(nat32 key)              affirmation_
{ return geolocation.get_station_number() == key; }
//______________________________________________________________________________180820_/
nat32 WMO_location::get_key_nat32() const
{  return (uint32)geolocation.get_station_number();
}
//______________________________________________________________________________
