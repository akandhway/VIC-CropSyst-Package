#include "corn/container/bilist.h"
#include "UED/library/varcodes.h"
#include "CS_suite/observation/CS_optation.h"
#include "UED/convert/ED_tabular_format.hX"
using namespace CORN;
//______________________________________________________________________________
#define LABEL_format                "format"
//200220 obsolete #define LABEL_header_terminator     "header_terminator"
#define LABEL_column_headers        "column_headers"
#define LABEL_rows                  "rows"
#define LABEL_units_rows            "units_rows"
#define LABEL_units_start_row       "units_start_row"
#define LABEL_detail_lines          "detail_lines"
#define LABEL_metadata_field        "metadata_field"
#define LABEL_column                "column"
//______________________________________________________________________________
const char *field_data_label_table[] =
{  "static_text"
,  "UED_general_comment"
,  "UED_database_description"
,  "UED_generating_application"
,  "UED_location"
,  "UED_scalar_data"
,  "UED_timestamp"
,  "parent_path"                                                                 //181030
,0};
//_field_data_label_table______________________________________________________/
const char *generating_application_vars_label_table[]=
{"version"
,"description"
,"build_date"
,0};
//_generating_application_vars_label_table_____________________________________/
const char *UED_scalar_record_code_label_table[]=
{
"value_year_var_units"
"value_year_var"
"value_year_units"
"value_year"
"value_time_var_units"
"value_time_var"
"value_time_units"
"value_time"
"value_date_var_units"
"value_date_var"
"value_date_units"
"value_date"
"value_date_time_var_units"
"value_date_time_var"
"value_date_time_units"
"value_date_time"
,0};
//_UED_scalar_record_code_label_table__________________________________________/
const char *location_var_label_table[]=
{"station_ID"
,"ISO 6709:2008"  //(either Annex H or D)
,"latitude_decimal_degrees"
,"latitude_deg_min_sec"
,"longitude_decimal_degrees"
,"longitude_deg_min_sec"
,"elevation_meters"
,"elevation_feet"
,"country_name"
,"state_name"
,"county_name"
,"description"
,"station_name"
,"comment"                                                                       //100107
,"station_number"
, 0};
//_location_var_label_table____________________________________________________/
#define     LABEL_data "data"
/////////////////////////////////////////////////////////////////////////////
//______________________________________________________________________________
// This constructor is used when creating a new format using an editor
/*200225
ED_tabular_file_format::ED_tabular_file_format
(
Format_file_type format_file_type_
)
:Tabular_format                                                               () //040925
,records_per_UED_record                                                    (366) //040925
,metadata_fields_numbered                                                     ()
,date_column_number                                                          (0) //050407
,ID_column_number                                                            (0) //050407
,name_column_number                                                          (0) //050407
,extension_year_digits                                                       (0) //150405
,year_filename_suffix_obsolete                                           (false) //150405_050408
{}
//______________________________________________________________________________
*/
// This constructor is used when loading a format
ED_tabular_file_format::ED_tabular_file_format
(const std::string   &description_
/*200225
,Format_file_type     format_file_type_
*/
)
:Tabular_format/*200220 Common_tabular_file__format*/                         () //040925
,records_per_UED_record                                                    (366) //040925
,metadata_fields_numbered                                                     ()
,date_column_number                                                          (0) //050407
,ID_column_number                                                            (0) //050407
,name_column_number                                                          (0) //050407
,extension_year_digits                                                       (0) //150405
,year_filename_suffix_obsolete                                           (false) //050408
{}
//______________________________________________________________________________
// This constructor is used by UED_export file
ED_tabular_file_format::ED_tabular_file_format()
:Tabular_format/*200220 Common_tabular_file__format*/()                          //040925
,records_per_UED_record                                                    (366) //040925
,metadata_fields_numbered                                                     ()
,date_column_number                                                          (0) //050407
,ID_column_number                                                            (0) //050407
,name_column_number                                                          (0) //050407
,extension_year_digits                                                       (0) //150405
,year_filename_suffix_obsolete                                           (false) //050408
{}
//______________________________________________________________________________
#define LABEL_format_file_type "format_file_type"
#define LABEL_big_endian       "big_endian"
#define LABEL_line_feed        "line_feed"
#define LABEL_FORTRAN_carriage_control "FORTRAN_carriage_control"
//200306obs #define LABEL_defined_by       "defined_by"
//200306obs #define LABEL_defined_date     "defined_date"
#define LABEL_notes            "notes"

