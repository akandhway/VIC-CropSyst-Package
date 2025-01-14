#ifndef aggregateH
#define aggregateH

#include "corn/file_sys/smart_filename.h"
#include "corn/container/enumlist.h"

namespace STATSGO
{  class Database;
}
class Polygon_aggregation;
//______________________________________________________________________________
namespace CORN
{  class Generic_data_record;
   class Dynamic_int32_indexed_dBase_data_source;
}
//______________________________________________________________________________
class Tally_aggregator
{
   CORN::Smart_file_name tally_table_dbf;       // Input file
   CORN::Smart_file_name aggregate_table_dbf;   // output file
   STATSGO::Database    &STATSGO_database;
   std::string            polygon_ID_attribute;
   CORN::Enumeration_list polygon_aggregations;
   nat8                   upto_capability_class;  // comes from GIS_params
   bool                   with_irrigation;        // comes from GIS_params
   CORN::Dynamic_int32_indexed_dBase_data_source &GIS_table;
public:
   Tally_aggregator
      (const char          *_tally_table_dbf
      ,const char          *_polygon_ID_attribute
      ,CORN::Dynamic_int32_indexed_dBase_data_source   &_GIS_table
      ,STATSGO::Database    &_STATSGO_database
      ,nat8    _upto_capability_class
      ,bool    _with_irrigation);
   nat32 aggregate();
      // returns the number of files aggregated
   Polygon_aggregation *provide_polygon_aggregation
      (sint32 _polygon_ID,int16 _cropable_percentage
      ,CORN::Generic_data_record *_weighted_average_data_record);
};
//_Tally_aggregator____________________________________________________________/
#endif

