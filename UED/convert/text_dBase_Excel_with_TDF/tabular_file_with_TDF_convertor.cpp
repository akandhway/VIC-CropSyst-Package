
#include "UED/convert/text_dBase_Excel_with_TDF/tabular_file_with_TDF_convertor.h"
#include "import_column.h"
#include "UED/library/UED_codes.h"
#include "corn/data_source/vv_file.h"
#include "corn/data_type_enum.h"
#include "UED/library/datasetrec_creation.h"
#include "UED/library/timeqry.h"
#include "UED/library/controls.h"
#include "UED/library/UED.h"
#include "UED/library/locrec.h"
#include "corn/container/SDF_list.h"
#include "export_column.h"
#include "corn/measure/measures.h"
#include "corn/math/compare.hpp"
#include "corn/OS/file_system_engine.h"
#include "corn/container/SDF_list.h"

//CSVC only included for debugging
#include "CropSyst/source/csvc.h"

using namespace std;
using namespace UED;
using namespace CORN;
using namespace CORN::OS;
using namespace CORN::OS::FS;
//______________________________________________________________________________
Tabular_file_with_TDF_convertor::Tabular_file_with_TDF_convertor
(Convertor_arguments &arguments_)
: UED::Convertor                                                    (arguments_)
, time_step_years                                                            (0)
   // Only one timestep increment will be used
, time_step_days                                                             (0)
   // all others will be 0
, time_step_minutes                                                          (0)
, time_step_seconds                                                          (0)
   // Setup in initialize
, headers_written                                                        (false) //141019
, detail_line_data_record                                                    (0)
, header_identified_geolocation                                              (0)
, importing_metadata                                                     (false)
, current_sheet_year                                                         (0) //011013
   // Somehow I will need to compose
, current_sheet_month                                                        (0) //011013
   // detail line dates
