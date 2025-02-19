
#include "LARS_WG_convertor.h"
#include <fstream>

#include "UED/convert/text_dBase_Excel_with_TDF/export.h"
#include "corn/data_source/vv_file.h"
#include "corn/container/SDF_list.h"
#include "UED/convert/station_database.h"
#include "UED/library/std_codes.h"
#include "UED/library/timeqry.h"
using namespace std;

#include "corn/OS/file_system_engine.h"
//______________________________________________________________________________
LARS_WG_format_convertor::LARS_WG_format_convertor(int argc, const char *argv[])
:Text_dBase_Excel_with_TDF_convertor(argc,argv)
,element_column_count(0)
{  CORN::OS::File_name_concrete program_filename(argv[0]);
   CORN::OS::Directory_name_concrete convert_dir(*program_filename.get_path_directory_name());
   CORN::OS::Directory_name_concrete convert_formats_dir(convert_dir,"formats");

   CORN::OS::File_name_concrete default_LARS_WG_filename_TDF(convert_formats_dir,"LARS-WG (YEAR JDAY MIN MAX RAIN RAD).tdf");
   if (!CORN::OS::file_system_engine.exists(parameters.format_filename)) // Eventually the UED export wizard may allow the user to specify different LARS-WG format options with different TDF file, but currently there is only one which we will use as the default
      parameters.format_filename.set_DEN(default_LARS_WG_filename_TDF);
}
//______________________________________________________________________________
bool LARS_WG_format_convertor::import_ST_file()
{
   std::ifstream st_file(st_filename.c_str());
   std::string in_line;
   geoloc_record_import = new UED::Geolocation_record();
   while (!st_file.eof())
   {
      getline(st_file,in_line);                                                  //140918
      if (in_line == "[SITE]")
      {
         getline(st_file,in_line);
         geoloc_record_import->set_station_name(in_line.c_str());

      }
      if (in_line == "[LAT, LON and ALT]")
      {  float32 lat; float32 lon; float32 alt;
         st_file >> lat >> lon >> alt;
         geoloc_record_import->set_latitude_dec_deg_32  (lat);
         geoloc_record_import->set_longitude_dec_deg_32 (lon);
         geoloc_record_import->set_elevation_meter     (alt);
      }
      if (in_line == "[WEATHER FILES]")
      {  getline(st_file,in_line);                                               //140918
         // (WARNING there could possibly more files listed here, I have not see this case.
         sr_filename.set_string(in_line);                                        //140918
      }
      if (in_line == "[FORMAT]")
      {  getline(st_file,in_line);                                               //140918
         //in_line.read_line(st_file);
         CORN::SDF_List variable_codes_list(in_line.c_str());
         element_column_count = variable_codes_list.count();
         nat8 VCi = 0;
         FOR_EACH_IN(var_code,SDF_List::Item,variable_codes_list,each_var_code)
         {
            /*180820 
            if (strcmp(var_code->get_key(),"YEAR")==0) { variable_codes[VCi] = UED::STD_VC_date.get();            units_codes[VCi] = UT_year; }
            if (strcmp(var_code->get_key(),"JDAY")==0) { variable_codes[VCi] = UED::STD_VC_date.get();            units_codes[VCi] = UT_day_of_year; }
            if (strcmp(var_code->get_key(),"MIN") ==0) { variable_codes[VCi] = UED::STD_VC_min_temperature.get(); units_codes[VCi] = UC_Celcius; }
            if (strcmp(var_code->get_key(),"MAX") ==0) { variable_codes[VCi] = UED::STD_VC_max_temperature.get(); units_codes[VCi] = UC_Celcius; }
            if (strcmp(var_code->get_key(),"RAIN")==0) { variable_codes[VCi] = UED::STD_VC_precipitation.get();   units_codes[VCi] = UC_mm; }
            if (strcmp(var_code->get_key(),"RAD") ==0) { variable_codes[VCi] =
            */
            if (var_code->is_key_cstr("YEAR") { variable_codes[VCi] = UED::STD_VC_date.get();            units_codes[VCi] = UT_year; }
            if (var_code->is_key_cstr("JDAY") { variable_codes[VCi] = UED::STD_VC_date.get();            units_codes[VCi] = UT_day_of_year; }
            if (var_code->is_key_cstr("MIN")  { variable_codes[VCi] = UED::STD_VC_min_temperature.get(); units_codes[VCi] = UC_Celcius; }
            if (var_code->is_key_cstr("MAX")  { variable_codes[VCi] = UED::STD_VC_max_temperature.get(); units_codes[VCi] = UC_Celcius; }
            if (var_code->is_key_cstr("RAIN") { variable_codes[VCi] = UED::STD_VC_precipitation.get();   units_codes[VCi] = UC_mm; }
            if (var_code->is_key_cstr("RAD")  { variable_codes[VCi] =
            
               #if ((CS_VERSION >0) && (CS_VERSION <= 4))
               UED::STD_VC_solar_radiation.get();
               #else
               UED::STD_VC_solar_radiation_global.get();
               #endif
               units_codes[VCi] = UC_MJ_per_m2_solar_radiation; }
            VCi ++;
         } FOR_EACH_END(each_var_code);
      }
      // if (in_line == "[END]") ignored, just read the rest of the file
   }
   return true;
}
//______________________________________________________________________________

bool LARS_WG_format_convertor::import_SR_file()
{
   std::ifstream sr_file(sr_filename.c_str());
   std::string in_line;
   parameters.UED_filename;
   bool added_now = false;
   Station_database * station_database = provide_station_database_from_UED_filename(parameters.UED_filename,added_now);
   UED::Database_file_indexed &ued_file = station_database->ued_file;
   ued_file.take_geolocation_record(geoloc_record_import);
   float32 value; // 32bit because UED is 32bit
   while (!sr_file.eof())
   {  //st_file >> in_line;
      // Warning presumming the date values are the first columns and will be setup before the values are read.
      Year record_year  = 0;
      DOY  record_DOY   = 0;
      for (int VCi = 0; VCi < element_column_count ; VCi++)
      {
         sr_file >> value;
         if       (units_codes[VCi] == UT_year)        record_year = (CORN::Year)value;
         else if  (units_codes[VCi] == UT_day_of_year) record_DOY = (CORN::DOY)  value;
         else
         {
             const Data_record_creation_layout &creation_layout =
                 (variable_codes[VCi] == UED::STD_VC_min_temperature.get())? min_temperature_creation
                :(variable_codes[VCi] == UED::STD_VC_max_temperature.get())? max_temperature_creation
                :(variable_codes[VCi] == UED::STD_VC_precipitation.get())  ? precipitation_creation
                :(variable_codes[VCi] ==
               #if ((CS_VERSION >0) && (CS_VERSION <= 4))
               UED::STD_VC_solar_radiation.get()
               #else
               UED::STD_VC_solar_radiation_global.get()
               #endif

                     )? solar_radiation_creation
                : precipitation_creation; // Should not occur, but default to precipitation
            CORN::Quality_clad value_quality(measured_quality);              //141025
            if (CORN::is_approximately<float32> (value,-99.0,0.00001))
               value_quality.assume_code(missing_quality);
            if ((variable_codes[VCi] ==
               #if ((CS_VERSION >0) && (CS_VERSION <= 4))
               UED::STD_VC_solar_radiation.get()
               #else
               UED::STD_VC_solar_radiation_global.get()
               #endif
                  )
                && CORN::is_approximately<float32> (value,0.0,0.000001))
            {   value = 0.01; // We had one location with very low solar radiation above the artic circle
                              // This should be considered valid, but current ClimGen considers this invalid.
                              // Here I provided a value for ClimGen to work with.
                value_quality.assume_code(other_real_quality);
            }
            bool override_safety = false;
            bool record_created = false;
            CORN::Date record_date((CORN::Year)(record_year + parameters.offset_years),(CORN::DOY)record_DOY);
            if (record_date.get() != 0)  // May be 0 for blank records or last line of file.
            {
             UED::Time_query_single_date  time_query
               (record_date.get_datetime64()
               ,UT_day // Normally UT_day for daily timestep
               ,Time_query::at);
             ued_file.set_value
               (value
               ,time_query
               ,units_codes[VCi]
               ,value_quality
               ,creation_layout
               ,record_created
               ,override_safety );
            }
         } // variable code
      }
   }
   return true;
}
//_2008-03-03___________________________________________________________________
nat32 LARS_WG_format_convertor::perform_import()
{
   st_filename.set_DEN(CORN::OS::File_name_concrete(parameters.target_filename,CORN::OS::Extension("st")));  // the target filename could be either the .st or the .sr filename
   for (int i = 0; i < 10; i++)
   {  variable_codes[i] = 0;
      units_codes[i] = 0;
   }
   sr_filename.set_DEN(CORN::OS::File_name_concrete(parameters.target_filename,CORN::OS::Extension("sr")));  // the target filename could be either the .st or the .sr filename
                                               //080317
   if (!import_ST_file())
   {  std::cerr << "Unable to process st file:" << st_filename << endl;
      return 0;
   }
   // The SR file is specified in the ST file however,
   // Could be unqualified or may have been moved.
   if (!CORN::OS::file_system_engine.exists(sr_filename))  // If the specified filename doesn't exist try the same ST file with the SR extension
   {
      std::clog << "Unable to find the sr file specified in the st file:" << sr_filename << endl;
      sr_filename.set_DEN(CORN::OS::File_name_concrete(st_filename,CORN::OS::Extension("sr")));
      std::clog << "Trying :" << sr_filename << endl;
      if (!CORN::OS::file_system_engine.exists(sr_filename))
      {
         std::clog << "Unable to find the file:" << sr_filename << endl;
      }
   }
   import_SR_file();
   return 0; // NYI
}
//_2008-03-03___________________________________________________________________
bool LARS_WG_format_convertor::export_ST_file(const char *filename_st,const char *filename_sr)
{
   const UED::Geolocation_record * geoloc_record = ued_file_to_export->ref_geolocation_record(); //050429
   if (!geoloc_record) return false;
   ofstream file_st(filename_st);
   file_st
      << "[SITE]" << endl
      << geoloc_record->get_station_name() << endl
      << "[LAT, LON and ALT]" << endl
      <<    geoloc_record->get_latitude_dec_deg() << ' '
         << geoloc_record->get_longitude_dec_deg() << ' '
         << geoloc_record->get_elevation() << endl
      << "[WEATHER FILES]" << endl
      <<  filename_sr << endl
      << "[FORMAT]" <<endl
      << "YEAR JDAY MIN MAX RAIN RAD" << endl
         // Could also output MONTH and DAY  (of month) but not necessary with JDAY.
         // Could also output sunshine hours (SUN), but not currently outputs by ClimGen
      << "[END]" << endl;
   return true;
}
//______________________________________________________________________________
nat32 LARS_WG_format_convertor::perform_export()
{  nat32 records_written = 0;
   CORN::OS::File_name_concrete filename_st(parameters.UED_filename,CORN::OS::Extension("st"));
   records_written = Tabular_file_with_TDF_convertor::perform_export();
   std::clog << "Writing LARS-WG site file (st):" << filename_st << endl;
   if (!export_ST_file(filename_st.c_str(),parameters.target_filename.c_str()))
   {  std::cerr << "Unable to generate ST file.\nPerhaps geolocation data was not stored in the UED file" << endl;
      press_return_before_exit = true;
   }
   std::clog << "Data records written:" << records_written << endl;
   if (records_written)
         std::clog << "Export complete." << endl;
   else  std::cerr << "Export unsuccessful: No data records written." << endl;
   return records_written;
}
//______________________________________________________________________________


/*
Any
explanatory notes to the *.st file,
you can use �//� at the beginning of lines containing comments � the �//�
will result in these lines being ignored by the model.
The layout of the *.st file is as follows:
� [SITE] � the station name identifier, e.g., Debrecen
� [LAT, LON and ALT] � latitude, longitude and altitude for the site
� [WEATHER FILES] � the directory path location and name of the file containing the observed
weather data for the site
� [FORMAT] � the format of the observed weather data in the file. Here the DEBR6090.sr file
contains information for the year (YEAR), Julian day (JDAY, i.e., from 1 to 365 or 366), minimum
temperature (MIN; �C), maximum temperature (MAX; �C), precipitation (RAIN; mm) and solar
radiation (RAD; MJm-2day-1). Other tags which can be used in the format line are: DAY (day of
month), MONTH (month identifier from 1 [January] to 12 [December]) and SUN (sunshine hours).
If solar radiation is not available for a particular site, then sunshine hours may be used instead � the
weather generator automatically converts sunshine hours to solar radiation using an algorithm based
on that described in Rietveld (1978). LARS-WG will work with precipitation data alone, or with
precipitation plus any combination of the other climate variables listed above.
� [END] � denoting the end of the file


Observed weather data file.

� The observed weather data can be contained in more than one file. Simply list the directory path
and file names in chronological order under the [WEATHER FILES] tag in the *.st file.
� Each line in the data record represents a single day and the values must be in the same order for
each day according to the tags in the [FORMAT] statement in the *.st file.
� The days must be in chronological order starting with the earliest record.
� The values for each day should be separated by spaces or tabs. The data should not contain blank
lines, comment lines or headers.
*/


