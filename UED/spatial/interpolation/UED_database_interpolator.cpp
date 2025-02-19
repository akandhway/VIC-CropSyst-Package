#pragma hdrstop
#include "UED/spatial/interpolation/UED_database_interpolator.h"
#include "UED/spatial/interpolation/ued_database_interpolate_datarec.h"
#include "UED/spatial/station_database_point.h"
#include "common/geodesy/UTM.h"
#include "UED/library/locrec.h"
#include "UED/library/std_codes.h"
#include "UED/library/datasetrec_creation.h"
#include "UED/library/timeqry.h"
#include "common/geodesy/geometry/terrestrial_geometry.h"
#include "corn/measure/measures.h"
#include "corn/container/text_list.h"
#include "corn/math/compare.hpp"
#include "corn/chronometry/time_types.hpp"

using namespace std;
//______________________________________________________________________________
UED_database_interpolator::UED_database_interpolator
(const UED_database_interpolate_data_record &_options                            //100926
,UED_variable_code  _variables_to_include[]           // null terminated array.  Lists the variables to be included in the interpolation
,Association_list  &source_database_filenames_list           // will be modified to return only the filename included in interpolation (that have reolocation record)
,Association_list  &source_database_filenames_not_included)  // return
:options(_options)
, target_UED_filename(_options.current_target_UED_filename.c_str())
, current_value(0,10,0)
,target_database(0)
,target_geolocation(0)
, variables_to_include(_variables_to_include)
, verbose(true)  // Eventually make this a parameter option
, included_stations_log(CORN_File_name_instance(_options.current_target_UED_filename,"stations_log").c_str())
{
   Association_list  databases_included;
   const CORN_File_name *source_ued_filename = 0;                                //130311
   Terrestrial_geometry  geometry;
   if (options.target_mode != 1)
   {  // We are in update existing source file mode so we expecting the UED database
      target_database = new UED_indexed_database(target_UED_filename.c_str()
         ,(std::ios_base::in | std::ios_base::out));                             //130311
      target_geolocation = target_database->mod_geolocation_record();
   } else
   {  // Generating point We need to provide the geolocation                     //100926
      {
         if (options.single_point_mode == 0) //geolocation specific               //100926
         {
            // The geolocation was provided by the user                          //100926
            // or the UI is running one of multiple point                        //100926
            // and has set the geolocation.                                      //100926
         }
         else // determine centroid                                              //100926
         {
         //NYI load all source locations UED and calculate centroid.             //100926
         // this will be the target geolocation                                  //100926
                                                                                 //100926
         //   set this in options.geolocation                                    //100926
         }
         target_geolocation = const_cast<Geolocation *>(&options.geolocation);
      }
   }
 if (target_geolocation)
 { // The target UED must be geolocated
   // Currently limiting to 32 points because windows limits the number of files that can be openned at one time
   included_stations_log << target_UED_filename.c_str() << endl;
   included_stations_log << target_geolocation->get_station_name().c_str() << endl;
   included_stations_log << "source UED files and distance (km):" << endl;
   CORN::Text_list omissions;
   nat8 used_source_count = 0;
   while ((source_ued_filename = dynamic_cast<const CORN_File_name *>
      (source_database_filenames_list.pop_first())) && (used_source_count < 32))
   {
      // NYI eventually should be wstring const std::wstring &source_station_ID =source_ued_filename->get_name();
      std::string source_station_ID;
      CORN::wstring_to_string(source_ued_filename->get_name(),source_station_ID);
      // Here we just the the database name as the ID,
      // the this point we just name a unique identifier for the dB.

      // open and add to UED database list
      Station_database_point *source_station_db = new Station_database_point
         (source_station_ID.c_str(),source_ued_filename->c_str()
         ,(std::ios_base::in | std::ios_base::out));                             //130311
      std::string name_only; CORN::wstring_to_string(source_ued_filename->get_name(),name_only);
      if (source_station_db)
      {  if (source_station_db->geolocated)
         {
            // Determine the station distance from the target station.
            const Geolocation *source_geolocation = source_station_db->ued_file.ref_geolocation_record();
            float32 distance_between_source_and_target_km = geometry.distance_between_km(*target_geolocation,*source_geolocation);
            source_station_db->set_distance(distance_between_source_and_target_km); // added so we can sort by distance

            bool station_in_range =  (distance_between_source_and_target_km < options.within_radius_km)
                  && !CORN::is_approximately<float32>(distance_between_source_and_target_km,0.0,0.0001) ; // very close distances through the power weight terms to huge numbers
            if (station_in_range)
            {
               // Determine the period of available data
               CORN::Date source_earliest_date(source_station_db->ued_file.get_earliest_date_time ( UED_FOR_ALL_RECORD_TYPES , UED_FOR_ALL_VARIABLES));
               CORN::Date source_latest_date  (source_station_db->ued_file.get_latest_date_time   ( UED_FOR_ALL_RECORD_TYPES , UED_FOR_ALL_VARIABLES));
               bool station_covers_any_of_the_target_period
                  =  ((source_earliest_date.get_year() >= options.begin_year) && (source_earliest_date.get_year() <= options.end_year))
                   ||((source_latest_date  .get_year() >= options.begin_year) && (source_latest_date.get_year()   <= options.end_year));
               if (station_in_range && station_covers_any_of_the_target_period)
               {  databases_included.append(source_ued_filename);
                  source_databases.append(source_station_db);
                  const Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point &point
                     = append_point(*source_geolocation,used_source_count++);
                  if (verbose) cout << "add :"<< name_only /*.c_str()*/ << "     " <<endl ;
                  included_stations_log
                     << '"' << source_ued_filename->c_str() << "\""
                     << "\"\t\"" << point.get_station_name()
                     << "\"\t"<<distance_between_source_and_target_km
                    //	<< '\t'  << point.weight   Not yet available here
                     <<  endl;

               } else
               {  source_database_filenames_not_included.append(source_ued_filename);
                  std::string omission("omit:");
                  omission += name_only;
                  char distance_cstr[10];
                  if ( !station_in_range)                         {omission.append(" (distance="); omission.append(itoa((int) distance_between_source_and_target_km ,distance_cstr,10)); omission.append("km) ");  };
                  if ( !station_covers_any_of_the_target_period)  {omission.append(" (period=");   omission.append(source_earliest_date.c_str()); omission.append("-"); omission.append(source_latest_date.c_str()); omission.append( ") " ); };

                  omissions.add_string(omission);                            //130311
                  if (verbose) cout << omission << '\r';
                  delete source_station_db;
               }
            } else // else completely forget the target
            {  if (verbose) std::cout << "out of range:" << name_only << std::endl;
               delete source_station_db;
            }
         } else
         {  delete source_station_db;
            if (verbose) cout << "not geolocated:"<<  source_ued_filename->c_str()<< endl  ;
         }
      }
   }
   if (omissions.count())
   {
      included_stations_log << "Omitted stations:" << endl;
      omissions.write(included_stations_log);
   }
   if (source_databases.count())
   {
      if (!target_database && target_geolocation)
      {  target_database = new UED_indexed_database(target_UED_filename.c_str()
            , (std::ios_base::in | std::ios_base::out)  );                       //130311
         target_database->know_geolocation(*target_geolocation);
      }
   } else
      std::cout << "No stations in range with requested weather elements." << std::endl;
   source_databases.sort(CORN::Container::ASCENDING); // 091221 give the nearest file priority incase we exceed the bitmask limit.
   source_database_filenames_list.transfer_all_from(databases_included);
 } else // the target is not geolocated so we can't continue (no databases will be included.
 {
   delete target_database; target_database = 0;
   cerr << "Aborting interpolation: Target UED file is not geolocated.\n (Use a UED database editor to add geolocation parameters)" << endl;
   included_stations_log << "Aborting interpolation: Target UED file is not geolocated.\n (Use a UED database editor to add geolocation parameters)" << endl;
 }
}
//______________________________________________________________________________
UED_database_interpolator::~UED_database_interpolator()                                         //100926
{  delete target_database;
}
//_100926__________________________________________________________destructor__/
#ifdef  MULTI_INTERPS
float64 Stations_interpolator::calc_prescribed_function(nat32  index) const
{  return current_value.get(index); }
#else
float64 UED_database_interpolator::calc_prescribed_function(nat32  index) const
{  return current_value.get_no_resize(index); }
#endif
//______________________________________________________________________________
void UED_database_interpolator::interpolate_all_using_nearest()
{
   char variable_name[100];

   if (verbose)
   {  cout << "Interpolating to target:" << target_UED_filename.c_str() << endl;
      cout << "station count=" << source_databases.count() << endl;
   }
#ifdef disabled
   std::ofstream nearest_copied(CORN::File_name(target_UED_filename,"nearest_copy_log").c_str());
   nearest_copied << "Coping to target:" << target_UED_filename.c_str() << endl;
#endif
   float32 target_elevation_m = target_geolocation->get_elevation();
   static const float64 lapse_rate_C_per_m = -0.0065;  // -6.5�/km

   FOR_EACH_IN(source_station_database,Station_database_point,source_databases,each_database)
   {  UED_indexed_database &ued_file = source_station_database->ued_file;
      ued_file.goto_BOF();

      CORN_File_name_instance source_filename(ued_file.get_file_name());

      if (verbose)
         cout << endl;
      // NYI eventually I want to release data records from memory if the year is not currently being processed.
      for (UED_data_record *data_rec = ued_file.goto_next_data_record()
          ; data_rec
          ; data_rec = ued_file.goto_next_data_record())
      {  // for each data record in each database
         if (verbose)
         {
            CORN::Date out_date(data_rec->get_time_stamp());
             source_station_database->write_key(cout) 
             //180820 cout << source_station_database->get_key() 
             cout << '\t' << out_date.c_str() << '\r';
         }
         bool interpolate_this_record = false;
         // Make sure that the record store a variable to be included in the
         // interpolation
         for (int vi = 0;  variables_to_include[vi]; vi++)
         {  UED_variable_code  record_variable_code = data_rec->get_variable_code();
            if (record_variable_code == variables_to_include[vi])
               interpolate_this_record = true;
         }
//NYI         if (! matching record has already been
//NYI            interpolated (may already have been
//NYI            detected in a previous database in the loop)
//NYI            in the target database.
//NYI             interpolate_this_record = false;
         if (interpolate_this_record)
         {

            UED_data_record_creation_layout  matching_record_creation;
            data_rec->get_creation_layout(matching_record_creation);
            UED_variable_code variable_code = data_rec->get_variable_code();
            UED::Smart_variable_code smart_variable (variable_code);
            for (int value_i = 0; value_i < data_rec->data_values_get_count(); value_i++)
            {  // for each time step/value in the data record  (for each data value index of the data record)
               CORN::Quality this_quality;
               float32 this_value = data_rec->get_at(value_i, this_quality);
               CORN::Datetime64 date_time_i = data_rec->get_date_time_for_index(value_i);
               CORN::Date value_date(date_time_i);
               Year value_year = value_date.get_year();
               if ((value_year >= options.begin_year) && (value_year <= options.end_year))
               {

                  UED::Time_query_single_date time_qry(date_time_i);
                  CORN::Quality existing_quality = CORN::measured_quality;
                  CORN::Units_code records_units;
                  float32 existing_value = target_database->get_no_create
                     (matching_record_creation.options.record_type
                     ,matching_record_creation.variable_code
                     ,time_qry
                     ,records_units
                     ,existing_quality);
                  if (this_quality.is_better_than(existing_quality))
                  {  bool just_created;
                     CORN::Quality interpolation_quality = CORN::interpolated_quality ;
                     target_database->set_value(this_value,time_qry,data_rec->units_code,interpolation_quality,matching_record_creation,just_created,false);
                  }
               } // within date range                                           //100926
            } // for each value in the record
         } // interpolate this record
      } // end for each data record
   } FOR_EACH_END(each_database)
}
//______________________________________________________________________________
void UED_database_interpolator::interpolate_all_using_inverse_distance()  // was interpolate_all()
{
#ifndef  MULTI_INTERPS
   set_target(*target_geolocation);
   if (verbose) cout << "Calcuating weights (Shepard form)" << endl;
   calc_weights_shepard_form();
#endif
   if (verbose)
   {  cout << "Interpolating to target:" << target_UED_filename.c_str() << endl;
      cout << "station count=" << source_databases.count() << endl;
   }
   float32 target_elevation_m = target_geolocation->get_elevation();
   static const float64 lapse_rate_C_per_m = -0.0065;  // -6.5�/km
   FOR_EACH_IN(source_station_database,Station_database_point,source_databases,each_database)
   {  UED_indexed_database &ued_file = source_station_database->ued_file;
      ued_file.goto_BOF();

      if (verbose)
         cout << endl;
//             cout << endl << source_station_database->get_key() << endl;
      // NYI eventually I want to release data records from memory if the year is not currently being processed.
      for (UED_data_record *data_rec = ued_file.goto_next_data_record()
          ; data_rec
          ; data_rec = ued_file.goto_next_data_record())
      {  // for each data record in each database
         if (verbose)
         {
            CORN::Date out_date(data_rec->get_time_stamp());
             //180820 cout << source_station_database->get_key() 
             source_station_database->write_key(cout); //180820
             cout << '\t'
              << out_date.c_str() << '\r';
         };
         UED_variable_code variable_code = data_rec->get_variable_code();
         bool interpolate_this_record = false;
         // Make sure that the record store a variable to be included in the
         // interpolation
         for (int vi = 0;  variables_to_include[vi]; vi++)
            if (variable_code == variables_to_include[vi])
               interpolate_this_record = true;

//NYI         if (! matching record has already been
//NYI            interpolated (may already have been
//NYI            detected in a previous database in the loop)
//NYI            in the target database.
//NYI             interpolate_this_record = false;
         if (interpolate_this_record)
         {
            UED_data_record_creation_layout  matching_record_creation;
            data_rec->get_creation_layout(matching_record_creation);

            //100913 moved above UED_variable_code variable_code = data_rec->get_variable_code();

            for (int value_i = 0; value_i < data_rec->data_values_get_count(); value_i++)
            {  // for each time step/value in the data record  (for each data value index of the data record)





               CORN::Datetime64 date_time_i = data_rec->get_date_time_for_index(value_i);
               // Check to see that the date (year) is within the specified range of years.
               CORN::Date value_date(date_time_i);
               Year value_year = value_date.get_year();
               if ((value_year >= options.begin_year) && (value_year <= options.end_year))
               {
               CORN::Quality quality_i;
               UED::Time_query_single_date time_qry(date_time_i);
               int db_index = 0; // probably uint8
               current_valid_encoded = 0;

//         if (verbose)
//         {
//            CORN::Date out_date(date_time_i);
//             cout << source_station_database->get_key() << '\t'
//              << out_date << "   \r";
//         };

               FOR_EACH_IN(other_database, Station_database_point, source_databases, each_other_database)
               {
                  // Get the matching value in all existing UED databases )including the current source_station_database
                  // (it is possible that some of the existing
                  // UED database don't have a matching date/value
                  //  if that is the case then don't do the interpolation
                  // just use the value (could call the interpolation
                  // function, but it should just return the same value).

                  if (db_index < 32) // we only allow 32 databases because bitmasks are used to identify ...
                  {  //get the matching  value and quality from the other database
                  quality_i.set(CORN::measured_quality);   // prefer measured_quality if available
                  float64 value = other_database->ued_file.get_no_create
                  (data_rec->get_code()
                  ,variable_code
                  ,time_qry
                  ,data_rec->units_code
                  ,quality_i);

                  if (options.estimate_environmental_lapse_rate_adjustment)
                  {  //variable code is an air temperature (I don't think lapse rate applys to dew point temp)
                     if ((variable_code == UED::STD_VC_max_temperature.get())
                      || (variable_code == UED::STD_VC_min_temperature.get())
                      || (variable_code == UED::STD_VC_avg_temperature.get())
                      || (variable_code == UED::STD_VC_obs_temperature.get())
                        )
                     {  // Note that the lapse rate adjustment is
                        // for environmental lapse rate
                        float64 elevation_diff = other_database->get_elevation_m() - target_elevation_m;
                        float64 temperature_adjustment = elevation_diff * lapse_rate_C_per_m;
                        value += temperature_adjustment;
                     }
                  }
                  bool other_is_valid = quality_i.is_valid(); // UED_is_valid(quality_i);

                  // Set the valid indicator for the respective interpolation weighted points
#ifndef MULTI_INTERPS
                  set_point_valid(db_index,other_is_valid);
#endif
                  // update current value so it can be picked up by the
                  // Inverse_distance_weighted_interpolator::calc_prescribed_function()
                  current_value.set(db_index,value);
                  // always setting the value even if it is invalid;
                  // if it is
                  current_valid_encoded |= (other_is_valid << db_index);
                  db_index ++;
                  }
               } FOR_EACH_END(each_other_database)

               // I think always need to recalculate weights because each variable on each data
               // may be available at different locations.
               // Would probably be an efficiency to keep a list of weights for all station combinations.

               //apply inverse distance interpolation
               //to get the values for the target database record.

#ifdef  MULTI_INTERPS
               Stations_interpolator *interpolator = provide_stations_interpolator_for_current_value();
               float64 interpolated_value = interpolator->interpolate();
#else
                  calc_weights_shepard_form(); // so we can exclude points where the data is invalid/missing
               float64 interpolated_value = interpolate();
#endif
               CORN::Quality_code interpolated_quality(current_valid_encoded  ? UED_interpolated : UED_not_valid);
                       // probably should be estimated from ... but for we need to be able to run this through ClimGen

               // Add the value to the target database dataset.
               bool just_created = false;

                  CORN::Quality set_quality(interpolated_quality);
               if (set_quality.is_valid())

                  target_database->set_value
                     (interpolated_value,time_qry,data_rec->units_code
                     ,set_quality,matching_record_creation,just_created,false);

            } // end for each time step value
            } // within date range
         } // end not already interpolated
      } // end for each data record
   } FOR_EACH_END(each_database)
}
//______________________________________________________________________________
// Latitude_longitude_inverse_distance_weighted_interpolator ---------------------------
Latitude_longitude_inverse_distance_weighted_interpolator::Latitude_longitude_inverse_distance_weighted_interpolator
   (Method _method, float64 _power_parameter )
