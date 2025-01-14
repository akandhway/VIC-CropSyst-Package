#ifndef BIFF_recordH
#define BIFF_recordH

#include "corn/format/binary/binary_record.h"
#include "corn/format/binary/binary_file_interface.h"
#include "corn/format/spreadsheet/biffcode.h"
#include "corn/container/unilist.h"
#include "corn/spreadsheet/cell.h"
#include "corn/data_type_enum.h"
#include "corn/container/item.h"
#include "corn/string/strconv.hpp"
#include <algorithm>

#define NOT_APPLICABLE 0
/*
The following classes use naming convention:
BIFF_lowercase       Generally applicable classes, usually interface, abstract, common or otherwise base class.
BIFF234578_rec_NAME  usually a class
   Represents an BIFF record the digits indicate the BIFF versions that use the record.
   The data members are usually listed in the same order as they are layed out
   in the BIFF record, but this is NOT NECESSARILY the case.
   The type of the data member is often but not necessarily the same
   binary format as stored in the file (I.e. strings have a different representation.
BIFF234578_layout_NAME  usually a struct
   Represents the actual layout of the BIFF record and will have the
   same structure and binary format as stored in the file.
*/
namespace CORN {
//______________________________________________________________________________
class BIFF_record
: public Binary_record
{
    bool     in_memory;
//             Some records have a fixed layout which can be written with one write.
//             For such records, the fixed layout data will be assigned by the derived class
//             Some records have no Body, in this case fixed layout and size will be 0
//    nat32   fixed_layout_size; just use length field
    void     *fixed_layout;    // This references body in derived classes, dont try to delete!
      // May be NULL if the record body length is variable in which case we must override write_body //100113
   inline virtual bool is_in_memory()                               affirmation_
   { return in_memory; };
 public:
    BIFF_record
    (Record_code record_type_code
    ,nat32 _record_body_length
    ,void *fixed_layout = 0  // 0 if variable length
    );
    virtual void write_body(Binary_file_interface &out_file);
    virtual bool read_body(Binary_file_interface &in_file);
};
//______________________________________________________________________________
struct RichText_format      // Note a record, but a subrecord block
{  // In BIFF 23457 only 8 bit integers are written
   nat16  first_formatted_character; // First formatted character (zero-based)
   nat16  FONT_record_index;
 public:
   bool read (Binary_file_interface &in_file ,Data_type data_type);
   bool write(Binary_file_interface &out_file,Data_type data_type);
};
//______________________________________________________________________________
class BIFF23457_string : public std::string
{
 public:
    bool read (Binary_file_interface &in_file ,String_format string_format);
    bool write(Binary_file_interface &out_file,String_format string_format);
};
//______________________________________________________________________________
class BIFF8_string : public std::wstring
{
    class Asian_phonetic_settings_block
    {
               public:
                  struct NP
                  {
                     nat16 phonetic_text_first_char;
                     nat16 main_text_first_char;
                     nat16 main_text_char_count;
                  };
               public:
                  nat16 unknown_identifier;
                  nat16 datasize;
                  nat16 FONT_record_index; // used for Asian phonetic text
                  nat16 settings;
                  nat16 portions;
                  nat16 character_count;
                  NP    *NP_list;
                  // NYI
       bool read (Binary_file_interface &in_file /*,Data_type character_count_data_type*/);
       bool write(Binary_file_interface &out_file/*,Data_type character_count_data_type*/);
      // NYI nat32 compute_size() const;
    };
 public:

