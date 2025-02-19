#ifndef WMO_stationsH
#define WMO_stationsH

#include "corn/container/enumlist.h"
#include "common/geodesy/geolocation.h"
using namespace std;
//______________________________________________________________________________
class WMO_location
: public CORN::Item
{public:
   Geolocation geolocation;
 public:
   bool read(istream &stnlist_sorted_file);
   //170424 virtual const char *label_cstr(char* buffer)                const;
   virtual const char *label_string(std::string &buffer)                  const;
   virtual bool is_key_nat32(nat32 key)                            affirmation_; //180820
   virtual nat32 get_key_nat32()                                          const;
};
//______________________________________________________________________________
#endif

