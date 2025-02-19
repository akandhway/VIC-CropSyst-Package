#ifndef database_file_substitutionH
#define database_file_substitutionH
#include "UED/library/database_file.h"
#include "corn/const.h"
using namespace UED;
namespace CS_UED
{
//______________________________________________________________________________
class Date_substituter
: public CORN::Item //191010 Enumeration
{
   UED::Variable_code variable_code;
   CORN::Year base_year;                                                         //170323
      // This is the year of real data to substitute from
      // This is not necessarily the earliest date/year in the file,
      // but it could be (default)
   CORN::Year reference_year; // typically the simulation period start year      //170331
   CORN::Year latest_year;
   int8  year_offset; // used to offset the earliest year to substitute from     //161207
      // Normally 0 may be negative
      // Added for FlexCropping project but may be generally usuable
   nat16  years_range;
 public:
   Date_substituter
      (UED::Variable_code variable_code_
      ,CORN::Year base_year_
      ,CORN::Year reference_year                                                 //170331
      ,CORN::Year latest_year_
      ,int8       year_offset_);                                                 //161207
   Time_query *substitute(const Time_query &time_query)              rendition_; //160331
   inline virtual bool is_key_string(nat32 key)                    affirmation_  //180820
      { return variable_code == key; }
   inline virtual nat32 get_key_nat32()           const { return variable_code;} //160331_110305_020319
};
//_Date_substituter_________________________________________________2016-03-31_/
class _UED_DLL
Database_file_indexed
: public UED::Database_file_indexed
{
 protected:
   contribute_ CORN::Enumeration_list *date_substituters;
      // this option allows missing records to be subtitutes with dates that
      // are available at a  future (or past) time in the dataset
   contribute_ CORN::Year  substitute_base_year;                                 //170323
   contribute_ CORN::Year  substitute_reference_year;                            //170331
      // Initialized to 0 indicating that there is no subsitution (yet)
   contribute_ int8        substitute_year_offset;                               //161207
      // Usually 0 unless using date subsitution
      // Added for FlexCropping project but generally useful.
   contribute_ datetime64  preclude_latest_date_time;
      // non 0 values forces the UED to presume there is no data
      // after the specified date (This was special for Harsimran's project)
      // [probably should have a specialization class (CropSyst UED) to handle this
 public: // structors
    Database_file_indexed                                                        //070718
      (const char *ued_filename
      ,std::ios_base::openmode _openmode = std::ios::in                          //130311
      ,bool _auto_float16 = true);                                               //140612
    virtual ~Database_file_indexed();                                            //170323
 public: // UED::Database_file_indexed reimplementions
   virtual datetime64 get_latest_date_time                                       //970915
      ( Record_code   for_record_type_code = UED_FOR_ALL_RECORD_TYPES
      , Variable_code for_var_code = UED_FOR_ALL_VARIABLES)               const;
   virtual Binary_data_record_cowl *locate_data_record
      (Record_code  rec_type
      ,Variable_code var_code
      ,const Time_query &time_query)                                 stream_IO_;
 public:
   bool enable_substitution
      (CORN::Year    substitute_base_year
      ,CORN::Year    substitute_reference_year                                   //170331
      ,int8          substitute_year_offset)                      contribution_;
   bool enable_preclusion
      (datetime64    preclude_latest_date_time_)                  contribution_;
      // Also enables substitution.
      // If substitution was no previously enabled,
      // the base year is set to the earliest year in the file.

   Time_query * substitute_time_query
      (Record_code       rec_type
      ,Variable_code     variable_code
      ,const Time_query &time_query);
      // may return 0 if time query date cannot be substituted.
   virtual float32 get_no_create
      (Record_code  rec_type
      ,Variable_code       variable
      ,const Time_query   &time_query
      ,CORN::Units_code   &requested_units
      ,CORN::Quality      &attribute )                               stream_IO_;
   virtual bool get_parameter_on                                                 //151122
      (modifiable_ CS::Parameter &parameter
      ,Variable_code     variable
      ,const Time_query &time_query
      ,Record_code       rec_type)                                   stream_IO_; //160331
};
//_Database_file_indexed____________________________________________2017-03-11_/
}//_namespace CS_UED___________________________________________________________/
#endif
