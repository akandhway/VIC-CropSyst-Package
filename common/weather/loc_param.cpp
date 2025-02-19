
#  include <math.h>
//#  include "corn/datetime/date.hpp"  // For month abbrevs

#  ifndef compareHPP
#     include "corn/math/compare.hpp"
#  endif
#  include "corn/data_source/vv_file.h"
#  include "weather/loc_param.h"
#  include "corn/OS/directory_entry_name_concrete.h"
#include "corn/measure/units.ph"
#include "corn/OS/file_system_engine.h"
//______________________________________________________________________________
using namespace CORN;                                                             //001130
//______________________________________________________________________________
Location_parameters::Location_parameters
(const char *weather_filename_
,bool store_geolocation // set to true when creating location file for old CropSyst location/weather file sets //050525
)         //040416
:Common_parameters_data_record                                                   //020514
   (NO_ASSOCIATED_DIRECTORY,"location",4)
,geolocation(0)                                                                  //050526
#if ((CS_VERSION>0) && (CS_VERSION<5))
,parameterization_UED_filename("")                                               //050525
,database_filename(weather_filename_)
#ifdef SUPPORT_OBSOLETE_LOC_FILE
,weather_filename_V3(weather_filename_)                                         //010109
#endif
#endif
,ET_slope                   (DEF_ET_slope)                                       //000731
,ET_intercept               (DEF_ET_intercept)                                   //000731
,wind_speed_spring_summer_ms_32(1.0)                                             //040715
,wind_speed_fall_winter_ms_32  (1.0)                                             //040715
,wind_speed_classification_clad(WIND_SPEED_MEDIUM)                               //020322
,solar_radiation_parameters()                                                    //091214
#if ((CS_VERSION>0) && (CS_VERSION<5))
,ET_model_clad_V3(ET_FAO_56_Penman_Monteith_CropSyst)                            //020322
,PT_constant_32_V3                (DEF_PT_constant)
,relative_humidity_extremes()                                                    //091216
,v_PT_constant_V3                    (PT_constant_32_V3                     ,U_units_not_specified  ,LABEL_PT_constant               ,3,DEF_PT_constant      ,   1.0,   1.7,   1.0,   2.0,"1-2"/*TU_1_2*/,"Priestley-Taylor Constant"/*TL_PT_constant*/)
,v_wind_speed_spring_summer_ms (wind_speed_spring_summer_ms_32  ,UC_meters_per_second  ,LABEL_wind_speed_spring_summer  ,1,1.0                  ,   0.0,   9.0,   0.0,  15.0,"m/s"/*TU_m_s*/,"Summer"/*TL_Summer*/,"Average wind speed"/*TL_Average_wind_speed*/)
,v_wind_speed_fall_winter_ms   (wind_speed_fall_winter_ms_32    ,UC_meters_per_second  ,LABEL_wind_speed_fall_winter    ,1,1.0                  ,   0.0,   9.0,   0.0,  15.0,"m/s"/*TU_m_s*/,"Winter"/*TL_Winter*/,"Average wind speed"/*TL_Average_wind_speed*/)
,v_mean_peak_rainfall_1 (mean_peak_rainfall[1] ,UC_fraction, "1",2,0.0,0.0,5.0,0.0,10.0,"0-1","January")
,v_mean_peak_rainfall_2 (mean_peak_rainfall[2] ,UC_fraction, "2",2,0.0,0.0,5.0,0.0,10.0,"0-1","February")
,v_mean_peak_rainfall_3 (mean_peak_rainfall[3] ,UC_fraction, "3",2,0.0,0.0,5.0,0.0,10.0,"0-1","March")
,v_mean_peak_rainfall_4 (mean_peak_rainfall[4] ,UC_fraction, "4",2,0.0,0.0,5.0,0.0,10.0,"0-1","April")
,v_mean_peak_rainfall_5 (mean_peak_rainfall[5] ,UC_fraction, "5",2,0.0,0.0,5.0,0.0,10.0,"0-1","May")
,v_mean_peak_rainfall_6 (mean_peak_rainfall[6] ,UC_fraction, "6",2,0.0,0.0,5.0,0.0,10.0,"0-1","June")
,v_mean_peak_rainfall_7 (mean_peak_rainfall[7] ,UC_fraction, "7",2,0.0,0.0,5.0,0.0,10.0,"0-1","July")
,v_mean_peak_rainfall_8 (mean_peak_rainfall[8] ,UC_fraction, "8",2,0.0,0.0,5.0,0.0,10.0,"0-1","August")
,v_mean_peak_rainfall_9 (mean_peak_rainfall[9] ,UC_fraction, "9",2,0.0,0.0,5.0,0.0,10.0,"0-1","September")
,v_mean_peak_rainfall_10(mean_peak_rainfall[10],UC_fraction,"10",2,0.0,0.0,5.0,0.0,10.0,"0-1","October")
,v_mean_peak_rainfall_11(mean_peak_rainfall[11],UC_fraction,"11",2,0.0,0.0,5.0,0.0,10.0,"0-1","November")
,v_mean_peak_rainfall_12(mean_peak_rainfall[12],UC_fraction,"12",2,0.0,0.0,5.0,0.0,10.0,"0-1","December")
#else
//110124 NYI Need to use Parameter_numeric
#endif
,parameterized_from("")                                                          //050912
{  if (store_geolocation)                                                        //050525
      geolocation = new Geolocation;                                             //050525
   invalidate_ClimGen_parameters();                                              //081106
   for (uint8 m = 0; m <= 12; m++) mean_peak_rainfall[m] = 0;                    //060712
}
//_Location_parameters:constructor__________________________________1999-02-27_/
Location_parameters::Location_parameters
(const Geolocation &geoloc /*180626 ,nat8 version_release,nat8 version_minor*/)
:Common_parameters_data_record
   (NO_ASSOCIATED_DIRECTORY,"location",4/**CS_VERSION*/ /*180626 version_major,version_release,version_minor*/)
