#ifndef CC_station_datarecH
#define CC_station_datarecH

#include <corn/data_source/datarec.h>
using namespace CORN;
//______________________________________________________________________________
class Canadian_climatological_station_record
: public Data_record
{public:
   std::string STATIONID;
   std::string NAME;
   std::string PROVINCE;
   //NYI float64     LATDEG;  // In the original Access file, double floats are used
   //NYI float64     LATMIN;
   float64     LATDD;
   //NYI float64     LONGDEG;
   //NYI float64     LONGMIN;
   float64     LONGDD;
   float64     ELEV;
   //NYI DATEBEGAN
   //NYI SEASON
   //NYI DATEENDED
   //NYI SYNO
   //NYI HLY
   //NYI TEMP
   //NYI PRECIP
   //NYI RATE
   //NYI WIND
   //NYI SOIL
   //NYI EVAP
   //NYI SUN
   //NYI RAD
   //NYI O3
   //NYI UA
   //NYI SURVEY
   //NYI TOWER
   //NYI AQ
   //NYI NIPHER
   //NYI REGION
 public:
   inline Canadian_climatological_station_record()
      : Data_record("-")
      {}
   virtual bool expect_structure(bool for_write);                                //170111
};
//_2003-12-17___________________________________________________________________

#endif