    // character count is from the wstring length
    nat8  options;
    nat16 rich_text_format_count;
    nat32 asian_phonetic_settings_block_size;
    // the character array comes next in the file
    RichText_format  *rich_text_formats;
    Asian_phonetic_settings_block *asian_phonetic_settings_block;
            private:
               bool expect_RTF; bool expect_asian_phonetic_settings;
 public:
    BIFF8_string(const std::wstring &initial_value=L"",bool _expect_RTF=true, bool _expect_asian_phonetic_settings=true)
       : std::wstring(initial_value)
       , expect_RTF(_expect_RTF)
       , expect_asian_phonetic_settings(_expect_asian_phonetic_settings)
       , rich_text_formats(0)
       , asian_phonetic_settings_block(0)
       {};
    virtual ~BIFF8_string();
    bool read (Binary_file_interface &in_file ,Data_type character_count_data_type);
    bool write(Binary_file_interface &out_file,Data_type character_count_data_type);
    nat32 compute_size() const;
};
//______________________________________________________________________________
struct BIFF_cell_layout
{
   nat16   row;   // 0-based
   nat16   col;   // 0-based
};
//______________________________________________________________________________
class BIFF_cell_record_variable
: public BIFF_record
{  // This is pure derived for cell records
public:
   nat16   row;   // 0-based
   nat16   col;   // 0-based
public:
    inline BIFF_cell_record_variable
    (Record_code _record_code
    ,nat32  _record_body_length
    ,nat16  _row,nat16 _col
    )
    :BIFF_record(_record_code,_record_body_length,0) // write body provided
    ,row(_row)
    ,col(_col)
    {};
    virtual void write_body(Binary_file_interface  &out_file);
    virtual bool read_body(Binary_file_interface  &in_file);

    inline nat16   get_row() const { return row; };   // 0-based
    inline nat16   get_col() const { return col; };   // 0-based
};
//______________________________________________________________________________
struct BIFF_attribute_cell_layout
: public BIFF_cell_layout
{
   nat8    attribute_0; //
   nat8    attribute_1; //
   nat8    attribute_2; //
};
//______________________________________________________________________________
class BIFF_attribute_cell_record_variable
: public BIFF_cell_record_variable
{  // This is pure derived for old style cell data records
public:
   nat8    attribute_0; //
   nat8    attribute_1; //
   nat8    attribute_2; //
   inline BIFF_attribute_cell_record_variable
    (Record_code _record_code
    ,nat32  _record_body_length
    ,nat16  _row,nat16 _col
    ,nat8   _attribute_0
    ,nat8   _attribute_1
    ,nat8   _attribute_2)
    :BIFF_cell_record_variable(_record_code,_record_body_length,_row,_col)
    ,attribute_0(_attribute_0)
    ,attribute_1(_attribute_1)
    ,attribute_2(_attribute_2)
    {};
    virtual void write_body(Binary_file_interface  &out_file);
};
//______________________________________________________________________________
class BIFF_XF_cell_layout
: public BIFF_cell_layout
{  // This is pure derived for new style cell records (I.e. RK)
  public:
   nat16   ixfe; // Index to XF record.
};
//______________________________________________________________________________
class BIFF_XF_cell_record_variable
: public BIFF_cell_record_variable
{  // This is pure derived for new style cell records (I.e. RK)
 protected:
   nat16   ixfe; // Index to XF record.
 public:
   inline BIFF_XF_cell_record_variable
      (Record_code _record_code
      ,nat32 _record_body_length
      ,nat16  _row,nat16 _col
      ,nat16  _ixfe)
      :BIFF_cell_record_variable(_record_code,_record_body_length,_row,_col)
      ,ixfe(_ixfe)
      {};
   virtual void write_body(Binary_file_interface  &out_file);
   virtual bool read_body(Binary_file_interface &in_file)     ;
};
//______________________________________________________________________________
// BOF records for version 1-4 had no body
class BIFF_rec_BOF     // common
: public BIFF_record
{
 public:
   enum Substream_type
   { // Note these enumerations correspond to dt (stream type field) of BOF record in BIFF5>.
     WORKBOOK_GLOBALS      = 0x0005
   , VISUAL_BASIC_MODULE   = 0x0006
   , WORKSHEET             = 0x0010
   , CHART                 = 0x0020
   , EXCEL_4_MACRO_SHEET   = 0x0040
   , WORKSPACE             = 0x0100};
 public:
   inline BIFF_rec_BOF
      (Record_code BOF_rec_head_code
      ,nat32 _record_body_length = 0
      ,void *_fixed_layout = 0)
      : BIFF_record(BOF_rec_head_code,_record_body_length,_fixed_layout)
      {};
   virtual BIFF_Format get_format() const = 0;                                   //100123
};
//______________________________________________________________________________
struct BIFF2_layout_BOF
{  nat16            vers;  // version_number
       // present, but unused in BIFF2,BIFF3,BIFF4 I presume this could be set to 0x0200,0x0300,0x0400 respectively
       // 0x0500 for BIFF 5 and 7
       // 0x0600 for BIFF8
   nat16            dt;         // BIFF5> Substream_type
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF4_layout_BOF
: public BIFF2_layout_BOF
{  nat16            unknown;   // BIFF7> Build identifier  (=0x0DBB for MS Excel 97)
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF2_rec_BOF
: public BIFF_rec_BOF
{  BIFF2_layout_BOF  body;
 public:
   inline BIFF2_rec_BOF()
      : BIFF_rec_BOF(BIFF2_CODE_BOF,4,&body)
      {
          body.vers      = 0x0000;
          body.dt        = 0x09;
      };
    inline virtual BIFF_Format get_format() const {return BIFF2; }               //100123
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF34_rec_BOF
: public BIFF_rec_BOF
{
    BIFF4_layout_BOF  body;
 public:
   inline BIFF34_rec_BOF
   (BIFF_Format format
   ,BIFF_rec_BOF::Substream_type _substream_type  = WORKBOOK_GLOBALS)
   : BIFF_rec_BOF(BIFF4?BIFF4_CODE_BOF:BIFF3_CODE_BOF,6,&body)
   {
       body.vers      = 0x0000;
       body.dt        = (nat16)_substream_type;
       body.unknown  = 0x0000;
   };
    inline virtual BIFF_Format get_format() const {return rec_code == BIFF4_CODE_BOF ? BIFF4 : BIFF3; }   //100123
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF57_layout_BOF
: public BIFF2_layout_BOF
{
   nat16            rupBuild;   // BIFF7> Build identifier  (=0x0DBB for MS Excel 97)
   nat16            rupYear;    // BIFF7> Build year        (=0x07CC for MS Excel 97)
};
//______________________________________________________________________________
class BIFF57_rec_BOF
: public BIFF_rec_BOF
{
    BIFF57_layout_BOF  body;
public:
   inline BIFF57_rec_BOF
   ( bool workbook_globals = false
   ,Substream_type _substream_type  = WORKBOOK_GLOBALS
   )
   : BIFF_rec_BOF(BIFF578_CODE_BOF,sizeof(BIFF57_layout_BOF),&body)
   {
       body.vers      = workbook_globals ? 0x0500 : 0x0000;
       body.dt        = (nat16)_substream_type;
       body.rupBuild  = 0x126B;
       body.rupYear   = 0x07CC;
   };
    inline virtual BIFF_Format get_format() const {return body.vers == 0x0500 ? BIFF5 : BIFF7; } //100123
    // It is not exactly clear how to distinguish BIFF7
    // (I definitely know it is not body.vers == 0x0700 because 0x0600 is BIFF8)
};
//______________________________________________________________________________
struct BIFF8_layout_BOF
: public BIFF57_layout_BOF
{
   nat32            bfh;        // BIFF8> file history flags
   nat32            sfo;        // BIFF8> lowest BIFF version that can read all the record in this file
};
//______________________________________________________________________________
class BIFF8_rec_BOF
: public BIFF_rec_BOF
{
    BIFF8_layout_BOF  body;
  // Note it should be safe to use BIFF8_rec_BOF
   // Even though it may be a biff5 or biff7 record
public:
    inline BIFF8_rec_BOF
    (
    Substream_type _substream_type = BIFF_rec_BOF::WORKBOOK_GLOBALS
    )
    : BIFF_rec_BOF(BIFF578_CODE_BOF,sizeof(BIFF8_layout_BOF),&body)
    {
       body.vers      = 0x0600;
       body.dt        = (nat16)_substream_type;
       body.rupBuild  = 0x0DBB;
       body.rupYear   = 0x07CC;
       body.bfh       = 0xC1; // last edited on Windows
       body.sfo       = 0x06; // earliest version of Excel that can read all the records in this file.
    };
    inline virtual BIFF_Format get_format()                                const //100123
       {return /* body.vers == 0x0600 ? BIFF8 : */ BIFF8; }
       // I presume later versions will have values greater than 0x0600 but not available as of this date
};
//______________________________________________________________________________
class BIFF_rec_DIMENSIONS
: public BIFF_record
{   // pure
public:
    inline BIFF_rec_DIMENSIONS(Record_code dimensions_code
    ,nat32 _record_body_length
    ,void *_body)
    : BIFF_record(dimensions_code,_record_body_length,_body)
    { };

    virtual void redimension(nat16 col, nat32 row)              = 0; // PURE
/*100110*/     virtual nat32 get_first_define_row()                        const = 0;
/*100110*/     virtual nat32 get_first_define_column()                     const = 0;
/*100110*/     virtual nat32 get_last_define_row()                         const = 0;
/*100110*/     virtual nat32 get_last_define_column()                      const = 0;
};
//______________________________________________________________________________
// BIFF2_rec_DIMENSIONS format is unknown
struct BIFF2_layout_DIMENSIONS
{
   nat16   rwMic;      // First defined row on the sheet
                        // in BIFF8, nat32 stored for all other versions nat16 is written
   nat16   rwMac;      // Last defined row on the sheet, plus 1
                        // in BIFF8, nat32 stored for all other versions nat16 is written
   nat16   colMic;     // First defined column on the sheet
   nat16   colMac;     // Last define column on the sheet, plus 1
};
//______________________________________________________________________________
class BIFF2_rec_DIMENSIONS
: public BIFF_rec_DIMENSIONS
{
protected:
   BIFF2_layout_DIMENSIONS body;
public:
    inline BIFF2_rec_DIMENSIONS()
    : BIFF_rec_DIMENSIONS(BIFF2_CODE_DIMENSIONS,8,&body)
    {
       body.rwMic =0;
       body.rwMac =0;
       body.colMic =0;
       body.colMac =0;
    };
    inline virtual void redimension(nat16 col, nat32 row)
    {
       body.rwMic = std::min<nat16>(body.rwMic,(nat16)row);
       body.rwMac = std::max<nat16>(body.rwMac,(nat16)row);
       body.colMic = std::min<nat16>(body.colMic,(nat16)col);
       body.colMac = std::max<nat16>(body.colMac,(nat16)col);
    };
    inline virtual nat32 get_first_define_row()                            const { return body.rwMic; };  //100110
    inline virtual nat32 get_first_define_column()                         const { return body.rwMac; };  //100110
    inline virtual nat32 get_last_define_row()                             const { return body.colMic; }; //100110
    inline virtual nat32 get_last_define_column()                          const { return body.colMac; }; //100110
};
//______________________________________________________________________________
struct BIFF3457_layout_DIMENSIONS
{
   nat16   rwMic;      // First defined row on the sheet.    In BIFF8, nat32 stored for all other versions nat16 is written
   nat16   rwMac;      // Last defined row on the sheet, plus 1. In BIFF8, nat32 stored for all other versions nat16 is written
   nat16   colMic;     // First defined column on the sheet
   nat16   colMac;     // Last define column on the sheet, plus 1
   nat16   reserved;   // Reserved, must be 0
};
//______________________________________________________________________________
class BIFF3457_rec_DIMENSIONS
: public BIFF_rec_DIMENSIONS
{
protected:
   BIFF3457_layout_DIMENSIONS body;
public:
   inline BIFF3457_rec_DIMENSIONS()
      : BIFF_rec_DIMENSIONS(BIFF34578_CODE_DIMENSIONS,sizeof(BIFF3457_layout_DIMENSIONS),&body)
      {
         body.rwMic =0;
         body.rwMac =0;
         body.colMic =0;
         body.colMac =0;
         body.reserved =0;
      };
    inline virtual void redimension(nat16 col, nat32 row)
      {
         body.rwMic = std::min<nat16>(body.rwMic,(nat16)row);
         body.rwMac = std::max<nat16>(body.rwMac,(nat16)row);
         body.colMic = std::min<nat16>(body.colMic,(nat16)col);
         body.colMac = std::max<nat16>(body.colMac,(nat16)col);
      };
    inline virtual nat32 get_first_define_row()                const { return body.rwMic; };  //100110
    inline virtual nat32 get_first_define_column()             const { return body.rwMac; };  //100110
    inline virtual nat32 get_last_define_row()                 const { return body.colMic; }; //100110
    inline virtual nat32 get_last_define_column()              const { return body.colMac; }; //100110
};
//______________________________________________________________________________
struct BIFF8_layout_DIMENSIONS
{
   nat32   rwMic;      // First defined row on the sheet. in BIFF8, nat32 stored for all other versions nat16 is written
   nat32   rwMac;      // Last defined row on the sheet, plus 1. in BIFF8, nat32 stored for all other versions nat16 is written
   nat16   colMic;     // First defined column on the sheet
   nat16   colMac;     // Last define column on the sheet, plus 1
   nat16   reserved;   // Reserved, must be 0
};
class BIFF8_rec_DIMENSIONS
: public BIFF_rec_DIMENSIONS
{
protected:
   BIFF8_layout_DIMENSIONS body;
public:
    inline BIFF8_rec_DIMENSIONS()
    : BIFF_rec_DIMENSIONS(BIFF34578_CODE_DIMENSIONS,14,&body)
    {
       body.rwMic =0;
       body.rwMac =0;
       body.colMic =0;
       body.colMac =0;
       body.reserved =0;
    };
    inline virtual void redimension(nat16 col, nat32 row)
    {
       body.rwMic = std::min<nat32>(body.rwMic,row);
       body.rwMac = std::max<nat32>(body.rwMac,row);
       body.colMic = std::min<nat32>(body.colMic,(nat16)col);
       body.colMac = std::max<nat32>(body.colMac,(nat16)col);
    };
    inline virtual nat32 get_first_define_row()                const { return body.rwMic; };  //100110
    inline virtual nat32 get_first_define_column()             const { return body.rwMac; };  //100110
    inline virtual nat32 get_last_define_row()                 const { return body.colMic; }; //100110
    inline virtual nat32 get_last_define_column()              const { return body.colMac; }; //100110
};
//______________________________________________________________________________
//NYI class BIFF_rec_BLANK: public BIFF_record {                            0x01
//______________________________________________________________________________
struct BIFF2_layout_INTEGER
: public BIFF_attribute_cell_layout
{
   int16      value;      //  BIFF2 only had 16bit integers
};
//______________________________________________________________________________
class BIFF2_rec_INTEGER
            : public BIFF_record
//: public BIFF_attribute_cell_record
/*100113*/ , public Cell_numeric // interface
{//                          0x02
//   int16      value;      //  BIFF2 only had 16bit integers
               BIFF2_layout_INTEGER body;
public:
    inline BIFF2_rec_INTEGER
    (nat16  _row =0 ,nat16 _col =0
    ,nat8   _attribute_0 =0
    ,nat8   _attribute_1 =0
    ,nat8   _attribute_2 =0
    ,int16  _value =0
    )
//    : BIFF_attribute_cell_record(BIFF2_CODE_INTEGER,BIFF_LEN_INTEGER,_row, _col,_attribute_0,_attribute_1,_attribute_2)
//    ,value(_value)
    : BIFF_record(BIFF2_CODE_INTEGER,BIFF_LEN_INTEGER,&body)
    {
       body.row = _row;
       body.col = _col;
       body.attribute_0 = _attribute_0;
       body.attribute_1 = _attribute_1;
       body.attribute_2 = _attribute_2;
       body.value = _value;
    };
//    virtual void write_body(Binary_file_interface  &out_file);
    inline virtual sint16 set_value(sint16 _value)           modification_ { return body.value = _value; };
    inline virtual sint16 get_value()                           const  { return body.value; };
/*100111*/  protected:
    virtual nat32  get_row()                                   const { return body.row; };
    virtual nat32  get_col()                                   const { return body.col; }; // Note: Excel currently only uses nat16  for columns
/*100111*/     inline virtual int16   set_int16 (int16  _value)         modification_ { return body.value = _value;};
                      // since we can only store 32 bit integers, we must cast down.
/*100111*/     inline virtual int32   set_int32 (int32  _value)         modification_ { return (int32)set_int16(_value);};
/*100111*/     inline virtual float32 set_float32(float32 _value)       modification_ { return set_int16((int16)_value);}; // should not be used
/*100111*/     inline virtual float64 set_float64(float64 _value)       modification_ { return set_int16((int16)_value);};  // should not be used
   inline virtual int16   get_int16()                          const  { return body.value; };
/*100111*/     inline virtual int32   get_int32()                          const  { return get_int16();};
/*100111*/     inline virtual float32 get_float32()                        const  { return (float32)body.value ;};
/*100111*/     inline virtual float64 get_float64()                        const  { return (float64)body.value ;};
};
//______________________________________________________________________________
struct BIFF2_layout_NUMBER
: public BIFF_attribute_cell_layout
{
   int16      value;      //  BIFF2 only had 16bit integers
};
//______________________________________________________________________________
class BIFF2_rec_NUMBER
: public BIFF_record
, public Cell_numeric // interface                                                           //100113
{ //                          0x03
               BIFF2_layout_NUMBER body;
//    float64 value;
public:
    inline BIFF2_rec_NUMBER
    (nat16  _row = 0,nat16 _col =0
    ,nat8   _attribute_0 =0
    ,nat8   _attribute_1 =0
    ,nat8   _attribute_2 =0
    ,float64  _value =0
    )
//    : BIFF_attribute_cell_record(BIFF2_CODE_NUMBER,BIFF2_LEN_NUMBER,_row, _col,_attribute_0,_attribute_1,_attribute_2)
//    ,value(_value)
    : BIFF_record(BIFF2_CODE_NUMBER,BIFF2_LEN_NUMBER,&body)
    {
       body.row = _row;
       body.col = _col;
       body.attribute_0 = _attribute_0;
       body.attribute_1 = _attribute_1;
       body.attribute_2 = _attribute_2;
       body.value = _value;
    };
// using layout mode     virtual void write_body(Binary_file_interface  &out_file);
    inline virtual float64 set_value(float64 _value)                modification_{ return body.value = _value; };
    inline virtual float64 get_value()                                     const { return body.value; };
 protected:
    virtual nat32  get_row()                                               const { return body.row; };
    virtual nat32  get_col()                                               const { return body.col; }; // Note: Excel currently only uses nat16  for columns
    inline virtual int32 set_int32(int32   _value)                  modification_ { return body.value = (float64)_value;};
    inline virtual float64 set_float64(float64 _value)              modification_ { return body.value = (float64)_value;};
    inline virtual int32 get_int32()                                       const  { return (int32) body.value;};
    inline virtual float64 get_float64()                                   const  { return (float64)body.value;};
};
//______________________________________________________________________________
struct BIFF34578_layout_NUMBER
: public BIFF_XF_cell_layout
{
   float64 value;
};
//______________________________________________________________________________
class BIFF34578_rec_NUMBER
: public BIFF_record
, public Cell_numeric // interface                                               //100113
{
 public:
   BIFF34578_layout_NUMBER body;
//
    inline BIFF34578_rec_NUMBER
    (nat16  _row = 0,nat16 _col =0
    ,nat16  _ixfe = 0
    ,float64 _value =0
    )
//    : BIFF_XF_cell_record(BIFF2_CODE_NUMBER,BIFF34578_LEN_NUMBER,_row, _col, _ixfe)
//    ,value(_value)
//    {};
               : BIFF_record(BIFF34578_CODE_NUMBER,BIFF34578_LEN_NUMBER,&body)
    {
       body.row = _row;
       body.col = _col;
       body.ixfe = _ixfe;
       body.value = _value;
    };
//    virtual void write_body(Binary_file_interface &out_file);
/*100810*/  public:


               // Need read_body and write body

    inline virtual float64 set_value(float64 _value)         modification_{ return body.value = _value; };
    inline virtual float64 get_value()                          const { return body.value; };
/*100111*/  protected:
    virtual nat32  get_row()                                    const { return body.row; };
    virtual nat32  get_col()                                    const { return body.col; }; // Note: Excel currently only uses nat16  for columns
    inline virtual int16  set_int16(int16   _value)          modification_ { return body.value = (float64)_value;};
    inline virtual int32  set_int32(int32   _value)          modification_ { return body.value = (float64)_value;};
    inline virtual float64 set_float64(float64 _value)       modification_ { return body.value = (float64)_value;};
    inline virtual float32 set_float32(float32 _value)       modification_ { return body.value = (float64)_value;};
    inline virtual int16  get_int16()                           const  { return (int16) body.value;};
    inline virtual int32 get_int32()                            const  { return (int32) body.value;};
    inline virtual float32 get_float32()                        const  { return (float32)body.value;};
    inline virtual float64 get_float64()                        const  { return (float64)body.value;};
 public:
/*100810*/     virtual void write_body(Binary_file_interface &out_file);
/*100810*/     virtual bool read_body(Binary_file_interface &in_file);
};
//______________________________________________________________________________
class BIFF23457_rec_LABEL_common
: public Cell_textual                                                                        //100113
{protected:
    std::string value;
 public:
   inline BIFF23457_rec_LABEL_common(const std::string &_value="") :
                     value(_value) {};
   virtual const std::string  & set_string  (const std::string  &_value) modification_ { value.assign(_value); return _value; };
   virtual const std::wstring & set_wstring (const std::wstring &_value) modification_ { wstring_to_string(_value,value); return _value; };
   virtual std::string  & get_string  (      std::string  &_value)         const { _value.assign(value); return _value; };
   virtual std::wstring & get_wstring (      std::wstring &_value)         const { string_to_wstring(value,_value); return _value; };
};
//______________________________________________________________________________
class BIFF2_rec_LABEL
: public BIFF_attribute_cell_record_variable
, public BIFF23457_rec_LABEL_common
{ //                           0x04
public:
   inline BIFF2_rec_LABEL
    (nat16  _row=0,nat16 _col =0
    ,nat8   _attribute_0 =0
    ,nat8   _attribute_1=0
    ,nat8   _attribute_2=0
    ,const std::string  &_value  =""
    )
    : BIFF_attribute_cell_record_variable(BIFF2_CODE_LABEL,BINREC_VARIABLE_LENGTH,_row, _col,_attribute_0,_attribute_1,_attribute_2)
    ,BIFF23457_rec_LABEL_common(_value)
    {};
    virtual void write_body(Binary_file_interface  &out_file);
    virtual nat32 compute_record_body_size()                               const;//001008
 public:  // Cell_textual implementation
   virtual nat32  get_row()                                                const { return row; };
   virtual nat32  get_col()                                                const { return col; }; // Note: Excel currently only uses nat16  for columns
};
//______________________________________________________________________________
class BIFF3457_rec_LABEL
: public BIFF_XF_cell_record_variable
, public BIFF23457_rec_LABEL_common
{ /* LABEL - Cell Value, String Constant */
public:
    inline BIFF3457_rec_LABEL
    (nat16  _row = 0,nat16 _col =0
    ,nat16  _ixfe =0
    ,const std::string  &_value = ""
    )
    : BIFF_XF_cell_record_variable(BIFF34578_CODE_LABEL,BINREC_VARIABLE_LENGTH,_row, _col,_ixfe)
//    ,value(_value)
               , BIFF23457_rec_LABEL_common(_value)
    {};
    virtual void write_body(Binary_file_interface  &out_file);
    virtual nat32 compute_record_body_size()                               const;//001008
 public: // Cell_textual implementation
    virtual nat32  get_row()                                               const { return row; };
    virtual nat32  get_col()                                               const { return col; }; // Note: Excel currently only uses nat16  for columns
};
//______________________________________________________________________________
class BIFF8_rec_LABEL
: public BIFF_XF_cell_record_variable
, public Cell_textual                                                            //100113
{ //                           0x204 /* LABEL - Cell Value, String Constant */
    BIFF8_string value;
public:
   inline BIFF8_rec_LABEL
    (nat16  _row=0, nat16 _col=0
    ,nat16  _ixfe=0
    ,const std::wstring  &_value =L"")
    : BIFF_XF_cell_record_variable(BIFF34578_CODE_LABEL,BINREC_VARIABLE_LENGTH,_row, _col,_ixfe)
    ,value(_value,false,false)
    {};
   virtual ~BIFF8_rec_LABEL() {};
   virtual void write_body(Binary_file_interface  &out_file) { /* NYI may need to transform wstring to Excel string */ };
   virtual nat32 compute_record_body_size()                                const;//001008
 // Cell_textual implementation
 public:
    virtual nat32  get_row()                                               const { return row; };
    virtual nat32  get_col()                                               const { return col; }; // Note: Excel currently only uses nat16  for columns
    inline virtual const std::string  & set_string  (const std::string &_value)  modification_ { string_to_wstring(_value,value); return _value; } ;
    inline virtual const std::wstring & set_wstring (const std::wstring &_value) modification_ { value.assign(_value); return value; } ;
    inline virtual std::string  & get_string  (std::string &_value)        const { wstring_to_string(value,_value);  return _value; } ;
    inline virtual std::wstring & get_wstring (std::wstring &_value)       const { _value.assign(value); return _value; } ;
};
//______________________________________________________________________________
//NYI class BIFF_rec_BOOLERR     : public BIFF_record {                          0x05
//______________________________________________________________________________
/* For the formula option flags:
Bit
 0  Recalculate always  (introduced in BIFF2)
 1  Calculate on open (introduced in BIFF3)
 2  <undefined>
 3  Part of a shared formula (Introduced in BIFF8)
*/
/*
struct BIFF34_layout_FORMULA
:public BIFF34578_layout_NUMBER
{
};
*/
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF34578_rec_FORMULA_common
: public  BIFF34578_rec_NUMBER
{protected:
   nat16 options;
   std::string RPN_token_array;
   union FORMULA_Result_union
   {  float64 value;
      struct Other_result
      {  nat8 identifier;
         uint8 unused_1;
         uint8 value;    // if boolean   or error code
         char unused_2[3];
         nat32 FFFF;
      } non_numeric;
   } formula_result;
public:
   inline BIFF34578_rec_FORMULA_common(nat32 _rec_length)
      {  rec_length = _rec_length; };
   Data_type get_result_data_type()                                        const;
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   virtual nat32 get_token_array_byte_count()                              const = 0;
   inline virtual bool is_string()                                         const { return formula_result.non_numeric.identifier == 0x00; };
};
#ifdef NYI
class BIFF2_rec_FORMULA
: public BIFF_record
{

};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _

class BIFF34_rec_FORMULA
: public BIFF34578_rec_FORMULA_common
{
public:
   BIFF34_rec_FORMULA(nat32 record_length)
   : BIFF34578_rec_FORMULA_common(record_length)
   {
      rec_code =  BIFF34_CODE_FORMULA
   }
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   inline virtual nat32 get_token_array_byte_count() const { return rec_length - (6 + sizeof(float64)+ sizeof(options)); };
};
#endif
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF578_rec_FORMULA
: public BIFF34578_rec_FORMULA_common
{
   nat32 unused;     // Unused only appears in BIFF8
public:
   BIFF578_rec_FORMULA(nat32 record_length)
   : BIFF34578_rec_FORMULA_common(record_length)
      {  rec_code =  BIFF2578_CODE_FORMULA; };
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   inline virtual nat32 get_token_array_byte_count()                       const
      { return rec_length - (6 + sizeof(float64) + sizeof(options) + sizeof(unused)); };
};
//______________________________________________________________________________
class BIFF2_rec_STRING
: public BIFF_record
{
public:
   nat8  char_count;
   std::string text;
public:
   inline BIFF2_rec_STRING()
      : BIFF_record(BIFF2_CODE_STRING,BINREC_VARIABLE_LENGTH)
      {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   virtual nat32 compute_record_body_size()                                const { return 1 + text.length(); };
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF3457_rec_STRING
: public BIFF_record
{
public:
   nat16 char_count;
   std::string text;
public:
   inline BIFF3457_rec_STRING()
      : BIFF_record(BIFF34578_CODE_STRING,BINREC_VARIABLE_LENGTH)
      {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   virtual nat32 compute_record_body_size() const { return 2 + text.length(); };
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF8_rec_STRING
: public BIFF_record
{
public:
   BIFF8_string text;
public:
   inline BIFF8_rec_STRING()
      : BIFF_record(BIFF34578_CODE_STRING,BINREC_VARIABLE_LENGTH)
      , text(L"",false,false)
      {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   virtual nat32 compute_record_body_size() const { return 2 + text.compute_size(); };
};
//______________________________________________________________________________
class BIFF_rec_EOF
: public BIFF_record
{ //                           0x0A
public:
   inline BIFF_rec_EOF()
   : BIFF_record(BIFF234578_CODE_EOF,BIFF_LEN_EOF,0)
   {};
};
//______________________________________________________________________________
#ifdef NYI
class BIFF_rec_INDEX
: public BIFF_record
{   //                       0x0B
};
//______________________________________________________________________________
class BIFF4_rec_INDEX
: public BIFF_record
{   //                  0x20B /* INDEX - Index Record */
};
//______________________________________________________________________________
//struct BIFF7_layout_INDEX // May be the same as BIFF4
//{
//};
class BIFF7_rec_INDEX
: public BIFF_rec_INDEX
{
   nat32   reserved_a;    // all 0
   nat16   rwMic;         // first row that exists on the sheet (I.e. 0)
   nat16   rwMac;         // last row that exists on the sheet (I.e. C = 3);
   nat32   reserved_b;
   nat32   dbcell_offsets[1000]; // WARNING this number may need to be expanded
   uint26   dbcell_counts;
};
//______________________________________________________________________________
#endif

//NYI class BIFF_rec_CALCCOUNT: public BIFF_record {                      0x0C
//NYI class BIFF_rec_CALCMODE: public BIFF_record {                       0x0D
//NYI class BIFF_rec_PRECISION: public BIFF_record {                      0x0E
//NYI class BIFF_rec_REFMODE: public BIFF_record {                        0x0F
//NYI class BIFF_rec_DELTA: public BIFF_record {                          0x10
//NYI class BIFF_rec_ITERATION: public BIFF_record {                      0x11
//NYI class BIFF_rec_PROTECT: public BIFF_record {                        0x12
//NYI class BIFF_rec_PASSWORD: public BIFF_record {                       0x13
//NYI class BIFF_rec_HEADER: public BIFF_record {                         0x14
//NYI class BIFF_rec_FOOTER: public BIFF_record {                         0x15
//NYI class BIFF_rec_EXTERNCOUNT: public BIFF_record {                    0x16
//NYI class BIFF_rec_EXTERNSHEET: public BIFF_record {                    0x17
//NYI class BIFF_rec_NAME: public BIFF_record {                           0x18  0x218
//NYI class BIFF_rec_WINDOW_PROTECT: public BIFF_record {                 0x19
//NYI class BIFF_rec_VERTICAL_PAGE_BREAKS: public BIFF_record {           0x1A  /* VERTICALPAGEBREAKS - Explicit Column Page Breaks */
//NYI class BIFF_rec_HORIZONTAL_PAGE_BREAKS: public BIFF_record {         0x1B /* HORIZONTALPAGEBREAKS - Explicit Row Page Breaks */
//NYI class BIFF_rec_NOTE: public BIFF_record {                           0x1C


//NYI class BIFF_rec_FORMAT: public BIFF_record {                         0x1E
//NYI class BIFF_rec_FORMATCOUNT: public BIFF_record {                    0x1F
//NYI class BIFF_rec_COLUMN_DEFAULT: public BIFF_record {                 0x20
//NYI class BIFF_rec_ARRAY: public BIFF_record {                          0x21
//______________________________________________________________________________
#ifdef OBSOLETE
struct BIFF234578_layout_1904
{
   nat16 f1904;
};
#endif
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF234578_rec_1904
: public BIFF_record
{
//   BIFF234578_layout_1904 body;
   nat16 f1904;
public:
   inline BIFF234578_rec_1904()
   : BIFF_record(BIFF234578_CODE_1904,2)
   , f1904(0)
   {};
   inline bool is_data_mode_1904()      const { return f1904; };
   inline virtual void write_body(Binary_file_interface &out_file) { out_file.write_nat16(f1904); };
   inline virtual bool read_body(Binary_file_interface &in_file)   { f1904 = in_file.read_nat16(); return true; };

};
//______________________________________________________________________________
//NYI class BIFF_rec_EXTERNNAME: public BIFF_record {                     0x23
//NYI class BIFF_rec_COLWIDTH: public BIFF_record {                       0x24
//NYI class BIFF_rec_DEFAULT_ROW_HEIGHT: public BIFF_record {             0x25
//NYI class BIFF_rec_LEFT_MARGIN: public BIFF_record {                    0x26
//NYI class BIFF_rec_RIGHT_MARGIN: public BIFF_record {                   0x27
//NYI class BIFF_rec_TOP_MARGIN: public BIFF_record {                     0x28
//NYI class BIFF_rec_BOTTOM_MARGIN: public BIFF_record {                  0x29
//NYI class BIFF_rec_PRINT_ROW_HEADERS: public BIFF_record {              0x2A
//NYI class BIFF_rec_PRINT_GRIDLINES: public BIFF_record {                0x2B
//NYI class BIFF_rec_FILEPASS: public BIFF_record {                       0x2F
//______________________________________________________________________________
class BIFF_rec_FONT
: public BIFF_record
{
protected:
   nat16   dyHeight;
   nat16   grbit;

public:
   inline BIFF_rec_FONT(Record_code font_rec_code)
   : BIFF_record(font_rec_code,BINREC_VARIABLE_LENGTH,0)
   , dyHeight(0x00c8)
   , grbit(0x0000)
   {};
    virtual void write_body(Binary_file_interface  &out_file) = 0;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF2_rec_FONT
: public BIFF_rec_FONT
{          //                 0x31 /* @ FONT - Font Description */
protected:
   std::string font_name;

public:
   inline BIFF2_rec_FONT(Record_code font_rec_code = BIFF2578_CODE_FONT)
   : BIFF_rec_FONT(font_rec_code)
   , font_name("Arial")
   {};
//                       0x31 /* @ FONT - Font Description */

   virtual void write_body(Binary_file_interface  &out_file);
   virtual nat32 compute_record_body_size() const;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF34_rec_FONT
: public BIFF2_rec_FONT
{ //                   0x231
protected:  // Warning data member ordering does not match inheritence order
   nat16      color_index;
public:
   inline BIFF34_rec_FONT(Record_code font_rec_code = BIFF34_CODE_FONT)
   : BIFF2_rec_FONT(font_rec_code)
   {};
   virtual void write_body(Binary_file_interface  &out_file);
   virtual nat32 compute_record_body_size() const;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF57_rec_FONT
: public BIFF34_rec_FONT
{
protected:  // Warning data member ordering does not match inheritence order
   nat16   icv;     // index to the color palette
   nat16   bls;     // bold style
   nat16   sss;     // super/subscript
   nat8    uls;     // underline style
   nat8    bFamily;    // Font family as defined by the Win API LOGFONT structure
   nat8    bCharSet;   // Characers set as defined by the Win API LOGFONT structure
   nat8    reserved;

#define normal_text_font   0x019
#define bold_text_font     0x2BC

#define  no_super_sub_script 0
#define  superscript 1
#define  subscript   2

#define  underline_none    0
#define  underline_single  1
#define  underline_double  2
#define  underline_single_accounting   3
#define  underline_double_accounting   4

public:
   inline BIFF57_rec_FONT(Record_code font_rec_code = BIFF2578_CODE_FONT)
   : BIFF34_rec_FONT(font_rec_code) // it appears 0x31 is used instead of the expected 0x231 so done use BIFF4_CODE_FONT
   , icv(0x7FFF)
   , bls(normal_text_font) // normal text
   , sss(no_super_sub_script)
   , uls(underline_none)
   , bFamily(0)
   , bCharSet(0)
   , reserved(0)
   {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual nat32 compute_record_body_size() const;  
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
#ifdef NYI
class BIFF8_rec_FONT
: BIFF57_rec_FONT // same as BIFF5, but font name written in UNICODE?
{
   // WARNING need to store the font_name in Unicode.
   inline BIFF8_rec_FONT()
   : BIFF57_rec_FONT(BIFF2578_CODE_FONT)
   {};

   virtual void write_body(Binary_file_interface &out_file);

   virtual nat32 compute_record_body_size() const;  
};
#endif
//______________________________________________________________________________
//NYI class BIFF_rec_FONT2: public BIFF_record {                          0x32
//______________________________________________________________________________
//NYI class BIFF_rec_TABLE: public BIFF_record {                          0x36
//______________________________________________________________________________
//NYI class BIFF_rec_TABLE2: public BIFF_record {                         0x37
//______________________________________________________________________________
//NYI class BIFF_rec_CONTINUE: public BIFF_record {                       0x3C /* CONTINUE - Continues Long Records */
//______________________________________________________________________________
struct BIFF_layout_WINDOW1_common
{  // document window position and size are in twips = 1/20 of a point
   nat16   xWn;     // signed? Horizontal position
   nat16   yWn;     // signed? Vertical position
   nat16   dxWn;    // signed? Width
   nat16   dyWn;    // signed? Height
 public:
    inline BIFF_layout_WINDOW1_common()
    : xWn    (0)
    , yWn    (0)
    , dxWn   (20000)
    , dyWn   (10000)
    {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF234_layout_WINDOW1
: public BIFF_layout_WINDOW1_common
{
   nat8    hidden;   //  0 = Window is visible; 1 = Window is hidden
 public:
    inline BIFF234_layout_WINDOW1()
    : BIFF_layout_WINDOW1_common()
    , hidden(0)
    {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF578_layout_WINDOW1
: public BIFF_layout_WINDOW1_common
{
   nat16   grbit;      //
   nat16   itabCur;    // Index of the selected workbook tab (0based)
   nat16   itabFirst;  // Index of the first displayed workbook tab
   nat16   ctabSel;    // Number of workbook tabs that are selected
   nat16   wTabRatio; // Ratio of width of the workbook tabs to the width of the horiz scroll bar to obtain the ratio convert to decimal and divide by 1000
public: // constructor
   inline BIFF578_layout_WINDOW1()
   : BIFF_layout_WINDOW1_common()
   , itabCur       (0)
   , itabFirst     (0)
   , ctabSel  (1)
   , wTabRatio(0x0258)
   {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF_rec_WINDOW1_interface
{
public:
   virtual bool get_hidden()                                    const = 0;
   virtual nat16 get_selected_workbook_tab_index()             const = 0;
   virtual nat16 get_first_displayed_workbook_tab_index()      const = 0;
   virtual nat16 get_selected_workworkbook_tab_count()         const = 0;
   virtual nat16 get_tab_ratio()                               const = 0;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF_rec_WINDOW1_common
: public BIFF_record
, public BIFF_rec_WINDOW1_interface
{                        //0x3D /* WINDOW1 - Window Information */
public:
   inline BIFF_rec_WINDOW1_common
   (Record_code window1_code
    ,nat32 _record_body_length
    ,void *_body)
   :BIFF_record(window1_code,_record_body_length,_body)
   {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF234_rec_WINDOW1
: public BIFF_rec_WINDOW1_common
//, public BIFF_rec_WINDOW1_interface
{
protected:
   BIFF234_layout_WINDOW1 body;
public:
   inline BIFF234_rec_WINDOW1()  // There is only one code for WINDOW1 in all versions
   : BIFF_rec_WINDOW1_common(BIFF234578_CODE_WINDOW1,10,&body)
   {};
   inline bool get_hidden()                                            const { return body.hidden ; };
   inline virtual nat16 get_selected_workbook_tab_index()             const { return NOT_APPLICABLE; };
   inline virtual nat16 get_first_displayed_workbook_tab_index()      const { return NOT_APPLICABLE; };
   inline virtual nat16 get_selected_workworkbook_tab_count()         const { return NOT_APPLICABLE; };
   inline virtual nat16 get_tab_ratio()                               const { return NOT_APPLICABLE; };
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF578_rec_WINDOW1 // Probably BIFF7 or BIFF8
: public BIFF_rec_WINDOW1_common
//, public BIFF_rec_WINDOW1_interface
{
   BIFF578_layout_WINDOW1 body;
public:
   enum Option_flags  // grbit
   {
      fHidden        = 0x01
   ,  fIconic        = 0x02
   ,  fReserved      = 0x04
   ,  fDspHScroll    = 0x08
   ,  fDspVScroll    = 0x10
   ,  fBotAdornment  = 0x20
   };

public:
   inline BIFF578_rec_WINDOW1()    // There is only one code for WINDOW1 in all versions
   : BIFF_rec_WINDOW1_common(BIFF234578_CODE_WINDOW1,18,&body)
   {
      body.xWn = 0;
      body.yWn = 0;
      body.dxWn = 20000;
      body.dyWn = 10000;
      body.grbit = fDspHScroll | fDspVScroll | fBotAdornment;   // not hidden, not iconic,display scroll bars
   };
   inline bool get_hidden()                                     const { return body.grbit & fHidden ; };
   inline virtual nat16 get_selected_workbook_tab_index()      const { return body.itabCur; };
   inline virtual nat16 get_first_displayed_workbook_tab_index()const{ return body.itabFirst; };
   inline virtual nat16 get_selected_workworkbook_tab_count()  const { return body.ctabSel; };
   inline virtual nat16 get_tab_ratio()                        const { return body.wTabRatio; };
};
//______________________________________________________________________________
/*100111*/ class BIFF_rec_WINDOW2_interface
{
 public:
    enum Option_flags_mask
    { fDspFmla     = 0x0001
    , fDspGrid     = 0x0002
    , fDspRwCol    = 0x0004
    , fFrozen      = 0x0008
    , fDspZeros    = 0x0010
    , fDefaultHdr  = 0x0020
    , fArabic      = 0x0040
    , fDspGuts     = 0x0080
    , fFrozenNoSplit=0x0100
    , fSelected    = 0x0200
    , fPaged       = 0x0400
    , fSLV         = 0x0800  // BIFF8
    };
 public:
    virtual void set_has_frozen_panes(bool frozen) = 0;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF2_layout_WINDOW2
{
   nat8     show_formulas;    // 0 = Show formula results 1 = Show formulas
   nat8     show_grid_lines;  // 0 = Do not show grid lines 1 = Show grid lines
   nat8     show_headers;     // 0 = Do not show sheet headers 1 = Show sheet headers
   nat8     frozen_panes;     // 0 = Panes are not frozen 1 = Panes are frozen (freeze
   nat8     show_zeros;       // 0 = Show zero values as empty cells 1 = Show zero values
   nat16   rwTop;   // Top row visible in the window
   nat16   colLeft; // Leftmost column visible in the window
   nat8    grid_line_color_mode;  // 0 = Use manual grid line colour (below) 1 = Use automatic grid line colour
   nat32   rgbHdr;  // Row/column heading and gridline color (BIFF7<)
                     // this is icvHdr BIFF8 (Index to color value for row/column headins and gridlines
 public :
    BIFF2_layout_WINDOW2()
    :show_formulas         (0)
    , show_grid_lines       (1)
    , show_headers          (1)
    , frozen_panes          (1)
    , show_zeros            (0)
    , rwTop                 (0)
    , colLeft               (0)
    , grid_line_color_mode  (1)
    , rgbHdr                (0)  // 0 is probably black
    {}
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF3457_layout_WINDOW2
{
   nat16   grbit;   // Option flags  Option_flags_mask
   nat16   rwTop;   // Top row visible in the window
   nat16   colLeft; // Leftmost column visible in the window
   nat32   grid_line_RGB_color;  // Row/column heading and gridline color (BIFF7<)
 public :
               BIFF3457_layout_WINDOW2()
    : grbit              (BIFF_rec_WINDOW2_interface::fDspGrid | BIFF_rec_WINDOW2_interface::fDspRwCol | BIFF_rec_WINDOW2_interface::fDspZeros | BIFF_rec_WINDOW2_interface::fSelected | BIFF_rec_WINDOW2_interface::fPaged)
    , rwTop              (0)
    , colLeft            (0)
    , grid_line_RGB_color(0)  // 0 is probably black
    {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF8_layout_WINDOW2
{
   nat16   grbit;   // Option flags  Option_flags_mask
   nat16   rwTop;   // Top row visible in the window
   nat16   colLeft; // Leftmost column visible in the window
   nat16   icvHdr;  // Row/column heading and gridline color. This is icvHdr BIFF8 (Index to color value for row/column headins and gridlines
   nat16   reserved1;
   nat16   wScaleSLV;     // Zoom magnification in page break preview
   nat16   wScaleNormal;  // Zoom magnification in normal view
   nat32   reserved2;
#ifdef OBSOLETE
Appears to be adding an extra word
 public :
    BIFF8_layout_WINDOW2()
    : grbit              (BIFF_rec_WINDOW2_interface::fDspGrid | BIFF_rec_WINDOW2_interface::fDspRwCol | BIFF_rec_WINDOW2_interface::fDspZeros | BIFF_rec_WINDOW2_interface::fSelected | BIFF_rec_WINDOW2_interface::fPaged)
    , rwTop              (0)
    , colLeft            (0)
    , icvHdr             (0)   // index 0 is black in the built in color palette
    , reserved1          (0)
    , wScaleSLV          (0)   // 0 = Default (60%)
    , wScaleNormal       (0)   // 0 = Default (100%)
    , reserved2          (0)
    {};
#endif
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF_rec_WINDOW2_common
: public BIFF_record
, public BIFF_rec_WINDOW2_interface
{  //                      0x3E
 public:
    inline BIFF_rec_WINDOW2_common
       (Record_code record_type_code
       ,nat32 _record_body_length
       ,void *body
       )
    : BIFF_record(BIFF2_CODE_WINDOW2,_record_body_length,body)
    {};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF2_rec_WINDOW2
: public BIFF_rec_WINDOW2_common
{  //                      0x3E
 protected:
    BIFF2_layout_WINDOW2 body;
 public:
    inline BIFF2_rec_WINDOW2()
    : BIFF_rec_WINDOW2_common(BIFF2_CODE_WINDOW2,BIFF2_LEN_WINDOW2,&body)
    , body()
    {};
    inline virtual void set_has_frozen_panes(bool frozen)
    {  set_modified(true);
       body.frozen_panes = frozen;
    };
};
//______________________________________________________________________________
class BIFF3457_rec_WINDOW2
: public BIFF_rec_WINDOW2_common
{   //                  0x23E /* WINDOW2 - Sheet Window Information (enhanced WINDOW2)*/
 protected:
    BIFF8_layout_WINDOW2 body;
 public:
    inline BIFF3457_rec_WINDOW2()
    : BIFF_rec_WINDOW2_common(BIFF34578_CODE_WINDOW2,BIFF345_LEN_WINDOW2/*sizeof(BIFF3457_layout_WINDOW2)*/,&body)
    {   body.grbit   =          (BIFF_rec_WINDOW2_interface::fDspGrid | BIFF_rec_WINDOW2_interface::fDspRwCol | BIFF_rec_WINDOW2_interface::fDspZeros | BIFF_rec_WINDOW2_interface::fSelected | BIFF_rec_WINDOW2_interface::fPaged);
    };
/*______*/
    inline virtual void set_has_frozen_panes(bool frozen)
               {  set_modified(true);
                  if (frozen)
                     body.grbit |= (fFrozen | fFrozenNoSplit);
                  else
                     body.grbit &= ~(fFrozen | fFrozenNoSplit) ; // warning need to check
               };
};
//______________________________________________________________________________
class BIFF8_rec_WINDOW2
: public BIFF_rec_WINDOW2_common
{   //                  0x23E /* WINDOW2 - Sheet Window Information (enhanced WINDOW2)*/
 protected:
    BIFF8_layout_WINDOW2 body;
 public:
    inline BIFF8_rec_WINDOW2()
    : BIFF_rec_WINDOW2_common(BIFF34578_CODE_WINDOW2,BIFF8_LEN_WINDOW2/*sizeof(BIFF8_layout_WINDOW2)*/,&body)
    {  body.grbit = (BIFF_rec_WINDOW2_interface::fDspGrid | BIFF_rec_WINDOW2_interface::fDspRwCol | BIFF_rec_WINDOW2_interface::fDspZeros | BIFF_rec_WINDOW2_interface::fSelected | BIFF_rec_WINDOW2_interface::fPaged);
    };
/*______*/
    inline virtual void set_has_frozen_panes(bool frozen)
               {  set_modified(true);
                  if (frozen)
                     body.grbit |= (fFrozen | fFrozenNoSplit);
                  else
                     body.grbit &= ~(fFrozen | fFrozenNoSplit) ; // warning need to check
               };
};
//______________________________________________________________________________
//NYI class BIFF_rec_BACKUP: public BIFF_record {                         0x40 /* BACKUP - Save Backup Version of the File */
struct BIFF_layout_PANE
{
   nat16   x;       // Horizontal position of the split (0 if none)   warning may be nat32 in later version
   nat16   y;       // Vert position of the split (0 if none)
   nat16   rwTop;   // Top row visibale in the bottom pane            warning may be nat32 in later version
   nat16   colLeft; // Leftmost column visible in the right pane
   nat16   pnnAct;  // Pane number of the active pane
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF_rec_PANE
: public BIFF_record
{ //                          0x41  /* PANE - Number of Panes and Their Position */
   BIFF_layout_PANE body;
public:
   enum Panes {lower_right,upper_right,lower_left,upper_left};
public:
   inline BIFF_rec_PANE()
   : BIFF_record(BIFF234578_CODE_PANE,BIFF_LEN_PANE,&body)
   {};

   void freeze(nat16 _left_most_column , nat32 _top_most_row,Panes active_pane );
   //NYI void split(xxx)
};
//______________________________________________________________________________
class BIFF_rec_SELECTION: public BIFF_record
{ //                     0x1D
            protected:
               class Ref
               : public Item
               {
                  nat16   rwFirst;
                  nat16   rwLast;
                  nat8    colFirst;
                  nat8    colLast;
               public:
                  inline Ref
                  (  nat16   _rwFirst
                  ,  nat16   _rwLast
                  ,  nat8    _colFirst
                  ,  nat8    _colLast
                  ) // Warning when we add selections this will need
                  : Item()
                  ,rwFirst(_rwFirst)
                  ,rwLast(_rwLast)
                  ,colFirst(_colFirst)
                  ,colLast(_colLast)
                  {};

                  void write_ref(Binary_file_interface  &out_file);
               };

            protected:
   BIFF_rec_PANE::Panes pnn;
   nat16               rwAct;
   nat16               colAct;
   nat16               irefAct;
   Unidirectional_list   array_of_refs;
public:
   inline BIFF_rec_SELECTION()
   : BIFF_record(BIFF234578_CODE_SELECTION,BIFF_VLEN_SELECTION,0)
   ,pnn(BIFF_rec_PANE::lower_left) // default is 3 (lower left) indicating no splits
   ,rwAct(0)
   ,colAct(0)
   ,irefAct(0)
   ,array_of_refs()
   {};
    virtual void write_body(Binary_file_interface  &out_file);
   virtual nat32 compute_record_body_size() const ;  
};
//______________________________________________________________________________

// The following are BIFF3 or later   @ indicates I could probably implement this record

/* The following records are required for BIFF5 files
Required Records
BOF - Set the 6 byte offset to 0x0005 (workbook globals)
Window1
FONT - At least five of these records must be included
XF - At least 15 Style XF records and 1 Cell XF record must be included
STYLE
BOUNDSHEET - Include one BOUNDSHEET record per worksheet
EOF
BOF - Set the 6 byte offset to 0x0010 (worksheet)
INDEX
DIMENSIONS
WINDOW2
EOF
*/

//NYI class BIFF4_rec_CODEPAGE: public BIFF_record {             0x42 /* @ CODEPAGE - Default Code Page */
// 0x43-0x4C not used
//NYI class BIFF4_rec_PLS: public BIFF_record {                     0x4D /* PLS - Environment-Specific Print Record */
// 0x4E-0x4F not used
//NYI class BIFF4_rec_DCON: public BIFF_record {                     0x50 /* DCON - Data Consolidation Information */
//NYI class BIFF4_rec_DCONREF: public BIFF_record {                     0x51     /* DCONREF - Data Consolidation References */
//NYI class BIFF4_rec_DCONNAME: public BIFF_record {                     0x52 /* DCONNAME - Data Consolidation Named References */
// 0x53-0x54 not used
//NYI class BIFF4_rec_DEFCOLWIDTH: public BIFF_record {                     0x55 /* DEFCOLWIDTH - Default Width for Columns */
// 0x65-0x58 not used
//NYI class BIFF4_rec_XCT: public BIFF_record {                     0x59 /* XCT - CRN Record Count */
//NYI class BIFF4_rec_CRN: public BIFF_record {                     0x5A /* CRN - Nonresident Operands */
//NYI class BIFF4_rec_FILESHARING: public BIFF_record {                     0x5B  /* FILESHARING - File-Sharing Information */
//NYI class BIFF4_rec_WRITEACCESS: public BIFF_record {                     0x5C  /* @ WRITEACCESS - Write Access User Name */
//NYI class BIFF4_rec_OBJ: public BIFF_record {                     0x5D  /* OBJ - Describes a Graphic Object */
//NYI class BIFF4_rec_UNCALCED: public BIFF_record {                     0x5E /* UNCALCED - Recalculation Status */
//NYI class BIFF4_rec_SAVERECALC: public BIFF_record {                     0x5F /* SAVERECALC - Recalculate Before Save */
//NYI class BIFF4_rec_TEMPLATE: public BIFF_record {                     0x60 /* TEMPLATE - Workbook Is a Template */
// 0x61-0x62 not used
//NYI class BIFF4_rec_OBJPROTECT: public BIFF_record {                     0x63 /* OBJPROTECT - Objects Are Protected */
// 0x64-0x7C not used
//NYI class BIFF4_rec_COLINFO: public BIFF_record {                     0x7D /* COLINFO - Column Formatting Information */
//______________________________________________________________________________
class BIFF34578_rec_RK
: public BIFF_XF_cell_record_variable
, public Cell_numeric                                                             //100113
{    //                     0x7E  /* RK - Cell values in RK format */
    nat32   rk;      // RK encoded number
    // BIFF34578_layout_RK body;
 public:
   BIFF34578_rec_RK
    (//Record_code _record_code ,
    nat16  _row=0,nat16 _col =0
    ,nat16  _ixfe=0
    ,int32   integer_value=0  // Although we allow a 32bit integer value, the RK encoded number will lose 2 digits of precision
    ,bool    multiply_by_100 = false
    );
   BIFF34578_rec_RK
    (//Record_code _record_code ,
    nat16  _row=0,nat16 _col=0
    ,nat16  _ixfe=0
    ,float64   float32_value =0
    ,bool    multiply_by_100 = false
    );
   BIFF34578_rec_RK
    (//Record_code _record_code ,
    nat16  _row=0,nat16 _col =0
    ,nat16  _ixfe =0
    ,nat32  _rk =0
    ); // This constructor is used when BIFF readers encounter a MULRK recorder.
       // The reader will generate a psuedo RK record for each RK entry.

   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
private: // contributes
   int32   set_RK_integer(int32 integer_value,bool multiply_by_100 = false);     //100113
   float64 set_RK_float(float64 float_value  ,bool multiply_by_100 = false);     //100113
   int32   get_RK_integer()                                    const;            //100113
   float64 get_RK_float()                                      const;            //100113
protected: //
   float64 RK_to_float64(nat32 rk)                             const;
protected: // Cell_numeric implementations
   virtual nat32  get_row()                                    const { return BIFF_XF_cell_record_variable::get_row(); };
   virtual nat32  get_col()                                    const { return BIFF_XF_cell_record_variable::get_col(); }; // Note: Excel currently only uses nat16  for columns
   inline virtual int32   set_int32(sint32   _value)    modification_{ set_RK_integer(_value); return get_RK_integer();};
   inline virtual float64 set_float64(float64 _value)   modification_{ set_RK_float(_value);   return get_RK_float();};
   inline virtual float32 set_float32(float32 _value)   modification_{ set_RK_float(_value);   return get_RK_float();};
   inline virtual int32   get_int32()                          const { return (int32) get_RK_integer();};
   inline virtual float32 get_float32()                        const { return (float32)get_RK_float();};
   inline virtual float64 get_float64()                        const { return (float64)get_RK_float();};
};
//______________________________________________________________________________
//NYI class BIFF4_rec_IMDATA: public BIFF_record {                     0x7F /* IMDATA - Image Data */
//NYI class BIFF4_rec_GUTS: public BIFF_record {                     0x80    /* GUTS - Size of Row and Column Gutters */
//NYI class BIFF4_rec_WSBOOL: public BIFF_record {                     0x81 /* WSBOOL - Additional Workspace Information */
//NYI class BIFF4_rec_GRIDSET: public BIFF_record {                     0x82 /* GRIDSET - State Change of Gridlines Option */
//NYI class BIFF4_rec_HCENTER: public BIFF_record {                     0x83 /* HCENTER - Center Between Horizontal Margins */
//NYI class BIFF4_rec_VCENTER: public BIFF_record {                     0x84 /* VCENTER - Center Between Vertical Margins */
//______________________________________________________________________________

// NYI I should make BIFF_rec_BOUNDSHEET_interface

class BIFF_rec_BOUNDSHEET_common
: public BIFF_record       // Not fixed layout because sheet_name length is variable
{
public:
   enum Sheet_type
   {WORKSHEET
   ,EXCEL_4_MACRO_SHEET
   ,CHART
   ,VB_MODULE = 0x06
   };
   enum Hidden_state
   {VISIBLE
   ,HIDDEN
   ,VERYHIDDEN
   };
protected:
   nat32      lbPlyPos;   // Stream postion of the start of the BOF record for the sheet
                           // 0 indicates the sheet has not been written yet and will need to be determined
   nat16      grbit;      // Option flags
public:
   inline BIFF_rec_BOUNDSHEET_common
      (Sheet_type          _sheet_type = WORKSHEET
      ,Hidden_state        _hidden_state = VISIBLE)
      : BIFF_record(BIFF578_CODE_BOUNDSHEET,BIFF5_VLEN_BOUNDSHEET,0)
      , lbPlyPos(0) // will be determined when written
      ,grbit((nat16)(_hidden_state << 8) | (nat16)_sheet_type)
      {};
   virtual void write_body(Binary_file_interface  &out_file);
   virtual bool read_body(Binary_file_interface  &in_file);
   virtual nat32 compute_record_body_size()                                const = 0;
   inline void set_BOF_stream_pos(nat32 BOF_pos)
      {  lbPlyPos = BOF_pos; set_modified(true);};
   virtual std::string &get_sheet_name(std::string &result)                const = 0;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF57_rec_BOUNDSHEET
: public BIFF_rec_BOUNDSHEET_common
{
   std::string sheet_name; // Will be stored in cch and rgch
public:
   inline BIFF57_rec_BOUNDSHEET
      (const std::string  &_sheet_name
      ,Sheet_type          _sheet_type = WORKSHEET
      ,Hidden_state        _hidden_state = VISIBLE)
      :BIFF_rec_BOUNDSHEET_common(_sheet_type,_hidden_state)
      ,sheet_name(_sheet_name)
      {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   virtual nat32 compute_record_body_size()                                const;
   inline virtual std::string &get_sheet_name(std::string &result)         const { return result = sheet_name;};
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF8_rec_BOUNDSHEET
: public BIFF_rec_BOUNDSHEET_common
{
   // The sheet name is in Unicode
   BIFF8_string sheet_name; // I think use wstring for unicode
public:
   inline BIFF8_rec_BOUNDSHEET
      (const std::string  &_sheet_name
      ,Sheet_type          _sheet_type = WORKSHEET
      ,Hidden_state        _hidden_state = VISIBLE
      )
      : BIFF_rec_BOUNDSHEET_common(_sheet_type,_hidden_state)
      , sheet_name(L"",false,false)
      {  string_to_wstring(_sheet_name,sheet_name);};

   inline BIFF8_rec_BOUNDSHEET
      (const std::wstring  &_sheet_name
      ,Sheet_type          _sheet_type = WORKSHEET
      ,Hidden_state        _hidden_state = VISIBLE
      )
      : BIFF_rec_BOUNDSHEET_common(_sheet_type,_hidden_state)
      , sheet_name(_sheet_name,false,false)
      {};

   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &out_file);
   virtual nat32 compute_record_body_size() const;  // obs cannot be const!!!
   virtual std::string &get_sheet_name(std::string &result) const;
   virtual std::wstring &get_sheet_name(std::wstring &result) const ;
};
//______________________________________________________________________________
//NYI class BIFF4_rec_WRITEPROT: public BIFF_record {                     0x86 /* WRITEPROT - Workbook Is Write-Protected */
//NYI class BIFF4_rec_ADDIN: public BIFF_record {                     0x87  /* ADDIN - Workbook Is an Add-in Macro */
//NYI class BIFF4_rec_EDG: public BIFF_record {                     0x88 /* EDG - Edition Globals */
//NYI class BIFF4_rec_PUB: public BIFF_record {                      0x89  /* PUB - Publisher */
// 0x8A-0x8B not used
//NYI class BIFF4_rec_COUNTRY: public BIFF_record {                     0x8C /* COUNTRY - Default Country and WIN.INI Country */
//NYI class BIFF4_rec_HIDEOBJ: public BIFF_record {                     0x8D /* @ HIDEOBJ - Object Display Options */
// 0x8E-0x8F not used
//NYI class BIFF4_rec_SORT: public BIFF_record {                     0x90  /* SORT - Sorting Options */
//NYI class BIFF4_rec_SUB: public BIFF_record {                     0x91  /* SUB - Subscriber */
//NYI class BIFF4_rec_PALETTE: public BIFF_record {                     0x92  /* PALETTE - Color Palette Definition */

//NYI class BIFF4_rec_LHRECORD: public BIFF_record {                     0x94    /* LHRECORD - .WK? File Conversion Information */
//NYI class BIFF4_rec_LHNGRAPH: public BIFF_record {                     0x95 /* LHNGRAPH - Named Graph Information */
//NYI class BIFF4_rec_SOUND: public BIFF_record {                     0x96    /* SOUND - Sound Note */
//NYI class BIFF4_rec_LPR: public BIFF_record {                     0x98   /* LPR - Sheet Was Printed Using LINE.PRINT */
//NYI class BIFF4_rec_STANDARDWIDTH: public BIFF_record {                     0x99  /* STANDARDWIDTH - Standard Column Width */
//NYI class BIFF4_rec_FNGROUPNAME: public BIFF_record {                     0x9A  /* FNGROUPNAME - Function Group Name */
//NYI class BIFF4_rec_FILTERMODE: public BIFF_record {                     0x9B   /* FILTERMODE - Sheet Contains Filtered List */
//NYI class BIFF4_rec_FNGROUPCOUNT: public BIFF_record {                     0x9C /* FNGROUPCOUNT - Built-in Function Group Count */
//NYI class BIFF4_rec_AUTOFILTERINFO: public BIFF_record {                     0x9D  /* AUTOFILTERINFO - Drop-Down Arrow Count */
//NYI class BIFF4_rec_AUTOFILTER: public BIFF_record {                     0x9E /* AUTOFILTER - AutoFilter Data */

// 0x9F - 0xFE

//NYI class BIFF4_rec_SCL: public BIFF_record {                     0xA0 /* SCL - Window Zoom Magnification */
//NYI class BIFF4_rec_SETUP: public BIFF_record {                     0xA1  /* SETUP - Page Setup */

//NYI class BIFF4_rec_COORDLIST: public BIFF_record {                     0xA9  /* COORDLIST - Polygon Object Vertex Coordinates */

//NYI class BIFF4_rec_GCW: public BIFF_record {                     0xAB  /* GCW - Global Column-Width Flags */

//NYI class BIFF4_rec_SCENMAN: public BIFF_record {                     0xAE  /* SCENMAN - Scenario Output Data */
//NYI class BIFF4_rec_SCENARIO: public BIFF_record {                     0xAF  /* SCENARIO - Scenario Data� */
//NYI class BIFF4_rec_SXVIEW: public BIFF_record {                     0xB0  /* SXVIEW - View Definition */
//NYI class BIFF4_rec_SXVD: public BIFF_record {                     0xB1  /* SXVD - View Fields */
//NYI class BIFF4_rec_SXVI: public BIFF_record {                     0xB2   /* SXVI - View Item */

//NYI class BIFF4_rec_SXIVD: public BIFF_record {                     0xB4  /* SXIVD - Row/Column Field IDs */
//NYI class BIFF4_rec_SXLI: public BIFF_record {                     0xB5 /* SXLI - Line Item Array */
//NYI class BIFF4_rec_SXPI: public BIFF_record {                     0xB6  /* SXPI - Page Item */

//NYI class BIFF4_rec_DOCROUTE: public BIFF_record {                     0xB8  /* DOCROUTE - Routing Slip Information */
//NYI class BIFF4_rec_RECIPNAME: public BIFF_record {                     0xB9 /* RECIPNAME - Recipient Name */


//NYI class BIFF4_rec_SHRFMLA: public BIFF_record {                     0xBC   /* SHRFMLA - Shared Formula */
//______________________________________________________________________________
class BIFF578_rec_MULRK
: public BIFF_record
{     //                0xBD    /* @ MULRK - Multiple RK Cells */
public:
   struct RK_record // rkrec
   {  nat16 ixfe;
      nat32 RK;
   };

   nat16 row_index;       // Index of the row the cells are on
   nat16 first_col_index; // Index of the column of the first cell in the series
   RK_record *records;     // owned by this. Dynamically allocated Array of RK_record
   nat16 last_col_index;  // Index of the last column containing the RKREC structure
                           // Why Microsoft put this after the RK_record escapes me!
                           // Now you have to look at the record length to calculate how many RK_records there are, stupid!
private:  // not written to the file
   contribute_ nat16 number_records;
public:
   BIFF578_rec_MULRK(nat32 record_length);
   virtual ~BIFF578_rec_MULRK();
   virtual bool read_body(Binary_file_interface &in_file);        // stream_IO_;
   virtual nat32 compute_record_body_size()                               const;
   virtual void write_body(Binary_file_interface &out_file);
   inline nat16 get_number_records()                                       const
      { return number_records; };
};
//______________________________________________________________________________
//NYI class BIFF4_rec_MULBLANK: public BIFF_record {                     0xBE    /* * MULBLANK - Multiple Blank Cells */


//NYI class BIFF4_rec_MMS: public BIFF_record {                     0xC1   /* MMS - ADDMENU/DELMENU Record Group Count */
//NYI class BIFF4_rec_ADDMENU: public BIFF_record {                     0xC2   /* ADDMENU - Menu Addition */
//NYI class BIFF4_rec_DELMENU: public BIFF_record {                     0xC3  /* DELMENU - Menu Deletion */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xC4
//NYI class BIFF4_rec_SXDI: public BIFF_record {                     0xC5 /* SXDI - Data Item */
//NYI class BIFF8_rec_SXDB: public BIFF_record {                     0xC6 /* SXDB - PivotTable Cache Data (May have been in BIFF4)*/
////NYI class : public BIFF_record {  BIFFx_rec_                   0xC7
////NYI class : public BIFF_record {  BIFFx_rec_                   0xC8
////NYI class : public BIFF_record {  BIFFx_rec_                   0xC9
////NYI class : public BIFF_record {  BIFFx_rec_                   0xCA
////NYI class : public BIFF_record {  BIFFx_rec_                   0xCB
////NYI class : public BIFF_record {  BIFFx_rec_                   0xCC
//NYI class BIFF4_rec_SXSTRING: public BIFF_record {                     0xCD /* SXSTRING - String */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xCE
////NYI class : public BIFF_record {  BIFFx_rec_                   0xCF
//NYI class BIFF4_rec_SXTBL: public BIFF_record {                     0xD0  /* SXTBL - Multiple Consolidation Source Info */
//NYI class BIFF4_rec_SXTBRGIITM: public BIFF_record {                     0xD1   /* SXTBRGIITM - Page Item Name Count */
//NYI class BIFF4_rec_SXTBPG: public BIFF_record {                     0xD2    /* SXTBPG - Page Item Indexes */
//NYI class BIFF4_rec_OBPROJ: public BIFF_record {                     0xD3    /* OBPROJ - Visual Basic Project */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xD4
//NYI class BIFF4_rec_SXIDSTM: public BIFF_record {                     0xD5   /* SXIDSTM - Stream ID */
//NYI class BIFF4567_rec_RSTRING: public BIFF_record {             0xD6   /* @ RSTRING - Cell with Character Formatting */  obsolete in BIFF8
//NYI class BIFF4_rec_DBCELL: public BIFF_record {                     0xD7  /* DBCELL - Stream Offsets */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xD8
////NYI class : public BIFF_record {  BIFFx_rec_                   0xD9
//NYI class BIFF4_rec_BOOKBOOL: public BIFF_record {                     0xDA /* BOOKBOOL - Workbook Option Flag */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xDB
//NYI class : public BIFF_record {  BIFF8_rec_SXEXT                   0xDC /* PARAMQRY-SXEXT - Query Parameters-External Source Information (May have been in BIFF4)*/
//NYI class : public BIFF_record {  BIFF4_rec_SCENPROTECT                   0xDD  /* SCENPROTECT - Scenario Protection */
//NYI class : public BIFF_record {  BIFF4_rec_OLESIZE                   0xDE  /* OLESIZE - Size of OLE Object */
//NYI class : public BIFF_record {  BIFF4_rec_UDDESC                   0xDF   /* UDDESC - Description String for Chart Autoformat */
////NYI class : public BIFF_record {  BIFFx_rec_                   0x
////NYI class : public BIFF_record {  BIFFx_rec_                   0x
//______________________________________________________________________________
class BIFF_rec_XF
: public BIFF_record
{

public:
   inline BIFF_rec_XF
   (Record_code XF_rec_code
   ,nat32 XF_rec_length
   ,void *XF_rec_body)
   : BIFF_record(XF_rec_code,XF_rec_length,XF_rec_body)
   {};
};
//______________________________________________________________________________
/*NYI
Not sure if BIFF4 is actually implemented in BIFF4 only BIFF7 and BIFF8 are known to me to have XF records
class BIFF4_rec_XF
: public BIFF_rec_XF
{  // 0xE0  Extended Format

};

*/
//______________________________________________________________________________
struct BIFF2_layout_XF
{
   nat8    inft;
   nat8    unused;
   nat8    number_format_and_cell_flags;
   nat8    horizontal_alignment_border_style_background;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF3_layout_XF
{
   nat8    inft;
   nat8    ifmt;
   nat8    protection;
   nat8    attribute_group_flags;
   nat16   horizontal_alignment_text_break_parent_style_XF;
   nat16   XF_area_34; // cell background area
   nat32   XF_border_34; // cell border lines
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF4_layout_XF
{
   nat8    inft;
   nat8    ifmt;
   nat16   XF_type_cell_protection_parent_style_XF;
   nat8    alignment_text_break_text_orientation;
   nat8    attribute_group_flags;
   nat16   XF_area_34; // cell background area
   nat32   XF_border_34; // cell border lines
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF57_layout_XF    // I think the same for 5 and 7
{
   nat16   inft;
   nat16   ifmt;
   nat16   XF_type_cell_protection_parent_style_XF;
   nat8    alignment_text_break;
   nat32   cell_border_lines_background_area;
   nat32   line_style_color;
   nat16   flags5;
   nat16   flags6;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF8_layout_XF
{
   nat16   inft;
   nat16   ifmt;
   nat16   XF_type_cell_protection_parent_style_XF;
   nat8    alignment_text_break;
   nat8    XF_rotation;
   nat16   flags4;
   nat32   flags5;
   nat16   flags6;
};
//______________________________________________________________________________
// NYI BIFF2_rec_XF
// NYI BIFF3_rec_XF
// NYI BIFF4_rec_XF
//______________________________________________________________________________
class BIFF57_rec_XF
: public BIFF_rec_XF
{
   BIFF57_layout_XF body;
public:
   BIFF57_rec_XF(nat8 *default_values);
};
//______________________________________________________________________________
//extern BIFF8_layout_XF XF_0;
//extern BIFF8_layout_XF XF_n;
//______________________________________________________________________________
class BIFF8_rec_XF
: public BIFF_rec_XF
{
   BIFF8_layout_XF body;
public:
   BIFF8_rec_XF()
   : BIFF_rec_XF(BIFF578_CODE_XF,16,&body)
   {
/*
      body.inft   = XF_n.inft;
      body.ifmt   = XF_n.ifmt;
      body.flags1 = XF_n.flags1;
      body.flags2 = XF_n.flags2;
      body.flags3 = XF_n.flags3;
      body.flags4 = XF_n.flags4;
      body.flags5 = XF_n.flags5;
      body.flags6 = XF_n.flags6;
*/
   };
};
//______________________________________________________________________________
//NYI class BIFF4_rec_INTERFACEHDR: public BIFF_record {                     0xE1    /* @ INTERFACEHDR - Beginning of User Interface Records */
//NYI class BIFF4_rec_INTERFACEEND: public BIFF_record {                     0xE2   /* INTERFACEEND - End of User Interface Records */
//NYI class BIFF4_rec_SXVS: public BIFF_record {                     0x0xE3     /* SXVS - View Source */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xE4
//NYI class BIFF4_rec_UNKNOWN_E5: public BIFF_record {                     0xE5  /* ?UNKNOWN1 - Guessing */
////NYI class : public BIFF_record {  BIFFx_rec_                   0xE6
////NYI class : public BIFF_record {  BIFFx_rec_                   0xE7
////NYI class : public BIFF_record {  BIFFx_rec_                   0xE8
////NYI class : public BIFF_record {  BIFFx_rec_                   0xE9
//NYI class BIFF4_rec_TABIDCONF: public BIFF_record {                     0xEA /* TABIDCONF - Sheet Tab ID of Conflict History */
//NYI class BIFF8_rec_MSODRAWINGGROUP: public BIFF_record {                     0xEB   /* MSODRAWINGGROUP - Microsoft Office Drawing Group (May have been in BIFF4)*/
//NYI class BIFF8_rec_MSODRAWING: public BIFF_record {                     0xEC /* MSODRAWING - Microsoft Office Drawing (May have been in BIFF4)*/
//NYI class BIFF8_rec_MSODRAWINGSELECTION: public BIFF_record {                     0xED /* MSODRAWINGSELECTION - Microsoft Office Drawing Selection (May have been in BIFF4)*/
////NYI class : public BIFF_record {  BIFFx_rec_                   0xEF
//NYI class BIFF8_rec_SXRULE: public BIFF_record {                     0xF0  /* SXRULE - PivotTable Rule Data (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXEX: public BIFF_record {                     0xF1 /* SXEX - PivotTable View Extended Information (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXFILT: public BIFF_record {                     0xF2  /* SXFILT - PivotTable Rule Filter (May have been in BIFF4)*/
////NYI class : public BIFF_record {  BIFFx_rec_                   0xF3
////NYI class : public BIFF_record {  BIFFx_rec_                   0xF4
////NYI class : public BIFF_record {  BIFFx_rec_                   0xF5
//NYI class BIFF8_rec_SXNAME: public BIFF_record {                     0xF6 /* SXNAME - PivotTable Name (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXSELECT: public BIFF_record {                     0xF7 /* SXSELECT - PivotTable Selection Information (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXPAIR: public BIFF_record {                     0xF8  /* SXPAIR - PivotTable Name Pair (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXFMLA : public BIFF_record {                     0xF9    /* SXFMLA - PivotTable Parsed Expression (May have been in BIFF4)*/
////NYI class : public BIFF_record {  BIFFx_rec_                   0xFA
//NYI class BIFF8_rec_SXFORMAT: public BIFF_record {                     0xFB    /* SXFORMAT - PivotTable Format Record (May have been in BIFF4)*/
//______________________________________________________________________________
class BIFF8_rec_SST
: public BIFF_record
, public CORN::Unidirectional_list
{  // SST - Shared String Table
   nat32 total_string_count;   //cstTotal Total number of strings in the shared string table and extended string table (EXTSST record)
                     // cstTotal is probably only used internally in Excel
   nat32 unique_string_count;  // cstUnique Number of unique strings in the shared string table
public:
   class Item
   : public CORN::Item
   , public BIFF8_string
   {
   public:
      Item(const std::string &str);
      virtual ~Item() {};                                                        //081125
      //170424 replaced with label_wstring virtual const char *label_cstr(char *result)             const;//050619
      virtual const char *label_string(std::string &result)               const;//050619
      inline virtual bool write(std::ostream &strm)                   stream_IO_{ return 0;/*NYN*/}; //091222
   };
   // BIFF8_string strings; // string array
   //___________________________________________________________________________
public:
   inline BIFF8_rec_SST()
                  : BIFF_record(BIFF8_CODE_SST,BINREC_VARIABLE_LENGTH)
                  {};
   virtual void write_body(Binary_file_interface &out_file);
   virtual bool read_body(Binary_file_interface &in_file);
   // NYI append (const std::wstring )
};
//______________________________________________________________________________
class BIFF8_rec_LABELSST
//: public BIFF_record
: public BIFF_XF_cell_record_variable
{  // Cell Value, String Constant/SST
   nat32    SST_index; // isst
public:
   inline BIFF8_rec_LABELSST() : BIFF_XF_cell_record_variable(BIFF8_CODE_LABELSST,BIFF8_LEN_LABELSST,0,0,0) {};
   inline nat32 get_shared_string_table_index() const { return SST_index; };
   virtual void write_body(Binary_file_interface &out_file)   ;
   virtual bool read_body(Binary_file_interface &in_file)     ;
};
//______________________________________________________________________________
////NYI class : public BIFF_record {  BIFFx_rec_                   0xFE
//NYI class BIFF8_rec_EXTSST: public BIFF_record {                     0xFF  /* EXTSST - Extended Shared String Table (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXVDEX: public BIFF_record {                     0x100  /* SXVDEX - Extended PivotTable View Fields (May have been in BIFF4)*/
////NYI class : public BIFF_record {  BIFFx_rec_                   0x101
////NYI class : public BIFF_record {  BIFFx_rec_                   0x102
//NYI class BIFF8_rec_SXFORMULA: public BIFF_record {                     0x103 /* SXFORMULA - PivotTable Formula Record (May have been in BIFF4)*/
//NYI class BIFF8_rec_SXDBEX: public BIFF_record {                     0x122 /* SXDBEX - PivotTable Cache Data (May have been in BIFF4)*/
//NYI class BIFF4_rec_TABID: public BIFF_record {                     0x13D
//NYI class BIFF8_rec_USESELFS: public BIFF_record {                     0x160  /* USESELFS - Natural Language Formulas Flag (May have been in BIFF4)*/
//NYI class BIFF8_rec_DSF: public BIFF_record {                     0x161       /* DSF - Double Stream File (May have been in BIFF4)*/
//NYI class BIFF8_rec_XL5MODIFY: public BIFF_record {                     0x162  /* XL5MODIFY - Flag for DSF (May have been in BIFF4)*/
//NYI class BIFF4_rec_FILESHARING2: public BIFF_record {                     0x1A5 /* FILESHARING2 - File-Sharing Information for Shared Lists */
////NYI class : public BIFF_record {  BIFFx_rec_                   0x1A6
////NYI class : public BIFF_record {  BIFFx_rec_                   0x1A7
////NYI class : public BIFF_record {  BIFFx_rec_                   0x1A8
//NYI class BIFF8_rec_USERBVIEW: public BIFF_record {                     0x1A9 /* USERBVIEW - Workbook Custom View Settings (May have been in BIFF4)*/
//NYI class BIFF8_rec_USERSVIEWBEGIN: public BIFF_record {                     0x1AA   /* USERSVIEWBEGIN - Custom View Settings (May have been in BIFF4)*/
//NYI class BIFF8_rec_USERSVIEWEND: public BIFF_record {                     0x1AB     /* USERSVIEWEND - End of Custom View Records (May have been in BIFF4)*/
//NYI class BIFF4_rec_unused_1AC: public BIFF_record {                   0x1AC
//NYI class BIFF8_rec_QSI: public BIFF_record {                     0x1AD    /* QSI - External Data Range (May have been in BIFF4)*/
//NYI class BIFF8_rec_SUPBOOK: public BIFF_record {                     0x1AE   /* SUPBOOK - Supporting Workbook (May have been in BIFF4)*/
//NYI class BIFF4_rec_PROT4REV: public BIFF_record {                     0x1AF /* Shared Workbook Protection Flag */
//NYI class BIFF8_rec_CONDFMT: public BIFF_record {                     0x1B0 /* CONDFMT - Conditional Formatting Range Information (May have been in BIFF4)*/
//NYI class BIFF8_rec_CF: public BIFF_record {                     0x1B1 /* CF - Conditional Formatting Conditions (May have been in BIFF4) */
//NYI class BIFF8_rec_DVAL: public BIFF_record {                     0x1B2 /* DVAL - Data Validation Information (May have been in BIFF4)*/
//NYI class BIFF8_rec_DCONBIN: public BIFF_record {                     0x1B5 /* DCONBIN - Data Consolidation Information (May have been in BIFF4)*/
//NYI class BIFF8_rec_TXO: public BIFF_record {                     0x1B6 /* TXO - Text Object (May have been in BIFF4)*/
//NYI class BIFF8_rec_REFRESHALL: public BIFF_record {                     0x1B7   /* REFRESHALL - Refresh Flag (May have been in BIFF4)*/
//NYI class BIFF8_rec_HLINK: public BIFF_record {                     0x1B8 /* HLINK - Hyperlink (May have been in BIFF4)*/
//NYI class BIFF8_rec_CODENAME: public BIFF_record {                     0x1BA /* CODENAME - xxx*/
//NYI class BIFF8_rec_SXFDBTYPE: public BIFF_record {                     0x1BB          /* SXFDBTYPE - SQL Datatype Identifier (May have been in BIFF4)*/
//NYI class BIFF8_rec_PROT4REVPASS: public BIFF_record {                     0x1BC  /* PROT4REVPASS - Shared Workbook Protection Password (May have been in BIFF4)*/
//NYI class BIFF8_rec_DV: public BIFF_record {                     0x1BE /* DV - Data Validation Criteria (May have been in BIFF4)*/
// 0x1BF - 1FF not yet defined
//NYI class BIFF4_rec_DIMENSIONS: public BIFF_record {                     0x200 /* DIMENSIONS - Cell Table Size */
//NYI class BIFF4_rec_BLANK: public BIFF_record {                     0x201 /* BLANK - Cell Value, Blank Cell */
//______________________________________________________________________________
//NYI class BIFF4_rec_BOOLERR: public BIFF_record {                     0x205 /* BOOLERR - Cell Value, Boolean or Error */
//______________________________________________________________________________
struct BIFF2_layout_ROW        // Not sure exactly which version
{  // Record Body Length: 16 bytes
/*
Description: Specifies a spreadsheet row
*/
   nat16 rw;        // Row number
   nat16 colMic;    // First defined column in the row
   nat16 colMac;    // Last defined column in the row plus 1
   nat16 miyRw;     // 6-7        Row height
   nat16 reserved;  // RESERVED  (cleared to 0)
   nat8  default_attributes;
      // Default attributes       1
      // Not default attributes   0
   nat16 cell_records_offset; // Offset to cell records for this row
   nat8  attrs1;
   nat8  attrs2;    // Cell attributes (rgbAttr) (see Table 2-3)
   nat8  attrs3;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
struct BIFF34578_layout_ROW     // Not sure exactly which version   208h
{  // Record Body Length: 16 bytes
   nat16   rw;         //  0-1        Row number      (Not sure what this is not 32bit!)
   nat16   colMic;     //  2-3        First defined column in the row
   nat16   colMac;     //  4-5        Last defined column in the row plus 1
   nat16   miyRw;      //  6-7        Row height
   nat16   irwMac;     //  8-9        Used by Excel to optimize loading the file; if you are creating a BIFF file, set irwMac to 0.                            0
   nat16   reserved;   //
   nat16   grbit;      //  Option flags.
   nat16   ixfe;
      // If fGhostDirty=1 (see grbit field), this is the index to the XF record for the row.
      // Otherwise, this field is undefined.
      //Note: ixfe uses only the low-order 12 bits of the field (bits 11�0).
      // Bit 12 is fExAsc, bit 13 is fExDsc, and bits 14 and 15 are reserved.
      // fExAsc and fExDsc are set to true if the row has a thick border on top or on bottom, respectively.
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF_rec_ROW_abstract     // abstract
: public BIFF_record
{  //                   0x208 /* ROW - Describes a Row */
      // Record Body Length: 16 bytes
      // Record Body Byte Structure:
public:
public:
   inline BIFF_rec_ROW_abstract(Record_code rec_code,void *fixed_layout)
   : BIFF_record(rec_code,BIFF_LEN_ROW,fixed_layout)
   {};
   virtual nat32 get_row() const = 0;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF2_rec_ROW
: public BIFF_rec_ROW_abstract
{
public:
   BIFF2_layout_ROW body;
public:
   inline BIFF2_rec_ROW() : BIFF_rec_ROW_abstract(BIFF2_CODE_ROW,&body) {};      // warning not sure the code for earlier version may be 0x08
   nat32 get_row() const { return body.rw; };
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
class BIFF34578_rec_ROW
: public BIFF_rec_ROW_abstract
{
public:
   BIFF34578_layout_ROW body;
public:
   inline BIFF34578_rec_ROW() : BIFF_rec_ROW_abstract(BIFF34578_CODE_ROW,&body) {};
   nat32 get_row() const { return body.rw; };
};
//______________________________________________________________________________
////NYI class : public BIFF_record {  BIFFx_rec_                   0x20A
//NYI class BIFF4_rec_NAME: public BIFF_record {                     0x218  /* NAME - Defined Name */
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x
//NYI class BIFF4_rec_ARRAY: public BIFF_record {                     0x221  /* ARRAY - Array-Entered Formula */
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x222
//NYI class BIFF4_rec_EXTERNNAME: public BIFF_record {                     0x223 /* EXTERNNAME - Externally Referenced Name */
//////NYI class : public BIFF_record {  BIFFx_rec_                   0x224
//NYI class BIFF4_rec_DEFAULTROWHEIGHT: public BIFF_record {                     0x225 /* DEFAULTROWHEIGHT - Default Row Height */
//NYI class BIFF4_rec_TABLE: public BIFF_record {                     0x236 /* TABLE - Data Table (enchanced)*/
//NYI class BIFF4_rec_RK: public BIFF_record {                     0x27E /* RK - Cell Value, RK Number */
//______________________________________________________________________________
struct BIFF34578_layout_STYLE
{
   nat16   ixfe;       // index to the style XF record
      // Note: ixfe uses only the low-order 12 bits of the field (bits (11-0).
      // Bits 12, 13, 14 are unused.
      // Bit 15 (fBuiltIn) is 1 for builtin styles.
   nat8    istyBuiltIn;
   nat8    iLevel;
};
//_BIFF34578_layout_STYLE______________________________________________________/

class BIFF34578_rec_STYLE
: public BIFF_record
{               //      0x293     /* STYLE - Style Information */
public:
   enum Builtin_style
   {
      Normal      // 0
   ,  RowLevel_n  // 1
   ,  ColLevel_n  // 2
   ,  Comma       // 3
   ,  Currency    // 4
   ,  Percent     // 5
   ,  Comma_0     // 6
   ,  Currency_0  // 7
   };
protected:
   BIFF34578_layout_STYLE body;
public:
   inline BIFF34578_rec_STYLE
   (bool          builtin
   ,nat16         ixfe_
   ,Builtin_style istyBuiltIn_
   ,nat8          iLevel_ = 0xFF // ignroed
   )
   : BIFF_record(BIFF34578_CODE_STYLE,6,&body)
   {  body.ixfe = (nat16)(builtin << 15) | ixfe_;
      body.istyBuiltIn = (nat8)istyBuiltIn_;
      body.iLevel = iLevel_;
   }
};
//_BIFF34578_rec_STYLE_________________________________________________________/
BIFF_rec_BOF *render_BIFF_BOF_record(Record_code code,nat32 record_length);

BIFF_record *render_BIFF_record
(Record_code code
,nat32 record_length   // Unfortunately some BIFF records need to know the record body length inorder to determine the number of items in lists/arrays.
,BIFF_Format format
   // Even though we know the record code, the same record code
   // was used in the different BIFF versions and the format
   // of the record may be different
);
//_render_BIFF_record__________________________________________________________/
}//_namespace_CORN_____________________________________________________________/

#endif