,geolocation(new Geolocation(geoloc))                                            //051107
#if ((CS_VERSION>0) && (CS_VERSION<5))
,parameterization_UED_filename("")                                               //050525
,database_filename("")                                                           //010109
#ifdef SUPPORT_OBSOLETE_LOC_FILE
,weather_filename_V3("")                                                         //010109
#endif
#endif
,wind_speed_classification_clad(WIND_SPEED_MEDIUM)                            //020322
,solar_radiation_parameters()                                                    //091214
,relative_humidity_extremes()                                                    //091216
,ET_slope                     (DEF_ET_slope)                                     //000731
,ET_intercept                 (DEF_ET_intercept)                                 //000731
,wind_speed_spring_summer_ms_32  (1.0)                                           //040715
,wind_speed_fall_winter_ms_32    (1.0)                                           //040715
#if (CS_VERSION==4)
,ET_model_clad_V3          (ET_FAO_56_Penman_Monteith_CropSyst)               //020322
,PT_constant_32_V3            (DEF_PT_constant)
,v_PT_constant_V3                (PT_constant_32_V3               ,U_units_not_specified  ,LABEL_PT_constant               ,3,DEF_PT_constant      ,   1.0,   1.7,   1.0,   2.0,"1-2"/*TU_1_2*/,"Priestley-Taylor Constant"/*TL_PT_constant*/)
,v_wind_speed_spring_summer_ms   (wind_speed_spring_summer_ms_32  ,UC_meters_per_second  ,LABEL_wind_speed_spring_summer  ,1,1.0                  ,   0.0,   9.0,   0.0,  15.0,"m/s"/*TU_m_s*/,"Summer"/*TL_Summer*/,"Average wind speed"/*TL_Average_wind_speed*/)
,v_wind_speed_fall_winter_ms     (wind_speed_fall_winter_ms_32    ,UC_meters_per_second  ,LABEL_wind_speed_fall_winter    ,1,1.0                  ,   0.0,   9.0,   0.0,  15.0,"m/s"/*TU_m_s*/,"Winter"/*TL_Winter*/,"Average wind speed"/*TL_Average_wind_speed*/)

