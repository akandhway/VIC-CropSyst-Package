#ifndef convertor_weatherH
#define convertor_weatherH

#include "UED/convert/convertor.h"
#include "common/weather/validator.h"
#include "common/weather/database/common_weather_creators.h"
#include "common/weather/stations/all_stations.h"
#include "common/weather/atmospheric_CO2_change_recalibration.h"
#include "corn/math/statistical/stats.h"
// Station code selection (if applicable)
// Stores the UED database file or files for each station encountered
class Station_database;
namespace UED {
//______________________________________________________________________________
class Convertor_weather
: public extends_ Convertor
, public extends_ Common_weather_creators
{
 protected:
   Bidirectional_list station_databases;
      // This is a list of the UED files created for the stations encountered
      // in the import file.
   mutable All_known_weather_stations all_known_weather_stations;
   std::string last_encountered_station_ID;                                      //140609
   Weather_validator *validator;                                                 //140530
   contribute_ std::string station_ID_to_use;                                    //160212_050526
   provided_ Station_database *station_database;                                 //160216
//NYI   void calculate_daily_values();
 protected:
   inline virtual Weather_validator *provide_weather_data_validator() provision_
      { return 0;}
   virtual const Data_record_creation_layout                                     //160212
      *get_recognized_creation_layout(UED::Variable_code variable_code);
   virtual UED::Database_file_indexed *provide_UED_file();                       //160212
   virtual bool is_import_data_record                                            //160212
      (const Delineated_data_record &data_rec
      ,const ED_tabular_file_format &format
      ,const Bidirectional_list &import_columns
      ,const UED::Geolocation_record *header_identified_geolocation) contribution_;
   const char *get_station_ID_from_data_record                                   //160212_100614
      (const CORN::Delineated_data_record &data_rec
      ,const ED_tabular_file_format &format
      ,const Bidirectional_list &import_columns
      ,std::string &record_station_ID_result)                     contribution_;
   virtual bool set_validated_elements                                           //160212
      (const CORN::Date_time &row_date_time
      ,CORN::Delineated_data_record &data_rec
      ,bool calculate_daily_values);
 private:  // The following are for tallying daily value
   CORN::Date_clad_32  stats_curr_date;
   Statistical_running_tally  daily_precipitation_mm_stats;       CORN::Quality_clad daily_precipitation_attribute;
   Statistical_running_tally  daily_avg_temperature_stats;        CORN::Quality_clad daily_avg_temperature_attribute;
   Statistical_running_tally  daily_avg_relative_humidity_stats;  CORN::Quality_clad daily_avg_relative_humidity_attribute;
   Statistical_running_tally  daily_solar_radiation_MJ_m2_stats;  CORN::Quality_clad daily_solar_radiation_attribute;
   Statistical_running_tally  daily_wind_speed_m_sec_stats;       CORN::Quality_clad daily_wind_speed_attribute;
   Statistical_running_tally  daily_wind_direction_stats;         CORN::Quality_clad daily_wind_direction_attribute;
      // NYI: min&max dewpt, daytime and full day VPD, snow depth, snowfall
 public:
   Convertor_weather(Convertor_arguments &arguments);                            //170725
   virtual ~Convertor_weather();
   virtual float32 get_valid_element_amount
      (const UED::Column_elements::Element *element
      ,CORN::Quality_clad &identified_quality)                            const;
   bool calc_TAV_and_AMP
      (UED::Database_file_indexed &UED_file
      ,CORN::Year start_year
      ,CORN::Year stop_year
      ,float32 &TAV, float32 &AMP);
      // calc_TAV_and_AMP used by both DSSAT and APSIM
   inline virtual const char *get_daily_weather_target_file_extension()    const
      { return "dat";}                                                           //140627
      // This is used to assume a target filename extension
      // when export from UED file doesn't specifiy target filename.
      // (It will be given the same name as the UED file with this extension).
   Atmospheric_CO2_change_recalibration *render_atmospheric_CO2_change           //150330
      (const CORN::OS::Directory_name &UED_filename
      ,const CORN::date32  &today_ref)                               rendition_;
      // This could actually return Atmospheric_CO2_change_interface
   virtual Geolocation *render_geolocation(const std::string &station_ID) const; // renders
   Station_database *provide_station_database
      (const std::string &station_ID,bool &added_now
      ,const CORN::OS::File_name *known_UED_filename=0);
   Station_database *provide_station_database_from_UED_filename
      (const CORN::OS::File_name &known_UED_filename,bool &added_now);
   bool done_with_station(const std::string &station_ID)          modification_; //140609
   bool setup_stations_from_response_UED_filenames();
   void notify_climgen();
 protected:
   virtual bool inject_calculated_values();                                      //160212
   virtual Geolocation_record  */*Convertor_weather::*/provide_geolocation_record();     //160212
};
//_2015-12-29___________________________________________________________________
} // namespace UED
#endif
