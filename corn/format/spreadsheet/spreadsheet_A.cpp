#include <stdio.h>
#include "corn/math/compare.h"
#include "corn/format/spreadsheet/spreadsheet_A.h"
using namespace CORN;
//______________________________________________________________________________
Spreadsheet_abstract::Spreadsheet_abstract
(const char *filename_
,std::ios_base::openmode openmode_)                                              //111108
: Old_binary_record_file                       (filename_,false,false,openmode_) //990412
, col                                                                        (0)
, row                                                                        (0)
, max_col                                                                    (0)
, max_row                                                                    (0)
, int_value                                                                  (0)
, data_type                                                           (not_data)
, float_value                                                              (0.0)
, precision                                                                  (2)
{}
//_constructor:reading___________________________________________________1998?_/
Spreadsheet_abstract::Spreadsheet_abstract
(const char *filename_                                                           //000211
,sint32 first_row  ,sint32 rows                                                  //090618
,int16 first_col   ,int16 cols  )
: Old_binary_record_file              (filename_,false,false,std::ios_base::out) //111108
, col                                                                (first_col) //090618
, row                                                                (first_row) //090618
, max_col                                                                    (0)
, max_row                                                                    (0)
, int_value                                                                  (0)
, data_type                                                           (not_data)
, float_value                                                              (0.0)
, precision                                                                  (2)
{  initialize();                                                                 //990331
}
//_constructor:writing___________________________________________________1998?_/
void Spreadsheet_abstract::initialize()
{  for (int col_ = 0; col_ < MAX_COLUMNS; col_++)                                //021216
      column_widths[col_] = 8;
}
//_initialize_______________________________________________________1999-03-21_/
void Spreadsheet_abstract::update_extent(int16 i_column,int16 i_row)
{  // record the maximum extent of the sheet
   max_col = CORN_max(max_col,i_column);
   max_row = CORN_max(max_row,i_row);
}
//_update_extent_________________________________________________________1998?_/
void Spreadsheet_abstract::write_integer_cell
(int16 _column
,int16 _row
,int16 _value)
{  update_extent(_column,_row); }
//_write_integer_cell____________________________________________________1998?_/
void Spreadsheet_abstract::write_number_cell
(int16 _column
,int16 _row
,float64 _value
,int8  _precision)
{  update_extent(_column,_row); }
//_write_number_cell_____________________________________________________1998?_/
void Spreadsheet_abstract::write_label_cell
(int16 _column
,int16 _row
,CSS_justifications _alignment
,const char *_value)                                                             //030120
{  update_extent(_column,_row); }
//_write_label_cell______________________________________________________1998?_/
void Spreadsheet_abstract::read_number_cell(bool expect_header)
{  update_extent(col,row); }
//_read_number_cell______________________________________________________1998?_/
void Spreadsheet_abstract::read_integer_cell(bool expect_header)
{  update_extent(col,row); }
//_read_integer_cell_____________________________________________________1998?_/
void Spreadsheet_abstract::read_label_cell(bool expect_header)
{  update_extent(col,row); }
//_read_label_cell__________________________________________________1998-10-18_/
// The following function output a spreadsheet cell of the respective data type.}
void Spreadsheet_abstract::write_integer(int16 value , int16 width)
{  write_integer_cell(col,row,value);
   col++;
}
//_write_integer____________________________________________________1998-09-15_/
void Spreadsheet_abstract::write_real(float64 value, int8 width, int8 precision_)
{  write_number_cell(col,row,value,precision_);
   col++;
}
//_write_real_______________________________________________________1998-09-15_/
void Spreadsheet_abstract::write_string_aligned
(CSS_justifications align ,const char *value)                                    //030120
{  write_label_cell(col,row,align,value);
   col++;
}
//_write_string_aligned_____________________________________________2002-12-16_/
void Spreadsheet_abstract::endl()
{  col = 0; row++; }
//_endl__________________________________________________________________1998?_/
char *Spreadsheet_abstract::cell_text_string(char *buffer)
{  switch (data_type)
   { // this should be expanded for the various cell formats
      case   type_integer : sprintf(buffer,"%*d",column_widths[col],int_value); break;
      case   type_float   : sprintf(buffer,"%*.*f",column_widths[col],precision,(float)float_value); break;
      case   type_str     : sprintf(buffer,"%*s",column_widths[col],cstr_value); break;
      case   type_date    : // not currently handling dates (don't know date format);
      break;
      default : sprintf(buffer,"%*s",column_widths[col]," "); break;
   };
   return buffer;
}
//_cell_text_string______________________________________________________1998?_/
bool Spreadsheet_abstract::reset()                                    stream_IO_
{  is_at_eof = 0;
   //clear(); /*Clear rdstate flags */                                           //111111
   seek(0);
   return true;                                                                  //120805
}
//_reset_______________________________________________________________________/
