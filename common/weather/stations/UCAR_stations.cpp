#include "corn/std/std_fstream.h"
#include "common/weather/stations/UCAR_stations.h"
#include <string.h>
//140516 #include "corn/string/uiostring.h"
#ifndef CS_suite_directoryH
#  include "CS_suite/application/CS_suite_directory.h"
#endif
using namespace std;
//______________________________________________________________________________
bool UCAR_location::read(istream &stationslib_file)
{  char buffer[255];
   // Format say blank, but this is missing in the file
   stationslib_file.read(buffer,5);    buffer[5] = 0;   BKIDN.assign(buffer);
   //140516 BKIDN.strip(Ustring::Both);
   CORN::strip_string(BKIDN,CORN::Both);                                         //140516

if (stationslib_file.gcount() == 0)
   return false;

   stationslib_file.read(buffer,6);    buffer[6] = 0;   LAT = atof(buffer);
   stationslib_file.read(buffer,7);    buffer[7] = 0;   LON = atof(buffer);
   stationslib_file.read(buffer,5);    buffer[5] = 0;   IELEV = atoi(buffer);
   stationslib_file.read(buffer,4);    buffer[4] = 0;   NUMCOU= atoi(buffer);
   stationslib_file.read(buffer,3);    buffer[3] = 0;   NUMREG= atoi(buffer);
   stationslib_file.read(buffer,1);    // space
   stationslib_file.read(buffer,1);    buffer[1] = 0;   IQUAL= atoi(buffer);
   stationslib_file.read(buffer,5);    buffer[4] = 0;   // There is a space after the call
   CALL.assign(buffer);    CORN::strip_string(CALL); //140516 CALL.strip();

   stationslib_file.read(buffer,23);   buffer[23] = 0;
   LNAME.assign(buffer);   CORN::strip_string(LNAME); //140516 LNAME.strip();
   // In case the file has already been strips we just read to the end of the line.
   std::string/*140516 CORN::UIO_string*/ CNAME_str;
   //140516 CNAME_str.read_to_delim(stationslib_file);
   getline(stationslib_file,CNAME_str);                                          //140516
   CORN::strip_string(CNAME_str); //140516 CNAME_str.strip();
   CNAME.assign(CNAME_str);
   unique_ID = BKIDN; if (unique_ID=="99999") unique_ID=CALL; // when BKIDN is 99999 then only the call letters uniquely identify
   return true;
}
//______________________________________________________________________________
bool UCAR_location::write(std::ostream &stationslib_file)           performs_IO_ //150728
{  char buffer[255];
   // Format say blank, but this is missing in the file

   if (strlen(BKIDN.c_str()) == 4) stationslib_file << ' ';  // station codes are 4 or 5 characters we need to right align
   stationslib_file << BKIDN;  // will have been stripped on read.

   strcpy(buffer,CORN::float32_to_cstr(LAT,0));
   for (int field_width=strlen(buffer);field_width < 6;field_width++) stationslib_file << ' '; // right justify LAT
   stationslib_file << buffer;

   strcpy(buffer,CORN::float32_to_cstr(LON,0));
   for (int field_width=strlen(buffer);field_width < 7;field_width++) stationslib_file << ' '; // right justify LON
   stationslib_file << buffer;

   strcpy(buffer,CORN::float32_to_cstr(IELEV,0));
   for (int field_width=strlen(buffer);field_width < 5;field_width++) stationslib_file << ' '; // right justify IELEV
   stationslib_file << buffer;

   strcpy(buffer,CORN::float32_to_cstr(NUMCOU,0));
   for (int field_width=strlen(buffer);field_width < 4;field_width++) stationslib_file << ' '; // right justify NUMCOU
   stationslib_file << buffer;

   strcpy(buffer,CORN::float32_to_cstr(NUMREG,0));
   for (int field_width=strlen(buffer);field_width < 3;field_width++) stationslib_file << ' '; // right justify NUMREG
   stationslib_file << buffer;
   stationslib_file << ' ';
   stationslib_file << IQUAL;

   strcpy(buffer,CALL.c_str());
   for (int field_width=strlen(buffer);field_width < 4;field_width++) stationslib_file << ' '; // call letters are right justified
   stationslib_file << buffer;
   stationslib_file << ' ';

   stationslib_file << LNAME;
   for (int field_width=strlen(LNAME.c_str());field_width < 23;field_width++) stationslib_file << ' ';

   stationslib_file << CNAME;
   stationslib_file << endl;
// Not sure if need to be space field:    for (int field_width=strlen(LNAME);field_width < ??;field_width++) stationslib_file << ' ';
   return true;                                                                  //150728
}
//______________________________________________________________________________
void UCAR_location::get_description(std::string  &buffer)                  const
{  buffer.assign(BKIDN);
   buffer.append(" (");
   buffer.append(LNAME);
   CORN::strip_string(buffer);                                                   //140516
   buffer.append(" - ");
   buffer.append(CNAME);
   buffer.append(")");
}
//______________________________________________________________________________
float32  UCAR_location::get_longitude_dec_deg()                            const
{  float32 longitude_dec_deg = LON / 100.0;
   // longitude here is in ranges 0 to 360 west of greenwich
   // we need to convert to decimal degrees 0 to 180 where east is positive and west is negative
   if (longitude_dec_deg > 180.0)
      longitude_dec_deg = (360.0 - longitude_dec_deg);
   else longitude_dec_deg = -longitude_dec_deg;
   return  longitude_dec_deg;
}
//______________________________________________________________________________
float32  UCAR_location::get_latitude_dec_deg()                             const
{ return LAT / 100.0; }
//______________________________________________________________________________
float32  UCAR_location::get_elevation_m()                                  const
{ return IELEV; }  // ELEV is in meters
//______________________________________________________________________________
int32 UCAR_location::get_station_number()                                  const
{
   // So far it looks like   BKIDN is always integers
   return atoi(BKIDN.c_str());
}
//______________________________________________________________________________
/*
     5. NUMCOU- A NUMBER ASSIGNED TO EACH COUNTRY BY CAC.
        FORMAT-I3
     6. NUMREG- A REGION NUMBER ASSIGNED BY CAC.
        FORMAT-I3
     7. IQUAL- A QUALITY NUMBER ASSIGNED BY CAC. LOCATIONS WHICH WE
               CONSIDER THE BEST ARE ASSIGNED 0. ALL OTHERS ARE
               ASSIGNED 1.
        FORMAT-I2
     8. CALL- AIRWAYS CALL LETTERS USED IN THE UNITED STATES OF AMERICA
	      AND CANADA. (3 or 4 letters; letters have been right-justified
	      in field)
        FORMAT-A4
     9. LNAME- LOCATION NAME.
        FORMAT-A23
    10. CNAME- REGION NAME.
        FORMAT-A20
   return true;
*/

