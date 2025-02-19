#include <corn/format/excel/BIFF_record.h>
#include <corn/format/binary/binary_file_interface.h>
#include <assert.h>
#include <string.h>
   // for memcpy
namespace CORN {
//______________________________________________________________________________
BIFF8_layout_XF XF_0 =
{
   0x0000
,   0x0000
,   0xFFF5
,   0x0020
,   0x0000
,   0x0000
,   0x00000000
,   0x20C0
};
//______________________________________________________________________________
BIFF8_layout_XF XF_n =
{
   0x0000
,   0x0000
,   0xFFF5
,   0x0020
,   0xF400
,   0x0000
,   0x00000000
,   0x20C0
};
//______________________________________________________________________________
BIFF_record::BIFF_record
(Record_code record_type_code
,nat32 _record_body_length
,void *fixed_layout)
: Binary_record(record_type_code,_record_body_length,false)
, in_memory(true)
, fixed_layout(fixed_layout)
{  set_modified();  // This constructor is for creating records so we must set modified.
};
//______________________________________________________________________________
void BIFF_record::write_body(Binary_file_interface &out_file)
{  if (fixed_layout)
      out_file.write((char *)fixed_layout,get_length());
};
//______________________________________________________________________________
bool BIFF_record::read_body(Binary_file_interface &in_file)
{  if (fixed_layout)
      in_file.read((char *)fixed_layout,get_length());
   return true;
};
//______________________________________________________________________________
bool BIFF23457_string::read(Binary_file_interface &in_file,String_format string_format)
{  in_file.read_string(*this,string_format);
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF23457_string::write(Binary_file_interface &out_file,String_format string_format)
{  out_file.write_string(*this,string_format,length());                          //100114
   return true;
};
//______________________________________________________________________________
BIFF8_string::~BIFF8_string()
{  if (rich_text_formats)              delete [] rich_text_formats;
   if (asian_phonetic_settings_block)  delete [] asian_phonetic_settings_block;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
nat32 BIFF8_string::compute_size() const
{  nat32 string_size = 0;
   assert(false); // WARNING NOT YET IMPLEMENTED.
/*             nat32 size
               =
               + 2   // character count
               + 1   // nat8  options;
               + 2   // nat16 rich_text_format_count;
               + 4   // nat32 asian_phonetic_settings_block_size;
               + 2 * length()  if compressed unicode
               or length() if uncompressed unicode
               // the character array comes next in the file
               +  rich_text_format_count *
                  ( 2   //nat16  first_formatted_character; // First formatted character (zero-based)
                  + 2 ) //nat16  FONT_record_index;
               *  count up the asian_phonetic_settings_block sizes
*/
   return  string_size;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_string::read(Binary_file_interface &in_file,Data_type character_count_data_type)
{  nat16 character_count =
      (character_count_data_type == DT_nat8) // according to the open office documentation character count,
      ? in_file.read_nat8()                 // some time strings have 8bit
      : in_file.read_nat16();               // others 16bit. Microsoft documentation only mentions 16bit
   options = in_file.read_nat8();
   if (expect_RTF)
      rich_text_format_count = in_file.read_nat16();
   if (expect_asian_phonetic_settings)
      asian_phonetic_settings_block_size = in_file.read_nat32();
   clear();
   if (options & 0x01)  // compressed characters UTF16
   {  wchar_t  *buffer = new wchar_t[character_count]; // warning an aribrary large number
      in_file.read_nat16_array((nat16*)buffer,character_count);
      assign(buffer,character_count);
      delete []buffer;
   } else // uncompressed characters  ASCII
   {  nat8 *buffer = new nat8[character_count]; // warning an aribrary large number
      in_file.read_nat8_array(buffer,character_count);
      for (nat16 c_i = 0; c_i < character_count; c_i++)
         append(1,(nat16)buffer[c_i]);
      delete []buffer;
   }
   if (expect_RTF)
   {  rich_text_formats = new RichText_format[rich_text_format_count];
      for (int rt_i = 0; rt_i < rich_text_format_count; rt_i++)
          rich_text_formats[rt_i].read(in_file,DT_nat16);
   };
   if (expect_asian_phonetic_settings)
   {  asian_phonetic_settings_block = new Asian_phonetic_settings_block[asian_phonetic_settings_block_size];
      for (nat32 aps_i = 0; aps_i < asian_phonetic_settings_block_size; aps_i++)
         asian_phonetic_settings_block[aps_i].read(in_file);
   };
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_string::write(Binary_file_interface &out_file,Data_type character_count_data_type)
{  // continue here
   nat8  options;
   nat16 rich_text_format_count;
   nat32 asian_phonetic_settings_block_size;
   // the character array comes next in the file
   // NYI RichText_format  *rich_text_formats;
   // NYI Asian_phonetic_settings_block *asian_phonetic_settings_block;
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool RichText_format::read (Binary_file_interface &in_file ,Data_type data_type)
{  first_formatted_character = (data_type == DT_nat8) // according to the open office documentation character count,
      ? in_file.read_nat8()                 // some time strings have 8bit
      : in_file.read_nat16();               // others 16bit. Microsoft documentation only mentions 16bit
   FONT_record_index = (data_type == DT_nat8) // according to the open office documentation character count,
      ? in_file.read_nat8()                 // some time strings have 8bit
      : in_file.read_nat16();               // others 16bit. Microsoft documentation only mentions 16bit
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool RichText_format::write(Binary_file_interface &out_file,Data_type data_type)
{  if (data_type == DT_nat8) // according to the open office documentation character count,
         out_file.write_nat8((nat8)first_formatted_character);                 // some time strings have 8bit
   else  out_file.write_nat16(first_formatted_character);               // others 16bit. Microsoft documentation only mentions 16bit
   if (data_type == DT_nat8) // according to the open office documentation character count,
         out_file.write_nat8((nat8)FONT_record_index);                 // some time strings have 8bit
   else  out_file.write_nat16(FONT_record_index);               // others 16bit. Microsoft documentation only mentions 16bit
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_string::Asian_phonetic_settings_block::read
(Binary_file_interface &in_file /*,Data_type character_count_data_type*/)
{  unknown_identifier   = in_file.read_nat16();
   datasize             = in_file.read_nat16();
   FONT_record_index    = in_file.read_nat16();
   settings             = in_file.read_nat16();
   portions             = in_file.read_nat16();
   character_count      = in_file.read_nat16();
   for (nat16 np_i = 0; np_i < portions; np_i++)
   {
      NP_list[np_i].phonetic_text_first_char = in_file.read_nat16();;
      NP_list[np_i].main_text_first_char     = in_file.read_nat16();;
      NP_list[np_i].main_text_char_count     = in_file.read_nat16();;
   };
   return true;
};
//_2010-01-16_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_string::Asian_phonetic_settings_block::write
(Binary_file_interface &out_file/*,Data_type character_count_data_type*/)
{  out_file.write_nat16(unknown_identifier);
   out_file.write_nat16(datasize);
   out_file.write_nat16(FONT_record_index);
   out_file.write_nat16(settings);
   out_file.write_nat16(portions);
   out_file.write_nat16(character_count);
   for (nat16 np_i = 0; np_i < portions; np_i++)
   {
      out_file.write_nat16(NP_list[np_i].phonetic_text_first_char);
      out_file.write_nat16(NP_list[np_i].main_text_first_char);
      out_file.write_nat16(NP_list[np_i].main_text_char_count);
   };
   return true;
};
//______________________________________________________________________________
void BIFF_cell_record_variable::write_body(Binary_file_interface &out_file)
{  out_file.write_nat16(row);
   out_file.write_nat16(col);
};
//______________________________________________________________________________
bool BIFF_cell_record_variable::read_body(Binary_file_interface &in_file)
{  row = in_file.read_nat16();
   col = in_file.read_nat16();
   return true;
};
//______________________________________________________________________________
void BIFF_attribute_cell_record_variable::write_body(Binary_file_interface &out_file)
{  BIFF_cell_record_variable::write_body(out_file);
   out_file.write_nat8(attribute_0);
   out_file.write_nat8(attribute_1);
   out_file.write_nat8(attribute_2);
};
//______________________________________________________________________________
void BIFF_XF_cell_record_variable::write_body(Binary_file_interface &out_file)
{  BIFF_cell_record_variable::write_body(out_file);
   out_file.write_nat16(ixfe);
};
//______________________________________________________________________________
bool BIFF_XF_cell_record_variable::read_body(Binary_file_interface &in_file)
{  BIFF_cell_record_variable::read_body(in_file);
   ixfe = in_file.read_nat16();
   return true;
};
//______________________________________________________________________________
void BIFF34578_rec_NUMBER::write_body(Binary_file_interface &out_file)
{  out_file.write_nat16(body.row);
   out_file.write_nat16(body.col);
   out_file.write_nat16(body.ixfe);
   out_file.write_float64(body.value);
};
//______________________________________________________________________________
bool BIFF34578_rec_NUMBER::read_body(Binary_file_interface &in_file)
{  body.row = in_file.read_nat16();
   body.col = in_file.read_nat16();
   body.ixfe = in_file.read_nat16();
   body.value= in_file.read_float64();
   return true;
};
//______________________________________________________________________________
void BIFF_rec_BOUNDSHEET_common::write_body(Binary_file_interface &out_file)
{  out_file.write_nat32(lbPlyPos);
   out_file.write_nat16(grbit);
};
//______________________________________________________________________________
bool BIFF_rec_BOUNDSHEET_common::read_body(Binary_file_interface &in_file)
{  lbPlyPos = in_file.read_nat32();
   grbit = in_file.read_nat16();
   return true;
};
//______________________________________________________________________________
nat32 BIFF57_rec_BOUNDSHEET::compute_record_body_size()                    const
{  return  sheet_name.length() + 7;
};
//______________________________________________________________________________
void BIFF57_rec_BOUNDSHEET::write_body(Binary_file_interface &out_file)
{  BIFF_rec_BOUNDSHEET_common::write_body(out_file);
   out_file.write_string(sheet_name,ASCII_C8_string,sheet_name.length());
};
//______________________________________________________________________________
bool BIFF57_rec_BOUNDSHEET::read_body(Binary_file_interface &in_file)
{  BIFF_rec_BOUNDSHEET_common::read_body(in_file);
   in_file.read_string(sheet_name,ASCII_C8_string/*,sheet_name.length()*/);
   return true;
};
//______________________________________________________________________________
//const std::string &BIFF8_rec_BOUNDSHEET::get_sheet_name(std::string &result) const
//   {  return wstring_to_string(sheet_name,result);    };
//______________________________________________________________________________
//const std::wstring &BIFF8_rec_BOUNDSHEET::get_sheet_name(std::wstring &result) const
//   {  return result.assign(sheet_name); };
//______________________________________________________________________________
nat32 BIFF8_rec_BOUNDSHEET::compute_record_body_size()                     const
{  return  sheet_name.length() + 8; // WARNING need to check
   // Note sure if sheet name is unicode
};
//______________________________________________________________________________
void BIFF8_rec_BOUNDSHEET::write_body(Binary_file_interface &out_file)
{  BIFF_rec_BOUNDSHEET_common::write_body(out_file);
   sheet_name.write(out_file,DT_nat8);      // only 255 character for sheet name
//   bool all_chars_ASCII = true;
//   std::string sheet_name_ASCII;
//   for (int i = 0; (i < sheet_name) && all_chars_ASCII; i++)
//   {
//      nat16 char_code =  sheet_name.data()[i];
//      if (char_code < 256)
//         sheet_name_ASCII.append(1,(nat8)char_code);
//      else all_chars_ASCII = false;
//   };
//   if (all_chars_ASCII);
//      out_file.write_string(sheet_name_ASCII,ASCII_C8_string,sheet_name.length());
//   else
//   out_file.write_wstring(sheet_name,UTF16LE_C8_string,sheet_name.length());
};
//______________________________________________________________________________
bool BIFF8_rec_BOUNDSHEET::read_body(Binary_file_interface &in_file)
{  BIFF_rec_BOUNDSHEET_common::read_body(in_file);
   sheet_name.read(in_file,DT_nat8); // only 255 character for sheet name
//   if compressed
//      in_file.read_wstring(sheet_name,UTF16LE_C8_string/*,sheet_name.length()*/);
//   else
//   in_file.read_wstring(sheet_name,UTF16LE_C8_string/*,sheet_name.length()*/);
   return true;
};
//______________________________________________________________________________
std::string &BIFF8_rec_BOUNDSHEET::get_sheet_name(std::string &result)     const
   {  return wstring_to_string(sheet_name,result);    };
//______________________________________________________________________________
std::wstring &BIFF8_rec_BOUNDSHEET::get_sheet_name(std::wstring &result)   const
   {  return result.assign(sheet_name); };
//______________________________________________________________________________
#ifdef OBSOLETE
now usng layout mode
void BIFF2_rec_INTEGER::write_body(Binary_file_interface &out_file)
{
   BIFF_attribute_cell_record::write_body(out_file);
   out_file.write_sint16(value);
};
//______________________________________________________________________________
void BIFF2_rec_NUMBER::write_body(Binary_file_interface &out_file)
{
   BIFF_attribute_cell_record::write_body(out_file);
   out_file.write_float64(value);
};
#endif
//______________________________________________________________________________
nat32 BIFF2_rec_LABEL::compute_record_body_size()                          const
{  return 7+2+value.length();
};
//______________________________________________________________________________
nat32 BIFF3457_rec_LABEL::compute_record_body_size()                       const
{  return 6+2+value.length();
};
//______________________________________________________________________________
void BIFF2_rec_LABEL::write_body(Binary_file_interface &out_file)
{  BIFF_attribute_cell_record_variable::write_body(out_file);
   out_file.write_string(value,ASCII_C8_string );   // only one by for string len
};
//______________________________________________________________________________
void BIFF3457_rec_LABEL::write_body(Binary_file_interface  &out_file)
{  BIFF_XF_cell_record_variable::write_body(out_file);       // Note BIFF4_rec_LABEL will have 2 bytes for string length (not 1).
   out_file.write_string(value,ASCII_C16_string );
};
//______________________________________________________________________________
nat32 BIFF8_rec_LABEL::compute_record_body_size()                          const
{  return 0; // NYI 6+2+value.length();   need to account for Unicode
};
//______________________________________________________________________________
BIFF34578_rec_RK::BIFF34578_rec_RK
(//Record_code _record_code,
nat16  _row,nat16 _col
,nat16  _ixfe
,int32   integer_value
,bool    multiply_by_100)
:BIFF_XF_cell_record_variable(BIFF34578_CODE_RK,BIFFx_LEN_RK,_row,_col,_ixfe)
{  set_RK_integer(integer_value,multiply_by_100);
   row =_row;
   col =_col;
   ixfe =_ixfe;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
BIFF34578_rec_RK::BIFF34578_rec_RK
(nat16  _row,nat16 _col
,nat16  _ixfe
,float64   float64_value
,bool    multiply_by_100)
:BIFF_XF_cell_record_variable(BIFF34578_CODE_RK,BIFFx_LEN_RK,_row,_col,_ixfe)
{   set_RK_float(float64_value);
    row =_row;
    col =_col;
    ixfe =_ixfe;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
BIFF34578_rec_RK::BIFF34578_rec_RK
(nat16  _row,nat16 _col
,nat16  _ixfe
,nat32  _rk)
:BIFF_XF_cell_record_variable(BIFF34578_CODE_RK,BIFFx_LEN_RK,_row,_col,_ixfe)
{  rk = _rk;
   row =_row;
   col =_col;
   ixfe =_ixfe;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void BIFF34578_rec_RK::write_body(Binary_file_interface &out_file)
{  BIFF_XF_cell_record_variable::write_body(out_file);
   out_file.write_nat32(rk);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF34578_rec_RK::read_body(Binary_file_interface &in_file)
{  BIFF_XF_cell_record_variable::read_body(in_file);
   rk = in_file.read_nat32();
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
static const nat8 multiplied_by_100_mask = 0x1;
static const nat8 integer_mask           = 0x2;
int32 BIFF34578_rec_RK::set_RK_integer(int32 integer_value,bool multiply_by_100)
{  //nat32 &rk = rk;
   rk = ((multiply_by_100 ? (integer_value * 100) : integer_value)  << 2)
        | integer_mask | (multiply_by_100 ? multiplied_by_100_mask : 0);
   return integer_value;
};
//_2010-01-13_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
float64 BIFF34578_rec_RK::set_RK_float(float64 float_value,bool multiply_by_100)
{  // nat32 &rk = body.rk;
   float64 float64_value = (multiply_by_100 ? float_value * 100.0 : float_value);
   nat32 *msb = (nat32 *)((&float64_value) + 4); // WARNING need to check
   // nat32 *lsb = (nat32 *)(&float64_value);  the lsb are simply ignored
   rk = (((*msb) & 0xFFFFFFFC)) | (multiply_by_100 ? multiplied_by_100_mask : 0x0); // RK is an unscaled float
   return float64_value;
};
//_2010-01-13_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
int32 BIFF34578_rec_RK::get_RK_integer() const
{  return (int32)get_RK_float();
//    if (rk & integer_mask)
//       result = (rk >> 2);
//    if (rk & multiplied_by_100_mask)
//       result /= 100;
//    return result
};
//_2010-01-13_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
float64 BIFF34578_rec_RK::get_RK_float()   const
{  float64 result = 0.0;
   //nat32 rk = body.rk;
   if (rk & integer_mask)
      result = (float64)(rk >> 2);
   else
   { // hi words of IEEE num
      *((long *)(&result)+1) = (rk & 0xfffffffc);
      *((long *)&result) = 0;
   };
   if (rk & multiplied_by_100_mask)
      result /= 100;
   return result;
};
//_2010-01-13___________________________________________________________________
BIFF578_rec_MULRK::BIFF578_rec_MULRK(nat32 record_length)
: BIFF_record(BIFF578_CODE_MULRK,0,NULL)
, row_index(0)
, first_col_index(0)
, records(0)
, last_col_index(0)
, number_records((record_length - 6) / 6)
{  // This constructor is used when reading the file
   last_col_index =  first_col_index + number_records;
   records = new RK_record[number_records];
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF578_rec_MULRK::read_body(Binary_file_interface &in_file)
{  row_index      = in_file.read_nat16();
   first_col_index= in_file.read_nat16();
   for (nat16 rec_i = 0; rec_i < number_records; rec_i++)
   {
      records[rec_i].ixfe = in_file.read_nat16();
      records[rec_i].RK = in_file.read_nat32();
   };
   last_col_index = in_file.read_nat16();;
   return true;
};
//_1997-11-03_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
nat32 BIFF578_rec_MULRK::compute_record_body_size()            const
{  return 6 + number_records * 6;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void BIFF578_rec_MULRK::write_body(Binary_file_interface &out_file)
{  out_file.write_nat16(row_index);
   out_file.write_nat16(first_col_index);
   for (nat16 rec_i = 0; rec_i < number_records; rec_i++)
   { out_file.write_nat16(records[rec_i].ixfe );
      out_file.write_nat32(records[rec_i].RK);
   };
   out_file.write_nat16(last_col_index);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
BIFF578_rec_MULRK::~BIFF578_rec_MULRK()
{  if (records)   delete [] records;
};
//______________________________________________________________________________
void BIFF_rec_PANE::freeze
(nat16 _left_most_column
,nat32 _top_most_row
,Panes _active_pane)
{  set_modified(true);
   body.x         = _left_most_column;
   body.y         = _top_most_row;
   body.rwTop     = _left_most_column;
   body.colLeft   = _top_most_row;
   body.pnnAct    = _active_pane;
};
//______________________________________________________________________________
void BIFF_rec_SELECTION::write_body(Binary_file_interface &out_file)
{  out_file.write_nat8((nat8)pnn);
   out_file.write_nat16(rwAct);
   out_file.write_nat16(colAct);
   out_file.write_nat16(irefAct);
   out_file.write_nat16(array_of_refs.count());
   FOR_EACH_IN(ref,Ref ,array_of_refs,each_ref)                                  //100111
   {   ref->write_ref(out_file);
   } FOR_EACH_END(each_ref);
};
//______________________________________________________________________________
nat32 BIFF_rec_SELECTION::compute_record_body_size() const
{  nat16 size = sizeof(pnn);
   size += sizeof(rwAct);
   size += sizeof(colAct);
   size += sizeof(irefAct);
   size += sizeof(6 * array_of_refs.count());
   return size;
};  // cannot be const!!!
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void BIFF_rec_SELECTION::Ref::write_ref(Binary_file_interface &out_file)
{  out_file.write_nat16(rwFirst);
   out_file.write_nat16(rwLast);
   out_file.write_nat16(colFirst);
   out_file.write_nat16(colLast);
};
//_2011-01-11___________________________________________________________________
void BIFF2_rec_FONT::write_body(Binary_file_interface &out_file)
{  out_file.write_nat16(dyHeight);
   out_file.write_nat16(grbit);
   out_file.write_string(font_name,ASCII_C8_string);    //apparently only to 256 characters
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
nat32 BIFF2_rec_FONT::compute_record_body_size() const  // obs cannot be const!!!
{  return font_name.length() + 4;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void BIFF57_rec_FONT::write_body(Binary_file_interface &out_file)
{  out_file.write_nat16(dyHeight);
   out_file.write_nat16(grbit);

   out_file.write_nat16(icv);     // index to the color palette
   out_file.write_nat16(bls);     // bold style
   out_file.write_nat16(sss);     // super/subscript
   out_file.write_nat8(uls);     // underline style
   out_file.write_nat8(bFamily);    // Font family as defined by the Win API LOGFONT structure
   out_file.write_nat8(bCharSet);   // Characers set as defined by the Win API LOGFONT structure
   out_file.write_nat8(reserved);
   out_file.write_string(font_name,ASCII_C8_string);
};
//______________________________________________________________________________
nat32 BIFF57_rec_FONT::compute_record_body_size() const  // obs: {        } break; cannot be const!!!
{
   return font_name.length() + 14;
};
//______________________________________________________________________________
BIFF57_rec_XF::BIFF57_rec_XF(nat8 *default_values)
: BIFF_rec_XF(BIFF578_CODE_XF,sizeof(BIFF57_rec_XF),&body)
{
   #ifdef __BCPLUSPLUS__
   std::
   #endif
   memcpy((void *)&body,(void *)default_values,sizeof(BIFF57_rec_XF));
};
//______________________________________________________________________________
BIFF8_rec_SST::Item::Item(const std::string &str)
: CORN::Item()
, BIFF8_string(L"",false,false) // doesn't appear to have RTF or asian font
{  string_to_wstring(str,*this); };
//_ _ _ _ _ _ _ _ _ _
/* //170424 replaced with label_wstring
const char *BIFF8_rec_SST::Item::label_cstr_deprecated(char *result)                  const //050619
{  std::string str;
   wstring_to_string(*this,str);
   strcpy(result,str.c_str());
   return result;
};
//_ _ _ _ _ _ _ _ _ _
*/
const char *BIFF8_rec_SST::Item::label_string(std::string &result)         const
{  wstring_to_string(*this,result);
   return result.c_str();
}
//_2017-04-23 _ _ _ _ _ _ _ _ _
void BIFF8_rec_SST::write_body(Binary_file_interface &out_file)
{  out_file.write_nat32(total_string_count);
   unique_string_count = count();
   out_file.write_nat32(unique_string_count);
   FOR_EACH(item,BIFF8_rec_SST::Item,each_item)
   {  item->BIFF8_string::write(out_file,DT_nat16);
   } FOR_EACH_END(each_item);
};
//_ _ _ _ _ _ _ _ _ _
bool BIFF8_rec_SST::read_body(Binary_file_interface &in_file)
{  total_string_count  = in_file.read_nat32();
   unique_string_count = in_file.read_nat32();
   for (uint32 str_i = 0; str_i < unique_string_count; str_i++)
   {  BIFF8_rec_SST::Item *item = new BIFF8_rec_SST::Item(""); // Does not appear to have RTF or Asian settings
      item->read(in_file ,DT_nat16 /*UTF16LE_string*/);
      append(item);
   };
   return true;
};
//______________________________________________________________________________
void BIFF34578_rec_FORMULA_common::write_body(Binary_file_interface &out_file)
{
//   BIFF34578_rec_NUMBER::write_body(out_file);
//   out_file.write((const char *)&(body) ,14/*sizeof(BIFF34578_layout_NUMBER)*/);
   out_file.write_nat16(body.row);
   out_file.write_nat16(body.col);
   out_file.write_nat16(body.ixfe);
   out_file.write_float64(body.value);

   out_file.write_nat16(options);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
void BIFF578_rec_FORMULA::write_body(Binary_file_interface &out_file)
{
   BIFF34578_rec_FORMULA_common::write_body(out_file);
   out_file.write_nat32(unused);
   nat32 token_array_byte_count = get_token_array_byte_count();
   // Warning write body here is assuming we have read this record from
   // an existing file and are simply copying back the data
   // because I currently have not using the token array.
   out_file.write_string(RPN_token_array,ASCII_string,token_array_byte_count);
};

//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF34578_rec_FORMULA_common::read_body(Binary_file_interface &in_file)
{  body.row = in_file.read_nat16();     // 0-based
   body.col = in_file.read_nat16();     // 0-based
   body.ixfe = in_file.read_nat16();
   in_file.read((char *)&formula_result,8);
   if (formula_result.non_numeric.identifier <= 3)
   {   body.value = 0.0;
   } else
      body.value = formula_result.value; // in_file.read_float64();
   options = in_file.read_nat16();
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF578_rec_FORMULA::read_body(Binary_file_interface &in_file)
{  BIFF34578_rec_FORMULA_common::read_body(in_file);
   unused = in_file.read_nat32();
   nat32 token_array_byte_count = get_token_array_byte_count();
   // Warning write body here is assuming we have read this record from
   // an existing file and are simply copying back the data
   // because I currently have not using the token array.
   in_file.seek(token_array_byte_count,std::ios_base::cur);
   // Ignoring the RPN_token_array
   return true;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
Data_type BIFF34578_rec_FORMULA_common::get_result_data_type() const
{  Data_type return_type;
   const FORMULA_Result_union *result = reinterpret_cast<const FORMULA_Result_union *>(&body.value);
   switch (result->non_numeric.identifier)
   {  case 0x00 : return_type = DT_string; break;
      case 0x01 : return_type = DT_bool; break;
      case 0x02 : return_type = DT_not_assigned; break; // actually error code
      case 0x03 : return_type = DT_not_assigned; break; // actually empty cell
   };
   return return_type;
};
//______________________________________________________________________________
void BIFF2_rec_STRING::write_body(Binary_file_interface &out_file)
{  out_file.write_string(text,ASCII_C8_string);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF2_rec_STRING::read_body(Binary_file_interface &in_file)
{  in_file.read_string(text,ASCII_C8_string);
   return true;
};
//______________________________________________________________________________
void BIFF3457_rec_STRING::write_body(Binary_file_interface &out_file)
{  out_file.write_string(text,ASCII_C16_string);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF3457_rec_STRING::read_body(Binary_file_interface &in_file)
{  in_file.read_string(text,ASCII_C16_string);
   return true;
};
//______________________________________________________________________________
void BIFF8_rec_STRING::write_body(Binary_file_interface &out_file)
{  text.write(out_file,DT_nat16);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_rec_STRING::read_body(Binary_file_interface &in_file)
{  text.read(in_file,DT_nat16);
   return true;
};
//______________________________________________________________________________
void BIFF8_rec_LABELSST::write_body(Binary_file_interface &out_file)
{  BIFF_XF_cell_record_variable::write_body(out_file);
   out_file.write_nat32(SST_index);
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
bool BIFF8_rec_LABELSST::read_body(Binary_file_interface &in_file)
{  BIFF_XF_cell_record_variable::read_body(in_file);
   SST_index = in_file.read_nat32();
   return true;
};
//______________________________________________________________________________
BIFF_rec_BOF *render_BIFF_BOF_record(Record_code code,nat32 record_length)
{  BIFF_rec_BOF *BOF_rec = 0;
   switch (code)
   {  case BIFF2_CODE_BOF    : { BOF_rec = new BIFF2_rec_BOF;     } break;
      case BIFF3_CODE_BOF    : { BOF_rec = new BIFF34_rec_BOF(BIFF3);        } break;  // 0x0209
      case BIFF4_CODE_BOF    : { BOF_rec = new BIFF34_rec_BOF(BIFF4); } break;
      case BIFF578_CODE_BOF  :
      {  switch (record_length)
         {  case BIFF57_LEN_BOF : BOF_rec = new BIFF57_rec_BOF();        break;  // 0x0809 /* @ 5 7 8 */
            case BIFF8_LEN_BOF  : BOF_rec = new BIFF8_rec_BOF();        break;  // 0x0809 /* @ 5 7 8 */
         };
      } break;
   };
   return BOF_rec;
};
//______________________________________________________________________________
BIFF_record *render_BIFF2_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF2_CODE_BOF                      : {        } break;  // 0x09
      case BIFF2_CODE_INTEGER                  : { record = new BIFF2_rec_INTEGER();  } break;  // 0x02
      case BIFF2_CODE_NUMBER                   : { record = new BIFF2_rec_NUMBER();   } break;  // 0x03
      case BIFF2_CODE_LABEL                    : { record = new BIFF2_rec_LABEL();    } break;  // 0x04
      case BIFF2_CODE_STRING                   : { record = new BIFF2_rec_STRING();       } break;  // 0x07

#ifdef NYN
      // Declared but not tested
      case BIFF2578_CODE_FONT                  : { record = new BIFF2_rec_FONT();      } break;  // 0x31 /* @ FONT - Font Description */
      case BIFF2_CODE_DIMENSIONS               : { record = new BIFF2_rec_DIMENSIONS; } break;  // 0x00
      case BIFF2_CODE_WINDOW2                  : { record = new BIFF2_rec_WINDOW2;      } break;  // 0x3E
      case BIFF2_CODE_ROW                      : { record = new BIFF2_rec_ROW;     } break;  // 0x08

      // not tested
      case BIFF2_CODE_BLANK                    : {        } break;  // 0x01
      case BIFF2_CODE_BOOLERR                  : {        } break;  // 0x05
      case BIFF2_CODE_INDEX                    : {        } break;  // 0x0B
      case BIFF2_CODE_FORMATCOUNT              : {        } break;  // 0x1F
      case BIFF2_CODE_COLUMN_DEFAULT           : {        } break;  // 0x20
      case BIFF2_CODE_ARRAY                    : {        } break;  // 0x21
      case BIFF2_CODE_COLWIDTH                 : {        } break;  // 0x24
      case BIFF2_CODE_DEFAULT_ROW_HEIGHT       : {        } break;  // 0x25
      case BIFF2_CODE_FONT2                    : {        } break;  // 0x32
      case BIFF2_CODE_TABLE                    : {        } break;  // 0x36
      case BIFF2_CODE_TABLE2                   : {        } break;  // 0x37
      case BIFF2_CODE_XF                          : {        } break;  // 0x43
      case BIFF2_CODE_IXFE                        : {        } break;  // 0x44
      case BIFF2_CODE_FONTCOLOR                   : {        } break;  // 0x45
      case BIFF2_CODE_RK                         : {        } break;  // 0x7E  /* RK - Cell values in RK format */
#endif
   };
   return record;
};
//_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
BIFF_record *render_BIFF8_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {
      case BIFF578_CODE_BOF            : { record = new BIFF8_rec_BOF();      } break;
      case BIFF34578_CODE_LABEL        : { record = new BIFF8_rec_LABEL();    } break;
      case BIFF34578_CODE_WINDOW2      : { record = new BIFF8_rec_WINDOW2;    } break;
      case BIFF578_CODE_BOUNDSHEET     : { record = new BIFF8_rec_BOUNDSHEET("Sheet1");       } break;
      case BIFF8_CODE_SST              : { record = new BIFF8_rec_SST;        } break;
      case BIFF34578_CODE_STRING       : { record = new BIFF8_rec_STRING;     } break;
      case BIFF8_CODE_LABELSST         : { record = new BIFF8_rec_LABELSST;   } break;  // 0xFD /* @ LABELSST - Cell Value, String Constant/SST (May have been in BIFF4)*/

#ifdef NYN
      // Declared but not yet tested
      case BIFF2578_CODE_FONT                   : { record = new BIFF8_rec_FONT;       } break;  // 0x31 /* @ FONT - Font Description */
      case BIFF578_CODE_XF                      : { record = new BIFF8_rec_XF;       } break;  // 0xE0  /* XF - Extended Format */
      case BIFF34578_CODE_DIMENSIONS   : { record = format == BIFF8 ? BIFF8_rec_DIMENSIONS : new BIFF3457_rec_DIMENSIONS       } break;  // 0x0200  /* DIMENSIONS - Cell Table Size */

      // Not yet declared
      case BIFF8_CODE_MERGEDCELLS                : {        } break;  // 0xE5  /* ?UNKNOWN1 - Guessing */
      case BIFF8_CODE_BITMAP                     : {        } break;  // 0xE9

      case BIFF8_CODE_PHONETICPR                 : {        } break;  // 0xEF
      case BIFF8_CODE_SXRULE                     : {        } break;  // 0xF0  /* SXRULE - PivotTable Rule Data (May have been in BIFF4)*/
      case BIFF8_CODE_SXEX                       : {        } break;  // 0xF1  /* SXEX - PivotTable View Extended Information (May have been in BIFF4)*/
      case BIFF8_CODE_SXFILT                     : {        } break;  // 0xF2  /* SXFILT - PivotTable Rule Filter (May have been in BIFF4)*/
      case BIFF8_CODE_SXNAME                     : {        } break;  // 0xF6 /* SXNAME - PivotTable Name (May have been in BIFF4)*/
      case BIFF8_CODE_SXSELECT                   : {        } break;  // 0xF7 /* SXSELECT - PivotTable Selection Information (May have been in BIFF4)*/
      case BIFF8_CODE_SXPAIR                     : {        } break;  // 0xF8  /* SXPAIR - PivotTable Name Pair (May have been in BIFF4)*/
      case BIFF8_CODE_SXFMLA                     : {        } break;  // 0xF9    /* SXFMLA - PivotTable Parsed Expression (May have been in BIFF4)*/
      case BIFF8_CODE_SXFORMAT                   : {        } break;  // 0xFB    /* SXFORMAT - PivotTable Format Record (May have been in BIFF4)*/
      case BIFF8_CODE_EXTSST                     : {        } break;  // 0xFF  /* EXTSST - Extended Shared String Table (May have been in BIFF4)*/

      case BIFF8_CODE_SXVDEX                    : {        } break;  // 0x100  /* SXVDEX - Extended PivotTable View Fields (May have been in BIFF4)*/
      case BIFF8_CODE_SXFORMULA                 : {        } break;  // 0x103 /* SXFORMULA - PivotTable Formula Record (May have been in BIFF4)*/
      case BIFF8_CODE_SXDBEX                    : {        } break;  // 0x122 /* SXDBEX - PivotTable Cache Data (May have been in BIFF4)*/

      case BIFF8_CODE_SXDB                      : {        } break;  // 0xC6 /* SXDB - PivotTable Cache Data (May have been in BIFF4)*/
      case BIFF8_CODE_LABELRAGNES               : {        } break;  // 0x15F
      case BIFF8_CODE_USESELFS                  : {        } break;  // 0x160  /* USESELFS - Natural Language Formulas Flag (May have been in BIFF4)*/
      case BIFF8_CODE_DSF                       : {        } break;  // 0x161       /* DSF - Double Stream File (May have been in BIFF4)*/
      case BIFF8_CODE_XL5MODIFY                 : {        } break;  // 0x162  /* XL5MODIFY - Flag for DSF (May have been in BIFF4)*/
      case BIFF8_CODE_USERBVIEW                 : {        } break;  // 0x1A9 /* USERBVIEW - Workbook Custom View Settings (May have been in BIFF4)*/
      case BIFF8_CODE_USERSVIEWBEGIN            : {        } break;  // 0x1AA /* USERSVIEWBEGIN - Custom View Settings (May have been in BIFF4)*/
      case BIFF8_CODE_USERSVIEWEND              : {        } break;  // 0x1AB /* USERSVIEWEND - End of Custom View Records (May have been in BIFF4)*/
      case BIFF8_CODE_QSI                       : {        } break;  // 0x1AD /* QSI - External Data Range */
      case BIFF8_CODE_SUPBOOK                   : {        } break;  // 0x1AE /* SUPBOOK - AKA EXTERNALBOOK - Supporting Workbook */
      case BIFF8_CODE_EXTERNALBOOK              : {        } break;  // 0x1AE /* EXTERNALBOOK - AKA SUPBOOK - Supporting Workbook */
      case BIFF8_CODE_CONDFMT                   : {        } break;  // 0x1B0 /* CONDFMT - AKA CFHEADER - Conditional Formatting Range Information */
      case BIFF8_CODE_CFHEADER                  : {        } break;  // 0x1B0 /* CFHEADER - AKA CONDFMT - Conditional Formatting Range Information */*/
      case BIFF8_CODE_CF                        : {        } break;  // 0x1B1 /* CF - Conditional Formatting Conditions (May have been in BIFF4) */
      case BIFF8_CODE_DVAL                      : {        } break;  // 0x1B2 /* DVAL - AKA DATAVALIDATIONS - Data Validation Information (May have been in BIFF4)*/
      case BIFF8_CODE_DATAVALIDATIONS           : {        } break;  // 0x1B2 /* DATAVALIDATIONS - AKA DVAL - Data Validation Information (May have been in BIFF4)*/

      case BIFF8_CODE_DCONBIN                   : {        } break;  // 0x1B5 /* DCONBIN - Data Consolidation Information (May have been in BIFF4)*/
      case BIFF8_CODE_TXO                       : {        } break;  // 0x1B6 /* TXO - Text Object (May have been in BIFF4)*/
      case BIFF8_CODE_REFRESHALL                : {        } break;  // 0x1B7 /* REFRESHALL - Refresh Flag (May have been in BIFF4)*/
      case BIFF8_CODE_HYPERLINK                 : {        } break;  // 0x1B8 /* HLINK - Hyperlink (May have been in BIFF4)*/
      case BIFF8_CODE_CODENAME                  : {        } break;  // 0x1BA /* CODENAME - xxx*/
      case BIFF8_CODE_SXFDBTYPE                 : {        } break;  // 0x1BB /* SXFDBTYPE - SQL Datatype Identifier (May have been in BIFF4)*/
      case BIFF8_CODE_PROT4REVPASS              : {        } break;  // 0x1BC /* PROT4REVPASS - Shared Workbook Protection Password (May have been in BIFF4)*/
      case BIFF8_CODE_DATAVALIDATION            : {        } break;  // 0x1BE /* DATAVALIDATION - AKA DV - Data Validation Criteria (May have been in BIFF4)*/

      case BIFF8_CODE_QUICKTIP                 : {        } break;  // 0x0800
      case BIFF8_SHEETLAYOUT                   : {        } break;  // 0x0862
      case BIFF8_SHEEPROTECTION                : {        } break;  // 0x0867
      case BIFF8_RANGEROTECTION                : {        } break;  // 0x0868
#endif
   }; //switch
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF23_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
#ifdef NYN
   switch (code)
   {  case BIFF23_CODE_FORMAT                  : {        } break;  // 0x1E
   };
#endif
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF234_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF234578_CODE_WINDOW1                  : { record = new BIFF234_rec_WINDOW1;       } break;  // 0x1E
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF2578_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
#ifdef NYN
   switch (code)
   {  case BIFF2578_CODE_NAME                  : {        } break;  // 0x18
      case BIFF2578_CODE_EXTERNNAME            : {        } break;  // 0x23
   };
#endif
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF3_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF3_CODE_BOF                       : { record = new BIFF34_rec_BOF(BIFF3);        } break;  // 0x0209
#ifdef NYN
      case BIFF3_CODE_XF                        : {        } break;  // 0x243
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF34_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {
#ifdef NYN
      // Declared but not tested
      case BIFF34_CODE_FORMULA                   : { record = new BIFF34_rec_FORMULA;  } break;
      case BIFF34_CODE_FONT                     : { record = new BIFF34_rec_FONT; } break;  // 0x231
      // not yet declared
      case BIFF34_CODE_BUILTINFMTCOUNT          : {     } break;  // 0x56
      case BIFF34_CODE_DEFINEDNAME              : {        } break;  // 0x218  /* DEFINEDNAME - AKA NAME - Defined Name */
      case BIFF34_CODE_EXTERNNAME               : {        } break;  // 0x223 /* EXTERNNAME - Externally Referenced Name */
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF4_record(Record_code code,nat32 record_length,BIFF_Format format)
{  // Note the SXxxxx records may also be applicable to later BIFF formats,
   // but they are not listed in the MicroSoft documentation so I can't tell
   BIFF_record *record = 0;
   switch (code)
   {  case BIFF4_CODE_BOF                      : { record = new BIFF34_rec_BOF(BIFF4); } break;
#ifdef NYN
      case BIFF4_CODE_SXSTRING                   : {        } break;  // 0xCD /* SXSTRING - String */
      case BIFF4_CODE_TABID                     : {        } break;  // 0x13D
      case BIFF4_CODE_FILESHARING2              : {        } break;  // 0x1A5 /* FILESHARING2 - File-Sharing Information for Shared Lists */


      case BIFF4_CODE_XF                        : {        } break;  // 0x443
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF457_record(Record_code code,nat32 record_length,BIFF_Format format)
{
   BIFF_record *record = 0;
#ifdef NYN
   switch (code)
   {
      case BIFF45x_CODE_GCW                       : {        } break;  // 0xAB  /* GCW - Global Column-Width Flags */
                  // in BIFF7?
   };
#endif
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF57_record(Record_code code,nat32 record_length,BIFF_Format format)
{
   BIFF_record *record = 0;
   switch (code)
   {
      case BIFF578_CODE_BOF            : { record = new BIFF57_rec_BOF(format);       } break;
      case BIFF578_CODE_BOUNDSHEET     : { record = new BIFF57_rec_BOUNDSHEET("");       } break;  // 0x85   /* @ BOUNDSHEET - Sheet Information   5,7  changed in 8  */
#ifdef NYN
      // declared but not tested
      case BIFF2578_CODE_FONT          : { record = new BIFF57_rec_FONT;       } break;
      case BIFF578_CODE_XF             : { record = new BIFF57_rec_XF;       } break;  // 0xE0  /* XF - Extended Format */
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF578_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF578_CODE_MULRK          : { record = new BIFF578_rec_MULRK(record_length); } break;
      case BIFF234578_CODE_WINDOW1     : { record = new BIFF578_rec_WINDOW1();            } break;
      case BIFF2578_CODE_FORMULA       : { record = new BIFF578_rec_FORMULA(record_length);    } break;  // 0x06

#ifdef NYN

      case BIFF578_CODE_SORT           : {        } break;  // 0x90  /* SORT - Sorting Options */
      case BIFF578_CODE_SHRFMLA        : {        } break;  // 0xBC   /* SHRFMLA - AKA SHAREDFMLA Shared Formula */
      case BIFF578_CODE_SHAREDFMLA     : {        } break;  // 0xBC  /* SHAREDFMLA - AKA SHRFMLA  - Shared Formula */
      case BIFF578_CODE_MULBLANK       : {        } break;  // 0xBE    /* * MULBLANK - Multiple Blank Cells */
      case BIFF578_CODE_RSTRING        : {        } break;  // 0xD6   /* @ RSTRING - Cell with Character Formatting */
      case BIFF578_CODE_DBCELL         : {        } break;  // 0xD7  /* DBCELL - Stream Offsets */
      case BIFF578_CODE_BOOKBOOL       : {        } break;  // 0xDA /* BOOKBOOL - Workbook Option Flag */
      case BIFF578_CODE_SCENPROTECT    : {        } break;  // 0xDD  /* SCENPROTECT - Scenario Protection */
      case BIFF578_CODE_SHARED_FORMULA : {        } break;  // 0x4BC /* Formula related, always before there are: {        } break;  // 0x06 (FORMULA) */
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF4578_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {
#ifdef NYN
      case BIFF4578_CODE_SCL                     : {        } break;  // 0xA0 /* SCL - Window Zoom Magnification */
      case BIFF4578_CODE_SETUP                   : {        } break;  // 0xA1  /* SETUP - Page Setup */
      case BIFF4578_CODE_STANDARDWIDTH           : {        } break;  // 0x99  /* STANDARDWIDTH - Standard Column Width */
      case BIFF4578_CODE_FORMAT                 : {        } break;  // 0x41E  /* FORMAT - Number Format */
#endif
    };
    return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF3457_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF34578_CODE_LABEL        : { record = new BIFF3457_rec_LABEL(0,0,0,"");       } break;
      case BIFF34578_CODE_WINDOW2      : { record = new BIFF3457_rec_WINDOW2;       } break;
      case BIFF34578_CODE_STRING       : { record = new BIFF3457_rec_STRING;       } break;
#ifdef NYN
      // Declared but not tested
      case BIFF34578_CODE_DIMENSIONS   : { record = new BIFF3457_rec_DIMENSIONS;       } break;  // 0x0200  /* DIMENSIONS - Cell Table Size */
#endif
    };
    return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF34578_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF34578_CODE_NUMBER       : { record = new BIFF34578_rec_NUMBER;    } break;  // 0x203 /* NUMBER - Cell Value, Floating-Point Number */
      case BIFF34578_CODE_RK           : { record = new BIFF34578_rec_RK(0,0,0,0.1,false);      } break;  // 0x27E /* RK - Cell Value, RK Number */
#ifdef NYN
      // Declared but not tested
      case BIFF34578_CODE_ROW          : { record = new BIFF34578_rec_ROW;       } break;  // 0x208 /* ROW - Describes a Row */
      case BIFF34578_CODE_STYLE        : { record = new BIFF34578_rec_STYLE;       } break;  // 0x293 /* STYLE - Style Information */

      // Not declared
      case BIFF34578_CODE_XCT          : {        } break;  // 0x59 /* XCT - CRN Record Count */
      case BIFF34578_CODE_CRN          : {        } break;  // 0x5A /* CRN - Nonresident Operands */
      case BIFF34578_CODE_FILESHARING  : {        } break;  // 0x5B  /* FILESHARING - File-Sharing Information */
      case BIFF34578_CODE_WRITEACCESS  : {        } break;  // 0x5C  /* @ WRITEACCESS - Write Access User Name */
      case BIFF34578_CODE_UNCALCED     : {        } break;  // 0x5E /* UNCALCED - Recalculation Status */
      case BIFF34578_CODE_SAVERECALC   : {        } break;  // 0x5F /* SAVERECALC - Recalculate Before Save */
      case BIFF34578_CODE_OBJPROTECT   : {        } break;  // 0x63 /* OBJPROTECT - Objects Are Protected */
      case BIFF34578_CODE_COLINFO      : {        } break;  // 0x7D /* COLINFO - Column Formatting Information */
      case BIFF34578_CODE_GUTS         : {        } break;  // 0x80    /* GUTS - Size of Row and Column Gutters */
      case BIFF34578_CODE_SHEETPR      : {        } break;  // 0x81 /* WSBOOL - Additional Workspace Information */
      case BIFF34578_CODE_GRIDSET      : {        } break;  // 0x82 /* GRIDSET - State Change of Gridlines Option */
      case BIFF34578_CODE_HCENTER      : {        } break;  // 0x83 /* HCENTER - Center Between Horizontal Margins */
      case BIFF34578_CODE_VCENTER      : {        } break;  // 0x84 /* VCENTER - Center Between Vertical Margins */
      case BIFF34578_CODE_WRITEPROT    : {        } break;  // 0x86 /* WRITEPROT - Workbook Is Write-Protected */
      case BIFF34578_CODE_COUNTRY      : {        } break;  // 0x8C /* COUNTRY - Default Country and WIN.INI Country */
      case BIFF34578_CODE_HIDEOBJ      : {        } break;  // 0x8D /* @ HIDEOBJ - Object Display Options */
      case BIFF34578_CODE_PALETTE      : {        } break;  // 0x92  /* PALETTE - Color Palette Definition */
      case BIFF34578_CODE_BLANK        : {        } break;  // 0x201 /* BLANK - Cell Value, Blank Cell */

      case BIFF34578_CODE_BOOLERR      : {        } break;  // 0x205 /* BOOLERR - Cell Value, Boolean or Error */

      case BIFF34578_CODE_INDEX        : {        } break;  // 0x20B /* INDEX - Index Record */
      case BIFF34578_CODE_ARRAY        : {        } break;  // 0x221  /* ARRAY - Array-Entered Formula */
      case BIFF34578_CODE_DEFAULTROWHEIGHT      : {        } break;  // 0x225 /* DEFAULTROWHEIGHT - Default Row Height */
      case BIFF34578_CODE_TABLE        : {        } break;  // 0x236 /* TABLE - Data Table (enchanced)*/
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF2345_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
#ifdef NYN
   switch (code)
   {  case BIFF2345_CODE_EXTERNCOUNT           : {        } break;  // 0x16
            // may also actually be in BIFF7
   };
#endif
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF234578_record(Record_code code,nat32 record_length,BIFF_Format format)
{  BIFF_record *record = 0;
   switch (code)
   {  case BIFF234578_CODE_EOF                 : { record = new BIFF_rec_EOF();       } break;
      case BIFF234578_CODE_1904                : { record = new BIFF234578_rec_1904;  } break;
      case BIFF234578_CODE_PANE                : { record = new BIFF_rec_PANE;       } break;  // 0x41  /* PANE - Number of Panes and Their Position */
#ifdef NYN
      case BIFF234578_CODE_CALCCOUNT           : {        } break;  // 0x0C
      case BIFF234578_CODE_CALCMODE            : {        } break;  // 0x0D
      case BIFF234578_CODE_PRECISION           : {        } break;  // 0x0E
      case BIFF234578_CODE_REFMODE             : {        } break;  // 0x0F
      case BIFF234578_CODE_DELTA               : {        } break;  // 0x10
      case BIFF234578_CODE_ITERATION           : {        } break;  // 0x11
      case BIFF234578_CODE_PROTECT             : {        } break;  // 0x12
      case BIFF234578_CODE_PASSWORD            : {        } break;  // 0x13
      case BIFF234578_CODE_HEADER              : {        } break;  // 0x14
      case BIFF234578_CODE_FOOTER              : {        } break;  // 0x15
      case BIFF234578_CODE_EXTERNSHEET         : {        } break;  // 0x17

      case BIFF234578_CODE_WINDOW_PROTECT      : {        } break;  // 0x19
      case BIFF234578_CODE_VERTICAL_PAGE_BREAKS: {        } break;  // 0x1A  /* VERTICALPAGEBREAKS - Explicit Column Page Breaks */
      case BIFF234578_CODE_HORIZONTAL_PAGE_BREAKS    : {        } break;  // 0x1B /* HORIZONTALPAGEBREAKS - Explicit Row Page Breaks */
      case BIFF234578_CODE_NOTE                : {        } break;  // 0x1C
      case BIFF234578_CODE_SELECTION           : {        } break;  // 0x1D

      case BIFF234578_CODE_DATEMODE            : {        } break;  // 0x22  /* DATEMODE - AKA 1904 */
      case BIFF234578_CODE_LEFT_MARGIN         : {        } break;  // 0x26
      case BIFF234578_CODE_RIGHT_MARGIN        : {        } break;  // 0x27
      case BIFF234578_CODE_TOP_MARGIN          : {        } break;  // 0x28
      case BIFF234578_CODE_BOTTOM_MARGIN       : {        } break;  // 0x29
      case BIFF234578_CODE_PRINT_ROW_HEADERS   : {        } break;  // 0x2A
      case BIFF234578_CODE_PRINT_GRIDLINES     : {        } break;  // 0x2B
      case BIFF234578_CODE_FILEPASS            : {        } break;  // 0x2F
      case BIFF234578_CODE_CONTINUE            : {        } break;  // 0x3C /* CONTINUE - Continues Long Records */
      case BIFF234578_CODE_BACKUP              : {        } break;  // 0x40 /* BACKUP - Save Backup Version of the File */
      case BIFF234578_CODE_CODEPAGE               : {        } break;  // 0x42 /* @ CODEPAGE - Default Code Page */
      case BIFF234578_CODE_PLS                   : {        } break;  // 0x4D /* PLS - Environment-Specific Print Record */
      case BIFF234578_CODE_DCONREF               : {        } break;  // 0x51     /* DCONREF - Data Consolidation References */
      case BIFF234578_CODE_DEFCOLWIDTH           : {        } break;  // 0x55 /* DEFCOLWIDTH - Default Width for Columns */
#endif
   };
   return record;
};
//______________________________________________________________________________
BIFF_record *render_other_record(Record_code code,nat32 record_length,BIFF_Format format)
{ // BIFFX indicates I dont know which BIFF version the record is applicable
   BIFF_record *record = 0;
#ifdef NYN
   switch (code)
   {  case BIFF_CODE_DCONNAME                    : {        } break;  // 0x52 /* DCONNAME - Data Consolidation Named References */

      case BIFFX_CODE_DCON                       : {        } break;  // 0x50 /* DCON - Data Consolidation Information */
      case BIFFX_CODE_OBJ                        : {        } break;  // 0x5D  /* OBJ - Describes a Graphic Object */
      case BIFFx_CODE_ADDIN                      : {        } break;  // 0x87  /* ADDIN - Workbook Is an Add-in Macro */
      case BIFFx_CODE_EDG                        : {        } break;  // 0x88 /* EDG - Edition Globals */
      case BIFFx_CODE_PUB                        : {        } break;  // 0x89  /* PUB - Publisher */
      case BIFFx_CODE_SUB                        : {        } break;  // 0x91  /* SUB - Subscriber */
      case BIFFX_CODE_TEMPLATE                   : {        } break;  // 0x60 /* TEMPLATE - Workbook Is a Template */
      case BIFFx_CODE_IMDATA                     : {        } break;  // 0x7F /* IMDATA - Image Data */
      case BIFFx_CODE_LHRECORD                   : {        } break;  // 0x94    /* LHRECORD - .WK? File Conversion Information */
      case BIFFx_CODE_LHNGRAPH                   : {        } break;  // 0x95 /* LHNGRAPH - Named Graph Information */
      case BIFFx_CODE_SOUND                      : {        } break;  // 0x96    /* SOUND - Sound Note */
      case BIFFx_CODE_LPR                        : {        } break;  // 0x98   /* LPR - Sheet Was Printed Using LINE.PRINT */
      case BIFFx_CODE_FNGROUPNAME                : {        } break;  // 0x9A  /* FNGROUPNAME - Function Group Name */
      case BIFFx_CODE_FILTERMODE                 : {        } break;  // 0x9B   /* FILTERMODE - Sheet Contains Filtered List */
      case BIFFx_CODE_FNGROUPCOUNT               : {        } break;  // 0x9C /* FNGROUPCOUNT - Built-in Function Group Count */
      case BIFFx_CODE_AUTOFILTERINFO             : {        } break;  // 0x9D  /* AUTOFILTERINFO - Drop-Down Arrow Count */
      case BIFFx_CODE_AUTOFILTER                 : {        } break;  // 0x9E /* AUTOFILTER - AutoFilter Data */

      case BIFFx_CODE_COORDLIST                  : {        } break;  // 0xA9  /* COORDLIST - Polygon Object Vertex Coordinates */
      case BIFFx_CODE_SCENMAN                    : {        } break;  // 0xAE  /* SCENMAN - Scenario Output Data */
      case BIFFx_CODE_SCENARIO                   : {        } break;  // 0xAF  /* SCENARIO - Scenario Data� */
      case BIFFx_CODE_SXVIEW                     : {        } break;  // 0xB0  /* SXVIEW - View Definition */
      case BIFFx_CODE_SXVD                       : {        } break;  // 0xB1  /* SXVD - View Fields */
      case BIFFx_CODE_SXVI                       : {        } break;  // 0xB2   /* SXVI - View Item */

      case BIFFx_CODE_SXIVD                      : {        } break;  // 0xB4  /* SXIVD - Row/Column Field IDs */
      case BIFFx_CODE_SXLI                       : {        } break;  // 0xB5 /* SXLI - Line Item Array */
      case BIFFx_CODE_SXPI                       : {        } break;  // 0xB6  /* SXPI - Page Item */

      case BIFFx_CODE_DOCROUTE                   : {        } break;  // 0xB8  /* DOCROUTE - Routing Slip Information */
      case BIFFx_CODE_RECIPNAME                  : {        } break;  // 0xB9 /* RECIPNAME - Recipient Name */
      case BIFFx_CODE_MMS                        : {        } break;  // 0xC1   /* MMS - ADDMENU/DELMENU Record Group Count */
      case BIFFx_CODE_ADDMENU                    : {        } break;  // 0xC2   /* ADDMENU - Menu Addition */
      case BIFFx_CODE_DELMENU                    : {        } break;  // 0xC3  /* DELMENU - Menu Deletion */
      case BIFFx_CODE_SXDI                       : {        } break;  // 0xC5 /* SXDI - Data Item */
      case BIFFx_CODE_SXTBL                      : {        } break;  // 0xD0  /* SXTBL - Multiple Consolidation Source Info */
      case BIFFx_CODE_SXTBRGIITM                 : {        } break;  // 0xD1   /* SXTBRGIITM - Page Item Name Count */
      case BIFFx_CODE_SXTBPG                     : {        } break;  // 0xD2    /* SXTBPG - Page Item Indexes */
      case BIFFx_CODE_OBPROJ                     : {        } break;  // 0xD3    /* OBPROJ - Visual Basic Project */
      case BIFFx_CODE_SXIDSTM                    : {        } break;  // 0xD5   /* SXIDSTM - Stream ID */
      case BIFFx_CODE_SXEXT                      : {        } break;  // 0xDC /* PARAMQRY-SXEXT - Query Parameters-External Source Information (May have been in BIFF4)*/

      case BIFFx_CODE_OLESIZE                    : {        } break;  // 0xDE  /* OLESIZE - Size of OLE Object */
      case BIFFx_CODE_UDDESC                     : {        } break;  // 0xDF   /* UDDESC - Description String for Chart Autoformat */
      case BIFFx_CODE_INTERFACEHDR               : {        } break;  // 0xE1    /* @ INTERFACEHDR - Beginning of User Interface Records */
      case BIFFx_CODE_INTERFACEEND               : {        } break;  // 0xE2   /* INTERFACEEND - End of User Interface Records */
      case BIFFx_CODE_SXVS                       : {        } break;  // 0xE3     /* SXVS - View Source */
      case BIFFx_CODE_TABIDCONF                  : {        } break;  // 0xEA  /* TABIDCONF - Sheet Tab ID of Conflict History */
      case BIFFx_CODE_MSODRAWINGGROUP            : {        } break;  // 0xEB  /* MSODRAWINGGROUP - Microsoft Office Drawing Group (May have been in BIFF4)*/
      case BIFFx_CODE_MSODRAWING                 : {        } break;  // 0xEC  /* MSODRAWING - Microsoft Office Drawing (May have been in BIFF4)*/
      case BIFFx_CODE_MSODRAWINGSELECTION        : {        } break;  // 0xED  /* MSODRAWINGSELECTION - Microsoft Office Drawing Selection (May have been in BIFF4)*/
      case BIFFx_CODE_PROT4REV                  : {        } break;  // 0x1AF /* Shared Workbook Protection Flag */
   };
#endif
   return record;
};
//______________________________________________________________________________
BIFF_record *render_BIFF_record
(Record_code code
,nat32 record_length   // Unfortunately some BIFF records need to know the record body length inorder to determine the number of items in lists/arrays.
,BIFF_Format format
   // Even though we know the record code, the same record code
   // was used in the different BIFF versions and the format
   // of the record may be different
) 
{
   BIFF_record *rendered_record = 0;

   if (!rendered_record && ((format == BIFF2)                                                                                                    ))  rendered_record =render_BIFF2_record(code,record_length,format);
   if (!rendered_record && ((format == BIFF2) | (format == BIFF3)                                                                                ))  rendered_record =render_BIFF23_record(code,record_length,format);
   if (!rendered_record && ((format == BIFF2) | (format == BIFF3) | (format == BIFF4)                                                            ))  rendered_record =render_BIFF234_record(code,record_length,format);
// if (!rendered_record && ((format == BIFF2) | (format == BIFF3) | (format == BIFF4) | (format == BIFF5)                                        ))  rendered_record =render_BIFF2345_record(code,record_length,format);
// if (!rendered_record && ((format == BIFF2) | (format == BIFF3) | (format == BIFF4) | (format == BIFF5) | (format == BIFF7)                    ))  rendered_record =render_BIFF23457_record(code,record_length,format);
   if (!rendered_record && ((format == BIFF2)                                         | (format == BIFF5) | (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF2578_record(code,record_length,format);
   if (!rendered_record && ((format == BIFF2) | (format == BIFF3) | (format == BIFF4) | (format == BIFF5) | (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF234578_record(code,record_length,format);
   if (!rendered_record && (                    (format == BIFF3)                                                                                ))  rendered_record =render_BIFF3_record(code,record_length,format);
   if (!rendered_record && (                    (format == BIFF3) | (format == BIFF4)                                                            ))  rendered_record =render_BIFF34_record(code,record_length,format);
// if (!rendered_record && (                    (format == BIFF3) | (format == BIFF4) | (format == BIFF5)                                        ))  rendered_record =render_BIFF345_record(code,record_length,format);
   if (!rendered_record && (                    (format == BIFF3) | (format == BIFF4) | (format == BIFF5) | (format == BIFF7)                    ))  rendered_record =render_BIFF3457_record(code,record_length,format);
   if (!rendered_record && (                    (format == BIFF3) | (format == BIFF4) | (format == BIFF5) | (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF34578_record(code,record_length,format);
// if (!rendered_record && (                                        (format == BIFF4)                                                          ))    rendered_record =render_BIFF4_record(code,record_length,format);
// if (!rendered_record && (                                        (format == BIFF4) | (format == BIFF5)                                        ))  rendered_record =render_BIFF45_record(code,record_length,format);
   if (!rendered_record && (                                        (format == BIFF4) | (format == BIFF5) | (format == BIFF7)                    ))  rendered_record =render_BIFF457_record(code,record_length,format);
   if (!rendered_record && (                                        (format == BIFF4) | (format == BIFF5) | (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF4578_record(code,record_length,format);
// if (!rendered_record && (                                                            (format == BIFF5)                                        ))  rendered_record =render_BIFF5_record(code,record_length,format);
   if (!rendered_record && (                                                            (format == BIFF5) | (format == BIFF7)                    ))  rendered_record =render_BIFF57_record(code,record_length,format);
   if (!rendered_record && (                                                            (format == BIFF5) | (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF578_record(code,record_length,format);
// if (!rendered_record && (                                                                                (format == BIFF7)                    ))  rendered_record =render_BIFF7_record(code,record_length,format);
// if (!rendered_record && (                                                                                (format == BIFF7) | (format == BIFF8)))  rendered_record =render_BIFF78_record(code,record_length,format);
   if (!rendered_record && (                                                                                                    (format == BIFF8)))  rendered_record =render_BIFF8_record(code,record_length,format);
   if (!rendered_record)
      render_other_record(code,record_length,format);
   return rendered_record;
};
//______________________________________________________________________________
}//_namespace_CORN_____________________________________________________________/

