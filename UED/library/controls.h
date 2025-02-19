#ifndef controlsH
#define controlsH
#include "UED/library/record.h"
#include "UED/library/UED_types.h"
namespace UED {
//______________________________________________________________________________
class Text_record : public Record_base
{  // This class serves as a base record for commentary data
 public:
   std::string text;        // as of 040924 this may be SDF format
 public: // constructors
   inline Text_record
      (CORN::Record_code rec_type_code
      ,const char *_text)                                                        //070420
      : Record_base(rec_type_code)
      , text(_text ? _text : "")
      {}
   inline Text_record(const Text_record &from_copy)                              //050126
      : Record_base(from_copy)
      , text(from_copy.text)
      {} // copy constructor
   virtual Text_record* clone()                                            const //140116
      { return new Text_record(*this); }
 public:  // UED record base overrides
   inline virtual nat32 compute_record_body_size()const{return text.length()+1;}
      // Returns the size of the record body (not including the header).
   virtual bool read_body(CORN::Binary_file_interface &parent_database);         //971103
   virtual void write_body(CORN::Binary_file_interface &parent_database);        //971103
 public: // Accessors
   inline const std::string &get_text()                   const { return  text;}
};
//_Text_record______________________________________________________1997-12-08_/
class General_comment_record : public Text_record
{public:
   inline General_comment_record
      (const char *_general_comment = 0)
      : Text_record((CORN::Record_code)(UED_general_comment),_general_comment)
      {}
 public:
   inline const std::string &get_general_comment()   const { return get_text();}
};
//_General_comment_record___________________________________________1997-12-08_/
class Period_record : public Text_record
{   // The text of the text record specifies a description of the period
 public:
    nat32 options; // bitmask identifies following optional record information
   CORN::datetime64 initiation; //was start_date_time;
      //  0-1 Can be used to associate the comment with a particular date/time
      //    (stored as datetime64)
      //  0 set indicates time is significant
      //  1 set indicates date is significant
   CORN::datetime64  completion;
      //  2-3 Indicates this record defines a period. date_time (if present) is the start of the period
      //  2 set indicates time is significant
      //  3 set indicates date is significant
   nat32      application_code;
      //  4 Specific applications can use this to store specific information between models or utilities
   nat32      enumeration;
      //  5 This can be used to store an enumeration (I.e. a number representing the period type).
   nat32      index;
      //  6 An application can use this number to represent the periods's index (I.e. a cross reference associated with some other output)
      // For crop periods this is the Crop's sowing date
      // Used by REACCH and OFoot where UED's are aggregate.
      // As of version 5 this use is now obsolete because all versions of
      // CropSyst now output crop types in separate output files

   nat32      variable_code;
      //  7 An associated variable code (CropSyst uses this to indicate text variables (Crop Name, and growth stage)
      // I am moving away from using variable codes in favour of ontology definitions

   // Conceptual: add ontology definition (URI)  std::string ontology;

 public:  // constructors
   Period_record
      (const char       *description_
      ,nat32             options_           =0
      ,CORN::datetime64  start_date_time_   =0
      ,CORN::datetime64  end_date_time_     =0
      ,nat32             application_code_  =0
      ,nat32             enumeration_       =0
      ,nat32             index_             =0
      ,UED::Variable_code variable_code_    =0);
   Period_record(const Period_record &from_copy); // copy constructor
   virtual Period_record* clone()                                         const  //140116
      { return new Period_record(*this); }
 public:
   virtual nat32 compute_record_body_size()                               const;
//             Returns the size of the record body (not including the header).
   virtual bool read_body (CORN::Binary_file_interface &parent_database);        //971103
   virtual void write_body(CORN::Binary_file_interface &parent_database);        //971103
#ifdef UED_DUMP
   void dumpX(std::ostream &txt_strm);
#endif
   virtual void write_YAML(std::ostream &YAML_strm,nat16 indent_level);         //180108
};
//_2004-09-23___________________________________________________________________
class Generating_application_record : public Text_record
{
   nat16 version;
 public:  // constructor
   inline Generating_application_record                                          //070420
      (nat16 version_ = 0                                                        //990107
      ,const char *application_name_ = 0)
      : Text_record((CORN::Record_code)(UED_generating_application),application_name_)   //990107
      , version(version_)
      {}
   inline virtual Generating_application_record* clone()                  const  //140116
      { return new Generating_application_record(*this); }
 public:  // UED record base overrides
   inline virtual nat32 compute_record_body_size()                         const
               {return Text_record::compute_record_body_size() + sizeof(nat16);}
      // Returns the size of the record body (not including the header).
   virtual bool read_body(CORN::Binary_file_interface  &parent_database);
   virtual void write_body(CORN::Binary_file_interface  &parent_database);
 public: // data accessors                                            //991209
   inline nat16 get_version()                           const { return version;}
   inline nat16 get_major() const {return (nat16)((version &(nat16)0xF00)>>16);}
   inline nat16 get_release() const {return (nat16)((version&(nat16)0X0F0)>>8);}
   inline nat16 get_minor()   const { return (nat16)((version & (nat16)0X00F));}
   inline const std::string &get_description()       const { return get_text();}
};
//_Generating_application_record_______________________________________________/
class Database_description_record : public Text_record
{public: // constrctor
   inline Database_description_record                                            //990107
      (const char *description_ = 0)                                             //070420
      : Text_record((CORN::Record_code)(UED_database_description),description_)
      {}
 public:
   inline const std::string &get_database_description() const {return get_text();}
};
//_Database_description_record______________________________________1997-12-08_/
class Free_space_record : public Text_record
{ // Usually a block of deleted data created automatically when
  // records are deleted but there may be some situations where
  // is may be desirable to reserve a record block for future use.
 public: // constructor
   inline Free_space_record
      (nat32 reserve_size)
      : Text_record
         ((CORN::Record_code)(UED_free_space)
         ,std::string(' ',reserve_size).c_str())                                 //070420
      {}
};
//_Free_space_record________________________________________________1997-12-08_/
}//_namespace UED______________________________________________________________/
#endif
//controls.h

