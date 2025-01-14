#include "UED/convert/IBSNAT_ICASA_DSSAT/DSSAT_ICASA_Convertor.h"
#include "UED/convert/station_database.h"
#include "UED/library/locrec.h"
#include "UED/library/timeqry.h"
#include "UED/library/database_file.h"
#include "UED/library/std_codes.h"
#include "common/weather/weather_provider.h"
#include "weather/parameter/WC_air_temperature.h"
#include "corn/container/text_list.h"
#include "corn/string/strconv.hpp"

#include "corn/math/compare.hpp"
#include <iomanip.h>
//______________________________________________________________________________
uint32 DSSAT_ICASA_Convertor::perform_import()
{  uint32 records_imported = 0;
   ICASA::File  ICASA_file(parameters.target_filename,"WEATHER");
   ICASA_file.read();
   for (Weather_unit *curr_weather_unit = ICASA_file.pop_weather_unit(); curr_weather_unit;curr_weather_unit = ICASA_file.pop_weather_unit())
   {
      records_imported += import_weather_unit(*curr_weather_unit);
      delete curr_weather_unit;
   }
   return records_imported;
}
//______________________________________________________________________________
uint32 DSSAT_ICASA_Convertor::import_weather_unit(ICASA::Weather_unit &weather_unit)
{
   nat32 records_imported = 0;
   // NYI Pop off miscellaneous groups (Ie. General) to get description information
   // This can be added to each database.
   for (ICASA::Weather_unit::Group *popped_weather_group = weather_unit.pop_group()
      ;popped_weather_group
      ;popped_weather_group = weather_unit.pop_group())
   {
      bool added_just_now = false;
      std::string station_ID (popped_weather_group->identifier);                 //110805
      /* The weather group doesn't necessarily have a stationID
         In this case don't use station databases
      //180309
      if (!station_ID.length())                                                  //110805
      {  // ICASA file may have multiple station weather data set in
         // a single file, in this case an ID will/should be provided for each
         // If not specified, assuming a single station in a single file
         // with the name of the file identifing the site.
         CORN::wstring_to_string(parameters.target_filename.get_name(),station_ID);
      }
      */
      Station_database *station_database = provide_station_database(station_ID,added_just_now);
      Weather_validator *weather_data_validator = 0;
      if (station_database) // should always be provided
      {  UED_file = &(station_database->ued_file);                               //140527
         weather_data_validator = station_database->provide_weather_data_validator(); //180308
      } else                                                                     //180309
      {  UED_file = new UED::Database_file_indexed(arguments.UED_filename.c_str(),std::ios::app);
         weather_data_validator = new Weather_validator;
      }
      if (UED_file)                                                              //180309
      {
         UED::Geolocation_record  *geolocation_rec = 0;
         {  const ICASA::Weather_unit::Group::Geolocation_info &geolocation_info       =popped_weather_group->get_geolocation_info();
            geolocation_rec = new UED::Geolocation_record();
            geolocation_rec->set_latitude_dec_deg_32  (geolocation_info.WTHLAT);
            geolocation_rec->set_longitude_dec_deg_32 (geolocation_info.WTHLONG);
            geolocation_rec->set_elevation_meter      (geolocation_info.WELEV);
            geolocation_rec->set_station_ID_code      (geolocation_info.INSI.c_str());
            geolocation_rec->set_screening_height
               (CORN_max(CORN_max(geolocation_info.WNDHT,geolocation_info.TEMHT)
               ,geolocation_info.REFHT));
            if (geolocation_rec->get_screening_height() <= 0) // missing
               geolocation_rec->set_screening_height(2.0);
//NYI                   TAV;      // Temperature average for whole year (�C)  (RLN: I think average annual temperature of all years?)
//NYI                   TAMP;     // Temperature amplitude,monthly averages (�C)  (RLN: I think annual average of the monthly averages
            UED_file->take_geolocation_record(geolocation_rec);
         }
         //180309  Weather_validator *weather_data_validator = station_database->provide_weather_data_validator();
      // For import we ignore the date range, but we can export it
         for (ICASA::Weather_unit::Group::Daily_weather *daily_weather_rec = popped_weather_group->pop_daily_weather_record()
            ;daily_weather_rec
            ;daily_weather_rec = popped_weather_group->pop_daily_weather_record())
         {
            daily_weather_rec->set_century_for_old_formats
               (parameters.offset_years);                                        //100303
            CORN::Quality_clad SRAD_smart_quality(measured_quality);   Units_code solar_radiation_units = UC_MJ_per_m2_solar_radiation;   // MJ/m2/day  (day is implicit in UED records with daily timestep)
            CORN::Quality_clad TMAX_smart_quality(measured_quality);   Units_code temperature_min_units = UC_Celcius;
            CORN::Quality_clad TMIN_smart_quality(measured_quality);   Units_code temperature_max_units = UC_Celcius;
            CORN::Quality_clad RAIN_smart_quality(measured_quality);   Units_code precipitation_units   = UC_mm;
            CORN::Quality_clad WIND_smart_quality(measured_quality);   Units_code windspeed_units       = UC_meters_per_second;  // km/day   (day is implicit in UED records with daily timestep)  //130322 was M
            CORN::Quality_clad TDEW_smart_quality(measured_quality);   Units_code mean_dew_point_units  = UC_Celcius;
            CORN::Quality_clad RHUM_smart_quality(measured_quality);   Units_code rel_humid_units       = UC_percent;
            CORN::Quality_clad VPRS_smart_quality(measured_quality);   Units_code VPRS_units            = UC_hPa;   // hectoPascals
            if (weather_data_validator)
            {  std::string quality_description;                                //110805
               if (SRAD_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->SRAD ,solar_radiation_creation           .variable_code,solar_radiation_units,UT_day,CORN::measured_quality,CORN::measured_quality,SRAD_smart_quality,quality_description);
               if (TMAX_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->TMAX ,max_temperature_creation           .variable_code,temperature_max_units,UT_day,CORN::measured_quality,CORN::measured_quality,TMAX_smart_quality,quality_description);
               if (TMIN_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->TMIN ,min_temperature_creation           .variable_code,temperature_min_units,UT_day,CORN::measured_quality,CORN::measured_quality,TMIN_smart_quality,quality_description);
               if (RAIN_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->RAIN ,precipitation_creation             .variable_code,precipitation_units  ,UT_day,CORN::measured_quality,CORN::measured_quality,RAIN_smart_quality,quality_description);
               if (WIND_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->WIND ,wind_speed_creation                .variable_code,windspeed_units      ,UT_day,CORN::measured_quality,CORN::measured_quality,WIND_smart_quality,quality_description);
               if (TDEW_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->TDEW ,avg_dew_point_temperature_creation .variable_code,mean_dew_point_units ,UT_day,CORN::measured_quality,CORN::measured_quality,TDEW_smart_quality,quality_description);
               if (RHUM_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->RHUM ,avg_relative_humidity_creation     .variable_code,rel_humid_units      ,UT_day,CORN::measured_quality,CORN::measured_quality,RHUM_smart_quality,quality_description);
               if (VPRS_smart_quality.is_valid()) weather_data_validator->validate_float32(daily_weather_rec->date.get_DOY(),daily_weather_rec->VPRS ,avg_vapor_pressure_creation        .variable_code,VPRS_units           ,UT_day,CORN::measured_quality,CORN::measured_quality,VPRS_smart_quality,quality_description);
            }
            UED::Time_query set_query(daily_weather_rec->date.get_datetime64(),daily_weather_rec->date.get_datetime64(),daily_weather_rec->date.get_datetime64(),UED::Time_query::at);
            bool created;  // to catch the result of set_values

            if (CORN::is_approximately<float32>
                (daily_weather_rec->TMAX,daily_weather_rec->TMIN,0.0001))        //150518
            {  daily_weather_rec->TMAX += 0.1;
               daily_weather_rec->TMIN -= 0.1;
            }

            UED_file->set_value(daily_weather_rec->SRAD  ,set_query,solar_radiation_units ,SRAD_smart_quality  ,solar_radiation_creation          ,created);
            UED_file->set_value(daily_weather_rec->TMAX  ,set_query,temperature_max_units ,TMAX_smart_quality  ,max_temperature_creation          ,created);
            UED_file->set_value(daily_weather_rec->TMIN  ,set_query,temperature_min_units ,TMIN_smart_quality  ,min_temperature_creation          ,created);
            UED_file->set_value(daily_weather_rec->RAIN  ,set_query,precipitation_units   ,RAIN_smart_quality  ,precipitation_creation            ,created);
            UED_file->set_value(daily_weather_rec->WIND  ,set_query,windspeed_units       ,WIND_smart_quality  ,wind_speed_creation               ,created);
            UED_file->set_value(daily_weather_rec->TDEW  ,set_query,mean_dew_point_units  ,TDEW_smart_quality  ,avg_dew_point_temperature_creation,created);
            UED_file->set_value(daily_weather_rec->RHUM  ,set_query,rel_humid_units  ,RHUM_smart_quality       ,avg_relative_humidity_creation    ,created);
            UED_file->set_value(daily_weather_rec->VPRS  ,set_query,VPRS_units  ,VPRS_smart_quality            ,avg_vapor_pressure_creation       ,created);

            records_imported+=1;
         }
      }
   }
   return records_imported;
}
//______________________________________________________________________________
nat32 DSSAT_ICASA_Convertor::perform_export()
{  nat32 records_exported = 0;
/*  ICASA 1.x format (http://dssat.net/wp-content/uploads/2012/02/icasa_lah8_Jww11_gh6.pdf)
$WEATHER:KSAS
*GENERAL
@ PEOPLE
 Wagger,M.G. Kissel,D.
@ INSTITUTES
 Kansas State Univ.
@ CONTACTS
 Hunt,L.A. thunt@uoguelph.ca
@ NOTES
 Wind and dewpoint included; wind in m/s as average over 24h
 Data from DSSAT 3.0 file
@ DISTRIBUTION
 Use at will but acknowledge source. No secondary distribution
@ VERSION
 ICASA1.0 10-08-2006 (GH,JW,LAH;email)
! And additional data items can be added for comprehensive documentation:
@ METHODS
 Standard Met Station instruments
@ PUBLICATIONS
 None of direct application
@ FLAG FLAG_DETAILS
 0 All data ok
 1 SRAD estimated from sun hours

!And data subsets similar to the following are necessary for all stations.

*WEATHER_STATION:KSAS2004
@ NAME
 Example weather dataset
@ COUNTRY REGION LOCATION
 USA Kansas Ashland
@ LAT LONG ELEV TAV TAMP TEMHT WNDHT CO2 CO2A
 37.1137 -90.4567 81 8.5 18.9 2.0 2.0 370 1.1
@ YEAR DOY SRAD TMAX TMIN RAIN FLAGW
 2004 1 11.5 1.4 -1.4 3.5 1
 2004 2 11.4 0.0 -2.1 2.5 0

[data series truncated]

!Additional data items can be added as illustrated below.
@ YEAR DOY SRAD TMAX TMIN RAIN WIND TDEW
 2004 1 11.5 1.4 -1.4 3.5 4.2 0.6
 2004 2 11.4 0.0 -2.1 2.5 5.4 0.5

[data series truncated]

*/
   bool ICASA_mode = parameters.subformat == "ICASA";                                       //140916

   UED::Database_file_indexed UED_file(parameters.UED_filename.c_str(),std::ios::in,false);
   Geolocation *geoloc = UED_file.mod_geolocation_record();
   if (!geoloc)
   {  std::cerr << parameters.UED_filename << " has no geolocation meta information" << std::endl;
      geoloc = new Geolocation; // Providing dummy geolocation so we can proceed.
   }
   CORN::Text_list descriptions; UED_file.get_descriptions(descriptions);  std::string description_str;  descriptions.string_items(description_str,'\t');
   CORN::Text_list comments;     UED_file.get_comments(comments);          std::string comments_str;     comments    .string_items(comments_str,'\t');
   std::ofstream WTH_file(parameters.target_filename.c_str());
   // ICASA 1.x weather files
   std::string name;
   CORN::wstring_to_string(parameters.UED_filename.get_name(),name);
   WTH_file
      << (ICASA_mode ? "$WEATHER : " : "*WEATHER DATA : " )<< name              << std::endl
/* NYI
            << "*GENERAL"                                         << std::endl
            << "@ NAME"                                           << std::endl
            <<  name  << std::endl
*/
/* NYI
            << "@ PEOPLE" << std::endl;
            << "unknown" << std::endl;
   I.e.:
  Wagger,M.G. Agronomy,Kansas State Univ,Manhattan,Kansas
  Kissel,D. Agronomy,Kansas State Univ,Manhattan,Kansas
  Hunt,L.A. Plant Ag.,Univ.Guelph,Guelph,Ontario Thunt@Plant.Uoguelph.Ca

*/
/* NYI
            << "@ INSTITUTES"                                     << std::endl
example  Kansas State Univ.
*/
/*NYI
            << "@ CONTACTS
 Hunt,L.A. thunt@uoguelph.ca
*/
/* NYI
            << "@ NOTES" << std::endl
            << comments_str  << std::endl
*/
/*NYI
            << "@ DISTRIBUTION" << std::endl
   examples:
   Public
   Use at will but acknowledge source. No secondary distribution
*/
/*NYI
            << "@ADDRESS" << std::endl
*/
/*
            << "@ METHODS" << std::endl
  Standard Met Station instruments
*/
/* NYI
            << "@ VERSION"                                        << std::endl
            << "ICASA1.0 10-08-2006 ()"                           << std::endl
            << "@ SOURCES"                                        << std::endl
            << "ICASA 1.x format"                                 << std::endl
*/
   /* NYI
            << "@ MEASUREMENTS"                                   << std::endl
   example: Temperature,solar radiation and precipitation
   */
  /* NYI
            << "@ PROBLEMS"                                       << std::endl
  example: No formal check therefore unknown
  */
/* NYI
            << "@ QUALITY"                                        << std::endl
   // example: No missing values; wind is unusually high
*/
   /* NYI
            << "@ PUBLICATIONS"                                   << std::endl
            << "None of direct application"
   */
/* NYI
@ FLAG FLAG_DETAILS
 0 All data ok
 1 SRAD estimated from sun hours
*/
            ;
   // One or more data groups similar to the following are necessary in all files.
   // All clusters except the one beginning with WBYR are also necessary.
/*
   std::string dataset_code;
   dataset_code.append("??"); //An institute or region code (two characters, e.g., �UF� for �University of Florida�, �CA� for �Canada�),
   dataset_code.append("??");
   CORN::Date today;
   WTH_file << "*WEATHER_STATION:" << dataset_code << today.get_year() << std::endl
            << "@ NAME" << std::endl
            << name  // Actually should get this from geolocation ID
            << std::endl
               // Just using the source filename
            << "@ COUNTRY REGION LOCATION:"                       << std::endl
            << geoloc->get_country_name()                         << " "
            << geoloc->get_state_name()                           << " "
            << geoloc->get_county_name()                          << " "
            << geoloc->get_station_name()                         << std::endl;
*/

   float32 TAV;
   float32 AMP;
   calc_TAV_and_AMP
      (UED_file
      ,parameters.year_earliest
      ,parameters.year_latest
      ,TAV, AMP);
   WTH_file << "@ INSI      LAT     LONG  ELEV   TAV   AMP WNDHT TEMHT" /*TAV TAMP */;
   /// NYI WTH_file <<  "CO2 CO2A";
   WTH_file << std::endl;
   WTH_file
         <<std::setw(6)
         << "  WSU1"
         <<std::fixed
         <<std::setw(9)
         <<std::setprecision(3)
            << geoloc->get_latitude_dec_deg_32()
         <<std::setw(9)
         << geoloc->get_longitude_dec_deg_32()
         <<std::setw(6) <<std::setprecision(0)
            << geoloc->get_elevation()
         <<std::setw(6)<<std::setprecision(1)
            << TAV
         <<std::setw(6)<<std::setprecision(1)
            << AMP         ;
   WTH_file << std::setw(6) <<std::setprecision(1)
         << (geoloc->has_screening_height() ? geoloc->get_screening_height() : 2.0);
   // I should add to UED geolocation station the temperature measurement height
   WTH_file << std::setw(6) <<std::setprecision(1)
         << (geoloc->has_screening_height() ? geoloc->get_screening_height() : 2.0);
   // NYI CO2 CO2A
   WTH_file << std::endl;

   //140814   WTH_file << "@DATE    RAIN  TMAX  TMIN  SRAD  TDEW  WIND" << std::endl;
   WTH_file << "@DATE  RAIN  TMAX  TMIN  SRAD  TDEW  WIND" << std::endl;

   CORN::Date_clad_32 earliest_date   = UED_file.get_earliest_date_time();
   CORN::Date_clad_32 latest_date     = UED_file.get_latest_date_time();
   CORN::Date_time_clad_64 date(UED_file.get_earliest_date_time());
/*
   Extraterrestrial_solar_irradiance_day ET_solar_rad(UC_MJ_per_m2,*geoloc);
   Location_parameters location(*geoloc,0,0,0);
   Weather_provider weather_provider(date,ET_solar_rad,&location);
*/
   Precipitation                             precipitation;
   Air_temperature_maximum                   air_temperature_max;
   Air_temperature_minimum                   air_temperature_min;
   Air_temperature_average_calculator        air_temperature_avg(air_temperature_max,air_temperature_min);
   Solar_radiation                           solar_radiation;
   Relative_humidity                         relative_humidity_max;
   Relative_humidity                         relative_humidity_min;
   Relative_humidity                         relative_humidity_avg;

   Dew_point_temperature_maximum_calculator  DewPt_max
      (&relative_humidity_min
      ,&air_temperature_max
      ,0
      ,0
      /*
      ,Vapor_pressure_deficit_max   *_vapor_pressure_deficit_max
      ,Dew_point_temperature_average*_dew_point_temperature_avg               //081106
      */
      );
   Dew_point_temperature_minimum_calculator DewPt_min
      (&relative_humidity_max
      ,&air_temperature_min
      ,0 //Dew_point_temperature_maximum   *_dew_point_temperature_max
      ,0 //const Location_parameters       *_location
      ,0 //Season_S_W_SF                   *_season
      ,0 //Vapor_pressure_nighttime        *_vapor_pressure_min                      //081027
      ,0 //Dew_point_temperature_average   *dew_point_temperature_avg
      );
   //   Dew_point_temperature_average DewPt_avg;
   /*
   Dew_point_temperature_average_calculator dew_point_temperature_avg
      (&air_temperature_avg
      ,&relative_humidity_avg);
   */
   Wind_speed                               wind_speed;
   for (CORN::Date_clad_32 date(earliest_date)
       ;date <= latest_date
       ;date.inc_day())
   {
      precipitation           .invalidate();
      air_temperature_max     .invalidate();
      air_temperature_min     .invalidate();
      relative_humidity_max   .invalidate();
      relative_humidity_min   .invalidate();
      DewPt_max               .invalidate();
      DewPt_min               .invalidate();

      CORN::Year date_year = date.get_year();
      std::clog << (int)date_year/*date.get_year()*/ << ' ' << (int)date.get_DOY() << '\r';
      UED::Time_query_single_date time_qry(date.get_datetime64(),(UED_units_code)UT_day);
      bool override_set_safety = false;

      CORN::Quality_clad RAIN_quality    (CORN::measured_quality);
      CORN::Quality_clad TMAX_quality    (CORN::measured_quality);
      CORN::Quality_clad TMIN_quality    (CORN::measured_quality);
      CORN::Quality_clad SRAD_quality    (CORN::measured_quality);
      CORN::Quality_clad RH_max_quality  (CORN::measured_quality);
      CORN::Quality_clad RH_min_quality  (CORN::measured_quality);
      CORN::Quality_clad WIND_quality    (CORN::measured_quality);

      UED_units_code RAIN_units     = UC_mm;
      UED_units_code TMAX_units     = UC_Celcius;
      UED_units_code TMIN_units     = UC_Celcius;
      UED_units_code SRAD_units     = UC_MJ_per_m2;
      UED_units_code RH_max_units   = UC_percent;
      UED_units_code RH_min_units   = UC_percent;
      UED_units_code WIND_units     = UC_kilometers_per_day;

      precipitation         .set_mm     (UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_precipitation               .get(),time_qry,RAIN_units,RAIN_quality)     ,RAIN_quality);
      air_temperature_max   .set_Celcius(UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_temperature             .get(),time_qry,TMAX_units,TMAX_quality),TMAX_quality);
      air_temperature_min   .set_Celcius(UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_min_temperature             .get(),time_qry,TMIN_units,TMIN_quality),TMIN_quality);
      solar_radiation       .set_MJ_m2  (UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_solar_radiation_global      .get(),time_qry,SRAD_units,SRAD_quality)     ,SRAD_quality);

      float32 tmax = air_temperature_max.Celcius();
      float32 tmin = air_temperature_min.Celcius();
      if (tmax < tmin)
      {  tmax = tmin + 1.0;  //DSSAT does not accept Tmax and Tmin having same value.
         air_temperature_max.set_Celcius(tmax,TMAX_quality);
      }
      if (!SRAD_quality.is_valid()) // For old files
         solar_radiation       .set_MJ_m2  (UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_solar_radiation_ERRONEOUS/*180308 STD_VC_solar_radiation*/ .get(),time_qry,SRAD_units,SRAD_quality)     ,SRAD_quality);

      relative_humidity_max .force_value_qualified(UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_max_relative_humidity       .get(),time_qry,RH_max_units,RH_max_quality) ,RH_max_quality);
      relative_humidity_min .force_value_qualified(UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_min_relative_humidity       .get(),time_qry,RH_min_units,RH_min_quality) ,RH_min_quality);
      //float RH_avg = ((relative_humidity_max.get() + relative_humidity_min)) / 2.0;
      //RH_avg = CORN::must_be_between<float32>(RH_avg,relative_humidity_min.get(),relative_humidity_max.get());
      //relative_humidity_avg .set (RH_avg,RH_min_quality);

      wind_speed            .set_km_d   (UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_wind_speed                  .get(),time_qry,WIND_units  ,WIND_quality)   ,WIND_quality);
      /* NYI
      SUNH Total daily sunshine hours. best.set(CORN::measured_quality); UED_units_code SUNH_units = UC_UUUUU; float32 SUNH = UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_XXXX.get(),time_qry,UC_UUUUU,best);
      PAR  Total daily photosynthetic radiation.      best.set(CORN::measured_quality); UED_units_code PAR_units = UC_UUUUU; float32 PAR = UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_XXXX.get(),time_qry,UC_UUUUU,best);
      TWET Wet bulb air temperature at 9 a.m.best.set(CORN::measured_quality); UED_units_code XXX_units = UC_UUUUU; float32 XXX = UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_XXXX.get(),time_qry,UC_UUUUU,best);
      EVAP Total daily pan evaporation.best.set(CORN::measured_quality); UED_units_code XXX_units = UC_UUUUU; float32 XXX = UED_file.get_no_create(UED::Record_type_distinguisher_interface::UED_data_set_year_var_units ,UED::STD_VC_XXXX.get(),time_qry,UC_UUUUU,best);
      */
      //weather_provider.update();
      //air_temperature_avg.update();
      DewPt_max               .update();
      DewPt_min               .update();
      float32 DP_avg=(DewPt_max.Celcius() + DewPt_min.Celcius()) / 2.0;
      //DewPt_avg.set_Celcius(DP_avg,CORN::calculated_quality);
