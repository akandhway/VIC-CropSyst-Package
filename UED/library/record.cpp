#include "UED/library/record.h"
#include "corn/chronometry/date_time_64.h"
#include "corn/format/binary/binary_file_interface.h"
namespace UED {
//______________________________________________________________________________
bool Record_code_clad::get_timestamp
   (CORN::datetime64 target_date_time_64
   ,CORN::datetime64 &timestamp_date_time)
{  CORN::Date_time_clad_64 target_date_time(target_date_time_64);                //030714
   CORN::Date_time_clad_64 time_stamp(0);                                        //030714
   // Given a date/tim this method deterines a record time stamp for the specified record_type
   bool result = false;
   if (has_year_time_stamp())
   {  time_stamp.set_year(target_date_time.get_year());
      time_stamp.set_DOY(1);
      result = true;
   } else if (has_date_time_stamp())
   {  time_stamp.set_YD(target_date_time.get_year(),target_date_time.get_DOY()); //030714
      result = true;
   } else if (has_time_time_stamp())
      time_stamp.set_datetime64(target_date_time_64);                            //030714
   timestamp_date_time = time_stamp.get_datetime64();                            //030714
   return result;
}
//_get_timestamp____________________________________________________1998-01-14_/
bool Record_base::read_body(CORN::Binary_file_interface &parent_database)
{  if (rec_length)        // This is the default read_body that simply reads an arbitrary blob
   {  std::string buffer;
      parent_database.read_string(buffer,CORN::ASCII_string,rec_length);
      unknown_data.assign(buffer);
   }
   return true;
}
//_read_body____________________________________________2003-01-25__1997-11-04_/
void Record_base::write_body(CORN::Binary_file_interface&parent_database)
{  //050601  May need to always recompute rec_length (because we may add things in Geolocation record
   rec_length=compute_record_body_size() ;              // This line probably not needed because we should have already updated record_length when writing the header
   if (rec_length)
   {  // Make sure data is no more or no less than specified length
     parent_database.write_string(unknown_data,CORN::ASCII_string);              //100114
   }
}
//_write_body_______________________________________________________1997-11-04_/
Record_base::Record_base(CORN::Record_code record_type_code)
:Record_type_distinguisher_interface()
,CORN::Binary_record(record_type_code,0)
,unknown_data()
{  // Since we are creating a record, we presume it to be modified so it will be written when disposed.
   set_modified(true);                                                           //000711
   in_memory = true;                                                             //100202
}
//_Record_base:constructor_____________________________________________________/
Record_base::Record_base(const Record_base &from_copy)
:Record_type_distinguisher_interface(from_copy)
,CORN::Binary_record(from_copy)
,unknown_data(from_copy.unknown_data)
{  // Since we are creating a record, we presume it to be modified so it will be written when disposed.
    set_modified(true);
   in_memory = true;                                                             //100202
}
//_Record_base:constructor__________________________________________2005-01-28_/
Record_base::~Record_base()
{}
//_Record_base:destructor___________________________________________1997-11-04_/
Record_base *Record_base::clone()                                          const
{  return new Record_base(*this); }
//_clone____________________________________________________________2014-01-16_/
#ifdef UED_DUMP
using namespace std;
void Record_base::dumpX(std::ostream &txt_strm)
{
   CORN::Binary_record::dumpX(txt_strm);
   if (is_standard())            txt_strm << "| standard";
   if (is_control())
   {                             txt_strm << "| is control";
      if (is_global_control())   txt_strm << "| global control";
   }
   if (has_data())
   {  if (is_data_set())         txt_strm << "| data set record ";
      if (is_data_scalar())      txt_strm << "| data scalar";
      if (is_data_vector())      txt_strm << "| data vector";
      /* if (is????())*/         txt_strm << "| time stamped";
      if (get_time_stamp_code()) txt_strm << "| time stamp code";
      if (has_year_time_stamp()) txt_strm << "| has year time stamp";
      if (has_date_time_stamp()) txt_strm << "| has date time stamp";
      if (has_time_time_stamp()) txt_strm << "| has time time stamp";
      if (has_variable_code())   txt_strm << "| has_variable code";
      if (has_units_code())      txt_strm << "| has units code";
   }
   txt_strm << std::endl;
}
#endif
//_dump________________________________________________________________________/
void Record_base::write_YAML(std::ostream &YAML_strm,nat16 indent_level)
{
   std::string indent(indent_level,' ');
   CORN::Binary_record::write_YAML(YAML_strm,indent_level);
   YAML_strm<<indent << "components: [";
   if (is_standard())
      YAML_strm << " standard";
   else
      YAML_strm << " nonstandard";

   if (is_control())
   {                             YAML_strm << ", is_control";
      if (is_global_control())   YAML_strm << ", global_control";
   }
   if (has_data())
   {  if (is_data_set())         YAML_strm << ", data_set_record ";
      if (is_data_scalar())      YAML_strm << ", data_scalar";
      if (is_data_vector())      YAML_strm << ", data_vector";
      /* if (is????())*/         YAML_strm << ", time_stamped";
      if (get_time_stamp_code()) YAML_strm << ", time_stamp code";
      if (has_year_time_stamp()) YAML_strm << ", has_year_time_stamp";
      if (has_date_time_stamp()) YAML_strm << ", has_date_time_stamp";
      if (has_time_time_stamp()) YAML_strm << ", has_time_time_stamp";
      if (has_variable_code())   YAML_strm << ", has_variable_code";
      if (has_units_code())      YAML_strm << ", has_units_code";
   }
   YAML_strm<< " ]" << std::endl;
}
//_write_YAML_______________________________________________________2018-01-08_/
}//_namespace UED______________________________________________________________/

