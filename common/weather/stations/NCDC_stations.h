#ifndef NCDC_stationsH
#define NCDC_stationsH
#include "corn/container/bilist.h"
using namespace std;
/*
Example of format
COOPID WBANI STATION NAME                  COUNTRY                         ST COUNTY                    CL LAT    LON    ELEV
------ ----- ----------------------------- ------------------------------- -- ------------------------- -- ------ ------ ------
010008 99999 ABBEVILLE 1 NNW               UNITED STATES                   AL HENRY                     07   3135  -8517  141.7

The cooperative ID number consist of 2 fields
The first two digits are the state number (I don't think this corresponds to the FIPS code number)
The next 4 digits are the station number (these match the station ID)

Note that NCDC NOAA Summary of the Day files have the WMO/DATSAV3
number and the WBAN number. This is confusing because the WMO number
is also 6 digits but the WMO station numbers do not match the COOPID
of the NCDC_sumary_of_the_day
*/
//______________________________________________________________________________
class NCDC_WBAN_location
: public CORN::Item
{
public:
   std::string COOPID;
   std::string WBANI;
   std::string STATION_NAME;
   std::string COUNTRY;
   std::string ST;
   std::string COUNTY;
   nat8       CL;
   int16       LAT;     // in degrees minutes  I.e. -11710 is
   int16       LON;     // in degrees minutes  117 degrees 10 mintutes west
   float32     ELEV;    // in meters????
private: // contributes
   bool WBAN_mode;
public:
   inline NCDC_WBAN_location(bool _WBAN_mode)
      : CORN::Item()
      , CL(0)
      , LAT(0)
      , LON(0)
      , ELEV(0)
      , WBAN_mode(_WBAN_mode)
      {};
   bool read(istream &sodstn_file);
   virtual const char *label_string(std::string &buffer)                  const;
   virtual bool is_key_string(const std::string &key)              affirmation_; //180820
   virtual const char *get_key()                                          const;
   float32 get_longitude_dec_deg()                                        const;
   float32 get_latitude_dec_deg()                                         const;
   float32 get_elevation_m()                                              const;
   float32 deg_minute_to_dec_deg(int16 deg_minute)                        const;
   std::string &deg_minute_to_str(int16 deg_min)                          const;
   void get_description(std::string &buffer)                              const;
   int32 get_state_number()                                               const;
   int32 get_station_number()                                             const;
   bool  is_WBAN_station()                                                const;
};
//______________________________________________________________________________
extern nat8 FIPS_to_NCDC_state_number[];
extern bool load_NCDC_stations(CORN::Bidirectional_list &locations);
extern bool load_WBAN_stations(CORN::Bidirectional_list &locations);
#endif