//Note, I have the units for these, I need to look them up!
,v_mean_peak_rainfall_1 (mean_peak_rainfall[1] ,UC_fraction, "1",2,0.0,0.0,5.0,0.0,10.0,"0-1","January")
,v_mean_peak_rainfall_2 (mean_peak_rainfall[2] ,UC_fraction, "2",2,0.0,0.0,5.0,0.0,10.0,"0-1","February")
,v_mean_peak_rainfall_3 (mean_peak_rainfall[3] ,UC_fraction, "3",2,0.0,0.0,5.0,0.0,10.0,"0-1","March")
,v_mean_peak_rainfall_4 (mean_peak_rainfall[4] ,UC_fraction, "4",2,0.0,0.0,5.0,0.0,10.0,"0-1","April")
,v_mean_peak_rainfall_5 (mean_peak_rainfall[5] ,UC_fraction, "5",2,0.0,0.0,5.0,0.0,10.0,"0-1","May")
,v_mean_peak_rainfall_6 (mean_peak_rainfall[6] ,UC_fraction, "6",2,0.0,0.0,5.0,0.0,10.0,"0-1","June")
,v_mean_peak_rainfall_7 (mean_peak_rainfall[7] ,UC_fraction, "7",2,0.0,0.0,5.0,0.0,10.0,"0-1","July")
,v_mean_peak_rainfall_8 (mean_peak_rainfall[8] ,UC_fraction, "8",2,0.0,0.0,5.0,0.0,10.0,"0-1","August")
,v_mean_peak_rainfall_9 (mean_peak_rainfall[9] ,UC_fraction, "9",2,0.0,0.0,5.0,0.0,10.0,"0-1","September")
,v_mean_peak_rainfall_10(mean_peak_rainfall[10],UC_fraction,"10",2,0.0,0.0,5.0,0.0,10.0,"0-1","October")
,v_mean_peak_rainfall_11(mean_peak_rainfall[11],UC_fraction,"11",2,0.0,0.0,5.0,0.0,10.0,"0-1","November")
,v_mean_peak_rainfall_12(mean_peak_rainfall[12],UC_fraction,"12",2,0.0,0.0,5.0,0.0,10.0,"0-1","December")
#else
//110124 NYI need to add Parameter_numeric

