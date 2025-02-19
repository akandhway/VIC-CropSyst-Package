#ifndef varrecH
#define varrecH
#include "UED/library/variable_format.h"
#include "UED/library/record.h"
namespace UED {
//______________________________________________________________________________
class Variable_definition_format_0_record
: public Record_base // Record base must be first super!!!                       //000707
, public Variable_format_0
{
public:  // constructors
   Variable_definition_format_0_record();
      // This constructor is for reading the variable definition from the database
   Variable_definition_format_0_record
      (  Variable_code  _code                                                    //990107
      ,  CORN::Units_code   _preferred_units                                     //050630
      ,  float32            _min_warning
      ,  float32            _max_warning
      ,  float32            _min_error
      ,  float32            _max_error
      ,  const char        *_description                                         //040908
      ,  const char        *_abbreviation);                                      //040908
      // This constructor is for writing the variable definition to the database
   Variable_definition_format_0_record                                           //990426
      (const Variable_format_0 &variable_def);
      // This constructor is for including standard or other definitions.
   virtual Variable_definition_format_0_record* clone()                   const; //140116
public:
   inline virtual nat32 compute_record_body_size()                        const  //001008
      { return compute_variable_format_size(); }
   virtual bool read_body(CORN::Binary_file_interface &parent_database);         //000908
      // This method must be overridden it will be called by read() after reading the record type code
      // For unrecognized record types, the data will be read it to unknown data
   virtual void write_body(CORN::Binary_file_interface &parent_database);        //000908
      // This  method must be overridden it will be called by write () after writing the record type code
#ifdef UED_DUMP
   inline virtual void dump(std::ostream &txt_strm)
      { Variable_format_0::dump(txt_strm); }
#endif
   inline virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level)    //180108
      { Variable_format_0::write_YAML(YAML_strm,indent_level); }                 //180108

 private:
   void write_definition(CORN::Binary_file_interface &parent_database);          //971103
   void read_definition (CORN::Binary_file_interface &parent_database);          //010525
};
//_Variable_definition_format_0_record______________________________1997-11-01_/
class Variable_definition_format_ontological_record
: public Record_base // Record base must be first super!!!                       //000707
, public Variable_format_ontological
{
public:  // constructors
   Variable_definition_format_ontological_record();
      // This constructor is for reading the variable definition from the database
   Variable_definition_format_ontological_record
      (  Variable_code  _code
      ,  CORN::Units_code   _preferred_units
      ,  const char        *_ontology);
      // This constructor is for writing the variable definition to the database
   Variable_definition_format_ontological_record
      (const Variable_format_ontological &variable_def);
      // This constructor is for including standard or other definitions.
   virtual Variable_definition_format_ontological_record* clone()         const;
public:
   inline virtual nat32 compute_record_body_size()                        const
      { return compute_variable_format_size(); }
   virtual bool read_body(CORN::Binary_file_interface &parent_database);
      // This method must be overridden it will be called by read() after reading the record type code
      // For unrecognized record types, the data will be read it to unknown data
   virtual void write_body(CORN::Binary_file_interface &parent_database);
      // This  method must be overridden it will be called by write () after writing the record type code
#ifdef UED_DUMP
   inline virtual void dumpX(std::ostream &txt_strm)
      { Variable_format_ontological::dumpX(txt_strm); }
#endif
   inline virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level)    //180108
      { Variable_format_ontological::write_YAML(YAML_strm,indent_level); }       //180108
 private:
   void write_definition(CORN::Binary_file_interface &parent_database);
   void read_definition (CORN::Binary_file_interface &parent_database);
};
//_Variable_definition_format_ontological_record____________________2016-06-19_/
}//_namespace UED______________________________________________________________/
#endif
//varrec