// These are 1 based indexes  (because column references (in identification need to be 1 based)
DECLARE_CONTAINER_ENUMERATED_SECTION(ED_tabular_file_format::Metadata_field,Metadata_fields_section_VV,1);
DECLARE_CONTAINER_ENUMERATED_SECTION(ED_tabular_file_format::Column,ED_Column_section_VV,1);
//______________________________________________________________________________
bool ED_tabular_file_format::expect_structure(bool for_write )
{  bool expected = Tabular_format/*200220 Common_tabular_file__format*/::expect_structure(for_write);      //161025
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_format);
   EXPECT_ENUMERATED_SECTION
      (LABEL_metadata_field,Metadata_fields_section_VV,metadata_fields_numbered);//040925
   set_current_section("identification");
      expect_int16("date_column",date_column_number);
      expect_nat8 ("extension_year_digits",extension_year_digits,10);            //150405
      expect_bool("year_filename_suffix",year_filename_suffix_obsolete);         //150405_050408
      expect_int16("ID_column",ID_column_number);
      expect_int16("name_column",name_column_number);
   set_current_section(LABEL_format);
      expect_string("filename_extension",filename_extension,255);                //080228
   EXPECT_ENUMERATED_SECTION(LABEL_column,ED_Column_section_VV,columns);
   structure_defined = true;                                                     //120314
   return expected;                                                              //161025
}
//_expect_structure____________________________________________________________/
bool ED_tabular_file_format::set_start()
{  // we need to tell the metadata_fields what format
   // so we dont write unecessary fields for the format
   /*200225 I think now obsolete
   FOR_EACH_IN(metadata_field,Metadata_field,metadata_fields_numbered,each_metadata_field)
      metadata_field->set_format_file_type(format_file_type_clad.get());
   FOR_EACH_END(each_metadata_field)
   */
   return Tabular_format/*200220 Common_tabular_file__format*/::set_start();                               //161023_051028
}
//_set_start________________________________________________________2005-04-05_/
bool ED_tabular_file_format::Metadata_field::setup_structure
(CORN::Data_record &data_rec,bool for_write)                       modification_
{  bool satup =Parameter_properties_string::setup_structure(data_rec,for_write)  //170808_100124
            && Parameter_number_properties::setup_structure(data_rec,for_write); //100124
   data_rec.expect_string("description",description,255);   // Description of the header //040909
   data_rec.expect_enum ("relative_to",relative_to_clad);                        //070925
   data_rec.expect_bool ("every_page",every_page);                               //070925
   data_rec.expect_nat16("row",row_num,10);
   data_rec.expect_nat16("col",col_num_not_pos,10);
   data_rec.expect_nat16("pos",pos_num,10);                                      //181030_171129
   // These "caption"  variable/value used to be  "label"
   data_rec.expect_nat16("caption_row",caption_row,10);                          //060324
   data_rec.expect_nat16("caption_col",caption_col,10);                          //060324
   //100124 label has been replaced by caption from parameter_properties
   // These are provides for backward compatibility
   if (!for_write) data_rec.expect_string("label"     ,caption    ,255);         //060324
   if (!for_write) data_rec.expect_nat16("label_row" ,caption_row,10);          //060324
   if (!for_write) data_rec.expect_nat16("label_col" ,caption_col,10);          //060324
   data_rec.expect_enum  ("field"               ,field_data_clad);
   data_rec.expect_enum  ("location_variable"   ,location_var_clad);             //050405
   data_rec.expect_string("static_text"         ,static_text,255);
   data_rec.expect_nat8  ("parent_path"         ,parent_path,10);                //181030
   data_rec.expect_nat16 ("database_description",database_description_index,10); //040927
   data_rec.expect_nat16 ("database_comment"    ,database_description_index,10); //040927
#ifdef NYI
      // The following are for UED scalar records
      Record_code       UED_scalar_record_code;
      UED_variable_code UED_scalar_variable_code;
      UED_units_code    UED_scalar_units_code;
      UED_units_code    UED_scalar_time_stamp;
#endif
   data_rec.expect_enum("generating_application_variable",generating_application_var_clad);
   return satup;                                                                 //170808
}
//_Metadata_field::setup_structure_____________________________________________/
bool ED_tabular_file_format::Column::setup_structure(CORN::Data_record &data_rec,bool for_write) modification_
{  bool satup = Tabular_format::Column::setup_structure(data_rec,for_write);
   data_rec.expect_nat32("variable_code",variable_code_clad.code,16);            //040925
   data_rec.expect_enum ("UED_units_code",UED_import_to_smart_units_code_enum);  //050518
   data_rec.expect_nat16(LABEL_UED_record_code,record_type_clad.code ,16);       //040925
   data_rec.expect_enum ("statistic", statistic_clad);
   if (!for_write || (for_write && period >= 0))                                 //041004
   data_rec.expect_int16("period",period);  // must be signed                    //041004
   if (!for_write || (for_write && record_date))                                 //041004
      data_rec.expect_bool("record_date",record_date);                           //041004
   data_rec.expect_bool("missing_is_0",missing_is_0);                            //140608
   return satup;
}
//_Column::setup_structure__________________________________________2004-09-26_/
void ED_tabular_file_format::list_required_variables()                     const //170225
{  //for each column append the variable to required_variables
   FOR_EACH_IN(column,Column,columns,each_column)
   {  UED::Variable_code var_code = (column->variable_code_clad.is_layered())
      ? column->variable_code_clad.get_without_layer()                           //041107
      : column->variable_code_clad.get();                                        //041107
      CS::optation_global.desire(var_code,UT_day);                               //190121
   } FOR_EACH_END(each_column)
}
//_list_required_variables__________________________________________2004-10-21_/
const ED_tabular_file_format::Metadata_field *ED_tabular_file_format
::find_metadata_field(const std::string &sheet_name,nat16 row, nat16 col)  const
{  FIND_FOR_EACH_IN(found_field,field,Metadata_field,metadata_fields_numbered,true,each_field)
   {  bool cell_matches =  (field->row_num == row) && (field->col_num_not_pos == col);
      if (cell_matches)
      {  bool sheet_matches =
              (field->worksheet.length() == 0)
            ||(field->worksheet == sheet_name);
         if (sheet_matches)
            found_field = field;
      }
   } FOR_EACH_END(each_field)
   return found_field;
}
//_find_metadata_field___________________________________2010-01-24_2010-01-15_/
bool ED_tabular_file_format::get_end()
{
   // CropSyst version 3 separated years in to files with 4 digit extension for year.
   if (year_filename_suffix_obsolete)
      extension_year_digits = 4;
   return CORN::Tabular_format::get_end();                           //161023
}
//_get_end__________________________________________________________2015-04-05_/
/*abandoned
bool ED_tabular_file_format::is_column_record_date(const Column &column)   const
{  return column.record_date;
}
//_is_column_record_date____________________________________________2020-06-18_/
*/
