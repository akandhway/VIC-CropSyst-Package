#ifndef tabular_file_with_TDF_convertorH
#define tabular_file_with_TDF_convertorH
#include "UED/convert/convertor.h"
#include "UED/convert/ED_tabular_format.h"
#include "corn/tabular/delineated_datarec.h"
#include "corn/tabular/file.h"
#include "tuple_index.h"
#ifndef dynamic_array_TH
#  include "corn/dynamic_array/dynamic_array_T.h"
#endif
#define IMPLEMENTING_EXPORT
//______________________________________________________________________________
namespace UED
{
   class Database_file_indexed;
   class Export_column; // forward declaration
}
//______________________________________________________________________________
class Tabular_file_with_TDF_convertor
: public UED::Convertor // I need to have Tabular_file_with_TDF_convertor_weather
{
 public: // The following are common to import and export
   //Tabular_format_text_file format_text;     // the TDF format
   ED_tabular_file_format   format;     // the TDF format
 protected:
   Units_code time_step_stamp_units_code;                                        //050413
   contribute_ bool headers_written;                                             //141013
 private: // The following are used for import only
   mutable Unidirectional_list import_columns;     //050404

      // There is going to be a lot of searching on export columns,
      // this should be a fast searchable list
      // List of UED_import_columns

   Years                   time_step_years;
   Days                    time_step_days;
   Minutes                 time_step_minutes;
   Seconds                 time_step_seconds;
//NYI Milliseconds                 time_step_milliseconds;      //  I dont have this type yet
   public:
   UED::Geolocation_record *header_identified_geolocation;
 public: // The following are for export only
   Unidirectional_list export_columns;                                           //070203

      // There is going to be a lot of searching on export columns,
      // this should be a fast searchable list

   // UED_file is already available in Convertor
   // So this pointer is just a reference and it can be rename
   // where used once verified to work

   CORN::Date_time_clad_64 export_start_date_time;                               //170531_070203
   CORN::Date_time_clad_64 export_end_date_time;                                 //170531_070203
   CORN::Tabular_file  *tabular_file;                                            //070203
   CORN::Dynamic_array<nat32> column_widths;                                     //050411
   CORN::Dynamic_array<nat32> column_start;                                      //100104
   bool importing_metadata; // false when reading detail lines Use when reading spreadsheets.
 public: // Special to provide RHavg
   //This is temporary because ClimGen currently doesn't record Tavg
   // After adding Tavg output to ClimGen this can be removed.                   //140610
   CORN::Dynamic_array<float32> RHmax;
   CORN::Dynamic_array<float32> RHmin;
 private:
   Bidirectional_list sorted_tuples;                                             //171125
 public:
   Tabular_file_with_TDF_convertor
      (UED::Convertor_arguments &arguments_);                                    //170328
   virtual ~Tabular_file_with_TDF_convertor();                                   //070306
 public:
   virtual nat32  perform_import();
   virtual nat32  perform_export();
 protected:
   Record_code initialize_common();
   bool initialize_import();
 public:
   bool initialize_export();
   nat32 tuplize_opened_UED_file                                                 //141014
      (Tuple_index &export_tuples);                                              //180107
 protected:
   nat32 tuplize_UED_file
      (const CORN::OS::File_name &UED_filename                                   //171125
      ,Tuple_index &export_tuples);                                              //180107
   nat32 export_tuples_for_period(CORN::Container &sorted_tuples);            //150405
   nat32 export_tuples_by_year                                                   //150405
      (CORN::Container &sorted_tuples
      ,CORN::Year first_year
      ,CORN::Year last_year);
 public: //191009 protected:
   nat32 extract_tuples(Tuple_index &export_tuples);                             //171125
         // not sure if const
   nat32 commit_export();                                                        //171125
 private:
   nat32  import_header_lines(CORN::Tabular_file &tabular_file);
   // Returns the number of header lines encountered
   nat32  import_metadata_fields(CORN::Tabular_file &tabular_file);
   nat32  import_column_headers(CORN::Tabular_file &tabular_file);

   bool get_date_time_from_data_record
      (CORN::Delineated_data_record &data_rec
      ,CORN::Date_time &row_date_time);
   nat32  import_data_lines
      (CORN::Tabular_file &tabular_file
      ,Year               file_year       // For some file types (CropSyst text files), the year is part of the file name
      ); // Returns the number of data lines encountered
 protected:
   bool import_data_line(Delineated_data_record &data_rec,Year file_year );      //100106
 protected:
   provided_ Delineated_data_record *detail_line_data_record;                    //100116
 protected :
   virtual CORN::Tabular_file *provide_tabular_file
      (Year &file_year) = 0;
      ///< \param file_year is only used by formats that split files into years.
   Delineated_data_record &provide_detail_line_data_record()provision_;          //100116
 protected:
   virtual nat32  export_metadata_fields();
   virtual nat32  export_column_headers();                                       //070305
 private:
   void tuplize_database
      (modifiable_ Tuple_index &export_tuples);                                  //141013
   nat32  export_header_lines();
   nat32  export_data_lines
      (CORN::Container &sorted_tuples);                                          //150405
      ///< \returns the number of column header lines written;
 protected:
   virtual bool transfer_tuple_column
      ( CORN::Date_time &tuple_date_time
      , float32 value
      , nat16 export_column_number
      , UED::Export_column  *export_column);
 private:
   void output_column_header_row(int CH_row_number, int col_number,int rows
      , const char *description_SDF,ED_tabular_file_format::Column &column);     //040928
   bool   set_metadata_field_text                                                //100208
     (std::string &line
      ,size_t pos
      ,const std::string &text);
 protected :
   bool set_imported_textual_metadata                                            //100208
      (const ED_tabular_file_format::Metadata_field &metadata_field
      ,const std::string &value);
   bool set_imported_float_metadata                                              //100208
      (const ED_tabular_file_format::Metadata_field &metadata_field
      ,float64 value
      ,bool dates_based_1904);
   bool set_imported_integer_metadata                                            //100208
      (const ED_tabular_file_format::Metadata_field &metadata_field
      ,int32 value);
   virtual Geolocation *render_geolocation
      (const std::string &station_ID)                                rendition_; //090922
 protected :    // previously I only had the current date elements for Excel files but they are generally applicable so current_XXXX should be renamed accoringly I.e. sheet_or_file_XXXX
   CORN::Year  current_sheet_year;                                               //100113
   CORN::Month current_sheet_month;                                              //100113
   CORN::DOM   current_sheet_day_of_month;                                       //100113
   CORN::DOY   current_sheet_day_of_year;                                        //100113
   nat32       current_sheet_row; // 1-based
 protected: // tabular_file_with_TDF_write implementations
   virtual std::string &get_units_description(UED_units_code units_code,bool abbreviated);
};
//______________________________________________________________________________
#endif