/*

      float32 TDEW_avg = (TDEW_max + TDEW_min) / 2.0;
*/
      if ((date_year >= parameters.year_earliest) && (date_year <= parameters.year_latest))
      {
         std::string date_str;
         nat32 DSSAT_date = ((date.get_year() % 100) * 1000) + date.get_DOY();
         CORN::append_nat32_to_string  (DSSAT_date,date_str,10,5,'0');
         WTH_file <<std::fixed
         << std::setw(5) << std::setprecision(0)<< date_str
         /*
         << std::setw(6) << std::setprecision(0)<< (int)date.get_year()          //140814
         << std::setw(6) << std::setprecision(0)<< (int)date.get_DOY()           //140814
         */
          //<< (int)date.get_year() << ' ' << (int)date.get_DOY()
         << std::setw(6) << std::setprecision(1)<< (precipitation              .is_valid() ? precipitation            .get_mm()      : -99)
         << std::setw(6) << std::setprecision(1)<< (air_temperature_max        .is_valid() ? air_temperature_max      .Celcius() : -99)
         << std::setw(6) << std::setprecision(1)<< (air_temperature_min        .is_valid() ? air_temperature_min      .Celcius() : -99)
         << std::setw(6) << std::setprecision(1)<< (solar_radiation            .is_valid() ? solar_radiation          .get_MJ_m2()   : -99)
         << std::setw(6) << std::setprecision(1)<< ((DewPt_max.is_valid() && DewPt_max.is_valid()) ? DP_avg : -99)
         << std::setw(6) << std::setprecision(1)<< (wind_speed                 .is_valid() ? wind_speed               .get_km_d()    : -99)
         << std::endl;
      records_exported++;
      }
   }
   WTH_file << "=" << std::endl;   // terminator
   return records_exported;
}
//______________________________________________________________________________
#ifdef NYI
nat32 DSSAT_ICASA_Convertor::export_DSSAT_3_5()
{
   This format is quite obsolete (not Y2k compilate)

/*Example format
*WEATHER : GAINESVILLE,FLORIDA,U.S.A
@ INSI LAT LONG ELEV TAV AMP REFHT WNDHT
UFGA 29.630 -82.370 40 20.9 7.4 1.5 2.0
@DATE SRAD TMAX TMIN RAIN
78001 5.3 18.3 13.3 4.8
78002 11.1 18.3 8.3 0.0
*/

/*
   std::ofstream WTH_file(parameters.target_filename.c_str());
   // ICASA 1.x weather files
   WTH_file << "$WEATHER:" << geolocation.get_description() << std::endl;
   WTH_file << "*GENERAL" << std::endl;
   WTH_file << "@ NAME" << std::endl;
   WTH_file <<  parameters.UED_filename.get_name(false)  << std::endl;
   WTH_file << "@ PEOPLE" << std::endl;
   WTH_file << "unknown" << std::endl;
   // I.e.:
  Wagger,M.G. Agronomy,Kansas State Univ,Manhattan,Kansas
  Kissel,D. Agronomy,Kansas State Univ,Manhattan,Kansas
  Hunt,L.A. Plant Ag.,Univ.Guelph,Guelph,Ontario Thunt@Plant.Uoguelph.Ca

   //NYI WTH_file << "@ADDRESS" << std::endl;
   WTH_file << "@ VERSION" << std::endl;
   WTH_file << "unknown" << std::endl;
   //I.e.   31-08-99(LAH,Guelph);26-08-99(LAH,Guelph);30-07-99(LAH,Guelph)

   //! The above are recommended as a minimum. Additional data items, as illustrated
   //! below, can be added for comprehensive documentation.

   WTH_file << "@ SOURCES" << std::endl;
   WTH_file << "IBSNAT DSSAT 3.0" << std::endl;
   //NYI
   //WTH_file << "@ MEASUREMENTS" << std::endl;
   //Temperature,solar radiation and precipitation
   \

   WTH_file << "@ METHODS" << std::endl;
  Standard Met Station instruments
   WTH_file << "@ PROBLEMS" << std::endl;
  No formal check therefore unknown
   WTH_file << "@ NOTES" << std::endl;
  Wind and dewpoint included; wind in m/s as average over 24h

   WTH_file << "@ QUALITY" << std::endl;
   // NYI No missing values; wind is unusually high

   WTH_file << "@ PUBLICATIONS" << std::endl;
   WTH_file << "None of direct application"
   WTH_file << "@ DISTRIBUTION" << std::endl;
   WTH_file << "Public"<< std::endl;


   // One or more data groups similar to the following are necessary in all files.
   // All clusters except the one beginning with WBYR are also necessary.


   //?   WTH_file << "*WEATHER:KSAS  Ashland,Kansas

   WTH_file << "@ SITE" << std::endl;
   geolocation
  Ashland,Kansas fictitious weather station
   WTH_file << "@ INSI  WTHLAT  WTHLONG  WELEV   TAV  TAMP  TEMHT  WNDHT" << std::endl;
   WTH_file << setprecision(1);
  KSAS   37.11   -90.45     81   8.5  18.9    2.0    2.0
   WTH_file << "@ WBYR   WBDAY     WTYR  WTDAY" << std::endl;
  1980       1     1980      2


   WTH_file << "@ WEYR  WEDAY  SRAD  TMAX  TMIN  RAIN  WIND  TDEW" << std::endl;
   also VPRS and RHUM //110805
   WTH_file << "  " << setw(4) << 1980 << setw(7)<< doy<< setprecision(1) << setw(6) srad << setw(6) << tmax <<setw(6) tmin <<setw(6) << rain << setw(6)<< wind_ms << setw(6) << TDew << std::endl;
  1980      2  11.4   0.0  -2.1   2.5   5.4   0.5

*/
}
#endif
//______________________________________________________________________________
DSSAT_ICASA_Convertor::DSSAT_ICASA_Convertor
(UED::Convertor_arguments &arguments)                                            //180308
: UED::Convertor_weather(arguments)                                              //180308
/*180308
(int argc,const  char *argv[])
: UED::Convertor(argc,argv)
*/
{}
//______________________________________________________________________________