, current_sheet_day_of_month                                                 (0) //011013
, current_sheet_day_of_year                                                  (0) //011013
, current_sheet_row                                                          (0)
, column_widths                                                        (10,10,0) //100322
, column_start                                                         (10,10,0) //100322
, tabular_file                                                               (0) //101127
{  column_widths.clear();
   column_start.clear();
}
//______________________________________________________________________________
Tabular_file_with_TDF_convertor::~Tabular_file_with_TDF_convertor()
{
   delete tabular_file; tabular_file = 0;                                        //101127
}
//_2007-03-06___________________________________________________________________
Record_code Tabular_file_with_TDF_convertor::initialize_common()
{
   // At this point we only have the defined format
   // we need to specify how to associate the format information with UED data record creation.
   // This is done once to save a little processing time.
   File_name *format_filename = parameters.format_filename_discovery.provide_primary();     //200806
   //200806 if (CORN::OS::file_system_engine.exists(parameters.format_filename)) //160306
   if (format_filename) //200806 CORN::OS::file_system_engine.exists(parameters.format_filename)) //160306
   {  VV_File format_file(format_filename->c_str(),true);                         //200806
      //200806 VV_File format_file(parameters.format_filename.c_str(),true); // We need to remember the structure because it is used to identify the column format

      // We need to remember the structure because it is used to identify the column format
      format_file.get(format);
   } else
   {  // In the case of UED collator, the default format will have been created
      // in memory.
      if (!format.columns.count())                                               //200429_160306
      {
         std::cerr << "warning: Unable to establish columns."
         //200806 << parameters.format_filename.c_str()
         << std::endl;
         press_return_before_exit = true;
         return 0;                                                               //070307
      }
   }
   // Compose the record type code expected for all records
   // This may be overrided by column information for specific columns (I.e. spread vectors)
   Record_code  record_type_code = standard_subcode; // this will be compose from subcodes
   // Generally, tabular data format represent a data series (data set)
   record_type_code |= data_set_float32_subcode;                                 //171110
   time_step_stamp_units_code = UT_year;                                         //050413
   if ((format.records_per_UED_record == 365) ||
       (format.records_per_UED_record == 366))
       record_type_code |= timestamp_year_subcode; // Special for CropSyst (only I think)
   else
      //200429 switch (format.detail_lines_delineation.time_step_units_enum.get())
      switch (format.detail.timestep_units_enum.get())                           //200429
      {
         // For yearly data we have one record for all years the specified year is the first in the series
         // (may need to be vector? or separate scalars)
         case UT_year      : record_type_code |= timestamp_year_subcode;   time_step_years = 1;    break;
         case UT_decade    : record_type_code |= timestamp_year_subcode;   time_step_years = 10;   break;
         case UT_century   : record_type_code |= timestamp_year_subcode;   time_step_years = 100;  break;
         case UT_millenium : record_type_code |= timestamp_year_subcode;   time_step_years = 1000; break;

         case UT_1_minute     : record_type_code |= timestamp_date_subcode; time_step_minutes = 1; time_step_stamp_units_code = UT_day; break;// 1440 values
         case UT_5_minutes    : record_type_code |= timestamp_date_subcode; time_step_minutes = 5; time_step_stamp_units_code = UT_day; break;// 288 values
         case UT_10_minutes   : record_type_code |= timestamp_date_subcode; time_step_minutes = 10; time_step_stamp_units_code = UT_day; break;// 144 values
         case UT_15_minutes   : record_type_code |= timestamp_date_subcode; time_step_minutes = 15; time_step_stamp_units_code = UT_day; break;// 96 values
         case UT_30_minutes   : record_type_code |= timestamp_date_subcode; time_step_minutes = 30; time_step_stamp_units_code = UT_day; break;// 48 values
         case UT_hour         : record_type_code |= timestamp_date_subcode; time_step_minutes = 60; time_step_stamp_units_code = UT_day; break;// one record for each day with 24 values
         // Other minutes are possible, but these factors of 60 are the most common

         case UT_minute       : record_type_code |= timestamp_date_time_subcode; time_step_seconds = 60; time_step_stamp_units_code = UT_hour; break; // One record for each hour with 60 values
         case UT_second       : record_type_code |= timestamp_date_time_subcode; time_step_seconds = 1;  time_step_stamp_units_code = UT_hour; break; // one record for each minute with 60 values
//NYI    case UT_millisecond  : record_type_code |= timestamp_date_time_subcode; time_step_milliseconds = 1; break; // one record for each second with 1000 values;
         // NYI  can add here UT_xx seconds (use an if statement)
   #define TIMESTEP_BY_MONTH  0
         case UT_day       : record_type_code |= timestamp_year_subcode; time_step_days = 1; break; // here we have one record for each year with 365/366 daily value
         case UT_week      : record_type_code |= timestamp_year_subcode; time_step_days = 7; break; // here we have one record for each year with 52 value
         case UT_fortnight : record_type_code |= timestamp_year_subcode; time_step_days = 14; break;  // here we have one record for each year with 26 values
         case UT_month     : record_type_code |= timestamp_year_subcode; time_step_days = TIMESTEP_BY_MONTH; break;  // here we have one record for each year with 12 values
         case UT_quarter   : record_type_code |= timestamp_year_subcode; time_step_days = TIMESTEP_BY_MONTH; break;  // here we have one record for each year with 4 values
         case UT_trimester : record_type_code |= timestamp_year_subcode; time_step_days = TIMESTEP_BY_MONTH; break;  // here we have one record for each year with 3 values
         case UT_biannum   : record_type_code |= timestamp_year_subcode; time_step_days = TIMESTEP_BY_MONTH; break;  // here we have one record for each year with 2 values
         default           : record_type_code |= timestamp_year_subcode; time_step_days = 1; break; // here we have one record for each year with 365/366 daily value
      }
   record_type_code |= variable_code_subcode | units_code_subcode;
   return record_type_code;
}
//_initialize_common___________________________________________________________/
bool Tabular_file_with_TDF_convertor::initialize_import()
{  Record_code  record_type_code = initialize_common();
   //200429 FOR_EACH_IN(format_column,ED_tabular_file_format::Column,format.columns_1based,each_format_column)
   FOR_EACH_IN(format_column,ED_tabular_file_format::Column,format.columns,each_format_column) //200429
   {
      Record_code adj_record_type_code = record_type_code;
#ifdef NYI
// This crashes
      // for precipitation with where the record type code is daily time_stamp step, we use storm record type codes.
      if ((record_type_code == 0x28) && (format_column->smart_variable_code.get() == UED_STD_VC_precipitation.get()))
         adj_record_type_code = Storm_event_UED_record_type;
#endif
      const Data_record_creation_layout *recognized_creation_layout /*optional*/ //100125
         = get_recognized_creation_layout                                        //160212
         (format_column->variable_code_clad.get());                              //100125
      Import_column *import_column = new Import_column
      ( *format_column
      , adj_record_type_code
      , format.detail.timestep_units_enum.get()                                  //200429
      //200429 , format.detail_lines_delineation.time_step_units_enum.get()
      , time_step_stamp_units_code                                               //050413
      , CORN::measured_quality  // This could be specified in the format
      , recognized_creation_layout);                                             //100125

      // For dates and time columns, set the format so it can be imported accordingly.
      const std::string &units_caption = format_column->ref_units_caption();     //170531
      if (format_column->data_type_clad.get() == CORN::DT_date)                  //190506_141120
      {
         if (units_caption.length())                                             //170531
         {  import_column->date_format = new Date_format(units_caption.c_str()); //170531
            import_column->value_datetime.ref_date()
               .prefer_date_format(import_column->date_format);                  //170826_170521
         }
      }
      if (format_column->data_type_clad.get() == CORN::DT_time)                  //109506
      {
         if (units_caption.length())                                             //170531
         {  import_column->time_format= new Time_format(units_caption.c_str());  //170521
            import_column->value_datetime.ref_time()
               .prefer_time_format(import_column->time_format);                  //170826_170521
         }
      }
       if (format_column->data_type_clad.get() == CORN::DT_datetime)             //190506
       {   // the set format_string functions should parse the
           // date and time components from the caption
         if (units_caption.length())                                             //170531
         {  import_column->date_format = new Date_format(units_caption.c_str()); //170531
            import_column->value_datetime.ref_date()
               .prefer_date_format(import_column->date_format);                  //170826_170521
            import_column->time_format= new Time_format(units_caption.c_str());  //170521
            import_column->value_datetime.ref_time()
               .prefer_time_format(import_column->time_format);                  //170826_170521
         }
       }
      // Currently I am presuming the the TDF format always specifies the time
      // time step  (I could assume if there is no time step, that it is sparse scalar data)
      import_columns.append(import_column);
   } FOR_EACH_END(each_format_column)
   return true;
}
//_initialize_import___________________________________________________________/
Delineated_data_record &Tabular_file_with_TDF_convertor::provide_detail_line_data_record() provision_
{  if (!detail_line_data_record) detail_line_data_record = new Delineated_data_record
      (import_columns,"Sheet 1"
      ,format.year_offset_for_dates_with_only_two_digits);
   return *detail_line_data_record;
}
//_provide_detail_line_data_record__________________________________2010-01-16_/
bool Tabular_file_with_TDF_convertor::initialize_export()
{  Record_code  record_type_code = initialize_common();
   FOR_EACH_IN(format_column,
      ED_tabular_file_format::Column
      ,format.columns //200429 columns_1based
      ,each_format_column)
   {  Record_code adj_record_type_code = record_type_code;
#ifdef NYI
// This crashes
      // for precipitation with where the record type code is daily time_stamp step, we use storm record type codes.
      if (   (record_type_code == 0x28)
          && (format_column->variable_code_clad.get() == UED_STD_VC_precipitation.get()))
         adj_record_type_code = Storm_event_UED_record_type;
#endif
      UED::Export_column *export_column = new UED::Export_column( *format_column);
      // For dates and time columns, set the format so it can be imported accordingly.

      const std::string &units_caption = format_column->ref_units_caption();     //170531
      if (format_column->data_type_clad.get() == CORN::DT_date)                  //190506_141120
      {  export_column->date_format = new Date_format(units_caption.c_str());    //170531
         export_column->value_datetime.ref_date().prefer_date_format(export_column->date_format); //170521
      }
      if (format_column->data_type_clad.get() == CORN::DT_time)                  //190506_141120
      {  export_column->time_format = new Time_format(units_caption.c_str());    //170531
         export_column->value_datetime.ref_time().prefer_time_format(export_column->time_format); //170521
      }
      if (format_column->data_type_clad.get() == CORN::DT_datetime)              //190506_141120
      {  export_column->date_format = new Date_format(units_caption.c_str());    //170531
         export_column->time_format = new Time_format(units_caption.c_str());    //170531
         export_column->value_datetime.ref_date().prefer_date_format(export_column->date_format); //170521
         export_column->value_datetime.ref_time().prefer_time_format(export_column->time_format); //170521
      }
      // Currently I am presuming the the TDF format always specifies the time
      // time step  (I could assume if there is no time step, that it is sparse scalar data)
      export_columns.append(export_column);
   } FOR_EACH_END(each_format_column)
   return true;
}
//_initialize_export___________________________________________________________/
nat32  Tabular_file_with_TDF_convertor::import_metadata_fields(CORN::Tabular_file &tabular_file)
{  nat32  encountered = 0;
   FOR_EACH_IN(metadata_field,ED_tabular_file_format::Metadata_field
              ,format.metadata_fields_numbered,each_metadata_field)
   {
      const std::string *header_row = tabular_file.get_Nth_header(metadata_field->row_num);
      nat16 metadata_pos_num = (metadata_field->pos_num);                        //181030
      nat16 metadata_pos_ndx = metadata_pos_num -1 ;                             //181030_171129

      nat16 metadata_field_width = metadata_field->delineation.width; //200429 format. field_width;
      if (metadata_field_width == 0)
      {  // The width is not specified, presume it is read up to a delimiter
         metadata_field_width = 0xFFFF;
         nat16 field_width_max = header_row->length() - metadata_pos_ndx;
         metadata_field_width = CORN::must_be_less_or_equal_to<nat16>
            (metadata_field_width,field_width_max);
      }
      if (header_row && (header_row->length() >= (metadata_pos_ndx+metadata_field_width)))
       {
          std::string metadata_value(*header_row,metadata_pos_ndx,metadata_field_width);
          if (!set_imported_textual_metadata(*metadata_field,metadata_value))
          {    if (!set_imported_integer_metadata(*metadata_field,atol(metadata_value.c_str())))
                  set_imported_float_metadata(*metadata_field,atof(metadata_value.c_str()),false);
          }
       }
#ifdef NYI
         This isn't crutial just now, but will eventually want to implement
      Don't forget, if the header contains geolocation data
      instanciate header_identified_geolocation
       continue here
#endif
   } FOR_EACH_END(each_metadata_field)
   return encountered;
}
//_import_metadata_fields______________________________________________________/
nat32  Tabular_file_with_TDF_convertor::import_column_headers(CORN::Tabular_file &tabular_file)
{  nat32  encountered = 0;

   // Currently I am simply skipping the column header rows
   // but should eventually read them, but it is not needed at this time
   // Column header raw data rows have already been loaded into the file headers

   //ED_tabular_file_format &format = tabular_file.format ;

//             This currently isn't needed
//             May eventually be needed if units are variable  (in this case read the units for the units column header and adjust the creation record accordingly)
//             Also if variable/column order varies
//                FOR_EACH_END
   return encountered;
}
//_import_column_headers________________________________________________________
nat32  Tabular_file_with_TDF_convertor::import_header_lines(CORN::Tabular_file &tabular_file)
{  nat32  lines_encountered = 0;
   nat32  metadata_fields_encountered  = import_metadata_fields(tabular_file);
   nat32  column_headers_encountered = import_column_headers(tabular_file);
   return lines_encountered = metadata_fields_encountered + column_headers_encountered; //140601
}
//_import_header_lines__________________________________________________________
bool Tabular_file_with_TDF_convertor::get_date_time_from_data_record
(Delineated_data_record &data_rec
,CORN::Date_time &row_date_time) // const but adjust pointers
{  bool row_date_time_overridded = false;
   CORN::Year  year  = 0;  // We need to hold of composing the date until we have year month and day
   CORN::Month month = 0;  // or year and DOY because the
   CORN::DOM   dom   = 0;  // element are encounter day first,
   CORN::DOY   doy   = 0;  // attempting to simply set the day then the month as encountered will mess up the date for months that dont have 31 days and especially leap years
   //100205 We do need a validate date in order to set values
   FOR_EACH_IN(data_column,Tabular_data_column ,import_columns,each_column)
   {  FIND_FOR_EACH_IN(found_import_column
         ,import_column,ED_tabular_file_format::Column
         ,format.columns,true,each_import_column)
      {  CORN::Tabular_column &import_column_TC_layout = *import_column;         //060224
         if (&import_column_TC_layout == &data_column->TC_layout)                //060224
         {  found_import_column = import_column;
            if (import_column->record_date)                                      //061012
            {
               switch (data_column->TC_layout.data_type_clad.get())              //190506
               {
                  // if the layout has this has the time stamp record then set the row date time
                  // compose record time if available 050404
                  // (not sure how to handle if time is a data series value
                  //    maybe if the date is not implicit in the time step the date is a time series date
                  // or if the is not a full date column.)
                  case CORN::DT_time         :  row_date_time.set_time64(data_column->value_datetime.get_datetime64());    row_date_time_overridded = true;  break;
                  case CORN::DT_datetime     :  row_date_time.set_datetime64(data_column->value_datetime.get_datetime64());row_date_time_overridded = true;
                                                year = row_date_time.get_year(); //120331
                                                break;
                  case CORN::DT_date         :  row_date_time.set_date32    (data_column->value_datetime.get_datetime64());row_date_time_overridded = true;
                                                year = row_date_time.get_year(); //120331
                                                break;
                  default: break;
               }
               switch (import_column->get_units_code())
               {
                  case UT_year            :  year  = data_column->get_value_as_int16(); break;
                  case UT_day_of_year     :  doy   = data_column->get_value_as_int16(); break;
                  case UT_month           :  month = data_column->get_value_as_int16(); break;
                  case UT_day_of_month    :  dom   = data_column->get_value_as_int16(); break;
                  /* NYI
                  case UT_hour            :  row_date_time.set_hour      (data_column->get_value_as_int16());     row_date_time_overridded = true; break;
                  case UT_minute          :  row_date_time.set_minute    (data_column->get_value_as_int16());     row_date_time_overridded = true;  break;
                  case UT_second          :  row_date_time.set_second    (data_column->get_value_as_int16());     row_date_time_overridded = true;  break;
                  */
               }
            }
         }
      } FOR_EACH_END(each_import_column)
   } FOR_EACH_END(each_column)
   if (year && month && dom)                                                     //100208
   {  row_date_time.set_YMD(year,month,dom);                                     //100208
      row_date_time_overridded = true;                                           //100208
   }                                                                             //100208
   if (year && doy)                                                              //100208
   {  row_date_time.set_YD(year,doy);                                            //100208
      row_date_time_overridded = true;                                           //100208
   }                                                                             //100208
   return row_date_time_overridded;
}
//_get_date_time_from_data_record______________________________________________/
Geolocation *Tabular_file_with_TDF_convertor::render_geolocation(const std::string &station_ID) rendition_
{  if (header_identified_geolocation)
         return new Geolocation(*header_identified_geolocation);
   else  return UED::Convertor::render_geolocation(station_ID);
}
//_render_geolocation_______________________________________________2009-09-22_/
bool Tabular_file_with_TDF_convertor::import_data_line
(Delineated_data_record &data_rec
,Year               file_year)       // For some file types (CropSyst text files), the year is part of the file name
{  // first scan the columns  to see if there are any station_ID
   // The format does not have to have these columns
   // station_ID may be specified on command line
   // and time step may be implicit in the format (I.e. one record for every day).
   int col_index = 0;
   bool data_record_time_step_less_than_1_day = compare_timestep_units
      (format.detail.timestep_units_enum.get(), UT_day) < 0;                     //050518
   bool calculate_daily_values = parameters.calculate_daily && data_record_time_step_less_than_1_day;
   Geocoordinate *row_geocoord = 0;                                              //190515
   // Some files have geocoordinate on every detail line (multiple locations in the file)
   // This remains 0 if no geoccord.
   row_date_time.set_S(0);
   if (file_year) row_date_time.set_year(file_year);                             //100205
   row_date_time.set_DOY((CORN::DOY)1);
   float32               values[500];                                            //190516
   CORN::Quality_code qualities[500];                                            //190516
   //arbitrary size
   // shouldn't need to clear array because we are looking at specific columns.
   bool import_this_data_rec = is_import_data_record                             //160212
      (data_rec
      ,format
      ,import_columns
      ,header_identified_geolocation);
   if (import_this_data_rec)                                                     //160212
   {  // if there is no filter or the station_ID matches the filter
      // then we import this record.              double
      // The record may contain date time information,
      // if that is the case, update the row_date_time
      get_date_time_from_data_record(data_rec,row_date_time);
      // Note, the record may specify the units (particularly for files that contain a single
      // data series variable (I.e. only precipitation)
      if (row_date_time.get_date32() > 0)  // Blank rows will give date 0  //061010
      {  UED_file = provide_UED_file();                                          //160212
         set_validated_elements(row_date_time,data_rec,calculate_daily_values);  //160212
         // NYI   the record may specify the units (particularly for files that contain a single
         //          data series variable (I.e. only precipitation)
         // We found in PAWS that some records are output with 0.0 for all values instead of specifying marker indicator. 050802_
         nat16 column_num = 1;
         FOR_EACH_IN(data_column,Import_column,data_rec.tabular_data_columns,each_column)
         {  float32 value = data_column->get_value_as_float32();                 //140606
            Units_code units_in_target_file = data_column->TFFC_layout.get_units_code();//140606
            qualities[column_num] = CORN::missing_quality;
            if (data_column->creation_layout)
            {  // the column contains time series data
               // Currently UED only supports 32bit floats
               UED::Variable_code variable= data_column->TFFC_layout.variable_code_clad.get();
               qualities[column_num] = validation_status_to_UED_quality_attribute(data_column->validation_status);
               float32 layout_missing_value_float32
                  = atof(format.detail.missing.c_str());                         //200429
               if ((data_column->value_string ==
                     format.detail.missing)                                      //200429
                   || (CORN::is_approximately<float32>(value,layout_missing_value_float32,0.0000001)
                       && (variable != UED_STD_VC_precip)))                      //160125
               {  qualities[column_num] = CORN::missing_quality;//
                  // If the read raw data value is missing we set the value to zero with no units conversion.
                  value = 0;
                  units_in_target_file = data_column->creation_layout->options.stored_units_code;
               }
               values[column_num] = value;                                       //190516
            } // if creation layout
            else                                                                 //140606
            {  // This is probly meta data
               switch (units_in_target_file)
               {  case UC_degrees_longitude :
                  {  if (!row_geocoord)   row_geocoord = new Geocoordinate;      //190515
                     row_geocoord->set_longitude_dec_deg_f32((float32)value);
                  } break;
                  case UC_degrees_latitude  :
                  {  if (!row_geocoord)   row_geocoord = new Geocoordinate;      //190515
                     row_geocoord->set_latitude_dec_deg_f32((float)value);
                  } break;
                  case UC_meter :
                  {  const char *caption_SDF = data_column->TFFC_layout.get_caption_SDF();
                     if (strstr(caption_SDF,"evation") != 0)
                        // This is special for KVA and SWAT_GWD
                     {  // I really should have an elevation UED variable code
                        if (!row_geocoord)   row_geocoord = new Geocoordinate;   //190515
                        row_geocoord->set_elevation_meter(value);
                     }
                  } break;
               }
            }
            data_column->clear_values();                                         //100126
            // This is needed because we need to at least clear
            // the date time stamp for the row because when reading
            // spread sheet or files with blank detail lines
            // if the date field is not cleared, any text
            // (I.e. cells with captions) could trigger the existance
            // of a detail line, even though a date isn't read
            // if the values are not cleared, there may still linger
            // a date for the last actual detail line row.
            column_num++;
         } FOR_EACH_END(each_column)
         // Here we update the row time step (in case there is implicit time stamp
         // (the record may actually have date and/or time column)
         // so may set the date and or time.
         // Note that there will only be one time_step_xxxxx all others will be 0.0 at this point
         row_date_time.inc_years(time_step_years);
         row_date_time.inc_days(time_step_days);
         row_date_time.inc_minutes(time_step_minutes);
         row_date_time.inc_seconds(time_step_seconds);
      } // else there is not date on this row, ignore the row
      if (row_geocoord)
      {  Geolocation_record *curr_geoloc = UED_file->mod_geolocation_record();
         geocoordinate_approximation = 0.0001;
         bool curr_geoloc_is_valid = curr_geoloc && curr_geoloc->Geocoordinate::is_valid();
         bool curr_and_row_geocoord_matches = curr_geoloc
            && curr_geoloc->Geocoordinate::compare(*row_geocoord)==0;
         if (!curr_geoloc_is_valid || !curr_and_row_geocoord_matches)
         {  UED_file->finalize();
            delete UED_file; UED_file = 0;
            std::string geoloc_UED_file;
            row_geocoord->label_decimal_degrees_with_format
                  (geoloc_UED_file,true,4,4);
            /*200805
            CORN::OS::File_name_concrete new_UED_filename
                  (parameters.UED_filename.get_parent_directory_name_qualified()
                  ,geoloc_UED_file
                  ,CORN::OS::Extension(L"UED"));
            parameters.UED_filename.set_DEN(new_UED_filename);
            */
            CORN::OS::File_name *UED_filename
               = parameters.UED_filename_discovery.provide_primary();
            if (UED_filename)
            {
               CORN::OS::File_name_concrete *new_UED_filename
                  = new CORN::OS::File_name_concrete
                     (UED_filename->get_parent_directory_name_qualified()
                     ,geoloc_UED_file
                     ,CORN::OS::Extension(L"UED"));

               parameters.UED_filename_discovery.primary = new_UED_filename;
               parameters.UED_filename_discovery.primary_owned = false;

               //the target UED filename should be derived from geocoordinate (no elev)
               provide_UED_file();
               parameters.UED_filename_discovery.primary = 0;
               parameters.UED_filename_discovery.primary_owned = true;
            }
            Geolocation_record *new_geoloc = new UED::Geolocation_record(*row_geocoord);
            UED_file->take_geolocation_record(new_geoloc);
         } // else now change in record geoloc
      }
      nat16 column_num = 1;
      FOR_EACH_IN(data_column,Import_column,data_rec.tabular_data_columns,each_column) //190516
      {  if (qualities[column_num] != CORN::missing_quality)
         {  Units_code units_in_target_file = data_column->TFFC_layout.get_units_code();
            const UED::Data_record_creation_layout *actual_set_creation_layout = data_column->creation_layout;   //110821
            CORN::Quality_clad attribute(CORN::measured_quality); // presume imported data is real
               // but do some basic valuation checks
            attribute.assume_code(qualities[column_num]);
            UED::Time_query_single_date set_time_query(row_date_time.get_datetime64()); //050404
            bool created = false;
            float32 value_set = UED_file->set_value
                  (values[column_num],set_time_query,units_in_target_file
                  ,attribute
                  ,*actual_set_creation_layout                                   //100222
                  ,created,false);
            if (calculate_daily_values) // from (I.e.) hourly, 30 minute, 15minute etc. data
                  inject_calculated_values();
         }
         column_num++;
      } FOR_EACH_END(each_column)
   } // else we simply ignore this row, it doesn't have a station we want
   return true;
}
//_import_data_line_____________________________________2019-05-16__2010-01-06_/
nat32  Tabular_file_with_TDF_convertor::import_data_lines
(CORN::Tabular_file &tabular_file
,Year               file_year)      // For some file types (CropSyst text files), the year is part of the file name
{  // We should now already have skipped headers and column headers and should be at the first detail line
   int lines_encountered = 0;  // This is currently just something to return
   nat16 data_row_number = 1; // 1 based
   Delineated_data_record data_rec(import_columns,"Sheet 1",format.year_offset_for_dates_with_only_two_digits);        // may want to use provide_detail_line_data_record()
   for (bool at_eof = !tabular_file.get(data_rec)  // May need to be get next
       ;!at_eof
       ;at_eof= !tabular_file.get_next(data_rec))
   {  lines_encountered += import_data_line(data_rec,file_year);                 //100116
      data_row_number++;
   }  // for each record
   FOR_EACH_IN(data_column,Import_column,data_rec.tabular_data_columns,each_column) //140608
   {  if (data_column->TFFC_layout.missing_is_0)
      {  UED_file->goto_BOF();
         for (Binary_data_record_cowl *data_rec = UED_file->goto_next_data_record()
             ;data_rec
             ;data_rec = UED_file->goto_next_data_record())
         {  if (data_rec->get_variable_code() == data_column->TFFC_layout.variable_code_clad.get())
               data_rec->set_missing_as(0.0,CORN::measured_quality);
         }
      }
   } FOR_EACH_END (each_column)
   return lines_encountered;
}
//_import_data_lines________________________________________________2005-04-04_/
nat32  Tabular_file_with_TDF_convertor::perform_import()
{  parameters.location_info_imported = false;
   Year file_year = 0;
   initialize_import();   // could be moved to constructor
   CORN::Tabular_file  *tabular_file = provide_tabular_file(file_year);
   if (tabular_file)                                                             //100113
   {        // The tabular_file should read over any pretext (up to begin_marker) automatically.
      // Warning, make sure the skip of any pretext is working (I think corn tabular text does this)
      nat32  header_line_count = import_header_lines(*tabular_file);
      nat32  data_line_count   = import_data_lines(*tabular_file,file_year);
      // any post text (after end_marker) is ignored.
   } //  else  In the case of Excel perform_import() is overridden
   return tabular_file ? EXIT_SUCCESS : EXIT_FAILURE; // haven't decided what to return yet
}
//______________________________________________________________________________
void Tabular_file_with_TDF_convertor::tuplize_database
(modifiable_ Tuple_index      &export_tuples)                                    //141013
{  // Caller has validated UED_file existance
   UED_file->goto_BOF();
   for (UED::Binary_data_record_cowl * from_record
         = (UED::Binary_data_record_cowl *)UED_file->goto_next_data_record()     //151125
          ; from_record
          ; from_record = (UED::Binary_data_record_cowl *)UED_file->goto_next_data_record())
   {  bool column_exported = false;
       { nat16 export_column_index = 0;
          FOR_EACH_IN(export_column,Export_column,export_columns,each_col_desc)
          { // Try each column to see if it can export the data recordexport_grid,
            // the output file, and the data record to be exported.
             if (export_column->tuplize(export_column_index,*from_record,export_tuples,*UED_file
                  ,  export_start_date_time.get_datetime64()                     //141013
                  ,  export_end_date_time.get_datetime64()                       //141013
                  ,  arguments.seasonal))                                        //181106
                column_exported = true;
             export_column_index += 1;
          } FOR_EACH_END(each_col_desc)
       }
   } // each data record
}
//______________________________________________________________________________
bool Tabular_file_with_TDF_convertor::set_metadata_field_text
(std::string &line,size_t pos,const std::string &text)
{  size_t text_length = text.length();
   if (!text_length) return false;
   size_t line_length = line.length();
   size_t required_string_length_for_this_caption = (pos + text_length);
   if (line_length < pos)
   {  size_t tab = pos - line_length;
      string padding(tab,' ');
      line.append(padding);
      line_length = line.length();
   }
   if (line_length == pos)
      line.append(text);
   else
   {  size_t required_string_length_for_this_field = (pos + text_length);
      if (line_length < required_string_length_for_this_field)                   //100108
         line.resize(required_string_length_for_this_field,'M');                 //100108
      line.replace(pos,text_length, text);
   }
   return true;
}
//______________________________________________________________________________
nat32  Tabular_file_with_TDF_convertor::export_metadata_fields()
{  // find the number of header rows and columns
   std::string header_line[100]; // Arbitrary number of header lines (index 0 not currently used)
   for (int i = 0; i < 100; i++) header_line[i].clear();
   int last_header_row = 0;
   CORN::OS::Directory_name_concrete CWD;                                        //181030
   FOR_EACH_IN(metadata_field,ED_tabular_file_format::Metadata_field
              ,format.metadata_fields_numbered,each_header)   //100107
   {  // Write the caption
      // WARNING may need to offset the row to the to after the start data marker  100111
      size_t caption_length =  metadata_field->caption.length();
      if (caption_length)
      {  if (set_metadata_field_text
            (header_line[metadata_field->caption_row]
            ,metadata_field->caption_col-1,metadata_field->caption))
            last_header_row = std::max<nat16>(last_header_row,metadata_field->caption_row);
      }
      // Write the value                                                         //100111
      std::string field_value;
      switch (metadata_field->field_data_clad.get())                             //190506
      {  case ED_tabular_file_format::Metadata_field::FD_static_text :
         {  field_value = metadata_field->static_text;
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_general_comment :
         {  // get from UED database description record with database_comment_index
            CORN::Text_list comments;
            nat16 comment_count = UED_file->get_comments(comments);
            CORN::Item *primary = dynamic_cast<CORN::Item *>(comments.get_at(0));
            if (primary)
               primary->append_to_string(field_value);                           //180119
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_database_description :
         {  // get from UED database description record with database_description_index
            CORN::Text_list descriptions;
            nat32 description_count = UED_file->get_descriptions(descriptions);
            CORN::Item *primary = dynamic_cast<CORN::Item *>(descriptions.get_at(0));
            if (primary)
               primary->append_to_string(field_value);                           //180119
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_generating_application :
         {  // NYI get from UED generating application record
            const Generating_application_record *gen_app = UED_file->get_generating_application_record();
            if (gen_app)
            {  field_value = gen_app->get_description();
               field_value.append(" ");
               CORN::append_nat16_to_string(gen_app->get_version(),field_value);
               field_value.append(".");
               CORN::append_nat16_to_string(gen_app->get_major(),field_value);
               field_value.append(".");
               CORN::append_nat16_to_string(gen_app->get_minor(),field_value);
            }
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_location :
         {  const UED::Geolocation_record  *geoloc_rec = UED_file->ref_geolocation_record();
            if (geoloc_rec)
            {  char value_buffer[50]; value_buffer[0] = 0;
               switch (metadata_field->location_var_clad.get())                  //190506
               {
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_station_ID                :
                     field_value.assign(geoloc_rec->get_station_ID_code());
                     break;
                     // Note that decimal marks probably should be a property of the file,
                     // but currently they are denoted for column header and detail lines separately
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_latitude_decimal_degrees  :
                     CORN::float32_to_cstr_formated(geoloc_rec->get_latitude_dec_deg(), value_buffer
                     ,metadata_field->delineation.width                          //200429
                     ,metadata_field->format.precision                           //200429
                     ,format.document.separation.decimal                         //200429
                     ,false);    break;

                  case ED_tabular_file_format::Metadata_field::LOC_VAR_latitude_deg_min_sec      :
                     geoloc_rec->decimal_degrees_to_DMS_string(
                        geoloc_rec->get_latitude_dec_deg(),false
                        ,metadata_field->format.precision                        //200429
                        //200429 ,metadata_field->precision
                        ,field_value);       break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_longitude_decimal_degrees :
                     CORN::float32_to_cstr_formated(geoloc_rec->get_longitude_dec_deg()
                     , value_buffer
                     ,metadata_field->delineation.width                          //200429
                     ,metadata_field->format.precision                           //200429
                     ,format.document.separation.decimal                         //200429
                     ,false);    break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_longitude_deg_min_sec     :
                     geoloc_rec->decimal_degrees_to_DMS_string
                     (geoloc_rec->get_longitude_dec_deg(),true
                     ,metadata_field->format.precision,field_value); break;      //200429
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_meters          :
                     CORN::float32_to_cstr_formated(geoloc_rec->get_elevation()
                     , value_buffer
                     ,metadata_field->delineation.width                          //200429
                     ,metadata_field->format.precision                           //200429
                     ,format.header.separation.decimal                           //200429
                     ,false);          break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_feet            :
                     CORN::float32_to_cstr_formated(m_to_foot(geoloc_rec->get_elevation()),value_buffer
                     ,metadata_field->delineation.width                          //200429
                     ,metadata_field->format.precision                           //200429
                     ,format.document.separation.decimal                         //200429
                     ,false); break;
/*
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_meters          : CORN::float32_to_cstr_formated(geoloc_rec->get_elevation(), value_buffer,metadata_field->field_width, metadata_field->precision,format.column_headers_delineation.decimal_mark.c_str()[0],false);          break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_feet            : CORN::float32_to_cstr_formated(m_to_foot(geoloc_rec->get_elevation()),value_buffer,metadata_field->field_width, metadata_field->precision,format.column_headers_delineation.decimal_mark.c_str()[0],false); break;
*/
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_country_name              : field_value.assign(geoloc_rec->get_country_name().c_str());                                           break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_state_name                : field_value.assign(geoloc_rec->get_state_name().c_str());                                             break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_county_name               : field_value.assign(geoloc_rec->get_county_name().c_str());                                            break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_description               : /* NYI */                                                                                     break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_station_name              : field_value.assign(geoloc_rec->get_station_name().c_str());                                          break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_station_number            : CORN::nat32_to_cstr(geoloc_rec->get_station_number(),value_buffer,10);                                         break;
                  case ED_tabular_file_format::Metadata_field::LOC_VAR_comment                   : field_value.assign(geoloc_rec->get_comment());                                                break;
               } // switch
               if (value_buffer[0] != 0) field_value.assign(value_buffer);
            } // if
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_scalar_data :
         {
      // NYI,  This option is if there is a scalar record in the UED file we want to output
      // I dont currently have any UED files with scalar record except CropSyst soil outputs with number of layers.
      // Here I would scan (read_next_record) the UED file for matching UED_scalar_record_code, and UED_scalar_variable_code
      // convert the value units to desired UED_scalar_units_code_enum
             field_value = "N.Y.I";
         } break;
         case ED_tabular_file_format::Metadata_field::FD_parent_path :           //181030
         {
            // instead of CWD should use UED file's parent path
            CORN::Unidirectional_list CWD_names;
            CWD.append_path_list_to(CWD_names);
            nat32 parent_index = CWD_names.count() - metadata_field->parent_path;
            CORN::Item_wstring *parent_name = dynamic_cast<CORN::Item_wstring *>
               (CWD_names.get_at(parent_index));
            if (parent_name)
               CORN::wstring_to_string(*parent_name,field_value);

         } break;
      }
      // always remember the last header row irresective of if text is set or not 181030
         (set_metadata_field_text(header_line[metadata_field->row_num],metadata_field->pos_num -1,field_value)) //120115
         ;
      last_header_row = max<uint16>(last_header_row,metadata_field->row_num);
   } FOR_EACH_END(each_header)
   // header rows are 1 based
   for (int header_row = 1;header_row <= last_header_row;header_row++)
   {  // header columns are 1 based so skipping the first character at index 0
      // WARNING may need to offset the row to the to after the start data marker//100111
      tabular_file->goto_raw_line(header_row);                                   //100111
      tabular_file->write_raw_string(header_line[header_row].c_str());
   }
   return 0;
}
//_export_metadata_fields______________________________________________________/
nat32  Tabular_file_with_TDF_convertor::export_column_headers()
{  nat32  exported_col_header_rows = 0;
   SDF_Clad header_rows[20];
      // Allocate a text list for each row (format.column_headers_delineation.rows)
            // HACK: 20 is an arbitrary number should use dynamic allocation with respect to format.column_headers_delineation.rows.
            // For each row there will be one item corresponding to the portion of the caption of that column on that row.
   tabular_file->goto_raw_line(format.header.extent.begin_offset+1);             //200429
   //200429 tabular_file->goto_raw_line(format.column_headers_delineation.start_row_number);
   // else we are dumping directory to cout
   FOR_EACH_IN(column,ED_tabular_file_format::Column,format.columns/*200429 columns_1based*/,each_col_desc)
   {  SDF_Clad /*200202 SDF_List*/ caption_SDF_list(column->get_caption_SDF());
      std::string sheet_name(column->sheet.name); if (!sheet_name.length()) sheet_name.assign("Sheet 1");
      tabular_file->goto_sheet(sheet_name);
      for (int8 row_idx = std::min<int16>
/*200429
         (format.column_headers_delineation.rows
         -format.column_headers_delineation.units_rows                           //181030
*/
         (format.header.extent.rows                                              //200429
         -format.header.units_rows                                               //200429181030
         -1,19)  // HACK The minimum is because using fixed array size
          ;row_idx >=0
          ;row_idx--)
      {  // For each column header, take the description (caption) and split the text into words
         // and distribute the words two the number of rows allocated for column header.
         // I.e.  If format.column_headers_delineation.rows is 4 and the
         // column header is  "The caption"
         // 'caption' will be appended to the forth row text list,
         // 'The' to the third, and the first and second will have appended empty string.
         // If the caption is "This is a long caption"
         // the text will be distributed as 'This is' 'a' 'long' 'caption'.
         // The caption in the TDF file can have a '_' to prevent word wrap'
         // If the caption is long the TDF caption may
         // indicate the line brakes using system data format for the caption string.
         Item_string *caption_fragment =
            dynamic_cast < Item_string *>(caption_SDF_list.pop_tail());
         if (!caption_fragment) caption_fragment = new Item_string("");
         if (row_idx == 0)
         {  // In this case the caption has more rows than number of rows reserved for column header captions
            // So will put the remaining caption text on the top line.
            // (Often occurs when the format designates only one line for column header captions).
            for (Item *remaining_caption_fragment = dynamic_cast<Item *>(caption_SDF_list.pop_tail())
                ;remaining_caption_fragment
                ;remaining_caption_fragment =  dynamic_cast<Item *>(caption_SDF_list.pop_tail()))
            {  std::string caption_fragment_value; remaining_caption_fragment->key_string(caption_fragment_value);
                caption_fragment_value.append(" ");
                caption_fragment->append_to_string(caption_fragment_value);      //180119
                caption_fragment->assign( caption_fragment_value);               //171125
                delete remaining_caption_fragment;
            }
         }
         header_rows[row_idx].append(caption_fragment );
      } // for row_idx
   } FOR_EACH_END(each_col_desc)
   bool delimiter = format.header.has_delimiter();                               //200429_190107
   for (nat8 CH_row_index = 0
      ; CH_row_index < format.header.extent.rows                                 //200429
         - format.header.units_rows                                              //181030
      /*200429
      ; CH_row_index < format.column_headers_delineation.rows
         - format.column_headers_delineation.units_rows                          //181030
      */
      ; CH_row_index++)
   {
      tabular_file->goto_raw_line
         ((format.header.extent.begin_offset+1) +CH_row_index);                  //200429
         //200429 (format.column_headers_delineation.start_row_number+CH_row_index);
      //200429 if (format.column_header_static.length())  // The TDF file has static text for this row that superceeds column captions //100104_
      /*200429 decided to drop this
      if (format. column_header_static.length())  // The TDF file has static text for this row that superceeds column captions //100104_
         // column_header_static text is currently only applicable to text files. but it may have some applicability to Spreadsheets, however normally we will compose the column header text
         tabular_file->write_raw_string(format.column_header_static.c_str());    //100104
      else
      */
      {
      // After all the column headers have been split into the row text lists
      // write each column header row honoring delineation field width and formatting.
      int curr_pos_num = 1; // 1 based                                           //100104
      FOR_EACH_IN(column,ED_tabular_file_format::Column,format.columns /*200429 columns_1based*/,each_col_desc);
      {  std::string sheet_name(column->sheet.name); if (!sheet_name.length()) sheet_name.assign("Sheet 1");   //200429
         tabular_file->goto_sheet(sheet_name);
         CORN::Item *caption_fragment_item = dynamic_cast<CORN::Item *>
            (header_rows[CH_row_index].pop_first());
         std::string caption_fragment;
         if (caption_fragment_item)                                              //180119
            caption_fragment_item->append_to_string(caption_fragment);           //180119
         delete caption_fragment_item;
         if (format.header.tabulation_clad.get() == fixed_TABULATION)            //190506
         {  // For fixed column widths delimit mode we set the width of the string to the width of the
             caption_fragment.resize(column->get_field_width(),' ');
            // It is possible that the TDF description with fixed column widths, skips character columns and/or has white space.
            // we need to account for these                                      //100104
            tabular_file->goto_field_column(curr_pos_num);                       //100104
         }
         // Note that field sizing is handled in write_string
         if (delimiter&&(curr_pos_num > 1))                                      //110812
         {  std::string delimiter; delimiter = format.header.separation.delimiter; //200429 190107
            tabular_file->write_delimiter_string(delimiter.c_str());             //190107
         }
         tabular_file->write_field_string(caption_fragment.c_str()
            ,format.header.separation.quotation);                                //200429
         curr_pos_num++;                                                         //171129
      } FOR_EACH_END(each_col_desc)
      }
      // advance line
      exported_col_header_rows++;                                                //160306
   }
   //I also need to similarly output up to the units text.
   if (format.header.units_rows)                                                 //070307
   {  // Output column header units description
      uint16 column_header_number = 1;  // column numbers are 1 based            //050902
      tabular_file->goto_raw_line
         (  format.header.extent.begin_offset+1                                  //200429
          + format.header.units_row_offset);                                     //200429
          /*200429
         (  format.column_headers_delineation.start_row_number
          + format.column_headers_delineation.units_row_offset);
          */
      int curr_pos_num = 1; // 1 based
      FOR_EACH_IN(column,ED_tabular_file_format::Column,format.columns,each_col_desc)
      {  std::string sheet_name(column->sheet.name); if (!sheet_name.length()) sheet_name.assign("Sheet 1"); //200429
         tabular_file->goto_sheet(sheet_name);
         std::string units = column->get_units_caption();  // preferred units description if specified
         if (!units.length() && column->get_units_code())  // if not specified derived the units description of units_code
            units = get_units_description(column->get_units_code() ,true);
         UED::Variable_code_clad &smart_var_code = column->variable_code_clad;
         //200429if (format.column_headers_delineation.delimit_mode_clad.get()           //190506
         if (format.header.tabulation_clad.get()                                 //200429_190506
             == fixed_TABULATION)                      //090712
         {  // For fixed column widths delimit mode we set the width of the string to the width of the
            // It is possible that the TDF description with fixed column widths, skips character columns and/or has white space.
            // we need to account for these                                      //100104
            tabular_file->goto_field_column(curr_pos_num);
         }
         if (delimiter && curr_pos_num > 1)                                      //090712
         {
            std::string delimiter(format.header.separation.delimiter);           //200429
            //200429 std::string delimiter;format.column_headers_delineation.append_delimiter(delimiter);
            tabular_file->write_delimiter_string(delimiter.c_str());
         }
         std::string units_with_parens;
         //200429 if (format.column_headers_delineation.units_parenthesis)
         if (format.header.units_parenthesis)                                    //200429
            units_with_parens = "(" + units + ")";
         else units_with_parens = units; // no parens

         if (smart_var_code.is_layered() && column->spread )
         {  for (int layer = 0; layer < column->spread; layer++)
            {  tabular_file->write_field_string(units_with_parens.c_str()
               ,format.header.separation.quotation);                             //200429
               //200429 ,format.column_headers_delineation.separation.quotation);         //200220
               //200220 ,format.column_headers_delineation.text__quote_mode_clad.get());   //190506
               // Note the format may reserve multiple rows for units currently only using a single row //050901
            }
         } else
            tabular_file->write_field_string(units_with_parens.c_str()
              ,format.header.separation.quotation);                              //200429_200220
         // Note the format may reserve multiple rows for units currently only using a single row
         curr_pos_num++;                                                         //171129_100104
      } FOR_EACH_END(each_col_desc)
      exported_col_header_rows++;                                                //090709
   }
   return exported_col_header_rows;                                              //090709
}
//_export_column_headers____________________________________________2009-07-12_/
nat32  Tabular_file_with_TDF_convertor::export_header_lines()
{
   // Tabular file open is now down in constructor (or eventually in initialize) //141016
   nat32  metadata_fields_encountered = export_metadata_fields();
   nat32  column_headers_encountered  = export_column_headers();
   nat32  header_line_count = metadata_fields_encountered + column_headers_encountered;
   return header_line_count;
}
//_export_header_lines_________________________________________________________/
bool Tabular_file_with_TDF_convertor::transfer_tuple_column
(CORN::Date_time &tuple_date_time
, float32 value
, nat16 export_column_number   //appears to be column index
, modifiable_ UED::Export_column *export_column)
{ // CropSyst will override this to output crop name, growth stage and other text
 // Also set the date and date components
 // If i recall correctly all other fields should already be bound
   bool transferred = true;                                                      //141014
   bool date_or_time_set = false;
   bool missing = false;                                                         //070524

   modifiable_ ED_tabular_file_format::Column &TFFC_layout = export_column->TFFC_layout;

   //190107 unused const char *delimiter = export_column_number ? format.detail_lines_delineation.get_delimiter() : "";  // Don't prepend delimiter for column 0   //050902
   if (TFFC_layout.record_date)                                                  //041004
   {  // Output the tuples date time
      date_or_time_set = true;                                                   //041004
      switch (export_column->TFFC_layout.get_units_code())
      {  // Various date variables
         case UT_date :
          { export_column->value_datetime.set_datetime64(tuple_date_time.get_datetime64());
            export_column->value_datetime.inc_years(parameters.offset_years);    //100303
               // Options to shift the years in the output (also used to overcome the Y2K problem);
            if (export_column->date_format)                                      //170531
               export_column->date_format->set_string(export_column->TFFC_layout.date_time_format); //17053_141120
            export_column->value_float64 = tuple_date_time.get_datetime64();     //160517
            export_column->value_float32 = export_column->value_float64;         //171017
          } break;
         // NYI case UT_time
         // NYI case UT_date_time
         case UT_second:         export_column->set_int_value((int16)tuple_date_time.get_second());   break;
         case UT_minute :        export_column->set_int_value((int16)tuple_date_time.get_minute());  break;
         case UT_hour:           export_column->set_int_value((int16)tuple_date_time.get_hour());    break;
         case UT_day_of_year :   export_column->set_int_value((int16)tuple_date_time.get_DOY());     break;
         case UT_year   :
         {  int year = tuple_date_time.get_year();
            year += parameters.offset_years;  // Options to shift the years in the output (also used to overcome the Y2K problem); //100303_
            // Handle the case of stupid formats with only 2 year digits
            if (export_column->TFFC_layout.get_field_width() == 2)
               year = year % 100;
            export_column->set_int_value((int16)year);
         } break;
         case UT_day_of_month :  export_column->set_int_value((int16)tuple_date_time.get_DOM());    break;
         case UT_month :
            export_column->set_int_value((int16)(tuple_date_time.get_month()));  break;
      }
   }
   if (!date_or_time_set)
   {  if (export_column->TFFC_layout.variable_code_clad.get())
      {  if ( export_column->encountered)
          {  if (export_column->TFFC_layout.get_units_code() == UT_date)         //040931
             {
               export_column->set_float_value(value);                            //171120
               export_column->value_datetime.set_datetime64(value);              //141201
               export_column->value_datetime.inc_years(parameters.offset_years);
               // Options to shift the years in the output (also used to overcome the Y2K problem);   //100303
               // Incase dates are stored in column as numbers.
               export_column->set_float64_value(export_column->value_datetime.get_datetime64()); //141021
               export_column->TFFC_layout.data_type_clad.set(DT_date);           //190506
             } else
             { if (value > 999999999.9) value = 0;                               //111205
                  // this is a hack incase there is invalid data in the UED file.
               export_column->set_float_value(value);
             }
         } else
         {  if (export_column->TFFC_layout.data_type_clad.get() == CORN::DT_string) //190506_041013
            {  //this could be something like Crop name or growing season or growth stage.
               //check database for period with matching UED code for this date.
               std::string period_description("N/A");                            //041014
               const UED::Period_record *period_rec = UED_file->get_period
                  (UED_period_variable_code_option|UED_period_start_date_time_option|UED_period_end_date_time_option
                  ,tuple_date_time.get_datetime64()                              //041013
                  ,0,0,0,export_column->TFFC_layout.variable_code_clad.get());

               if (period_rec)
                  period_description.assign(period_rec->text);                   //041013
#ifdef NYN
               else
               {  // This is special for CropSyst shouldn't be needed because period text now has full description
                  if (export_column->TFFC_layout.variable_code_clad.get() ==  CSVC_crop_base_growth_stage /* 0x8021000e */)
                  {
                     switch () {}
                  }
               }
#endif
               export_column->value_string.assign(period_description);
            } else
            {  missing = true;                                                   //070524
            }
         }
      } else // Don't know the variable code
         missing = true;                                                         //070524
   }
   if (missing)                                                                  //070524
   {  nat8 parent_path = export_column->TC_layout.parent_path;                   //181104
      std::string missing(format.detail.missing);                                //200429
      if (parent_path)                                                           //181104
      {  // This was special output for AgMIP small holder
         // instead of CWD should use UED file's parent path
         CORN::OS::Directory_name_concrete CWD;
         CORN::Unidirectional_list CWD_names;
         CWD.append_path_list_to(CWD_names);
         nat32 parent_index = CWD_names.count() - parent_path;
         CORN::Item_wstring *parent_name = dynamic_cast<CORN::Item_wstring *>
               (CWD_names.get_at(parent_index));
         if (parent_name)
               CORN::wstring_to_string(*parent_name,export_column->value_string);
      }
      else if (export_column->TC_layout.static_text.length())                    //181104
      {
         export_column->value_string = export_column->TC_layout.static_text;
      }
      else
      export_column->value_string  .assign(missing);
      export_column->value_float64  = atof(missing.c_str());
      export_column->value_float32  = atof(missing.c_str());
      export_column->value_int16    = atoi(missing.c_str());
      export_column->value_int32    = atoi(missing.c_str());
      export_column->value_datetime.set_YMD(9999,9,9);
   }
   return transferred;                                                           //141014
}
//_transfer_tuple_column____________________________________________2004-08-29_/
/* This is for debugging
namespace UED {
const Period_record *Database_file::get_period
(nat32      consider_options
,datetime64 at_date
,nat32      i_application_code
,nat32      i_enumeration
,nat32      i_index
,UED_variable_code i_variable_code)
{
   // This returns the first period encountered that has a date time that matches the time query,
   //  options and values for selected options.
   // Returns 0 if no matching period.
   if (!initialized) initialize();                                               //050125
   Period_record *period = 0;
   FIND_FOR_EACH_IN(found_node,record_node,BINARY_RECORD_INDEX_NODE,index,true,each_record_node)
   {  if (record_node->get_record_code() == UED_period)
      {  Period_record *period_record =  (Period_record *)record_node->get(*this);
         {  // Now check if the dates match the query
            bool matchs = true;
            UED_variable_code period_variable_code =  period_record->variable_code;
            if ((consider_options & UED_period_start_date_time_option)  && (at_date < period_record->start_date_time)) matchs = false;
            if ((consider_options & UED_period_end_date_time_option)    && (at_date > period_record->end_date_time))   matchs = false;
            if ((consider_options & UED_period_application_code_option) && (i_application_code  != period_record->application_code))  matchs = false;
            if ((consider_options & UED_period_variable_code_option) && (i_variable_code  != period_variable_code))  matchs = false;
            if ((consider_options & UED_period_enumeration_option) && (i_enumeration       != period_record->enumeration))       matchs = false;
            if ((consider_options & UED_period_index_option) && (i_index             != period_record->index))             matchs = false;

if (period_variable_code  == 0x8021000e)  // fod debugging only
matchs = true;


            if (matchs)
            {  found_node = record_node;
               period = period_record;
   } } } } FOR_EACH_END(each_record_node)
   return period;
}
}
*/
//_get_period_______________________________________________________2004-10-11_/
nat32  Tabular_file_with_TDF_convertor::export_data_lines
(CORN::Container &sorted_tuples)                                                 //150405
{  nat32  data_line_count = 0;
   CORN::Delineated_data_record data_rec(export_columns,"Sheet 1",format.year_offset_for_dates_with_only_two_digits);
   if (arguments.is_verbose(3))  // progress? program-entry?
      std::clog << "Exporting" << std::endl;
   FOR_EACH_IN(tuple,Export_tuple,sorted_tuples,each_tuple)
   {  if ((tuple->date_time.temporal_compare(export_start_date_time) >= 0)
         // Tuple index may generate tuple sets for dates not with in the output //040929
         &&(tuple->date_time.temporal_compare(export_end_date_time) <= 0))
      {  if (arguments.is_verbose(3)) //inclusion?
            std::clog << tuple->date_time.as_string() << '\r';
         int tuple_count = tuple->data.get_count();
         nat16 export_column_number = 0;

         FOR_EACH_IN(export_column,UED::Export_column,export_columns,each_column)
         {
            /*160212 This was special to provide RHmax min average when not available
              should now have this in ClimGen
            if (export_column->TFFC_layout.variable_code_clad.get() == UED_STD_VC_RHavg) //140610
            {
               CORN::Date earliest_date(export_start_date_time.get_date32());
               nat32 tuple_date_index = earliest_date.days_between(tuple->date_time);
               float32 RHmx = RHmax.get(tuple_date_index);
               float32 RHmn = RHmin.get(tuple_date_index);
               float32 RHav = (RHmx + RHmn) / 2.0;
               export_column->set_float_value(RHav);
               float32 RHav_in_target_units = RHav;                              //150405
               if (export_column->TFFC_layout.units.get() == UC_decimal_percent)
                  RHav_in_target_units = RHav / 100.0;
               export_column->encountered = true;
               transfer_tuple_column(tuple->date_time,RHav_in_target_units,export_column_number,export_column);
            } else
            */
            transfer_tuple_column(tuple->date_time,tuple->data.get(export_column_number ),export_column_number,export_column);
            export_column_number += 1;                                           //040929
         } FOR_EACH_END(each_column)
         tabular_file->set(data_rec);                                            //070307
         data_line_count +=1;                                                    //070307
      } // tuple in date range
   } FOR_EACH_END(each_tuple)
   if (arguments.is_verbose(2)) // progress?
   std::clog << std::endl;
   return data_line_count;
}
//_export_data_lines___________________________________________________________/
nat32  Tabular_file_with_TDF_convertor::perform_export()
{  nat32 export_count = 0;
   initialize_export();
   Year file_year;   // Currently there is no year separation
   tabular_file = provide_tabular_file(file_year);
   if (!tabular_file) return 0; // Probably unable to determine the desired targe file type  //070307
   if (format.extension_year_digits) // when exporting to separate year files, each file will be provided in the annual loop
   { delete tabular_file; tabular_file = 0; }

   Tuple_index      export_tuples;                                               //180107
   /*180107 Warning, it would be better to setup the tuples here
      but it would require openning all the UED file just
      to get the start year,  currently presuming all the
      UED files span the same period
    export_tuples.setup                                                          //180107
       (limit_time_step_units // tuple_time_step    // This is the timestep of the output file detail lines
       ,UT_year   // This is the timestep of this index level
       ,export_start_date_time.get_year()
       ,export_end_date_time.get_year() - export_start_date_time.get_year() + 1); // years
   */

   FOR_EACH_IN(UED_filename,CORN::OS::File_name
              ,parameters.UED_filename_discovery.discovered,UED_iter)
   {  tuplize_UED_file((*UED_filename),export_tuples);                           //200805
   } FOR_EACH_END(UED_iter)

/*200805
   if (parameters.UED_filenames.empty())                                         //171125
      export_count = tuplize_UED_file(parameters.UED_filename,export_tuples);    //180107
   else
   {  SDF_Clad UED_filenames(parameters.UED_filenames.c_str(),false);            //180308
      FOR_EACH_IN(UEDfilename,CORN::Item_string,UED_filenames,each_UEDfile)
      {  std::wstring UED_filename_raw;                                          //180119
         UEDfilename->append_to_wstring(UED_filename_raw);                       //180119
         CORN::OS::File_name_concrete UED_filename(UED_filename_raw);            //180119
         tuplize_UED_file(UED_filename,export_tuples);                           //180107
      } FOR_EACH_END(each_UEDfile)
   }
*/
   nat32 data_line_count = extract_tuples(export_tuples);                        //180107
   commit_export();                                                              //171125
   return data_line_count;
}
//_perform_export___________________________________________________2014-10-13_/
nat32 Tabular_file_with_TDF_convertor::tuplize_UED_file
(const CORN::OS::File_name &UED_filename
,Tuple_index &export_tuples)                                                     //180107
{
   if (!CORN::OS::file_system_engine.exists(UED_filename))                       //120412
   {  cerr << "Unable to find UED file to export from:" << UED_filename.c_str() << endl; //070307
      press_return_before_exit = true;
      return 0;                                                                  //070307
   }

//200501 may want to use provide here

   if (UED_file) delete UED_file;
   UED_file = new UED::Database_file_indexed(UED_filename.c_str(),(std::ios_base::in ),true); //140705_130401
   if (UED_file)                                                                 //170404
      UED_file->initialize();                                                    //170404
   else
   {  std::cerr << "Unable to open UED file:" << UED_filename.c_str() << endl;   //070307
      press_return_before_exit = true;
      return 0;                                                                  //070307
   }
   nat32 tuplized = tuplize_opened_UED_file                                      //171125
      (export_tuples);                                                           //180107
   return tuplized;                                                              //171125
}
//_tuplize_UED_file_________________________________________________2017-11-25_/
nat32 Tabular_file_with_TDF_convertor::tuplize_opened_UED_file
(
Tuple_index      &export_tuples                                                  //170107
)
{
   if (!UED_file) return 0;                                                      //180107
   export_start_date_time.set_datetime64(UED_file->get_earliest_date_time());    //150610
   export_end_date_time  .set_datetime64(UED_file->get_latest_date_time());      //150610
   if (export_start_date_time.get_year() < parameters.year_earliest)             //101127
       export_start_date_time.set_year(parameters.year_earliest);                //101127
   if (export_end_date_time.get_year() > parameters.year_latest)                 //101127
       export_end_date_time.set_year(parameters.year_latest);                    //101127

   if (parameters.date_time_earliest)                                            //181101
      export_start_date_time.set_datetime64(parameters.date_time_earliest);      //181101
   if (parameters.date_time_latest)                                              //181101
      export_end_date_time.set_datetime64(parameters.date_time_latest);          //181101

   // Setup export tuples                                                        //070307
   Units_code time_step_units                                                    //170411
      = format.detail.timestep_units_enum.get();                                 //200429
      //200429 = format.detail_lines_delineation.time_step_units_enum.get();
   Units_code limit_time_step_units                                              //070307
      = (time_step_units > 0) ? time_step_units : UT_day;                        //070307

   //180107 Currently presuming that all the UED files (if multiple)
   // that are processed have the same start year, but should at least
   // check
    export_tuples.setup
    (limit_time_step_units // tuple_time_step    // This is the timestep of the output file detail lines
    ,UT_year   // This is the timestep of this index level
    ,export_start_date_time.get_year()
    ,export_end_date_time.get_year() - export_start_date_time.get_year() + 1); // years

   if (arguments.is_verbose(3)
       && parameters.UED_filename_discovery.provide_primary()                               //200805
      ) //progress? discovery?
   {
      std::clog << "Tuplizing database:"
      << parameters.UED_filename_discovery.provide_primary()->c_str()                       //200805
      //200805 parameters.UED_filename.c_str()
      << std::endl << "(This may take a few seconds)" <<std::endl;
   }
   tuplize_database(export_tuples);                                              //141013

#ifdef OUTPUT_VAR_CODES
#error reached
//varcodes file is temporary debug just so I can see the UED standard variable codes.
ofstream var_codes("UED_varcodes.txt");
char buf[20];
var_codes << CORN_uint32_to_str(UED_STD_VC_precipitation                      .code,buf,16) << " precipitation" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_max_temperature                    .code,buf,16) << " max_temperature" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_min_temperature                    .code,buf,16) << " min_temperature" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_avg_temperature                    .code,buf,16) << " avg_temperature" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_obs_temperature                    .code,buf,16) << " obs_temperature" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_max_dew_point_temperature          .code,buf,16) << " max_dew_point_temperature" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_min_dew_point_temperature          .code,buf,16) << " min_dew_point_temperature.code" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_max_relative_humidity              .code,buf,16) << " max_relative_humidity" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_min_relative_humidity              .code,buf,16) << " min_relative_humidity" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_solar_radiation                    .code,buf,16) << " solar_radiation" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_wind_speed                         .code,buf,16) << " wind_speed" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_pot_evapotranspiration             .code,buf,16) << " pot_evapotranspiration" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_act_evapotranspiration             .code,buf,16) << " act_evapotranspiration" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_pan_evaporation                    .code,buf,16) << " pan evaporation" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_ref_ET_penman_monteith             .code,buf,16) << " ref_ET_penman_monteith" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_ref_ET_priestly_taylor             .code,buf,16) << " ref_ET_priestly_taylor" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_ref_ET_hargraves                   .code,buf,16) << " ref_ET_hargraves" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_avg_daytime_vapor_pressure_deficit .code,buf,16) << " avg_daytime_vapor_pressure_deficit" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_avg_fullday_vapor_pressure_deficit .code,buf,16) << " avg_fullday_vapor_pressure_deficit" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_max_vapor_pressure_deficit         .code,buf,16) << " max_vapor_pressure_deficit" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_cloud_cover                        .code,buf,16) << " cloud_cover" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_snow_fall                          .code,buf,16) << " snow_fall" << endl;
var_codes << CORN_uint32_to_str(UED_STD_VC_snow_depth                         .code,buf,16) << " snow_depth" << endl;
#endif
    // the following date setting is primarily for CropSyst, we will use the simulation period
