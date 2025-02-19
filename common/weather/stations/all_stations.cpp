#ifndef CS_suite_directoryH
#include "CS_suite/application/CS_suite_directory.h"
#endif

#include "common/weather/stations/all_stations.h"
#include "common/weather/stations/AgriMet_stations.h"
#include "common/weather/stations/PAWS_stations.h"
#include "common/weather/stations/NCDC_stations.h"
#include "common/weather/stations/WMO_stations.h"
#include "common/weather/stations/UCAR_stations.h"
#include "common/weather/stations/CC_station_datarec.h"
#include "common/geomatics/USA_state_FIPS_code.h"
#include "common/geomatics/Canada_province_code.h"
#include "corn/container/SDF_list.h"
#include "corn/data_source/dbase_datasrc.h"

#include <fstream>
#if ((__BCPLUSPLUS__>0) && (__BCPLUSPLUS__ <= 0x340))
   // BC5 did not quite have the standard classes
#  include <cstring.h>
#else
#  include <string>
#endif
using namespace std;
//______________________________________________________________________________
Region_type reporting_agency_region_order[] =
{all_regions            // UNKNOWN_REPORTING_AGENCY
,state_province_region  // NCDC_REPORTING_AGENCY
,all_regions            // PAWS_REPORTING_AGENCY       // could eventually be by state, but not many stations
,all_regions            // AGRIMET_REPORTING_AGENCY    // could eventually be by state, but not many stations
,state_province_region  // CANADIAN_CLIMATOLOGICAL_REPORTING_AGENCY
,all_regions            // INM_ESPANA_REPORTING_AGENCY   // could eventually be by state, but not many stations
,country_region         // UCAR_REPORTING_AGENCY
,all_regions				// WMO_REPORTING_AGENCY
};
//______________________________________________________________________________
All_known_weather_stations::All_known_weather_stations()
: reporting_agency(UNKNOWN_REPORTING_AGENCY)
,Canadian_Climatological_stations_datasource(0)
{}
//______________________________________________________________________________
void All_known_weather_stations::set_reporting_agency(Reporting_agency _reporting_agency)
{  reporting_agency = _reporting_agency;
}
//______________________________________________________________________________
void All_known_weather_stations::set_reporting_agency(const char *_format_name)
{  std::string format_name(_format_name);                                        //140516
   if (find_case_insensitive(format_name,"NCDC")     != std::string::npos)  reporting_agency = NCDC_REPORTING_AGENCY;       // these may need to be find_substring
   if (find_case_insensitive(format_name,"AgriMet")  != std::string::npos)  reporting_agency = AGRIMET_REPORTING_AGENCY;
   if (find_case_insensitive(format_name,"PAWS")     != std::string::npos)  reporting_agency = PAWS_REPORTING_AGENCY;
   if (find_case_insensitive(format_name,"AgWeatherNet")     != std::string::npos)  reporting_agency = AgWeatherNet_REPORTING_AGENCY;
   if (find_case_insensitive(format_name,"UCAR")     != std::string::npos)  reporting_agency = UCAR_REPORTING_AGENCY;
   if (find_case_insensitive(format_name,"WMO")      != std::string::npos)	reporting_agency = WMO_REPORTING_AGENCY;
   if (find_case_insensitive(format_name,"WBAN")     != std::string::npos)	reporting_agency = WBAN_REPORTING_AGENCY;
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_PAWS_geolocation(uint16 station_ID_number)
{  return ::render_PAWS_geolocation(station_ID_number);
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_AgriMet_geolocation(const char *station_ID_code)
{  return ::render_AgriMet_geolocation (station_ID_code);
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_NCDC_WBAN_geolocation
(const char                *station_number
,CORN::Bidirectional_list  &known_stations)
{  static NCDC_WBAN_location *last_found_loc = 0; // Often the last accessed location is the one we want now
   Geolocation *geolocation = 0;
   //find the station
   NCDC_WBAN_location *loc =
     (last_found_loc &&
     ( last_found_loc->is_key_cstr(station_number)                               //181001
     //180820 ( strcmp(last_found_loc->get_key(),station_number) == 0)
      ))
   ?  last_found_loc : dynamic_cast<NCDC_WBAN_location *>
         (known_stations.find_cstr(station_number));                             //130322
   if (loc)
   {  geolocation = new Geolocation;
      nat32 station_ID = loc->get_key_nat32();                                   //181001
      geolocation->set_latitude_dec_deg_32       (loc->get_latitude_dec_deg());
      geolocation->set_longitude_dec_deg_32      (loc->get_longitude_dec_deg());
      geolocation->set_elevation_meter           (loc->get_elevation_m());
//NYI            geolocation->set_country_code     (uint16 i_country_code)
//NYI            geolocation->set_state_code       (uint16 i_state_code)
//NYI            geolocation->set_county_code      (uint16 i_county_code)
      geolocation->set_station_number           (station_ID);
      geolocation->set_station_ID_code          (loc->get_key());

//NYI       Not sure if the station name is available, if so the set_station_name()
      geolocation->set_station_name    (loc->STATION_NAME);
      geolocation->set_country_code_ISO3166(840);  // Always in the United states
      geolocation->set_country_name(loc->COUNTRY);
      geolocation->set_state_name  (loc->ST);
      geolocation->set_county_name (loc->COUNTY);
      last_found_loc =loc;
   }
   return geolocation;
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_NCDC_geolocation(const char *cooperative_station_number )
{
   if (!known_NCDC_stations.count())
      load_NCDC_WBAN_stations(known_NCDC_stations,false);
   return render_NCDC_WBAN_geolocation(cooperative_station_number,known_NCDC_stations);
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_WBAN_geolocation(const char *WBAN_station_number )
{  if (!known_WBAN_stations.count())
      load_NCDC_WBAN_stations(known_WBAN_stations,true);
   return render_NCDC_WBAN_geolocation(WBAN_station_number,known_WBAN_stations);
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_WMO_geolocation(nat32 station_ID_number)
{  static WMO_location *last_found_WMO_loc = 0;  // Often the last accessed location is the one we want now
   Geolocation *geolocation  = 0;
   if (!known_WMO_stations.count())
      load_WMO_stations(known_WMO_stations);
   WMO_location *WMO_loc = (last_found_WMO_loc && 
      //180820 (last_found_WMO_loc->get_key_nat32() == station_ID_number)
      last_found_WMO_loc->is_key_nat32(station_ID_number) )                       //180820
      ?  last_found_WMO_loc
      : (WMO_location *)known_WMO_stations.find(station_ID_number);
   if (WMO_loc)
   {  geolocation = new Geolocation(WMO_loc->geolocation);
      last_found_WMO_loc = WMO_loc;
   }
   return geolocation;
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_UCAR_geolocation(const char *BKIDN)
{  static UCAR_location *last_found_UCAR_loc = 0; // Often the last accessed location is the one we want now
   Geolocation *geolocation = 0;
   if (!known_UCAR_stations.count())
   {  cout << "loading UCAR stations." << endl;
      load_UCAR_stations(known_UCAR_stations);
   }
   UCAR_location *UCAR_loc =
     (last_found_UCAR_loc && 
      ( last_found_UCAR_loc->is_key_cstr(BKIDN)
      //180820 ( strcmp(last_found_UCAR_loc->get_key(),BKIDN) == 0)
      ))
   ?  last_found_UCAR_loc
   : (UCAR_location *)known_UCAR_stations.find_cstr(BKIDN);                      //130322
   if (UCAR_loc)
   {  geolocation = new Geolocation;
      nat32 station_ID = UCAR_loc->get_station_number();
      geolocation->set_latitude_dec_deg_32         (UCAR_loc->get_latitude_dec_deg());
      geolocation->set_longitude_dec_deg_32        (UCAR_loc->get_longitude_dec_deg());
      geolocation->set_elevation_meter             (UCAR_loc->get_elevation_m());
      geolocation->set_station_number          (station_ID);
      char ID_buffer[100];
      geolocation->set_station_ID_code
         (CORN::nat32_to_cstr(station_ID,ID_buffer,10));      //140923
      geolocation->set_station_name(CORN::strip_string(UCAR_loc->LNAME));
      // NYI  convert NUMCOU to ISO country code
      geolocation->set_country_name(CORN::strip_string(UCAR_loc->CNAME));
      last_found_UCAR_loc = UCAR_loc;
   }
   return geolocation;
}
//______________________________________________________________________________
All_known_weather_stations::~All_known_weather_stations()
{  if (Canadian_Climatological_stations_datasource)
      delete Canadian_Climatological_stations_datasource; Canadian_Climatological_stations_datasource = 0;
}
//______________________________________________________________________________
bool All_known_weather_stations::provide_Canadian_Climatological_datasource()
{  if (!Canadian_Climatological_stations_datasource)
   {  CORN::OS::File_name *StationCat_dbf = CS::Suite_directory->UED().find_file_name //100914
         (L"StationCatOct20_99.DBF"
         ,CORN::OS::File_system::subdirectory_recursion_inclusive);              //150909
      if (StationCat_dbf)
      {  Canadian_Climatological_stations_datasource = new Dynamic_string_indexed_dBase_data_source(StationCat_dbf->c_str(),"STATIONID",std::ios_base::in);
         delete StationCat_dbf;
      }
   }
   return Canadian_Climatological_stations_datasource != 0;
}
//______________________________________________________________________________
void All_known_weather_stations::convert_Canadian_climatological_station_record_to_geolocation
(const Canadian_climatological_station_record &CC_loc
,Geolocation &geolocation)
{     geolocation.set_station_number(0); // Canadian climatological station IDs are alphanumeric (set as station_ID_code)
      geolocation.set_station_ID_code(CC_loc.STATIONID.c_str());
      geolocation.set_station_name(CC_loc.NAME);
      geolocation.set_country_name("Canada");
      geolocation.set_state_name(CC_loc.PROVINCE);
#ifdef NYI
      geolocation.set_county_name.assign("");      // (may be empty if county code specified.
      geolocation.country_code_ISO3166 = 0;     // International standard country code   // (may be 0 indicating not avail)
      geolocation.state_code = 0;       // USA FIPS state number (other contries may have other enumerations) // (may be 0 indicating not avail)
      geolocation.county_code = 0;      // County/region number.                 // (may be 0 indicating not avail)
#endif
        // weather_filename should already be set by constructor
        // but will make sure incase it is removed from the constructor in the future
//      geolocation.weather_filename.assign(fully_qualified_UED_filename);
      geolocation.set_latitude_dec_deg_32(CC_loc.LATDD);
      geolocation.set_longitude_dec_deg_32(CC_loc.LONGDD);
      geolocation.set_elevation_meter(CC_loc.ELEV);    // meters

//      geolocation.station_name.assign(CC_loc.STATIONID);
//      geolocation.station_name.append(" ");
      geolocation.set_station_name(CC_loc.NAME);
//      geolocation.station_name.append(" ");
//      geolocation.station_name.append(CC_loc.PROVINCE);
//      geolocation.station_name.append(" ");
//      geolocation.station_name.append("Canada");
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_Canadian_Climatological_geolocation(const char *station_ID_code)
{
   if (!provide_Canadian_Climatological_datasource()) return 0;
   static Canadian_climatological_station_record last_found_CC_loc; // Often the last accessed location is the one we want now
   Geolocation *geolocation =0;
   bool CC_loc_read = false;
   if ( last_found_CC_loc.STATIONID == station_ID_code) // we have just loaded this record before
       CC_loc_read = true;
   else
   {  bool found = Canadian_Climatological_stations_datasource->find_record(station_ID_code,true);
      if (found)
      {  CC_loc_read = Canadian_Climatological_stations_datasource->get(last_found_CC_loc);
         // CC_loc_read = true;
      }
   }
   if (CC_loc_read)
   {  geolocation = new Geolocation;
      convert_Canadian_climatological_station_record_to_geolocation(last_found_CC_loc,*geolocation);
   }
   return geolocation;
}
//______________________________________________________________________________
Geolocation *All_known_weather_stations::render_geolocation(const std::string &station_ID_code)
{  Geolocation *found_geolocation = 0;
   // 060201 Now only do these searches if we know the reporting agency (UCAR is too big too search)
   if ((reporting_agency == PAWS_REPORTING_AGENCY) )
   {  uint16 station_ID_number = atoi(station_ID_code.c_str());
      found_geolocation = render_PAWS_geolocation(station_ID_number);
   }
/*
   if (reporting_agency == AgWeatherNet_REPORTING_AGENCY)
   {
      found_geolocation = get_AgWeatherNet_geolocation(station_ID_code);
   }
*/
   if ((reporting_agency == AGRIMET_REPORTING_AGENCY) )
   {  found_geolocation = render_AgriMet_geolocation(station_ID_code.c_str());
   }
   if ((reporting_agency == NCDC_REPORTING_AGENCY) )
   {  found_geolocation = render_NCDC_geolocation(station_ID_code.c_str());
   }
   if ((reporting_agency == UCAR_REPORTING_AGENCY) )
   {  found_geolocation = render_UCAR_geolocation(station_ID_code.c_str());
   }
   if ((reporting_agency == CANADIAN_CLIMATOLOGICAL_REPORTING_AGENCY) )
   {  found_geolocation = render_Canadian_Climatological_geolocation(station_ID_code.c_str());
   }
   if ((reporting_agency == WMO_REPORTING_AGENCY))
   {  nat32 station_ID_number = atoi(station_ID_code.c_str());
      found_geolocation = render_WMO_geolocation(station_ID_number);
   }
   if ((reporting_agency == WBAN_REPORTING_AGENCY) ) // added 2010-03-20
   {  found_geolocation = render_WBAN_geolocation(station_ID_code.c_str());
   }
   return found_geolocation;
}
//______________________________________________________________________________
class UCAR_country
: public Item
{  std::string name;
public:
   inline UCAR_country(const char *_name)
      : Item()
      , name(_name)
      {}
   inline virtual bool is_key_string(const std::string &key)        affirmation_  //180820
      { return name == key; }
   inline virtual const char *get_key() const { return name.c_str(); }
};
//______________________________________________________________________________
Region_type All_known_weather_stations::list_regions(Reporting_agency agency, CORN::SDF_Cowl &regions) const
{  Region_type region_type = all_regions;
   switch (agency)
   {  case NCDC_REPORTING_AGENCY :
      {  for (int s = 0; USA_state_FIPS[s].code;s++)
         if (USA_state_FIPS[s].code[0] != '_')
            regions.append(new Item_string(USA_state_FIPS[s].code));
         region_type = state_province_region;
      } break;
      case CANADIAN_CLIMATOLOGICAL_REPORTING_AGENCY :
      {  for (int s = 0; Canada_province_code[s].code;s++)
         if (Canada_province_code[s].code[0] != '_')
            regions.append(new Item_string(Canada_province_code[s].code));
         region_type = state_province_region;
      } break;
      case UCAR_REPORTING_AGENCY :
      { // Setup UCAR station selectors
         if (!known_UCAR_stations.count())
            load_UCAR_stations(known_UCAR_stations);
         CORN::Bidirectional_list UCAR_countries;
         FOR_EACH_IN(station,UCAR_location, known_UCAR_stations,each_station)
         {  if (!UCAR_countries.find_string(station->CNAME))
            {  UCAR_countries.append(new UCAR_country(station->CNAME.c_str()));
            }
         } FOR_EACH_END(each_station)
         UCAR_countries.sort();
         FOR_EACH_IN(country,UCAR_country,UCAR_countries,each_country)
               regions.append(new Item_string(country->get_key()));
         FOR_EACH_END(each_country)
         region_type = country_region;
      } break;
      case PAWS_REPORTING_AGENCY          :  // currently all regions because not a big list
      case AGRIMET_REPORTING_AGENCY       :  // currently all regions because not a big list
      case INM_ESPANA_REPORTING_AGENCY    :  // This needs to be split by province, but I don't know which provence the stations are in?
      default                             : // UNKNOWN_REPORTING_AGENCY
         region_type = all_regions;
      break;
   } // end switch
   return region_type;
}
//______________________________________________________________________________
int16 All_known_weather_stations::render_geolocations_for_region
(Reporting_agency agency
, Region_type region_type
, const char *region_identification  // (I.e. the country ,  provence or state name
                                     // for WMO this is a 2 letter country code or US state code.
, CORN::Bidirectional_list /*180101 Association_list*/ &geolocations)                              performs_IO_
{  // This appends geolocation info (copies) to the geolocations lists (the caller may delete these items because they are copies).
   // You can use list regions to identify the region type suitable for the selection.
   // This is currently used only be the import wizard
   int16 avail_location_count = 0;
   switch (agency)
   {  //________________________________________________________________________
      case NCDC_REPORTING_AGENCY :
      {  if (!known_NCDC_stations.count())   load_NCDC_WBAN_stations(known_NCDC_stations,false);
         FOR_EACH_IN(NCDC_loc, NCDC_WBAN_location,known_NCDC_stations,each_NCDC_loc)
         {  if ((region_type == all_regions) || (NCDC_loc->ST == region_identification))
            {  Geolocation *NCDC_geoloc = render_NCDC_geolocation(NCDC_loc->COOPID.c_str() );
               geolocations.append(new geodesy::Geolocation_item(*NCDC_geoloc)); //191010
               avail_location_count++;
            }
         } FOR_EACH_END(each_NCDC_loc)
      } break;
      //________________________________________________________________________
      case WBAN_REPORTING_AGENCY :
      {  if (!known_WBAN_stations.count())   load_NCDC_WBAN_stations(known_WBAN_stations,true);
         FOR_EACH_IN(WBAN_loc, NCDC_WBAN_location,known_WBAN_stations,each_WBAN_loc)
         {  if ((region_type == all_regions) || (WBAN_loc->ST == region_identification))
            {  Geolocation *WBAN_geoloc = render_WBAN_geolocation(WBAN_loc->get_key() );
               geolocations.append(new geodesy::Geolocation_item(*WBAN_geoloc)); //191010
               avail_location_count++;
            }
         } FOR_EACH_END(each_WBAN_loc)
      } break;
      //________________________________________________________________________
      case UCAR_REPORTING_AGENCY  :
      {  if (!known_UCAR_stations.count())   load_UCAR_stations(known_UCAR_stations);
         FOR_EACH_IN(UCAR_loc, UCAR_location,known_UCAR_stations,each_UCAR_loc)
         {  if ((region_type == all_regions) || (UCAR_loc->CNAME == region_identification))
            {  Geolocation *UCAR_geoloc = render_UCAR_geolocation(UCAR_loc->BKIDN.c_str() );
               geolocations.append(new geodesy::Geolocation_item(*UCAR_geoloc)); //191010
               avail_location_count++;
            }
         } FOR_EACH_END(each_UCAR_loc)
      } break;
      //________________________________________________________________________
      case WMO_REPORTING_AGENCY :
      { 	if (!known_WMO_stations.count()) load_WMO_stations(known_WMO_stations);
         FOR_EACH_IN(WMO_loc, WMO_location,known_WMO_stations,each_WMO_loc)
         {  if ((region_type == all_regions) || (WMO_loc->geolocation.get_country_name() == region_identification))
            {  geolocations.append(new geodesy::Geolocation_item(WMO_loc->geolocation)); //191010
               avail_location_count++;
            }
         } FOR_EACH_END(each_WMO_loc)
      }  break;
      //________________________________________________________________________
     case PAWS_REPORTING_AGENCY  :
      {  // NYI
         // avail_location_count++;
      } break;
      //________________________________________________________________________
      case AGRIMET_REPORTING_AGENCY :
      {  // NYI
         // avail_location_count++;
      } break;
      //________________________________________________________________________
      case CANADIAN_CLIMATOLOGICAL_REPORTING_AGENCY :
      {  // NYI
         provide_Canadian_Climatological_datasource();

         Canadian_climatological_station_record CC_loc_rec;
         for (bool more_records = Canadian_Climatological_stations_datasource->goto_BOF()
            ;more_records
            ;more_records = Canadian_Climatological_stations_datasource->goto_next())
         {
            bool CC_loc_read = Canadian_Climatological_stations_datasource->get(CC_loc_rec);
            if (CC_loc_read && ((region_type == all_regions) || (CC_loc_rec.PROVINCE == region_identification)))
            {
               Geolocation    CC_geoloc;
               convert_Canadian_climatological_station_record_to_geolocation(CC_loc_rec,CC_geoloc);
               geodesy::Geolocation_item * CC_geoloc_item
                  = new geodesy::Geolocation_item(CC_geoloc);                    //191010
               geolocations.append(CC_geoloc_item);
               avail_location_count++;
            }
         }
      } break;
      //________________________________________________________________________
      case INM_ESPANA_REPORTING_AGENCY :
      {  // NYI
         // avail_location_count++;
      } break;
      //________________________________________________________________________
   }
   return avail_location_count;
}
//______________________________________________________________________________
bool All_known_weather_stations::load_NCDC_WBAN_stations
(CORN::Bidirectional_list &locations
,bool WBAN_mode)
{  bool loaded = true;
   cout << "Reading station geolocation";
   CORN::OS::File_name *sodstn_txt_p = CS::Suite_directory->UED()
      .find_file_name(L"sodstn.txt"
      ,CORN::OS::File_system::subdirectory_recursion_inclusive);                 //150909
   if (sodstn_txt_p)                                                             //100914
   {  CORN::OS::File_name &sodstn_txt = *sodstn_txt_p;
      cout << " from file..." << endl;
      ifstream sodstn_file(sodstn_txt.c_str());
      if (sodstn_file.good())
      {  std::string header;
         // Skip the first 2 lines, they are headers
         getline(sodstn_file,header);                                           //140516
         getline(sodstn_file,header);                                           //140516
         while (!sodstn_file.eof())
         {  NCDC_WBAN_location *location = new NCDC_WBAN_location(WBAN_mode);
            if (location)
            {  if (location->read(sodstn_file) &&
                   ((location->is_WBAN_station() == WBAN_mode) || !WBAN_mode))
               {  locations.append(location);
                  cout << location->COOPID << '\r';
               } else delete location;
            }
         }
      } else
      {  cout << ". Unable to read file: sodstn.txt" << endl;
         loaded=false;
      }
   } else
   {  cout << "Unable to find file:sodstn.txt usually this is installed in C:\\CS_Suite_4\\UED\\convert\\stations\\NCDC" << endl;
      loaded=false;
   }
   delete sodstn_txt_p;
   return loaded;
}
//______________________________________________________________________________
bool All_known_weather_stations::load_WMO_stations(CORN::Enumeration_list &locations)
{  bool loaded = true;

   CORN::OS::File_name *stnlist_sorted_txt_p =
      CS::Suite_directory->UED().find_file_name                                  //120414
         (L"globalSOD_stnlist-sorted.txt"
         ,CORN::OS::File_system::subdirectory_recursion_inclusive);              //150909
   if (stnlist_sorted_txt_p)                                                     //120414
   {  CORN::OS::File_name &stnlist_sorted_txt = *stnlist_sorted_txt_p;           //120414
   ifstream stnlist_sorted_file(stnlist_sorted_txt.c_str());
   if (stnlist_sorted_file.good())
   {
      std::string header;
      // Skip the first 14 lines, they are headers
      for (int i = 0; i < 13; i++)
         getline( stnlist_sorted_file, header /*, delimiter = '\n'*/ );
      while (!stnlist_sorted_file.eof())
      {  WMO_location *location = new WMO_location();
         if (location && location->read(stnlist_sorted_file))
            locations.append(location);
      }
   } else loaded=false;
   } else loaded=false; // Unable to find the station list file
   delete stnlist_sorted_txt_p;
   return loaded;
}
//______________________________________________________________________________

