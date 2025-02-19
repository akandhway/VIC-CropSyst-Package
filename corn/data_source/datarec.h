#ifndef datarecH
#define datarecH
/*
   CORN::Data_record is an abstract class.
   It is used when working with data that is associated with variable names or labels.

   I.e.
   Database (I.e. dBase) records where there is a name associated with each field.
   Windows INI files.
   CGI post/query data data.
   XML

   This class provides a common interface for accessing record data in memory.
   Derived class will be responsible for reading/writing data to data streams.
   Instances of Data_record objects are intended to exist for the current
   record in an input file.

   CGI post/query data often will have a single data record
   which constitutes a Data_record.

   INI files have one or more Data_records separated by section.

   Data set tables such as dBase typically have multiple Data_records.

   When creating a Data_record, the data record is bound to a file format class.
   The file format class is responsible for reading and writing data records.

   The Data_record class is used to associated named fields of the "In memory"
   representation of the data with the data stored or is to be stored in the
   data file/stream.

   Aliases are now provided for Data sources that have limits on the size of
   field names.
   Aliases are optional,
*/

#ifndef VV_entryH
#  include "corn/data_source/vv_entry.h"
#endif
#include "corn/container/enumlist.h"
#include "corn/container/text_list.h"

#define ARBITRARY_STRING_LENGTH_256 256
#define ARBITRARY_STRING_LENGTH_512 512
#include "corn/OS/directory_entry_name.h"
#include "corn/dynamic_array/dynamic_array_T.h"
namespace CORN {

// forward declarations
class Data_record;
class Indexer;                                                                   //150731
//______________________________________________________________________________
class VV_enumerated_section : public Item
{
   // In this case the section key is the section label prefix
   // Sections are numerated from 0 unless they are specified as one_based
   // derived enumerated_sections should have their own data list
   friend class Data_record;                                                     //010103
 protected:
   bool one_based;                                                               //981001
   std::string section_label_prefix;                                             //131008_980209
 public:
   nat16 count;                                                                  //120314_990128
 public:
   inline VV_enumerated_section                                                  //030119
      (const char *section_label_prefix_,bool one_based_=false)
      :Item()
      ,one_based(one_based_)                                                     //981008
      ,section_label_prefix(section_label_prefix_)                               //980209
      ,count(0)                                                               {} //120314
 public:
   virtual bool setup_structure(Data_record &data_rec,bool for_write) modification_;
   virtual bool setup_element_structure
      (Data_record &data_rec,bool for_write , int16 index)      modification_=0;
      // This is a pure method, any object using enumerated sections
      // must derive from this class and instanciate entries
   virtual nat32 get_container_element_count()                          const=0; //990105
      // The is a pure method, the derived class should return the number of
      // should return the number of element in the list.
   void set_count(nat16 new_count,bool unexpected_only);                         //120314_040323
   inline virtual bool is_key_string(const std::string &key)       affirmation_  //180820
      { return section_label_prefix == key; }

/*180820  was probably only used for find_cstr now using is_key

   inline virtual const char *get_key()                                    const //030119
      { return section_label_prefix.c_str();}
*/
   inline bool is_one_based()                         const { return one_based;} //010103
   inline void set_one_based(bool _one_based)        { one_based = _one_based; } //010103
};
//_VV_enumerated_section____________________________________________1997-09-29_/
class VV_container_enumerated_section : public VV_enumerated_section
{ // The is provided so programs can easily create enumerated sections for lists
 public:
   Container &container;                                                         //970930
 public:
   inline VV_container_enumerated_section
      (const char *_section_label_prefix, Container &_container,bool _one_based) //030117
      :VV_enumerated_section(_section_label_prefix,_one_based)
      ,container(_container)
      {}
 public:
   virtual bool setup_element_structure(Data_record &data_rec, bool for_write, int16 index) modification_; //010103
   inline virtual nat32 get_container_element_count()                      const //041006
      {  return container.count(); }
};
//_VV_container_enumerated_section__________________________________1997-09-29_/
// This macro is used to define VV_list_enumerated_section

#define DECLARE_CONTAINER_ENUMERATED_SECTION(section_class,section_class_section_VV,_one_based) \
   class section_class_section_VV : public CORN::VV_container_enumerated_section \
   {public:                                                                      \
      section_class_section_VV                                                   \
      (const char * _section_label_prefix, CORN::Container &_entries)            \
      :CORN::VV_container_enumerated_section(_section_label_prefix,_entries,_one_based) \
      {}                                                                         \
    public:                                                                      \
      virtual bool setup_element_structure(CORN::Data_record &data_rec ,bool for_write, int16 index) modification_ \
      {  bool result = CORN::VV_container_enumerated_section::setup_element_structure(data_rec,for_write,index); \
         if (!result)                                                            \
         {  section_class *entry  = new section_class;                           \
            result = entry->setup_structure(data_rec,for_write);                 \
            container.append(entry);                                             \
         }                                                                       \
         return result;                                                          \
      }                                                                          \
   }
//______________________________________________________________________________
#define EXPECT_ENUMERATED_SECTION(label,xxx_section_VV,container)             \
   {                                                                          \
      xxx_section_VV *container_section = new                                 \
      xxx_section_VV(label,container);                                        \
      expect_enumerated_section(container_section,for_write);                 \
   }
//______________________________________________________________________________
#define DATA_REC_EXPECT_ENUMERATED_SECTION(label,xxx_section_VV,container)    \
   {                                                                          \
      xxx_section_VV *container_section = new                                 \
      xxx_section_VV(label,container);                                        \
      data_rec.expect_enumerated_section(container_section,for_write);        \
   }
//______________________________________________________________________________
#define ANY_STRING_LENGTH 0x7FFF
class Data_record
{protected:
   std::string        primary_section_name;                                      //980407
   VV_Section        *current_section; // Just a reference, don't delete         //010104
   nat32             position;                                                   //010104
      // position can be used as needed by derived classes.
      // I.e.  VV_file does not make use of position.
      //   dBase data source uses position to store the record_number (1 based)
      //   delineated file makes used of position
 public :   // made public so programmers can easily inspect if needed
   Bidirectional_list sections;                                                  //011209
      ///< All sections including enumerated sections are stored in sections.
   Bidirectional_list enumerated_sections_definitions;                           //011209
 public :   // adjuncts
   mutable bool modified;                                                        //010113
   bool structure_defined;                                                       //010516
   mutable bool preserve_unexpected_entries;                                     //080820
   CORN::Text_list inherits;                                                     //170721
      // List of file which a data set may be inherited from.
 public : // Constructors
   Data_record
      (const char *primary_section                                               //030109
      ,bool one_based_enumerated_sections_obsolete = false);  // Don't use true, this is for CropSyst only
   inline virtual ~Data_record()
      {  delete_structure(); }
   inline virtual void delete_structure()                                        //010112
      {  sections.delete_all();                                                  //010112
         enumerated_sections_definitions.delete_all();
         structure_defined = false;                                              //010516
      }
   virtual bool has_structure()                                /*affirmation_*/; //010112
      // returns true if there are any sections or enumerated sections.
   bool was_section_encountered(const char *section_name)          affirmation_; //030117
      // returns true if the specified section was encountered
      // (should only be used after a get())
   bool was_entry_encountered
      (const char *section_name,const char *entry_name)            affirmation_; //050927
 public:  // The following forms are for using current section
   VV_string_entry     *expect_string
      (const char *key_, modifiable_ std::string & value_
      , nat16 _max_strlen=ANY_STRING_LENGTH
      , Phrase_label annotation1_=OPTIONAL_PHRASE
      ,Phrase_label annotation2_=OPTIONAL_PHRASE);                               //111111_050518