//NYI   {  const UED_period_record *simulation_period_rec = export_DB.get_period(UED_period_application_code_option,0,0,0,0,0);
//NYI         if (simulation_period_rec)
//NYI            export_DB.set_date_ranges(simulation_period_rec->start_date_time,simulation_period_rec->end_date_time);
//NYI   }
   return 1; // actually should return something useful.                         //180107
   //180108 return data_line_count;
}
//_tuplize_opened_UED_file__________________________________________2017-11-25_/
nat32 Tabular_file_with_TDF_convertor::extract_tuples
(Tuple_index &export_tuples)
{
   // now member Bidirectional_list sorted_tuples;
   if (arguments.is_verbose(3)) //progress? discovery?
   std::clog << "Extracting tuples" << std::endl;
   return export_tuples.extract_tuples(sorted_tuples);
}
//_extract_tuples___________________________________________________2017-11-25_/
nat32 Tabular_file_with_TDF_convertor::commit_export()
{  nat32 data_line_count =                                                       //150405
      (format.extension_year_digits)                                             //150405
      ? export_tuples_by_year                                                    //150405
         (sorted_tuples ,export_start_date_time.get_year()
         ,export_end_date_time.get_year())                                       //150405
      : export_tuples_for_period(sorted_tuples );                                         //150405
   return data_line_count;
}
//_commit_export____________________________________________________2017-11-25_/
nat32 Tabular_file_with_TDF_convertor::export_tuples_for_period
(CORN::Container &sorted_tuples )
{  if (arguments.is_verbose(2)) // progress?
   {  CORN::OS::File_name * foreign_filename                                     //200805
         = parameters.foreign_filename_discovery.provide_primary();
      /*200805
      if (!parameters.target_filename.empty())
         std::clog << "Generating:" << parameters.target_filename.c_str() << std::endl; //070518
      */
      if (!foreign_filename)                                                     //200805
         std::clog << "Generating:" << foreign_filename->c_str() << std::endl;   //200805
      else
         if (arguments.is_verbose(3)) // progress?
            std::clog << "Target standard output" << std::endl;
      if (arguments.is_verbose(4)) // progression?
      std::clog << "Exporting any preamble, header text fields, column headers" << std::endl;
   }
   if (!headers_written)
   {  nat32  header_line_count = export_header_lines();
      headers_written = true;                                                    //141013
   }
   if (arguments.is_verbose(4)) // progression
   {  std::clog << "Exporting tabular data " << std::endl;
   }
   nat32 data_line_count = export_data_lines(sorted_tuples);                     //141013
   if (arguments.is_verbose(4)) // progression
   {
   std::clog << "Data export complete           " << std::endl;
   std::clog << "Saving closing                 " << std::endl;
   }
   return data_line_count;
}
//_export_tuples_for_period_________________________________________2015-04-05_/
nat32 Tabular_file_with_TDF_convertor::export_tuples_by_year
(CORN::Container &sorted_tuples
,CORN::Year first_year
,CORN::Year last_year)
{  nat32 data_line_count = 0;
   CORN::Date_clad_32 last_date(export_end_date_time.get_date32());

   std::wstring foreign_name_unqual_undated(L"unnamed");                         //200805
   std::wstring foreign_ext(L"dat");                                             //200805
   CORN::OS::File_name *foreign_filename                                         //200805
      = parameters.foreign_filename_discovery.provide_primary();                 //200805
   CORN::OS::Directory_name_concrete *foreign_path =0;                           //200805
   if (foreign_filename)  // The user provided a foreign filename                //200805
   {   foreign_name_unqual_undated.clear();                                      //200805
       foreign_ext.clear();                                                      //200805
       foreign_name_unqual_undated = foreign_filename->get_name();               //200805
       foreign_ext                 = foreign_filename->get_extension();          //200805
       foreign_path = new CORN::OS::Directory_name_concrete                      //200805
         (foreign_filename->get_parent_directory_name_qualified());              //200805
   } else                                                                        //200805
      foreign_path = new CORN::OS::Directory_name_concrete; // CWD               //200805
   // (At this point the target discovered files list will be empty,
   // except perhaps for the primary which would be the foreign filename )
   parameters.foreign_filename_discovery.discovered.clear();                     //200805

   for (CORN::Year curr_year = first_year
       ;curr_year <= last_year
       ;curr_year++)
   {
      export_start_date_time.set_year(curr_year);
      if (curr_year > first_year)
         export_start_date_time.set_DOY(1);
      if (curr_year < last_year)
      {  export_end_date_time.set_year(curr_year);
         export_end_date_time.set_DOY(last_date.days_in_year(curr_year));
      } else
      export_end_date_time.set_date(last_date);
      Year suffix_year = curr_year;   // Currently there is no year separation
      if (format.extension_year_digits == 3) suffix_year = curr_year % 1000;
      if (format.extension_year_digits == 2) suffix_year = curr_year % 100;
      std::string suffix_year_str;
      append_int16_to_string
         (suffix_year,suffix_year_str,10
         ,format.extension_year_digits? format.extension_year_digits : 4,'0');

/*
I think I will create a new filename foreign (target) file discovered.

*/

      // std::wstring foreign_name_unqual_dated
      //   std::wstring foreign_name_unqual_undated;

      CORN::OS::File_name_concrete *foreign_filename_dated  //200805
         = new CORN::OS::File_name_concrete
            (*foreign_path
            ,foreign_name_unqual_undated
            ,CORN::OS::Extension(suffix_year_str));
      parameters.foreign_filename_discovery.primary = foreign_filename_dated;
      parameters.foreign_filename_discovery.discovered.append(foreign_filename_dated);
         /*
         I don't really need keep these filenames, but the list might be useful
         in invocation UI displaying what has been processed as progress display.
         Also, the file names will be deleted automatically when out of scope.
         */

      /* 200805 now using Path_discovery
      CORN::OS::File_name_concrete parameters_target_filename(parameters.target_filename);
      CORN::OS::File_name_concrete target_filename_year_ext
         (parameters_target_filename,CORN::OS::Extension(suffix_year_str));
      parameters.target_filename.clear();
      parameters.target_filename.set_DEN(target_filename_year_ext);
      */

      tabular_file = provide_tabular_file(suffix_year);
      if (!tabular_file) return 0; // Probably unable to determine the desired targe file type  //070307
      data_line_count +=
         export_tuples_for_period                                                //171125
         (sorted_tuples);
      delete tabular_file; tabular_file = 0; headers_written = false;
   }
   parameters.foreign_filename_discovery.primary = 0;                                       //200805
   parameters.foreign_filename_discovery.primary_owned = false;                             //200805
   delete foreign_path;                                                          //200805
   return data_line_count;
}
//_export_tuples_by_year____________________________________________2015-04-05_/
bool Tabular_file_with_TDF_convertor::set_imported_textual_metadata
(const ED_tabular_file_format::Metadata_field &metadata_field
,const std::string &value
)
{  bool used = true;
   switch (metadata_field.field_data_clad.get())                                 //190506
   {
         // NA case ED_tabular_file_format::Metadata_field::FD_static_text                 :
         // Static text from spreadsheets is ignored on inport (nothing to add to UED)
         case ED_tabular_file_format::Metadata_field::FD_UED_general_comment         :
         {   // NYI
            // NYI create a new UED comment record and append uniquely it to the UED file
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_database_description    :
         {  // NYI
            // NYI create a new UED description record and append uniquely it to the UED file
         } break;
         // case ED_tabular_file_format::Metadata_field::FD_UED_generating_application  :
         // NA although generation application would be text field and could be specified in a TDF,
         // it is unlikely that it would be relevent in any arbitrary Excel file.
         case ED_tabular_file_format::Metadata_field::FD_UED_location :
         {
            if (!header_identified_geolocation)
                header_identified_geolocation = new UED::Geolocation_record;
            switch (metadata_field.location_var_clad.get())                      //190506
            {
               case ED_tabular_file_format::Metadata_field::LOC_VAR_station_ID                :  { header_identified_geolocation->set_station_ID_code       (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_geocoordinate_ISO_6709_2008: { header_identified_geolocation->set_from_ISO_6709         (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_latitude_decimal_degrees  :  { header_identified_geolocation->set_latitude_dec_deg_cstr (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_latitude_deg_min_sec      :  { header_identified_geolocation->set_latitude_DMS_cstr/*191007 set_latitude_DMS_str */     (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_longitude_decimal_degrees :  { header_identified_geolocation->set_longitude_dec_deg_cstr(value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_longitude_deg_min_sec     :  { header_identified_geolocation->set_longitude_DMS_cstr/*191007 set_longitude_DMS_str  */   (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_meters          :  { header_identified_geolocation->set_elevation_meter_cstr  (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_feet            :  { header_identified_geolocation->set_elevation_feet_cstr   (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_country_name              :  { header_identified_geolocation->set_country_name          (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_state_name                :  { header_identified_geolocation->set_state_name            (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_county_name               :  { header_identified_geolocation->set_county_name           (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_description               :  { header_identified_geolocation->set_comment               (value.c_str()) ; } break;  // Currently the geolocation only has comment
               case ED_tabular_file_format::Metadata_field::LOC_VAR_station_name              :  { header_identified_geolocation->set_station_name          (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_comment                   :  { header_identified_geolocation->set_comment               (value.c_str()) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_station_number            :  { header_identified_geolocation->set_station_number        (atof(value.c_str())) ; } break;
               // NYI set_screening_height_cstr
            }
         } break;
         //case ED_tabular_file_format::Metadata_field::FD_UED_scalar_data :
         // NA scalar data is always numeric and would not be in text cell

         case ED_tabular_file_format::Metadata_field::FD_date_time_stamp :
         {
            switch (metadata_field.units.get()) // UED_scalar_time_stamp_units_enum.get())
            {
               // NYI case UT_date  string formatted date in some conventional format
               // I will probably need to specify a date format string in the scalar field
               // N/A case UT_year  Years should always be numeric
               // NYI case UT_month for dates that are entered as text or abbreviations but may need to get language
               // N/A case UT_day_of_month should always be numeric
               // N/A case UT_day_of_year should alwways be numerict
               // NYI case UT_date_time
               // the string from the cell completely represents both date and time (Not sure if Excel actually represents this)
               // I will probably need to specify a time format string in the scalar field
               // NYI case UT_time
               // I will probably need to specify a time format string in the scalar field
               // or probably could pars
               //     UT_second
               //     UM_minute
               //     UT_hour
            }
            // NYI
         } break;
         default : used = false; break;
   }
   return used;
}
//_set_imported_textual_metadata____________________________________2010-02-08_/
bool Tabular_file_with_TDF_convertor::set_imported_float_metadata
(const ED_tabular_file_format::Metadata_field &metadata_field
,float64 value
,bool dates_based_1904
)
{  bool used = true;
   switch (metadata_field.field_data_clad.get())                                 //190506
      {
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_static_text                 :
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_UED_general_comment         :
         // N/A Only taking numerics here case ED_tabular_file_format::Metadata_field::FD_UED_database_description    :
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_UED_generating_application  :
         case ED_tabular_file_format::Metadata_field::FD_UED_location :
         {
            if (!header_identified_geolocation)
                header_identified_geolocation = new UED::Geolocation_record;
            switch (metadata_field.location_var_clad.get())                      //190506
            {
               case ED_tabular_file_format::Metadata_field::LOC_VAR_latitude_decimal_degrees  :  { header_identified_geolocation->set_latitude_dec_deg_32   (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_longitude_decimal_degrees :  { header_identified_geolocation->set_longitude_dec_deg_32  (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_meters          :  { header_identified_geolocation->set_elevation_meter       (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_feet            :  { header_identified_geolocation->set_elevation_feet        (value) ; } break;
               // NYI set_screening_height_cstr
            }
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_scalar_data :
         {
            /* NYI   need to render a UED scalar record using
            metadata_field->UED_scalar_record_code
            metadata_field->UED_scalar_variable_code
            metadata_field->UED_scalar_units_code_enum
            metadata_field->UED_scalar_time_stamp_units_enum   //  when field_data_labeled = FD_date_time_stamp this is the units of the time stamp
            */
         } break;
         case ED_tabular_file_format::Metadata_field::FD_date_time_stamp :
         {
            switch (metadata_field.units.get())
            {
               case UT_date :
               {  nat32 days_since_epoch_inclusive = value;
                  nat32 days_since_epoch_exclusive
                     = days_since_epoch_inclusive -
                        ((dates_based_1904) ? 1 : 2);   // minus 2 is to fix the stupid 1900 leap year error in Lotus 1-2-3 and Excel
                  CORN::Date_clad_32 cell_date = (dates_based_1904) ? 1904001 : 1900001;
                  cell_date.inc_days(days_since_epoch_exclusive);

                  //NYI  continue here need to set the current sheet import date.

               } break;
            }
            // NYI
         } break;
         default : used = false; break;
      } // switch
   return used;
}
//_set_imported_float_metadata______________________________________2010-02-08_/
bool Tabular_file_with_TDF_convertor::set_imported_integer_metadata
(const ED_tabular_file_format::Metadata_field &metadata_field
,int32 value)
{  bool used = true;
   switch (metadata_field.field_data_clad.get())                                 //190506
   {
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_static_text                 :
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_UED_general_comment         :
         // N/A Only taking numerics here case ED_tabular_file_format::Metadata_field::FD_UED_database_description    :
         // N/A Only taking numerics here  case ED_tabular_file_format::Metadata_field::FD_UED_generating_application  :
         case ED_tabular_file_format::Metadata_field::FD_UED_location :
         {
            if (!header_identified_geolocation)
                header_identified_geolocation = new UED::Geolocation_record;
            switch (metadata_field.location_var_clad.get())                      //190506
            {
               case ED_tabular_file_format::Metadata_field::LOC_VAR_station_ID                :  { header_identified_geolocation->set_station_number        (value) ; } break; // Even though the ID is different from station number, if it is numeric it could only be a station number and was probably specified incorrectly in the TDF file.
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_meters          :  { header_identified_geolocation->set_elevation_meter       (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_elevation_feet            :  { header_identified_geolocation->set_elevation_feet        (value) ; } break;
               case ED_tabular_file_format::Metadata_field::LOC_VAR_station_number            :  { header_identified_geolocation->set_station_number        (value) ; } break;
               // NYI set_screening_height_cstr
            }
         } break;
         case ED_tabular_file_format::Metadata_field::FD_UED_scalar_data :
         {
            /* NYI   need to render a UED scalar record using
            metadata_field->UED_scalar_record_code
            metadata_field->UED_scalar_variable_code
            metadata_field->UED_scalar_units_code_enum
            metadata_field->UED_scalar_time_stamp_units_enum   //  when field_data_labeled = FD_date_time_stamp this is the units of the time stamp
            */
         } break;
         case ED_tabular_file_format::Metadata_field::FD_date_time_stamp :
         {
            switch (metadata_field.units.get())
            {
               case UT_date :
               {  // NYI file_date.set_date32(value);
               } break;
               case UT_year            : current_sheet_year = value; break; // years can be negative
               case UT_month           : current_sheet_month = value; break;
               case UT_day_of_month    : current_sheet_day_of_month = value; break;
               case UT_day_of_year     : current_sheet_day_of_year = value; break;

               // NYI case UT_date_time  I dont think Excel store date and time in a single cell as a numeric

               // NYI case UT_time  I am not sure how Excel represents time (I presume seconds since midnight)

               // NYI   UT_second   these would probably never occur, but I should implement them for completion
               // NYI   UM_minute
               // NYI   UT_hour
            }
            // NYI
         } break;
         default : used = false; break;
   } // switch
   return used;
}
//_set_imported_integer_metadata____________________________________2010-02-08_/
std::string &Tabular_file_with_TDF_convertor::get_units_description
(UED_units_code units_code,bool abbreviated)
{  return UED_file->get_units_description(units_code ,abbreviated);
}
//_get_units_description____________________________________________2015-09-19_/

