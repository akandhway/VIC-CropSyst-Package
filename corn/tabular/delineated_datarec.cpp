#include "corn/spreadsheet/cell.h"
#include "corn/tabular/delineated_datarec.h"
#include "corn/tabular/tabular.h"
#include "corn/chronometry/date_32.h"
namespace CORN
{
//______________________________________________________________________________
Delineated_data_record::Delineated_data_record
(Container &_tabular_data_columns,const char *_primary_section
, sint16 _year_offset_for_dates_with_only_two_digits /* = 0 */                   //120331
)
: CORN::Data_record             (_primary_section) // No sections in this format
, tabular_data_columns                                   (_tabular_data_columns)
, year_offset_for_dates_with_only_two_digits_Y
                                   (_year_offset_for_dates_with_only_two_digits) //120331
{}
//______________________________________________________________________________
Tabular_data_column *Delineated_data_record::find_tabular_data_column
(const std::string &current_sheet_name,nat16 cell_column_number)
{  nat16 col_number = 1;
   FIND_FOR_EACH_IN(found_col,tab_data_col,Tabular_data_column,tabular_data_columns,true,each_data_col)
   {  // We must search for column, we can not simply get the column by indexing because
      // the TDF format may omit columns from the layout description.
      // (I.e. because a spreadsheet column is completely blank)
      bool column_matched =
              (tab_data_col->TC_layout.sheet.column/*200302 number*/ == cell_column_number)
         ||  ((tab_data_col->TC_layout.sheet.column/*200302 number*/ == 0) && (col_number == cell_column_number));
         // <- In the case of spreadsheets, the columns listed are the columns   101130
      if (column_matched)
      {  bool sheet_name_matched =
               (tab_data_col->TC_layout.sheet.name.length()==0)                  //200302
            || (tab_data_col->TC_layout.sheet.name == current_sheet_name);       //200302
         if (sheet_name_matched)
            found_col = tab_data_col;
      }
      col_number++;
   } FOR_EACH_END(each_data_col)
   return found_col;
}
//_find_tabular_data_column____________________________________________________/
 bool Delineated_data_record::set_numeric
 (const std::string &current_sheet_name
 ,const CORN::Cell_numeric &cell,bool dates_based_1904)
 {  // This is used when data is coming from a spreadsheet (from cell receiver)
    nat32 cell_column_number = cell.get_col()+1;
    Tabular_data_column *data_col = Delineated_data_record
      ::find_tabular_data_column(current_sheet_name,cell_column_number);
    if (data_col)
    {
       switch (data_col->TC_layout.data_type_clad.get())
       {
          case DT_nat8    : data_col->value_bool    = cell.get_nat8();    break;
          case DT_nat16   : data_col->value_int16   = cell.get_nat16();   break;  // Warning, currently data_col only has integers
          case DT_nat32   : data_col->value_int32   = cell.get_nat32();   break;  // Warning, currently data_col only has integers
          case DT_int16   : data_col->value_int16   = cell.get_int16();   break;
          case DT_int32   : data_col->value_int32   = cell.get_int32();   break;
          case DT_float32 : data_col->value_float32 = cell.get_float32(); break;
          case DT_float64 : data_col->value_float64 = cell.get_float64(); break;
          case DT_date    :
          {
             if (data_col->TC_layout.date_time_format == "YYYYDDD")
             {
                CORN::date32 date_value = cell.get_int32();
                data_col->value_int32 = date_value;                              //170825_170525
                data_col->value_datetime.set_date32(date_value);                 //170825
             }
             else  // Assume this is an Excel date
             {  nat32 days_since_epoch_inclusive = cell.get_float64();
                nat32 days_since_epoch_exclusive
                   = days_since_epoch_inclusive -
                      (dates_based_1904 ? 1 : 2);   // minus 2 is fixup for the Lotus and Excel thinking 1900 is a leap year!
                CORN::Date_clad_32  date(dates_based_1904 ? 1904001 : 1900001);  //170827
                date.inc_days(days_since_epoch_exclusive);
                data_col->value_int32 = date.get_date32();                       //170525
                data_col->value_datetime.set_date(date);
             }
          } break;   // CORN date and time types
#ifdef NYI
I don't know how Excel stores time in cell (I guess seconds or milliseconds since midnight).
date with time (if it stores date and time is a cell)
case DT_time      : tab_data_col->number.value.as_xxx = cell.get_xxxx(); break;
case DT_datetime  : tab_data_col->number.value.as_xxx = cell.get_xxxx(); break;
#endif
      } // switch
   }
   return data_col != 0;
}
//_set_numeric_________________________________________________________________/
bool Delineated_data_record::set_textual
(const std::string &current_sheet_name
,const Cell_textual& cell)
{  nat32 cell_column_number = cell.get_col()+1;
   Tabular_data_column *data_col = Delineated_data_record::
      find_tabular_data_column(current_sheet_name,cell_column_number);
   if (data_col)
      cell.get_string(data_col->value_string);
   return true;
}
//_set_textual_________________________________________________________________/
bool Delineated_data_record::expect_structure(bool for_write)
{  bool expected = CORN::Data_record::expect_structure(for_write);               //161023
   FOR_EACH_IN(tab_data_col,Tabular_data_column,tabular_data_columns,each_data_col)
   {  // depending on the data type expect to value_*
      VV_base_entry *entry = 0;                                                  //050516
      std::string TC_layout_key(tab_data_col->TC_layout.get_key());              //180820
      switch (tab_data_col->TC_layout.data_type_clad.get())
      {  case CORN::DT_bool         : entry = expect_bool        (TC_layout_key.c_str(),tab_data_col->value_bool);                      break;
         case CORN::DT_string       : entry = expect_string      (TC_layout_key.c_str(),tab_data_col->value_string,tab_data_col->TC_layout.get_field_width()/*width*/); break;
         case CORN::DT_int16        : entry = expect_int16       (TC_layout_key.c_str(),tab_data_col->value_int16);                     break;
         case CORN::DT_int32        : entry = expect_int32       (TC_layout_key.c_str(),tab_data_col->value_int32);                     break;

     // Currently I don't have buffers for nat16 and nat32 so simply using int16 and int32
         case CORN::DT_nat16        : entry = expect_int16       (TC_layout_key.c_str(),tab_data_col->value_int16);                     break;
         case CORN::DT_nat32        : entry = expect_int32       (TC_layout_key.c_str(),tab_data_col->value_int32);                     break;

         case CORN::DT_float32      : entry = expect_float32     (TC_layout_key.c_str(),tab_data_col->value_float32);                   break;
         case CORN::DT_float64      : entry = expect_float64     (TC_layout_key.c_str(),tab_data_col->value_float64);                   break;
         case CORN::DT_date         :
            entry = expect_datetime64_formatted                                  //190514
               (TC_layout_key.c_str(),tab_data_col->value_datetime
               ,tab_data_col->TC_layout.date_time_format.c_str());
               // should be format/pattern
            //190514entry = expect_datetime64 (TC_layout_key.c_str(),tab_data_col->value_datetime,true);        //120331
         break;
//         case CORN::DT_time         : entry = expect_int32  (TC_layout_key,tab_data_col->value_int32);                  break; //170525
            //170525 Warning need to decide if time is 32bit integer
         case CORN::DT_time         : entry = expect_datetime64  (TC_layout_key.c_str(),tab_data_col->value_datetime,true);                  break;
         case CORN::DT_datetime     : entry = expect_datetime64  (TC_layout_key.c_str(),tab_data_col->value_datetime,true);                  break; //120331
         default              :                      entry = expect_string      (TC_layout_key.c_str(),tab_data_col->value_string,tab_data_col->TC_layout.get_field_width()/*width*/); break;
      }
      if (entry)                                                                 //050516
      {  entry->bind_to_validation_status(&(tab_data_col->validation_status));
         entry->set_tabular_column(&(tab_data_col->TC_layout));                  //070524
      }
   } FOR_EACH_END(each_data_col)
   structure_defined = true;                                                     //120314
   return expected;                                                              //161023
}
//_expect_structure____________________________________________________________/
}//_namespace_CORN_____________________________________________________________/