: Inverse_distance_weighted_interpolator(_method,  _power_parameter)
{}
//______________________________________________________________________________
void Latitude_longitude_inverse_distance_weighted_interpolator::set_target
(   const Geolocation &_from_geolocation)
{  if (target_point) delete target_point;
   target_point = new  Weighted_point(_from_geolocation,0);
}
//______________________________________________________________________________
#ifdef  MULTI_INTERPS
const Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point & UED_database_interpolator::append_point
(  const Geolocation &_from_geolocation,nat32  point_index)
{  Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point *point
   = new Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point(_from_geolocation,point_index);
   all_points_in_range.append(point);
   return *point;
}
//______________________________________________________________________________
#endif
const Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point
& Latitude_longitude_inverse_distance_weighted_interpolator::append_point
(const Geolocation &_from_geolocation,nat32  point_index)
{  Weighted_point *point=new Weighted_point(_from_geolocation,point_index);
   points.append(point);
   return *point;
}
//______________________________________________________________________________
void Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point::update_distance_to (Inverse_distance_weighted_interpolator::Weighted_point &target_point)
{  Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point &lat_long_target_point =  (Latitude_longitude_inverse_distance_weighted_interpolator::Weighted_point &)target_point;
   Terrestrial_geometry  geometry;
   distance_to_target = km_to_m(geometry.distance_between_km(lat_long_target_point,*this));
}
//______________________________________________________________________________
Stations_interpolator::Stations_interpolator
(
 nat32  encoded_valid_sources
, Dynamic_float_array  &_current_value
,Unidirectional_list &_all_points_in_range
, Geolocation        *target_geolocation
)
: Latitude_longitude_inverse_distance_weighted_interpolator()
, Enumeration()
, valid_sources(encoded_valid_sources)
, current_value(_current_value)
, valid_count_debug_only(0)
{
   set_target(*target_geolocation);
   int  db_index = 0;  // problaby uint8 should not exceed 32
   FOR_EACH_IN(weighted_point,Weighted_point,_all_points_in_range,each_point)
   {
      append_point(*weighted_point,db_index);
      bool point_valid = (1 << db_index) & valid_sources;
      valid_count_debug_only += point_valid;
      set_point_valid(db_index++,point_valid);
   } FOR_EACH_END(each_point)
   calc_weights_shepard_form();
}
//______________________________________________________________________________
Stations_interpolator *UED_database_interpolator::provide_stations_interpolator_for_current_value()
{
   Stations_interpolator * found_interpolator = (Stations_interpolator * )stations_interpolators.find(current_valid_encoded);
   if (!found_interpolator)
   {  // Couldn't find matching, so create a new one
      found_interpolator = new Stations_interpolator
         (current_valid_encoded,current_value,all_points_in_range,target_geolocation);
      stations_interpolators.append(found_interpolator);
   }
   return  found_interpolator;
}
//______________________________________________________________________________
nat32   UED_database_interpolator::encode_valid_sources(Dynamic_uint8_array &valid_stations) const
{
   nat32  code = 0;
   for (nat32  i = 0 ; i < valid_stations.get_count(); i++)
   {
      bool valid_i = valid_stations.get(i);
      code |=  valid_i << i;
   }
   return code;
}
//______________________________________________________________________________

