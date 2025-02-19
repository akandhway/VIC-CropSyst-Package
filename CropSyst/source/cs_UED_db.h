#ifndef cs_UED_dbH
#define cs_UED_dbH
#include "UED/library/database_file.h"
#include "corn/chronometry/date_32.h"
#include "corn/chronometry/date_time_64.h"

#define CROPSYST_major_version   1
#define CROPSYST_minor_version   10
//______________________________________________________________________________
class CropSyst_UED_database
: public UED::Database_file_indexed
{
   CORN::Date_time_clad_64 current_record_time_stamp;                            //030713
      // The time stamp to use for dataset for this year
 public:
   CropSyst_UED_database
      (const char *_ued_filename
      ,std::ios_base::openmode _openmode                                         //140422
      ,const CORN::Date_const &current_record_time_stamp);
   virtual void initialize_with_descriptions                                     //030107
      (const std::string &simulation_description
      ,const std::string &location_description
      ,const std::string &soil_description
      ,bool fast_graph);
   void write_units_variables_definitions();                                     //000608
   virtual ~CropSyst_UED_database();                                             //000608
   inline void set_time_stamp_for_records(const CORN::Date_const &_current_record_time_stamp)  //000403
      { current_record_time_stamp.set_date32(_current_record_time_stamp.get_date32()); }   //030713
      // Requests all recording objects to commit the current recording of their daily records.
      // If it is the last day of year, requests the recording objects to flush their
      // records and add the desired variables again.
      // Increments the current time step.
};
//_CropSyst_UED_database_______________________________________________________/
#endif
//CS_UED_DB_H

