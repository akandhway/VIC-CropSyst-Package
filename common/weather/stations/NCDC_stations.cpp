#include "corn/std/std_fstream.h"
#include "common/weather/stations/NCDC_stations.h"
#include "corn/measure/measures.h"
#include "corn/string/strconv.hpp"
#include <string.h>

/*
Example of format
COOPID WBANI STATION NAME                  COUNTRY                         ST COUNTY                    CL LAT    LON    ELEV
------ ----- ----------------------------- ------------------------------- -- ------------------------- -- ------ ------ ------
010008 99999 ABBEVILLE 1 NNW               UNITED STATES                   AL HENRY                     07   3135  -8517  141.7

ELEV Elevation is in meters

*/
//______________________________________________________________________________

const char *NCDC_WBAN_location::get_key()                                  const
{ return WBAN_mode ? WBANI.c_str() : COOPID.c_str();
}
bool NCDC_WBAN_location::is_key_string(const std::string &key)      affirmation_
{  return WBAN_mode ? WBANI == key : COOPID == key;
}
//__________________________________________________________________2018-08-20_/
bool  NCDC_WBAN_location::is_WBAN_station()                                const
{  std::string str_99999("99999");
   return WBANI != str_99999;                                                    //140516
}
//______________________________________________________________________________
bool NCDC_WBAN_location::read(istream &sodstn_file)
{  char buffer[255];
      sodstn_file.read(buffer,7);   buffer[6] = 0;    COOPID.assign(buffer);        CORN::strip_string(COOPID);
      sodstn_file.read(buffer,6);   buffer[6] = 0;    WBANI.assign(buffer);         CORN::strip_string(WBANI);
      sodstn_file.read(buffer,30);  buffer[30] = 0;   STATION_NAME.assign(buffer);  CORN::strip_string(STATION_NAME);
      sodstn_file.read(buffer,32);  buffer[32] = 0;   COUNTRY.assign(buffer);       CORN::strip_string(COUNTRY);
      sodstn_file.read(buffer,3);   buffer[3] = 0;    ST.assign(buffer);            CORN::strip_string(ST);
      sodstn_file.read(buffer,26);  buffer[26] = 0;   COUNTY.assign(buffer);        CORN::strip_string(COUNTY);
      sodstn_file.read(buffer,3);   buffer[3] = 0;    CL = atoi(buffer);
      sodstn_file.read(buffer,7);   buffer[7] = 0;    LAT = atoi(buffer);
      sodstn_file.read(buffer,7);   buffer[7] = 0;    LON = atoi(buffer) ;

      std::string elev_str;                                                      //140516
      getline(sodstn_file,elev_str);                                             //140516
      ELEV = atof(elev_str.c_str());
   return true;
}
//______________________________________________________________________________
const char *NCDC_WBAN_location::label_string(std::string &buffer)          const
{
   if (WBAN_mode) buffer += WBANI;
   else           buffer += COOPID;
   buffer += " ("+STATION_NAME+", "+COUNTRY+", "+ST+", "+COUNTY+", ";
   buffer.append(deg_minute_to_str(LAT));
   buffer += ", ";
   buffer.append(deg_minute_to_str(LON));
   buffer += ", ";
   CORN::append_float32_to_string(get_elevation_m(), 2,buffer);
   buffer += "m)";
   return buffer.c_str();
}
//_2017-04-24___________________________________________________________________
float32 NCDC_WBAN_location::deg_minute_to_dec_deg(int16 deg_minute)        const
{  float32 deg = deg_minute/100;
   float32 min = deg_minute % 100;
   float32 dec_deg = min / 60.0  + deg;
   return dec_deg;
}
//______________________________________________________________________________
std::string &NCDC_WBAN_location::deg_minute_to_str(int16 deg_min)          const
{  static std::string bufferX;
   bufferX.assign("");
   int32 deg = deg_min/100;
   bool west = deg < 0;
   deg = abs(deg);
   int32 min = abs(deg_min % 100);
   char buff2[10];
   bufferX.append(CORN::int16_to_cstr(deg,buff2,10));
   bufferX.append("�");
   bufferX.append(CORN::int16_to_cstr(min,buff2,10));
   bufferX.append("\'");
   bufferX.append(west?"W":"E");
   return bufferX;
}
//______________________________________________________________________________
void NCDC_WBAN_location::get_description(std::string &buffer)              const
{
   buffer.assign(WBAN_mode ? WBANI : COOPID);
   buffer.append(" (");
   buffer.append(STATION_NAME);
   CORN::strip_string(buffer);
   buffer.append(")");
}
//______________________________________________________________________________
float32  NCDC_WBAN_location::get_longitude_dec_deg() const { return deg_minute_to_dec_deg(LON); }
float32  NCDC_WBAN_location::get_latitude_dec_deg()  const { return deg_minute_to_dec_deg(LAT); }
float32  NCDC_WBAN_location::get_elevation_m()       const { return ELEV; }  // ELEV is in meters
//______________________________________________________________________________
int32 NCDC_WBAN_location::get_state_number()                               const
{  // extract the first 2 digits of the COOPID
   char state_cstr[3];
   strncpy(state_cstr,COOPID.c_str(),2);
   return  atoi(state_cstr);
}
//______________________________________________________________________________
int32 NCDC_WBAN_location::get_station_number()                             const
{  // extract the last for digits of the COOPID
   const char *station_c_str
      = WBAN_mode
        ?  WBANI.c_str()
        :  COOPID.c_str()+2;
   long station_number = atol(station_c_str);
   return station_number;
}
//______________________________________________________________________________
// The index of this array is the FIPS state number
uint8 FIPS_to_NCDC_state_number[] =
{ //    FIPS   NCDC  State abbrev
        /*00*/ 99 // "__"
,       /*01*/ 99 //  "AA",
,       /*02*/  1 //  "AL",
,       /*03*/ 99 //  "__",
,       /*04*/  2 //  "AZ",
,       /*05*/  3 //  "AR",
,       /*06*/  4 //  "CA",
,       /*07*/ 99 //  "__",
,       /*08*/  5 //  "CO",
,       /*09*/  6 //  "CT",
,       /*10*/  7 //  "DE",
,       /*11*/ 99 //  "DC",   // No stations in DC?
,       /*12*/  8 //  "FL",
,       /*13*/  9 //  "GE",    // GA?  Georgia?
,       /*14*/ 99 //  "__",
,       /*15*/ 99 //  "HA",    // Hawaii?
,       /*16*/ 10 //  "ID",
,       /*17*/ 11 //  "IL",
,       /*18*/ 12 //  "IN",
,       /*19*/ 13 //  "IA",
,       /*20*/ 99 //  "KA",   // ???
,       /*21*/ 15 //  "KY",
,       /*22*/ 16 //  "LA",
,       /*23*/ 17 //  "ME",
,       /*24*/ 18 //  "MD",
,       /*25*/ 19 //  "MA",
,       /*26*/ 20 //  "MI", // Michigan
,       /*27*/ 21 //  "MN",
,       /*28*/ 22 //  "MS", // Mississippi
,       /*29*/ 23 //  "MO",
,       /*30*/ 24 //  "MT",
,       /*31*/ 99 //  "NB",
,       /*32*/ 26 //  "NV",
,       /*33*/ 27 //  "NH",
,       /*34*/ 28 //  "NJ",
,       /*35*/ 29 //  "NM",
,       /*36*/ 30 //  "NY",
,       /*37*/ 31 //  "NC",
,       /*38*/ 32 //  "ND",
,       /*39*/ 33 //  "OH",
,       /*40*/ 34 //  "OK",
,       /*41*/ 34 //  "OR",
,       /*42*/ 36 //  "PN",   // PA?
,       /*43*/ 99 //  "__",
,       /*44*/ 37 //  "RI",
,       /*45*/ 38 //  "SC",
,       /*46*/ 40 //  "TN",
,       /*47*/ 41 //  "TX",
,       /*48*/ 42 //  "UT",
,       /*49*/ 43 //  "VT",
,       /*50*/ 44 //  "VA",
,       /*51*/ 67 //  "VI",
,       /*52*/ 99 //  "__",
,       /*53*/ 45 //  "WA",
,       /*54*/ 46 //  "WV",
,       /*55*/ 47 //  "WI",
,       /*56*/ 48 //  "WY"
};
//______________________________________________________________________________
