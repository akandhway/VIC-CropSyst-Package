#  include "corn/measure/measures.h"
#  include "common/weather/database/weather_DAT_GEN_database.h"
#  include "common/weather/weather_provider.h"
#  include "UED/library/std_codes.h"
#include "corn/chronometry/time_conversion.h"
#include "corn/data_source/vv_file.h"
#if (CORN_FS_VERSION==2020)
#include "OS/FS/file_system_engine.h"
#else
#include "corn/OS/file_system_engine.h"
#endif

#ifndef CLIMGEN
#  ifdef APPTYPE_CONSOLE
#     include <iostream>
      // for cerr
#  endif
#endif
#include <assert.h>

//I am requiring the input to be in UED format, it will be too slow to access from ORACLE.
//______________________________________________________________________________
Weather_data_text::Weather_data_text
(Location_parameters  &location_params_                                          //000313
,CORN::OS::File_name &template_filename_)                                        //130828
:Weather_database()
,template_filename(template_filename_)
,for_write(false)
,format_to_write(Weather_text_file::UNKNOWN_format)                              //021218
,current(0)
,aux(0)
,pref_ext("dat")                                                                 //001010
,allow_missing_dates(false)                                                      //040427
,location_parameters(location_params_)
,need_to_write_column_headers(false)                                             //060221
,single_outfile(0)                                                               //060221
{}
//______________________________________________________________________________
Weather_data_text::~Weather_data_text()
{  reset_buffers();                                                              //000519
   if (single_outfile) delete single_outfile; single_outfile = 0;
}
//______________________________________________________________________________
const Location_parameters *Weather_data_text::get_location_parameters()    const
{  return &location_parameters;
}
//_get_location_parameters__________________________________________2005-05-25_/
const Geolocation         *Weather_data_text::get_geolocation_constant()   const
{  return (location_parameters.geolocation);
}
//_get_geolocation_constant_________________________________________2005-05-25_/
const char *Weather_data_text::get_description(std::string &return_buffer) const
{  return  location_parameters.description.brief.c_str();                        //150112
}
//_get_description__________________________________________________2005-05-25_/
void Weather_data_text::setup_to_write(Weather_text_file::Format_old desired_format)
{  for_write = true;
   format_to_write = desired_format;
   if (!single_outfile && desired_format == Weather_text_file::V4_format)
      // this format stores all data in a single file //060221_
   {   single_outfile = new std::ofstream(template_filename.c_str());
       need_to_write_column_headers = true;
   }
}
//_setup_to_write___________________________________________________2002-12-16_/
void Weather_data_text::setup_current(Year year) mutates_
{
   if (aux && (aux->year == year))
   {  Weather_text_file *hold = current;
      current = aux;
      aux = hold;
      return;
   }
   GEN_DAT_UED_Filename curr_filename(template_filename.c_str());
   Year template_year = curr_filename.get_year();
   if (template_year)
   {  curr_filename.set_year(year);                                              //040427
      curr_filename.set_ext(pref_ext.c_str());                                   //001010
      if (for_write)
      { /* The year is always set
         curr_filename.set_year(year);
      */
      } else
      {  if ((int)year != MAXINT)        // We now need to allow year 0
         {
            if (!file_system_engine.exists(curr_filename))   // If file with .DAT does not exist
               curr_filename.set_ext("gen");// Try .GEN file
         } else
         {
#  ifndef CLIMGEN
#     ifdef APPTYPE_CONSOLE
            std::cerr << "Unable to open file" << ':'  << curr_filename.c_str() << std::endl;
#     else
#        ifdef _Windows
             MessageBox(0,curr_filename.c_str(),"Unable to open file",MB_ICONEXCLAMATION	);
#        endif
#     endif
#endif
        // Here we show throw an exception indicating that
        // The file doesn't exist
            return; //
         }
      }
   }
   if (aux) delete aux;                                                          //990831
   aux = current;
   current = new Weather_text_file
      (curr_filename
      ,year
      ,for_write
      ,format_to_write
      ,allow_missing_dates
      ,need_to_write_column_headers
      ,single_outfile);
   need_to_write_column_headers = false;                                         //060221
}
//______________________________________________________________________________
void Weather_data_text::set_today(const CORN::Date_const & date_)
{  current_date.set(date_);                                                      //091217
   if (!current ||(current->year != date_.get_year()))
      setup_current(date_.get_year());                                           //990831
}
//______________________________________________________________________________
void Weather_data_text::invalidate(bool absolutely)
{  select_buffer(current_date.get_year())->invalidate_DOY(current_date.get_DOY());
}
//_invalidate____________________________________________2009-12-16_1999-10-12_/
Weather_text_file *Weather_data_text::select_buffer(Year date_year_) mutates_
{  if (current && (current->year == date_year_))  return current;
   if (aux && (aux->year == date_year_))          return aux;
   setup_current(date_year_);
   return current;
}
//_select_buffer____________________________________________________1999-08-31_/
void Weather_data_text::set_precipitation_intervals_mm
(const CORN::Date_const &date_
,const CORN::Time &time_
,const CORN::Dynamic_array<float32>&precip_in_intervals
,CORN::Minutes interval_time_minutes
,CORN::Quality &attribute)
{  // NYI  we have never had a 30 weather text file format
}
//_set_precipitation_intervals_mm___________________________________1999-04-25_/
void Weather_data_text::set_temperature_intervals
( const CORN::Date_const &date_
,const CORN::Dynamic_array<float32>&temperature_in_intervals
,CORN::Minutes interval_time_minutes                                             //990425
,CORN::Quality &attribute)
{  // NYI  we have never had a 30 weather text file format
}
//_set_temperature_intervals________________________________________2002-11-24_/
bool Weather_data_text::get_to(Weather_provider& provider)
{
   CORN::Year year   = provider.date.get_year();
   CORN::DOY  doy    = provider.date.get_DOY();
   Weather_text_file *buffer =  select_buffer(year);                             //991012

   CORN::Quality_clad precipitation_attribute;  provider.precipitation_actual       .set_m                        (buffer->get_precipitation_m           (doy,precipitation_attribute  ),precipitation_attribute);
   CORN::Quality_clad Tmax_attribute;           provider.air_temperature_max        .update_value_qualified       (buffer->get_max_temperature           (doy,Tmax_attribute           ),Tmax_attribute);
   CORN::Quality_clad Tmin_attribute;           provider.air_temperature_min        .update_value_qualified       (buffer->get_min_temperature           (doy,Tmin_attribute           ),Tmin_attribute);
   //CORN::Quality_clad srad_attribute;           provider.solar_radiation            .set_MJ_m2                    (buffer->get_solar_radiation           (doy,srad_attribute           ),srad_attribute);
   Solar_radiation Srad;                        provider.solar_radiation            .set_MJ_m2                    (buffer->get_solar_radiation           (doy,Srad                     ),Srad);
   CORN::Quality_clad TdewMax_attribute;        provider.dew_point_temperature_max  .update_value_qualified       (buffer->get_max_dew_point_temperature (doy,TdewMax_attribute        ),TdewMax_attribute);
   CORN::Quality_clad TdewMin_attribute;        provider.dew_point_temperature_min  .update_value_qualified       (buffer->get_min_dew_point_temperature (doy,TdewMin_attribute        ),TdewMin_attribute);
   CORN::Quality_clad RHmax_attribute;          provider.relative_humidity_max      .set_value_quality            (buffer->get_max_relative_humidity     (doy,RHmax_attribute          ),RHmax_attribute);
   CORN::Quality_clad RHmin_attribute;          provider.relative_humidity_min      .set_value_quality            (buffer->get_min_relative_humidity     (doy,RHmin_attribute          ),RHmin_attribute);
   CORN::Quality_clad wind_attribute;           provider.wind_speed                 .set_m_d                      (buffer->get_wind_speed_m_d            (doy,wind_attribute           ),wind_attribute);
   //NYI CORN::Quality_clad ET_reference_attribute;   provider.ET_reference               .set_m                        (buffer->get_ET_reference_m            (doy,ET_reference_attribute   ),ET_reference_attribute); //180806
   return true;
}
//_get_to___________________________________________________________2017-01-11_/
bool Weather_data_text::set_from  (Weather_provider& provider)
{
   CORN::Year year   = provider.date.get_year();
   CORN::DOY  doy    = provider.date.get_DOY();
   Weather_text_file *buffer =  select_buffer(year);                             //991012
   bool sat = true;
   float32 RH_max  = provider.relative_humidity_max.percent();                   //170802
   float32 RH_min  = provider.relative_humidity_min.percent();                   //170802
   if (RH_max < RH_min)                                                          //170802
   {  // if max < min should rarely happen.
      std::clog << "warning RHmax (" << RH_max << ") is less than RHmin (" << RH_min << ") on " << (int)year << ' ' << (int)doy << " (swapped, but check original dataset)" << std::endl;
      float32 hold_RH_min = RH_min;
      RH_min = RH_max;
      RH_max = hold_RH_min;
   }
   float32 DP_max  = provider.dew_point_temperature_max   .Celcius        ();    //170802
   float32 DP_min  = provider.dew_point_temperature_min   .Celcius        ();    //170802
   if (DP_max < DP_min)                                                          //170802
   {  // if max < min should rarely happen.
      float32 hold_DP_min = DP_min;
      DP_min = DP_max;
      DP_max = hold_DP_min;
   }
   float32 T_max = provider.air_temperature_max         .Celcius        ();      //170802
   float32 T_min = provider.air_temperature_min         .Celcius        ();      //170802
   if (CORN::is_approximately<float32>(T_max,T_min,0.0001)) //170802
   {  T_max += 0.1;
      T_min -= 0.1;
   }
   buffer->set_precip_m         (doy,provider.precipitation_actual        .get_m          (),provider.precipitation_actual);
   buffer->set_max_temp         (doy,T_max                                                  ,provider.air_temperature_max);
   buffer->set_min_temp         (doy,T_min                                                  ,provider.air_temperature_min);
   buffer->set_solar_rad        (doy,provider.solar_radiation             .get_MJ_m2      (),provider.solar_radiation);
   buffer->set_max_rel_humid    (doy,RH_max                                                 ,provider.relative_humidity_max);
   buffer->set_min_rel_humid    (doy,RH_min                                                 ,provider.relative_humidity_min) ;
   buffer->set_wind_speed_m_d   (doy,provider.wind_speed                  .get_m_d        (),provider.wind_speed);
   buffer->set_max_dew_point    (doy,DP_max                                                 ,provider.dew_point_temperature_max);
   buffer->set_min_dew_point    (doy,DP_min                                                 ,provider.dew_point_temperature_min);

   //180806 ET_reference NYI
   return sat;
}
//_set_from_________________________________________________________2017-01-11_/
void Weather_data_text::reset_buffers()
{  //110822 We delete aux first because when writing,
   // we want the older data written first
   delete aux;       aux=0;
   delete current;   current =0;
}
////////////////////////////////////////////////////////////////////////////////