bool load_UCAR_stations(CORN::Bidirectional_list /*180101 Association_list*/ &unique_locations)
{  bool loaded = true;
   CORN::Bidirectional_list /*180101 Association_list*/ all_locations;

   const CORN::OS::File_name *stationslib_txt_found =
      CS::Suite_directory->UED().
         find_file_name(L"UCAR ds512.0 stationlib.txt"                           //100914
            ,CORN::OS::File_system::subdirectory_recursion_inclusive );          //150909
            //170111 ,CORN::OS::Directory_entry_name::full_qualification );                  //150909

      //150909find_file_name(L"UCAR ds512.0 stationlib.txt",true); //100914
   if (stationslib_txt_found)
   {
      const CORN::OS::File_name &stationslib_txt = *stationslib_txt_found;
//   if (abridged)
//      stationslib_txt.set_file("UCAR ds512.0 stationlib abridged.txt");
   ifstream stationslib_file(stationslib_txt.c_str());
   if (stationslib_file.good())
   {
      UCAR_location read_location;
      bool continue_read = true;
      while (!stationslib_file.eof() && continue_read)
      {  continue_read = read_location.read(stationslib_file);
         if (continue_read)
         {
// this was too slow           if (abridged || !locations.find(read_location.unique_ID.c_str())) // The stationslib file appears to have multiple entries (not sure why)
            {  UCAR_location *location = new UCAR_location(read_location);
               cout << read_location.unique_ID << "    \r"; // Just for progress
               all_locations.append(location);
      } } }

      // Now remove duplicate entries
   cout << "removing duplication stations geolocation information:" << endl;
   std::string last_location_unique_ID = "";

   for (UCAR_location *popped = (UCAR_location *)all_locations.pop_first()
       ;popped
       ;popped = (UCAR_location *)all_locations.pop_first())
   {
      if (popped->unique_ID == last_location_unique_ID)
         delete popped;
      else
      {  unique_locations.append(popped);
         last_location_unique_ID.assign(popped->unique_ID);
         cout << popped->unique_ID << "    \r"; // Just for progress
      }
   }
   } else loaded = false;
   } else loaded = false;
   return loaded;
}
//______________________________________________________________________________
