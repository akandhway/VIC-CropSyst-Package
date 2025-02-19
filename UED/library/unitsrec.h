#ifndef unitsrecH
#define unitsrecH
#include "UED/library/record.h"
#include "UED/library/units_def.h"
namespace UED {
//______________________________________________________________________________
class Units_definition_format_0_record
: public Record_base  // Must be first superclass!
, public Units_definition
{
 public:
   Units_definition_format_0_record();
      // This constructor is for reading the units definition from the database
   Units_definition_format_0_record
      (UED_units_code  i_code                                                    //990107
      ,  const char *i_description                                               //040908
      ,  const char *i_abbreviation);                                            //040908
   Units_definition_format_0_record
      (const Units_definition &to_copy);
   virtual Units_definition_format_0_record* clone()                       const;//140116
   inline virtual nat32 compute_record_body_size() const
      { return compute_units_format_size(); }
   virtual bool read_body(Binary_file_interface &parent_database);               //990115
      // This method must be overridden it will be called by read() after reading the record type code
      // For unrecognized record types, the data will be read it to unknown data
   inline virtual void write_body(Binary_file_interface&parent_database);        //990115
      // This  method must be overridden it will be called by write () after writing the record type code
#ifdef UED_DUMP
   inline virtual void dumpX(std::ostream &txt_strm)
      { Units_definition::dumpX(txt_strm);
      }
#endif
   inline virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level)    //180108
      { Units_definition::write_YAML(YAML_strm,indent_level); }                  //180108
};
//_Units_definition_format_0_record_________________________________1998-08-18_/
}//_namespace UED______________________________________________________________/
#endif
//unitsrec.h

