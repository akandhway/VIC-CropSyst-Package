#ifndef ClimGen_settingsH
#define ClimGen_settingsH
#include "corn/application/user/settings.h"
#include "UED/library/UED_fname.h"
//191004 #include "common/weather/loc_filename.h"
#include "corn/OS/directory_entry_name_concrete.h"
//______________________________________________________________________________
class ClimGen_Settings
: public CORN::User_settings
{
 public:
   std::string language_code; // Probably could be moved to User_settings.
   CORN::OS::File_name_concrete recent_database;
   CORN::OS::File_name_concrete recent_location; // this is not deprecated (used for generate) but Now centering on UED file
   CORN::OS::Directory_name_concrete recent_import_directory;
 public:
   ClimGen_Settings();
   bool setup_structure(CORN::Data_record &data_rec,bool for_write) modifiable_;
};
//______________________________________________________________________________
extern ClimGen_Settings *ClimGen_settings;
#endif

