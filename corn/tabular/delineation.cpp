#include "corn/tabular/delineation.h"
#include "corn/math/compare.hpp"

#if ((CS_VERSION>=1) && (CS_VERSION<=5))
#include "corn/data_source/datarec.h"
#include "CS_suite/observation/CS_optation.h"

#endif
#include <iostream>
#include "UED/library/varcodes.h"

namespace CORN {
//______________________________________________________________________________
const char *tabulation_label_table[] =
{"intrinsic"
,"fixed"
,"delimit"     // was renamed from character
,"character"   // obsolete rename this to delimit
,0 };
//______________________________________________________________________________
const char *relative_to_label_table[] =
{  "file"
,  "data"
,  "page"
,  "block"
,0};
//______________________________________________________________________________
#define LABEL_start_row        "start_row"
bool Delineation::is_delimiter(char potential_delimiter)            affirmation_
{  return separation.delimiter.find(potential_delimiter) != std::string::npos;   //200220
}
//_is_delimiter________________________________________________________________/
bool Delineation::has_delimiter()                                   affirmation_
{  bool has = !separation.delimiter.empty();                                     //200220
   return has;
}
//_has_delimiter____________________________________________________2019-01-07_/
const char *Delineation::append_delimiter(std::string &return_buffer)      const
{
   // When reading files, multiple potential delimiters could be optionally specified as delimiters
   // When writing files, only a single delimiter is available.

   return_buffer += separation.delimiter[0];                                     //200220
   return return_buffer.c_str();
}
//_append_delimiter_________________________2020-02-20__2019-01-06__2005-08-30_/
bool Delineation::default_R_text()                                 modification_
{  tabulation_clad.set(delimit_TABULATION);
   separation.continuous= false;
   separation.delimiter = "\t";
   separation.quotation = ""; //RLN warning, need to verify
   separation.decimal = '.';
   // . for US  , for other countries
   return true;
}
//_default_R_text_______________________________________2002-02-20__2014-10-08_/
bool Delineation::copy_from(const Delineation &from) modification_
{  tabulation_clad.set(from.tabulation_clad.get());
   separation.continuous= from.separation.continuous;
   separation.delimiter = from.separation.delimiter;
   separation.quotation = from.separation.quotation;
   separation.decimal   = from.separation.decimal;
   //extent.notes         = from.extent.notes;
   // NYI should eventually copy all the extent, at this time I only needed the start row
   extent.begin_offset  = from.extent.begin_offset;
   return true;
}
//_copy_from____________________________________________2002-02-20__2014-10-08_/
Delineation_tabular::Delineation_tabular()
: pagination                                                            ("none")
, lineation                                                           ("native")
, definer                                                                   ("") //990307
, inception                                                                   () //990307
, notes                                                                     ("")
, year_offset_for_dates_with_only_two_digits(0)                                //120331
, document                                                                    ()
, header                                                                      ()
, footer                                                                      ()
, detail                                                                      ()
{}
//_Delineation_tabular:constructor__________________________________2020-03-06_/
bool Delineation_tabular::default_R_text()                        modification_
{  // Sets up format and delimit for R import/export text files.
   document.extent.begin_offset = 0;                                             //200220
   header.extent.begin_offset = 0;                                               //200220
   detail.extent.begin_offset = 1;                                               //200220
   // no footer
   lineation = "native";                                                         //200220
   pagination = "none";                                                          //200220
   header.paginate = false;                                                      //200220
   document.missing           = "";
   bool col_delineation_set =
       header.default_R_text()
    && detail.default_R_text();
   return col_delineation_set;
}
//_default_R_text___________________________________________________2014-10-08_/
const std::string &compose_formatted_string_from_cstr
(std::string &formatted_string
,const char *c_str
,std::string quotation                                                           //200220
,nat16 field_width)   // will be 0 if not applicable                             //190106
{  int quote_count = 0;
   std::string quote_character(quotation);                                       //200220
      // using string because a quote may in the future be multiple characters (I.e. HTML)
   if (quotation.length() == 2) // special case for alternating indicator
#ifdef NYI
      {
         In this case scan the c_str, If it has a double quote in it then  append "'",
         if it has a single quote, then append
         quote_count = 2;
      }
#endif
   formatted_string.append(quote_character);
   // if right justified or centered, first append padding
   formatted_string.append(c_str);
   if (field_width)
   {  size_t formatted_string_length = formatted_string.length();             //130821
      int16 padding = field_width-formatted_string_length-quote_count;        //130821
      padding = CORN::must_be_0_or_greater<int16>(padding);                   //190106
      formatted_string.append(std::string(padding,' '));                      //190106
   }
   formatted_string.append(quote_character);
   return formatted_string;
}
//_compose_formatted_string_from_cstr___________________2019-01-06__2014-09-22_/
Table_definition::Column &Table_definition::provide_column
(nat16 column_index)                                                  provision_
{  Table_definition::Column *col = 0;
   while ((col = dynamic_cast<Table_definition::Column *>
          (columns.get_at(column_index))) == 0)
   {  col = new Table_definition::Column;
      columns.take(col);
   }
   return *col;
}
//_provide_column___________________________________________________2020-03-02_/
nat16 Table_definition::get_width_column(nat16 column_index)               const
{  Column *col = dynamic_cast<Column *>
      (columns.get_at(column_index));
   return col ? col->delineation.width : 0;
}
//_get_width_column_________________________________________________2017-03-27_/
nat16 Table_definition::get_column_number(nat16 column_index)              const
{Column *col = dynamic_cast<Column *>(columns.get_at(column_index));

   assert(false);
   // In need to check if column_index is truely index or number
   // if it is index then it probably needs to be get_at(column_index +1)

   return col ? col->sheet.column : 0;
}
//_get_start_column_________________________________________________2017-03-27_/
nat16 Table_definition::get_position(nat16 column_index)                   const
{
   Column *col = dynamic_cast<Column *>(columns.get_at(column_index));
   return col ? col->delineation.position : 0;
}
//_get_start_column_________________________________________________2017-03-27_/
std::string *Table_definition::render_blank
(nat16 column_index, nat32 row_index)                                 rendition_
{
   nat32 row_number = row_index + 1;
   std::string *blank = 0;
   if (row_number < header.extent.begin_offset+1)
   {  // we are in headers region
      return 0;  // NYI WARNING
      // should lookup if the position falls in metadata
   }
   else // we are in data region
   {
     Column *col = dynamic_cast<Column *>(columns.get_at(column_index));
     if (col)
     {
      nat16 col_width = col->get_field_width();
      if (row_number < detail.extent.begin_offset+1)
      {  // We haven't reached detail lines yet, so we must be in column headers
         if (col_width)
         {
            blank = new std::string;
            compose_formatted_string_from_cstr
               ((*blank),"",header.separation.quotation,col_width);              //200220
         }
      } else // we are in data / detail lines
      {  // assume the column is data
         // moved Column *col = dynamic_cast<Column *>(columns.get_at(column_index));
         if (col_width)
         {
            const char *preferred_value = "";
            if (col->static_text.length())
               preferred_value = col->static_text.c_str();
            if (detail.missing.length())                                         //200302
               preferred_value = detail.missing.c_str();                         //200302
            blank = new std::string;
            compose_formatted_string_from_cstr
               ((*blank)
               ,preferred_value
               ,detail.separation.quotation                                      //200220
               ,col_width);
         }
      }
     }
     else
     {   std::clog << "failed: column(index) = " << col << std::endl;
     }
   }
   return blank;
}
//_render_blank_____________________________________________________2019-01-08_/
nat32 Table_definition::position_columns()
{
   // This identifies column character start positions based on field widths.
   // The tabular description does not need to specify
   // column numbering or positioning the column starts in the file.
   // If omitted it is assumed these will be relative
   // But text file writers and worksheet output will
   // need these values, so we
   // setup columns number and position based on relative occurance
   // in the TDF file and/or available information

   nat32 col_position = 1;
   nat32 col_number   = 1;
   FOR_EACH_IN(column,Column,columns,col_iter)
   {
      if (column->delineation.position)                                          //200302
         // if the format file did specify a position for a column
         // the subsequent columns that don't have specific column numbers
         // will be assumed to be relative to this specified column number.
         col_position = column->delineation.position;                            //200302
      else (column->delineation.position = col_position);                        //200302

      nat16 field_width = column->delineation.width;                             //200224
      if (field_width == 0)
      {  field_width = 8;  //assuming numeric value default width 8
      }
      col_position += field_width;

      if (column->sheet.column == 0)
         column->sheet.column = col_number;
      col_number += (column->spread > 1)
         ? column->spread : 1;
   } FOR_EACH_END(col_iter)
   return columns.count();
}
//_position_column__________________________________________________2020-03-06_/
/*200808 abandoned
#if ((CS_VERSION>=1) && (CS_VERSION<=5))
bool Table_definition::Column::setup_structure
(CORN::Data_record &data_rec,bool for_write)             modification_
{  bool satup = Tabular_column::setup_structure(data_rec,for_write);
   satup &= data_rec.expect_nat32("variable_code",UED_variable,16);
   return satup;
}
//_Table_definition::Column::setup_structure________________________2020-08-08_/
#endif
*/
#if ((CS_VERSION >= 1) && (CS_VERSION <= 5))
void Table_definition::list_required_variables()                           const
{  //for each column append the variable to required_variables
   FOR_EACH_IN(column,Column,columns,each_column)
   {
      UED::Variable_code_cowl variable_code_cowl(column->UED_variable);
      UED::Variable_code var_code = (variable_code_cowl.is_layered())
      ? variable_code_cowl.get_without_layer()
      : variable_code_cowl.get();
      CS::optation_global.desire(var_code,UT_day);
   } FOR_EACH_END(each_column)
}
//_list_required_variables_______________________________2020-08-13_2004-10-21_/
#endif
}//_namespace_CORN_____________________________________________________________/