   VV_directory_entry_entry   *expect_directory_entry_name                       //000211
      (const char *key_
      , modifiable_ CORN::OS::Directory_entry_name & value_
      , nat16 _max_strlen=128, Phrase_label annotation1_=OPTIONAL_PHRASE
      ,Phrase_label annotation2_=OPTIONAL_PHRASE);
   #define expect_file_name expect_directory_entry_name
   #define expect_directory_name expect_directory_entry_name
#ifndef _MSC_VER
//100929 This is sort of deprecated because it is unsafe, it is currently only used in ClimGen.
   VV_char_array_entry *expect_char_array(const char *key_, modifiable_ char           * value_, nat16 _max_strlen,                Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
#endif
   VV_int8_entry       *expect_int8     (const char *key_, modifiable_ int8           & value_,                                     Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);  //980405
   VV_int16_entry      *expect_int16    (const char *key_, modifiable_ int16          & value_,                                     Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
   VV_int32_entry      *expect_int32    (const char *key_, modifiable_ int32          & value_,                                     Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
   VV_nat16_entry      *expect_nat16    (const char *key_, modifiable_ nat16         & value_, nat8 radix_,                      Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);     //980405
   VV_nat8_entry       *expect_nat8     (const char *key_, modifiable_ nat8          & value_, nat8 radix_,                      Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);     //980405
   VV_nat32_entry      *expect_nat32    (const char *key_, modifiable_ nat32         & value_, nat8 radix_,                      Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);     //980405

#define  expect_uint32 expect_nat32
#define  expect_uint16 expect_nat16
#define  expect_uint8  expect_nat8

   VV_float32_entry    *expect_float32   (const char *key_, modifiable_ float32        & value_,                                     Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
   VV_float64_entry    *expect_float64   (const char *key_, modifiable_ float64        & value_,                                     Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
   VV_bool_entry       *expect_bool      (const char *key_, modifiable_ bool           & value_, VV_bool_entry::Form _text_form = VV_bool_entry::form_true,    Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
     //050412 added formatted option (to allow tabular file formats which may have formatted data)

   virtual bool expect_switch                                                    //191104
      (bool &flag_value
      ,const char *long_form
      ,const char *short_form=0
      ,const char *caption1=0
      ,const char *caption2=0);

   // These following dates can probably take interfaces
   VV_date_entry       *expect_date      (const char *key, modifiable_ Date_clad_32      &value,bool formatted=false,Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE); //050412
   VV_datetime64_entry *expect_datetime64(const char *key, modifiable_ Date_time_clad_64 &value,bool formatted=false,Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE); //050412
   VV_datetime64_entry *expect_datetime64_formatted(const char *key, modifiable_ Date_time_clad_64 &value,const char *format,Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE); //050412

   VV_byte_entry       *expect_byte      (const char *key_, modifiable_ unsigned char  & value_,                                      Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);
   VV_enum_entry       *expect_enum      (const char *key_, modifiable_ Enum_abstract  &value_,                                       Phrase_label annotation1_=OPTIONAL_PHRASE,Phrase_label annotation2_=OPTIONAL_PHRASE);       //020318
   VV_units_entry      *expect_units     (const char *key_, modifiable_ Units_clad     &_units); // annotation not needed for units, will also confuse parser            //050413
      /**< expect_xxx methods define entries to be expected in the current section.
       <P>key is the variable label for the entry.
       <P>value is the value of the respective type.
       <P>annotation1 and annotation2 are optional comments or text that will be append to the entry line
       they can be char array constants or Phrase_label if using my CORN translator class
       for message cataloguing and/or foreign language translation.
       <P>label_table for enumerations should be a array of char array constants as in the following example
       <BR><LISTING>
       char *test_enumeration_labels[] =
       {"test0","test1","test2","test3","test4","test5","test6", 0 };
       </LISTING></BR>
       The last entry in the label table should be 0 (null pointer);
       It is strongly recommended that your label names match your enumeration declaration.
       CORN valid types can also be expected.
      **/
 protected:
   void expect_valid_number
      (const Valid_number &v_number,Bidirectional_list &section_entries);        //040414
 public:
  VV_int16_entry    *expect_valid_int16  (const Valid_int16    &v_int16);
  VV_int32_entry    *expect_valid_int32  (const Valid_int32    &v_int32);
  VV_float32_entry  *expect_valid_float32(const Valid_float32  &v_float32);
  VV_float64_entry  *expect_valid_float64(const Valid_float64  &v_float64);
 public:
   VV_parameter_entry *expect_parameter(Parameter_abstract &v_parameter_number); //090414
 public:
   virtual bool expect_structure(bool for_write);                                //161022_980405
   inline virtual bool get_preserve_unexpected_entries()                   const //080820
      { return preserve_unexpected_entries; }
   inline virtual bool set_preserve_unexpected_entries(bool enable)    mutation_ //090227
      {preserve_unexpected_entries = enable;return preserve_unexpected_entries;}
      // We now allow the derived class to determine if it want's                //020108
      // to preserve unexpected entries when reading, by default it does.
   virtual        bool get_start();                                              //161023_010109
   inline virtual bool get_end()   { return true;}                               //161023_010109
   inline virtual bool set_start() { return true;}                               //161023_010109
   inline virtual bool set_end()   { return true;}                               //161023_010109
   virtual nat16 resolve_directory_entry_names                                   //171204_151207
      (const CORN::OS::Directory_name &with_respect_to_dirname)           const; //160114

 public: // While these methods exist in Data_record, but they would not work for dBase files if the section_label,variable_code if very long
   void expect_bool_array     (const char *variable_or_section_label_, bool *array       , VV_bool_entry::Form    _text_form, bool one_based , nat16 max_index,bool enumerate_variable=false);                        //990519
   void expect_float32_array  (const char *variable_or_section_label_, float32 *array    , bool one_based , nat16 max_index,bool enumerate_variable=false);                                                            //981212
   void expect_float32_array_with_units_code_adjustable(const char *variable_or_section_label_, Units_clad &_units_code, float32 *array    , bool one_based , nat16 max_index,bool enumerate_variable=false);   //070403
   void expect_float32_array_with_units_code_fixed(const char *variable_or_section_label_,Units_code units_code,  float32 *array, bool one_based ,nat16 max_index,bool enumerate_variable);                           //070404
   void expect_dynamic_float32_array(const char *variable_or_section_label_,  CORN::Dynamic_array<float32> &array,const nat8 &array_size, bool one_based ,Units_code units_code,bool enumerate_variable);                           //160915
   void expect_float32_array_with_units_label(const char *variable_or_section_label_, const char *units_label, float32 *array    , bool one_based , nat16 max_index,bool enumerate_variable=false);                    //011129
   void expect_float64_array  (const char *variable_or_section_label_, float64 *array    , bool one_based , nat16 max_index,bool enumerate_variable=false);                                                            //030801
   void expect_int16_array    (const char *variable_or_section_label_, int16 *array      , bool one_based , nat16 max_index,bool enumerate_variable=false);                                                            //981212
   void expect_nat32_array    (const char *variable_or_section_label_, nat32 *array     , bool one_based , nat16 max_index, nat8 radix,bool enumerate_variable=false);                                               //000926
   void expect_int32_array    (const char *variable_or_section_label_, int32 *array     , bool one_based , nat16 max_index, bool enumerate_variable=false);                                               //000926
   void expect_string_array   (const char *variable_or_section_label_, std::string *array, nat16 _max_strlen, bool one_based , nat16 max_index,bool enumerate_variable=false);                                           //981212
      /**<  The expect_XXXX_array works in two modes
      enumerate section and enumerate variable
      enumerate section has the following form:
      [variable]
      0=xxxx
      1=xxxx
      :
      n=xxxx
      enumerate variable has the following form (for the current section)
      var_0=xxx
      var_1=xxx
      :
      var_n=xxx
      **/
   void expect_enumerated_section                                                //970926
      (VV_enumerated_section *enumerated_section,bool for_write);
      /**< Enumerated sections are used to simply creating lists of sections
      That usually correspond to arrays or lists of objects to be stored.
      A count of sections having a base name is placed in the primary section,
      and sections will be automatically numbered as in the following example:
      <LISTING>
      [primary section]    <- The primary section (whatever it is named).
      :
      xxx_count=3          <- Count of sections having the name prefix xxx.
      :
      [xxx_0]              <- Enumerated sections are 0 base indexed.
      :
      [xxx_1]
      :
      [xxx_2]
      :
      </LISTING>
      **/
   VV_Section *set_current_section(const char *new_current_section);             //010905
      /**< set_current_section defines a new section to be expected.
      // sections will be written in the order of set_current_section() calls.
      // set_current_section will not create multiple sections with the same name.
      // expect_xxx calls will put entries is the one section of the same name.
      // <P>I.e.
      // <LISTING>
      // set_current_section("A");
      // expect ...
      //   :
      // set_current_section("B");
      // expect ...
      //  :
      // set_current_section("A");
      // expect ...
      //  :
      // set_current_section("C");
      // expect ...
      //  :
      // </LISTING>
      // will create the following file structure:
      // [A]
      // :
      // [A]
      // :
      // [C]
      // :
      **/
//   VV_Section *set_current_enumerated_section(const char *new_current_section);    //120314
   VV_Section *get_current_section();                                            //010109
   void delete_section(const char *section);                                     //030117
   bool delete_entry(const char *section,const char *entry);                     //030117
      // Returns true if the entry was deleted.
   modifiable_ VV_abstract_entry *get_entry(const char *section,const char *entry); //111107
      // This is used to generically get entries
      // (currently used by Access::import/export tables
   inline virtual const char *get_primary_section_name()                   const //030109
      { return primary_section_name.c_str(); }                                   //030109
   //200802 inline virtual bool get_modified()                 const { return modified; } //deprecated, use is_modified()
   inline virtual bool is_modified()           affirmation_ { return modified; }
   inline virtual bool set_modified(bool _modified = true)         contribution_
      {modified = _modified; return modified;}
   inline virtual nat32 get_position()                       { return position;}
   inline virtual nat32 set_position(nat32 pos_)        { return position=pos_;}
   bool notify_indexer(Indexer &indexer);                                        //051108
private:
   Bidirectional_list units_labels; // 070403 Trying to replace this with units codes
   Enumeration_list units_codes;                                                 //070404
   char *enum_variable                                                           //030801
      (nat16 index,const char *variable,bool enumerate_variable)          const;
//200811 unimplemented   void remove_unencountered_sections();                                         //040208
private: // classes
   class Units_label_item : public Item                                          //011129
   {public:
      std::string units_label;                                                   //131008_040909
    public:
      inline Units_label_item(const char *_units_label)                          //030117
         : Item()
         , units_label(_units_label)
         {}
      virtual bool is_key_string(const std::string &key)           affirmation_; //180820
   };
   //_Units_label_item_________________________________________________________/
   class Units_code_item : public Item
   {public:
      Units_clad           units_code;
    public:
      inline Units_code_item(Units_code _units_code)
         : Item()
         , units_code(_units_code)                                            {}
      inline virtual nat32 get_key_nat32()     const { return units_code.get();}
   };
   //_Units_code_item_______________________________________________2007-04-04_/
};
//_Data_record_________________________________________________________________/
}//_namespace_CORN_____________________________________________________________/
#endif