#endif
{
   for (uint8 m = 0; m <= 12; m++) mean_peak_rainfall[m] = 0;                    //060712
}
//_Location_parameters:constructor__________________________________2005-03-07_/
void Location_parameters::invalidate_ClimGen_parameters()  // Only the constructor and ClimGen parameterization should call this function.
{  // This is used by ClimGen parameterization
   VPDmax_parameters.invalidate();                                               //091214
   solar_radiation_parameters.invalidate();                                      //091214
   ET_slope                      =DEF_ET_slope;                                  //000731
   ET_intercept                  =DEF_ET_intercept;                              //000731
   #if (CS_VERSION==4)
   PT_constant_32_V3             =0.0;                                           //081106
   #endif
   wind_speed_spring_summer_ms_32=0.0;                                           //081106
   wind_speed_fall_winter_ms_32  =0.0;                                           //081106
   vapor_pressure_nighttime_parameters.invalidate();                             //091214
   vapor_pressure_nighttime_parameters.slope = DEF_slope;                        //091210
   vapor_pressure_nighttime_parameters.intercept = DEF_intercept;                //091210
   vapor_pressure_nighttime_parameters.random_range_slope = DEF_slope;           //091210
   relative_humidity_extremes.invalidate();                                      //091216
}
//_invalidate_ClimGen_parameters_________________________________________2008?_/
Location_parameters::~Location_parameters()
{
/*
#if (CROPSYST_VERSION != 5)
// Crashing, haven't figured out why 160529
   delete geolocation;
#endif
*/
    delete geolocation;                                                          //201129LML
}
//_Location_parameters:destructor___________________________________2005-06-07_/
bool Location_parameters::expect_structure(bool for_write)
{  bool expected = Common_parameters_data_record::expect_structure(for_write);   //161025_031218
   structure_defined = false;                                                    //120314
   set_current_section(LABEL_location);

   /*191010 Warning this is broken Need to use geocoorinate record

   if (geolocation)                                                              //050525
       geolocation->setup_structure(*this,for_write);                            //080326

   */

   #if ((CS_VERSION>0) && (CS_VERSION<5))
      expect_file_name("parameterize",parameterization_UED_filename);            //050525
      expect_file_name(LABEL_database,database_filename);
   #else
      //moved to ClimGen parameters
   #endif
   #if (CS_VERSION==4)
      expect_enum(LABEL_ET_model,ET_model_clad_V3);
   #endif
   #ifdef SUPPORT_OBSOLETE_LOC_FILE
      expect_file_name(LABEL_weather_filename_V3,weather_filename_V3);           //060221
   #endif
   #if ((CS_VERSION > 0) && (CS_VERSION < 5))
       expect_float32(LABEL_fitted_B_solar_rad,solar_radiation_parameters.fitted_B);  // not user edited        //970711
       expect_float32(LABEL_Tnc_solar_rad     ,solar_radiation_parameters.Tnc);                                 //970712
       expect_float32(LABEL_clear_sky_transmission_coef,solar_radiation_parameters.clear_sky_transmission_coef);//970711
   #else
   set_current_section("solar_radiation");
      expect_float32("fitted_B"  ,solar_radiation_parameters.fitted_B);  // not user edited                 //970711
      expect_float32("Tnc"       ,solar_radiation_parameters.Tnc);                                          //970712
      expect_float32("clear_sky_transmission_coef",solar_radiation_parameters.clear_sky_transmission_coef); //970711
   #endif
   #if ((CS_VERSION > 0) && (CS_VERSION < 5))
      expect_valid_float32(v_PT_constant_V3);
      expect_valid_float32(v_wind_speed_spring_summer_ms);                       //980303
      expect_valid_float32(v_wind_speed_fall_winter_ms);                         //980303
      expect_float32(LABEL_aridity_factor,VPDmax_parameters.aridity_factor);     //170804
   #else
      #ifdef NYI
      // 110124   I haven't yet setup these parameters
      expect_parameter(p_PT_constant_V3);
      expect_parameter(p_wind_speed_spring_summer_ms);                           //980303
      expect_parameter(p_wind_speed_fall_winter_ms);                             //980303
      expect_parameter(p_aridity_factor);                                        //000914
   #  endif
   #endif
      expect_enum(LABEL_wind_speed_classification,wind_speed_classification_clad); //970723
   // I think this would be an appropriate place for this
   // It is also overridden by GIS applications
   set_current_section(LABEL_ET);                                                //000715
      expect_float32(LABEL_hargreaves_slope,ET_slope);                           //000731
      expect_float32(LABEL_hargreaves_intercept,ET_intercept);                   //000731
   set_current_section("vapor_pressure_deficit");                                //000715
      expect_float32("aridity_factor"  ,VPDmax_parameters.aridity_factor);       //980304
      expect_float32("slope"           ,VPDmax_parameters.slope);                //907010
      expect_float32("intercept"       ,VPDmax_parameters.intercept);            //907010
      expect_float32_array_with_units_label
         (LABEL_mean_peak_rainfall,"mm",mean_peak_rainfall,1,12,false);          //981212
   // special output for regional weather database
   set_current_section("vapor_pressure_nighttime");                              //091210
      expect_float32("slope",vapor_pressure_nighttime_parameters.slope);         //091210
      expect_float32("intercept",vapor_pressure_nighttime_parameters.intercept); //091210
      expect_float32("random_range_slope",vapor_pressure_nighttime_parameters.random_range_slope); //091210
   set_current_section("relative_humidity");                                                    //091216
      expect_float32("min_summer_low"        ,relative_humidity_extremes.min_summer_low);       //091216
      expect_float32("min_summer_high"       ,relative_humidity_extremes.min_summer_high);      //091216
      expect_float32("min_winter_low"        ,relative_humidity_extremes.min_winter_low);       //091216
      expect_float32("min_winter_high"       ,relative_humidity_extremes.min_winter_high);      //091216
      expect_float32("min_spring_fall_low"   ,relative_humidity_extremes.min_spring_fall_low);  //091216
      expect_float32("min_spring_fall_high"  ,relative_humidity_extremes.min_spring_fall_high); //091216
      expect_float32("max_summer_low"        ,relative_humidity_extremes.max_summer_low);       //091216
      expect_float32("max_summer_high"       ,relative_humidity_extremes.max_summer_high);      //091216
      expect_float32("max_winter_low"        ,relative_humidity_extremes.max_winter_low);       //091216
      expect_float32("max_winter_high"       ,relative_humidity_extremes.max_winter_high);      //091216
      expect_float32("max_spring_fall_low"   ,relative_humidity_extremes.max_spring_fall_low);  //091216
      expect_float32("max_spring_fall_high"  ,relative_humidity_extremes.max_spring_fall_high); //091216
   if (parameterized_from.length())                                              //050912
   {  set_current_section("region_database");                                    //050912
      expect_string("parameterized_from",parameterized_from,255);                //050912
   }
   structure_defined = true;                                                     //120314
   return expected;                                                              //161025
}
//_expect_structure____________________________________________________________/
bool Location_parameters::set_start()
{  if (description.brief == "")     description.brief = "-"; // Pascal has problem reading empty strings.
   if (!description.brief.length()) description.brief = "NONE";
   return Common_parameters_data_record::set_start();                            //161023_051028
}
//_set_start________________________________________________________2001-01-09_/
bool Location_parameters::get_end()
{  bool ended = Common_parameters_data_record::get_end();                        //161023_051028
   if (geolocation)
      if (CORN::is_approximately<float64>(geolocation->get_screening_height(),0.0,0.1))
         geolocation->set_screening_height(DEF_screening_height);
   return ended;                                                                 //161023
}
//_get_end_______________________________________________2011-01-09_2000-08-02_/
bool Location_parameters::has_mean_peak_rainfall()                         const
{  bool result = true;
   for (nat8 i = 1; i <= 12; i++)
      if (CORN::is_approximately<float32>(mean_peak_rainfall[i],0.0,0.000001))
         result = false;
   return result;
}
//_has_mean_peak_rainfall___________________________________________1999-05-27_/
#if (CS_VERSION>0) && (CS_VERSION<5)
//#ifdef SUPPORT_OBSOLETE_LOC_FILE
CORN::OS::File_name  &Location_parameters::get_database_or_weather_filename()
{  CORN::OS::File_name &return_filename =
      CORN::OS::file_system_engine.exists(weather_filename_V3)                   //150315
         ? dynamic_cast<CORN::OS::File_name &>(weather_filename_V3)              //150315
         : dynamic_cast<CORN::OS::File_name &>(database_filename);               //150315
//std::clog << return_filename.c_str() << std::endl;
   return return_filename;
}
#else
//_get_database_or_weather_filename______________________2011-01-04_2006-02-21_/
#endif
void Location_parameters::set_VPD_night_parameters
(float32 VP_slope_
,float32 VP_intercept_
,float32 random_range_slope_)                                      modification_
{  vapor_pressure_nighttime_parameters.slope             = VP_slope_;
   vapor_pressure_nighttime_parameters.intercept         = VP_intercept_;
   vapor_pressure_nighttime_parameters.random_range_slope= random_range_slope_;
}
//_set_VPD_night_parameters_________________________________________2009-12-10_/

